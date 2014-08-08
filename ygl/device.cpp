#include "device.h"

#include <windows.h>

#define USE_OPENGL
//#define USE_GLEW

#ifdef USE_OPENGL

#ifdef USE_GLEW
#include <gl/glew.h>
#pragma comment(lib,"glew32.lib")
#else
#include <gl/GL.h>
#endif

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#endif

// #include <stdlib.h>
// #include <stdio.h>
namespace ygl{
namespace device
{
//	LARGE_INTEGER pc1,pc2;
#ifdef USE_OPENGL

#ifdef USE_GLEW
	int dummy_init=glewInit();
#endif

//	void resize(int w,int h)
//	{
// 		glMatrixMode(GL_PROJECTION);
// 		glLoadIdentity();
// 		glOrtho(0,w,0,h,-1.0,1.0);
// 		glViewport(0,0,w,h);
//	}
//	GLfloat invertMap[256] = { 0.0f };
//	void init()
//	{
// 		for (int i = 0; i < 256; ++i)
// 		{
// 			invertMap[i] = 1.0f - 1.0f / 255.0f * (GLfloat)i;
// 		}
// 
// 		glPixelMapfv(GL_PIXEL_MAP_R_TO_R, 256, invertMap);
//		//glPixelMapfv(GL_PIXEL_MAP_G_TO_G, 256, invertMap);
//		//glPixelMapfv(GL_PIXEL_MAP_B_TO_B, 256, invertMap);
//
//		//glPixelTransferf(GL_RED_SCALE,255.f/0xffffffff);
//	}

	void draw(void* buffer,int l,int b,int w,int h,int linew)
	{
		//////////////////////////////////////////////////////////////////////////
// 		int tot_sz=w*h;
// 		GLubyte* buf=(GLubyte*)buffer;
// 		for(int i=0;i<tot_sz;++i)
// 		{
// 			buf[i]=255-buf[i];//rand()%256;
// 		}
		//////////////////////////////////////////////////////////////////////////
		//QueryPerformanceCounter(&pc1);

#ifdef USE_GLEW
		glWindowPos2i(l,b);
#else
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,l+w,0,b+h,-1.0,1.0);
		glViewport(0,0,l+w,b+h);

		glRasterPos2i(l,b);
#endif
		glDrawPixels(w, h,GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		//glDrawPixels(w,h,GL_RED,GL_UNSIGNED_BYTE,buffer);
		glFlush();


// 		QueryPerformanceCounter(&pc2);
// 		printf("draw pixels: %lld\n",pc2.QuadPart-pc1.QuadPart);
	}
#else
	void init(){}
	//HDC hdc=wglGetCurrentDC();
	void draw(void* buffer,int l,int b,int w,int h,int linew)
// 	{
// 	}
// 	void draw(void* buffer,int l,int t,int r,int b,int bufw)
	{	
		/*
		BITMAPINFO bmpInfo;   
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);   
		bmpInfo.bmiHeader.biWidth = linew;   
		bmpInfo.bmiHeader.biHeight = h;
		bmpInfo.bmiHeader.biPlanes = 1;   
		bmpInfo.bmiHeader.biBitCount = 32;   
		bmpInfo.bmiHeader.biCompression = BI_RGB;   
		bmpInfo.bmiHeader.biSizeImage = 0;   
		bmpInfo.bmiHeader.biXPelsPerMeter = 3000;   
		bmpInfo.bmiHeader.biYPelsPerMeter = 3000;   
		bmpInfo.bmiHeader.biClrUsed = 0;   
		bmpInfo.bmiHeader.biClrImportant = 0;

		SetDIBitsToDevice(hdc,l,b,w,h,0,0,0,h,buffer,&bmpInfo,DIB_RGB_COLORS);

		//BitBlt(hdc,l,b,w,h,0,0,0,SRCCOPY);

		//  BITMAPINFOHEADER bmhinfo;
		//  bmhinfo.biSize = sizeof(BITMAPINFOHEADER);   
		//  bmhinfo.biWidth = bufw;   
		//  bmhinfo.biHeight = t-b;   
		//  bmhinfo.biPlanes = 1;   
		//  bmhinfo.biBitCount = 24;   
		//  bmhinfo.biCompression = BI_RGB;   
		//  bmhinfo.biSizeImage = 0;   
		//  bmhinfo.biXPelsPerMeter = 3000;   
		//  bmhinfo.biYPelsPerMeter = 3000;   
		//  bmhinfo.biClrUsed = 0;   
		//  bmhinfo.biClrImportant = 0;   


		//SetDIBits(memDC, bmp, 0, b-t, buffer, &bmpInfo, DIB_RGB_COLORS);   
		//BitBlt(bindDC,l,t,r-l,b-t, memDC, l, 0, SRCCOPY); 
		//DeleteObject(SelectObject(memDC, oldBmp));
		//DeleteDC(memDC);
		//StretchDIBits(bindDC,l,t,r-l,b-t,0,b-t-1,r-l,t-b,buffer,&bmpInfo,DIB_RGB_COLORS,SRCCOPY); 

		//SetDIBitsToDevice(hdc,l,t,r-l,b-t,0,0,0,b-t,buffer,&bmpInfo,DIB_RGB_COLORS);
		*/
	}
#endif
}
}