// ygltest.cpp : Defines the entry point for the application.
//

//使用鼠标滑轮
#ifndef WINVER                // Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0501        // Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT        // Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0501        // Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif                        

#ifndef _WIN32_WINDOWS        // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0501 // Change this to the appropriate value to target Windows Me or later.
#endif

#include "stdafx.h"
#include "resource.h"

#include "ygl/ygl.h"
#include "./test/callback.h"

#include <time.h>

#include   <io.h> 
#include   <stdio.h> 
#include   <fcntl.h> 

#pragma comment(linker, "/NODEFAULTLIB:libcmt.lib  /NODEFAULTLIB:libcd.lib ")
#pragma comment(lib,"ygl.lib")

void   OpenConsole() 
{ 
	AllocConsole(); 
	HANDLE   handle   =   GetStdHandle(STD_OUTPUT_HANDLE); int   hCrt   =   _open_osfhandle((long)handle,_O_TEXT);   
	FILE   *   hf   =   _fdopen(   hCrt,   "w "   ); 
	*stdout   =   *hf; 
}


HWND gWnd=NULL;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_YGLTEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);


	if(!init())
	{
		MessageBox(NULL,"Failed to initialize. Maybe the image file is missing.","Error",MB_OK);
		return -1;
	}
	

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	//调试用的控制台
	OpenConsole();

	HDC hDC=GetDC(gWnd);	
	if(!glxAttachDC(hDC))
	{
		glxDetachDC();
		ReleaseDC(gWnd,hDC);
		return -1;
	}

	puts("该控制台创建用来显示fps. 使用鼠标或滑轮移动场景.");
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_YGLTEST);

	// Main message loop:
	int done=0;
	clock_t t1,t2,t3;
	t1=clock();
	int fcnt=0;
	int fps=33;
	//char buf[100];
	while(!done)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message==WM_QUIT)
				done=TRUE;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			//t3=clock();
			RenderCallBack();
			t2=clock();

// 			if(t2>t3) fps=1000/(t2-t3);
// 			sprintf(buf,"fps:%d\n",fps);
			//SetWindowText(gWnd,buf);

			++fcnt;
			if(t2-t1>=1000)
			{
				printf("fps:%d\n",fcnt);
				fcnt=0;
				t1=t2;
			}
		}
	}

	glxDetachDC();
	ReleaseDC(gWnd,hDC);

	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_YGLTEST);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//(LPCSTR)IDC_YGLTEST;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   gWnd = hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 600, 600, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	static int button=-1;

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_SIZE:
			ReshapeCallback(LOWORD(lParam),HIWORD(lParam));
			break;		
		case WM_ERASEBKGND:
			break;
		case WM_MOUSEWHEEL:
			MouseWheelCallBack(((short)HIWORD(wParam))/WHEEL_DELTA);
			break;
		case WM_CHAR:
			KeyboardCallBack(wParam);
			break;
		case WM_LBUTTONDOWN:
			SetCapture(gWnd);
			button=0;
			MouseCallBack(0,0,LOWORD(lParam),HIWORD(lParam));
			break;
		case WM_RBUTTONDOWN:
			SetCapture(gWnd);
			button=1;
			MouseCallBack(1,0,LOWORD(lParam),HIWORD(lParam));
			break;
		case WM_RBUTTONUP:
		case WM_LBUTTONUP:
			ReleaseCapture();
			button=-1;
			break;
		case WM_MOUSEMOVE:
			{
				if(button==-1)
					PassiveMotionCallBack(LOWORD(lParam),HIWORD(lParam));
				else
					MotionCallBack(button,LOWORD(lParam),HIWORD(lParam));
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}


// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
