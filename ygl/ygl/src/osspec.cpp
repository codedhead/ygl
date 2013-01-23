#include "../include/osspec.h"

#ifdef OPENGL_DRAW
#include <gl/GL.h>
#include <gl/GLu.h>
#include <gl/GLAux.h>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

static HGLRC hRC=NULL;
#endif

static HDC bindDC=NULL;

bool glxAttachDC(HDC hDC)
{
	bindDC=hDC;
#ifdef OPENGL_DRAW
	GLuint PixelFormat;                 // 保存查找匹配的结果
	static PIXELFORMATDESCRIPTOR pfd=	// pfd 告诉窗使用的像素格式
	{
		sizeof(PIXELFORMATDESCRIPTOR),								// 上述格式描述符的大小
		1,                                                          // 版本号
		PFD_DRAW_TO_WINDOW |										// 格式支持窗口
		PFD_SUPPORT_OPENGL,											// 格式必须支持OpenGL
		PFD_TYPE_RGBA,												// 申请 RGBA 格式
		24,															// 选定色彩深度
		0, 0, 0, 0, 0, 0,											// 忽略的色彩位
		0,                                                          // 无Alpha缓存
		0,                                                          // 忽略Shift Bit
		0,                                                          // 无累加缓存
		0, 0, 0, 0,													// 忽略聚集位
		0,															// 0位 Z-缓存 (深度缓存)
		0,                                                          // 无蒙板缓存
		0,                                                          // 无辅助缓存
		PFD_MAIN_PLANE,												// 主绘图层
		0,                                                          // Reserved
		0, 0, 0														// 忽略层遮罩
	};

	if (!(PixelFormat=ChoosePixelFormat(bindDC,&pfd)))// Windows 找到相应的象素格式了吗?
	{
		MessageBox(NULL,"不能设置像素格式","错误",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	if(!SetPixelFormat(bindDC,PixelFormat,&pfd))    // 能够设置象素格式么?
	{
		MessageBox(NULL,"不能设置像素格式","错误",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	if (!(hRC=wglCreateContext(bindDC)))                                      // 能否取得着色描述表?
	{
		MessageBox(NULL,"不能创建OpenGL渲染描述表","错误",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	if(!wglMakeCurrent(bindDC,hRC))                         // 尝试激活着色描述表
	{
		MessageBox(NULL,"不能激活当前的OpenGL渲然描述表","错误",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
#endif
	return true;
}

void glxDetachDC()
{ 	
#ifdef OPENGL_DRAW
	if (hRC)
	{
		if (!wglMakeCurrent(NULL,NULL))
		{
			MessageBox(NULL,"释放DC或RC失败。","关闭错误",MB_OK | MB_ICONINFORMATION);
		}
		if (!wglDeleteContext(hRC))
		{
			MessageBox(NULL,"释放RC失败。","关闭错误",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;
	}
#endif

	bindDC=NULL;
}
#ifdef OPENGL_DRAW
void glxResize(int width,int height)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,width,0,height);
	glViewport(0,0,width,height);
}

void glxDrawBuffer(void* buffer,int l,int b,int w,int h)
{
	glRasterPos2i(l,b);
	glDrawPixels(w, h,GL_RGB, GL_UNSIGNED_BYTE, buffer);
	glFlush();
}
#else
void glxDrawBuffer(void* buffer,int l,int t,int r,int b,int bufw)
{	
	BITMAPINFO bmpInfo;   
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);   
	bmpInfo.bmiHeader.biWidth = bufw;   
	bmpInfo.bmiHeader.biHeight = t-b;   
	bmpInfo.bmiHeader.biPlanes = 1;   
	bmpInfo.bmiHeader.biBitCount = 24;   
	bmpInfo.bmiHeader.biCompression = BI_RGB;   
	bmpInfo.bmiHeader.biSizeImage = 0;   
	bmpInfo.bmiHeader.biXPelsPerMeter = 3000;   
	bmpInfo.bmiHeader.biYPelsPerMeter = 3000;   
	bmpInfo.bmiHeader.biClrUsed = 0;   
	bmpInfo.bmiHeader.biClrImportant = 0;   

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

	SetDIBitsToDevice(bindDC,l,t,r-l,b-t,0,0,0,b-t,buffer,&bmpInfo,DIB_RGB_COLORS);
}
#endif