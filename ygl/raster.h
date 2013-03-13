#ifndef _YGL_RASTER_
#define _YGL_RASTER_

#include "ygltypes.h"
using namespace ygl;

struct Vertex;
namespace raster
{
#define line_bresenham raster::line
#define line_xiaolinwu raster::line_aa
	
	// bresenham
	void line(Vertex* v,GLfloat* color);
	// xiaolinwu
	void line_aa(Vertex* v);
	//void triangle(Vertex* v);
	void scanline(Vertex* v,int vcnt);
}

#endif