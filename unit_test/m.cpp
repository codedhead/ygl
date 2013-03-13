
#include "callbacks.h"

#include <stdio.h>
//#include <dvec.h>
#include <gl/glut.h>

namespace conflict{
	void swapbuf()
	{
		glutSwapBuffers();
	}
}

float rot_angle=0.f;

int old_x, old_y;  
int btn;
void mouse(int button, int state, int x, int y)  
{  
	old_x = x;  
	old_y = y;
	btn = button; 
} 
void motion(int x, int y)
{
// 	if(btn==GLUT_LEFT_BUTTON)
// 	{
// 		rot_angle+=x-old_x;
// 		glutPostRedisplay();
// 	}
	old_x=x;old_y=y;
}
void keyboard(unsigned char key, int x, int y)  
{  
	switch (key)
	{
	case 'q':
	case 'Q':
		exit(0);
	}  

	//glutPostRedisplay();  
} 

void idle()
{
	rot_angle+=3.f;
	glutPostRedisplay();
}

LARGE_INTEGER freq;

int main(int argc, char** argv)  
{
	//glEnable(GL_DEPTH_TEST);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);  
	glutInitWindowPosition(50, 50);  
	glutInitWindowSize(400,400);  
	glutInit(&argc, argv);  
	glutCreateWindow("ygl");  

	glutReshapeFunc(reshape);  
	glutDisplayFunc(display);  
	glutMotionFunc(motion);
	glutMouseFunc(mouse);  
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	QueryPerformanceFrequency(&freq);
	printf("freq: %lld\n",freq.QuadPart);


// 	glEnable(GL_DEPTH_TEST);  
// 	glDisable(GL_CULL_FACE);

	glutMainLoop();
	return 0;  
}  