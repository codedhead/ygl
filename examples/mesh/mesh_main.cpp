


#define USE_YGL
#ifdef USE_YGL
#include "../..//ygl/ygl.h"
#include "../../ygl/yglu.h"
using namespace ygl;
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gl/glut.h>

#include "trimesh.h"


#define INIT_WIN_WIDTH 512
#define INIT_WIN_HEIGHT 512


LARGE_INTEGER freq;

#define CHECKBOARD_REPEAT 8


int width = INIT_WIN_WIDTH;  
int height = INIT_WIN_HEIGHT;

#define TRIPPLE_RED 1.f,0.f,0.f
#define TRIPPLE_GREEN 0.f,1.f,0.f
#define TRIPPLE_BLUE 0.f,0.f,1.f
#define TRIPPLE_BLACK 0.f,0.f,0.f
#define TRIPPLE_WHITE 1.f,1.f,1.f
#define TRIPPLE_YELLOW 1.f,1.f,0.f
#define TRIPPLE_GRAY 0.5f,0.5f,0.5f

// texture pixels must always exist
// GLfloat check_pixels[]={
// 	TRIPPLE_BLACK,		TRIPPLE_WHITE,
// 
// 	TRIPPLE_WHITE,		TRIPPLE_BLACK,
// };
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

#define MODEL_CNT 6
char model_paths[MODEL_CNT][256]={
	"./off/sphere.off",
	"./off/chess.off",
	"./off/bunny.off",
	"./off/shuttle.off",
	"./off/killeroo.off",
	"./off/head.off",
};
int mesh_index=0;
int mesh_cnt=1;

TriMesh* meshs[MODEL_CNT+1]={0};

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

void draw_box(float halfSize)
{
#define DRAW_TYPE GL_POLYGON
//#define DRAW_TYPE GL_LINE_LOOP

	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

#define _TOP_FACE
#define _LEFT_FACE
#define _RIGHT_FACE
#define _BACK_FACE
#define _FRONT_FACE
#define _BOTTOM_FACE

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

float fogColor[4] = {0.6f, 0.6f, 0.6f, 1.0f};
void init()
{
	QueryPerformanceFrequency(&freq);

	GLfloat light_ambient0[] =
	{0.0, 0.0, 0.0, 1.0};
	GLfloat light_diffuse0[] =
	{1.0, 0.0, 0.0, 1.0};
	GLfloat light_specular0[] =
	{1.0, 1.0, 1.0, 1.0};
	/* light_position is NOT default value */
	GLfloat light_position0[] =
	{1.0, 1.0, 1.0, 0.0};

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

	GLfloat light_ambient1[] =
	{0.0, 0.0, 0.0, 1.0};
	GLfloat light_diffuse1[] =
	{1.0, 1.0, 1.0, 1.0};
	GLfloat light_specular1[] =
	{1.0, 1.0, 1.0, 1.0};
	/* light_position is NOT default value */
	GLfloat light_position1[] =
	{-1.0, 1.0, 1.0, 0.0};

	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glEnable(GL_NORMALIZE);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);	glEnable(GL_LIGHT1);	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_FOG);

	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glShadeModel(GL_SMOOTH);
	glFogi (GL_FOG_MODE, GL_LINEAR);
	glFogfv (GL_FOG_COLOR, fogColor);
	glFogf (GL_FOG_DENSITY,1.f);	GLfloat tex_env_col[4]={0.f,1.f,0.f,1.f};
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,tex_env_col);

	glGenTextures(1,&check_tex_name);
	glBindTexture(GL_TEXTURE_2D,check_tex_name);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

	// border color
	GLfloat tex_border_col[4]={1.f,0.f,0.f,1.f};
	glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,tex_border_col);

	// no alignment
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	// border 0??
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,16,16,0,
		GL_RGB,GL_FLOAT,check_pixels);
	glPixelStorei(GL_UNPACK_ALIGNMENT,4);

	//glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_FASTEST);

	TriMesh** mesh=meshs+1;
	mesh_cnt=1;
	for(int i=0;i<MODEL_CNT;++i)
	{
		if(*mesh=MeshLoader::readOffMesh(model_paths[i]))
		{
			++mesh_cnt;++mesh;
		}
	}
}

void reshape(int w, int h)  
{
	width=w;height=h;

#define VIEWPORT_BORDER 16
	glViewport(VIEWPORT_BORDER, VIEWPORT_BORDER, width-2*VIEWPORT_BORDER, height-2*VIEWPORT_BORDER);
}

float rot_angle=0.f;
float rot_x=0.f,rot_y=0.f;
float translate_x=0.f,translate_y=0.f,translate_z=0.f;

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
		translate_x+=(old_x-x);
		translate_y+=(y-old_y);

	}
	else if(btn==GLUT_LEFT_BUTTON)
	{
		rot_x+=(y-old_y);
		rot_y+=(x-old_x);
	}
	else if(btn==GLUT_RIGHT_BUTTON)
	{
		translate_z+=(old_y-y);
	}
	old_x=x;old_y=y;

	glutPostRedisplay();
}

bool option_smooth=true,option_persp=true,option_light0=true,option_light1=true,option_lighting=true,
	option_tex_replace=true,option_fog=false,option_animation=true,option_fil_linear=true;

void keyboard(unsigned char key, int x, int y)  
{  
	switch (key)
	{
	case 'a':
	case 'A':
		option_animation=!option_animation;
		break;
	case 'm':
	case 'M':
		if(option_fil_linear=!option_fil_linear)
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		}
		break;
	case 'f':
	case 'F':
		if(option_fog=!option_fog)
		{
			glClearColor(fogColor[0],fogColor[1],fogColor[2],fogColor[3]);
			glEnable(GL_FOG);
		}
		else
		{
			glClearColor(0.f,0.f,0.f,0.f);
			glDisable(GL_FOG);
		}
		break;
	case 't':
	case 'T':
		if(option_tex_replace=!option_tex_replace)
			glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
		else
			glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
		break;
	case 'l':
	case 'L':
		if(option_lighting=!option_lighting)
			glEnable(GL_LIGHTING);
		else
			glDisable(GL_LIGHTING);
		break;
	case '1':
		if(option_light0=!option_light0)
			glEnable(GL_LIGHT0);
		else
			glDisable(GL_LIGHT0);
		break;
	case '2':
		if(option_light1=!option_light1)
			glEnable(GL_LIGHT1);
		else
			glDisable(GL_LIGHT1);
		break;
	case 's':
	case 'S':
		if(option_smooth=!option_smooth)
			glShadeModel(GL_SMOOTH);
		else
			glShadeModel(GL_FLAT);
		break;
	case 'p':
	case 'P':
		if(option_persp=!option_persp)
			glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
		else
			glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_FASTEST);
		break;
	case 'n':
	case 'N':
		{
			int i=(mesh_index+1)%mesh_cnt;
			if(i==mesh_index) break;
			mesh_index=i;
			if(mesh_index>0&&meshs[mesh_index])
				printf("mesh face number: %d, vertex number: %d\n",meshs[mesh_index]->ntris,meshs[mesh_index]->nverts);

			rot_angle=rot_x=rot_y=translate_x=translate_y=translate_z=0.f;
		}
		break;
	case 'b':
	case 'B':
		{
			int i=(mesh_index+mesh_cnt-1)%mesh_cnt;
			if(i==mesh_index) break;
			mesh_index=i;
			if(mesh_index>0&&meshs[mesh_index])
				printf("mesh face number: %d, vertex number: %d\n",meshs[mesh_index]->ntris,meshs[mesh_index]->nverts);
			rot_angle=rot_x=rot_y=translate_x=translate_y=translate_z=0.f;
		}
		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	}

	glutPostRedisplay();
} 

void idle()
{
	if(option_animation) rot_angle+=0.8f;
	glutPostRedisplay();
}

int frame_cnt=0;
LARGE_INTEGER tot_counter={0},pc1,pc2;


void display(void)  
{
	//printf("display begin\n");
	QueryPerformanceCounter(&pc1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60, (double)width/(double)height, 0.1, 100.0);  
	

	if(mesh_index==0) //box
	{
		glFogf (GL_FOG_START, 3.f);
		glFogf (GL_FOG_END,10.f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0.,0.,4.,0,0.,0.,0.,1.,0.);

		glTranslatef(0.01f*translate_x,0.01f*translate_y,0.01f*translate_z);
		glRotatef(rot_angle,0.f,1.f,0.f);
		glRotatef(rot_x,1.f,0.f,0.f);
		glRotatef(rot_y,0.f,1.f,0.f);

		draw_box(1.f);
	}
	else if(meshs[mesh_index])
	{
		TriMesh* mesh=meshs[mesh_index];

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		Point c;
		float r;
		mesh->objbound.boundingSphere(&c,&r);
		

		gluLookAt(0.,0.,2.1*r,0,0.,0.,0.,1.,0.);

		glFogf (GL_FOG_START, 2.1*r);
		glFogf (GL_FOG_END,5.1*r);

//		bug
// 		rot_x=6;
// 		rot_y=1;
// 		translate_x=0;
// 		translate_y=0;
// 		translate_z=2.;
		r*=0.025f;
		glTranslatef(r*translate_x,r*translate_y,r*translate_z);
		glRotatef(rot_angle,0.f,1.f,0.f);
		glRotatef(rot_x,1.f,0.f,0.f);
		glRotatef(rot_y,0.f,1.f,0.f);

		glTranslatef(-c.x,-c.y,-c.z);


		glColor3f(1.f,0.f,0.f);
		
		// todo: bake mesh and use gl***Pointer

		glBegin(GL_TRIANGLES);
		int* pvi=mesh->vi,*pni=(mesh->ni?mesh->ni:mesh->vi);
		for(int i=0;i<mesh->ntris;++i)
		{
			glNormal3fv(&(mesh->n[pni[i*3]].x));
			glVertex3fv(&(mesh->p[pvi[i*3]].x));

			glNormal3fv(&(mesh->n[pni[i*3+1]].x));
			glVertex3fv(&(mesh->p[pvi[i*3+1]].x));

			glNormal3fv(&(mesh->n[pni[i*3+2]].x));
			glVertex3fv(&(mesh->p[pvi[i*3+2]].x));
		}
		glEnd();
	}
	
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
		printf("render %d frames in %lf ms, fps: %lf\n",TEST_FRAMES_CNT,1000.0*s_of_framecnt,
			TEST_FRAMES_CNT/s_of_framecnt);
		tot_counter.QuadPart=0;
	}

	glutSwapBuffers(); 
} 

int main(int argc, char** argv)  
{
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);  
	glutInitWindowPosition(50, 50);  
	glutInitWindowSize(INIT_WIN_WIDTH,INIT_WIN_HEIGHT);  
	glutInit(&argc, argv);  
	glutCreateWindow("ygl");  

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);  
	glutMotionFunc(motion);
	glutMouseFunc(mouse);  
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	init();

	glutMainLoop();
	return 0;  
}  