#include <stdio.h>
#include <stdlib.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include<math.h>

GLfloat tx = 0, ty = 0, ang1 = 0, ang2 = -90, ang3 = 45, win = 25;
float PI=3.1416;

void drawFilledCircle(GLfloat x, GLfloat y, GLfloat radius){
	int i;
	int triangleAmount = 20;
	
	GLfloat twicePi = 2.0f * PI;
	
	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x, y);
		for(i = 0; i <= triangleAmount;i++) { 
			glVertex2f(
		            x + (radius * cos(i *  twicePi / triangleAmount)), 
			    y + (radius * sin(i * twicePi / triangleAmount))
			);
		}
	glEnd();
}

void Desenha(void)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix(); 
	
	// Base do objeto 
	glTranslatef(tx,ty,0.0f);
	glPushMatrix();
	glScalef(2.5f,2.5f,1.0f); 
	glColor3f(1.0f,1.0f,0.0f); 		
	glBegin(GL_QUADS);
		glVertex2f(2.5,0.8);
		glVertex2f(2.5,-0.8);
		glVertex2f(-2.5,-0.8);
		glVertex2f(-2.5,0.8);
	glEnd();    
	glPopMatrix();
          
	// Primeiro Braço          
	glTranslatef(0.0f, 2.5f,0.0f);
	glRotatef(ang1,0.0f,0.0f,1.0f);    
	glScalef(1.4f,1.4f,1.0f);	
	glColor3f(0.0f,1.0f,0.0f);
	glBegin(GL_QUADS);
		glVertex2f(0.6,6.0);
		glVertex2f(0.6,-0.6);
		glVertex2f(-0.6,-0.6);
		glVertex2f(-0.6,6.0);
	glEnd();
	
	//Segundo Braço                                    
	glTranslatef(0.0f,6.0f,0.0f);
	glRotatef(ang2,0.0f,0.0f,1.0f);
	glColor3f(0.0f,0.0f,1.0f);
	glBegin(GL_QUADS);
		glVertex2f(0.6,6);
		glVertex2f(0.6,-0.6);
		glVertex2f(-0.6,-0.6);
		glVertex2f(-0.6,6);
	glEnd();
	
	//Terceiro Braço                                    
	glTranslatef(0.0f,6.0f,0.0f);
	glRotatef(ang3,0.0f,0.0f,1.0f);
	glColor3f(1.0f,0.0f,0.0f);
	glBegin(GL_QUADS);
		glVertex2f(0.6,6);
		glVertex2f(0.6,-0.6);
		glVertex2f(-0.6,-0.6);
		glVertex2f(-0.6,6);
	glEnd();
                    
	glPopMatrix();    
      
	glFlush();
}

void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
	GLsizei largura, altura;
                   
	if(h == 0) h = 1;

	largura = w;
	altura = h;
                                              
	glViewport(0, 0, largura, altura);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (largura <= altura) 
	{ 
		gluOrtho2D (-25.0f, 25.0f, -25.0f*altura/largura, 25.0f*altura/largura);
		win = 25.0f;
	}              
	else 
	{ 
		gluOrtho2D (-25.0f*largura/altura, 25.0f*largura/altura, -25.0f, 25.0f);
		win = 25.0f*largura/altura;           
	}

	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

void TeclasEspeciais(int key, int x, int y)
{
	switch(key) {
		case GLUT_KEY_LEFT:
				tx-=2;
				if ( tx < -win ) tx = -win;
			break; 
		case GLUT_KEY_RIGHT:
			tx+=2;
			if ( tx > win ) tx = win;
			break; 
		case GLUT_KEY_DOWN:
			ty-=2;
			if ( ty < -win ) ty = -win;
			break; 
		case GLUT_KEY_UP:
			ty+=2;
			if ( ty > win ) ty = win;
			break; 	
		default :
			break;
	}                                                   
	glutPostRedisplay();
}

void Teclado (unsigned char key, int x, int y)
{
	switch(key) {	
		case 'q':
			ang1+=5;
			break; 
		case 'w':
			ang1-=5;
			break; 
		case 'a':
			ang2+=5;
			break; 
		case 's':
			ang2-=5;
			break; 
		case 'z':
			ang3+=5;
			break; 
		case 'x':
			ang3-=5;
			break; 
		case 27:
			exit(0);
			break;
		default :
			break;
	}
	glutPostRedisplay();
}
           
void Inicializa (void)
{   
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	
	printf(" Setas para esquerda, direita, cima e baixa movimentam a base (Amarela)");
	printf("\n Q e W rotacionam o braço 1 (verde)");
	printf("\n A e S rotacionam o braço 2 (azul)"); 
	printf("\n Z e X rotacionam o braço 3 (vermelho)");    
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);  
	glutInitWindowPosition(0,0);     
	glutInitWindowSize(800,600);  
	glutCreateWindow("Atividade 2 - Hierarquia e transformacoes geometricas");
 
	glutDisplayFunc(Desenha);  
	glutReshapeFunc(AlteraTamanhoJanela);
	glutSpecialFunc(TeclasEspeciais);
	glutKeyboardFunc (Teclado);
	Inicializa(); 
	glutMainLoop();

	return 0;
}
