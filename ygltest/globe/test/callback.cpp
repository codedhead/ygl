#include "../ygl/ygl.h"
#include "sphere.h"
#include "box.h"
#include "camera.h"
#include <stdio.h>
#include <windows.h>

Camera cam;
int gHeight,gWidth;
int prevx,prevy;

GLfloat rtri;
GLfloat rquad;

GLfloat xrot;
GLfloat yrot;
GLfloat zrot;
static GLuint texName[2];

long ImageWidth=0;
long ImageHeight=0; 
long linewidth;
byte* Image=NULL;

#define SPHERE_STACK 30
#define SPHERE_SLICE 30

void ReadHeader(FILE *fp , BITMAPFILEHEADER * p_bitmapheader , BITMAPINFOHEADER *p_bitmapinfo) 
{ 
	fseek(fp, 0, SEEK_SET) ; fread( &p_bitmapheader->bfType,sizeof(unsigned short), 1, fp );
	fseek(fp, 2, SEEK_SET) ; fread( &p_bitmapheader->bfSize,sizeof(unsigned long), 1, fp );
	fseek(fp, 6, SEEK_SET) ; fread( &p_bitmapheader->bfReserved1,sizeof(unsigned short), 1, fp );
	fseek(fp, 8, SEEK_SET) ; fread( &p_bitmapheader->bfReserved2,sizeof(unsigned short), 1, fp );
	fseek(fp, 10, SEEK_SET) ; fread( &p_bitmapheader->bfOffBits,sizeof(unsigned long), 1, fp );
	fseek(fp, 14, SEEK_SET) ; fread( &p_bitmapinfo->biSize, sizeof(unsigned long), 1, fp );
	fseek(fp, 18, SEEK_SET) ; fread( &p_bitmapinfo->biWidth, sizeof(unsigned long), 1, fp );  
	fseek(fp, 22, SEEK_SET) ; fread( &p_bitmapinfo->biHeight, sizeof(unsigned long), 1, fp ); 
	fseek(fp, 26, SEEK_SET) ; fread( &p_bitmapinfo->biPlanes, sizeof(unsigned short), 1, fp );  
	fseek(fp, 28, SEEK_SET) ; fread( &p_bitmapinfo->biBitCount, sizeof(unsigned short), 1, fp ); 
	fseek(fp, 30, SEEK_SET) ; fread( &p_bitmapinfo->biCompression, sizeof(unsigned long), 1, fp ); 
	fseek(fp, 34, SEEK_SET) ; fread( &p_bitmapinfo->biSizeImage, sizeof(unsigned long), 1, fp ); 
	fseek(fp, 38, SEEK_SET) ; fread( &p_bitmapinfo->biXPelsPerMeter, sizeof(unsigned long), 1, fp ); 
	fseek(fp, 42, SEEK_SET) ; fread( &p_bitmapinfo->biYPelsPerMeter, sizeof(unsigned long), 1, fp );
	fseek(fp, 46, SEEK_SET) ; fread( &p_bitmapinfo->biClrUsed, sizeof(unsigned long), 1, fp );
	fseek(fp, 50, SEEK_SET) ; fread( &p_bitmapinfo->biClrImportant, sizeof(unsigned long), 1, fp ); 
}
int ReadBitmapFile(char* file)
{ 
	BITMAPFILEHEADER bitmapheader ; 
	BITMAPINFOHEADER bitmapinfo ; 
	FILE *fp;  
	fp = fopen(file , "rb") ; 
	if(!fp)
	{   
		puts("Read file failed.") ;   
		return 0; 
	}  
	ReadHeader(fp, &bitmapheader , &bitmapinfo) ;  
	if(bitmapinfo.biBitCount != 24) {   puts("UNSUPPORT") ;   return 0; }
	ImageWidth = bitmapinfo.biWidth; ImageHeight = bitmapinfo.biHeight;

	if(Image) delete Image;
	Image=new byte[ImageWidth*ImageHeight*3];
	byte* ig=Image;
	int offset=(ImageWidth*3+3)/4*4-ImageWidth*3;
	
	fseek(fp, bitmapheader.bfOffBits, SEEK_SET) ; 
	for(int i=0;i<ImageHeight;++i)
	{
		fread(ig,3,ImageWidth,fp);ig+=ImageWidth*3;
		if(offset) fseek(fp, offset, SEEK_CUR); 
	}

	fclose(fp) ;
	return 1;
}

int LoadGLTextures()
{
	if(ReadBitmapFile("world.bmp"))
	{
		glEnable(GL_TEXTURE_2D);
		glGenTextures(1,texName);		
		glBindTexture(GL_TEXTURE_2D,texName[0]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ImageWidth,ImageHeight, 0,    GL_RGB, GL_UNSIGNED_BYTE,Image);
		return 1;
	}
	return 0;
}

bool binit=false;

void initLight()
{
	GLfloat light_position[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat light_ambient[]   = {0.2f, 0.2f, 0.2f, 1.0f};
	GLfloat light_diffuse[]   = {0.5f, 0.5f, 0.5f, 0.0f};
	GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT,   light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,   light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	glEnable(GL_LIGHT0);

}
v4* spherevts;
int init()
{
	binit=true;
	initLight();
	if(!LoadGLTextures()) return 0;
	
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f,0.0f,0.0f,0.5f);
	glClearDepth(1.0f);
	spherevts=glxGenSphere(0.3,SPHERE_SLICE,SPHERE_STACK);

	float fogColor[4] = {0.6, 0.6, 0.6, 1.0};

	glFogi (GL_FOG_MODE, GL_EXP);
	glFogfv (GL_FOG_COLOR, fogColor);
	glFogf (GL_FOG_START, 4.0);
	glFogf (GL_FOG_END,15.0);
	glFogf (GL_FOG_DENSITY,3.0);

	glEnable(GL_FOG);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);

	return 1;

}

void rotateAxis(float rot,float x,float y,float z)
{
#define PI 3.141592653
	double ax=atan(z/sqrt(x*x+y*y))*180.0/PI,az=-atan(x/y)*180.0/PI;
	glRotatef(rot,x,y,z);
	glRotatef(az,0.0,0.0,1.0);
	glRotatef(ax,1.0,0.0,0.0);
}


void RenderCallBack()
{
	if(!binit) return;
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	

	GLfloat matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);

 	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glTranslatef(0.0,0.0,-1.50);

	rotateAxis(yrot,0.5,1.0,0.5);

	glxTextureSphere(spherevts,SPHERE_SLICE,SPHERE_STACK);

	glxSwapBuffer();

	yrot+=1.0f;
	glPopMatrix();
}

void ReshapeCallback(int width, int height)
{
#ifdef OPENGL_DRAW
	glxResize(width,height);
#endif
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,((double)width)/((double)height),0.1f,10000.0f);

	glViewport(0,0,width,height);

	gHeight=height;gWidth=width;
	RenderCallBack();
}
void MouseCallBack(int button, int state, int x, int y)
{
	prevx=x;prevy=y;
}

void MotionCallBack(int button,int x, int y)
{
	switch(button)
	{
	case 0:
		cam.slide((x-prevx)*0.01,(prevy-y)*0.01,0.0);
		break;
	case 1:		
		cam.roll((prevy-y)*0.0005);
		cam.yaw((prevx-x)*0.0005);
		break;
	default:
		break;
	}
	prevx=x;prevy=y;
	RenderCallBack();
}

void PassiveMotionCallBack(int x, int y)
{
}

void MouseWheelCallBack(int rCount)
{
	cam.slide(0.0,0.0,-0.3*rCount);
	RenderCallBack();
}

void KeyboardCallBack(unsigned char key)
{	
}