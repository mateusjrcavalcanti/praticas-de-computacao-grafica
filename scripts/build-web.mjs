import { existsSync, mkdirSync } from "node:fs";
import { spawnSync } from "node:child_process";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const scriptDirectory = dirname(fileURLToPath(import.meta.url));
const root = resolve(scriptDirectory, "..");
const wasm = join(root, "www", "public", "wasm");
const localCompiler = join(
  root,
  ".tools",
  "emsdk",
  "upstream",
  "emscripten",
  process.platform === "win32" ? "em++.exe" : "em++",
);
let compiler = existsSync(localCompiler) ? localCompiler : "em++";
const commonFlags = [
  "-std=c++17",
  "-O2",
  "-lglut",
  "-sLEGACY_GL_EMULATION=1",
  "-sGL_UNSAFE_OPTS=0",
  "-sALLOW_MEMORY_GROWTH=1",
  "-sENVIRONMENT=web",
];

function compile(args) {
  const result = spawnSync(compiler, args, {
    cwd: root,
    stdio: "inherit",
    shell: process.platform === "win32" && compiler.toLowerCase().endsWith(".bat"),
  });
  if (result.error?.code === "ENOENT") {
    console.error("em++ não foi localizado após a configuração automática.");
    process.exit(1);
  }
  if (result.status !== 0) process.exit(result.status ?? 1);
}

function compilerIsAvailable(command) {
  const result = spawnSync(command, ["--version"], {
    cwd: root,
    stdio: "ignore",
    shell: process.platform === "win32" && command.toLowerCase().endsWith(".bat"),
  });
  return result.status === 0;
}

if (!compilerIsAvailable(compiler)) {
  console.log("Emscripten não encontrado. Iniciando a instalação local automática…");
  const setup = spawnSync(process.execPath, [join(scriptDirectory, "setup-emscripten.mjs")], {
    cwd: root,
    stdio: "inherit",
    shell: false,
  });
  if (setup.status !== 0) process.exit(setup.status ?? 1);
  compiler = localCompiler;
  if (!compilerIsAvailable(compiler)) {
    console.error("A instalação terminou, mas o compilador em++ não foi localizado.");
    process.exit(1);
  }
}

for (const project of ["hierarquia-transformacoes", "desafio-obstaculos", "projeto-final"]) {
  mkdirSync(join(wasm, project), { recursive: true });
}

compile([
  join(root, "atividades", "hierarquia-transformacoes", "src", "main.cpp"),
  ...commonFlags,
  "-o",
  join(wasm, "hierarquia-transformacoes", "app.js"),
]);
compile([
  join(root, "atividades", "desafio-obstaculos", "src", "main.cpp"),
  ...commonFlags,
  "-o",
  join(wasm, "desafio-obstaculos", "app.js"),
]);
compile([
  join(root, "atividades", "projeto-final", "src", "main.cpp"),
  join(root, "atividades", "projeto-final", "src", "stb_image.cpp"),
  "-I",
  join(root, "atividades", "projeto-final", "include"),
  ...commonFlags,
  "--preload-file",
  `${join(root, "atividades", "projeto-final", "assets")}@assets`,
  "-o",
  join(wasm, "projeto-final", "app.js"),
]);

console.log(`Aplicações WebAssembly criadas em ${wasm}`);
