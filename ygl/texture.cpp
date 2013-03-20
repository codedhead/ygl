#include "texture.h"
#include "global.h"
#include "ygl.h"

#include <stdlib.h>
#include <string.h>

GLuint name,dimension;
// GL_CLAMP_TO_BORDER GL_MIRRORED_REPEAT
TextureObject::~TextureObject()
{
	for(int i=0;i<YGL_TEXTURE_MAX_LEVEL;++i)
	{
		if(mipmaps[i].pixels) free(mipmaps[i].pixels);
	}
}
void TextureObject::ctor()
{
// 	filter_mag=GL_LINEAR;
// 	filter_min=GL_NEAREST_MIPMAP_LINEAR;
	filter_min=filter_mag=GL_NEAREST;
	wrap_s=wrap_t=GL_REPEAT;
	ASSIGN_F4_WITH(border_color,0.f,0.f,0.f,0.f);
	
	dimension=0;//???
	memset(mipmaps,0,sizeof(mipmaps));
}
void TextureObject::fetch(GLfloat s,GLfloat t,GLfloat* res)
{
	GLfloat ss,tt,si,ti;
	if(wrap_s==GL_REPEAT) ss=modf(s,&si);
	else ss=YGL_CLAMP_MIN_MAX(s,0.f,1.f);
	if(wrap_t==GL_REPEAT) tt=modf(t,&ti);
	else tt=YGL_CLAMP_MIN_MAX(t,0.f,1.f);

	TexImage* img=mipmaps+0;
	const GLvoid* pixels=img->pixels;
	if(!pixels)
	{
		ASSIGN_F3_WITH(res,0.f,0.f,0.f);
		return;
	}
	//s->x, t->y
	GLfloat fx=(img->width-1)*ss,fy=(img->height-1)*tt;
#define PIXEL_ELE_SIZE (sizeof(GLubyte)*4)
	GLint line_width=img->width*PIXEL_ELE_SIZE;

	GLubyte* p;

#define PIXEL_INDEX(y,x) ((y)*(line_width)+x*PIXEL_ELE_SIZE)

	// determine minify or mag
	switch(filter_mag)
	{
	case GL_LINEAR:
		{
			// modf
			GLint ix=(GLint)fx,iy=(GLint)fy;
			GLfloat fraction_x=fx-ix,fraction_y=fy-iy;

			// both clamp
			GLubyte* p00=img->pixels+PIXEL_INDEX(iy,ix),
				*p10=p00+PIXEL_ELE_SIZE,
				*p01=p00+line_width,
				*p11=p01+PIXEL_ELE_SIZE;

			if(ix==img->width-1)
			{
				if(wrap_s==GL_REPEAT)
					p10=p00+(PIXEL_ELE_SIZE-line_width);

				if(iy==img->height-1&&wrap_t==GL_REPEAT)
				{
					p01=img->pixels+PIXEL_INDEX(0,ix);
					p11=img->pixels;
				}
				else
				{
					// p01 already right?
					//p01=p00+line_width;
					p11=p10+line_width;
				}
			}
			else if(iy==img->height-1&&wrap_t==GL_REPEAT)
			{
				p01=img->pixels+PIXEL_INDEX(0,ix);
				p11=img->pixels;
			}

			GLfloat w00=(1-fraction_x)*(1-fraction_y),
				w10=fraction_x*(1-fraction_y),
				w01=(1-fraction_x)*fraction_y,
				w11=fraction_x*fraction_y;


			*res++=INV255*(
				w00*(*p00++)+
				w10*(*p10++)+
				w01*(*p01++)+
				w11*(*p11++));
			*res++=INV255*(
				w00*(*p00++)+
				w10*(*p10++)+
				w01*(*p01++)+
				w11*(*p11++));
			*res++=INV255*(
				w00*(*p00++)+
				w10*(*p10++)+
				w01*(*p01++)+
				w11*(*p11++));
			*res++=INV255*(
				w00*(*p00++)+
				w10*(*p10++)+
				w01*(*p01++)+
				w11*(*p11++));
		}
		break;
	case GL_NEAREST:
		p=img->pixels+PIXEL_INDEX(ROUND(fy),ROUND(fx));
		*res++=(*(p++))*INV255;
		*res++=(*(p++))*INV255;
		*res++=(*(p++))*INV255;
		*res++=(*(p++))*INV255;
		break;
	}
}