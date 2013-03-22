
#include "sphere.h"

#define USE_YGL

#ifdef USE_YGL
#include "../ygl/ygl.h"
#include "../ygl/yglu.h"
#include "../ygl/device.h"
using namespace ygl;
#endif

#include <stdio.h>
//#include <dvec.h>
#include <gl/glut.h>


float rot_angle=0.f;
float rot_x=0.f,rot_y=0.f;
float translate_x=0.f,translate_y=0.f;

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
	if(btn==GLUT_MIDDLE_BUTTON)
	{
		//rot_angle+=x-old_x;
		translate_x+=/*0.01f**/(old_x-x);
		translate_y+=/*0.01f**/(y-old_y);
		glutPostRedisplay();
	}
	else if(btn==GLUT_LEFT_BUTTON)
	{
		//rot_angle+=x-old_x;
		rot_x+=/*0.01f**/(old_x-x);
		rot_y+=/*0.01f**/(old_y-y);
		glutPostRedisplay();
	}
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
	rot_angle+=0.8f;
	glutPostRedisplay();
}

LARGE_INTEGER freq;


#define SET_CPN(x,y,z)\
	glColor3fv(boxv_color[i++]);\
	glNormal3f(x,y,z);\
	glVertex3f(x,y,z);


#define TRIPPLE_RED 1.f,0.f,0.f
#define TRIPPLE_GREEN 0.f,1.f,0.f
#define TRIPPLE_BLUE 0.f,0.f,1.f
#define TRIPPLE_BLACK 0.f,0.f,0.f
#define TRIPPLE_WHITE 1.f,1.f,1.f
#define TRIPPLE_YELLOW 1.f,1.f,0.f
#define TRIPPLE_GRAY 0.5f,0.5f,0.5f

#define RANDOM_01 ((float)rand()/(float)RAND_MAX)
#define TRIPPLE_RANDOM RANDOM_01,RANDOM_01,RANDOM_01

float boxv_color[][3]={
	{TRIPPLE_RANDOM},
	{TRIPPLE_RANDOM},
	{TRIPPLE_RANDOM},
	{TRIPPLE_RANDOM},

	{TRIPPLE_RANDOM},
	{TRIPPLE_RANDOM},
	{TRIPPLE_RANDOM},
	{TRIPPLE_RANDOM},

	{TRIPPLE_RANDOM},
	{TRIPPLE_RANDOM},
	{TRIPPLE_RANDOM},
	{TRIPPLE_RANDOM},

	{TRIPPLE_RANDOM},
	{TRIPPLE_RANDOM},
	{TRIPPLE_RANDOM},
	{TRIPPLE_RANDOM},

	{TRIPPLE_RANDOM},
	{TRIPPLE_RANDOM},
	{TRIPPLE_RANDOM},
	{TRIPPLE_RANDOM},

	{TRIPPLE_RANDOM},
	{TRIPPLE_RANDOM},
	{TRIPPLE_RANDOM},
	{TRIPPLE_RANDOM},
};

#define CHECKBOARD_REPEAT 8

void drawWireBox(float halfSize)
{
#define DRAW_TYPE GL_QUADS
//#define DRAW_TYPE GL_LINE_LOOP

	//glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);

#define _LEFT_FACE
#define _RIGHT_FACE
#define _TOP_FACE
#define _BOTTOM_FACE
#define _FRONT_FACE
#define _BACK_FACE

	int i=0;
	glEnable(GL_TEXTURE_2D);

#ifdef _FRONT_FACE
	glBegin(DRAW_TYPE);
	glTexCoord2f(1.f*CHECKBOARD_REPEAT,1.f*CHECKBOARD_REPEAT);
	SET_CPN( halfSize, halfSize, halfSize);					// Top Right Of The Quad (Front)
	glTexCoord2f(0.f,1.f*CHECKBOARD_REPEAT);
	SET_CPN(-halfSize, halfSize, halfSize);					// Top Left Of The Quad (Front)
	glTexCoord2f(0.f,0.f);
	SET_CPN(-halfSize,-halfSize, halfSize);					// Bottom Left Of The Quad (Front)
	glTexCoord2f(1.f*CHECKBOARD_REPEAT,0.f);
	SET_CPN( halfSize,-halfSize, halfSize);					// Bottom Right Of The Quad (Front)
	glEnd();
#endif
// #ifdef _FRONT_FACE
// 	glBegin(DRAW_TYPE);
// 	glTexCoord2f(0.f,0.f);
// 	SET_CPN( halfSize, halfSize, halfSize);					// Top Right Of The Quad (Front)
// 	glTexCoord2f(1.f*CHECKBOARD_REPEAT,0.f);
// 	SET_CPN(-halfSize, halfSize, halfSize);					// Top Left Of The Quad (Front)
// 	glTexCoord2f(1.f*CHECKBOARD_REPEAT,1.f*CHECKBOARD_REPEAT);
// 	SET_CPN(-halfSize,-halfSize, halfSize);					// Bottom Left Of The Quad (Front)
// 	glTexCoord2f(0.f,1.f*CHECKBOARD_REPEAT);
// 	SET_CPN( halfSize,-halfSize, halfSize);					// Bottom Right Of The Quad (Front)
// 	glEnd();
// #endif

	glDisable(GL_TEXTURE_2D);

#ifdef _TOP_FACE
	glBegin(DRAW_TYPE);
	SET_CPN( halfSize, halfSize,-halfSize);					// Top Right Of The Quad (Top)
	SET_CPN(-halfSize, halfSize,-halfSize);					// Top Left Of The Quad (Top)
	SET_CPN(-halfSize, halfSize, halfSize);					// Bottom Left Of The Quad (Top)
	SET_CPN( halfSize, halfSize, halfSize);					// Bottom Right Of The Quad (Top)
	glEnd();
#endif

	
#ifdef _BOTTOM_FACE
	glBegin(DRAW_TYPE);
	SET_CPN( halfSize,-halfSize, halfSize);					// Top Right Of The Quad (Bottom)
	SET_CPN(-halfSize,-halfSize, halfSize);					// Top Left Of The Quad (Bottom)
	SET_CPN(-halfSize,-halfSize,-halfSize);					// Bottom Left Of The Quad (Bottom)
	SET_CPN( halfSize,-halfSize,-halfSize);					// Bottom Right Of The Quad (Bottom)
	glEnd();
#endif


#ifdef _BACK_FACE
	glBegin(DRAW_TYPE);
	SET_CPN( halfSize,-halfSize,-halfSize);					// Top Right Of The Quad (Back)
	SET_CPN(-halfSize,-halfSize,-halfSize);					// Top Left Of The Quad (Back)
	SET_CPN(-halfSize, halfSize,-halfSize);					// Bottom Left Of The Quad (Back)
	SET_CPN( halfSize, halfSize,-halfSize);					// Bottom Right Of The Quad (Back)
	glEnd();
#endif

#ifdef _LEFT_FACE
	glBegin(DRAW_TYPE);
	SET_CPN(-halfSize, halfSize, halfSize);					// Top Right Of The Quad (Left)
	SET_CPN(-halfSize, halfSize,-halfSize);					// Top Left Of The Quad (Left)
	SET_CPN(-halfSize,-halfSize,-halfSize);					// Bottom Left Of The Quad (Left)
	SET_CPN(-halfSize,-halfSize, halfSize);					// Bottom Right Of The Quad (Left)
	glEnd();
#endif

#ifdef _RIGHT_FACE
	glBegin(DRAW_TYPE);
	SET_CPN( halfSize, halfSize,-halfSize);					// Top Right Of The Quad (Right)
	SET_CPN( halfSize, halfSize, halfSize);					// Top Left Of The Quad (Right)
	SET_CPN( halfSize,-halfSize, halfSize);					// Bottom Left Of The Quad (Right)
	SET_CPN( halfSize,-halfSize,-halfSize);	
	glEnd();
#endif
	
}



int width = 512;  
int height = 512;

v4* sphere_data;
#define SPHERE_SLICE 10
#define SPHERE_STACK 10

// texture pixels must always exist
GLfloat check_pixels[]={
	TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,		TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,
	TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,		TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,
	TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,		TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,
	TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,		TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,
	TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,		TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,
	TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,		TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,
	TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,		TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,
	TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,		TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,

	TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,		TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,
	TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,		TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,
	TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,		TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,
	TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,		TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,
	TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,		TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,
	TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,		TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,
	TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,		TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,
	TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,TRIPPLE_WHITE,		TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,TRIPPLE_BLACK,
};
GLuint check_tex_name;

void init()
{
#ifdef USE_YGL
	device::init();
#endif

	GLfloat light_ambient[] =
	{0.0, 0.0, 0.0, 1.0};
	GLfloat light_diffuse[] =
	{1.0, 0.0, 0.0, 1.0};
	GLfloat light_specular[] =
	{1.0, 1.0, 1.0, 1.0};
	/* light_position is NOT default value */
	GLfloat light_position[] =
	{1.0, 1.0, 1.0, 0.0};

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_NORMALIZE);
	//glEnable(GL_RESC)
	//glEnable(GL_LIGHTING);
 	glEnable(GL_LIGHT0);	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glShadeModel(GL_SMOOTH);	GLfloat tex_env_col[4]={0.f,1.f,0.f,1.f};
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,tex_env_col);

 	glGenTextures(1,&check_tex_name);
	glBindTexture(GL_TEXTURE_2D,check_tex_name);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

	// border color
	GLfloat tex_border_col[4]={1.f,0.f,0.f,1.f};
	glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,tex_border_col);

	// border 0??
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,16,16,0,
		GL_RGB,GL_FLOAT,check_pixels);
 
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_FASTEST);

	sphere_data=glxGenSphere(1.0,SPHERE_SLICE,SPHERE_STACK);
}
void quit()
{
	if(sphere_data) glxFreeSphere(sphere_data);
}
void reshape(int w, int h)  
{
	//device::resize(w,h);
	//glClearColor(0.0, 0.0, 0.0, 0.0); 
	width=w;height=h;
}

int frame_cnt=0;
LARGE_INTEGER tot_counter={0},pc1,pc2;

void display(void)  
{

	QueryPerformanceCounter(&pc1);
#define VIEWPORT_BORDER 16
	glViewport(VIEWPORT_BORDER, VIEWPORT_BORDER, width-2*VIEWPORT_BORDER, height-2*VIEWPORT_BORDER);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//#define TEST_ORTHO
#ifdef TEST_ORTHO
	gluOrtho2D(-width/2,width/2,-height/2,height/2);
	glMatrixMode(GL_MODELVIEW);  
	glLoadIdentity();
#else
	gluPerspective(60, (double)width/(double)height, 0.1, 100.0);  
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.,0.,4.,0,0.,0.,0.,1.,0.);
#endif

	//rot_angle=0.f;

#ifdef TEST_ORTHO
	glTranslatef(translate_x,translate_y,0.f);
	//glRotatef(rot_angle,0.f,0.f,1.f);

	// 	glBegin(GL_LINES);
	// 	glVertex2f(-0.25f*width,0);
	// 	glVertex2f(0.25f*width,0);
	// 	glEnd();
	// 	glBegin(GL_TRIANGLES);
	// 	glVertex2f(-0.25f*width,-0.25f*height);
	// 	glVertex2f(0.25f*width,-0.25f*height);
	// 	glVertex2f(0,0.25f*height);
	// 	glEnd();
// 	glBegin(GL_QUADS);
// 	glColor3f(TRIPPLE_RED);
// 	glVertex2f(-0.25f*width,-0.25f*height);
// 	glColor3f(TRIPPLE_GREEN);
// 	glVertex2f(0.25f*width,-0.25f*height);
// 	glColor3f(TRIPPLE_BLUE);
// 	glVertex2f(0.25f*width,0.25f*height);
// 	glColor3f(TRIPPLE_YELLOW);
// 	glVertex2f(-0.25f*width,0.25f*height);
// 	glEnd();
	//glRectf(-0.25f*width,-0.25f*height,0.25f*width,0.25f*height);
	//glRectf(-0.5f*width,-0.5f*height,0.5f*width,0.5f*height);
	glColor3f(TRIPPLE_RED);
	//glRectf(-0.5f*width,-0.5f*height,0,0.5f*height);
	//glColor3f(TRIPPLE_BLUE);
	//glRectf(0,-0.5f*height,0.5f*width,0.5f*height);

	glRectf(-0.5f*width,-0.5f*height,0.5f*width,0.5f*height);


#else
	glTranslatef(0.01f*translate_x,0.01f*translate_y,0.f);
	glRotatef(rot_angle,0.f,1.f,0.f);
	//glRotatef(rot_y,1.f,0.f,0.f);
	//glRotatef(rot_x,0.f,1.f,0.f);
	drawWireBox(1.f);
	//glxWireSphere(sphere_data,SPHERE_SLICE,SPHERE_STACK);
#endif

#ifdef USE_YGL
	QueryPerformanceCounter(&pc2);
	glFlush();
#else
	glFlush();//glFinish
	QueryPerformanceCounter(&pc2);
#endif


	tot_counter.QuadPart+=pc2.QuadPart-pc1.QuadPart;

#define TEST_FRAMES_CNT 30
	if(++frame_cnt==TEST_FRAMES_CNT)
	{
		frame_cnt=0;
		double s_of_framecnt=((double)tot_counter.QuadPart/(double)freq.QuadPart);
		printf("%lf ms, fps: %lf\n",1000.0*s_of_framecnt,
			TEST_FRAMES_CNT/s_of_framecnt);
		tot_counter.QuadPart=0;
	}

	glutSwapBuffers(); 
} 

int main(int argc, char** argv)  
{
	//glEnable(GL_DEPTH_TEST);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);  
	glutInitWindowPosition(50, 50);  
	glutInitWindowSize(512,512);  
	glutInit(&argc, argv);  
	glutCreateWindow("ygl");  

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);  
	glutMotionFunc(motion);
	glutMouseFunc(mouse);  
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	init();

	QueryPerformanceFrequency(&freq);
	printf("freq: %lld\n",freq.QuadPart);



// 	glEnable(GL_DEPTH_TEST); 

	glutMainLoop();
	quit();
	return 0;  
}  