#ifndef _YGL_GLOBAL_
#define _YGL_GLOBAL_

#define YGL_CLAMPF(x) ((x)<0.f?0.f:((x)>1.f:1.f,(x)))
#define YGL_CLAMPD(x) ((x)<0?0:((x)>1.0:1.0,(x)))
#define YGL_CLAMP_MIN(x,minv) ((x)<(minv)?(minv):(x))
#define YGL_CLAMP_MAX(x,maxv) ((x)>(maxv)?(maxv):(x))
#define YGL_CLAMP_MIN_MAX(x,minv,maxv) ((x)<(minv)?(minv):((x)>(maxv)?(maxv):(x)))

#define YGL_COLOR_F2IF(fc) (YGL_CLAMP_MIN_MAX(fc,0.f,1.f)*255)
#define YGL_COLOR_F2I(fc) ((int)YGL_COLOR_F2IF(fc))

//#define YGL_DOSTH_3TIMES_1

#define FLOAT_EQ_0 1
#define FLOAT_GT_0 2
#define FLOAT_LE_0 4
#define FLOAT_ALWAYS (FLOAT_EQ_0|FLOAT_GT_0|FLOAT_LE_0)

// assume int 32
// return 0 for 0.0, 1 for positive, 2 for negative
#define FLOAT_AS_INT(f) (*((unsigned int*)&f))
#define INT_AS_FLOAT(i) (*((float*)&f))
// #define SIGN_OF_FLOAT(f) (\
// 	((   FLOAT_AS_INT(f) & 0x7FFFFFFF)==0)?1:(\
// 		2 << ( FLOAT_AS_INT(f) & 0x80000000) >> 31 )\
// 	))
inline int SIGN_OF_FLOAT(float f)
{
	return (
		((   FLOAT_AS_INT(f) & 0x7FFFFFFF)==0)?1:
		( 2 << (( FLOAT_AS_INT(f) & 0x80000000) >> 31) )
		);
}
inline int SIGN_OF_DOUBLE(double f)
{
	return (
		( f==0.0 )?1:
		( 2 << ( *((unsigned long long*)&f) >> 63) )
		);
}
// inline int fast_sign_of(float f)
// {
// 	if (((int&)f & 0x7FFFFFFF)==0) return 0; // test exponent & mantissa bits: is input zero?
// 	return 1 << (signed((int&)f & 0x80000000) >> 31 );
// }

// todo: sse??
#define ASSIGN_F4(a,b) {(a)[0]=(b)[0];(a)[1]=(b)[1];(a)[2]=(b)[2];(a)[3]=(b)[3];}
#define ASSIGN_F3_WITH(v,a,b,c) {(v)[0]=a;(v)[1]=b;(v)[2]=c;}
#define ASSIGN_F4_WITH(v,a,b,c,d) {(v)[0]=a;(v)[1]=b;(v)[2]=c;(v)[3]=d;}

// #define ASSIGN_XYZW(v) ASSIGN_F4_WITH(v,x,y,z,w)
// #define ASSIGN_RGB(c) {(c)[0]=r;(c)[1]=g;(c)[2]=b;}
// #define ASSIGN_RGBA(v) ASSIGN_F4_WITH(v,r,g,b,a)

#define GL_ODD true
#define GL_EVEN false

#define _NOT_IMPLEMENTED_(xx) printf(#xx "not implemented\n");

#ifdef M_PI
#undef M_PI
#endif
#define M_PI       3.14159265358979323846f
#define INV_PI     0.31830988618379067154f
#define INV_TWOPI  0.15915494309189533577f
#define INV_FOURPI 0.07957747154594766788f

#define INV255	   0.00392156862745098039f

#define R2D(rad) ((rad)*/*180.f/M_PI*/57.2957795130823208768f)
#define D2R(deg) ((deg)*/*M_PI/180.f*/0.01745329251994329577f)

#define ROUND(x) ((int)((x)+0.5f))

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2

#define X_OF(p) ((p)[0])
#define Y_OF(p) ((p)[1])
#define Z_OF(p) ((p)[2])
#define W_OF(p) ((p)[3])

#define XOR_SWAP(a,b) {a=a^b;b=a^b;a=a^b;}

#include <assert.h>
#define Assert assert
#include <float.h>
#define isnan _isnan
#define isinf(f) (!_finite((f)))
#include <math.h>

#ifdef YGL_POW_APPROXIMATE
#define YGL_POW(x,e) ((x)/((e)-(e)*(x)+(x)))
#else
#define YGL_POW(x,e) (powf(x,e))
#endif

inline float lerp(float a,float b,float f)
{
	return (1.f-f)*a+f*b;
}
// inline double lerp(double a,double b,double f)
// {
// 	return (1.0-f)*a+f*b;
// }

#endif