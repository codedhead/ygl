#ifndef _YGL_CLIP_
#define _YGL_CLIP_

#include "ygltypes.h"
struct Vertex;
using namespace ygl;

namespace clip
{
	GLboolean line(const Vertex* const a,const Vertex* const b,Vertex* clip_buf);
	GLboolean point(const Vertex* vert);
	GLboolean polygon(const Vertex** verts,GLint vcnt,Vertex* clip_buf,GLint* clip_cnt);
}


#endif