#include "device.h"

#include <windows.h>
#ifdef USE_OPENGL
#include <gl/GL.h>
#pragma comment(lib,"opengl32.lib")
//#pragma comment(lib,"glu32.lib")
#endif

#include <stdlib.h>
#include <stdio.h>


namespace device
{
	LARGE_INTEGER pc1,pc2;
#ifdef USE_OPENGL

	void draw(void* buffer,int l,int b,int w,int h)
	{
		//////////////////////////////////////////////////////////////////////////
// 		int tot_sz=w*h*3;
// 		GLubyte* buf=(GLubyte*)buffer;
// 		for(int i=0;i<tot_sz;++i)
// 		{
// 			buf[i]=rand()%256;
// 		}
		//////////////////////////////////////////////////////////////////////////
		//QueryPerformanceCounter(&pc1);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,w,0,h,-1.0,1.0);
		glViewport(0,0,w,h);

		
		
		

		glRasterPos2i(l,b);
		glDrawPixels(w, h,GL_RGB, GL_UNSIGNED_BYTE, buffer);
		glFlush();

// 		QueryPerformanceCounter(&pc2);
// 		printf("draw pixels: %lld\n",pc2.QuadPart-pc1.QuadPart);
	}
#else
	void draw(void* buffer,int l,int b,int w,int h)
	{

	}
// 	void draw(void* buffer,int l,int t,int r,int b,int bufw)
// 	{	
// 		BITMAPINFO bmpInfo;   
// 		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);   
// 		bmpInfo.bmiHeader.biWidth = bufw;   
// 		bmpInfo.bmiHeader.biHeight = t-b;   
// 		bmpInfo.bmiHeader.biPlanes = 1;   
// 		bmpInfo.bmiHeader.biBitCount = 24;   
// 		bmpInfo.bmiHeader.biCompression = BI_RGB;   
// 		bmpInfo.bmiHeader.biSizeImage = 0;   
// 		bmpInfo.bmiHeader.biXPelsPerMeter = 3000;   
// 		bmpInfo.bmiHeader.biYPelsPerMeter = 3000;   
// 		bmpInfo.bmiHeader.biClrUsed = 0;   
// 		bmpInfo.bmiHeader.biClrImportant = 0;   
// 
// 		//  BITMAPINFOHEADER bmhinfo;
// 		//  bmhinfo.biSize = sizeof(BITMAPINFOHEADER);   
// 		//  bmhinfo.biWidth = bufw;   
// 		//  bmhinfo.biHeight = t-b;   
// 		//  bmhinfo.biPlanes = 1;   
// 		//  bmhinfo.biBitCount = 24;   
// 		//  bmhinfo.biCompression = BI_RGB;   
// 		//  bmhinfo.biSizeImage = 0;   
// 		//  bmhinfo.biXPelsPerMeter = 3000;   
// 		//  bmhinfo.biYPelsPerMeter = 3000;   
// 		//  bmhinfo.biClrUsed = 0;   
// 		//  bmhinfo.biClrImportant = 0;   
// 
// 
// 		//SetDIBits(memDC, bmp, 0, b-t, buffer, &bmpInfo, DIB_RGB_COLORS);   
// 		//BitBlt(bindDC,l,t,r-l,b-t, memDC, l, 0, SRCCOPY); 
// 		//DeleteObject(SelectObject(memDC, oldBmp));
// 		//DeleteDC(memDC);
// 		//StretchDIBits(bindDC,l,t,r-l,b-t,0,b-t-1,r-l,t-b,buffer,&bmpInfo,DIB_RGB_COLORS,SRCCOPY); 
// 
// 		SetDIBitsToDevice(bindDC,l,t,r-l,b-t,0,0,0,b-t,buffer,&bmpInfo,DIB_RGB_COLORS);
// 	}
#endif
}