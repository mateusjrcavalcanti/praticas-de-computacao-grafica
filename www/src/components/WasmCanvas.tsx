import { Maximize2, Minimize2 } from "lucide-react";
import { useEffect, useRef, useState } from "react";
import type { Project } from "../data";

const wasmAssetVersion = Date.now().toString(36);

function createFrameDocument(project: Project, scriptUrl: string, assetVersion: string) {
  const title = project.title
    .replaceAll("&", "&amp;")
    .replaceAll('"', "&quot;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;");

  return `<!doctype html>
<html lang="pt-BR">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <style>
      * { box-sizing: border-box; }
      html, body { width: 100%; height: 100%; margin: 0; overflow: hidden; background: #02070d; }
      body { display: grid; place-items: center; }
      canvas { width: 100%; height: auto; max-height: 100vh; outline: none; }
      #status { position: fixed; right: 1rem; bottom: 1rem; left: 1rem; padding: .7rem .9rem; border: 1px solid #20344d; border-radius: 12px; background: rgba(7,17,31,.88); color: #9eb0c5; font: .9rem system-ui, sans-serif; }
    </style>
  </head>
  <body>
    <canvas id="canvas" width="${project.canvas.width}" height="${project.canvas.height}" tabindex="0" aria-label="Atividade interativa: ${title}"></canvas>
    <div id="status">Carregando atividade WebAssembly…</div>
    <script>
      const canvas = document.getElementById("canvas");
      const status = document.getElementById("status");
      canvas.addEventListener("pointerdown", () => canvas.focus());
      canvas.addEventListener("contextmenu", (event) => event.preventDefault());
      window.Module = {
        canvas,
        locateFile(path) {
          const url = new URL(path, ${JSON.stringify(scriptUrl)});
          url.searchParams.set("v", ${JSON.stringify(assetVersion)});
          return url.href;
        },
        onRuntimeInitialized() {
          status.textContent = "Atividade carregada — clique no quadro para interagir.";
        },
        onAbort(reason) {
          status.textContent = "Falha ao iniciar a atividade: " + String(reason);
        },
        printErr(message) {
          const text = String(message);
          const expectedLegacyWarning =
            text.startsWith("WARNING: using emscripten GL emulation.") ||
            text.startsWith("WARNING: using emscripten GL immediate mode emulation.");
          if (!expectedLegacyWarning) console.error(message);
        }
      };
      const runtime = document.createElement("script");
      runtime.src = ${JSON.stringify(scriptUrl)} + "?v=" + ${JSON.stringify(assetVersion)};
      runtime.onerror = () => {
        status.textContent =
          "Build WebAssembly ainda não disponível. Execute npm --prefix www run build.";
      };
      document.body.appendChild(runtime);
    </script>
  </body>
</html>`;
}

export function WasmCanvas({ project }: { project: Project }) {
  const shellRef = useRef<HTMLDivElement>(null);
  const [isFullscreen, setIsFullscreen] = useState(false);
  const scriptUrl = `${window.location.origin}${import.meta.env.BASE_URL}wasm/${project.slug}/app.js`;

  useEffect(() => {
    const updateFullscreenState = () => {
      setIsFullscreen(document.fullscreenElement === shellRef.current);
    };

    document.addEventListener("fullscreenchange", updateFullscreenState);
    return () => document.removeEventListener("fullscreenchange", updateFullscreenState);
  }, []);

  const toggleFullscreen = async () => {
    if (document.fullscreenElement === shellRef.current) {
      await document.exitFullscreen();
    } else {
      await shellRef.current?.requestFullscreen();
    }
  };

  return (
    <div className="demo-panel">
      <div className="canvas-shell" ref={shellRef}>
        <iframe
          key={project.slug}
          className="wasm-frame"
          srcDoc={createFrameDocument(project, scriptUrl, wasmAssetVersion)}
          title={`Atividade interativa: ${project.title}`}
          allow="fullscreen"
        />
        <button
          className="fullscreen"
          type="button"
          onClick={toggleFullscreen}
          aria-label={isFullscreen ? "Sair da tela cheia" : "Abrir em tela cheia"}
          title={isFullscreen ? "Sair da tela cheia" : "Abrir em tela cheia"}
        >
          {isFullscreen ? <Minimize2 size={18} /> : <Maximize2 size={18} />}
        </button>
      </div>
    </div>
  );
}
