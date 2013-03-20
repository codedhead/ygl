#ifndef _YGL_CONTEXT_
#define _YGL_CONTEXT_

#include "ygl.h"
#include "yglconstants.h"
#include "yglstructs.h"
#include "stack.h"
#include "light.h"
#include "texture.h"
#include "bitset.h"

#define YGL_IS_FLAG_ENABLED(flags,which) ((flags)&(((unsigned long long)1)<<(which)))
#define YGL_IS_ENABLED(which) (glctx.enable_flags&(((unsigned long long)1)<<(which)))
#define YGL_SET_ENABLED_FLAG(flags,which) flags|=(((unsigned long long)1)<<(which));
#define YGL_CLEAR_ENABLED_FLAG(flags,which) flags&=~(((unsigned long long)1)<<(which));

#define YGL_IS_LIGHT_ENABLED(which) (glctx.lighti_enable&(1<<(which)))
// #define YGL_SET_LIGHT_ENABLED_FLAG(which) glctx.lighti_enable|=(1<<(which));
// #define YGL_CLEAR_LIGHT_ENABLED_FLAG(which) glctx.lighti_enable&=~(1<<(which));

// polygon != line
#define YGL_IS_POLYGON \
	(glctx.begin_type!=GL_POINTS&&glctx.begin_type!=GL_LINES&&glctx.begin_type!=GL_LINE_LOOP&&glctx.begin_type!=GL_LINE_STRIP)

struct GLContext
{
	GLContext();
	//void init(GLint width, GLint height);

	GLenum begin_type;
	union
	{
		struct  
		{
			Vertex odd_v; // first v in cur segment
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
			//GLboolean is_first;
			GLbyte complete_cnt;
			Vertex first_v;
		}line_loop;
		struct  
		{
			//Vertex A,B;
			Vertex verts[2];
			GLubyte index;
		}tris;
		struct 
		{
			Vertex verts[3];
			GLubyte index;
		}quads;
// 		struct
// 		{
// 			Vertex A,B;
// 			//bool first_tri_finished;
// 			GLubyte cnt; // start to form triangle when cnt>=2?
// 		}tri_strip;
// 		struct
// 		{
// 			Vertex
// 		};
		struct  
		{
			bool culled;
		}polygon;
	}asm_state;

	// vertex data
// 	struct  
// 	{
// 		GLfloat s,t,r,q;
// 	}cur_tex_coord;
	GLfloat cur_tex_coord[4];
	Normal cur_normal/*[3]*/;
	GLfloat cur_color[4];
	GLfloat cur_secondary_color[4]; // specular
	//GLfloat cur_fog_coord;
	//GLfloat cur_color_index;
	GLboolean cur_edge_flag;// move to flags

	// viewport
	struct  
	{
		GLint ox,oy,w,h;
	}viewport;
	GLfloat znear,zfar;

	// matrix
	// todo: use ubyte, and YGL_PROJECTION... instead
	GLuint cur_matrix_mode;
	Matrix44* cur_matrix;
	stack<Matrix44,YGL_MODELVIEW_STACK_DEPTH> matrix_mv;
	stack<Matrix44,YGL_PROJECTION_STACK_DEPTH> matrix_proj;
//	stack<Matrix44,MAX_MODLEVIEW> matrix_texs;
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
	GLubyte color_material_face;
	GLubyte cull_face_mode,front_face_mode;
	GLubyte shade_model;


	GLfloat clear_color[4];
	// 32
	GLuint clear_depth;

	unsigned long long enable_flags;
	//GLuint flags;
	GLuint last_error;

	// depth
	GLubyte depth_test_mask;
	GLubyte buffer_write_mask;

	// texture
	bitset<YGL_TEXTURE_NAMES_MAX> tex_names;
	TextureObject* tex_objs[YGL_TEXTURE_NAMES_MAX];
	TextureObject* tex_targets[YGL_TEX_TARGET_MAX];
	//TextureObject tex_defaults[YGL_TEX_TARGET_MAX];
	GLint tex_env_mode;
	GLfloat tex_env_color[4];
	//GLint tex_gen_mode[4];

	// fog
	GLubyte fog_mode/*,fog_source not in 1.1*/;
	GLfloat fog_density,fog_end,fog_start;
	GLfloat fog_color[4];
	GLfloat cached_fog_e_s; // inv(e-s)

	// cached values
	GLboolean cached_mv_inverse_valid;
	Matrix44 cached_mv_inverse;
};
extern GLContext glctx;

#endif