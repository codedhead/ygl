#include "../include/gltypes.h"
#include "../include/ymath.h"
#include "../include/ygl.h"


GLContext::GLContext()
{
	shading_mode=GL_FLAT;
	begin_type=GL_NOBEGIN;

	matrix_mode=GL_MODELVIEW;
	current_matrix=modleview_matrix_stack.peek();

	current_vertex.color=current_vertex.front_color;
	current_vertex.front_color[0]=1.0;
	current_vertex.front_color[1]=1.0;
	current_vertex.front_color[2]=1.0;
	current_vertex.front_color[3]=1.0;
	current_vertex.back_color[0]=1.0;
	current_vertex.back_color[1]=1.0;
	current_vertex.back_color[2]=1.0;
	current_vertex.back_color[3]=1.0;
	current_vertex.tex_coord[0]=1.0;
	current_vertex.tex_coord[1]=1.0;
	current_vertex.tex_coord[2]=1.0;
	current_vertex.tex_coord[3]=1.0;


	clear_color[0]=0.0;
	clear_color[1]=0.0;
	clear_color[2]=0.0;
	clear_color[3]=1.0;
	clear_depth=1.0;

	view_x=view_y=0;
	view_width=0;view_height=0;
	z_far=z_near=0.0;

	texture_enabled=0;
	light_enabled=false;
	current_tex_object=NULL;
	tex_env_mode=GL_REPLACE;

	vertex_cnt=0;


	//default
	current_vertex.material_front.material_ambient[0]=0.2;
	current_vertex.material_front.material_ambient[1]=0.2;
	current_vertex.material_front.material_ambient[2]=0.2;
	current_vertex.material_front.material_ambient[3]=1.0;
	current_vertex.material_front.material_diffuse[0]=0.8;
	current_vertex.material_front.material_diffuse[1]=0.8;
	current_vertex.material_front.material_diffuse[2]=0.8;
	current_vertex.material_front.material_diffuse[3]=1.0;
	current_vertex.material_front.material_emission[0]=0.0;
	current_vertex.material_front.material_emission[1]=0.0;
	current_vertex.material_front.material_emission[2]=0.0;
	current_vertex.material_front.material_emission[3]=1.0;
	current_vertex.material_front.material_specular[0]=0.0;
	current_vertex.material_front.material_specular[1]=0.0;
	current_vertex.material_front.material_specular[2]=0.0;
	current_vertex.material_front.material_specular[3]=1.0;
	current_vertex.material_front.material_shininess=20.0;

	current_vertex.material_back.material_ambient[0]=0.2;
	current_vertex.material_back.material_ambient[1]=0.2;
	current_vertex.material_back.material_ambient[2]=0.2;
	current_vertex.material_back.material_ambient[3]=1.0;
	current_vertex.material_back.material_diffuse[0]=0.8;
	current_vertex.material_back.material_diffuse[1]=0.8;
	current_vertex.material_back.material_diffuse[2]=0.8;
	current_vertex.material_back.material_diffuse[3]=1.0;
	current_vertex.material_back.material_emission[0]=0.0;
	current_vertex.material_back.material_emission[1]=0.0;
	current_vertex.material_back.material_emission[2]=0.0;
	current_vertex.material_back.material_emission[3]=1.0;
	current_vertex.material_back.material_specular[0]=0.0;
	current_vertex.material_back.material_specular[1]=0.0;
	current_vertex.material_back.material_specular[2]=0.0;
	current_vertex.material_back.material_specular[3]=1.0;
	current_vertex.material_back.material_shininess=20.0;

	global_ambient_intensity[0]=0.2;
	global_ambient_intensity[1]=0.2;
	global_ambient_intensity[2]=0.2;
	global_ambient_intensity[3]=1.0;
	local_viewer=false;
	two_side=false;
	color_control=GL_SINGLE_COLOR;

	color_material_front_enabled=false;
	color_material_front_mode=GL_AMBIENT_AND_DIFFUSE;
	color_material_back_enabled=false;
	color_material_back_mode=GL_AMBIENT_AND_DIFFUSE;

	modleview_matrix_inv_dirty=true;

	//blending
	blend_enabled=false;
	blend_sfactor=GL_ONE;
	blend_dfactor=GL_ZERO;

	depth_test=true;
	depth_mask=true;
	depth_func=GL_LESS;
	depthFunc=depthFuncs[depth_func-GL_NEVER];

	//fogging
	fog_enabled=false;
	fog_color[0]=fog_color[1]=fog_color[2]=fog_color[3]=0.0;
	fog_start=0.0;
	fog_end=1.0;
	fog_mode=GL_EXP;
	fog_hint=GL_NICEST;
	fog_density=1.0;

	//cull
	front_face=GL_CCW;
	face_cull_enabled=false;
	face_cull_mode=GL_BACK;
	face_cull_determine=GL_CW;

	front_polygon_mode=GL_FILL;
	back_polygon_mode=GL_FILL;

	//stipple
	line_stipple_enabled=false;
	line_stipple_factor=1;
	line_stipple_pattern=0xffff;
}

GLContext::~GLContext()
{
	//delete matrix
}

void GLContext::mapToViewPort(GLVertex* vt)
{
	vt->w=1.0/vt->coord[3];
	vt->coord[0]*=vt->w;vt->coord[1]*=vt->w;vt->coord[2]*=vt->w;vt->coord[3]=1.0;
	vt->coord[0]=view_x+0.5*view_width*(vt->coord[0]+1.0);
	vt->coord[1]=view_y+0.5*view_height*(vt->coord[1]+1.0);
	vt->coord[2]=(vt->coord[2]+1.0)*0.5;
	if(texture_enabled)
	{
		vt->tex_coord[0]*=vt->w;
		vt->tex_coord[1]*=vt->w;
		vt->tex_coord[2]*=vt->w;
		vt->tex_coord[3]*=vt->w;
	}	
}