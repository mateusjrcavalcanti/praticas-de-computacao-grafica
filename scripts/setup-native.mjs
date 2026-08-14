import { existsSync, mkdtempSync, rmSync } from "node:fs";
import { spawnSync } from "node:child_process";
import { dirname, join } from "node:path";
import { tmpdir } from "node:os";

const checkOnly = process.argv.includes("--check");
const systemName =
  process.platform === "win32"
    ? "Windows"
    : process.platform === "linux"
      ? "Linux"
      : process.platform === "darwin"
        ? "macOS"
        : process.platform;
const msysRoot = process.env.MSYS2_ROOT || "C:\\msys64";
const compilerCandidates = [
  process.env.CXX,
  "g++",
  process.platform === "win32" ? join(msysRoot, "ucrt64", "bin", "g++.exe") : undefined,
].filter(Boolean);

function run(command, args) {
  const result = spawnSync(command, args, { stdio: "inherit", shell: false });
  if (result.error?.code === "ENOENT") return false;
  if (result.error) throw result.error;
  if (result.status !== 0) process.exit(result.status ?? 1);
  return true;
}

function available(command) {
  const result = spawnSync(command, ["--version"], { stdio: "ignore", shell: false });
  return !result.error && result.status === 0;
}

function toolchainReady() {
  const compilers = [...new Set(compilerCandidates)].filter(available);
  if (compilers.length === 0) return { ready: false, reason: "g++ não encontrado" };

  const libraries =
    process.platform === "win32"
      ? ["-lopengl32", "-lglu32", "-lfreeglut"]
      : process.platform === "darwin"
        ? ["-framework", "OpenGL", "-framework", "GLUT"]
        : ["-lGL", "-lGLU", "-lglut"];
  const header = process.platform === "darwin" ? "GLUT/glut.h" : "GL/freeglut.h";
  const source = `#include <${header}>\nint main(int argc, char** argv) { glutInit(&argc, argv); return 0; }\n`;
  const failures = [];

  for (const compiler of compilers) {
    const temporaryDirectory = mkdtempSync(join(tmpdir(), "cg-native-check-"));
    const executable = join(
      temporaryDirectory,
      process.platform === "win32" ? "check.exe" : "check",
    );
    try {
      const result = spawnSync(compiler, ["-x", "c++", "-", ...libraries, "-o", executable], {
        input: source,
        encoding: "utf8",
        stdio: ["pipe", "ignore", "pipe"],
        env:
          process.platform === "win32"
            ? { ...process.env, PATH: `${dirname(compiler)};${process.env.PATH || ""}` }
            : process.env,
        shell: false,
      });
      if (result.status === 0) return { ready: true, compiler };
      const detail =
        result.error?.message || result.stderr?.trim() || `código de saída ${result.status}`;
      failures.push(`${compiler}: ${detail}`);
    } finally {
      rmSync(temporaryDirectory, { recursive: true, force: true });
    }
  }

  return {
    ready: false,
    reason: `FreeGLUT ou bibliotecas OpenGL não encontrados:\n${failures.join("\n")}`,
  };
}

console.log(`Sistema detectado: ${systemName}.`);
const initialState = toolchainReady();
if (initialState.ready) {
  console.log(
    `Dependências nativas já estão disponíveis (${initialState.compiler}). Nenhuma instalação é necessária.`,
  );
  process.exit(0);
}

console.log(`Verificação: ${initialState.reason}.`);
if (checkOnly) {
  console.error("As dependências nativas precisam ser instaladas.");
  process.exit(1);
}

if (process.platform === "win32") {
  let bash = join(msysRoot, "usr", "bin", "bash.exe");

  if (!existsSync(bash)) {
    if (!available("winget")) {
      console.error(
        "winget não está disponível. Instale o MSYS2 por https://www.msys2.org e execute novamente.",
      );
      process.exit(1);
    }
    console.log("Instalando o MSYS2…");
    run("winget", [
      "install",
      "--id",
      "MSYS2.MSYS2",
      "--exact",
      "--accept-package-agreements",
      "--accept-source-agreements",
    ]);
    bash = join(msysRoot, "usr", "bin", "bash.exe");
  }

  if (!existsSync(bash)) {
    console.error(
      `MSYS2 instalado, mas não localizado em ${msysRoot}. Defina MSYS2_ROOT e execute novamente.`,
    );
    process.exit(1);
  }

  console.log("Instalando GCC e FreeGLUT para o ambiente UCRT64…");
  run(bash, [
    "-lc",
    "pacman -S --needed --noconfirm mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-freeglut",
  ]);
} else if (process.platform === "linux") {
  if (available("apt-get")) {
    run("sudo", ["apt-get", "update"]);
    run("sudo", ["apt-get", "install", "-y", "g++", "freeglut3-dev"]);
  } else if (available("dnf")) {
    run("sudo", ["dnf", "install", "-y", "gcc-c++", "freeglut-devel"]);
  } else if (available("pacman")) {
    run("sudo", ["pacman", "-S", "--needed", "gcc", "freeglut"]);
  } else {
    console.error(
      "Distribuição não reconhecida. Instale g++, FreeGLUT e os cabeçalhos de OpenGL pelo gerenciador do sistema.",
    );
    process.exit(1);
  }
} else if (process.platform === "darwin") {
  console.error("Instale as ferramentas de linha de comando do Xcode com: xcode-select --install");
  process.exit(1);
} else {
  console.error("Instalação automática não disponível para este sistema.");
  process.exit(1);
}

const finalState = toolchainReady();
if (!finalState.ready) {
  console.error(`A instalação terminou, mas a verificação falhou: ${finalState.reason}.`);
  process.exit(1);
}

console.log(`Dependências nativas instaladas e verificadas (${finalState.compiler}).`);
