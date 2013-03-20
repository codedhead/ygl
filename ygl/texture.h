#ifndef _YGL_TEXTURE_
#define _YGL_TEXTURE_

#include "ygltypes.h"
using namespace ygl;
#include "yglconstants.h"

struct TexImage
{
	// internal format only support RGBA, GL_UNSIGNED_BYTE
	GLint width,height,border;
	//GLenum format,type;
	GLubyte* pixels;
};

// [0,1] -> [0,width-1]??
struct TextureObject
{
	TextureObject(){ctor();}
	~TextureObject();
	void ctor();

	GLuint /*name,*/dimension;
	// GL_CLAMP_TO_BORDER GL_MIRRORED_REPEAT
	GLint wrap_s,wrap_t,filter_min,filter_mag;
	GLfloat border_color[4];
	TexImage mipmaps[YGL_TEXTURE_MAX_LEVEL];

	void fetch(GLfloat s,GLfloat t,GLfloat* res);
	
};

#endif