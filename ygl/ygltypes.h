#ifndef _YGL_TYPES_
#define _YGL_TYPES_

namespace ygl{

typedef void			GLvoid;
typedef bool			GLboolean;
typedef char			GLbyte;
typedef unsigned char	GLubyte;
typedef short			GLshort;
typedef unsigned short	GLushort;
typedef int				GLint;
typedef unsigned int	GLuint;
typedef unsigned int	GLsizei;
typedef unsigned int	GLenum;
typedef GLint*			GLintptr;
typedef GLsizei*		GLsizeiptr;
typedef unsigned int	GLbitfield;
typedef float			GLfloat;
typedef float			GLclampf;
typedef double			GLdouble;
typedef double			GLclampd;

// buffer type
typedef GLubyte cbuf_type;
typedef GLuint zbuf_type;
//typedef GLubyte zbuf_type;
typedef GLuint sbuf_type;

}
#endif