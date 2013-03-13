#include "buffer.h"
#include "yglconstants.h"
#include "global.h"
#include "device.h"

namespace buffer
{
	//GLubyte fragment[][][4]

	// index wrong!!!
	// we cluster all elements at the beginning of ther array

	//glPixelStore 1,2,4,8 align it


	// in bytes todo: can we do better??
#define BUFFER_ALIGN_BY_4BYTES_SIZE(sz) ((sz)+3)/4*4

	int buffer_line_width=YGL_MAX_BUFFER_WIDTH,
		buffer_line_bytes_rgb=BUFFER_ALIGN_BY_4BYTES_SIZE(YGL_MAX_BUFFER_WIDTH*3);


#define BUFFER_INDEX_RGB(y,x) ((y)*buffer_line_bytes_rgb+3*(x))


	GLubyte frame_buffer[YGL_MAX_BUFFER_HEIGHT*
		BUFFER_ALIGN_BY_4BYTES_SIZE(YGL_MAX_BUFFER_WIDTH*3)+4]; // alpha??
// 	GLubyte depth_buffer[YGL_MAX_BUFFER_HEIGHT][YGL_MAX_BUFFER_WIDTH];
// 	GLubyte alpha_buffer[YGL_MAX_BUFFER_HEIGHT][YGL_MAX_BUFFER_WIDTH];
	//GLubyte stencil_buffer[][][3];

	/*inline */void plot(GLint x,GLint y,GLfloat* col)
	{
		GLubyte* p=frame_buffer+BUFFER_INDEX_RGB(y,x);
		*p++=YGL_COLOR_F2I(col[0]);
		*p++=YGL_COLOR_F2I(col[1]);
		*p++=YGL_COLOR_F2I(col[2]);
	}
	GLubyte* framebuf_ptr(GLint x,GLint y)
	{
		return frame_buffer+BUFFER_INDEX_RGB(y,x);
	}

// 	void clear_depth()
// 	{
// 
// 	}

	void clear_color(GLfloat rf,GLfloat gf,GLfloat bf)
	{
		int r=YGL_COLOR_F2I(rf),g=YGL_COLOR_F2I(gf),b=YGL_COLOR_F2I(bf);
		GLubyte* p;
		for(int y=0;y<YGL_MAX_BUFFER_HEIGHT;++y)
			for(int x=0;x<YGL_MAX_BUFFER_WIDTH;++x)
			{
				p=frame_buffer+BUFFER_INDEX_RGB(y,x);
				*p++=r;
				*p++=g;
				*p++=b;
			}
	}

	/*inline */void draw(int l,int t,int r,int b)
	{
		device::draw(frame_buffer,l,t,r,b);
	}

	void resize(int w,int h)
	{
		buffer_line_width=YGL_CLAMP_MAX(w,YGL_MAX_BUFFER_WIDTH);
		buffer_line_bytes_rgb=BUFFER_ALIGN_BY_4BYTES_SIZE(buffer_line_width*3);
	}
}
