import { existsSync } from "node:fs";
import { spawnSync } from "node:child_process";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const scriptDirectory = dirname(fileURLToPath(import.meta.url));
const root = resolve(scriptDirectory, "..");
const emsdkDirectory = join(root, ".tools", "emsdk");
const isWindows = process.platform === "win32";

function run(command, args, cwd = root) {
  const result = spawnSync(command, args, {
    cwd,
    stdio: "inherit",
    shell: isWindows && command.toLowerCase().endsWith(".bat"),
  });
  if (result.error?.code === "ENOENT") {
    console.error(`${command} não encontrado. Instale-o e tente novamente.`);
    process.exit(1);
  }
  if (result.status !== 0) process.exit(result.status ?? 1);
}

if (!existsSync(join(emsdkDirectory, ".git"))) {
  console.log("Baixando o Emscripten SDK…");
  run("git", [
    "clone",
    "--depth",
    "1",
    "https://github.com/emscripten-core/emsdk.git",
    emsdkDirectory,
  ]);
} else {
  console.log("Atualizando o Emscripten SDK existente…");
  run("git", ["pull", "--ff-only"], emsdkDirectory);
}

const emsdk = join(emsdkDirectory, isWindows ? "emsdk.bat" : "emsdk");
console.log("Instalando a versão estável mais recente…");
run(emsdk, ["install", "latest"], emsdkDirectory);
run(emsdk, ["activate", "latest"], emsdkDirectory);

console.log("Emscripten instalado localmente em .tools/emsdk.");
console.log("Emscripten pronto para a compilação WebAssembly.");
