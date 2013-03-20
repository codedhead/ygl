#ifndef _YGL_CLIP_
#define _YGL_CLIP_

#include "ygltypes.h"
struct Vertex;
using namespace ygl;

namespace clip
{
	// is it linear in clip space??
	// that is : is the calced u wrt homogeneous coord

	GLboolean line(const Vertex* const a,const Vertex* const b,Vertex* clip_buf);
	GLboolean point(const Vertex* vert);
	// result store in clip_buf
	// vertex passed and clipped in homogeneous space
	GLboolean polygon(Vertex* clip_buf,GLint vcnt,GLint* clip_cnt);
}


#endif