//*****************************************************************
//
// Colete o Lixo
// Projeto da disciplina de Computacao Grafica 2021.2
// Autores: @mateusjrcavalcanti e Talisson Cardoso
//
//*****************************************************************

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <algorithm>
#include <array>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <random>
#include <string>

#include "stb_image.h"

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

namespace {

constexpr int LARGURA_INICIAL = 900;
constexpr int ALTURA_INICIAL = 600;
constexpr int INTERVALO_FRAME_MS = 16;
constexpr float MUNDO_LARGURA = 6.186f;
constexpr float MUNDO_ALTURA = 4.124f;
constexpr float MUNDO_ESQUERDA = -MUNDO_LARGURA / 2.0f;
constexpr float MUNDO_DIREITA = MUNDO_LARGURA / 2.0f;
constexpr float MUNDO_BAIXO = -MUNDO_ALTURA / 2.0f;
constexpr float MUNDO_CIMA = MUNDO_ALTURA / 2.0f;
constexpr float LIMITE_LAGO = -0.8f;
constexpr float VELOCIDADE_MINIMA = 0.20f;
constexpr float VELOCIDADE_MAXIMA = 1.40f;
constexpr float PASSO_VELOCIDADE = 0.15f;
constexpr float BOTAO_ESQUERDA = -1.05f;
constexpr float BOTAO_DIREITA = 1.05f;
constexpr float BOTAO_BAIXO = -0.30f;
constexpr float BOTAO_CIMA = 0.25f;

enum class EstadoJogo { AguardandoInicio, Jogando, Pausado, FimDeJogo };

enum ImagemId {
    LAGO,
    VIDA,
    LATA,
    PNEU,
    GARRAFA_AZUL,
    GARRAFA_VERDE,
    PAPEL,
    QUANTIDADE_IMAGENS
};

struct Imagem {
    std::string arquivo;
    float altura;
    float largura;
};

struct Lixo {
    ImagemId imagem;
    float x;
    float y;
    int pontos;
    float fatorVelocidade;
};

const std::array<Imagem, QUANTIDADE_IMAGENS> imagens = {{
    {"lago.png", MUNDO_ALTURA, MUNDO_LARGURA},
    {"vida.png", 0.30f, 0.30f},
    {"lata1.png", 0.50f, 0.25f},
    {"pneu.png", 0.52f, 0.52f},
    {"garrafa1.png", 0.55f, 0.28f},
    {"garrafa2.png", 0.70f, 0.36f},
    {"papel.png", 0.55f, 0.55f},
}};

std::array<Lixo, 6> lixos = {{
    {PNEU, -2.50f, 2.30f, 15, 0.85f},
    {LATA, -1.50f, 2.85f, 10, 1.10f},
    {GARRAFA_AZUL, -0.50f, 3.40f, 10, 0.95f},
    {GARRAFA_VERDE, 0.50f, 3.95f, 12, 1.05f},
    {PAPEL, 1.50f, 4.50f, 5, 1.15f},
    {GARRAFA_VERDE, 2.50f, 5.05f, 12, 0.90f},
}};

std::array<GLuint, QUANTIDADE_IMAGENS> texturas{};
std::string diretorioAssets = "assets";
EstadoJogo estado = EstadoJogo::AguardandoInicio;
int vidas = 3;
int pontos = 0;
float velocidade = 0.35f;
int ultimoTempoMs = 0;
int viewportX = 0;
int viewportY = 0;
int viewportLargura = LARGURA_INICIAL;
int viewportAltura = ALTURA_INICIAL;
int alturaJanela = ALTURA_INICIAL;
std::mt19937 gerador{std::random_device{}()};

void criarTexturaFallback(GLuint textura) {
    constexpr unsigned char pixels[] = {
        255, 0, 255, 255, 30, 30, 30, 255,
        30, 30, 30, 255, 255, 0, 255, 255,
    };
    glBindTexture(GL_TEXTURE_2D, textura);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

bool carregarTextura(GLuint textura, const std::string& caminho) {
    int larguraImagem = 0;
    int alturaImagem = 0;
    int canais = 0;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* dados = stbi_load(caminho.c_str(), &larguraImagem, &alturaImagem, &canais, 4);

    if (dados == nullptr) {
        std::cerr << "Nao foi possivel carregar a textura: " << caminho << '\n';
        criarTexturaFallback(textura);
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, textura);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, larguraImagem, alturaImagem, 0, GL_RGBA, GL_UNSIGNED_BYTE, dados);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    stbi_image_free(dados);
    return true;
}

void desenharImagem(ImagemId id, float x, float y) {
    const Imagem& imagem = imagens[id];
    glBindTexture(GL_TEXTURE_2D, texturas[id]);
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-imagem.largura / 2.0f, -imagem.altura / 2.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(imagem.largura / 2.0f, -imagem.altura / 2.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(imagem.largura / 2.0f, imagem.altura / 2.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-imagem.largura / 2.0f, imagem.altura / 2.0f);
    glEnd();
    glPopMatrix();
}

std::array<unsigned char, 7> glifo(char caractere) {
    switch (caractere) {
        case 'A': return {14, 17, 17, 31, 17, 17, 17};
        case 'B': return {30, 17, 17, 30, 17, 17, 30};
        case 'C': return {14, 17, 16, 16, 16, 17, 14};
        case 'D': return {30, 17, 17, 17, 17, 17, 30};
        case 'E': return {31, 16, 16, 30, 16, 16, 31};
        case 'F': return {31, 16, 16, 30, 16, 16, 16};
        case 'G': return {14, 17, 16, 23, 17, 17, 15};
        case 'H': return {17, 17, 17, 31, 17, 17, 17};
        case 'I': return {31, 4, 4, 4, 4, 4, 31};
        case 'J': return {7, 2, 2, 2, 18, 18, 12};
        case 'K': return {17, 18, 20, 24, 20, 18, 17};
        case 'L': return {16, 16, 16, 16, 16, 16, 31};
        case 'M': return {17, 27, 21, 21, 17, 17, 17};
        case 'N': return {17, 25, 21, 19, 17, 17, 17};
        case 'O': return {14, 17, 17, 17, 17, 17, 14};
        case 'P': return {30, 17, 17, 30, 16, 16, 16};
        case 'Q': return {14, 17, 17, 17, 21, 18, 13};
        case 'R': return {30, 17, 17, 30, 20, 18, 17};
        case 'S': return {15, 16, 16, 14, 1, 1, 30};
        case 'T': return {31, 4, 4, 4, 4, 4, 4};
        case 'U': return {17, 17, 17, 17, 17, 17, 14};
        case 'V': return {17, 17, 17, 17, 17, 10, 4};
        case 'W': return {17, 17, 17, 21, 21, 21, 10};
        case 'X': return {17, 17, 10, 4, 10, 17, 17};
        case 'Y': return {17, 17, 10, 4, 4, 4, 4};
        case 'Z': return {31, 1, 2, 4, 8, 16, 31};
        case '0': return {14, 17, 19, 21, 25, 17, 14};
        case '1': return {4, 12, 4, 4, 4, 4, 14};
        case '2': return {14, 17, 1, 2, 4, 8, 31};
        case '3': return {30, 1, 1, 14, 1, 1, 30};
        case '4': return {2, 6, 10, 18, 31, 2, 2};
        case '5': return {31, 16, 16, 30, 1, 1, 30};
        case '6': return {14, 16, 16, 30, 17, 17, 14};
        case '7': return {31, 1, 2, 4, 8, 8, 8};
        case '8': return {14, 17, 17, 14, 17, 17, 14};
        case '9': return {14, 17, 17, 15, 1, 1, 14};
        case ':': return {0, 4, 4, 0, 4, 4, 0};
        case '%': return {25, 26, 2, 4, 8, 11, 19};
        default: return {0, 0, 0, 0, 0, 0, 0};
    }
}

void desenharTexto(float x, float y, const std::string& texto, float escala = 0.025f) {
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.0f, 0.0f, 0.0f);
    for (char caractere : texto) {
        const auto linhas = glifo(caractere);
        glBegin(GL_QUADS);
        for (int linha = 0; linha < 7; ++linha) {
            for (int coluna = 0; coluna < 5; ++coluna) {
                if ((linhas[linha] & (1 << (4 - coluna))) == 0) {
                    continue;
                }
                const float px = x + coluna * escala;
                const float py = y - linha * escala;
                glVertex2f(px, py);
                glVertex2f(px + escala * 0.82f, py);
                glVertex2f(px + escala * 0.82f, py - escala * 0.82f);
                glVertex2f(px, py - escala * 0.82f);
            }
        }
        glEnd();
        x += escala * 6.0f;
    }
    glEnable(GL_TEXTURE_2D);
}

void desenharBotao(const std::string& rotulo, float textoX) {
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.78f, 0.92f, 0.68f);
    glBegin(GL_QUADS);
    glVertex2f(BOTAO_ESQUERDA, BOTAO_BAIXO);
    glVertex2f(BOTAO_DIREITA, BOTAO_BAIXO);
    glVertex2f(BOTAO_DIREITA, BOTAO_CIMA);
    glVertex2f(BOTAO_ESQUERDA, BOTAO_CIMA);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(BOTAO_ESQUERDA, BOTAO_BAIXO);
    glVertex2f(BOTAO_DIREITA, BOTAO_BAIXO);
    glVertex2f(BOTAO_DIREITA, BOTAO_CIMA);
    glVertex2f(BOTAO_ESQUERDA, BOTAO_CIMA);
    glEnd();
    glEnable(GL_TEXTURE_2D);
    desenharTexto(textoX, 0.07f, rotulo, 0.025f);
}

void desenharHud() {
    for (int i = 0; i < vidas; ++i) {
        desenharImagem(VIDA, MUNDO_DIREITA - 0.22f - i * 0.34f, MUNDO_BAIXO + 0.22f);
    }
    desenharTexto(MUNDO_ESQUERDA + 0.12f, MUNDO_CIMA - 0.18f, "PONTOS: " + std::to_string(pontos));
    desenharTexto(
        MUNDO_ESQUERDA + 0.12f,
        MUNDO_CIMA - 0.46f,
        "VELOCIDADE: " + std::to_string(static_cast<int>(velocidade * 100.0f)) + "%",
        0.018f);

    if (estado == EstadoJogo::AguardandoInicio) {
        desenharTexto(-0.90f, 0.62f, "COLETE O LIXO", 0.025f);
        desenharBotao("INICIAR JOGO", -0.89f);
    } else if (estado == EstadoJogo::Pausado) {
        desenharTexto(-0.52f, 0.12f, "PAUSADO", 0.025f);
        desenharTexto(-1.35f, -0.20f, "PRESSIONE P PARA CONTINUAR", 0.016f);
    } else if (estado == EstadoJogo::FimDeJogo) {
        desenharTexto(-0.82f, 0.62f, "FIM DE JOGO", 0.025f);
        desenharBotao("JOGAR NOVAMENTE", -1.00f);
    }
}

void desenhar() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(1.0f, 1.0f, 1.0f);

    desenharImagem(LAGO, 0.0f, 0.0f);
    if (estado != EstadoJogo::AguardandoInicio) {
        for (const Lixo& lixo : lixos) {
            desenharImagem(lixo.imagem, lixo.x, lixo.y);
        }
    }
    desenharHud();
    glutSwapBuffers();
}

float maiorYAtual() {
    float maior = MUNDO_CIMA;
    for (const Lixo& lixo : lixos) {
        maior = std::max(maior, lixo.y);
    }
    return maior;
}

void reposicionarLixo(Lixo& lixo) {
    std::uniform_real_distribution<float> intervalo(0.35f, 1.05f);
    lixo.y = maiorYAtual() + intervalo(gerador);
}

void redefinirPartida(EstadoJogo novoEstado) {
    vidas = 3;
    pontos = 0;
    velocidade = 0.35f;
    estado = novoEstado;
    for (std::size_t i = 0; i < lixos.size(); ++i) {
        lixos[i].y = 2.30f + static_cast<float>(i) * 0.55f;
    }
    ultimoTempoMs = glutGet(GLUT_ELAPSED_TIME);
}

void iniciarJogo() {
    redefinirPartida(EstadoJogo::Jogando);
}

void atualizar(float deltaTime) {
    if (estado != EstadoJogo::Jogando) {
        return;
    }

    for (Lixo& lixo : lixos) {
        lixo.y -= velocidade * lixo.fatorVelocidade * deltaTime;
        if (lixo.y < LIMITE_LAGO) {
            vidas = std::max(0, vidas - 1);
            if (vidas == 0) {
                estado = EstadoJogo::FimDeJogo;
                break;
            }
            reposicionarLixo(lixo);
        }
    }
}

void temporizador(int) {
    const int tempoAtual = glutGet(GLUT_ELAPSED_TIME);
    const float deltaTime = std::min((tempoAtual - ultimoTempoMs) / 1000.0f, 0.05f);
    ultimoTempoMs = tempoAtual;
    atualizar(deltaTime);
    glutPostRedisplay();
    glutTimerFunc(INTERVALO_FRAME_MS, temporizador, 0);
}

bool telaParaMundo(int mouseX, int mouseY, float& mundoX, float& mundoY) {
    const int yOpenGl = alturaJanela - mouseY;
    if (mouseX < viewportX || mouseX > viewportX + viewportLargura || yOpenGl < viewportY ||
        yOpenGl > viewportY + viewportAltura) {
        return false;
    }

    mundoX = MUNDO_ESQUERDA +
             static_cast<float>(mouseX - viewportX) / viewportLargura * MUNDO_LARGURA;
    mundoY = MUNDO_BAIXO +
             static_cast<float>(yOpenGl - viewportY) / viewportAltura * MUNDO_ALTURA;
    return true;
}

bool contemPonto(const Lixo& lixo, float x, float y) {
    const Imagem& imagem = imagens[lixo.imagem];
    return x >= lixo.x - imagem.largura / 2.0f && x <= lixo.x + imagem.largura / 2.0f &&
           y >= lixo.y - imagem.altura / 2.0f && y <= lixo.y + imagem.altura / 2.0f;
}

void gerenciarMouse(int botao, int acao, int x, int y) {
    if (botao != GLUT_LEFT_BUTTON || acao != GLUT_DOWN) {
        return;
    }

    float mundoX = 0.0f;
    float mundoY = 0.0f;
    if (!telaParaMundo(x, y, mundoX, mundoY)) {
        return;
    }

    if ((estado == EstadoJogo::AguardandoInicio || estado == EstadoJogo::FimDeJogo) &&
        mundoX >= BOTAO_ESQUERDA && mundoX <= BOTAO_DIREITA && mundoY >= BOTAO_BAIXO &&
        mundoY <= BOTAO_CIMA) {
        iniciarJogo();
        glutPostRedisplay();
        return;
    }

    if (estado != EstadoJogo::Jogando) {
        return;
    }

    for (auto it = lixos.rbegin(); it != lixos.rend(); ++it) {
        if (contemPonto(*it, mundoX, mundoY)) {
            pontos += it->pontos;
            reposicionarLixo(*it);
            break;
        }
    }
    glutPostRedisplay();
}

void teclado(unsigned char tecla, int, int) {
    switch (tecla) {
        case 'p':
        case 'P':
            if (estado == EstadoJogo::Jogando) {
                estado = EstadoJogo::Pausado;
            } else if (estado == EstadoJogo::Pausado) {
                estado = EstadoJogo::Jogando;
                ultimoTempoMs = glutGet(GLUT_ELAPSED_TIME);
            }
            break;
        case 'r':
        case 'R':
            iniciarJogo();
            break;
        case 27:
#ifndef __EMSCRIPTEN__
            std::exit(0);
#endif
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void teclasEspeciais(int tecla, int, int) {
    if (tecla == GLUT_KEY_DOWN) {
        velocidade = std::max(VELOCIDADE_MINIMA, velocidade - PASSO_VELOCIDADE);
    } else if (tecla == GLUT_KEY_UP) {
        velocidade = std::min(VELOCIDADE_MAXIMA, velocidade + PASSO_VELOCIDADE);
    }
    glutPostRedisplay();
}

void alterarTamanhoJanela(int largura, int altura) {
    altura = std::max(altura, 1);
    alturaJanela = altura;
    const float aspectoMundo = MUNDO_LARGURA / MUNDO_ALTURA;
    const float aspectoJanela = static_cast<float>(largura) / altura;

    if (aspectoJanela > aspectoMundo) {
        viewportAltura = altura;
        viewportLargura = static_cast<int>(altura * aspectoMundo);
        viewportX = (largura - viewportLargura) / 2;
        viewportY = 0;
    } else {
        viewportLargura = largura;
        viewportAltura = static_cast<int>(largura / aspectoMundo);
        viewportX = 0;
        viewportY = (altura - viewportAltura) / 2;
    }

    glViewport(viewportX, viewportY, viewportLargura, viewportAltura);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(MUNDO_ESQUERDA, MUNDO_DIREITA, MUNDO_BAIXO, MUNDO_CIMA, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void inicializar() {
    glClearColor(0.01f, 0.03f, 0.05f, 1.0f);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(static_cast<GLsizei>(texturas.size()), texturas.data());
    for (std::size_t i = 0; i < imagens.size(); ++i) {
        carregarTextura(texturas[i], diretorioAssets + "/" + imagens[i].arquivo);
    }

    redefinirPartida(EstadoJogo::AguardandoInicio);
}

void imprimirInstrucoes() {
    std::cout << "Colete o lixo antes que ele alcance o lago.\n"
              << "Clique: coletar | P: pausar | R: reiniciar | Setas: velocidade | Esc: sair\n";
}

}  // namespace

int main(int argc, char** argv) {
#ifndef __EMSCRIPTEN__
    diretorioAssets =
        (std::filesystem::absolute(argv[0]).parent_path() / "assets").string();
#endif

    imprimirInstrucoes();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(LARGURA_INICIAL, ALTURA_INICIAL);
    glutCreateWindow("Colete o Lixo");

    glutDisplayFunc(desenhar);
    glutReshapeFunc(alterarTamanhoJanela);
    glutKeyboardFunc(teclado);
    glutSpecialFunc(teclasEspeciais);
    glutMouseFunc(gerenciarMouse);

    inicializar();
#ifndef __EMSCRIPTEN__
    glutPositionWindow(
        (glutGet(GLUT_SCREEN_WIDTH) - LARGURA_INICIAL) / 2,
        (glutGet(GLUT_SCREEN_HEIGHT) - ALTURA_INICIAL) / 2);
#endif
    ultimoTempoMs = glutGet(GLUT_ELAPSED_TIME);
    glutTimerFunc(INTERVALO_FRAME_MS, temporizador, 0);
    glutMainLoop();
    return 0;
}
