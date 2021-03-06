#ifndef _YGL_GLOBAL_
#define _YGL_GLOBAL_

#define YGL_CLAMPF(x) ((x)<0.f?0.f:((x)>1.f:1.f,(x)))
#define YGL_CLAMPD(x) ((x)<0?0:((x)>1.0:1.0,(x)))
#define YGL_CLAMP_MIN(x,minv) ((x)<(minv)?(minv):(x))
#define YGL_CLAMP_MAX(x,maxv) ((x)>(maxv)?(maxv):(x))
#define YGL_CLAMP_MIN_MAX(x,minv,maxv) ((x)<(minv)?(minv):((x)>(maxv)?(maxv):(x)))

#define YGL_COLOR_F2IF(fc) (YGL_CLAMP_MIN_MAX(fc,0.f,1.f)*255)
#define YGL_COLOR_F2I(fc) ((int)YGL_COLOR_F2IF(fc))

#define YGL_COLOR_I2F(ic) (((GLfloat)(ic))*INV255)

#define FLOAT_EQ_0 1
#define FLOAT_GT_0 2
#define FLOAT_LE_0 4
#define FLOAT_ALWAYS (FLOAT_EQ_0|FLOAT_GT_0|FLOAT_LE_0)

//#define SUB_AS_INT(a,b) (((int)(a))-((int)(b)))

#define COMPARE_UINT(a,b) ((a)==(b)?1:((a)>(b)?2:4))
inline int compare_uint32(unsigned int a,unsigned int b)
{
	return COMPARE_UINT(a,b);
}

#define BIT_MASK(a,b,mask) ( ((a) & ~(mask)) | ((b) & (mask)) )

// assume int 32
// return 0 for 0.0, 1 for positive, 2 for negative
// #define FLOAT_AS_INT(f) (*((unsigned int*)&f))
// #define INT_AS_FLOAT(i) (*((float*)&f))
// #define SIGN_OF_FLOAT(f) (\
// 	((   FLOAT_AS_INT(f) & 0x7FFFFFFF)==0)?1:(\
// 		2 << ( FLOAT_AS_INT(f) & 0x80000000) >> 31 )\
// 	))
// inline int SIGN_OF_FLOAT(float f)
// {
// 	return (
// 		((   FLOAT_AS_INT(f) & 0x7FFFFFFF)==0)?1:
// 		( 2 << (( FLOAT_AS_INT(f) & 0x80000000) >> 31) )
// 		);
// }
// inline int SIGN_OF_DOUBLE(double f)
// {
// 	return (
// 		( f==0.0 )?1:
// 		( 2 << ( *((unsigned long long*)&f) >> 63) )
// 		);
// }
// inline int fast_sign_of(float f)
// {
// 	if (((int&)f & 0x7FFFFFFF)==0) return 0; // test exponent & mantissa bits: is input zero?
// 	return 1 << (signed((int&)f & 0x80000000) >> 31 );
// }

// todo: SSE assign?
#define ASSIGN_V4(a,b) {(a)[0]=(b)[0];(a)[1]=(b)[1];(a)[2]=(b)[2];(a)[3]=(b)[3];}
#define ASSIGN_V3_WITH(v,a,b,c) {(v)[0]=a;(v)[1]=b;(v)[2]=c;}
#define ASSIGN_V4_WITH(v,a,b,c,d) {(v)[0]=a;(v)[1]=b;(v)[2]=c;(v)[3]=d;}

#define YGL_V4_LIST_COMPONENTS(v) (v)[0],(v)[1],(v)[2],(v)[3]

#define FLOAT_AS_UINT(f) (*((unsigned int*)(&(f))))
#define DECODE_Z(z) ((double)FLOAT_AS_UINT(z))

// #define GL_ODD true
// #define GL_EVEN false

#define _NOT_IMPLEMENTED_(xx) printf(#xx "not implemented\n");

#define DO_TRIANGULATION

#ifdef M_PI
#undef M_PI
#endif
#define M_PI       3.14159265358979323846f
#define INV_PI     0.31830988618379067154f
#define INV_TWOPI  0.15915494309189533577f
#define INV_FOURPI 0.07957747154594766788f

#define INV255	   0.00392156862745098039f

#define Ln2		   0.69314718055994530942f
#define INV_Ln2	   1.44269504088896340736f

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


#define COMPUTE_YMIN(y) (ceil(y))
#define COMPUTE_YMAX(y) (ceil(y))
#define COMPUTE_XMIN(x) (ceil(x))
#define COMPUTE_XMAX(x) (ceil(x))

//#define IMAGE_PLANE_LERP_TO_WORLD_LERP(u,z1,z2) ((u)*(z1)/((z2)+(u)*((z1)-(z2))))

#define LERP_COMPONENT_4(dst,comp,va,vb,f) \
	(dst)->##comp##[0]=lerp((va)->##comp##[0],(vb)->##comp##[0],f);\
	(dst)->##comp##[1]=lerp((va)->##comp##[1],(vb)->##comp##[1],f);\
	(dst)->##comp##[2]=lerp((va)->##comp##[2],(vb)->##comp##[2],f);\
	(dst)->##comp##[3]=lerp((va)->##comp##[3],(vb)->##comp##[3],f);


// fixed-point real number
#define FIXED_BITS (4)
#define FIXED_F (1<<FIXED_BITS)
#define FIXED_FF (16.f)
#define FIXED_MASK (0xf)
//#define FIXED_FF ((GLfloat)FIXED_F)
typedef long fixed_real;


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
inline long lerpl(long a,long b,float f)
{
	return (1.f-f)*a+f*b;
}

inline void min_max(long X1,long X2,long X3,long& min_x,long& max_x)
{
	if(X1>X2)
	{
		if(X2>X3)
		{
			max_x=X1;min_x=X3;
		}
		else
		{
			min_x=X2;
			if(X1>X3)
			{
				max_x=X1;
			}
			else
			{
				max_x=X3;
			}
		}
	}
	else
	{
		if(X3>X2)
		{
			min_x=X1;
			max_x=X3;
		}
		else
		{
			max_x=X2;
			if(X1>X3)
			{
				min_x=X3;
			}
			else
			{
				min_x=X1;
			}
		}
	}
}

// inline int SIGN_OF_INT32(int i)
// {
// 	return (
// 		( i==0 )?1:( 2 << ( i>>31 ) )
// 		);
// }

// credit: http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
inline unsigned int log2(unsigned int v) // x must be power of 2
{
	static const unsigned int b[] = {0xAAAAAAAA, 0xCCCCCCCC, 0xF0F0F0F0, 
		0xFF00FF00, 0xFFFF0000};
	register unsigned int r = (v & b[0]) != 0;
	for (int i = 4; i > 0; i--) // unroll for speed...
	{
		r |= ((v & b[i]) != 0) << i;
	}
	return r;
}

#define MAX(a,b) ((a)>(b)?(a):(b))
#ifdef max
#undef max
#endif
inline int max(int a,int b){return a>b?a:b;}
inline float max(float a,float b){return a>b?a:b;}

#endif