#ifndef _YGL_TEXTURE_
#define _YGL_TEXTURE_

#include "ygltypes.h"
using namespace ygl;
#include "yglconstants.h"

struct TexImage
{
	// internal format only support RGBA, GL_UNSIGNED_BYTE, alignment 4!!(thus, if no border, no need to align)
	GLint width,height,border;
	//GLint internalformat;
	GLubyte* pixels;
};

//GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
struct TextureObject
{
	TextureObject(){ctor();}
	~TextureObject();
	void ctor();

	GLuint /*name,*/dimension;
	// GL_CLAMP_TO_BORDER GL_MIRRORED_REPEAT
	GLint wrap_s,wrap_t,filter_min,filter_mag;
	GLboolean auto_gen_mipmap;
	GLfloat border_color[4];
	TexImage mipmaps[YGL_TEXTURE_MAX_LEVEL];
	GLfloat c;
	GLint max_level;

	inline GLint width(){return mipmaps[0].width;}
	inline GLint height(){return mipmaps[0].height;}

	void fetch255(GLfloat rho,GLfloat s,GLfloat t,GLubyte* res);
	void fetch(GLfloat rho,GLfloat s,GLfloat t,GLfloat* res);

	void gen_mipmap();

private:
	void fetch255(TexImage* img,GLint filter_mode,GLfloat s,GLfloat t,GLubyte* res);
};

#endif