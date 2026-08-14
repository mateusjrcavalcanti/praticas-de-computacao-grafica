export type Project = {
  slug: string;
  type: string;
  title: string;
  shortTitle: string;
  description: string;
  image: string;
  imageAlt: string;
  controls: { key: string; action: string }[];
  sourceUrl: string;
  readmeUrl: string;
  canvas: { width: number; height: number };
};

const github =
  import.meta.env.VITE_REPOSITORY_URL ||
  "https://github.com/mateusjrcavalcanti/praticas-de-computacao-grafica";

export const projects: Project[] = [
  {
    slug: "hierarquia-transformacoes",
    type: "Atividade 02",
    title: "Hierarquia e transformações geométricas",
    shortTitle: "Hierarquia e transformações",
    description: "Manipule uma estrutura articulada com três níveis de transformação encadeada.",
    image: "assets/hierarquia.png",
    imageAlt: "Exemplo de estrutura articulada",
    controls: [
      { key: "Setas", action: "Movimentam a base" },
      { key: "Q / W", action: "Rotacionam o primeiro braço" },
      { key: "A / S", action: "Rotacionam o segundo braço" },
      { key: "Z / X", action: "Rotacionam o terceiro braço" },
    ],
    sourceUrl: `${github}/blob/main/atividades/hierarquia-transformacoes/src/main.cpp`,
    readmeUrl: `${github}#hierarquia-e-transformações-geométricas`,
    canvas: { width: 800, height: 600 },
  },
  {
    slug: "desafio-obstaculos",
    type: "Atividade 03",
    title: "Desafio dos obstáculos",
    shortTitle: "Desafio dos obstáculos",
    description:
      "Desvie dos sete obstáculos e alcance a faixa verde antes de perder suas três vidas.",
    image: "assets/obstaculos.png",
    imageAlt: "Enunciado visual do desafio dos obstáculos",
    controls: [
      { key: "Setas", action: "Movimentam o objeto" },
      { key: "Objetivo", action: "Alcance a faixa verde no topo" },
      { key: "Colisão", action: "Remove uma vida e reinicia a posição" },
      { key: "R", action: "Reinicia a partida" },
    ],
    sourceUrl: `${github}/blob/main/atividades/desafio-obstaculos/src/main.cpp`,
    readmeUrl: `${github}#desafio-dos-obstáculos`,
    canvas: { width: 600, height: 600 },
  },
  {
    slug: "projeto-final",
    type: "Projeto final",
    title: "Colete o lixo",
    shortTitle: "Colete o lixo",
    description: "Colete os resíduos antes que eles atinjam o lago e controle o ritmo da partida.",
    image: "assets/lago.jpg",
    imageAlt: "Lago usado como cenário do projeto final",
    controls: [
      { key: "Iniciar jogo", action: "Começa a partida" },
      { key: "Clique", action: "Coleta o resíduo" },
      { key: "↑ / ↓", action: "Altera a velocidade" },
      { key: "P", action: "Pausa ou continua" },
      { key: "R", action: "Reinicia a partida" },
    ],
    sourceUrl: `${github}/blob/main/atividades/projeto-final/src/main.cpp`,
    readmeUrl: `${github}#colete-o-lixo`,
    canvas: { width: 900, height: 600 },
  },
];
