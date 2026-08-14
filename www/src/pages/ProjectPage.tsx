import { ArrowLeft, BookOpen, Code2 } from "lucide-react";
import type { Project } from "../data";
import { WasmCanvas } from "../components/WasmCanvas";
import { Button } from "../components/ui/button";
import { Card, CardContent } from "../components/ui/card";

export function ProjectPage({ project }: { project: Project }) {
  const base = import.meta.env.BASE_URL;
  return (
    <main className="container">
      <section className="detail-hero">
        <p className="eyebrow">{project.type}</p>
        <h1>{project.title}</h1>
        <p className="hero-copy">{project.description}</p>
      </section>
      <section className="detail-layout">
        <WasmCanvas project={project} />
        <aside className="sidebar">
          <Card className="panel">
            <CardContent className="p-0">
              <h2>Controles</h2>
              <dl>
                {project.controls.map((control) => (
                  <div key={control.key}>
                    <dt>{control.key}</dt>
                    <dd>{control.action}</dd>
                  </div>
                ))}
              </dl>
              <p>Clique na área da atividade antes de usar o teclado.</p>
            </CardContent>
          </Card>
          <Button asChild className="w-full">
            <a href={project.sourceUrl} target="_blank" rel="noreferrer">
              <Code2 size={18} /> Ver código-fonte
            </a>
          </Button>
          <Button asChild variant="outline" className="w-full">
            <a href={project.readmeUrl} target="_blank" rel="noreferrer">
              <BookOpen size={18} /> Ver no README
            </a>
          </Button>
          <Button asChild variant="outline" className="w-full">
            <a href={base}>
              <ArrowLeft size={18} /> Voltar às atividades
            </a>
          </Button>
        </aside>
      </section>
    </main>
  );
}
