import { ArrowRight } from "lucide-react";
import { projects } from "../data";
import { Card, CardContent } from "../components/ui/card";

export function HomePage() {
  const base = import.meta.env.BASE_URL;
  return (
    <main>
      <section className="section container" id="atividades">
        <div className="section-heading">
          <h2>Atividades</h2>
        </div>
        <div className="grid">
          {projects.map((project) => (
            <a
              className="activity-card-link"
              href={`${base}atividades/${project.slug}`}
              key={project.slug}
              aria-label={`Abrir atividade: ${project.title}`}
            >
              <Card>
                <img
                  className="card-media"
                  src={`${base}${project.image}`}
                  alt={project.imageAlt}
                />
                <CardContent className="card-body">
                  <span className="card-number">{project.type}</span>
                  <h3>{project.shortTitle}</h3>
                  <p>{project.description}</p>
                  <span className="card-link">
                    Abrir atividade <ArrowRight size={17} />
                  </span>
                </CardContent>
              </Card>
            </a>
          ))}
        </div>
      </section>
    </main>
  );
}
