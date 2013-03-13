#ifndef _YGL_GLOBAL_
#define _YGL_GLOBAL_

#define YGL_CLAMPF(x) ((x)<0.f?0.f:((x)>1.f:1.f,(x)))
#define YGL_CLAMPD(x) ((x)<0?0:((x)>1.0:1.0,(x)))
#define YGL_CLAMP_MIN(x,minv) ((x)<(minv)?(minv):(x))
#define YGL_CLAMP_MAX(x,maxv) ((x)>(maxv)?(maxv):(x))
#define YGL_CLAMP_MIN_MAX(x,minv,maxv) ((x)<(minv)?(minv):((x)>(maxv)?(maxv):(x)))

#define YGL_COLOR_F2I(fc) ((int)(YGL_CLAMP_MIN_MAX(fc,0.f,1.f)*255))

#define GL_ODD true
#define GL_EVEN false

#ifdef M_PI
#undef M_PI
#endif
#define M_PI       3.14159265358979323846f
#define INV_PI     0.31830988618379067154f
#define INV_TWOPI  0.15915494309189533577f
#define INV_FOURPI 0.07957747154594766788f

#define R2D(rad) ((rad)*/*180.f/M_PI*/57.2957795130823208768f)
#define D2R(deg) ((deg)*/*M_PI/180.f*/0.01745329251994329577f)

#define ROUND(x) ((int)((x)+0.5f))

#define X_OF(p) ((p)[0])
#define Y_OF(p) ((p)[1])
#define Z_OF(p) ((p)[2])

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