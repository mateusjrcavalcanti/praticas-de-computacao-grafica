import { useEffect, useState } from "react";
import { projects } from "./data";
import { HomePage } from "./pages/HomePage";
import { ProjectPage } from "./pages/ProjectPage";

function route() {
  const basePath = new URL(import.meta.env.BASE_URL, window.location.origin).pathname.replace(
    /\/$/,
    "",
  );
  const pathname = window.location.pathname.startsWith(basePath)
    ? window.location.pathname.slice(basePath.length)
    : window.location.pathname;
  return pathname.split("/").filter(Boolean);
}

export function App() {
  const [parts, setParts] = useState(route);
  const project =
    parts[0] === "atividades" ? projects.find((item) => item.slug === parts[1]) : undefined;

  useEffect(() => {
    const update = () => setParts(route());
    window.addEventListener("popstate", update);
    return () => window.removeEventListener("popstate", update);
  }, []);

  useEffect(() => {
    const siteName = "Práticas de Computação Gráfica";
    const title = project ? `${project.title} | ${siteName}` : siteName;
    const canonicalUrl = new URL(
      project ? `atividades/${project.slug}` : "",
      `${window.location.origin}${import.meta.env.BASE_URL}`,
    ).href;

    document.title = title;
    document.querySelector<HTMLLinkElement>('link[rel="canonical"]')?.setAttribute("href", canonicalUrl);
    document.querySelector<HTMLMetaElement>('meta[property="og:title"]')?.setAttribute("content", title);
    document
      .querySelector<HTMLMetaElement>('meta[property="og:description"]')
      ?.setAttribute(
        "content",
        project?.description || "Atividades em C++, OpenGL e WebAssembly executáveis no navegador.",
      );
    document
      .querySelector<HTMLMetaElement>('meta[property="og:url"]')
      ?.setAttribute("content", canonicalUrl);
  }, [project]);

  return (
    <div className="app-shell">
      {project ? <ProjectPage project={project} /> : <HomePage />}
      <footer>
        <div className="container">
          Desenvolvido na disciplina de Computação Gráfica da UNIVASF.
        </div>
      </footer>
    </div>
  );
}
