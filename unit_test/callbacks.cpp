#include "callbacks.h"


#include <windows.h>
#include <stdio.h>

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


#define SET_PN(x,y,z)\
	glNormal3f(x,y,z);\
	glVertex3f(x,y,z);

#define TRIPPLE_RED 1.f,0.f,0.f
#define TRIPPLE_GREEN 0.f,1.f,0.f
#define TRIPPLE_BLUE 0.f,0.f,1.f
#define TRIPPLE_BLACK 0.f,0.f,0.f
#define TRIPPLE_WHITE 1.f,1.f,1.f
#define TRIPPLE_YELLOW 1.f,1.f,0.f
#define TRIPPLE_GRAY 0.5f,0.5f,0.5f

void drawWireBox(float halfSize)
{
	glBegin(GL_QUADS);
	glColor3f(TRIPPLE_RED);
	SET_PN( halfSize, halfSize,-halfSize);					// Top Right Of The Quad (Top)
	SET_PN(-halfSize, halfSize,-halfSize);					// Top Left Of The Quad (Top)
	SET_PN(-halfSize, halfSize, halfSize);					// Bottom Left Of The Quad (Top)
	SET_PN( halfSize, halfSize, halfSize);					// Bottom Right Of The Quad (Top)

	glColor3f(TRIPPLE_GREEN);
	SET_PN( halfSize,-halfSize, halfSize);					// Top Right Of The Quad (Bottom)
	SET_PN(-halfSize,-halfSize, halfSize);					// Top Left Of The Quad (Bottom)
	SET_PN(-halfSize,-halfSize,-halfSize);					// Bottom Left Of The Quad (Bottom)
	SET_PN( halfSize,-halfSize,-halfSize);					// Bottom Right Of The Quad (Bottom)

	glColor3f(TRIPPLE_BLUE);
	SET_PN( halfSize, halfSize, halfSize);					// Top Right Of The Quad (Front)
	SET_PN(-halfSize, halfSize, halfSize);					// Top Left Of The Quad (Front)
	SET_PN(-halfSize,-halfSize, halfSize);					// Bottom Left Of The Quad (Front)
	SET_PN( halfSize,-halfSize, halfSize);					// Bottom Right Of The Quad (Front)

	glColor3f(TRIPPLE_WHITE);
	SET_PN( halfSize,-halfSize,-halfSize);					// Top Right Of The Quad (Back)
	SET_PN(-halfSize,-halfSize,-halfSize);					// Top Left Of The Quad (Back)
	SET_PN(-halfSize, halfSize,-halfSize);					// Bottom Left Of The Quad (Back)
	SET_PN( halfSize, halfSize,-halfSize);					// Bottom Right Of The Quad (Back)

	glColor3f(TRIPPLE_YELLOW);
	SET_PN(-halfSize, halfSize, halfSize);					// Top Right Of The Quad (Left)
	SET_PN(-halfSize, halfSize,-halfSize);					// Top Left Of The Quad (Left)
	SET_PN(-halfSize,-halfSize,-halfSize);					// Bottom Left Of The Quad (Left)
	SET_PN(-halfSize,-halfSize, halfSize);					// Bottom Right Of The Quad (Left)

	glColor3f(TRIPPLE_GRAY);
	SET_PN( halfSize, halfSize,-halfSize);					// Top Right Of The Quad (Right)
	SET_PN( halfSize, halfSize, halfSize);					// Top Left Of The Quad (Right)
	SET_PN( halfSize,-halfSize, halfSize);					// Bottom Left Of The Quad (Right)
	SET_PN( halfSize,-halfSize,-halfSize);	
	glEnd();
}



int width = 512;  
int height = 512;

void init()
{
	device::init();
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
	//glEnable(GL_CULL_FACE);
	
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
	gluPerspective(90, (double)width/(double)height, 0.1, 10000.0);  
	glMatrixMode(GL_MODELVIEW);  
	glLoadIdentity();
	gluLookAt(0.,0.,4.,0,0.,0.,0.,1.,0.);
#endif
	
	

//rot_angle=90.f;
	glTranslatef(translate_x,translate_y,0.f);
	glRotatef(rot_angle,0.f,1.f,0.f);
// 	glBegin(GL_LINES);
// 	glVertex2f(-0.25f*width,0);
// 	glVertex2f(0.25f*width,0);
// 	glEnd();
// 	glBegin(GL_TRIANGLES);
// 	glVertex2f(-0.25f*width,-0.25f*height);
// 	glVertex2f(0.25f*width,-0.25f*height);
// 	glVertex2f(0,0.25f*height);
// 	glEnd();
//	glBegin(GL_QUADS);
// 	glVertex2f(-0.25f*width,-0.25f*height);
// 	glVertex2f(0.25f*width,-0.25f*height);
// 	glVertex2f(0.25f*width,0.25f*height);
// 	glVertex2f(-0.25f*width,0.25f*height);
// 	glVertex2f(-0.5f*width,-0.5f*height);
// 	glVertex2f(0.5f*width,-0.5f*height);
// 	glVertex2f(0.5f*width,0.5f*height);
// 	glVertex2f(-0.5f*width,0.5f*height);
// 	glEnd();
	//glRectf(-0.25f*width,-0.25f*height,0.25f*width,0.25f*height);
	//glRectf(-0.5f*width,-0.5f*height,0.5f*width,0.5f*height);
	drawWireBox(1.f);

#ifdef USE_YGL
	QueryPerformanceCounter(&pc2);
	glFlush();
#else
	glFlush();
	QueryPerformanceCounter(&pc2);
#endif

	//glFinish
	
	tot_counter.QuadPart+=pc2.QuadPart-pc1.QuadPart;

	if(++frame_cnt==30)
	{
		frame_cnt=0;
		printf("%lf ms\n",1000.0*((double)tot_counter.QuadPart/(double)freq.QuadPart));
		tot_counter.QuadPart=0;
	}

	//glutSwapBuffers();  
	conflict::swapbuf();
} 
