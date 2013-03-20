#ifndef _YGL_BUFFER_
#define _YGL_BUFFER_

#include "ygltypes.h"
using namespace ygl;
namespace buffer
{
	extern GLubyte z_test_mask;
	extern GLboolean z_write_mask;

	typedef void (*plot_ubytef_func)(GLint x,GLint y,GLfloat z,GLfloat* col);
	typedef void (*plot_ubyte_func)(GLint x,GLint y,GLfloat z,GLubyte* col);
	typedef void (*plot_float_func)(GLint x,GLint y,GLfloat z,GLfloat* col);

	GLubyte* framebuf_ptr(GLint x,GLint y);
	GLuint* depthbuf_ptr(GLint x,GLint y);
	void plot_float_no_ztest(GLint x,GLint y,GLfloat z,GLfloat* col);
	void plot_ubytef_no_ztest(GLint x,GLint y,GLfloat z,GLfloat* col);
	void plot_ubyte_no_ztest(GLint x,GLint y,GLfloat z,GLubyte* col);

	void plot_float(GLint x,GLint y,GLfloat z,GLfloat* col);
	void plot_ubytef(GLint x,GLint y,GLfloat z,GLfloat* col);
	void plot_ubyte(GLint x,GLint y,GLfloat z,GLubyte* col);

	namespace fragment
	{
		// scissor test -> alpha test 
		// -> stencil test -> depth test(may modify stencil)
		// -> dither

		void plot(GLint x,GLint y,GLuint z,GLubyte* rgba)

		// fragment includes z and color
		void plot(GLubyte* cbuf,GLubyte* col);
	}

	void clear_depth(GLuint d);
	void clear_color(GLfloat r,GLfloat g,GLfloat b);
	// defined by viewport
	void draw(int l,int t,int w,int h);
	// update buffer line width
	void resize(int w,int h);
}

#endif