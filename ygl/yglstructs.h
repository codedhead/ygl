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
		// identity
		m[0]=1.0f;m[4]=0.0f;m[8]=0.0f;m[12]=0.0f;
		m[1]=0.0f;m[5]=1.0f;m[9]=0.0f;m[13]=0.0f;
		m[2]=0.0f;m[6]=0.0f;m[10]=1.0f;m[14]=0.0f;
		m[3]=0.0f;m[7]=0.0f;m[11]=0.0f;m[15]=1.0f;
	}

	GLfloat affine_det() const
	{
		return m[0]*m[5]*m[10]+m[1]*m[6]*m[8]+m[2]*m[4]*m[9]
			-m[2]*m[5]*m[8]-m[0]*m[6]*m[9]-m[1]*m[4]*m[10];
	}
	// dst!=this
	// [ A-1  -A-1 . b]
	void affine_inv(GLfloat* dstm) const // possible change
	{
		//Assert(dstm!=this->m&&is_affine());
		float det=affine_det();
		Assert(det!=0.f&&!isnan(det));
		float invDet=1.f/det;

		dstm[0]=invDet*(m[5]*m[10]-m[6]*m[9]);
		dstm[5]=invDet*(m[0]*m[10]-m[2]*m[8]);
		dstm[10]=invDet*(m[5]*m[0]-m[1]*m[4]);
		dstm[1]=-invDet*(m[1]*m[10]-m[2]*m[9]);
		dstm[2]=invDet*(m[1]*m[6]-m[2]*m[5]);
		dstm[4]=-invDet*(m[4]*m[10]-m[6]*m[8]);
		dstm[6]=-invDet*(m[0]*m[6]-m[2]*m[4]);
		dstm[8]=invDet*(m[4]*m[9]-m[5]*m[8]);
		dstm[9]=-invDet*(m[0]*m[9]-m[1]*m[8]);

		dstm[3]=dstm[7]=dstm[11]=0.0;dstm[15]=1.0;

		dstm[12]=-(dstm[0]*m[12]+dstm[4]*m[13]+dstm[8]*m[14]);
		dstm[13]=-(dstm[1]*m[12]+dstm[5]*m[13]+dstm[9]*m[14]);
		dstm[14]=-(dstm[2]*m[12]+dstm[6]*m[13]+dstm[10]*m[14]);
	}

	GLboolean is_affine() const
	{
		return m[3]==0.f&&m[7]==0.f&&m[11]==0.f&&m[15]==1.f;
	}

	void onP(GLfloat* v) const
	{
		GLfloat x=v[0],y=v[1],z=v[2],w=v[3];
		v[0] = m[0]*x + m[4]*y + m[8]*z + m[12]*w;
		v[1] = m[1]*x + m[5]*y + m[9]*z + m[13]*w;
		v[2] = m[2]*x + m[6]*y + m[10]*z + m[14]*w;
		v[3] = m[3]*x + m[7]*y + m[11]*z + m[15]*w;
	}
	void onP(const GLfloat* v,GLfloat* dst) const
	{
		GLfloat x=v[0],y=v[1],z=v[2],w=v[3];
		dst[0] = m[0]*x + m[4]*y + m[8]*z + m[12]*w;
		dst[1] = m[1]*x + m[5]*y + m[9]*z + m[13]*w;
		dst[2] = m[2]*x + m[6]*y + m[10]*z + m[14]*w;
		dst[3] = m[3]*x + m[7]*y + m[11]*z + m[15]*w;
	}

	void onV(const GLfloat* v,GLfloat* dst) const
	{
		GLfloat x=v[0],y=v[1],z=v[2];
		dst[0] = m[0]*x + m[4]*y + m[8]*z;
		dst[1] = m[1]*x + m[5]*y + m[9]*z;
		dst[2] = m[2]*x + m[6]*y + m[10]*z;;
	}

	// /////// v?=dst
	void onVwithTranspose(const GLfloat* v,GLfloat* dst) const
	{
		GLfloat x=v[0],y=v[1],z=v[2];
		dst[0] = m[0]*x + m[1]*y + m[2]*z;
		dst[1] = m[4]*x + m[5]*y + m[6]*z;
		dst[2] = m[8]*x + m[9]*y + m[10]*z;
	}

// 	void onV(GLfloat* v) const
// 	{
// 		GLfloat x=v[0],y=v[1],z=v[2],w=v[3];
// 		v[0] = m[0]*x + m[4]*y + m[8]*z + m[12]*w;
// 		v[1] = m[1]*x + m[5]*y + m[9]*z + m[13]*w;
// 		v[2] = m[2]*x + m[6]*y + m[10]*z + m[14]*w;
//  }
};

struct Vertex
{
	union
	{
//		union
//		{
		GLfloat p[4];
// 		struct
// 		{
// 			GLfloat x,y;
// 			GLuint zint; // zbuf_type
// 			GLfloat w;
// 		};
// 		}
		fixed_real p_fixed[4];
	};
	
	GLfloat col_front_pri[4]/*,col_front_sec[4]*/;
	GLfloat col_back_pri[4]/*,col_back_sec[4]*/;

	GLfloat* col_pri;

	GLfloat tex_coords[4];
	GLfloat fog_coord;
	GLboolean edge_flag;

	// todo: per pixel lighting
	//GLfloat p_eye_space[4];
	//GLfloat n_eye_space[4];
};

#endif