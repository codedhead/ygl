#ifndef _YGL_GLTYPES_H_
#define _YGL_GLTYPES_H_

#include "ymath.h"
#include "ygl.h"
#include "bitset.h"
#include "texture.h"
#include "light.h"
#include "stack.h"

struct GLVertex
{
	GLdouble coord[4];
	GLdouble normal[4];
	GLdouble front_color[4],back_color[4],*color;
	GLdouble tex_coord[4];
	GLMaterial material_front;
	GLMaterial material_back;
	GLdouble w;// 1.0/coord[3]
	GLdouble specular_front_color[4],specular_back_color[4];
};

class GLContext
{
public:
	GLContext();
	~GLContext();	

	void mapToViewPort(GLVertex* vt);

	int shading_mode;

	int matrix_mode;
	stack<GL_MAX_MODELVIEW_STACK_DEPTH> modleview_matrix_stack;
	stack<GL_MAX_MODELVIEW_STACK_DEPTH> projection_matrix_stack;
	Matrix44* current_matrix;
	Matrix44 modleview_matrix_inv;
	bool modleview_matrix_inv_dirty;

	GLdouble clear_depth;
	GLdouble clear_color[4];

	GLVertex current_vertex;

	//front? back?
	bool color_material_front_enabled;
	int color_material_front_mode;
	bool color_material_back_enabled;
	int color_material_back_mode;

	bool light_enabled;	
	GLLightSource lights[GL_MAX_LIGHTS];

	// light model
	GLfloat global_ambient_intensity[4];
	bool local_viewer;
	bool two_side;
	int color_control;	


	int begin_type;

	GLVertex vertexs[MAX_VERTEX_BUF];
	int vertex_cnt;

	int view_x,view_y,view_width,view_height;
	GLdouble z_near,z_far;

	GLTextureObject* tex_objects[MAX_TEXOBJ_CNT+1],*current_tex_object;
	BitSet<MAX_TEXOBJ_CNT+1> tex_names;
	
	int texture_enabled;	

	GLfloat tex_env_color[4];
	int tex_env_mode;

	//blending
	bool blend_enabled;
	int blend_sfactor,blend_dfactor;

	//depth
	bool depth_test;
	bool depth_mask;
	int depth_func;

	//fogging
	bool fog_enabled;
	float fog_color[4];
	float fog_start;
	float fog_end;
	int fog_mode;
	int fog_hint;
	float fog_density;

	//cull
	bool face_cull_enabled;
	int face_cull_mode;
	int front_face;
	/* quick determine which face to cull */
	int face_cull_determine;

	int front_polygon_mode;
	int back_polygon_mode;

	// stipple
	int line_stipple_enabled;
	int line_stipple_factor;
	unsigned short line_stipple_pattern;

};

#endif


