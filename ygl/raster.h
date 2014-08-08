#ifndef _YGL_RASTER_
#define _YGL_RASTER_

#include "ygltypes.h"
using namespace ygl;

struct Vertex;
struct TextureObject;
namespace ygl{
namespace raster
{
#define line_bresenham raster::line
#define line_xiaolinwu raster::line_aa

	enum FragmentShaderType
	{
		FragFlat=1,
		FragSmooth=(1<<1),
		FragTex=(1<<2),
		FragFog=(1<<3),
		FragPersp=(1<<4),
		//FragPerPixelLighting
	};

// 	enum ETRecAttribColor
// 	{
// 		// EAttrib_COLOR_START
// 		EAttrib_COLOR_R,
// 		EAttrib_COLOR_G,
// 		EAttrib_COLOR_B,
// 		EAttrib_COLOR_A,
// 		EAttrib_COLOR_COMPS,
// 	};
// 	enum ETRecAttribTex
// 	{
// 		// EAttrib_TEX_START
// 		EAttrib_TEX_S,
// 		EAttrib_TEX_T,
// 		EAttrib_TEX_COMPS,
// 	};

// #define SETUP_ATTRIB_NOP(attr_index,from) \
// 	att[attr_index]=(from);
// 
// #define SETUP_DATTRIB_NOP(attr_index,to) \
// 	d_att_dy[attr_index]=((to)-att[attr_index])*_dy;
// 
// #define GET_ATTRIB_NOP(attr_index) (att_np[attr_index])
// 
// #define SETUP_ATTRIB_P(attr_index,from) \
// 	att_w[attr_index]=(from)*_w;
// #define SETUP_DATTRIB_P(attr_index,to) \
// 	d_att_w_dy[attr_index]=((to)*theother_w-att_w[attr_index])*_dy;
// 
// #define GET_ATTRIB_P(attr_index) (att_p_w[attr_index]*w)
// 
// #define SETUP_ATTRIB_AND_DELTA(attr_index,from,to) \
// 	SETUP_ATTRIB(attr_index,from);\
// 	SETUP_DATTRIB(attr_index,to);
	


	//todo: line_record (incre)
	

	void point(Vertex* v);
	// bresenham
	void line(Vertex* v);
	// lerp color
	void line_smooth(Vertex* v);
	// xiaolinwu
	void line_aa(Vertex* v);
	//void triangle(Vertex* v);
	void scanline(Vertex* v,int vcnt,GLenum option,GLubyte* flat_color,TextureObject* texobj);
}
}
#endif