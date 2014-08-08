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

#define GET_RASTER_OPTION \
	(glctx.shade_model==YGL_SMOOTH?raster::FragSmooth:raster::FragFlat)|\
	(YGL_IS_ENABLED(YGL_TEXTURE_2D)?raster::FragTex:0)|\
	(YGL_IS_ENABLED(YGL_FOG)?raster::FragFog:0)|\
	(YGL_IS_FLAG_ENABLED(glctx.hint,YGL_PERSPECTIVE_CORRECTION_HINT)?raster::FragPersp:0)

//#define GET_RASTER_OPTION raster::FragSmooth

// GLenum GET_RASTER_OPTION()
// {
// 	bool s=(glctx.shade_model==YGL_SMOOTH);
// 	unsigned long long t=(YGL_IS_ENABLED(YGL_TEXTURE_2D));
// 	unsigned long long f=(YGL_IS_ENABLED(YGL_FOG));
// 	return YGL_SMOOTH;
// }

// Caching
#define SET_MVINV_CACHED_INVALID if(glctx.cur_matrix_mode==GL_MODELVIEW)glctx.cached_mv_inverse_valid=false;

GLContext glctx;

GLContext::GLContext()
{	
	// viewport
	// todo: check standard specification!!! about viewport? should init as window size?
	viewport.ox=(YGL_MAX_BUFFER_WIDTH>>1);
	viewport.oy=(YGL_MAX_BUFFER_HEIGHT>>1);
	viewport.w=YGL_MAX_BUFFER_WIDTH;
	viewport.h=YGL_MAX_BUFFER_HEIGHT;
	znear=0.;zfar=1.;

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
	ASSIGN_V4_WITH(clear_color,0.f,0.f,0.f,0.f);
	clear_depth=YGL_DEPTH_BUFFER_MAX_RES;//*1.f;

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	enable_flags=0;
	last_error=0;

	begin_type=YGL_NOBEGIN;

	// vertex data
// 	cur_tex_coord.s=cur_tex_coord.t=cur_tex_coord.r=0.f;
// 	cur_tex_coord.q=1.f;
	ASSIGN_V4_WITH(cur_tex_coord,0.f,0.f,0.f,1.f);

	cur_normal[0]=cur_normal[1]=0.f;
	cur_normal[2]=1.f;

	cur_color[0]=cur_color[1]=cur_color[2]=cur_color[3]=1.f;

// 	cur_secondary_color[0]=cur_secondary_color[1]=cur_secondary_color[2]=0.f;
// 	cur_secondary_color[3]=1.f;

	cur_edge_flag=true;// move to flags

	//cur_fog_coord=0.f;
	//cur_color_index=1.f;

	// 		line_stipple=0xffff;
	// 		line_stipple_repcnt=1;line_stipple_cnter=0;

	// todo: init
	enable_flags=0;

	cull_face_mode=YGL_BACK;
	front_face_mode=YGL_CCW;
	shade_model=YGL_FLAT;

	color_material_face=YGL_FRONT_AND_BACK;
	color_material_mode=GL_AMBIENT_AND_DIFFUSE;

	lighti_enable=0;
	// light 0
	ASSIGN_V4_WITH(lights[0].diffuse,1.f,1.f,1.f,1.f);
	ASSIGN_V4_WITH(lights[0].specular,1.f,1.f,1.f,1.f);
	
	ASSIGN_V4_WITH(light_model.ambient_scene,0.2f,0.2f,0.2f,1.f);
	light_model.local_viewer=false;
	light_model.two_sided=false;
	// color control

	//caching
	cached_mv_inverse_valid=true; // identity

	// texture
	tex_names.set(0);// reserve 0??
	memset(tex_objs,0,sizeof(tex_objs));
	// point to 0
	tex_targets[YGL_TEX_TARGET_1D]=tex_objs[0];
	tex_targets[YGL_TEX_TARGET_2D]=tex_objs[0];
	tex_env_mode=GL_MODULATE;
	ASSIGN_V4_WITH(tex_env_color,0,0,0,0);

	// fog
	fog_mode=GL_EXP;
	// fog_source=GL_FRAGMENT_DEPTH;
	fog_density=fog_end=1.f;fog_start=0.f;
	ASSIGN_V4_WITH(fog_color,0,0,0,0);
	cached_fog_e_s=1.f;


	//buffer::z_test_mask=FLOAT_ALWAYS;
//	YGL_SET_ENABLED_FLAG(buffer_write_mask,YGL_DEPTH_WRITE);
	//buffer::z_write_mask=true;

	// fragment op
	scissor_left=scissor_bottom=0;
	scissor_right=YGL_MAX_BUFFER_WIDTH; // size of GL window? violate specification
	scissor_top=YGL_MAX_BUFFER_HEIGHT;
	depth_test_mask=FLOAT_LE_0;//GL_LESS;
	alpha_test_mask=FLOAT_ALWAYS;//GL_ALWAYS
	alpha_ref=0; 
	stencil_test_mask=FLOAT_ALWAYS;//GL_ALWAYS
	stencil_ref=0;
	stencil_mask=0xffffffff;
	stencil_op_sfail=stencil_op_dpfail=stencil_op_dppass=GL_KEEP;
	blend_sfactor=GL_ONE;
	blend_dfactor=GL_ZERO;
	logicop=GL_COPY;

	// client state
	memset(client_arrays,0,sizeof(client_arrays));
	client_state=0;
	client_arrays[YGL_EDGE_FLAG_ARRAY].size=1;
	//client_arrays[YGL_EDGE_FLAG_ARRAY].type=GL_BOOL;
	client_arrays[YGL_NORMAL_ARRAY].size=3;

	// pixel store
	pixel_store.unpack_alignment=4;

	// hint
	YGL_SET_ENABLED_FLAG(glctx.hint,YGL_PERSPECTIVE_CORRECTION_HINT);
}

GLContext::~GLContext()
{
	// todo: use bitset
	for(int i=0;i<YGL_TEXTURE_NAMES_MAX;++i)
	{
		if(tex_objs[i])
		{
			delete tex_objs[i];
			tex_objs[i]=0;
		}
	}
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

	//Z_OF(p)=((glctx.zfar-glctx.znear)*0.5*Z_OF(p)+(glctx.zfar+glctx.znear)*0.5);
	

	unsigned int zint=YGL_DEPTH_BUFFER_MAX_RES*((glctx.zfar-glctx.znear)*0.5*Z_OF(p)+(glctx.zfar+glctx.znear)*0.5);
	FLOAT_AS_UINT(Z_OF(p))=zint;



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
	flat_color[2]=YGL_COLOR_F2I(newv.col_front_pri[2]);\
	flat_color[3]=YGL_COLOR_F2I(newv.col_front_pri[3]);

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

// #define LINE_RASTERIZE \
// 	{if(glctx.shade_model==YGL_SMOOTH)\
// 		raster::line_smooth(clip_buf);\
// 	else\
// 		raster::line(clip_buf);}
#define LINE_RASTERIZE raster::line(clip_buf);

GLenum glGetError()
{
	return glctx.last_error;
}

void glEnable(GLenum cap)
{
	CHECK_VALID_VALUE

	switch(cap)
	{
	case GL_LIGHTING:
		YGL_ENABLE(YGL_LIGHTING);
		break;
	case GL_COLOR_MATERIAL:
		YGL_ENABLE(YGL_COLOR_MATERIAL);
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
		YGL_ENABLE(YGL_CULL_FACE);
		break;
	case GL_SCISSOR_TEST:
		YGL_ENABLE(YGL_DEPTH_TEST);
		buffer::test_scissor=true;
		break;
	case GL_ALPHA_TEST:
		YGL_ENABLE(YGL_ALPHA_TEST);
		buffer::test_alpha=true;
		break;
	case GL_STENCIL_TEST:
		YGL_ENABLE(YGL_STENCIL_TEST);
		buffer::test_stencil=true;
		break;
	case GL_DEPTH_TEST:
		YGL_ENABLE(YGL_DEPTH_TEST);
		buffer::test_depth=true;
// 		buffer::z_test_mask=glctx.depth_test_mask;
// 		buffer::z_write_mask=YGL_IS_FLAG_ENABLED(glctx.buffer_write_mask,YGL_DEPTH_WRITE);
		break;
	case GL_BLEND:
		YGL_ENABLE(YGL_BLEND);
		buffer::enabled_blend=true;
		break;
	case GL_COLOR_LOGIC_OP:
	case GL_LOGIC_OP: // merge with GL_COLOR_LOGIC_OP
		YGL_ENABLE(YGL_LOGIC_OP);
		YGL_ENABLE(YGL_COLOR_LOGIC_OP);
		buffer::enabled_logicop=true;
		break;
	case GL_NORMALIZE:
		YGL_ENABLE(YGL_NORMALIZE);
		break;
	case GL_TEXTURE_2D:
		YGL_ENABLE(YGL_TEXTURE_2D);
		break;
	case GL_FOG:
		YGL_ENABLE(YGL_FOG);
		break;
	}
	
}
void glDisable(GLenum cap)
{
	CHECK_VALID_VALUE

	switch(cap)
	{
	case GL_LIGHTING:
		YGL_DISABLE(YGL_LIGHTING);
		break;
	case GL_COLOR_MATERIAL:
		YGL_DISABLE(YGL_COLOR_MATERIAL);
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
		YGL_DISABLE(YGL_CULL_FACE);
		break;
	case GL_SCISSOR_TEST:
		YGL_DISABLE(YGL_DEPTH_TEST);
		buffer::test_scissor=false;
		break;
	case GL_ALPHA_TEST:
		YGL_DISABLE(YGL_ALPHA_TEST);
		buffer::test_alpha=false;
		break;
	case GL_STENCIL_TEST:
		YGL_DISABLE(YGL_STENCIL_TEST);
		buffer::test_stencil=false;
		break;
	case GL_DEPTH_TEST:
		YGL_DISABLE(YGL_DEPTH_TEST);
		buffer::test_depth=false;
// 		buffer::z_test_mask=FLOAT_ALWAYS;
// 		buffer::z_write_mask=false; // write not allowed
		break;
	case GL_BLEND:
		YGL_DISABLE(YGL_BLEND);
		buffer::enabled_blend=false;
		break;
	case GL_COLOR_LOGIC_OP:
	case GL_LOGIC_OP: // merge with GL_COLOR_LOGIC_OP
		YGL_DISABLE(YGL_LOGIC_OP);
		YGL_DISABLE(YGL_COLOR_LOGIC_OP);
		buffer::enabled_logicop=false;
		break;
	case GL_NORMALIZE:
		YGL_DISABLE(YGL_NORMALIZE);
		break;
	case GL_TEXTURE_2D:
		YGL_DISABLE(YGL_TEXTURE_2D);
		break;
	case GL_FOG:
		YGL_DISABLE(YGL_FOG);
		break;
	}

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
	glctx.znear=YGL_CLAMP_MIN_MAX(n,0,1.);
	glctx.zfar=YGL_CLAMP_MIN_MAX(f,0,1.);
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

	// standard?? clamp it and inform user?
	width=YGL_CLAMP_MAX(width,YGL_MAX_BUFFER_WIDTH);
	height=YGL_CLAMP_MAX(height,YGL_MAX_BUFFER_WIDTH);

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
		glctx.asm_state.line_loop.vcnt=0;
		break;
	case GL_TRIANGLES:
		glctx.asm_state.tris.index=0;
		break;
	case GL_TRIANGLE_STRIP:
		glctx.asm_state.tri_strip.vcnt=0;
		glctx.asm_state.tri_strip.replace_index=0;
		break;
	// treat it as tri_fan
	case GL_POLYGON:
	case GL_TRIANGLE_FAN:
		glctx.asm_state.tri_fan.vcnt=0;
		break;
	case GL_QUADS:
		glctx.asm_state.quads.index=0;
		break;
	case GL_QUAD_STRIP:
		glctx.asm_state.quad_strip.index=0;
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
		if(0)//glctx.asm_state.line_loop.vcnt>=3) // at least 3 verts
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
	newv.edge_flag=glctx.cur_edge_flag;
	ASSIGN_V4_WITH(newv.p,x,y,z,w);

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
		ASSIGN_V4(newv.col_front_pri,glctx.cur_color);
	}

	// tex coord
	ASSIGN_V4(newv.tex_coords,glctx.cur_tex_coord);

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
		if(clip::point(&newv))
		{
			CLIP_SPACE_TO_WINDOW(newv.p);
			raster::point(&newv);
		}
		break;
#undef GLSTATE
#define GLSTATE (glctx.asm_state.lines)
	case GL_LINES:
		if(GLSTATE.is_odd) // next is odd
			GLSTATE.odd_v=newv;
		else
		{
			if(clip::line(&GLSTATE.odd_v,&newv,clip_buf))
			{
				CLIP_SPACE_TO_WINDOW_2(clip_buf);

				LINE_RASTERIZE;
			}
		}
		GLSTATE.is_odd=!GLSTATE.is_odd;
		break;
#undef GLSTATE
#define GLSTATE (glctx.asm_state.line_strip)
	case GL_LINE_STRIP:
		if(GLSTATE.is_first)
		{
			GLSTATE.is_first=false;
		}
		else
		{
			if(clip::line(&GLSTATE.prev_v,&newv,clip_buf))
			{
				CLIP_SPACE_TO_WINDOW_2(clip_buf);

				LINE_RASTERIZE;
			}
		}
		GLSTATE.prev_v=newv;
		break;
#undef GLSTATE
#define GLSTATE (glctx.asm_state.line_loop)
	case GL_LINE_LOOP:
		if(GLSTATE.vcnt==0) // first vert
		{
			GLSTATE.first_v=newv;
		}
		else
		{
			if(clip::line(&GLSTATE.prev_v,&newv,clip_buf))
			{
				CLIP_SPACE_TO_WINDOW_2(clip_buf);

				LINE_RASTERIZE;
			}
		}
		++GLSTATE.vcnt;
		GLSTATE.prev_v=newv;
		break;
#undef GLSTATE
#define GLSTATE (glctx.asm_state.tris)
	case GL_TRIANGLES:
		if(GLSTATE.index==2)
		{
			GLSTATE.index=0;

			// copy, due to clip even planes ( 6 planes ), result back in clip_buf
			clip_buf[0]=GLSTATE.verts[0];
			clip_buf[1]=GLSTATE.verts[1];
			clip_buf[2]=newv;

			POLYGON_CLIP_AND_RASTERIZE(3);
		}
		else
		{
			GLSTATE.verts[GLSTATE.index++]=newv;
		}
		break;
#undef GLSTATE
#define GLSTATE (glctx.asm_state.tri_strip)
	case GL_TRIANGLE_STRIP:
		if(glctx.asm_state.tri_strip.vcnt<2)
		{
			++GLSTATE.vcnt;

			GLSTATE.verts[GLSTATE.replace_index]=newv;
		}
		else
		{
			// we don't need it any more, glctx.asm_state.tri_strip.vcnt

			clip_buf[0]=GLSTATE.verts[0];
			clip_buf[1]=GLSTATE.verts[1];
			clip_buf[2]=newv;
			
			POLYGON_CLIP_AND_RASTERIZE(3);

			GLSTATE.verts[GLSTATE.replace_index]=newv;
		}
		GLSTATE.replace_index=!GLSTATE.replace_index;
		break;

		// assume convex, treat it like tri_fan
	case GL_POLYGON:
		// todo: edge flag
#undef GLSTATE
#define GLSTATE (glctx.asm_state.tri_fan)
	case GL_TRIANGLE_FAN:
		if(GLSTATE.vcnt<2)
		{
			GLSTATE.verts[GLSTATE.vcnt++]=newv;
		}
		else
		{
			clip_buf[0]=GLSTATE.verts[0];
			clip_buf[1]=GLSTATE.verts[1];
			clip_buf[2]=newv;

			POLYGON_CLIP_AND_RASTERIZE(3);

			GLSTATE.verts[1]=newv;
		}
		break;
#undef GLSTATE
#define GLSTATE (glctx.asm_state.quads)
	case GL_QUADS:
		if(GLSTATE.index==3)
		{
			GLSTATE.index=0;

#ifdef DO_TRIANGULATION
			// todo: edge flag
			clip_buf[0]=GLSTATE.verts[0];
			clip_buf[1]=GLSTATE.verts[1];
			clip_buf[2]=GLSTATE.verts[2];
			POLYGON_CLIP_AND_RASTERIZE(3);
			

			clip_buf[0]=GLSTATE.verts[2];
			clip_buf[1]=newv;
			clip_buf[2]=GLSTATE.verts[0];
			POLYGON_CLIP_AND_RASTERIZE(3);
#else
			clip_buf[0]=GLSTATE.verts[0];
			clip_buf[1]=GLSTATE.verts[1];
			clip_buf[2]=GLSTATE.verts[2];
			clip_buf[3]=newv;

			POLYGON_CLIP_AND_RASTERIZE(4);
#endif
			
		}
		else
		{
			GLSTATE.verts[GLSTATE.index++]=newv;
		}
		break;
#undef GLSTATE
#define GLSTATE (glctx.asm_state.quad_strip)
	case GL_QUAD_STRIP:
		if(GLSTATE.index<3)
		{
			GLSTATE.verts[GLSTATE.index++]=newv;
		}
		else // ready for a quad
		{
#ifdef DO_TRIANGULATION
			clip_buf[0]=GLSTATE.verts[0];
			clip_buf[1]=GLSTATE.verts[1];
			clip_buf[2]=newv;
			POLYGON_CLIP_AND_RASTERIZE(3);

			clip_buf[0]=newv;
			clip_buf[1]=GLSTATE.verts[2];
			clip_buf[2]=GLSTATE.verts[0];
			POLYGON_CLIP_AND_RASTERIZE(3);
#else
			clip_buf[0]=GLSTATE.verts[0];
			clip_buf[1]=GLSTATE.verts[1];
			clip_buf[2]=newv;
			clip_buf[3]=GLSTATE.verts[2];

			POLYGON_CLIP_AND_RASTERIZE(4);
#endif
			GLSTATE.index=2;
			GLSTATE.verts[0]=GLSTATE.verts[2];
			GLSTATE.verts[1]=newv;
		}
		break;
	default:
		glctx.last_error=GL_INVALID_VALUE;
		return;
	}
}

void glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	ASSIGN_V4_WITH(glctx.cur_color,r,g,b,a);
	if(YGL_IS_ENABLED(YGL_COLOR_MATERIAL))
	{
		GLboolean has_front=(glctx.color_material_face==GL_FRONT||glctx.color_material_face==GL_FRONT_AND_BACK),
			has_back=(glctx.color_material_face==GL_BACK||glctx.color_material_face==GL_FRONT_AND_BACK);
		switch(glctx.color_material_mode)
		{
		case GL_AMBIENT:
			if(has_front)
			{
				ASSIGN_V4_WITH(glctx.material_front.ambient,r,g,b,a);
			}
			if(has_back)
			{
				ASSIGN_V4_WITH(glctx.material_back.ambient,r,g,b,a);
			}
			break;
		case GL_DIFFUSE:
			if(has_front)
			{
				ASSIGN_V4_WITH(glctx.material_front.diffuse,r,g,b,a);
			}
			if(has_back)
			{
				ASSIGN_V4_WITH(glctx.material_back.diffuse,r,g,b,a);
			}
			break;
		case GL_SPECULAR:
			if(has_front)
			{
				ASSIGN_V4_WITH(glctx.material_front.specular,r,g,b,a);
			}
			if(has_back)
			{
				ASSIGN_V4_WITH(glctx.material_back.specular,r,g,b,a);
			}
			break;
		case GL_EMISSION:
			if(has_front)
			{
				ASSIGN_V4_WITH(glctx.material_front.emission,r,g,b,a);
			}
			if(has_back)
			{
				ASSIGN_V4_WITH(glctx.material_back.emission,r,g,b,a);
			}
			break;
		case GL_AMBIENT_AND_DIFFUSE:
			if(has_front)
			{
				ASSIGN_V4_WITH(glctx.material_front.ambient,r,g,b,a);
				ASSIGN_V4_WITH(glctx.material_front.diffuse,r,g,b,a);
			}
			if(has_back)
			{
				ASSIGN_V4_WITH(glctx.material_back.ambient,r,g,b,a);
				ASSIGN_V4_WITH(glctx.material_front.diffuse,r,g,b,a);
			}
			break;
		}
	}
}

// void glSecondaryColor3f(GLfloat r, GLfloat g, GLfloat b/*, GLfloat alpha = 0.f*/)
// {
// 	ASSIGN_V3_WITH(glctx.cur_secondary_color,r,g,b);
// }

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
	ASSIGN_V4_WITH(glctx.cur_tex_coord,s,t,r,q);
}

void glMultiTexCoord4f(GLenum tex, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
	_NOT_IMPLEMENTED_(glMultiTexCoord4f)
}

void glEdgeFlag(GLboolean flag)
{
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
			ASSIGN_V4(glctx.material_front.ambient,params);
		}
		if(has_back)
		{
			ASSIGN_V4(glctx.material_back.ambient,params);
		}
		break;
	case GL_DIFFUSE:
		if(has_front)
		{
			ASSIGN_V4(glctx.material_front.diffuse,params);
		}
		if(has_back)
		{
			ASSIGN_V4(glctx.material_back.diffuse,params);
		}
		break;
	case GL_SPECULAR:
		if(has_front)
		{
			ASSIGN_V4(glctx.material_front.specular,params);
		}
		if(has_back)
		{
			ASSIGN_V4(glctx.material_back.specular,params);
		}
		break;
	case GL_EMISSION:
		if(has_front)
		{
			ASSIGN_V4(glctx.material_front.emission,params);
		}
		if(has_back)
		{
			ASSIGN_V4(glctx.material_back.emission,params);
		}
		break;
	case GL_AMBIENT_AND_DIFFUSE:
		if(has_front)
		{
			ASSIGN_V4(glctx.material_front.ambient,params);
			ASSIGN_V4(glctx.material_front.diffuse,params);
		}
		if(has_back)
		{
			ASSIGN_V4(glctx.material_back.ambient,params);
			ASSIGN_V4(glctx.material_front.diffuse,params);
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
		ASSIGN_V4(li->ambient,params);
		break;
	case GL_DIFFUSE:
		ASSIGN_V4(li->diffuse,params);
		break;
	case GL_SPECULAR:
		ASSIGN_V4(li->specular,params);
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
		ASSIGN_V4(glctx.light_model.ambient_scene,params);
		break;
	}
}

/* texture */

void glGenTextures(GLsizei n, GLuint *textures)
{
	int begin=0;
	while(n--)
	{
		begin=glctx.tex_names.lowest0(begin,true);
		if(begin==-1) break;
		*textures++=begin++;
	}
}
void glDeleteTextures(GLsizei n, const GLuint *textures)
{
	TextureObject* obj;
	while(n--)
	{
		if(*textures&&*textures<YGL_TEXTURE_NAMES_MAX)
		{
			obj=glctx.tex_objs[*textures];
			if(obj)
			{
				if(obj==glctx.tex_targets[obj->dimension-GL_TEXTURE_1D])
				{
					// default
					glctx.tex_targets[obj->dimension-GL_TEXTURE_1D]=glctx.tex_objs[0];
				}
				glctx.tex_objs[*textures]=0;
				delete obj;
			}
			glctx.tex_names.clear(*textures);
		}
	}
}
void glBindTexture(GLenum target, GLuint texture)
{
	CHECK_VALID_VALUE

	glctx.tex_names.set(texture);
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

	case GL_GENERATE_MIPMAP:
		targetobj->auto_gen_mipmap=(param==GL_TRUE?true:false);
		break;
	}
	if(targetobj->filter_mag==GL_LINEAR&&
		(targetobj->filter_min==GL_NEAREST_MIPMAP_NEAREST||
		targetobj->filter_min==GL_NEAREST_MIPMAP_LINEAR))
	{
		targetobj->c=0.5f;
	}
	else
	{
		targetobj->c=0.f;
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
			ASSIGN_V4(targetobj->border_color,params);
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
		glctx.tex_env_color[0]=YGL_COLOR_F2I(params[0]);
		glctx.tex_env_color[1]=YGL_COLOR_F2I(params[1]);
		glctx.tex_env_color[2]=YGL_COLOR_F2I(params[2]);
		glctx.tex_env_color[3]=YGL_COLOR_F2I(params[3]);
		//ASSIGN_V4(glctx.tex_env_color,params);
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
	// internalformat force as RGBA, GL_UNSIGNED_BYTE, always align with 4
	// check 2^m+b?
	// check power of two x&(x-1)
	CHECK_VALID_VALUE
	// YGL_MAX_TEXTURE_LEVEL
	border=(border>0); // 0,1
	
	TextureObject* texobj=glctx.tex_targets[YGL_TEX_TARGET_2D];
	TexImage* img=texobj->mipmaps+level;
	img->border=border;
	img->width=width;
	img->height=height;

	if(img->pixels) free(img->pixels);
	img->pixels=(GLubyte*)malloc(4*sizeof(GLubyte)*width*height); // align with 4, todo: border
	GLubyte* p=img->pixels;
	GLint components=(format==GL_RGBA?4:3);

#define UNPACK_ALIGN_WIDTH(w,align) (((w)+(align)-1)/(align)*(align))

	// todo: add border
	if(type==GL_FLOAT)
	{
		GLint unpack_line_width=UNPACK_ALIGN_WIDTH(width*components*sizeof(GLfloat),glctx.pixel_store.unpack_alignment);

		GLfloat* line=(GLfloat*)pixels,*q;
		for(int y=0;y<height;++y)
		{
			q=line;
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
			line=(GLfloat*)(((GLubyte*)line)+unpack_line_width);
		}
	}
	else if(type==GL_UNSIGNED_BYTE)
	{
		GLint unpack_line_width=UNPACK_ALIGN_WIDTH(components*sizeof(GLubyte),glctx.pixel_store.unpack_alignment);

		GLubyte* line=(GLubyte*)pixels,*q;
		for(int y=0;y<height;++y)
		{
			q=line;
			for(int x=0;x<width;++x)
			{
				*p++=*q++;*p++=*q++;*p++=*q++;
				*p++=(type==GL_RGBA?*q++:255);
			}
			line=(GLubyte*)(((GLubyte*)line)+unpack_line_width);
		}
	}

	if(level==0)
	{
		// width-border?
		GLuint a=log2(width),b=log2(height);
		texobj->max_level=MAX(a,b);
		if(texobj->auto_gen_mipmap)
		{
			texobj->gen_mipmap();
		}
	}
}
/* fog */
void glFogi(GLenum pname, GLint param)
{
	CHECK_VALID_VALUE

	switch(pname)
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
		glctx.fog_color[0]=YGL_COLOR_F2I(params[0]);
		glctx.fog_color[1]=YGL_COLOR_F2I(params[1]);
		glctx.fog_color[2]=YGL_COLOR_F2I(params[2]);
		glctx.fog_color[3]=YGL_COLOR_F2I(params[3]);
		//ASSIGN_V4(glctx.fog_color,params);
		break;
	}

}

/* fragment op */
void glDepthMask(GLboolean flag)
{
	buffer::write_z=flag;
}
void glColorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a)
{
	buffer::write_r=r;
	buffer::write_g=g;
	buffer::write_b=b;
	buffer::write_a=a;
}
void glStencilMask(GLuint mask)
{
	buffer::write_s_mask=mask;
}
void glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
	CHECK_VALID_VALUE
	glctx.scissor_left=x;glctx.scissor_bottom=y;
	glctx.scissor_right=x+width;glctx.scissor_top=y+height;
}
void glAlphaFunc(GLenum func, GLclampf ref)
{
#define MASK_TO_SET glctx.alpha_test_mask
	switch(func)
	{
	case GL_NEVER:
		MASK_TO_SET=0;
		break;
	case GL_ALWAYS:
		MASK_TO_SET=(FLOAT_EQ_0|FLOAT_LE_0|FLOAT_GT_0);
		break;
	case GL_LESS:
		MASK_TO_SET=(FLOAT_LE_0);
		break;
	case GL_LEQUAL:
		MASK_TO_SET=(FLOAT_EQ_0|FLOAT_LE_0);
		break;
	case GL_EQUAL:
		MASK_TO_SET=(FLOAT_EQ_0);
		break;
	case GL_GREATER:
		MASK_TO_SET=(FLOAT_GT_0);
		break;
	case GL_NOTEQUAL:
		MASK_TO_SET=(FLOAT_LE_0|FLOAT_GT_0);
		break;
	case GL_GEQUAL:
		MASK_TO_SET=(FLOAT_EQ_0|FLOAT_GT_0);
		break;
	}
#undef MASK_TO_SET
	ref=YGL_CLAMP_MIN_MAX(ref,0.f,1.f);
	glctx.alpha_ref=YGL_COLOR_F2I(ref);
}
void glDepthFunc(GLenum func)
{
#define MASK_TO_SET glctx.depth_test_mask
	switch(func)
	{
	case GL_NEVER:
		MASK_TO_SET=0;
		break;
	case GL_ALWAYS:
		MASK_TO_SET=(FLOAT_EQ_0|FLOAT_LE_0|FLOAT_GT_0);
		break;
	case GL_LESS:
		MASK_TO_SET=(FLOAT_LE_0);
		break;
	case GL_LEQUAL:
		MASK_TO_SET=(FLOAT_EQ_0|FLOAT_LE_0);
		break;
	case GL_EQUAL:
		MASK_TO_SET=(FLOAT_EQ_0);
		break;
	case GL_GREATER:
		MASK_TO_SET=(FLOAT_GT_0);
		break;
	case GL_NOTEQUAL:
		MASK_TO_SET=(FLOAT_LE_0|FLOAT_GT_0);
		break;
	case GL_GEQUAL:
		MASK_TO_SET=(FLOAT_EQ_0|FLOAT_GT_0);
		break;
	}
#undef MASK_TO_SET

	//buffer::z_test_mask=(YGL_IS_ENABLED(YGL_DEPTH_TEST)?glctx.depth_test_mask:FLOAT_ALWAYS);
}
void glStencilFunc(GLenum func, GLint ref, GLuint mask)
{
	CHECK_VALID_VALUE

#define MASK_TO_SET glctx.stencil_test_mask
	switch(func)
	{
	case GL_NEVER:
		MASK_TO_SET=0;
		break;
	case GL_ALWAYS:
		MASK_TO_SET=(FLOAT_EQ_0|FLOAT_LE_0|FLOAT_GT_0);
		break;
	case GL_LESS:
		MASK_TO_SET=(FLOAT_LE_0);
		break;
	case GL_LEQUAL:
		MASK_TO_SET=(FLOAT_EQ_0|FLOAT_LE_0);
		break;
	case GL_EQUAL:
		MASK_TO_SET=(FLOAT_EQ_0);
		break;
	case GL_GREATER:
		MASK_TO_SET=(FLOAT_GT_0);
		break;
	case GL_NOTEQUAL:
		MASK_TO_SET=(FLOAT_LE_0|FLOAT_GT_0);
		break;
	case GL_GEQUAL:
		MASK_TO_SET=(FLOAT_EQ_0|FLOAT_GT_0);
		break;
	}
#undef MASK_TO_SET

	glctx.stencil_ref=ref;
	glctx.stencil_mask=mask;
}
void glStencilOp(GLenum sfail,GLenum dpfail,GLenum dppass)
{
	glctx.stencil_op_sfail=sfail;
	glctx.stencil_op_dpfail=dpfail;
	glctx.stencil_op_dppass=dppass;
}
void glBlendFunc(GLenum sfactor, GLenum dfactor)
{
	glctx.blend_sfactor=sfactor;
	glctx.blend_dfactor=dfactor;
}
void glLogicOp(GLenum opcode)
{
	glctx.logicop=opcode;
}
void glClear(GLbitfield mask)
{
	if(mask&GL_COLOR_BUFFER_BIT)
		buffer::clear_color(YGL_V4_LIST_COMPONENTS(glctx.clear_color));
	if(mask&GL_DEPTH_BUFFER_BIT)
		buffer::clear_depth(glctx.clear_depth);
	if(mask&GL_STENCIL_BUFFER_BIT)
		buffer::clear_stencil(glctx.clear_stencil);
}
void glClearColor(GLclampf rf, GLclampf gf, GLclampf bf, GLclampf af)
{
	cbuf_type r=YGL_COLOR_F2I(rf),g=YGL_COLOR_F2I(gf),b=YGL_COLOR_F2I(bf),a=YGL_COLOR_F2I(af);
	ASSIGN_V4_WITH(glctx.clear_color,r,g,b,a);
}
void glClearDepth(GLclampd depth)
{
	glctx.clear_depth=(YGL_DEPTH_BUFFER_MAX_RES)*YGL_CLAMP_MIN_MAX(depth,0.,1.);
}
void glClearStencil(GLint s)
{
	glctx.clear_stencil=s;
}

// void glDepthMask(GLboolean flag)
// {
// 	if(flag)
// 	{
// 		YGL_SET_ENABLED_FLAG(glctx.buffer_write_mask,YGL_DEPTH_WRITE);
// 	}
// 	else
// 	{
// 		YGL_CLEAR_ENABLED_FLAG(glctx.buffer_write_mask,YGL_DEPTH_WRITE);
// 	}
// 
// 	buffer::z_write_mask=(YGL_IS_ENABLED(YGL_DEPTH_TEST)?flag:false);
// 	//_NOT_IMPLEMENTED_(glDepthMask);
// }

/* client state */
void glEnableClientState(GLenum array)
{
	CHECK_VALID_VALUE
	switch(array)
	{
	case GL_COLOR_ARRAY:
		YGL_SET_ENABLED_FLAG(glctx.client_state,YGL_COLOR_ARRAY);
		break;
	case GL_EDGE_FLAG_ARRAY:
		YGL_SET_ENABLED_FLAG(glctx.client_state,YGL_EDGE_FLAG_ARRAY);
		break;
	//GL_INDEX_ARRAY,
	case GL_NORMAL_ARRAY:
		YGL_SET_ENABLED_FLAG(glctx.client_state,YGL_NORMAL_ARRAY);
		break;
	case GL_TEXTURE_COORD_ARRAY:
		YGL_SET_ENABLED_FLAG(glctx.client_state,YGL_TEXTURE_COORD_ARRAY);
		break;
	case GL_VERTEX_ARRAY:
		YGL_SET_ENABLED_FLAG(glctx.client_state,YGL_VERTEX_ARRAY);
		break;
	}
}
void glDisableClientState(GLenum array)
{
	CHECK_VALID_VALUE
	switch(array)
	{
	case GL_COLOR_ARRAY:
		YGL_CLEAR_ENABLED_FLAG(glctx.client_state,YGL_COLOR_ARRAY);
		break;
	case GL_EDGE_FLAG_ARRAY:
		YGL_CLEAR_ENABLED_FLAG(glctx.client_state,YGL_EDGE_FLAG_ARRAY);
		break;
		//GL_INDEX_ARRAY,
	case GL_NORMAL_ARRAY:
		YGL_CLEAR_ENABLED_FLAG(glctx.client_state,YGL_NORMAL_ARRAY);
		break;
	case GL_TEXTURE_COORD_ARRAY:
		YGL_CLEAR_ENABLED_FLAG(glctx.client_state,YGL_TEXTURE_COORD_ARRAY);
		break;
	case GL_VERTEX_ARRAY:
		YGL_CLEAR_ENABLED_FLAG(glctx.client_state,YGL_VERTEX_ARRAY);
		break;
	}
}
void glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	// only GL_FLOAT
	CHECK_VALID_VALUE

	glctx.client_arrays[YGL_COLOR_ARRAY].size=size;
	glctx.client_arrays[YGL_COLOR_ARRAY].type=GL_FLOAT/*type*/;
	glctx.client_arrays[YGL_COLOR_ARRAY].stride=(stride?stride:(sizeof(GLfloat)*size));
	glctx.client_arrays[YGL_COLOR_ARRAY].pointer=pointer;
}
void glEdgeFlagPointer(GLsizei stride, const GLvoid *pointer)
{
	// only GL_FLOAT
	CHECK_VALID_VALUE

	//client_arrays[YGL_EDGE_FLAG_ARRAY].size=1;
	//client_arrays[YGL_EDGE_FLAG_ARRAY].type=GL_BOOL;
	glctx.client_arrays[YGL_EDGE_FLAG_ARRAY].stride=(stride?stride:sizeof(GLboolean));
	glctx.client_arrays[YGL_EDGE_FLAG_ARRAY].pointer=pointer;
}
//void glIndexPointer(GLenum type, GLsizei stride, const GLvoid *pointer);
void glNormalPointer(GLenum type, GLsizei stride, const GLvoid *pointer)
{
	// only GL_FLOAT
	CHECK_VALID_VALUE

	//client_arrays[YGL_NORMAL_ARRAY].size=3;
	glctx.client_arrays[YGL_NORMAL_ARRAY].type=GL_FLOAT/*type*/;
	glctx.client_arrays[YGL_NORMAL_ARRAY].stride=(stride?stride:(sizeof(GLfloat)*3));
	glctx.client_arrays[YGL_NORMAL_ARRAY].pointer=pointer;
}
void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	// only GL_FLOAT
	CHECK_VALID_VALUE

	glctx.client_arrays[YGL_TEXTURE_COORD_ARRAY].size=size;
	glctx.client_arrays[YGL_TEXTURE_COORD_ARRAY].type=GL_FLOAT/*type*/;
	glctx.client_arrays[YGL_TEXTURE_COORD_ARRAY].stride=(stride?stride:(sizeof(GLfloat)*size));
	glctx.client_arrays[YGL_TEXTURE_COORD_ARRAY].pointer=pointer;
}
void glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	// only GL_FLOAT
	CHECK_VALID_VALUE

	glctx.client_arrays[YGL_VERTEX_ARRAY].size=size;
	glctx.client_arrays[YGL_VERTEX_ARRAY].type=GL_FLOAT/*type*/;
	glctx.client_arrays[YGL_VERTEX_ARRAY].stride=(stride?stride:(sizeof(GLfloat)*size));
	glctx.client_arrays[YGL_VERTEX_ARRAY].pointer=pointer;
}
// #define GET_CLIENT_ARRAY_ELEMENT(arr,ele_type,i) \
// 	(ele_type*)(((GLubyte*)(arr)->pointer)+(i)*((arr)->size*sizeof(ele_type)+(arr)->stride))
#define GET_CLIENT_ARRAY_ELEMENT(arr,ele_type,i) \
	(ele_type*)(((GLubyte*)(arr)->pointer)+(i)*(arr)->stride)
// generate multiple version?
void glArrayElement(GLint i)
{
	GLContext::ClientArray* arr=0;
	if(YGL_IS_CLIENTSTATE_ENABLED(YGL_COLOR_ARRAY))
	{
		arr=glctx.client_arrays+YGL_COLOR_ARRAY;
		switch(arr->size)
		{
		case 3:
			glColor3fv(GET_CLIENT_ARRAY_ELEMENT(arr,GLfloat,i));
			break;
		case 4:
			glColor4fv(GET_CLIENT_ARRAY_ELEMENT(arr,GLfloat,i));
			break;
		}
	}
	if(YGL_IS_CLIENTSTATE_ENABLED(YGL_NORMAL_ARRAY))
	{
		arr=glctx.client_arrays+YGL_NORMAL_ARRAY;

		glNormal3fv(GET_CLIENT_ARRAY_ELEMENT(arr,GLfloat,i));
	}
	if(YGL_IS_CLIENTSTATE_ENABLED(YGL_TEXTURE_COORD_ARRAY))
	{
		arr=glctx.client_arrays+YGL_TEXTURE_COORD_ARRAY;
		switch(arr->size)
		{
		case 1:
			glTexCoord1fv(GET_CLIENT_ARRAY_ELEMENT(arr,GLfloat,i));
			break;
		case 2:
			glTexCoord2fv(GET_CLIENT_ARRAY_ELEMENT(arr,GLfloat,i));
			break;
		case 3:
			glTexCoord3fv(GET_CLIENT_ARRAY_ELEMENT(arr,GLfloat,i));
			break;
		case 4:
			glTexCoord4fv(GET_CLIENT_ARRAY_ELEMENT(arr,GLfloat,i));
			break;
		}
	}
	if(YGL_IS_CLIENTSTATE_ENABLED(YGL_EDGE_FLAG_ARRAY))
	{
		arr=glctx.client_arrays+YGL_EDGE_FLAG_ARRAY;

		glEdgeFlag(GET_CLIENT_ARRAY_ELEMENT(arr,GLboolean,i));
	}

	if(YGL_IS_CLIENTSTATE_ENABLED(YGL_VERTEX_ARRAY))
	{
		arr=glctx.client_arrays+YGL_VERTEX_ARRAY;
		switch(arr->size)
		{
		case 2:
			glVertex2fv(GET_CLIENT_ARRAY_ELEMENT(arr,GLfloat,i));
			break;
		case 3:
			glVertex3fv(GET_CLIENT_ARRAY_ELEMENT(arr,GLfloat,i));
			break;
		case 4:
			glVertex4fv(GET_CLIENT_ARRAY_ELEMENT(arr,GLfloat,i));
			break;
		}
	}
}
void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
	CHECK_VALID_VALUE

	glBegin(mode);

	switch(type)
	{
	case GL_UNSIGNED_BYTE:
		{
			GLubyte* ids=(GLubyte*)indices;
			for(int i=0;i<count;++i)
				glArrayElement(ids[i]);
		}
		break;
	case GL_UNSIGNED_SHORT:
		{
			GLushort* ids=(GLushort*)indices;
			for(int i=0;i<count;++i)
				glArrayElement(ids[i]);
		}
		break;
	case GL_UNSIGNED_INT:
		{
			GLuint* ids=(GLuint*)indices;
			for(int i=0;i<count;++i)
				glArrayElement(ids[i]);
		}
		break;
	}
	
	glEnd();
}
void glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	CHECK_VALID_VALUE

	glBegin(mode);
	
	for(int end=first+count;first<end;++first)
		glArrayElement(first);

	glEnd();

}
void glInterleavedArrays(GLenum format, GLsizei stride, const GLvoid *pointer)
{
	CHECK_VALID_VALUE

	GLboolean et=false,ec=false,en=false;
	GLsizei st=0,sc=0,sv=0;
	//GLenum tc;
	GLuint pc=0,pn=0,pv=0;
	GLsizei s=0;

#define F sizeof(GLfloat)

	switch(format)
	{
	case GL_V2F:
		et=ec=en=false;
		sv=2;
		pv=0;
		s=2*F;
	case GL_V3F:
		et=ec=en=false;
		sv=3;
		pv=0;
		s=3*F;
	// GL_C4UB_V2F, GL_C4UB_V3F
	case GL_C3F_V3F:
		et=false;ec=true;en=false;
		sc=3;
		sv=3;
		pc=0;pv=3*F;
		s=6*F;
		break;
	case GL_N3F_V3F:
		et=false;ec=false;en=true;
		sv=3;
		pn=0;pv=3*F;
		s=6*F;
		break;
	case GL_C4F_N3F_V3F:
		et=false;ec=true;en=true;
		sc=4;
		sv=3;
		pc=0;pn=4*F;pv=7*F;
		s=10*F;
		break;
	case GL_T2F_V3F:
		et=true;ec=false;en=false;
		st=2;sv=3;
		pv=2*F;
		s=5*F;
		break;
	case GL_T4F_V4F:
		et=true;ec=false;en=false;
		st=4;sv=4;
		pv=4*F;
		s=8*F;
		break;
	//case GL_T2F_C4UB_V3F
	case GL_T2F_C3F_V3F:
		et=true;ec=true;en=false;
		st=2;sc=3;sv=3;
		pc=2*F;pv=5*F;
		s=8*F;
		break;
	case GL_T2F_N3F_V3F:
		et=true;ec=false;en=true;
		st=2;sv=3;
		pn=2*F;pv=5*F;
		s=8*F;
		break;
	case GL_T2F_C4F_N3F_V3F:
		et=ec=en=true;
		st=2;sc=4;sv=3;
		pc=2*F;pn=6*F;pv=9*F;
		s=12*F;
		break;
	case GL_T4F_C4F_N3F_V4F:
		et=ec=en=true;
		st=4;sc=4;sv=4;
		pc=4*F;pn=8*F;pv=11*F;
		s=15*F;
		break;
	}
#undef F
	GLint str=stride;
	if(!str) str=s;

	glDisableClientState(GL_EDGE_FLAG_ARRAY);
	//glDisableClientState(GL_INDEX_ARRAY);
	if(et)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(st,GL_FLOAT,str,pointer);
	}
	else
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	if(ec)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(sc,GL_FLOAT,str,((GLubyte*)pointer)+pc);
	}
	else
	{
		glDisableClientState(GL_COLOR_ARRAY);
	}

	if(en)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT,str,((GLubyte*)pointer)+pn);
	}
	else
	{
		glDisableClientState(GL_NORMAL_ARRAY);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(sv,GL_FLOAT,str,((GLubyte*)pointer)+pv);

}

/* pixel */
void glPixelStorei(GLenum pname, GLint param)
{
	CHECK_VALID_VALUE
	switch(pname)
	{
	case GL_UNPACK_ALIGNMENT:
		// 1,2,4,8
		glctx.pixel_store.unpack_alignment=param;
		break;
	}
}

/* hint */
void glHint(GLenum target, GLenum mode)
{
	switch(target)
	{
	case GL_PERSPECTIVE_CORRECTION_HINT:
		if(mode==GL_FASTEST)
		{
			YGL_CLEAR_ENABLED_FLAG(glctx.hint,YGL_PERSPECTIVE_CORRECTION_HINT);
		}
		else // default nicest(dont_care)
		{
			YGL_SET_ENABLED_FLAG(glctx.hint,YGL_PERSPECTIVE_CORRECTION_HINT);
		}
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


/* query */
void glGetFloatv(GLenum pname, GLfloat *params)
{
	switch(pname)
	{
	case GL_MODELVIEW_MATRIX:
		memcpy(params,glctx.matrix_mv.peek()->m,sizeof(GLfloat)*16);
		break;
	}
}

/* attribs*/
void glPushAttrib(GLbitfield mask)
{

}
void glPopAttrib()
{

}

}//end namespace