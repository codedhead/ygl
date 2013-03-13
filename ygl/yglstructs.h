#ifndef _YGL_STRUCTS_
#define _YGL_STRUCTS_

#include "geom.h"
#include "ygltypes.h"
using namespace ygl;

struct Matrix44
{
	GLfloat m[16];

	Matrix44()
	{
		m[0]=1.0f;m[4]=0.0f;m[8]=0.0f;m[12]=0.0f;
		m[1]=0.0f;m[5]=1.0f;m[9]=0.0f;m[13]=0.0f;
		m[2]=0.0f;m[6]=0.0f;m[10]=1.0f;m[14]=0.0f;
		m[3]=0.0f;m[7]=0.0f;m[11]=0.0f;m[15]=1.0f;
	}

	void onP(GLfloat* v) const
	{
		GLfloat x=v[0],y=v[1],z=v[2],w=v[3];
		v[0] = m[0]*x + m[4]*y + m[8]*z + m[12]*w;
		v[1] = m[1]*x + m[5]*y + m[9]*z + m[13]*w;
		v[2] = m[2]*x + m[6]*y + m[10]*z + m[14]*w;
		v[3] = m[3]*x + m[7]*y + m[11]*z + m[15]*w;
// 		float w   = m[3]*x + m[7]*y + m[11]*z + m[15];
// 		if (w != 1.) *ptrans /= w;
	}
// 	void onV(GLfloat* v) const
// 	{
// 
// 	}
// 	void onVwithTranspose(GLfloat* v) const
// 	{
// 
// 	}
};

struct Vertex
{
// 	Vertex(GLfloat x,GLfloat y,GLfloat z,GLfloat w)
// 	{
// 		p[0]=x;p[1]=y;p[2]=z;p[3]=w;
// 	}
	GLfloat p[4];//x,y,z,w;
	GLfloat col_front_pri[4],col_front_sec[4];
	GLfloat col_back_pri[4],col_back_sec[4];
};

struct VertAssocData
{
	// color
	// boundary
};

#endif