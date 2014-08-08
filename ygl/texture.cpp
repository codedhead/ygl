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
	max_level=0;
	filter_mag=GL_LINEAR;
	filter_min=GL_NEAREST_MIPMAP_LINEAR;
	c=0.5f;
	//filter_min=filter_mag=GL_NEAREST;
	wrap_s=wrap_t=GL_REPEAT;
	ASSIGN_V4_WITH(border_color,0.f,0.f,0.f,0.f);
	auto_gen_mipmap=false;

	dimension=0;//???
	memset(mipmaps,0,sizeof(mipmaps));
}
#define TEXEL_SIZE (sizeof(GLubyte)*4)
#define TEXEL_INDEX(linew,x,y) ((y)*(linew)+x*TEXEL_SIZE)
void TextureObject::fetch255(TexImage* img,GLint filter_mode,GLfloat s,GLfloat t,GLubyte* res)
{
	const GLvoid* pixels=img->pixels;
	if(!pixels)
	{
		ASSIGN_V4_WITH(res,0.f,0.f,0.f,0.f);
		return;
	}

	GLint line_width=img->width*TEXEL_SIZE; // border, align


	// determine minify or mag
	switch(filter_mode)
	{
	case GL_LINEAR:
		{
			// u-0.5f, v-0.5f
			GLfloat u=s*img->width-0.5f,v=t*img->height-0.5f;
			GLint iu=floor(u),iv=floor(v);
			GLint i0,i1,j0,j1;
			if(wrap_s==GL_REPEAT)
			{
				i0=iu%img->width;
				i1=(i0+1)%img->width;
			}
			else // clamp
			{
				i0=iu;
				i1=i0+1;
			}
			
			if(wrap_t==GL_REPEAT)
			{
				j0=iv%img->height;
				j1=(j0+1)%img->height;
			}
			else // clamp
			{
				j0=iv;
				j1=j0+1;
			}

			GLfloat alpha=u-iu,beta=v-iv;
			GLfloat w00=(1-alpha)*(1-beta),
				w10=alpha*(1-beta),
				w01=(1-alpha)*beta,
				w11=alpha*beta;

			GLubyte *p00=img->pixels+TEXEL_INDEX(line_width,i0,j0),
				*p10=img->pixels+TEXEL_INDEX(line_width,i1,j0),
				*p01=img->pixels+TEXEL_INDEX(line_width,i0,j1),
				*p11=img->pixels+TEXEL_INDEX(line_width,i1,j1);

			/*

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

			*/
			*res++=/*INV255**/(
				w00*(*p00++)+
				w10*(*p10++)+
				w01*(*p01++)+
				w11*(*p11++));
			*res++=/*INV255**/(
				w00*(*p00++)+
				w10*(*p10++)+
				w01*(*p01++)+
				w11*(*p11++));
			*res++=/*INV255**/(
				w00*(*p00++)+
				w10*(*p10++)+
				w01*(*p01++)+
				w11*(*p11++));
			*res++=/*INV255**/(
				w00*(*p00++)+
				w10*(*p10++)+
				w01*(*p01++)+
				w11*(*p11++));
		}
		break;
	case GL_NEAREST:
		{
			GLint iu=(s==1.f?(img->width-1):floor(img->width*s)),
				iv=(t==1.f?(img->height-1):floor(img->height*t));

			GLubyte* p=img->pixels+TEXEL_INDEX(line_width,iu,iv);
			// 		*res++=(*(p++))/**INV255*/;
			// 		*res++=(*(p++))/**INV255*/;
			// 		*res++=(*(p++))/**INV255*/;
			// 		*res++=(*(p++))/**INV255*/;
			ASSIGN_V4(res,p);
			break;
		}
	}
}

void TextureObject::fetch255(GLfloat rho,GLfloat s,GLfloat t,GLubyte* res)
{
	GLfloat lambda=log(rho)*INV_Ln2;

	GLfloat ss,tt,si,ti;
	if(wrap_s==GL_REPEAT) ss=modf(s,&si);
	else ss=YGL_CLAMP_MIN_MAX(s,0.f,1.f);
	if(wrap_t==GL_REPEAT) tt=modf(t,&ti);
	else tt=YGL_CLAMP_MIN_MAX(t,0.f,1.f);
	

	if(lambda<=c) // magnify
	{
		fetch255(mipmaps+0,filter_mag,ss,tt,res);
	}
	else
	{
		if(filter_min==GL_NEAREST_MIPMAP_NEAREST
			||filter_min==GL_LINEAR_MIPMAP_NEAREST)
		{
			GLint d=floor(lambda+0.5f);
			if(lambda+0.5f==d) ++d;
			d=YGL_CLAMP_MIN_MAX(d,1,max_level);

			GLint fil_mode=(filter_min==GL_NEAREST_MIPMAP_NEAREST)?GL_NEAREST:GL_LINEAR;

			fetch255(mipmaps+d,fil_mode,ss,tt,res);
		}
		else if(filter_min==GL_NEAREST_MIPMAP_LINEAR
			||filter_min==GL_LINEAR_MIPMAP_LINEAR)
		{
			GLint fil_mode=(filter_min==GL_NEAREST_MIPMAP_LINEAR)?GL_NEAREST:GL_LINEAR;

			if(lambda>=max_level)
			{
				fetch255(mipmaps+max_level,fil_mode,ss,tt,res);
			}
			else
			{
				GLint d=floor(lambda+1.f);
				// sure d<=max_level
				d=YGL_CLAMP_MIN(d,1);
				GLfloat frac=lambda-d/*,alpha=1.f-beta*/;

				GLubyte res1[4];
				fetch255(mipmaps+d-1,fil_mode,ss,tt,res1);
				fetch255(mipmaps+d,fil_mode,ss,tt,res);
				res[0]=lerp(res1[0],res[0],frac);
				res[1]=lerp(res1[1],res[1],frac);
				res[2]=lerp(res1[2],res[2],frac);
				res[3]=lerp(res1[3],res[3],frac);
			}
		}
		else
		{
			fetch255(mipmaps+0,filter_min,ss,tt,res);
		}
	}

	
}
void TextureObject::fetch(GLfloat rho,GLfloat s,GLfloat t,GLfloat* res)
{
	GLubyte* ures=(GLubyte*)res;
	fetch255(rho,s,t,ures);
	res[3]=ures[3]*INV255;
	res[2]=ures[2]*INV255;
	res[1]=ures[1]*INV255;
	res[0]=ures[0]*INV255;
}
void TextureObject::gen_mipmap()
{
	Assert(mipmaps[0].pixels);
	GLint w=mipmaps[0].width,h=mipmaps[0].height;

	TexImage* parent=mipmaps+0,*img=mipmaps+1;
	GLint lv=1;
	w>>=1;h>>=1;
	GLubyte* p,*p00,*p01,*p10,*p11;
	while(w>1&&h>1)
	{
		img->width=w;
		img->height=h;
		if(!img->pixels)
			img->pixels=(GLubyte*)malloc(TEXEL_SIZE*w*h);

		// todo: border
		GLint img_lw=img->width*TEXEL_SIZE,parent_lw=parent->width*TEXEL_SIZE;

		for(int j=0;j<h;++j)
		{
			p=img->pixels+TEXEL_INDEX(img_lw,0,j);
			p00=parent->pixels+TEXEL_INDEX(parent_lw,0,j*2);
			p10=p00+4;
			p01=p00+parent_lw;
			p11=p01+4;

			for(int i=0;i<w;++i)
			{
				for(int k=0;k<4;++k)
				{
					*p++=0.25f*(
						(*p00++)
						+(*p10++)
						+(*p01++)
						+(*p11++)
						);
				}

				p00=p10;p10=p00+4;
				p01=p11;p11=p01+4;
			}
		}

		w>>=1;
		h>>=1;
		parent=img;
		++img;
	}
	while(w>1)
	{
		img->width=w;
		img->height=1;
		if(!img->pixels)
			img->pixels=(GLubyte*)malloc(TEXEL_SIZE*w);

		p=img->pixels+TEXEL_INDEX(img->width*TEXEL_SIZE,0,0);
		p00=parent->pixels+TEXEL_INDEX(parent->width*TEXEL_SIZE,0,0);
		p10=p00+4;

		for(int i=0;i<w;++i)
		{
			for(int k=0;k<4;++k)
			{
				*p++=0.5f*(
					(*p00++)+
					(*p10++));
			}
			p00=p10;p10=p00+4;
		}

		w>>=1;
		parent=img;
		++img;
	}
	while(h>1)
	{
		img->width=1;
		img->height=h;
		if(!img->pixels)
			img->pixels=(GLubyte*)malloc(TEXEL_SIZE*h);

		// todo: border
		GLint img_lw=img->width*TEXEL_SIZE,parent_lw=parent->width*TEXEL_SIZE;

		for(int j=0;j<h;++j)
		{
			p=img->pixels+TEXEL_INDEX(img_lw,0,j);
			p00=parent->pixels+TEXEL_INDEX(parent_lw,0,j*2);
			p01=p00+parent_lw;
			for(int k=0;k<4;++k)
			{
				*p++=0.5f*(
					(*p00++)+
					(*p10++));
			}
		}

		h>>=1;
		parent=img;
		++img;
	}

}

