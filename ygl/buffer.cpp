#include "buffer.h"
#include "yglconstants.h"
#include "global.h"
#include "device.h"
#include "glcontext.h"

namespace ygl{
namespace buffer
{
	//GLubyte fragment[][][4]

	// index wrong!!!
	// we cluster all elements at the beginning of ther array

	//glPixelStore 1,2,4,8 align it


	// in bytes todo: can we do better??
#define BUFFER_ALIGN_BY_4BYTES_SIZE(sz) ((sz)+3)/4*4

	int buffer_height=YGL_MAX_BUFFER_HEIGHT,buffer_line_width=YGL_MAX_BUFFER_WIDTH,
		buffer_line_bytes_rgba=BUFFER_ALIGN_BY_4BYTES_SIZE(YGL_MAX_BUFFER_WIDTH*4);


#define BUFFER_INDEX_RGBA(y,x) ((y)*buffer_line_bytes_rgba+4*(x))
#define BUFFER_INDEX_DEPTH(y,x) ((y)*buffer_line_width+(x))
#define BUFFER_INDEX_STENCIL(y,x) ((y)*buffer_line_width+(x))


	// RGBA
	cbuf_type color_buffer_frontleft[YGL_MAX_BUFFER_HEIGHT*
		BUFFER_ALIGN_BY_4BYTES_SIZE(YGL_MAX_BUFFER_WIDTH*4)+4]; // alpha??
	// int32
	zbuf_type depth_buffer[YGL_MAX_BUFFER_HEIGHT*YGL_MAX_BUFFER_WIDTH+4];
	sbuf_type stencil_buffer[YGL_MAX_BUFFER_HEIGHT*YGL_MAX_BUFFER_WIDTH+4];

	//GLubyte z_test_mask;

// 	GLboolean z_write_mask;
// 	GLubyte color_write_mask;


	// todo: mask
	/*inline void plot(GLint x,GLint y,GLfloat z,GLfloat* col)
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
	}*/
	GLubyte* framebuf_ptr(GLint x,GLint y)
	{
		return color_buffer_frontleft+BUFFER_INDEX_RGBA(y,x);
	}
	GLuint* depthbuf_ptr(GLint x,GLint y)
	{
		return depth_buffer+BUFFER_INDEX_DEPTH(y,x);
	}
	GLuint* stencilbuf_ptr(GLint x,GLint y)
	{
		return stencil_buffer+BUFFER_INDEX_STENCIL(y,x);
	}

	void clear_depth(zbuf_type d)
	{
		zbuf_type* p=depth_buffer,*pend=depth_buffer+(buffer_height*buffer_line_width);
		while(p<pend)
			*p++=d;
	}
	void clear_stencil(sbuf_type s)
	{
		sbuf_type* p=stencil_buffer,*pend=stencil_buffer+(buffer_height*buffer_line_width);
		while(p<pend)
			*p++=s;
	}

	void clear_color(cbuf_type r,cbuf_type g,cbuf_type b,cbuf_type a)
	{
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
			p=color_buffer_frontleft+BUFFER_INDEX_RGBA(y,0);
			for(int x=0;x<buffer_line_width;++x)
			{
				*p++=r;
				*p++=g;
				*p++=b;
				*p++=a;
			}
		}
	}

	// todo: sub l, r when plot??
	// so can directly pass in frame_buffer

	/*inline */void draw(int l,int t,int w,int h)
	{
		device::draw(color_buffer_frontleft/*+BUFFER_INDEX_RGB(t,l)*/,l,t,w,h);
	}

	void resize(int w,int h)
	{
		buffer_height=YGL_CLAMP_MAX(h,YGL_MAX_BUFFER_HEIGHT);
		buffer_line_width=YGL_CLAMP_MAX(w,YGL_MAX_BUFFER_WIDTH);
		buffer_line_bytes_rgba=BUFFER_ALIGN_BY_4BYTES_SIZE(buffer_line_width*4);
	}

	//namespace fragment
	//{
		// fragment:
		// window_x, window_y, z, rgba

		// ownership test
		// scissor test -> alpha test 
		// -> stencil test -> depth test(may modify stencil)
		// -> blending (conflicts with logicop?)
		//			(no support??)	-> dither
		// -> logicop

		GLboolean test_scissor=false,test_alpha=false,test_stencil=false,test_depth=false,
			enabled_blend=false,enabled_logicop=false,
			write_z=true,write_r=true,write_g=true,write_b=true,write_a=true;
		sbuf_type write_s_mask=0xffffffff;

		// todo: optimize, also consider write mask
		void plot(GLint x,GLint y,GLuint z,cbuf_type* rgba)
// 		{
// 			plot(x,y,z,color_buffer_frontleft+BUFFER_INDEX_RGBA(y,x),
// 				depth_buffer+BUFFER_INDEX_DEPTH(y,x),rgba);
// 		}
// 		void plot(GLint x,GLint y,zbuf_type z,cbuf_type* cbuf,zbuf_type* zbuf,cbuf_type* rgba)
		{
			// write mask

			// ownership test
			if(test_scissor&&(x<glctx.scissor_left||x>glctx.scissor_right||
				y<glctx.scissor_bottom||y>glctx.scissor_top))
			{
				return;
			}
			if(test_alpha&&!(COMPARE_UINT(rgba[3],glctx.alpha_ref)&glctx.alpha_test_mask))
			{
				return;
			}

			//+(zbuf-depth_buffer)
			sbuf_type* sbuf=stencil_buffer+BUFFER_INDEX_STENCIL(y,x);
			// todo: cache glctx.stencil_ref&glctx.stencil_mask
			if(test_stencil&&!(
				compare_uint32((glctx.stencil_ref&glctx.stencil_mask),(*sbuf&glctx.stencil_mask))&glctx.stencil_test_mask))
			{
				sbuf_type dst=*sbuf;
				switch(glctx.stencil_op_sfail)
				{
				case GL_KEEP:break;
				case GL_ZERO:dst=0;break;
				case GL_REPLACE:dst=glctx.stencil_ref;break;
				case GL_INCR:if(dst<STENCIL_BUFFER_MAX_RES)++dst;break;// clamp
				case GL_DECR:if(dst) --dst;break;// clamp
				case GL_INVERT:dst=~dst;break;
				}
				*sbuf=BIT_MASK(*sbuf,dst,write_s_mask);

				return;
			}
			
			if(test_depth)
			{
				zbuf_type* zbuf=depth_buffer+BUFFER_INDEX_DEPTH(y,x);

				if(COMPARE_UINT(z,*zbuf)&glctx.depth_test_mask) // z-pass
				{
					sbuf_type dst=*sbuf;
					switch(glctx.stencil_op_dppass)
					{
					case GL_KEEP:break;
					case GL_ZERO:dst=0;break;
					case GL_REPLACE:dst=glctx.stencil_ref;break;
					case GL_INCR:if(dst<STENCIL_BUFFER_MAX_RES)++dst;break;// clamp
					case GL_DECR:if(dst) --dst;break;// clamp
					case GL_INVERT:dst=~dst;break;
					}
					*sbuf=BIT_MASK(*sbuf,dst,write_s_mask);
				}
				else // z-fail
				{
					sbuf_type dst=*sbuf;
					switch(glctx.stencil_op_dpfail)
					{
					case GL_KEEP:break;
					case GL_ZERO:dst=0;break;
					case GL_REPLACE:dst=glctx.stencil_ref;break;
					case GL_INCR:if(dst<STENCIL_BUFFER_MAX_RES)++dst;break;// clamp
					case GL_DECR:if(dst) --dst;break;// clamp
					case GL_INVERT:dst=~dst;break;
					}
					*sbuf=BIT_MASK(*sbuf,dst,write_s_mask);
					return;
				}
			}
			
			

			// no dithering


			cbuf_type* cbuf=color_buffer_frontleft+BUFFER_INDEX_RGBA(y,x);

			// if no colormask

			// logic-op
			// blending
			if(enabled_logicop&&glctx.logicop!=GL_NOOP)
			{
				GLubyte opres[4];
				GLuint* cbuf_32=(GLuint*)cbuf,*rgba_32=(GLuint*)rgba,*res_32=(GLuint*)opres;
				switch(glctx.logicop)
				{
				case GL_CLEAR:
					//ASSIGN_V4_WITH(cbuf,0,0,0,0);
					res_32=0;
					break;
				case GL_AND:
					*res_32=*rgba_32&*cbuf_32;
// 					cbuf[0]=rgba[0]&cbuf[0];
// 					cbuf[0]=rgba[0]&cbuf[0];
// 					cbuf[0]=rgba[0]&cbuf[0];
// 					cbuf[0]=rgba[0]&cbuf[0];
					break;
				case GL_AND_REVERSE:
					*res_32=*rgba_32&(~(*cbuf_32));
					break;
				case GL_COPY:
					ASSIGN_V4(cbuf,rgba);
					break;
				case GL_AND_INVERTED:
					*res_32=(~(*rgba_32))&*cbuf_32;
					break;
// 				case GL_NOOP:
// 					break;
				case GL_XOR:
					*res_32=*rgba_32^*cbuf_32;
					break;
				case GL_OR:
					*res_32=*rgba_32|*cbuf_32;
					break;
				case GL_NOR:
					*res_32=~(*rgba_32|*cbuf_32);
					break;
				case GL_EQUIV:
					*res_32=~(*rgba_32^*cbuf_32);
					break;
				case GL_INVERT:
					*res_32=~(*cbuf_32);
					break;
				case GL_OR_REVERSE:
					*res_32=*rgba_32|(~(*cbuf_32));
					break;
				case GL_COPY_INVERTED:
					*res_32=~(*rgba_32);
					break;
				case GL_OR_INVERTED:
					*res_32=(~(*rgba_32))|*cbuf_32;
					break;
				case GL_NAND:
					*res_32=~(*rgba_32&*cbuf_32);
					break;
				case GL_SET:
					//ASSIGN_V4_WITH(cbuf,1,1,1,1);
					*res_32=0x01010101;
					break;
				}
				if(write_r) cbuf[0]=opres[0];
				if(write_g) cbuf[1]=opres[1];
				if(write_b) cbuf[2]=opres[2];
				if(write_a) cbuf[3]=opres[3];
			}
			else if(enabled_blend)
			{
				// 255
				GLushort blend_res[4]={0};
				// src: rgba
				// dst: cbuf
				
				switch(glctx.blend_sfactor)
				{
				case GL_ZERO:
					ASSIGN_V4_WITH(blend_res,0,0,0,0);
					break;
				case GL_ONE:
					ASSIGN_V4(blend_res,rgba);
					break;
				case GL_DST_COLOR:
					blend_res[0]=cbuf[0]*rgba[0];
					blend_res[1]=cbuf[1]*rgba[1];
					blend_res[2]=cbuf[2]*rgba[2];
					blend_res[3]=cbuf[3]*rgba[3];
					break;
				case GL_ONE_MINUS_DST_COLOR:
					blend_res[0]=(255-cbuf[0])*rgba[0];
					blend_res[1]=(255-cbuf[1])*rgba[1];
					blend_res[2]=(255-cbuf[2])*rgba[2];
					blend_res[3]=(255-cbuf[3])*rgba[3];
					break;
				case GL_SRC_ALPHA:
					{
						GLushort alpha=rgba[3];
						blend_res[0]=alpha*rgba[0];
						blend_res[1]=alpha*rgba[1];
						blend_res[2]=alpha*rgba[2];
						blend_res[3]=alpha*rgba[3];
					}
					break;
				case GL_ONE_MINUS_SRC_ALPHA:
					{
						GLushort _1_alpha=255-rgba[3];
						blend_res[0]=_1_alpha*rgba[0];
						blend_res[1]=_1_alpha*rgba[1];
						blend_res[2]=_1_alpha*rgba[2];
						blend_res[3]=_1_alpha*rgba[3];
					}
					break;
				case GL_DST_ALPHA:
					{
						GLushort alpha=cbuf[3];
						blend_res[0]=alpha*rgba[0];
						blend_res[1]=alpha*rgba[1];
						blend_res[2]=alpha*rgba[2];
						blend_res[3]=alpha*rgba[3];
					}
					break;
				case GL_ONE_MINUS_DST_ALPHA:
					{
						GLushort _1_alpha=255-cbuf[3];
						blend_res[0]=_1_alpha*rgba[0];
						blend_res[1]=_1_alpha*rgba[1];
						blend_res[2]=_1_alpha*rgba[2];
						blend_res[3]=_1_alpha*rgba[3];
					}
					break;
				case GL_SRC_ALPHA_SATURATE:
					{
						// todo: min
						GLushort f=255-cbuf[3];
						if(rgba[3]<f) f=rgba[3];
						blend_res[0]=f*rgba[0];
						blend_res[1]=f*rgba[1];
						blend_res[2]=f*rgba[2];
						blend_res[3]=rgba[3];
					}
					break;
				}

				switch(glctx.blend_dfactor)
				{
				case GL_ZERO:
					break;
				case GL_ONE:
					blend_res[0]+=cbuf[0];
					blend_res[1]+=cbuf[1];
					blend_res[2]+=cbuf[2];
					blend_res[3]+=cbuf[3];
					break;
				case GL_SRC_COLOR:
					blend_res[0]+=cbuf[0]*rgba[0];
					blend_res[1]+=cbuf[1]*rgba[1];
					blend_res[2]+=cbuf[2]*rgba[2];
					blend_res[3]+=cbuf[3]*rgba[3];
					break;
				case GL_ONE_MINUS_SRC_COLOR:
					blend_res[0]+=(255-rgba[0])*cbuf[0];
					blend_res[1]+=(255-rgba[1])*cbuf[1];
					blend_res[2]+=(255-rgba[2])*cbuf[2];
					blend_res[3]+=(255-rgba[3])*cbuf[3];
					break;
				case GL_SRC_ALPHA:
					{
						GLushort alpha=rgba[3];
						blend_res[0]+=alpha*cbuf[0];
						blend_res[1]+=alpha*cbuf[1];
						blend_res[2]+=alpha*cbuf[2];
						blend_res[3]+=alpha*cbuf[3];
					}
					break;
				case GL_ONE_MINUS_SRC_ALPHA:
					{
						GLushort _1_alpha=255-rgba[3];
						blend_res[0]+=_1_alpha*cbuf[0];
						blend_res[1]+=_1_alpha*cbuf[1];
						blend_res[2]+=_1_alpha*cbuf[2];
						blend_res[3]+=_1_alpha*cbuf[3];
					}
					break;
				case GL_DST_ALPHA:
					{
						GLushort alpha=cbuf[3];
						blend_res[0]+=alpha*cbuf[0];
						blend_res[1]+=alpha*cbuf[1];
						blend_res[2]+=alpha*cbuf[2];
						blend_res[3]+=alpha*cbuf[3];
					}
					break;
				case GL_ONE_MINUS_DST_ALPHA:
					{
						GLushort _1_alpha=255-cbuf[3];
						blend_res[0]+=_1_alpha*cbuf[0];
						blend_res[1]+=_1_alpha*cbuf[1];
						blend_res[2]+=_1_alpha*cbuf[2];
						blend_res[3]+=_1_alpha*cbuf[3];
					}
					break;
				}
				if(write_r) cbuf[0]=INV255*blend_res[0];
				if(write_g) cbuf[1]=INV255*blend_res[1];
				if(write_b) cbuf[2]=INV255*blend_res[2];
				if(write_a) cbuf[3]=INV255*blend_res[3];
			}
			else
			{
				//ASSIGN_V4(cbuf,rgba);
				if(write_r) cbuf[0]=rgba[0];
				if(write_g) cbuf[1]=rgba[1];
				if(write_b) cbuf[2]=rgba[2];
				if(write_a) cbuf[3]=rgba[3];
			}
		}

		//void fast_plot(cbuf_type* cbuf,zbuf_type* zbuf,sbuf_type* sbuf,GLuint z,GLfloat* rgba);
	//}
}
}