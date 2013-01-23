#ifndef _YGL_TEXTURE_H_
#define _YGL_TEXTURE_H_

#include "ygl.h"

struct GLTextureImage
{
	int width,height,border;
	int format,type;
	void* texels;
};

typedef void (*TEXFUNC)(GLdouble,GLdouble,byte*);
typedef void (*TEXFUNCf)(GLdouble,GLdouble,float*);

extern TEXFUNC tex2DFuncs[];
extern TEXFUNCf tex2DFuncsf[];

//only support 2d texture in this version
struct GLTextureObject
{	
public:
	//	int wrap_s_1d;
	int wrap_s_2d;
	int wrap_t_2d;

	int filter_2d;
	// 	int mag_filter_1d;
	// 	int min_filter_1d;
	// 	int mag_filter_2d;
	// 	int min_filter_2d;

	int name;

	//GLTextureImage tex_image_1d[MAX_TEXTURE_LEVEL];
	GLTextureImage tex_image_2d[MAX_TEXTURE_LEVEL];
	//GLTextureImage tex_image_3d[MAX_TEXTURE_LEVEL];

	int components_cnt;
	int components_size;
	int units,width_bytes;
	GLTextureObject()
	{
		//		wrap_s_1d=GL_CLAMP;
		wrap_s_2d=GL_CLAMP;
		wrap_t_2d=GL_CLAMP;
		// 		mag_filter_1d=GL_NEAREST;
		// 		min_filter_1d=GL_NEAREST;
		// 		mag_filter_2d=GL_NEAREST;
		// 		min_filter_2d=GL_NEAREST;
		filter_2d=GL_NEAREST;
		components_cnt=components_size=0;
	}	
};


#endif