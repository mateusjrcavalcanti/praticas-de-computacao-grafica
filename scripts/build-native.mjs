import { cpSync, existsSync, mkdirSync, readFileSync, writeFileSync } from "node:fs";
import { spawnSync } from "node:child_process";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const scriptDirectory = dirname(fileURLToPath(import.meta.url));
const root = resolve(scriptDirectory, "..");
const output = join(root, "build");
const packageJson = JSON.parse(readFileSync(join(root, "www", "package.json"), "utf8"));

const setup = spawnSync(process.execPath, [join(scriptDirectory, "setup-native.mjs")], {
  cwd: root,
  stdio: "inherit",
  shell: false,
});
if (setup.error) throw setup.error;
if (setup.status !== 0) process.exit(setup.status ?? 1);

const executableSuffix = process.platform === "win32" ? ".exe" : "";
const windowsMsysRoot = process.env.MSYS2_ROOT || "C:\\msys64";
const windowsToolchain = join(windowsMsysRoot, "ucrt64", "bin");
const compilerCandidates = [
  process.env.CXX,
  "g++",
  process.platform === "win32" ? join(windowsToolchain, "g++.exe") : undefined,
].filter(Boolean);
const openGlLibraries =
  process.platform === "win32"
    ? ["-lopengl32", "-lglu32", "-lfreeglut"]
    : process.platform === "darwin"
      ? ["-framework", "OpenGL", "-framework", "GLUT"]
      : ["-lGL", "-lGLU", "-lglut"];

function commandExists(command) {
  const result = spawnSync(command, ["--version"], { stdio: "ignore", shell: false });
  return !result.error && result.status === 0;
}

const compiler = compilerCandidates.find(commandExists);

if (!compiler) {
  console.error("g++ não foi localizado após a configuração automática.");
  process.exit(1);
}

function compile(sources, name, extra = []) {
  const result = spawnSync(
    compiler,
    [
      ...sources,
      ...extra,
      "-std=c++17",
      ...openGlLibraries,
      "-o",
      join(output, `${name}${executableSuffix}`),
    ],
    {
      cwd: output,
      stdio: "inherit",
      shell: false,
      env:
        process.platform === "win32"
          ? { ...process.env, PATH: `${dirname(compiler)};${process.env.PATH || ""}` }
          : process.env,
    },
  );
  if (result.error) throw result.error;
  if (result.status !== 0) process.exit(result.status ?? 1);
}

mkdirSync(output, { recursive: true });
writeFileSync(
  join(output, "VERSAO.txt"),
  `Praticas de Computacao Grafica ${packageJson.version}\nPlataforma: ${process.platform} ${process.arch}\n`,
);

if (process.platform === "win32" && compiler.startsWith(windowsToolchain)) {
  for (const dll of [
    "libfreeglut.dll",
    "libgcc_s_seh-1.dll",
    "libstdc++-6.dll",
    "libwinpthread-1.dll",
  ]) {
    const source = join(windowsToolchain, dll);
    if (existsSync(source)) cpSync(source, join(output, dll));
  }
}
cpSync(join(root, "atividades", "projeto-final", "assets"), join(output, "assets"), {
  recursive: true,
});

compile(
  [join(root, "atividades", "hierarquia-transformacoes", "src", "main.cpp")],
  "hierarquia-transformacoes",
);
compile([join(root, "atividades", "desafio-obstaculos", "src", "main.cpp")], "desafio-obstaculos");
compile(
  [
    join(root, "atividades", "projeto-final", "src", "main.cpp"),
    join(root, "atividades", "projeto-final", "src", "stb_image.cpp"),
  ],
  "colete-o-lixo",
  ["-I", join(root, "atividades", "projeto-final", "include")],
);

console.log(`Executáveis nativos criados em ${output}`);
