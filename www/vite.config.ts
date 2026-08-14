import react from "@vitejs/plugin-react";
import tailwindcss from "@tailwindcss/vite";
import { copyFileSync, writeFileSync } from "node:fs";
import { resolve } from "node:path";
import { defineConfig } from "vite";

const configuredBasePath = process.env.VITE_BASE_PATH || "/";
const base = configuredBasePath.endsWith("/") ? configuredBasePath : `${configuredBasePath}/`;
const siteUrl = (process.env.VITE_SITE_URL || "http://localhost:5173").replace(/\/$/, "");
const commitSha = process.env.VITE_COMMIT_SHA || "desenvolvimento";
const publicRoutes = [
  "",
  "atividades/hierarquia-transformacoes",
  "atividades/desafio-obstaculos",
  "atividades/projeto-final",
];

export default defineConfig({
  base,
  plugins: [
    {
      name: "site-metadata",
      transformIndexHtml(html) {
        return html.replaceAll("%SITE_URL%", siteUrl).replaceAll("%COMMIT_SHA%", commitSha);
      },
    },
    {
      name: "github-pages-spa-fallback",
      closeBundle() {
        copyFileSync(resolve("dist/index.html"), resolve("dist/404.html"));
        writeFileSync(
          resolve("dist/robots.txt"),
          `User-agent: *\nAllow: /\nSitemap: ${siteUrl}/sitemap.xml\n`,
        );
        const urls = publicRoutes
          .map((route) => `  <url><loc>${siteUrl}/${route}</loc></url>`)
          .join("\n");
        writeFileSync(
          resolve("dist/sitemap.xml"),
          `<?xml version="1.0" encoding="UTF-8"?>\n<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">\n${urls}\n</urlset>\n`,
        );
      },
    },
    react(),
    tailwindcss(),
  ],
});
