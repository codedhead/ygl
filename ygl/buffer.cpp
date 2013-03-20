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

	int buffer_height=YGL_MAX_BUFFER_HEIGHT,buffer_line_width=YGL_MAX_BUFFER_WIDTH,
		buffer_line_bytes_rgb=BUFFER_ALIGN_BY_4BYTES_SIZE(YGL_MAX_BUFFER_WIDTH*3);


#define BUFFER_INDEX_RGB(y,x) ((y)*buffer_line_bytes_rgb+3*(x))
#define BUFFER_INDEX_DEPTH(y,x) ((y)*buffer_line_width+(x))


	GLubyte frame_buffer[YGL_MAX_BUFFER_HEIGHT*
		BUFFER_ALIGN_BY_4BYTES_SIZE(YGL_MAX_BUFFER_WIDTH*3)+4]; // alpha??
	// int32
	GLuint depth_buffer[YGL_MAX_BUFFER_HEIGHT*YGL_MAX_BUFFER_WIDTH+4];

	GLubyte z_test_mask;
	GLboolean z_write_mask;

	GLubyte rgba_write_mask;
// 	GLubyte depth_buffer[YGL_MAX_BUFFER_HEIGHT][YGL_MAX_BUFFER_WIDTH];
// 	GLubyte alpha_buffer[YGL_MAX_BUFFER_HEIGHT][YGL_MAX_BUFFER_WIDTH];
	//GLubyte stencil_buffer[][][3];

	// todo: mask
	/*inline */void plot(GLint x,GLint y,GLfloat z,GLfloat* col)
	{
		GLuint* dz=depth_buffer+BUFFER_INDEX_DEPTH(y,x);
		if(SIGN_OF_FLOAT(z-*dz)&z_test_mask)
		{
			GLubyte* p=frame_buffer+BUFFER_INDEX_RGB(y,x);
			*p++=YGL_COLOR_F2I(col[0]);
			*p++=YGL_COLOR_F2I(col[1]);
			*p++=YGL_COLOR_F2I(col[2]);

			*dz=z;
		}
	}
	void plot_no_ztest(GLint x,GLint y,GLfloat z,GLfloat* col)
	{
		GLubyte* p=frame_buffer+BUFFER_INDEX_RGB(y,x);
		*p++=YGL_COLOR_F2I(col[0]);
		*p++=YGL_COLOR_F2I(col[1]);
		*p++=YGL_COLOR_F2I(col[2]);
		depth_buffer[BUFFER_INDEX_DEPTH(y,x)]=z;
	}
	void plot_ubytef(GLint x,GLint y,GLfloat z,GLfloat* col)
	{
		GLuint* dz=depth_buffer+BUFFER_INDEX_DEPTH(y,x);
		if(SIGN_OF_FLOAT(z-*dz)&z_test_mask)
		{
			GLubyte* p=frame_buffer+BUFFER_INDEX_RGB(y,x);
			*p++=(int)(col[0]);
			*p++=(int)(col[1]);
			*p++=(int)(col[2]);

			*dz=z;
		}
	}
	void plot_ubytef_no_ztest(GLint x,GLint y,GLfloat z,GLfloat* col)
	{
		GLubyte* p=frame_buffer+BUFFER_INDEX_RGB(y,x);
		*p++=(int)(col[0]);
		*p++=(int)(col[1]);
		*p++=(int)(col[2]);
		depth_buffer[BUFFER_INDEX_DEPTH(y,x)]=z;
	}
	void plot_ubyte(GLint x,GLint y,GLfloat z,GLubyte* col)
	{
		GLuint* dz=depth_buffer+BUFFER_INDEX_DEPTH(y,x);
		if(SIGN_OF_FLOAT(z-*dz)&z_test_mask)
		{
			GLubyte* p=frame_buffer+BUFFER_INDEX_RGB(y,x);
			*p++=*col++;
			*p++=*col++;
			*p++=*col++;

			*dz=z;
		}
	}
	void plot_ubyte_no_ztest(GLint x,GLint y,GLfloat z,GLubyte* col)
	{
		GLubyte* p=frame_buffer+BUFFER_INDEX_RGB(y,x);
		*p++=*col++;
		*p++=*col++;
		*p++=*col++;
		depth_buffer[BUFFER_INDEX_DEPTH(y,x)]=z;
	}
	GLubyte* framebuf_ptr(GLint x,GLint y)
	{
		return frame_buffer+BUFFER_INDEX_RGB(y,x);
	}
	GLuint* depthbuf_ptr(GLint x,GLint y)
	{
		return depth_buffer+BUFFER_INDEX_DEPTH(y,x);
	}

	void clear_depth(GLuint d)
	{
		GLuint* p=depth_buffer,*pend=depth_buffer+(buffer_height*buffer_line_width);
		while(p<pend)
			*p++=d;
	}

	void clear_color(GLfloat rf,GLfloat gf,GLfloat bf)
	{
		int r=YGL_COLOR_F2I(rf),g=YGL_COLOR_F2I(gf),b=YGL_COLOR_F2I(bf);
		GLubyte* p;
// 		for(int y=0;y<YGL_MAX_BUFFER_HEIGHT;++y)
// 			for(int x=0;x<YGL_MAX_BUFFER_WIDTH;++x)
// 			{
// 				p=frame_buffer+BUFFER_INDEX_RGB(y,x);
// 				*p++=r;
// 				*p++=g;
// 				*p++=b;
// 			}
		for(int y=0;y<buffer_height;++y)
		{
			p=frame_buffer+BUFFER_INDEX_RGB(y,0);
			for(int x=0;x<buffer_line_width;++x)
			{
				
				*p++=r;
				*p++=g;
				*p++=b;
			}
		}
	}

	// todo: sub l, r when plot??
	// so can directly pass in frame_buffer

	/*inline */void draw(int l,int t,int w,int h)
	{
		device::draw(frame_buffer/*+BUFFER_INDEX_RGB(t,l)*/,l,t,w,h);
	}

	void resize(int w,int h)
	{
		buffer_height=YGL_CLAMP_MAX(h,YGL_MAX_BUFFER_HEIGHT);
		buffer_line_width=YGL_CLAMP_MAX(w,YGL_MAX_BUFFER_WIDTH);
		buffer_line_bytes_rgb=BUFFER_ALIGN_BY_4BYTES_SIZE(buffer_line_width*3);
	}

	namespace fragment
	{
		void plot(GLubyte* cbuf,GLubyte* col)
		{
			*cbuf++=*col++;
			*cbuf++=*col++;
			*cbuf++=*col++;
		}
	}
}
