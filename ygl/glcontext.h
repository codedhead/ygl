#ifndef _YGL_CONTEXT_
#define _YGL_CONTEXT_

#include "ygl.h"
#include "yglconstants.h"
#include "yglstructs.h"
#include "stack.h"
#include "light.h"

#define YGL_IS_ENABLED(which) (glctx.enable_flags&(1<<(which)))

struct GLContext
{
	GLContext();
	void init(GLint width, GLint height);

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
			GLubyte complete_cnt;
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
	struct  
	{
		GLfloat s,t,r,q;
	}cur_tex_coord;
	GLfloat cur_normal[3];
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
	GLubyte cur_matrix_mode;
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
		GLenum color_control;
		GLboolean two_sided;
	}light_model;
	LightSource lights[YGL_MAX_LIGHTS];
	Material material_front,material_back;


	GLfloat clear_color[4];

	unsigned long long enable_flags;
	//GLuint flags;
	GLuint last_error;
};
extern GLContext glctx;

#endif