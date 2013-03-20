#include "ygl.h"
#include "glcontext.h"
#include "global.h"
#include "clip.h"
#include "raster.h"
#include "buffer.h"

#include <string.h>
#include <stdlib.h>
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

#define CHECK_VALID_OPERATION
#define CHECK_VALID_VALUE
#define CHECK_VALID_HANDLE
#define CHECK_SIZEI(x)

#define CHECK_ERROR

// #define GET_RASTER_OPTION \
// 	((glctx.smooth?raster::ETRecSmooth:0)|(glctx.texture?raster::ETRecTex:0))
#define GET_RASTER_OPTION \
	(glctx.shade_model==YGL_SMOOTH?raster::ETRecSmooth:0)|\
	(YGL_IS_ENABLED(YGL_TEXTURE_2D)?raster::ETRecTex:0)|\
	(YGL_IS_ENABLED(YGL_FOG)?raster::ETRecFog:0)


// Caching
#define SET_MVINV_CACHED_INVALID if(glctx.cur_matrix_mode==GL_MODELVIEW)glctx.cached_mv_inverse_valid=false;

GLContext glctx;

GLContext::GLContext()
{	
	// viewport
	// todo: check standard specification!!! about viewport?
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


	// clear value
	ASSIGN_F4_WITH(clear_color,0.f,0.f,0.f,0.f);
	clear_depth=YGL_DEPTH_BUFFER_MAX_RES;//*1.f;

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	enable_flags=0;
	last_error=0;

	begin_type=YGL_NOBEGIN;

	// vertex data
// 	cur_tex_coord.s=cur_tex_coord.t=cur_tex_coord.r=0.f;
// 	cur_tex_coord.q=1.f;
	ASSIGN_F4_WITH(cur_tex_coord,0.f,0.f,0.f,1.f);

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

	enable_flags=0;

	cull_face_mode=YGL_BACK;
	front_face_mode=YGL_CCW;
	shade_model=YGL_FLAT;

	color_material_face=YGL_FRONT_AND_BACK;
	color_material_mode=GL_AMBIENT_AND_DIFFUSE;

	// light 0
	ASSIGN_F4_WITH(lights[0].diffuse,1.f,1.f,1.f,1.f);
	ASSIGN_F4_WITH(lights[0].specular,1.f,1.f,1.f,1.f);
	
	ASSIGN_F4_WITH(light_model.ambient_scene,0.2f,0.2f,0.2f,1.f);
	light_model.local_viewer=false;
	light_model.two_sided=false;
	// color control

	//caching
	cached_mv_inverse_valid=true; // identity

	//depth test
	buffer::z_test_mask=FLOAT_ALWAYS;
	depth_test_mask=FLOAT_LE_0;//GL_LESS;
	YGL_SET_ENABLED_FLAG(buffer_write_mask,YGL_DEPTH_WRITE);
	buffer::z_write_mask=true;

	// texture
	tex_names.set(0);// reserve 0??
	memset(tex_objs,0,sizeof(tex_objs));
	// point to 0
	tex_targets[YGL_TEX_TARGET_1D]=tex_objs[0];
	tex_targets[YGL_TEX_TARGET_2D]=tex_objs[0];
	tex_env_mode=GL_MODULATE;
	ASSIGN_F4_WITH(tex_env_color,0.f,0.f,0.f,0.f);

	// fog
	fog_mode=GL_EXP;
	// fog_source=GL_FRAGMENT_DEPTH;
	fog_density=fog_end=1.f;fog_start=0.f;
	ASSIGN_F4_WITH(fog_color,0.f,0.f,0.f,0.f);
	cached_fog_e_s=1.f;
}

// void GLContext::init(GLint width, GLint height)
// {
// 	glctx.viewport.w=width;
// 	glctx.viewport.h=height;
// 	glctx.viewport.ox=width/2;
// 	glctx.viewport.oy=height/2;
// 
// 	// init flags
// }

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
	// leave me alone??
	// invw used in perspective interpolation
	p[3]=invw;
}
inline void __viewport_transform(GLfloat* p)
{
	// todo: cache it??

	// we ignore left,top, move pixels to (0,0)
	p[0]=(glctx.viewport.w-1)*0.5f*p[0]/*+glctx.viewport.ox*/+(glctx.viewport.w-1)*0.5f;
	p[1]=(glctx.viewport.h-1)*0.5f*p[1]/*+glctx.viewport.oy*/+(glctx.viewport.h-1)*0.5f;
	// map to depth range
	// we might not be able to get a 32-bit precision, since we store znear,zfar,p[3] in float
	// wrong, and we store depth_buffer as float
	// clamp 0,1
// 	GLdouble z_in_double=(YGL_DEPTH_BUFFER_MAX_RES)*((glctx.zfar-glctx.znear)*0.5f*Z_OF(p)+(glctx.zfar+glctx.znear)*0.5f);
// 	// hack, use float to store int
// 	FLOAT_AS_INT(Z_OF(p))=(GLuint)z_in_double;
	
	// 0,1
	Z_OF(p)=((glctx.zfar-glctx.znear)*0.5f*Z_OF(p)+(glctx.zfar+glctx.znear)*0.5f);



	// width=480
	// mapto [0,479] or handle this using top left fill convention??
}

namespace ygl{

inline GLfloat signed_area(Vertex* verts,int vcnt)
{
	if(vcnt<3) return 0.f;
	GLfloat ret=0.f;
	Vertex* pv=verts;
	for(int i=0;i<vcnt-1;++i,++pv)
		ret+= X_OF(verts[i].p)*Y_OF(verts[i+1].p)-
		X_OF(verts[i+1].p)*Y_OF(verts[i].p);
	ret+= X_OF(verts[vcnt-1].p)*Y_OF(verts[0].p)-
		X_OF(verts[0].p)*Y_OF(verts[vcnt-1].p);
	return glctx.front_face_mode==YGL_CCW?.5f*ret:-.5f*ret;
}

#define WHAT_FACING(clip_buf,clip_cnt) \
	(signed_area(clip_buf,clip_cnt)>0.f?YGL_FRONT:YGL_BACK)

#define DO_CULL_AND_SCANLINE \
	if(!YGL_IS_ENABLED(YGL_CULL_FACE)||\
		(WHAT_FACING(clip_buf,clip_cnt)&glctx.cull_face_mode)==0)\
	raster::scanline(clip_buf,clip_cnt,GET_RASTER_OPTION,flat_color,glctx.tex_targets[YGL_TEX_TARGET_2D]);

#define PREPARE_FLATCOLOR \
	flat_color[0]=YGL_COLOR_F2I(newv.col_front_pri[0]);\
	flat_color[1]=YGL_COLOR_F2I(newv.col_front_pri[1]);\
	flat_color[2]=YGL_COLOR_F2I(newv.col_front_pri[2]);

#define POLYGON_CLIP_AND_RASTERIZE(_vert_cnt_) \
	PREPARE_FLATCOLOR;\
	if(clip::polygon(clip_buf,_vert_cnt_,&clip_cnt))\
	{\
		for(int i=0;i<clip_cnt;++i)\
		{\
			CLIP_SPACE_TO_WINDOW(clip_buf[i].p);\
		}\
		DO_CULL_AND_SCANLINE;\
	}

#define LINE_RASTERIZE \
	{if(glctx.shade_model==YGL_SMOOTH)\
		raster::line_smooth(clip_buf);\
	else\
		raster::line(clip_buf);}


void glEnable(GLenum cap)
{
	CHECK_VALID_VALUE

	switch(cap)
	{
	case GL_LIGHTING:
		YGL_SET_ENABLED_FLAG(glctx.enable_flags,YGL_LIGHTING);
		break;
	case GL_COLOR_MATERIAL:
		YGL_SET_ENABLED_FLAG(glctx.enable_flags,YGL_COLOR_MATERIAL);
		break;
	case GL_LIGHT0:
	case GL_LIGHT1:
	case GL_LIGHT2:
	case GL_LIGHT3:
	case GL_LIGHT4:
	case GL_LIGHT5:
	case GL_LIGHT6:
	case GL_LIGHT7:
		YGL_SET_ENABLED_FLAG(glctx.lighti_enable,cap-GL_LIGHT0);
		break;
	case GL_CULL_FACE:
		YGL_SET_ENABLED_FLAG(glctx.enable_flags,YGL_CULL_FACE);
		break;
	case GL_DEPTH_TEST:
		YGL_SET_ENABLED_FLAG(glctx.enable_flags,YGL_DEPTH_TEST);
		buffer::z_test_mask=glctx.depth_test_mask;
		buffer::z_write_mask=YGL_IS_FLAG_ENABLED(glctx.buffer_write_mask,YGL_DEPTH_WRITE);
		break;
	case GL_NORMALIZE:
		YGL_SET_ENABLED_FLAG(glctx.enable_flags,YGL_NORMALIZE);
		break;
	case GL_TEXTURE_2D:
		YGL_SET_ENABLED_FLAG(glctx.enable_flags,YGL_TEXTURE_2D);
		break;
	}
	
}
void glDisable(GLenum cap)
{
	CHECK_VALID_VALUE

	switch(cap)
	{
	case GL_LIGHTING:
		YGL_CLEAR_ENABLED_FLAG(glctx.enable_flags,YGL_LIGHTING);
		break;
	case GL_COLOR_MATERIAL:
		YGL_CLEAR_ENABLED_FLAG(glctx.enable_flags,YGL_COLOR_MATERIAL);
		break;
	case GL_LIGHT0:
	case GL_LIGHT1:
	case GL_LIGHT2:
	case GL_LIGHT3:
	case GL_LIGHT4:
	case GL_LIGHT5:
	case GL_LIGHT6:
	case GL_LIGHT7:
		YGL_CLEAR_ENABLED_FLAG(glctx.lighti_enable,cap-GL_LIGHT0);
		break;
	case GL_CULL_FACE:
		YGL_CLEAR_ENABLED_FLAG(glctx.enable_flags,YGL_CULL_FACE);
		break;
	case GL_DEPTH_TEST:
		YGL_CLEAR_ENABLED_FLAG(glctx.enable_flags,YGL_DEPTH_TEST);
		buffer::z_test_mask=FLOAT_ALWAYS;
		buffer::z_write_mask=false; // write not allowed
		break;
	case GL_NORMALIZE:
		YGL_CLEAR_ENABLED_FLAG(glctx.enable_flags,YGL_NORMALIZE);
		break;
	case GL_TEXTURE_2D:
		YGL_CLEAR_ENABLED_FLAG(glctx.enable_flags,YGL_TEXTURE_2D);
		break;
	}

}
void glDepthFunc(GLenum func)
{
	switch(func)
	{
	case GL_NEVER:
		glctx.depth_test_mask=0;
		break;
	case GL_ALWAYS:
		glctx.depth_test_mask=(FLOAT_EQ_0|FLOAT_LE_0|FLOAT_GT_0);
		break;
	case GL_LESS:
		glctx.depth_test_mask=(FLOAT_LE_0);
		break;
	case GL_LEQUAL:
		glctx.depth_test_mask=(FLOAT_EQ_0|FLOAT_LE_0);
		break;
	case GL_EQUAL:
		glctx.depth_test_mask=(FLOAT_EQ_0);
		break;
	case GL_GREATER:
		glctx.depth_test_mask=(FLOAT_GT_0);
		break;
	case GL_NOTEQUAL:
		glctx.depth_test_mask=(FLOAT_LE_0|FLOAT_GT_0);
		break;
	case GL_GEQUAL:
		glctx.depth_test_mask=(FLOAT_EQ_0|FLOAT_GT_0);
		break;
	}
	buffer::z_test_mask=(YGL_IS_ENABLED(YGL_DEPTH_TEST)?glctx.depth_test_mask:FLOAT_ALWAYS);
}
void glDepthMask(GLboolean flag)
{
	if(flag)
	{
		YGL_SET_ENABLED_FLAG(glctx.buffer_write_mask,YGL_DEPTH_WRITE);
	}
	else
	{
		YGL_CLEAR_ENABLED_FLAG(glctx.buffer_write_mask,YGL_DEPTH_WRITE);
	}

	buffer::z_write_mask=(YGL_IS_ENABLED(YGL_DEPTH_TEST)?flag:false);
	//_NOT_IMPLEMENTED_(glDepthMask);
}
void glFrontFace(GLenum mode)
{
	CHECK_VALID_VALUE
	glctx.front_face_mode=(mode==GL_CCW?YGL_CCW:YGL_CW);
}
void glCullFace(GLenum mode)
{
	CHECK_VALID_VALUE
	glctx.cull_face_mode=
		(mode==GL_FRONT?YGL_FRONT:
		(mode==GL_BACK?YGL_BACK:YGL_FRONT_AND_BACK));
}
void glColorMaterial(GLenum face, GLenum mode)
{
	glctx.color_material_face=(face==GL_FRONT?YGL_FRONT:
		(face==GL_BACK?YGL_BACK:YGL_FRONT_AND_BACK));
	glctx.color_material_mode=mode;
}
void glShadeModel(GLenum mode)
{
	CHECK_VALID_VALUE
	glctx.shade_model=(mode==GL_SMOOTH?YGL_SMOOTH:YGL_FLAT);
}
void glDepthRange(GLclampd n, GLclampd f)
{
	// n can > f
	glctx.znear=YGL_CLAMP_MIN_MAX(n,0.f,1.f);
	glctx.zfar=YGL_CLAMP_MIN_MAX(f,0.f,1.f);
}
void glClear(GLbitfield mask)
{
	if(mask&GL_COLOR_BUFFER_BIT)
		buffer::clear_color(glctx.clear_color[0],glctx.clear_color[1],glctx.clear_color[2]);
	if(mask&GL_DEPTH_BUFFER_BIT)
		buffer::clear_depth(glctx.clear_depth);
}
void glClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a)
{
	ASSIGN_F4_WITH(glctx.clear_color,r,g,b,a);
}
void glClearDepth(GLclampd depth)
{
	glctx.clear_depth=(YGL_DEPTH_BUFFER_MAX_RES)*YGL_CLAMP_MIN_MAX(depth,0.,1.);
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

				LINE_RASTERIZE;
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
	ASSIGN_F4_WITH(newv.p,x,y,z,w);

	// mv matrix and lighting
	Matrix44* mvmat=glctx.matrix_mv.peek();
	mvmat->onP(newv.p);
	newv.fog_coord=abs(Z_OF(newv.p));

	if(YGL_IS_ENABLED(YGL_LIGHTING)&&YGL_IS_POLYGON) // mode == per_vertex??
	{
		// dont calc normal everytime?
		if(!glctx.cached_mv_inverse_valid)
		{
			mvmat->affine_inv(glctx.cached_mv_inverse.m);
		}
		Normal nrm_in_eye_system;
		// no rescaling if matrix is scaled
		glctx.cached_mv_inverse.onVwithTranspose(&glctx.cur_normal.x,&nrm_in_eye_system.x);
		do_lighting(&newv,nrm_in_eye_system);
	}
	else
	{
		ASSIGN_F4(newv.col_front_pri,glctx.cur_color);
	}

	// tex coord
	ASSIGN_F4(newv.tex_coords,glctx.cur_tex_coord);

	// projection -1,1
	glctx.matrix_proj.peek()->onP(newv.p);

	// if gen_s,gen_t enable, gen tex coords

	//const Vertex* vert_buf[10];
	Vertex /*vert_buf[10],*/clip_buf[10];
	GLint clip_cnt=0;
	//might use in flat shading
	GLubyte flat_color[4]={255,255,255,255};

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

				LINE_RASTERIZE;
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

				LINE_RASTERIZE;
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
			if(++glctx.asm_state.line_loop.complete_cnt>2)
				glctx.asm_state.line_loop.complete_cnt=2;
			
			if(clip::line(&glctx.asm_state.line_loop.prev_v,&newv,clip_buf))
			{
				CLIP_SPACE_TO_WINDOW_2(clip_buf);

				LINE_RASTERIZE;
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

			POLYGON_CLIP_AND_RASTERIZE(3);
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

			POLYGON_CLIP_AND_RASTERIZE(4);
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
	ASSIGN_F4_WITH(glctx.cur_color,r,g,b,a);
	if(YGL_IS_ENABLED(YGL_COLOR_MATERIAL))
	{
		GLboolean has_front=(glctx.color_material_face==GL_FRONT||glctx.color_material_face==GL_FRONT_AND_BACK),
			has_back=(glctx.color_material_face==GL_BACK||glctx.color_material_face==GL_FRONT_AND_BACK);
		switch(glctx.color_material_mode)
		{
		case GL_AMBIENT:
			if(has_front)
			{
				ASSIGN_F4_WITH(glctx.material_front.ambient,r,g,b,a);
			}
			if(has_back)
			{
				ASSIGN_F4_WITH(glctx.material_back.ambient,r,g,b,a);
			}
			break;
		case GL_DIFFUSE:
			if(has_front)
			{
				ASSIGN_F4_WITH(glctx.material_front.diffuse,r,g,b,a);
			}
			if(has_back)
			{
				ASSIGN_F4_WITH(glctx.material_back.diffuse,r,g,b,a);
			}
			break;
		case GL_SPECULAR:
			if(has_front)
			{
				ASSIGN_F4_WITH(glctx.material_front.specular,r,g,b,a);
			}
			if(has_back)
			{
				ASSIGN_F4_WITH(glctx.material_back.specular,r,g,b,a);
			}
			break;
		case GL_EMISSION:
			if(has_front)
			{
				ASSIGN_F4_WITH(glctx.material_front.emission,r,g,b,a);
			}
			if(has_back)
			{
				ASSIGN_F4_WITH(glctx.material_back.emission,r,g,b,a);
			}
			break;
		case GL_AMBIENT_AND_DIFFUSE:
			if(has_front)
			{
				ASSIGN_F4_WITH(glctx.material_front.ambient,r,g,b,a);
				ASSIGN_F4_WITH(glctx.material_front.diffuse,r,g,b,a);
			}
			if(has_back)
			{
				ASSIGN_F4_WITH(glctx.material_back.ambient,r,g,b,a);
				ASSIGN_F4_WITH(glctx.material_front.diffuse,r,g,b,a);
			}
			break;
		}
	}
}

void glSecondaryColor3f(GLfloat r, GLfloat g, GLfloat b/*, GLfloat alpha = 0.f*/)
{
	ASSIGN_F3_WITH(glctx.cur_secondary_color,r,g,b);
}

void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
	// todo: normalize it??
// 	glctx.cur_normal[0]=nx;
// 	glctx.cur_normal[1]=ny;
// 	glctx.cur_normal[2]=nz;
	if(YGL_IS_ENABLED(YGL_NORMALIZE))
		glctx.cur_normal.set_and_normalize(nx,ny,nz);
	else
		glctx.cur_normal.set(nx,ny,nz);
}

void glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
	ASSIGN_F4_WITH(glctx.cur_tex_coord,s,t,r,q);
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


/* lighting */
void glMaterialf(GLenum face, GLenum pname, GLfloat param)
{
	CHECK_VALID_VALUE

	if(pname==GL_SHININESS)
	{
		// [0,128]
		switch(face)
		{
		case GL_BACK:
			glctx.material_back.shininess=param;
			break;
		case GL_FRONT_AND_BACK:
			glctx.material_back.shininess=param;
		case GL_FRONT:
			glctx.material_front.shininess=param;
			break;
		}
	}

}
void glMaterialfv(GLenum face, GLenum pname, const GLfloat *params)
{
	CHECK_VALID_VALUE
	GLboolean has_front=(face==GL_FRONT||face==GL_FRONT_AND_BACK),
		has_back=(face==GL_BACK||face==GL_FRONT_AND_BACK);
	switch(pname)
	{
	case GL_AMBIENT:
		if(has_front)
		{
			ASSIGN_F4(glctx.material_front.ambient,params);
		}
		if(has_back)
		{
			ASSIGN_F4(glctx.material_back.ambient,params);
		}
		break;
	case GL_DIFFUSE:
		if(has_front)
		{
			ASSIGN_F4(glctx.material_front.diffuse,params);
		}
		if(has_back)
		{
			ASSIGN_F4(glctx.material_back.diffuse,params);
		}
		break;
	case GL_SPECULAR:
		if(has_front)
		{
			ASSIGN_F4(glctx.material_front.specular,params);
		}
		if(has_back)
		{
			ASSIGN_F4(glctx.material_back.specular,params);
		}
		break;
	case GL_EMISSION:
		if(has_front)
		{
			ASSIGN_F4(glctx.material_front.emission,params);
		}
		if(has_back)
		{
			ASSIGN_F4(glctx.material_back.emission,params);
		}
		break;
	case GL_AMBIENT_AND_DIFFUSE:
		if(has_front)
		{
			ASSIGN_F4(glctx.material_front.ambient,params);
			ASSIGN_F4(glctx.material_front.diffuse,params);
		}
		if(has_back)
		{
			ASSIGN_F4(glctx.material_back.ambient,params);
			ASSIGN_F4(glctx.material_front.diffuse,params);
		}
		break;
	}
}
void glLightf(GLenum light, GLenum pname, GLfloat param)
{
	CHECK_VALID_VALUE
	// light < YGL_MAX_LIGHS
	LightSource* li=glctx.lights+(light-GL_LIGHT0);
	switch(pname)
	{
	case GL_SPOT_EXPONENT:
		// [0,128]
		li->spot_exponent=param;
		break;
	case GL_SPOT_CUTOFF:
		// [0,90],180
		if(param==180.f)
			li->spot_cutoff_cos=-1.f;
		else
			li->spot_cutoff_cos=cosf(param);
		break;
	case GL_CONSTANT_ATTENUATION:
		li->constant_attenuation=param;
		break;
	case GL_LINEAR_ATTENUATION:
		li->linear_attenuation=param;
		break;
	case GL_QUADRATIC_ATTENUATION:
		li->quadratic_attenuation=param;
		break;
	}
}
void glLightfv(GLenum light, GLenum pname, const GLfloat *params)
{
	CHECK_VALID_VALUE
	// light < YGL_MAX_LIGHS
	LightSource* li=glctx.lights+(light-GL_LIGHT0);
	switch(pname)
	{ 
	case GL_AMBIENT:
		ASSIGN_F4(li->ambient,params);
		break;
	case GL_DIFFUSE:
		ASSIGN_F4(li->diffuse,params);
		break;
	case GL_SPECULAR:
		ASSIGN_F4(li->specular,params);
		break;
	case GL_POSITION:
		// may be direction
		glctx.matrix_mv.peek()->onP(params,li->position);
		if(W_OF(li->position)==0.f) Vector::normalize(li->position);
		break;
	case GL_SPOT_DIRECTION:
		glctx.matrix_mv.peek()->onV(params,&li->spot_direction.x);
		li->spot_direction.normalize();
		break;
	
	case GL_SPOT_EXPONENT:
		// [0,128]
		li->spot_exponent=*params;
		break;
	case GL_SPOT_CUTOFF:
		// [0,90],180
		if(*params==180.f)
			li->spot_cutoff_cos=-1.f;
		else
			li->spot_cutoff_cos=cosf(*params);
		break;
	case GL_CONSTANT_ATTENUATION:
		li->constant_attenuation=*params;
		break;
	case GL_LINEAR_ATTENUATION:
		li->linear_attenuation=*params;
		break;
	case GL_QUADRATIC_ATTENUATION:
		li->quadratic_attenuation=*params;
		break;
	}
}
void glLightModeli(GLenum pname, GLint param)
{
	switch(pname)
	{
	case GL_LIGHT_MODEL_LOCAL_VIEWER:
		glctx.light_model.local_viewer=param;
		break;
	case GL_LIGHT_MODEL_TWO_SIDE:
		glctx.light_model.two_sided=param;
		break;
	}
}
void glLightModelfv(GLenum pname, const GLfloat *params)
{
	switch(pname)
	{
	case GL_LIGHT_MODEL_LOCAL_VIEWER:
		glctx.light_model.local_viewer=*params;
		break;
	case GL_LIGHT_MODEL_TWO_SIDE:
		glctx.light_model.two_sided=*params;
		break;
	case GL_LIGHT_MODEL_AMBIENT:
		ASSIGN_F4(glctx.light_model.ambient_scene,params);
		break;
	}
}

/* texture */

void glGenTextures(GLsizei n, GLuint *textures)
{
	*textures=1;
	//glctx.tex_names.lowest0(n,textures);
}
void glDeleteTextures(GLsizei n, const GLuint *textures)
{
	TextureObject* obj;
	while(n--)
	{
		if(textures[n]&&textures[n]<YGL_TEXTURE_NAMES_MAX)
		{
			obj=glctx.tex_objs[textures[n]];
			if(obj)
			{
				if(obj==glctx.tex_targets[obj->dimension-GL_TEXTURE_1D])
				{
					// default
					glctx.tex_targets[obj->dimension-GL_TEXTURE_1D]=glctx.tex_objs[0];
				}
				delete obj;
			}
			//glctx.tex_names.clear(textures[n]);
		}
	}
}
void glBindTexture(GLenum target, GLuint texture)
{
	CHECK_VALID_VALUE

	//glctx.tex_names.set(texture);
	if(!glctx.tex_objs[texture])
	{
		// todo: factory
		glctx.tex_objs[texture]=new TextureObject;
		glctx.tex_objs[texture]->dimension=target;
	}
	if(glctx.tex_objs[texture]->dimension!=target)
	{
		glctx.last_error=GL_INVALID_OPERATION;
		return;
	}
	glctx.tex_targets[target-GL_TEXTURE_1D]=glctx.tex_objs[texture];
}
void glTexParameteri(GLenum target, GLenum pname, GLint param)
{
	CHECK_VALID_VALUE
	TextureObject* targetobj=glctx.tex_targets[target-GL_TEXTURE_1D];
	switch(pname)
	{
	case GL_TEXTURE_MIN_FILTER:
		targetobj->filter_min=param;
		break;
	case GL_TEXTURE_MAG_FILTER:
		// only GL_NEAREST and GL_LINEAR
		targetobj->filter_mag=param;
		break;
	case GL_TEXTURE_WRAP_S:
		targetobj->wrap_s=param;
		break;
	case GL_TEXTURE_WRAP_T:
		targetobj->wrap_t=param;
		break;
	}

}
void glTexParameterfv(GLenum target, GLenum pname, const GLfloat *params)
{
	CHECK_VALID_VALUE
	TextureObject* targetobj=glctx.tex_targets[target-GL_TEXTURE_1D];
	switch(pname)
	{
	//case GL_TEXTURE_PRIORITY:
	//	break;
	case GL_TEXTURE_BORDER_COLOR:
		{
			ASSIGN_F4(targetobj->border_color,params);
		}
		break;
	}
}

void glTexEnvi(GLenum target, GLenum pname, GLint param)
{
	//GL_TEXTURE_ENV
	CHECK_VALID_VALUE

	if(pname==GL_TEXTURE_ENV_MODE)
	{
		glctx.tex_env_mode=param;
	}
}
void glTexEnvfv(GLenum target, GLenum pname, GLfloat* params)
{
	CHECK_VALID_VALUE

	if(pname==GL_TEXTURE_ENV_COLOR)
	{
		ASSIGN_F4(glctx.tex_env_color,params);
	}
}
// void glTexGeni(GLenum coord, GLenum pname, GLint param)
// {
// 	CHECK_VALID_VALUE
// 
// 	glctx.tex_gen_mode[coord-GL_S]=param;
// }
void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
	// format only support RGB, RGBA
	// type only support GL_UNSIGNED_BYTE, GL_FLOAT
	// internal data force as RGBA, GL_UNSIGNED_BYTE
	CHECK_VALID_VALUE
	// YGL_MAX_TEXTURE_LEVEL
	
	TexImage* img=glctx.tex_targets[YGL_TEX_TARGET_2D]->mipmaps+level;
	img->border=border;
	img->width=width;
	img->height=height;

	if(img->pixels) free(img->pixels);
	img->pixels=(GLubyte*)malloc(4*width*height);
	GLubyte* p=img->pixels;
	// add border
	if(type==GL_FLOAT)
	{
		GLfloat* q=(GLfloat*)pixels;
		for(int y=0;y<height;++y)
			for(int x=0;x<width;++x)
			{
				*p++=YGL_CLAMP_MIN_MAX(*q,0.f,1.f)*255;++q;
				*p++=YGL_CLAMP_MIN_MAX(*q,0.f,1.f)*255;++q;
				*p++=YGL_CLAMP_MIN_MAX(*q,0.f,1.f)*255;++q;
				if(type==GL_RGBA)
				{
					*p++=YGL_CLAMP_MIN_MAX(*q,0.f,1.f)*255;++q;
				}
				else *p++=255;
			}
	}
	else if(type==GL_UNSIGNED_BYTE)
	{
		GLfloat* q=(GLfloat*)pixels;
		for(int y=0;y<height;++y)
			for(int x=0;x<width;++x)
			{
				*p++=*q++;*p++=*q++;*p++=*q++;
				*p++=(type==GL_RGBA?*q++:255);
			}
	}
}
/* fog */
void glFogi(GLenum pname, GLint param)
{
	CHECK_VALID_VALUE

	switch(param)
	{
	case GL_FOG_MODE:
		glctx.fog_mode=param;
		break;
	}
}
void glFogf(GLenum pname, GLfloat param)
{
	CHECK_VALID_VALUE
	switch(pname)
	{
	case GL_FOG_DENSITY:
		glctx.fog_density=param;
		break;
	case GL_FOG_START:
		glctx.fog_start=param;
		glctx.cached_fog_e_s=1.f/(glctx.fog_end-glctx.fog_start);
		break;
	case GL_FOG_END:
		glctx.fog_end=param;
		glctx.cached_fog_e_s=1.f/(glctx.fog_end-glctx.fog_start);
		break;
	}

}
void glFogfv(GLenum pname, const GLfloat *params)
{
	CHECK_VALID_VALUE

	switch(pname)
	{
	case GL_FOG_DENSITY:
		glctx.fog_density=*params;
		break;
	case GL_FOG_START:
		glctx.fog_start=*params;
		glctx.cached_fog_e_s=1.f/(glctx.fog_end-glctx.fog_start);
		break;
	case GL_FOG_END:
		glctx.fog_end=*params;
		glctx.cached_fog_e_s=1.f/(glctx.fog_end-glctx.fog_start);
		break;
	case GL_FOG_COLOR:
		ASSIGN_F4(glctx.fog_color,params);
		break;
	}

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

		if(glctx.cur_matrix_mode==GL_MODELVIEW)
		{
			memset(glctx.cached_mv_inverse.m,0,sizeof(GLfloat)*16);
			glctx.cached_mv_inverse.m[0]=
				glctx.cached_mv_inverse.m[5]=
				glctx.cached_mv_inverse.m[10]=
				glctx.cached_mv_inverse.m[15]=1.0f;
			glctx.cached_mv_inverse_valid=true;
		}
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

		glctx.cached_mv_inverse_valid=false;
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
	{
		memcpy(glctx.cur_matrix->m,m,16*sizeof(GLfloat));
		SET_MVINV_CACHED_INVALID;
	}
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

		SET_MVINV_CACHED_INVALID;
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

		SET_MVINV_CACHED_INVALID;
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

		SET_MVINV_CACHED_INVALID;
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