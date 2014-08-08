#ifndef _YGL_BUFFER_
#define _YGL_BUFFER_

#include "ygltypes.h"
using namespace ygl;

#define STENCIL_BUFFER_MAX_RES 0xffffffffu
namespace ygl{
namespace buffer
{
	//extern GLubyte z_test_mask;
	//extern GLboolean z_write_mask;
	extern GLboolean test_scissor,test_alpha,test_stencil,test_depth,
		enabled_blend,enabled_logicop,
		write_z,write_r,write_g,write_b,write_a;
	extern sbuf_type write_s_mask;

	cbuf_type* framebuf_ptr(GLint x,GLint y);
	zbuf_type* depthbuf_ptr(GLint x,GLint y);

// 	typedef void (*plot_ubytef_func)(GLint x,GLint y,GLfloat z,GLfloat* col);
// 	typedef void (*plot_ubyte_func)(GLint x,GLint y,GLfloat z,GLubyte* col);
// 	typedef void (*plot_float_func)(GLint x,GLint y,GLfloat z,GLfloat* col);
// 
// 	
// 	void plot_float_no_ztest(GLint x,GLint y,GLfloat z,GLfloat* col);
// 	void plot_ubytef_no_ztest(GLint x,GLint y,GLfloat z,GLfloat* col);
// 	void plot_ubyte_no_ztest(GLint x,GLint y,GLfloat z,GLubyte* col);
// 
// 	void plot_float(GLint x,GLint y,GLfloat z,GLfloat* col);
// 	void plot_ubytef(GLint x,GLint y,GLfloat z,GLfloat* col);
// 	void plot_ubyte(GLint x,GLint y,GLfloat z,GLubyte* col);

	//namespace fragment
	//{
		// fragment:
		// window_x, window_y, z, rgba

		// scissor test -> alpha test 
		// -> stencil test -> depth test(may modify stencil)
		// -> blending (conflicts with logicop?)
		// -> dither -> logicop

		// passed between 0,1?
		void plot(GLint x,GLint y,zbuf_type z,cbuf_type* rgba);

		//void plot(GLint x,GLint y,zbuf_type z,cbuf_type* cbuf,zbuf_type* zbuf,cbuf_type* rgba);
	//}

	void clear_depth(zbuf_type d);
	void clear_stencil(sbuf_type s);
	void clear_color(cbuf_type r,cbuf_type g,cbuf_type b,cbuf_type a);
	// defined by viewport
	void draw(int l,int t,int w,int h);
	// update buffer line width
	void resize(int w,int h);
}
}

#endif