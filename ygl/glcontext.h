#ifndef _YGL_CONTEXT_
#define _YGL_CONTEXT_

#include "ygl.h"
#include "yglconstants.h"
#include "yglstructs.h"
#include "stack.h"
#include "light.h"
#include "texture.h"
#include "bitset.h"


#define YGL_SET_ENABLED_FLAG_INT64(flags,which) flags|=(((unsigned long long)1)<<(which));
#define YGL_CLEAR_ENABLED_FLAG_INT64(flags,which) flags&=~(((unsigned long long)1)<<(which));

#define YGL_SET_ENABLED_FLAG(flags,which) flags|=(((GLuint)1)<<(which));
#define YGL_CLEAR_ENABLED_FLAG(flags,which) flags&=~(((GLuint)1)<<(which));
#define YGL_IS_FLAG_ENABLED(flags,which) ((flags)&(((GLuint)1)<<(which)))

#define YGL_ENABLE(which) YGL_SET_ENABLED_FLAG_INT64(glctx.enable_flags,which)
#define YGL_DISABLE(which) YGL_CLEAR_ENABLED_FLAG_INT64(glctx.enable_flags,which)
#define YGL_IS_ENABLED(which) (glctx.enable_flags&(((unsigned long long)1)<<(which)))


#define YGL_IS_LIGHT_ENABLED(which) YGL_IS_FLAG_ENABLED(glctx.lighti_enable,which)
#define YGL_IS_CLIENTSTATE_ENABLED(which) YGL_IS_FLAG_ENABLED(glctx.client_state,which)
// #define YGL_SET_LIGHT_ENABLED_FLAG(which) glctx.lighti_enable|=(1<<(which));
// #define YGL_CLEAR_LIGHT_ENABLED_FLAG(which) glctx.lighti_enable&=~(1<<(which));

// polygon != line
#define YGL_IS_POLYGON \
	(glctx.begin_type!=GL_POINTS&&glctx.begin_type!=GL_LINES&&glctx.begin_type!=GL_LINE_LOOP&&glctx.begin_type!=GL_LINE_STRIP)

// GLuint -> GLubyte, use YGL enum?
struct GLContext
{
	GLContext();
	~GLContext();
	//void init(GLint width, GLint height);

	GLenum begin_type;
	union
	{
		struct  
		{
			Vertex odd_v;
			GLboolean is_odd;
		}lines;
		struct  
		{
			Vertex prev_v;
			GLboolean is_first;
		}line_strip;
		struct  
		{
			Vertex prev_v;
			GLbyte vcnt;
			Vertex first_v;
		}line_loop;
		struct  
		{
			Vertex verts[2];
			GLubyte index;
		}tris;
		struct 
		{
			Vertex verts[2];
			GLubyte replace_index;
			GLubyte vcnt;
		}tri_strip;
		struct 
		{
			Vertex verts[2];
			GLubyte vcnt;
		}tri_fan;
		struct 
		{
			Vertex verts[3];
			GLubyte index;
		}quads;
		struct 
		{
			Vertex verts[3];
			GLubyte index;
		}quad_strip;
		// polygon same as tri_fan
	}asm_state;

	// vertex data
// 	struct  
// 	{
// 		GLfloat s,t,r,q;
// 	}cur_tex_coord;
	GLfloat cur_tex_coord[4];
	Normal cur_normal/*[3]*/;
	GLfloat cur_color[4];
	//GLfloat cur_secondary_color[4]; // specular
	//GLfloat cur_fog_coord;
	//GLfloat cur_color_index;
	GLboolean cur_edge_flag;// move to flags

	GLuint client_state;
	struct ClientArray
	{
		GLint size;
		GLenum type;
		GLsizei stride;
		const GLvoid* pointer;
	}client_arrays[YGL_CLIENT_ARRAY_MAX];

	// viewport
	struct  
	{
		GLint ox,oy,w,h;
	}viewport;
	GLclampd znear,zfar;

	// matrix
	// todo: use ubyte, and YGL_PROJECTION... instead
	GLuint cur_matrix_mode;
	Matrix44* cur_matrix;
	stack<Matrix44,YGL_MODELVIEW_STACK_DEPTH> matrix_mv;
	stack<Matrix44,YGL_PROJECTION_STACK_DEPTH> matrix_proj;
	//stack<> matrix_color;

// 	GLubyte cur_texture;
// 
// 	GLdouble clip_planes[max_clip_planes][0];
// 	GLuint clip_planes_flag;
// 
// 	GLfloat point_size;
// 	GLfloat line_width;
// 	GLushort line_stipple;
// 	GLubyte line_stipple_repcnt,line_stipple_cnter;
	
	struct  
	{
		GLint unpack_alignment; // 1,2,4,8
	}pixel_store;

	GLuint hint;


	// light
	struct
	{
		GLfloat ambient_scene[4];
		GLboolean local_viewer;
		//GLenum color_control;
		GLboolean two_sided;
	}light_model;
	LightSource lights[YGL_MAX_LIGHTS];
	// front for points,lines,bitmaps,no two-sided polygons, and front-facing
	Material material_front,material_back;
	
	GLuint lighti_enable,clip_planei_enable;
	// one bit??
	GLuint color_material_mode;
	GLuint color_material_face;
	GLubyte cull_face_mode,front_face_mode;
	GLubyte shade_model;


	unsigned long long enable_flags;
	GLuint last_error;

	// texture
	bitset<YGL_TEXTURE_NAMES_MAX> tex_names;
	TextureObject* tex_objs[YGL_TEXTURE_NAMES_MAX];
	TextureObject* tex_targets[YGL_TEX_TARGET_MAX];
	GLint tex_env_mode;
	cbuf_type tex_env_color[4];
	//GLint tex_gen_mode[4];

	// fog
	GLint fog_mode/*,fog_source not in 1.1*/;
	GLfloat fog_density,fog_end,fog_start;
	cbuf_type fog_color[4];
	GLfloat cached_fog_e_s; // inv(e-s)

	// fragment op && buffer
	GLint scissor_left,scissor_bottom,scissor_right,scissor_top;
	GLubyte depth_test_mask; // converted from DepthFunc
	GLubyte alpha_test_mask; // converted from AlphaFunc
	cbuf_type alpha_ref; 
	GLubyte stencil_test_mask; // converted from StencilFunc
	sbuf_type stencil_ref;
	GLuint stencil_mask;
	GLenum stencil_op_sfail,stencil_op_dpfail,stencil_op_dppass;
	GLenum blend_sfactor,blend_dfactor;
	GLenum logicop;

	cbuf_type clear_color[4];
	zbuf_type clear_depth;
	sbuf_type clear_stencil;

	// cached values
	GLboolean cached_mv_inverse_valid;
	Matrix44 cached_mv_inverse;



};
extern GLContext glctx;

#endif