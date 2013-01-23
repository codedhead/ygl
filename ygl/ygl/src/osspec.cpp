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
	GLuint PixelFormat;                 // �������ƥ��Ľ��
	static PIXELFORMATDESCRIPTOR pfd=	// pfd ���ߴ�ʹ�õ����ظ�ʽ
	{
		sizeof(PIXELFORMATDESCRIPTOR),								// ������ʽ�������Ĵ�С
		1,                                                          // �汾��
		PFD_DRAW_TO_WINDOW |										// ��ʽ֧�ִ���
		PFD_SUPPORT_OPENGL,											// ��ʽ����֧��OpenGL
		PFD_TYPE_RGBA,												// ���� RGBA ��ʽ
		24,															// ѡ��ɫ�����
		0, 0, 0, 0, 0, 0,											// ���Ե�ɫ��λ
		0,                                                          // ��Alpha����
		0,                                                          // ����Shift Bit
		0,                                                          // ���ۼӻ���
		0, 0, 0, 0,													// ���Ծۼ�λ
		0,															// 0λ Z-���� (��Ȼ���)
		0,                                                          // ���ɰ建��
		0,                                                          // �޸�������
		PFD_MAIN_PLANE,												// ����ͼ��
		0,                                                          // Reserved
		0, 0, 0														// ���Բ�����
	};

	if (!(PixelFormat=ChoosePixelFormat(bindDC,&pfd)))// Windows �ҵ���Ӧ�����ظ�ʽ����?
	{
		MessageBox(NULL,"�����������ظ�ʽ","����",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	if(!SetPixelFormat(bindDC,PixelFormat,&pfd))    // �ܹ��������ظ�ʽô?
	{
		MessageBox(NULL,"�����������ظ�ʽ","����",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	if (!(hRC=wglCreateContext(bindDC)))                                      // �ܷ�ȡ����ɫ������?
	{
		MessageBox(NULL,"���ܴ���OpenGL��Ⱦ������","����",MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	if(!wglMakeCurrent(bindDC,hRC))                         // ���Լ�����ɫ������
	{
		MessageBox(NULL,"���ܼ��ǰ��OpenGL��Ȼ������","����",MB_OK|MB_ICONEXCLAMATION);
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
			MessageBox(NULL,"�ͷ�DC��RCʧ�ܡ�","�رմ���",MB_OK | MB_ICONINFORMATION);
		}
		if (!wglDeleteContext(hRC))
		{
			MessageBox(NULL,"�ͷ�RCʧ�ܡ�","�رմ���",MB_OK | MB_ICONINFORMATION);
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