#include "../include/buf.h"
#include "../include/osspec.h"
#include "../include/gltypes.h"
extern GLContext gl_context;

FrameBufferItem frameBuffer[MAX_FRAMEBUF_SIZE];
double zBuffer[MAX_FRAMEBUF_SIZE];
double alphaBuffer[MAX_FRAMEBUF_SIZE];
int bufLineWidth,bufLineBytes;
int bufL=0;
int bufT=0;
int bufR=1000;
int bufB=1000;

#ifdef OPENGL_DRAW
#define BUF_LINE_POS(i) (i)
#else
#define BUF_LINE_POS(i) (bufT+bufB-i)
#endif

bool depthNever(double& incoming,double& stored)
{
	return false;
}

bool depthLess(double& incoming,double& stored)
{
	return incoming<stored;
}

bool depthLEqual(double& incoming,double& stored)
{
	return incoming<=stored;
}

bool depthEqual(double& incoming,double& stored)
{
	return incoming==stored;
}

bool depthGreater(double& incoming,double& stored)
{
	return incoming>stored;
}

bool depthNotEqual(double& incoming,double& stored)
{
	return incoming!=stored;
}

bool depthGEqual(double& incoming,double& stored)
{
	return incoming>=stored;
}
bool depthAlways(double& incoming,double& stored)
{
	return true;
}

DEPTHFUNC depthFuncs[]={depthNever,depthLess,depthEqual,depthLEqual,
						depthGreater,depthNotEqual,
						depthGEqual,depthAlways};
DEPTHFUNC depthFunc;


byte* GET_FRAME_BUFFER(int i,int j)
{ 
	i=BUF_LINE_POS(i);
	return ((byte*)frameBuffer)+(i)*bufLineBytes+(j)*3;
}
double* GET_Z_BUFFER(int i,int j)
{
	i=BUF_LINE_POS(i);
	return zBuffer+(i)*bufLineWidth+(j);
}
double* GET_ALPHA_BUFFER(int i,int j)
{
	i=BUF_LINE_POS(i);
	return alphaBuffer+(i)*bufLineWidth+(j);
}

void glbufResize(int l,int t,int r,int b)
{
	int tmp;
	if(l>r){tmp=l;l=r;r=tmp;}
	if(b>t){tmp=t;t=b;b=tmp;}
	if(l<0) bufL=0;else bufL=l;
	if(t<0) bufT=0;else bufT=t;
	if(r<0) bufR=0;else bufR=r;
	if(b<0) bufB=0;else bufB=b;

	bufLineWidth=r-l;
	bufLineBytes=(bufLineWidth*3+3)/4*4;
}

void glbufSetPixel(int x, int y, double z,double r,double g,double b,double a)
{
	if(x>=bufL&&x<=bufR&&y>=bufB&&y<=bufT)
	{
		x-=bufL;y-=bufB;
		if(!gl_context.depth_test||depthFunc(z,*(GET_Z_BUFFER(y,x))))
		{
			byte* buf=GET_FRAME_BUFFER(y,x);
			buf[0]=min(255*r,255);
			buf[1]=min(255*g,255);
			buf[2]=min(255*b,255);
			if(gl_context.depth_mask) *(GET_Z_BUFFER(y,x))=z;
			*(GET_ALPHA_BUFFER(y,x))=a;
		}		
	}
}


// struct CopyStruct
// {
// 	char buf[64];
// };
// 
// void xmemcpy(void* dst,void* src,int cnt)
// {
// 	int c=cnt/64,d=cnt%64;
// 	CopyStruct* dt=(CopyStruct*)dst,*sc=(CopyStruct*)src;
// 	
// 	while(c--)
// 		*dt++=*sc++;
// 
// 	if(d){memcpy(dt,sc,d);}
// }


void glbufClearDepthBuffer(double d)
{
	double* first=zBuffer,*p=zBuffer;
	int cnt=bufLineWidth*(bufT-bufB);
	while(cnt--) *p++=d;
}
// void glbufClearColorBuffer(double *v)
// {
// 	byte* first=(byte*)frameBuffer,*p=(byte*)frameBuffer;
// 	int cnt=bufLineWidth*(bufT-bufB);
// 	byte a,b,c;
// 
// 	if(gl_context.fog_enabled)
// 	{
// 		double ffar=exp(-gl_context.fog_density*gl_context.fog_end);
// 		double color[3];
// 
// 		color[0]=lerp(gl_context.fog_color[0],v[0],ffar);
// 		color[1]=lerp(gl_context.fog_color[1],v[1],ffar);
// 		color[2]=lerp(gl_context.fog_color[2],v[2],ffar);
// 
// 		a=255*color[RGB_CORRECT(0)];b=255*color[RGB_CORRECT(1)];c=255*color[RGB_CORRECT(2)];
// 	}
// 	else
// 	{
// 		a=255*v[RGB_CORRECT(0)];b=255*v[RGB_CORRECT(1)];c=255*v[RGB_CORRECT(2)];
// 	}	
// 	while(cnt--)
// 	{
// 		*p++=a;*p++=b;*p++=c;
// 	}
// 	
// }

//二分赋值
void glbufClearColorBuffer(double *v)
{
	byte* first=(byte*)frameBuffer,*p=(byte*)frameBuffer;
	int cpcnt=1,cnt=bufLineWidth*(bufT-bufB);

	if(gl_context.fog_enabled)
	{
		double ffar=exp(-gl_context.fog_density*gl_context.fog_end);
		double color[3];

		color[0]=lerp(gl_context.fog_color[0],v[0],ffar);
		color[1]=lerp(gl_context.fog_color[1],v[1],ffar);
		color[2]=lerp(gl_context.fog_color[2],v[2],ffar);

		*p++=255*color[RGB_CORRECT(0)];*p++=255*color[RGB_CORRECT(1)];*p++=255*color[RGB_CORRECT(2)];
	}
	else
	{
		*p++=255*v[RGB_CORRECT(0)];*p++=255*v[RGB_CORRECT(1)];*p++=255*v[RGB_CORRECT(2)];
	}	

	while((cpcnt<<1)<=cnt)
	{
		memcpy(p,first,cpcnt*3);
		p+=cpcnt*3;
		cpcnt<<=1;
	}
	if(cpcnt<cnt)
		memcpy(p,first,3*(cnt-cpcnt));
}

//二分赋值
// void glbufClearDepthBuffer(double d)
// {	
// 	double* first=zBuffer,*p=zBuffer;
// 	int cpcnt=1,cnt=bufLineWidth*(bufT-bufB);;
// 
// 	*p++=d;
// 
// 	while((cpcnt<<1)<=cnt)
// 	{
// 		memcpy(p,first,cpcnt*sizeof(double));
// 		p+=cpcnt;
// 		cpcnt<<=1;
// 	}
// 	if(cpcnt<cnt)
// 		memcpy(p,first,(cnt-cpcnt)*sizeof(double));
// }

//二分赋值
void glbufClearAlphaBuffer(double d)
{	
	double* first=alphaBuffer,*p=alphaBuffer;
	int cpcnt=1,cnt=bufLineWidth*(bufT-bufB);;

	*p++=d;

	while((cpcnt<<1)<=cnt)
	{
		memcpy(p,first,cpcnt*sizeof(double));
		p+=cpcnt;
		cpcnt<<=1;
	}
	if(cpcnt<cnt)
		memcpy(p,first,(cnt-cpcnt)*sizeof(double));
}

void glxSwapBuffer()
{
#ifdef OPENGL_DRAW
	glxDrawBuffer(frameBuffer,bufL,bufB,bufLineWidth,bufT-bufB);
#else
	glxDrawBuffer(frameBuffer,bufL,bufB,bufR,bufT,bufLineWidth);
#endif
}