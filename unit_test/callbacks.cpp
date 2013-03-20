#include "callbacks.h"
#include "sphere.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define USE_YGL

#ifdef USE_YGL
#include "../ygl/ygl.h"
#include "../ygl/yglu.h"
#include "../ygl/device.h"
using namespace ygl;
#else
#include <gl/GL.h>
#include <gl/glu.h>
#endif


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

void drawWireBox(float halfSize)
{
//#define _LINE_BOX

	int i=0;
#ifdef _LINE_BOX
	glBegin(GL_LINE_LOOP);
#else
	glBegin(GL_QUADS);
#endif
	//glColor3f(TRIPPLE_RANDOM);
	SET_CPN( halfSize, halfSize,-halfSize);					// Top Right Of The Quad (Top)
	SET_CPN(-halfSize, halfSize,-halfSize);					// Top Left Of The Quad (Top)
	SET_CPN(-halfSize, halfSize, halfSize);					// Bottom Left Of The Quad (Top)
	SET_CPN( halfSize, halfSize, halfSize);					// Bottom Right Of The Quad (Top)
#ifdef _LINE_BOX
	glEnd();glBegin(GL_LINE_LOOP);
#endif
	//glColor3f(TRIPPLE_RANDOM);
	SET_CPN( halfSize,-halfSize, halfSize);					// Top Right Of The Quad (Bottom)
	SET_CPN(-halfSize,-halfSize, halfSize);					// Top Left Of The Quad (Bottom)
	SET_CPN(-halfSize,-halfSize,-halfSize);					// Bottom Left Of The Quad (Bottom)
	SET_CPN( halfSize,-halfSize,-halfSize);					// Bottom Right Of The Quad (Bottom)
#ifdef _LINE_BOX
	glEnd();glBegin(GL_LINE_LOOP);
#endif
	//glColor3f(TRIPPLE_RANDOM);
	SET_CPN( halfSize, halfSize, halfSize);					// Top Right Of The Quad (Front)
	SET_CPN(-halfSize, halfSize, halfSize);					// Top Left Of The Quad (Front)
	SET_CPN(-halfSize,-halfSize, halfSize);					// Bottom Left Of The Quad (Front)
	SET_CPN( halfSize,-halfSize, halfSize);					// Bottom Right Of The Quad (Front)
#ifdef _LINE_BOX
	glEnd();glBegin(GL_LINE_LOOP);
#endif
	//glColor3f(TRIPPLE_RANDOM);
	SET_CPN( halfSize,-halfSize,-halfSize);					// Top Right Of The Quad (Back)
	SET_CPN(-halfSize,-halfSize,-halfSize);					// Top Left Of The Quad (Back)
	SET_CPN(-halfSize, halfSize,-halfSize);					// Bottom Left Of The Quad (Back)
	SET_CPN( halfSize, halfSize,-halfSize);					// Bottom Right Of The Quad (Back)
#ifdef _LINE_BOX
	glEnd();glBegin(GL_LINE_LOOP);
#endif
	//glColor3f(TRIPPLE_RANDOM);
	SET_CPN(-halfSize, halfSize, halfSize);					// Top Right Of The Quad (Left)
	SET_CPN(-halfSize, halfSize,-halfSize);					// Top Left Of The Quad (Left)
	SET_CPN(-halfSize,-halfSize,-halfSize);					// Bottom Left Of The Quad (Left)
	SET_CPN(-halfSize,-halfSize, halfSize);					// Bottom Right Of The Quad (Left)
#ifdef _LINE_BOX
	glEnd();glBegin(GL_LINE_LOOP);
#endif
	//glColor3f(TRIPPLE_RANDOM);
	SET_CPN( halfSize, halfSize,-halfSize);					// Top Right Of The Quad (Right)
	SET_CPN( halfSize, halfSize, halfSize);					// Top Left Of The Quad (Right)
	SET_CPN( halfSize,-halfSize, halfSize);					// Bottom Left Of The Quad (Right)
	SET_CPN( halfSize,-halfSize,-halfSize);	
	glEnd();
}



int width = 512;  
int height = 512;

v4* sphere_data;
#define SPHERE_SLICE 10
#define SPHERE_STACK 10

void init()
{
#ifdef USE_YGL
	device::init();
#endif

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
extern float rot_angle;
extern float translate_x,translate_y;

int frame_cnt=0;
LARGE_INTEGER tot_counter={0},pc1,pc2;
extern LARGE_INTEGER freq;

void display(void)  
{ 
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glShadeModel(GL_FLAT);
	
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
	gluPerspective(45, (double)width/(double)height, 0.1, 10000.0);  
	glMatrixMode(GL_MODELVIEW);  
	glLoadIdentity();
	gluLookAt(0.,0.,4.,0,0.,0.,0.,1.,0.);
#endif
	
	//rot_angle=0.f;
	
#ifdef TEST_ORTHO
	glTranslatef(translate_x,translate_y,0.f);
	glRotatef(rot_angle,0.f,0.f,1.f);

// 	glBegin(GL_LINES);
// 	glVertex2f(-0.25f*width,0);
// 	glVertex2f(0.25f*width,0);
// 	glEnd();
// 	glBegin(GL_TRIANGLES);
// 	glVertex2f(-0.25f*width,-0.25f*height);
// 	glVertex2f(0.25f*width,-0.25f*height);
// 	glVertex2f(0,0.25f*height);
// 	glEnd();
	glBegin(GL_QUADS);
	glColor3f(TRIPPLE_RED);
	glVertex2f(-0.25f*width,-0.25f*height);
	glColor3f(TRIPPLE_GREEN);
	glVertex2f(0.25f*width,-0.25f*height);
	glColor3f(TRIPPLE_BLUE);
	glVertex2f(0.25f*width,0.25f*height);
	glColor3f(TRIPPLE_YELLOW);
	glVertex2f(-0.25f*width,0.25f*height);
	glEnd();
	//glRectf(-0.25f*width,-0.25f*height,0.25f*width,0.25f*height);
	//glRectf(-0.5f*width,-0.5f*height,0.5f*width,0.5f*height);

#else
	glTranslatef(0.01f*translate_x,0.01f*translate_y,0.f);
	glRotatef(rot_angle,0.f,1.f,0.f);
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

	//glutSwapBuffers();  
	conflict::swapbuf();
} 
