#ifndef _YGL_OSSPEC_H_
#define _YGL_OSSPEC_H_

#include <windows.h>

//#define OPENGL_DRAW

bool glxAttachDC(HDC);
void glxDetachDC();
#ifdef OPENGL_DRAW
void glxResize(int width,int height);
void glxDrawBuffer(void* buffer,int l,int b,int w,int h);
#else
void glxDrawBuffer(void* buffer,int l,int t,int r,int b,int bufw);
#endif

#endif