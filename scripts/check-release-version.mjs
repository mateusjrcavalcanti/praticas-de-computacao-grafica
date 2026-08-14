import { readFileSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const scriptDirectory = dirname(fileURLToPath(import.meta.url));
const root = resolve(scriptDirectory, "..");
const packageJson = JSON.parse(readFileSync(join(root, "www", "package.json"), "utf8"));
const tag = process.env.GITHUB_REF_NAME || process.argv[2];
const expectedTag = `v${packageJson.version}`;

if (!tag) {
  console.error("Informe a tag como argumento ou pela variavel GITHUB_REF_NAME.");
  process.exit(1);
}

if (tag !== expectedTag) {
  console.error(
    `A tag ${tag} nao corresponde a versao ${packageJson.version}. Esperado: ${expectedTag}.`,
  );
  process.exit(1);
}

console.log(`Versao validada: ${expectedTag}`);
