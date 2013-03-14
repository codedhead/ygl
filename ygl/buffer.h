#ifndef _YGL_BUFFER_
#define _YGL_BUFFER_

#include "ygltypes.h"
using namespace ygl;
namespace buffer
{
	GLubyte* framebuf_ptr(GLint x,GLint y);
	void plot(GLint x,GLint y,GLfloat* col);
	void clear_color(GLfloat r,GLfloat g,GLfloat b);
	// defined by viewport
	void draw(int l,int t,int w,int h);
	// update buffer line width
	void resize(int w,int h);
}

#endif