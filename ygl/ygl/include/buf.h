#ifndef _YGL_BUF_H_
#define _YGL_BUF_H_

#include "ylimits.h"

#define lerp(a,b,f) ((a)+(f)*((b)-(a)))

#define EQUALS(a,b) (fabs(a-b)<1e-8)

#ifdef OPENGL_DRAW
#define RGB_CORRECT(x) (x)
#else
#define RGB_CORRECT(x) (2-x)
#endif

#define TEX_CORRECT(x) (2-x)


typedef unsigned char byte;
typedef byte FrameBufferItem[3];
typedef bool (*DEPTHFUNC)(double& incoming,double& stored);

extern FrameBufferItem frameBuffer[MAX_FRAMEBUF_SIZE];
extern double zBuffer[MAX_FRAMEBUF_SIZE];
extern int bufLineWidth,bufLineBytes;

extern DEPTHFUNC depthFuncs[];
extern DEPTHFUNC depthFunc;

// #define GET_FRAME_BUFFER(i,j) (((byte*)frameBuffer)+(i)*bufLineBytes+(j)*3)
// #define GET_Z_BUFFER(i,j) (zBuffer+(i)*bufLineWidth+(j))

byte* GET_FRAME_BUFFER(int i,int j);
double* GET_Z_BUFFER(int i,int j);
double* GET_ALPHA_BUFFER(int i,int j);


void glbufResize(int l,int t,int r,int b);
void glbufSetPixel(int, int ,double,double ,double ,double,double);
#define glbufSetPixelv(x,y,z,v) glbufSetPixel(x,y,z,v[0],v[1],v[2],v[3])
void glbufClearColorBuffer(double *);
void glbufClearDepthBuffer(double);
void glbufClearAlphaBuffer(double);
void glxSwapBuffer();

#endif