#include "ygl.h"
#include "glcontext.h"
#include "global.h"
#include "clip.h"
#include "raster.h"
#include "buffer.h"

#include <string.h>

#include <stdio.h>

using namespace ygl;

// invalid begin type
#define YGL_NOBEGIN 0x1111
#define IS_IN_BEGIN(type) ((type)!=YGL_NOBEGIN)
//#define IS_NO_END(type) IS_IN_BEGIN(type)

#define CHECK_IS_IN_BEGIN \
	if(!IS_IN_BEGIN(glctx.begin_type))\
	{\
		glctx.last_error=GL_INVALID_OPERATION;\
		return;\
	}

#define CHECK_IS_NO_BEGIN \
	if(IS_IN_BEGIN(glctx.begin_type))\
	{\
	glctx.last_error=GL_INVALID_OPERATION;\
	return;\
	}

#define CHECK_MATRIX \
	if(!glctx.matrix_mv.peek()||!glctx.matrix_proj.peek())\
	{\
		glctx.last_error=GL_INVALID_OPERATION;\
		return;\
	}

#define MATRIX_ELEMENTS_4_HASNAN(m) \
	(isnan((m)[0])||isnan((m)[1])||isnan((m)[2])||isnan((m)[3]))
#define CHECK_MATRIX_ELEMENTS(m) \
	if(MATRIX_ELEMENTS_4_HASNAN(m)||MATRIX_ELEMENTS_4_HASNAN(m+4)||\
	MATRIX_ELEMENTS_4_HASNAN(m+8)||MATRIX_ELEMENTS_4_HASNAN(m+12))\
	{\
		glctx.last_error=GL_INVALID_VALUE;\
		return;\
	}

#define _NOT_IMPLEMENTED_(xx) printf(#xx "not implemented\n");

#define CHECK_VALID_OPERATION
#define CHECK_VALID_VALUE
#define CHECK_VALID_HANDLE
#define CHECK_SIZEI(x)

#define CHECK_ERROR

#define ASSIGN_F4(a,b) {(a)[0]=(b)[0];(a)[1]=(b)[1];(a)[2]=(b)[2];(a)[3]=(b)[3];}
#define ASSIGN_XYZW(a) {(a)[0]=x;(a)[1]=y;(a)[2]=z;(a)[3]=w;}
#define ASSIGN_RGB(c) {(c)[0]=r;(c)[1]=g;(c)[2]=b;}
#define ASSIGN_RGBA(c) {(c)[0]=r;(c)[1]=g;(c)[2]=b;(c)[3]=a;}


GLContext glctx;

GLContext::GLContext()
{	
	// viewport
	// todo: check standard specification!!!
	viewport.ox=viewport.oy=viewport.w=viewport.h=0;
	znear=0.f;zfar=1.f;

	// matrix
	cur_matrix_mode=GL_MODELVIEW;
	cur_matrix=matrix_mv.push(Matrix44());
	matrix_proj.push(Matrix44());
	


	// light
// 	struct
// 	{
// 		GLfloat ambient_scene[4];
// 		GLboolean local_viewer;
// 		GLenum color_control;
// 		GLboolean two_sided;
// 	}light_model;
// 	LightSource lights[YGL_MAX_LIGHTS];
// 	Material material_front,material_back;


	GLfloat clear_color[4];

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	enable_flags=0;
	last_error=0;

	begin_type=YGL_NOBEGIN;

	// vertex data
	cur_tex_coord.s=cur_tex_coord.t=cur_tex_coord.r=0.f;
	cur_tex_coord.q=1.f;

	cur_normal[0]=cur_normal[1]=0.f;
	cur_normal[2]=1.f;

	cur_color[0]=cur_color[1]=cur_color[2]=cur_color[3]=1.f;

	cur_secondary_color[0]=cur_secondary_color[1]=cur_secondary_color[2]=0.f;
	cur_secondary_color[3]=1.f;

	cur_edge_flag=true;// move to flags

	//cur_fog_coord=0.f;
	//cur_color_index=1.f;

	// 		line_stipple=0xffff;
	// 		line_stipple_repcnt=1;line_stipple_cnter=0;
}

void GLContext::init(GLint width, GLint height)
{
	glctx.viewport.w=width;
	glctx.viewport.h=height;
	glctx.viewport.ox=width/2;
	glctx.viewport.oy=height/2;

	// init flags
}

//////////////////////////////////////////////////////////////////////////
// helpers

// todo: perspective divide for attribs
#define CLIP_SPACE_TO_WINDOW(p) \
	__perspect_divide(p);\
	__viewport_transform(p);

// clipped verts
#define CLIP_SPACE_TO_WINDOW_2(vt) \
	CLIP_SPACE_TO_WINDOW((vt)[0].p);\
	CLIP_SPACE_TO_WINDOW((vt)[1].p);

#define CLIP_SPACE_TO_WINDOW_3(vt) \
	CLIP_SPACE_TO_WINDOW((vt)[0].p);\
	CLIP_SPACE_TO_WINDOW((vt)[1].p);\
	CLIP_SPACE_TO_WINDOW((vt)[2].p);

#define CLIP_SPACE_TO_WINDOW_4(vt) \
	CLIP_SPACE_TO_WINDOW((vt)[0].p);\
	CLIP_SPACE_TO_WINDOW((vt)[1].p);\
	CLIP_SPACE_TO_WINDOW((vt)[2].p);\
	CLIP_SPACE_TO_WINDOW((vt)[3].p);

inline void __perspect_divide(GLfloat* p)
{
	GLfloat invw=1.f/p[3];
	Assert(!isnan(invw)&&invw!=0.f);
	p[0]*=invw;
	p[1]*=invw;
	p[2]*=invw;
	//p[3]=1.f;
}
inline void __viewport_transform(GLfloat* p)
{
	// todo: cache it??

	// we ignore left,top, move pixels to (0,0)
	p[0]=(glctx.viewport.w-1)*0.5f*p[0]/*+glctx.viewport.ox*/+(glctx.viewport.w-1)*0.5f;
	p[1]=(glctx.viewport.h-1)*0.5f*p[1]/*+glctx.viewport.oy*/+(glctx.viewport.h-1)*0.5f;
	p[2]=(glctx.zfar-glctx.znear)*0.5f*p[3]+(glctx.zfar+glctx.znear)*0.5f;

	// width=480
	// mapto [0,479] or handle this using top left fill convention??
}

namespace ygl{
void glDepthRange(GLclampd n, GLclampd f)
{
	glctx.znear=YGL_CLAMP_MIN(n,0.f);
	glctx.zfar=YGL_CLAMP_MAX(f,1.f);
}
void glClear(GLbitfield mask)
{
	if(mask&GL_COLOR_BUFFER_BIT)
		buffer::clear_color(glctx.clear_color[0],glctx.clear_color[1],glctx.clear_color[2]);
}
void glClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a)
{
	ASSIGN_RGBA(glctx.clear_color);
}
void glFlush()
{
	buffer::draw(glctx.viewport.ox-glctx.viewport.w/2,
		glctx.viewport.oy-glctx.viewport.h/2,
		glctx.viewport.w,
		glctx.viewport.h);
}
void glFinish()
{
	buffer::draw(glctx.viewport.ox-glctx.viewport.w/2,
		glctx.viewport.oy-glctx.viewport.h/2,
		glctx.viewport.w,
		glctx.viewport.h);
}
void glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	CHECK_SIZEI(width)
	CHECK_SIZEI(height)
	// todo: drop it, why use ox,oy
	glctx.viewport.ox=x+width/2;
	glctx.viewport.oy=y+height/2;
	glctx.viewport.w=width;
	glctx.viewport.h=height;

	// todo: check height<=buf_height
	buffer::resize(width,height);
}

void glBegin(GLenum mode)
{
	CHECK_IS_NO_BEGIN
	//CHECK_MATRIX

	// init assemble state
	switch(mode)
	{
	case GL_POINTS:
		break;
	case GL_LINES:
		glctx.asm_state.lines.is_odd=true;
		break;
	case GL_LINE_STRIP:
		glctx.asm_state.line_strip.is_first=true;;
		break;
	case GL_LINE_LOOP:
		glctx.asm_state.line_loop.complete_cnt=-1;
		break;
	case GL_TRIANGLES:
		glctx.asm_state.tris.index=0;
		break;
	case GL_TRIANGLE_STRIP:
		break;
	case GL_TRIANGLE_FAN:
		break;
	case GL_QUADS:
		glctx.asm_state.quads.index=0;
		break;
	case GL_QUAD_STRIP:
		break;
	case GL_POLYGON:
		break;
	
	default:
		glctx.last_error=GL_INVALID_VALUE;
		return;
	}
	glctx.begin_type=mode;
}

void glEnd()
{
	CHECK_IS_IN_BEGIN

	switch(glctx.begin_type)
	{
	case GL_LINE_LOOP:
		if(glctx.asm_state.line_loop.complete_cnt>1) // at least 3 verts
		{
			Vertex clip_buf[2];
			if(clip::line(&glctx.asm_state.line_loop.prev_v,&glctx.asm_state.line_loop.first_v,clip_buf))
			{
				CLIP_SPACE_TO_WINDOW_2(clip_buf);

				// todo: smooth shading!!
				raster::line(clip_buf,clip_buf[0].col_front_pri);
			}
		}
		break;
	}

	glctx.begin_type=YGL_NOBEGIN;
}

/* vertex */
void glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	CHECK_IS_IN_BEGIN

	Vertex newv;
	ASSIGN_XYZW(newv.p);

	// mv matrix and lighting
	glctx.matrix_mv.peek()->onP(newv.p);

// 	if(YGL_IS_ENABLED(YGL_LIGHTING)) // mode == per_vertex
// 		do_lighting(newv);
// 	else
	{
		ASSIGN_F4(newv.col_front_pri,glctx.cur_color);
	}

	// projection -1,1
	glctx.matrix_proj.peek()->onP(newv.p);


	//const Vertex* vert_buf[10];
	Vertex /*vert_buf[10],*/clip_buf[10];
	GLint clip_cnt=0;
	switch(glctx.begin_type)
	{
	case GL_POINTS:
		//clip();
		break;
	case GL_LINES:
		// negate and use previous value
		if(!(glctx.asm_state.lines.is_odd=!glctx.asm_state.lines.is_odd))
			glctx.asm_state.lines.odd_v=newv;
		else
		{
			if(clip::line(&glctx.asm_state.lines.odd_v,&newv,clip_buf))
			{
				CLIP_SPACE_TO_WINDOW_2(clip_buf);

				raster::line(clip_buf,clip_buf[0].col_front_pri);
			}
		}
		break;
	case GL_LINE_STRIP:
		if(glctx.asm_state.line_strip.is_first)
		{
			glctx.asm_state.line_strip.prev_v=newv;
			glctx.asm_state.line_strip.is_first=false;
		}
		else
		{
			if(clip::line(&glctx.asm_state.line_strip.prev_v,&newv,clip_buf))
			{
				CLIP_SPACE_TO_WINDOW_2(clip_buf);

				raster::line(clip_buf,clip_buf[0].col_front_pri);
			}
		}
		break;
	case GL_LINE_LOOP:
		if(glctx.asm_state.line_loop.complete_cnt==-1) // first vert
		{
			glctx.asm_state.line_loop.first_v=newv;
			glctx.asm_state.line_loop.complete_cnt=0;
		}
		else
		{
			++glctx.asm_state.line_loop.complete_cnt;
			
			if(clip::line(&glctx.asm_state.line_loop.prev_v,&newv,clip_buf))
			{
				CLIP_SPACE_TO_WINDOW_2(clip_buf);

				raster::line(clip_buf,clip_buf[0].col_front_pri);
			}
		}
		glctx.asm_state.line_loop.prev_v=newv;
		break;
	case GL_TRIANGLES:
		///

		// 		signed_area=...;
		// 		if(!cull)

		if(++glctx.asm_state.tris.index==3)
		{
			glctx.asm_state.tris.index=0;

			// copy
			clip_buf[0]=glctx.asm_state.tris.verts[0];
			clip_buf[1]=glctx.asm_state.tris.verts[1];
			clip_buf[2]=newv;

			if(clip::polygon(clip_buf,3,&clip_cnt))
			{
				//CLIP_SPACE_TO_WINDOW_3(clip_buf);
				for(int i=0;i<clip_cnt;++i)
				{
					CLIP_SPACE_TO_WINDOW(clip_buf[i].p);
				}

				raster::scanline(clip_buf,clip_cnt);
			}
		}
		else
		{
			glctx.asm_state.tris.verts[glctx.asm_state.tris.index-1]=newv;
		}

		break;
	case GL_TRIANGLE_STRIP:
		break;
	case GL_TRIANGLE_FAN:
		break;
	case GL_QUADS:
		if(++glctx.asm_state.quads.index==4)
		{
			glctx.asm_state.quads.index=0;

			// copy
			clip_buf[0]=glctx.asm_state.tris.verts[0];
			clip_buf[1]=glctx.asm_state.tris.verts[1];
			clip_buf[2]=glctx.asm_state.tris.verts[2];
			clip_buf[3]=newv;

			// pass in clip_buf, return back in clip_buf
			if(clip::polygon(clip_buf,4,&clip_cnt))
			{
				//CLIP_SPACE_TO_WINDOW_4(clip_buf);
				for(int i=0;i<clip_cnt;++i)
				{
					CLIP_SPACE_TO_WINDOW(clip_buf[i].p);
				}

				raster::scanline(clip_buf,clip_cnt);
			}
		}
		else
		{
			glctx.asm_state.quads.verts[glctx.asm_state.quads.index-1]=newv;
		}
		break;
	case GL_QUAD_STRIP:
		break;
	case GL_POLYGON:
		break;

	default:
		glctx.last_error=GL_INVALID_VALUE;
		return;
	}
}

void glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	ASSIGN_RGBA(glctx.cur_color);
}

void glSecondaryColor3f(GLfloat r, GLfloat g, GLfloat b/*, GLfloat alpha = 0.f*/)
{
	ASSIGN_RGB(glctx.cur_secondary_color);
}

void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
	// todo: normalize it??
	glctx.cur_normal[0]=nx;
	glctx.cur_normal[1]=ny;
	glctx.cur_normal[2]=nz;
}

void glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
	_NOT_IMPLEMENTED_(glTexCoord4f)
	//glctx.cur_tex_coord
}

void glMultiTexCoord4f(GLenum tex, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
	_NOT_IMPLEMENTED_(glMultiTexCoord4f)
}

void glEdgeFlag(GLboolean flag)
{
	_NOT_IMPLEMENTED_(glEdgeFlag)
	glctx.cur_edge_flag=flag;
}

// void glRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
// {
// 	CHECK_IS_NO_BEGIN
// }

void glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
	CHECK_IS_NO_BEGIN

	glBegin(GL_QUADS);
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2); 
	glEnd();
}

/* matrix */
void glMatrixMode(GLenum mode)
{
	CHECK_IS_NO_BEGIN

	switch(mode)
	{
	case GL_MODELVIEW:
		glctx.cur_matrix=glctx.matrix_mv.peek();
		break;
	case GL_PROJECTION:
		glctx.cur_matrix=glctx.matrix_proj.peek();
		break;
	//case GL_TEXTURE:
	default: return;
	}
	glctx.cur_matrix_mode=mode;
}
void glLoadIdentity(void)
{
	CHECK_IS_NO_BEGIN

	if(glctx.cur_matrix)
	{
		GLfloat* m=glctx.cur_matrix->m;
		m[0]=1.0f;m[4]=0.0f;m[8]=0.0f;m[12]=0.0f;
		m[1]=0.0f;m[5]=1.0f;m[9]=0.0f;m[13]=0.0f;
		m[2]=0.0f;m[6]=0.0f;m[10]=1.0f;m[14]=0.0f;
		m[3]=0.0f;m[7]=0.0f;m[11]=0.0f;m[15]=1.0f;
	}
}
void glPushMatrix(void)
{
	CHECK_IS_NO_BEGIN

	switch(glctx.cur_matrix_mode)
	{
	case GL_MODELVIEW:
		if(glctx.matrix_mv.full()){glctx.last_error=GL_STACK_OVERFLOW;return;}
		glctx.cur_matrix=glctx.matrix_mv.push(*glctx.cur_matrix);
		break;
	case GL_PROJECTION:
		if(glctx.matrix_proj.full()){glctx.last_error=GL_STACK_OVERFLOW;return;}
		glctx.cur_matrix=glctx.matrix_proj.push(*glctx.cur_matrix);
		break;
		//case GL_TEXTURE:
	default: return;
	}
}
void glPopMatrix(void)
{
	CHECK_IS_NO_BEGIN

	switch(glctx.cur_matrix_mode)
	{
	case GL_MODELVIEW:
		// at least one entry
		if(glctx.matrix_mv.cnt()<=1){glctx.last_error=GL_STACK_UNDERFLOW;return;}
		// so don't modify cur_matrix??
		glctx.cur_matrix=glctx.matrix_mv.pop();
		break;
	case GL_PROJECTION:
		if(glctx.matrix_mv.cnt()<=1){glctx.last_error=GL_STACK_UNDERFLOW;return;}
		glctx.cur_matrix=glctx.matrix_proj.pop();
		break;
		//case GL_TEXTURE:
	default: return;
	}
}
void glLoadMatrixf(const GLfloat *m)
{
	CHECK_IS_NO_BEGIN

	if(glctx.cur_matrix)
		memcpy(glctx.cur_matrix->m,m,16*sizeof(GLfloat));
}
// right mul
void glMultMatrixf(const GLfloat *m)
{
	CHECK_IS_NO_BEGIN

	if(glctx.cur_matrix)
	{
		GLfloat res[16];
		GLfloat* n=glctx.cur_matrix->m;
		res[0]=n[0]*m[0]+n[4]*m[1]+n[8]*m[2]+n[12]*m[3];
		res[4]=n[0]*m[4]+n[4]*m[5]+n[8]*m[6]+n[12]*m[7];
		res[8]=n[0]*m[8]+n[4]*m[9]+n[8]*m[10]+n[12]*m[11];
		res[12]=n[0]*m[12]+n[4]*m[13]+n[8]*m[14]+n[12]*m[15];
		res[1]=n[1]*m[0]+n[5]*m[1]+n[9]*m[2]+n[13]*m[3];
		res[5]=n[1]*m[4]+n[5]*m[5]+n[9]*m[6]+n[13]*m[7];
		res[9]=n[1]*m[8]+n[5]*m[9]+n[9]*m[10]+n[13]*m[11];
		res[13]=n[1]*m[12]+n[5]*m[13]+n[9]*m[14]+n[13]*m[15];
		res[2]=n[2]*m[0]+n[6]*m[1]+n[10]*m[2]+n[14]*m[3];
		res[6]=n[2]*m[4]+n[6]*m[5]+n[10]*m[6]+n[14]*m[7];
		res[10]=n[2]*m[8]+n[6]*m[9]+n[10]*m[10]+n[14]*m[11];
		res[14]=n[2]*m[12]+n[6]*m[13]+n[10]*m[14]+n[14]*m[15];
		res[3]=n[3]*m[0]+n[7]*m[1]+n[11]*m[2]+n[15]*m[3];
		res[7]=n[3]*m[4]+n[7]*m[5]+n[11]*m[6]+n[15]*m[7];
		res[11]=n[3]*m[8]+n[7]*m[9]+n[11]*m[10]+n[15]*m[11];
		res[15]=n[3]*m[12]+n[7]*m[13]+n[11]*m[14]+n[15]*m[15];
		memcpy(n,res,sizeof(GLfloat)*16);
	}
}
void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	CHECK_IS_NO_BEGIN

	angle=D2R(angle);
	GLfloat invulen=1.f/sqrt(x*x+y*y+z*z);
	Assert(invulen!=0.f&&!isnan(invulen)&&!isinf(invulen));
	GLfloat ux=x*invulen,uy=y*invulen,uz=z*invulen;
	GLfloat cosA=cosf(angle),sinA=sinf(angle);
	GLfloat oneC=1-cosA;
	
	GLfloat m[16]={0.f};
	m[0]=ux*ux*oneC+cosA;
	m[4]=ux*uy*oneC-uz*sinA;
	m[8]=ux*uz*oneC+uy*sinA;
	m[1]=uy*ux*oneC+uz*sinA;
	m[5]=uy*uy*oneC+cosA;
	m[9]=uy*uz*oneC-ux*sinA;
	m[2]=uz*ux*oneC-uy*sinA;
	m[6]=uz*uy*oneC+ux*sinA;
	m[10]=uz*uz*oneC+cosA;
	m[15]=1.f;
	glMultMatrixf(m);

	// glctx.modleview_matrix_inv_dirty=true;
}
void glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
	CHECK_IS_NO_BEGIN

	if(glctx.cur_matrix)
	{
		GLfloat* m=glctx.cur_matrix->m;
		m[12]+=m[0]*x+m[4]*y+m[8]*z;
		m[13]+=m[1]*x+m[5]*y+m[9]*z;
		m[14]+=m[2]*x+m[6]*y+m[10]*z;
		m[15]+=m[3]*x+m[7]*y+m[11]*z;
	}
}
void glScalef(GLfloat x, GLfloat y, GLfloat z)
{
	CHECK_IS_NO_BEGIN

	if(glctx.cur_matrix)
	{
		GLfloat* m=glctx.cur_matrix->m;
		m[0]*=x;m[4]*=y;m[8]*=z;
		m[1]*=x;m[5]*=y;m[9]*=z;
		m[2]*=x;m[6]*=y;m[10]*=z;
		m[3]*=x;m[7]*=y;m[11]*=z;
	}
}

/* projection */
// right mul, must set before o2w matrix
void glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	// glDepthRange(zNear,zFar);
	GLfloat m[16]={0.f};
	m[0]=(zNear+zNear)/(right-left);
	m[5]=(zNear+zNear)/(top-bottom);
	m[8]=(left+right)/(right-left);
	m[9]=(top+bottom)/(top-bottom);
	m[10]=(zNear+zFar)/(zNear-zFar);
	m[11]=-1.f;
	m[14]=(zNear+zNear)*zFar/(zNear-zFar);

	glMultMatrixf(m);
}
void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	GLfloat m[16]={0.f};
	m[0]=2.f/(right-left);
	m[5]=2.f/(top-bottom);
	m[10]=2.f/(zNear-zFar);
	m[12]=(left+right)/(left-right);
	m[13]=(top+bottom)/(bottom-top);
	m[14]=(zNear+zFar)/(zNear-zFar);
	m[15]=1.f;

	glMultMatrixf(m);
}

}//end namespace