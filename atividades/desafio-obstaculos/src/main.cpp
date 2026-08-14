#include <stdio.h>
#include <stdlib.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const float POSICAO_INICIAL_Y = -22.0f;
const float POSICAO_CHEGADA_Y = 22.0f;
const float LIMITE_PISTA_X = 21.0f;
GLfloat tx = 0, ty = POSICAO_INICIAL_Y, win = 25;
float limiteVisivelY = 25.0f;
float altura = 4.8, largura = 2.2;
int vidas = 3;

enum EstadoJogo { JOGANDO, VITORIA, DERROTA };
EstadoJogo estadoJogo = JOGANDO;

void ReiniciaJogo(void)
{
	tx = 0;
	ty = POSICAO_INICIAL_Y;
	vidas = 3;
	estadoJogo = JOGANDO;
}

typedef struct
{
    char tipo;
    int tamanho;
    float posicao[2];
    float colors[3];
} Obstaculo;

Obstaculo SetObstaculo(char tipo, int tamanho, float posicaoX, float posicaoY, float colorum, float colordois, float colortres)
{ 
    Obstaculo Ob;
    Ob.tipo = tipo; 
    Ob.posicao[0] = posicaoX;  
    Ob.posicao[1] = posicaoY;   
    Ob.tamanho = tamanho;  
    Ob.colors[0] = colorum;  
    Ob.colors[1] = colordois;  
    Ob.colors[2] = colortres;
    return Ob;
}

Obstaculo obstaculos[] = {
	SetObstaculo('t', 3, 	-17.0f, 18.0f,	1.0f, 0.0f, 0.0f),
	SetObstaculo('r', 5, 	-16.0f, 5.0f, 	0.0f, 1.0f, 0.0f),
	SetObstaculo('p', 6, 	-1.0f, 8.0f, 	1.0f, 1.0f, 0.0f),
	SetObstaculo('q', 8, 	15.0f, 17.0f, 	0.0f, 0.0f, 1.0f),
	SetObstaculo('g', 7, 	11.0f, -5.5f, 	1.0f, 0.0f, 1.0f),
	SetObstaculo('g', 7, 	-13.0f, -11.0f, 0.0f, 1.0f, 1.0f),
	SetObstaculo('z', 6, 	17.0f, 5.5f, 	0.7f, 0.4f, 0.2f),
	};
	
bool ptInTriangle(float x, float y, float x0, float y0, float x1, float y1, float x2, float y2) {
	float p0[2] = {x0, y0}, p1[2] = {x1,y1}, p2[2] = {x2,y2};
    float dX = x-p2[0];
    float dY = y-p2[1];
    float dX21 = p2[0]-p1[0];
    float dY12 = p1[1]-p2[1];
    float D = dY12*(p0[0]-p2[0]) + dX21*(p0[1]-p2[1]);
    float s = dY12*dX + dX21*dY;
    float t = (p2[1]-p0[1])*dX + (p0[0]-p2[0])*dY;
    if (D<0) return s<=0 && t<=0 && s+t>=D;
    return s>=0 && t>=0 && s+t<=D;
}

bool colidiu(float x0, float y0, float x1, float y1, float x2, float y2) {
	return	ptInTriangle(tx + largura/2, ty + altura/2, x0, y0, x1, y1, x2, y2) || 
			ptInTriangle(tx + largura/2, ty - altura/2, x0, y0, x1, y1, x2, y2) || 
			ptInTriangle(tx - largura/2, ty + altura/2, x0, y0, x1, y1, x2, y2) || 
			ptInTriangle(tx - largura/2, ty - altura/2, x0, y0, x1, y1, x2, y2);
}

void Colisao(void)
{
	if (estadoJogo != JOGANDO) return;
	tx = 0;
	ty = POSICAO_INICIAL_Y;
	vidas--;
	if (vidas <= 0) {
		vidas = 0;
		estadoJogo = DERROTA;
	}
}

const char *Glifo(char c)
{
	switch (c) {
		case 'A': return "01110100011000111111100011000110001";
		case 'C': return "01111100001000010000100001000001111";
		case 'D': return "11110100011000110001100011000111110";
		case 'E': return "11111100001000011110100001000011111";
		case 'F': return "11111100001000011110100001000010000";
		case 'G': return "01111100001000010111100011000101111";
		case 'I': return "11111001000010000100001000010011111";
		case 'J': return "00111000100001000010000101001001100";
		case 'M': return "10001110111010110101100011000110001";
		case 'N': return "10001110011010110011100011000110001";
		case 'O': return "01110100011000110001100011000101110";
		case 'P': return "11110100011000111110100001000010000";
		case 'R': return "11110100011000111110101001001010001";
		case 'S': return "01111100001000001110000010000111110";
		case 'T': return "11111001000010000100001000010000100";
		case 'U': return "10001100011000110001100011000101110";
		case 'V': return "10001100011000110001010100010000100";
		case '0': return "01110100011001110101110011000101110";
		case '1': return "00100011000010000100001000010001110";
		case '2': return "01110100010000100010001000100011111";
		case '3': return "11110000010000101110000010000111110";
		case '4': return "00010001100101010010111110001000010";
		case '5': return "11111100001000011110000010000111110";
		case '6': return "01110100001000011110100011000101110";
		case '7': return "11111000010001000100010000100001000";
		case '8': return "01110100011000101110100011000101110";
		case '9': return "01110100011000101111000010000101110";
		case ':': return "00000001000010000000001000010000000";
		case '%': return "11001110100001000100010001011110011";
		default: return "00000000000000000000000000000000000";
	}
}

void DesenhaTexto(float x, float y, const char *texto, float escala = 0.22f)
{
	float cursor = x;
	for (const char *c = texto; *c != '\0'; c++) {
		const char *glifo = Glifo(*c);
		for (int linha = 0; linha < 7; linha++) {
			for (int coluna = 0; coluna < 5; coluna++) {
				if (glifo[linha * 5 + coluna] != '1') continue;
				float px = cursor + coluna * escala;
				float py = y - linha * escala;
				glBegin(GL_QUADS);
					glVertex2f(px, py); glVertex2f(px + escala * 0.82f, py);
					glVertex2f(px + escala * 0.82f, py + escala * 0.82f); glVertex2f(px, py + escala * 0.82f);
				glEnd();
			}
		}
		cursor += escala * 6.0f;
	}
}

void DesenhaTextoCentralizado(float centroX, float y, const char *texto, float escala = 0.22f)
{
	int caracteres = 0;
	while (texto[caracteres] != '\0') caracteres++;
	float larguraTexto = caracteres > 0 ? (caracteres * 6.0f - 1.0f) * escala : 0.0f;
	DesenhaTexto(centroX - larguraTexto / 2.0f, y, texto, escala);
}

void DesenhaInterface(void)
{
	char texto[80];
	float progresso = (ty - POSICAO_INICIAL_Y) / (POSICAO_CHEGADA_Y - POSICAO_INICIAL_Y);
	if (progresso < 0.0f) progresso = 0.0f;
	if (progresso > 1.0f) progresso = 1.0f;

	glColor3f(0.08f, 0.12f, 0.18f);
	sprintf(texto, "VIDAS: %d", vidas);
	DesenhaTexto(-win + 1.5f, 23.0f, texto);
	sprintf(texto, "PROGRESSO: %d%%", (int)(progresso * 100.0f));
	DesenhaTexto(-win + 1.5f, 21.2f, texto, 0.16f);

	glColor3f(0.82f, 0.85f, 0.88f);
	glBegin(GL_QUADS);
		glVertex2f(-win + 1.5f, 19.8f); glVertex2f(-win + 13.5f, 19.8f);
		glVertex2f(-win + 13.5f, 20.5f); glVertex2f(-win + 1.5f, 20.5f);
	glEnd();
	glColor3f(0.10f, 0.62f, 0.32f);
	glBegin(GL_QUADS);
		glVertex2f(-win + 1.5f, 19.8f); glVertex2f(-win + 1.5f + 12.0f * progresso, 19.8f);
		glVertex2f(-win + 1.5f + 12.0f * progresso, 20.5f); glVertex2f(-win + 1.5f, 20.5f);
	glEnd();

	if (estadoJogo != JOGANDO) {
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);
			glVertex2f(-13.0f, -4.0f); glVertex2f(13.0f, -4.0f);
			glVertex2f(13.0f, 4.0f); glVertex2f(-13.0f, 4.0f);
		glEnd();
		glColor3f(estadoJogo == VITORIA ? 0.05f : 0.72f, estadoJogo == VITORIA ? 0.50f : 0.08f, 0.12f);
		DesenhaTextoCentralizado(0.0f, 1.0f, estadoJogo == VITORIA ? "VOCE VENCEU!" : "FIM DE JOGO");
		glColor3f(0.08f, 0.12f, 0.18f);
		DesenhaTexto(-10.5f, -1.5f, "PRESSIONE R PARA REINICIAR", 0.13f);
	}
}
	
void DesenhaObstaculo(char tipo, float posicao[2], int tamanho, float colors[3] )
{
	float t1, t2, t3, t4, n;	
	float angleIncrement, angle;
	glColor3f(colors[0], colors[1], colors[2]);	
	switch(tipo) {
	  case 't':
	  	t1 = posicao[0]-2*tamanho;
	  	t2 = posicao[0]+2*tamanho;
	  	t3 = posicao[1]+1.5*tamanho;
	  	t4 = posicao[1]-1.5*tamanho;
	    glScalef(1.0f, 1.0f, 1.0f);
	    glBegin(GL_TRIANGLES);
			glVertex2f(t1, t4);
			glVertex2f(t1, t3);
			glVertex2f(t2, t4);
		glEnd();	
		if(colidiu(t1, t4, t1, t3, t2, t4)) Colisao();		
	    break;	  
	  case 'r':
	  	t1 = posicao[0]-tamanho;
	  	t2 = posicao[0]+tamanho;
	  	t3 = posicao[1]+tamanho;
	  	t4 = posicao[1]-tamanho;
	    glBegin(GL_TRIANGLES);
			glVertex2f(t1, t4);
			glVertex2f(t2, t4);
			glVertex2f(posicao[0],t3);
		glEnd();
		if(colidiu(t1, t4, posicao[0], t3, t2, t4)) Colisao();	
	    break;	  
	  case 'p':
	  	n= 5;
	  	angleIncrement = 360.0f / n;
		angleIncrement *= M_PI / 180.0f;
		glBegin(GL_TRIANGLE_FAN);
		angle = 0.0f;
		float anterior[2], primeiro[2];
		for (int k = 0; k < n; ++k) {
			if(k == 0){
				primeiro[0] = tamanho * cos(angle) + posicao[0];
				primeiro[1] = tamanho * sin(angle) + posicao[1];
			}else{
				if(k == n - 1){
					if(colidiu(posicao[0],posicao[1], tamanho * cos(angle) + posicao[0], tamanho * sin(angle) + posicao[1], primeiro[0], primeiro[1])) Colisao();
				}else{
					if(colidiu(posicao[0],posicao[1], tamanho * cos(angle) + posicao[0], tamanho * sin(angle) + posicao[1], anterior[0], anterior[1])) Colisao();	
				}
			}
			anterior[0] = tamanho * cos(angle) + posicao[0];
			anterior[1] = tamanho * sin(angle) + posicao[1];
			
		    glVertex3f(tamanho * cos(angle) + posicao[0], tamanho * sin(angle) + posicao[1], 0.0f);		    
		    angle += angleIncrement;
		}
		glEnd();
	    break;	  
	  case 'q':
	  	t1 = posicao[0]+tamanho/2;
	  	t2 = posicao[0]-tamanho/2;
	  	t3 = posicao[1]+tamanho/2;
	  	t4 = posicao[1]-tamanho/2;
	    glScalef(1.0f, 1.0f, 1.0f);
	    glBegin(GL_QUADS);
			glVertex2f(t1, t3);
			glVertex2f(t2, t3);
			glVertex2f(t2, t4)
			;glVertex2f(t1, t4);			
		glEnd();
		if(colidiu(posicao[0], posicao[1], t1, t3, t2, t3)) Colisao();
		if(colidiu(posicao[0], posicao[1], t2, t4, t2, t3)) Colisao();
		if(colidiu(posicao[0], posicao[1], t2, t4, t1, t4)) Colisao();
		if(colidiu(posicao[0], posicao[1], t1, t3, t1, t4)) Colisao();
	    break; 
	  case 'g':
	  	t1 = posicao[0]+3*tamanho/2;
	  	t2 = posicao[0]-3*tamanho/2;
	  	t3 = posicao[1]+tamanho/2;
	  	t4 = posicao[1]-tamanho/2;
	    glScalef(1.0f, 1.0f, 1.0f);
	    glBegin(GL_QUADS);
			glVertex2f(t1, t3);
			glVertex2f(t2, t3);
			glVertex2f(t2, t4)
			;glVertex2f(t1, t4);			
		glEnd();
		if(colidiu(posicao[0], posicao[1], t1, t3, t2, t3)) Colisao();
		if(colidiu(posicao[0], posicao[1], t2, t4, t2, t3)) Colisao();
		if(colidiu(posicao[0], posicao[1], t2, t4, t1, t4)) Colisao();
		if(colidiu(posicao[0], posicao[1], t1, t3, t1, t4)) Colisao();
	    break;	  
	  case 'z':
	  	n= 4;
	  	angleIncrement = 360.0f / n;
		angleIncrement *= M_PI / 180.0f;
		glBegin(GL_TRIANGLE_FAN);
		angle = 0.0f;
		for (int k = 0; k < n; ++k) {
			if(k == 0){
				primeiro[0] = tamanho * cos(angle) + posicao[0];
				primeiro[1] = tamanho * sin(angle) + posicao[1];
			}else{
				if(k == n - 1){
					if(colidiu(posicao[0],posicao[1], tamanho * cos(angle) + posicao[0], tamanho * sin(angle) + posicao[1], primeiro[0], primeiro[1])) Colisao();
				}else{
					if(colidiu(posicao[0],posicao[1], tamanho * cos(angle) + posicao[0], tamanho * sin(angle) + posicao[1], anterior[0], anterior[1])) Colisao();	
				}
			}
			anterior[0] = tamanho * cos(angle) + posicao[0];
			anterior[1] = tamanho * sin(angle) + posicao[1];
			
		    glVertex3f(tamanho * cos(angle) + posicao[0], tamanho * sin(angle) + posicao[1], 0.0f);		    
		    angle += angleIncrement;
		}
		glEnd();
	    break;	  
	  default:	  	
	  break;	    
	}
	glFlush();	
}

void Desenha(void)
{	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();

	glColor3f(0.18f, 0.72f, 0.30f);
	glBegin(GL_QUADS);
		glVertex2f(-LIMITE_PISTA_X, POSICAO_CHEGADA_Y);
		glVertex2f(LIMITE_PISTA_X, POSICAO_CHEGADA_Y);
		glVertex2f(LIMITE_PISTA_X, 25.0f);
		glVertex2f(-LIMITE_PISTA_X, 25.0f);
	glEnd();
	// Bordas impedem que os obstáculos sejam contornados fora da pista.
	glColor3f(0.28f, 0.31f, 0.35f);
	glBegin(GL_QUADS);
		glVertex2f(-win, -limiteVisivelY); glVertex2f(-LIMITE_PISTA_X, -limiteVisivelY);
		glVertex2f(-LIMITE_PISTA_X, limiteVisivelY); glVertex2f(-win, limiteVisivelY);
		glVertex2f(LIMITE_PISTA_X, -limiteVisivelY); glVertex2f(win, -limiteVisivelY);
		glVertex2f(win, limiteVisivelY); glVertex2f(LIMITE_PISTA_X, limiteVisivelY);
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
	DesenhaTexto(-3.2f, 23.5f, "CHEGADA", 0.18f);
	
	glPushMatrix();	
	glTranslatef(tx, ty, 0.0f);
	glScalef(1.0f, 1.0f, 1.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	
	glBegin(GL_QUADS);
		glVertex2f(largura/2, altura/2);
		glVertex2f(largura/2, -(altura/2));
		glVertex2f(-(largura/2), -(altura/2));
		glVertex2f(-(largura/2), altura/2);
	glEnd();
	glPopMatrix();
	
	const int quantidadeObstaculos = sizeof(obstaculos) / sizeof(obstaculos[0]);
	for(int i = 0; i < quantidadeObstaculos; i++){
	if(obstaculos[i].tipo != '\0'){
		DesenhaObstaculo(obstaculos[i].tipo, obstaculos[i].posicao, obstaculos[i].tamanho, obstaculos[i].colors);
		}		
    }

	// A colisão tem prioridade sobre a chegada no mesmo movimento.
	if (estadoJogo == JOGANDO && ty >= POSICAO_CHEGADA_Y)
		estadoJogo = VITORIA;
	
	glPopMatrix();
	DesenhaInterface();
	glFlush();
}

void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
	GLsizei largura, altura;
	if (h == 0)
		h = 1;
	largura = w;
	altura = h;

	glViewport(0, 0, largura, altura);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (largura <= altura)
	{
		gluOrtho2D(-25.0f, 25.0f, -25.0f * altura / largura, 25.0f * altura / largura);
		win = 25.0f;
		limiteVisivelY = 25.0f * altura / largura;
	}
	else
	{
		gluOrtho2D(-25.0f * largura / altura, 25.0f * largura / altura, -25.0f, 25.0f);
		win = 25.0f * largura / altura;
		limiteVisivelY = 25.0f;
	}

	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

void TeclasEspeciais(int key, int x, int y)
{
	if (estadoJogo != JOGANDO) return;

	switch (key)
	{
	case GLUT_KEY_LEFT:
		tx -= 1;
		if (tx < -LIMITE_PISTA_X + largura / 2)
			tx = -LIMITE_PISTA_X + largura / 2;
		break;
	case GLUT_KEY_RIGHT:
		tx += 1;
		if (tx > LIMITE_PISTA_X - largura / 2)
			tx = LIMITE_PISTA_X - largura / 2;
		break;
	case GLUT_KEY_DOWN:
		ty -= 1;
		if (ty < -win)
			ty = -win;
		break;
	case GLUT_KEY_UP:
		ty += 1;
		if (ty >= POSICAO_CHEGADA_Y)
			ty = POSICAO_CHEGADA_Y;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void Teclado(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'r':
	case 'R':
		ReiniciaJogo();
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}


void Inicializa(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(450, 450);
	glutCreateWindow("Atividade 3 - Desafio dos Obstaculos");
	glutDisplayFunc(Desenha);
	glutReshapeFunc(AlteraTamanhoJanela);
	glutSpecialFunc(TeclasEspeciais);
	glutKeyboardFunc(Teclado);
	Inicializa();
	glutMainLoop();
	return 0;
}
