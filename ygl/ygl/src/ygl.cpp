#include "../include/ygl.h"
#include "../include/ymath.h"
#include "../include/gltypes.h"
#include "../include/buf.h"
#include "../include/draw.h"
#include "../include/light.h"

GLContext gl_context;

void glShadeModel(int mode)
{
	gl_context.shading_mode=mode;
}

void glTranslatef(float x,float y,float z)
{
	glTranslated(x,y,z);
}

void glTranslated(GLdouble x, GLdouble y, GLdouble z)
{
	Matrix44 n;
	n.m[12]=x;n.m[13]=y;n.m[14]=z;
	gl_context.current_matrix->rightMul(n);
	gl_context.modleview_matrix_inv_dirty=true;
}

void glRotatef(float angle,float x,float y,float z)
{
	glRotated(angle,x,y,z);
}

void glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
	angle=angle*GL_PI/180.0;
	GLdouble ulen=sqrt(x*x+y*y+z*z);
	GLdouble ux=x/ulen,uy=y/ulen,uz=z/ulen;
	GLdouble cosA=cos(angle),sinA=sin(angle);
	GLdouble oneC=1-cosA;
	Matrix44 n;
	n.m[0]=ux*ux*oneC+cosA;
	n.m[4]=ux*uy*oneC-uz*sinA;
	n.m[8]=ux*uz*oneC+uy*sinA;
	n.m[1]=uy*ux*oneC+uz*sinA;
	n.m[5]=uy*uy*oneC+cosA;
	n.m[9]=uy*uz*oneC-ux*sinA;
	n.m[2]=uz*ux*oneC-uy*sinA;
	n.m[6]=uz*uy*oneC+ux*sinA;
	n.m[10]=uz*uz*oneC+cosA;

	gl_context.current_matrix->rightMul(n);
	gl_context.modleview_matrix_inv_dirty=true;
}

void glScalef(float x,float y,float z)
{
	glScaled(x,y,z);
}

void glScaled(GLdouble x, GLdouble y, GLdouble z)
{
	Matrix44 n;
	n.m[0]=x;n.m[5]=y;n.m[10]=z;
	gl_context.current_matrix->rightMul(n);
	gl_context.modleview_matrix_inv_dirty=true;
}

void glMatrixMode(int mode)
{
	if(mode==GL_MODELVIEW)
		gl_context.current_matrix=gl_context.modleview_matrix_stack.peek();
	else if(mode==GL_PROJECTION)
		gl_context.current_matrix=gl_context.projection_matrix_stack.peek();
	else return;
	gl_context.matrix_mode=mode;
}
void glLoadMatrixf(const float *m)
{
	gl_context.current_matrix->loadMatrixf(m);
}
void glLoadIdentity(void)
{
	gl_context.current_matrix->loadIdentity();
}
void glMultMatrixf(const float *m)
{
	gl_context.current_matrix->rightMul(m);
}
void glPushMatrix(void)
{
	if(gl_context.matrix_mode==GL_MODELVIEW)
	{
		gl_context.modleview_matrix_stack.push(*gl_context.current_matrix);
		gl_context.current_matrix=gl_context.modleview_matrix_stack.peek();
	}
	else if(gl_context.matrix_mode==GL_PROJECTION)
	{
		gl_context.projection_matrix_stack.push(*gl_context.current_matrix);
		gl_context.current_matrix=gl_context.projection_matrix_stack.peek();
	}
	
}
void glPopMatrix(void)
{
	if(gl_context.matrix_mode==GL_MODELVIEW)
	{		
		gl_context.current_matrix=gl_context.modleview_matrix_stack.pop();
	}
	else if(gl_context.matrix_mode==GL_PROJECTION)
	{		
		gl_context.current_matrix=gl_context.projection_matrix_stack.pop();
	}
}

void glClear(int mask)
{
	if(mask&GL_COLOR_BUFFER_BIT)
		glbufClearColorBuffer(gl_context.clear_color);
	if(mask&GL_DEPTH_BUFFER_BIT)
		glbufClearDepthBuffer(gl_context.clear_depth);
}

void glBegin(int type)
{
	if(gl_context.begin_type!=GL_NOBEGIN) glEnd();
	gl_context.vertex_cnt=0;
	gl_context.begin_type=type;
}

void glEnd()
{
	if(gl_context.begin_type==GL_NOBEGIN) return;

	draw(gl_context.begin_type,gl_context.vertexs,gl_context.vertex_cnt);
	gl_context.vertex_cnt=0;
	gl_context.begin_type=GL_NOBEGIN;	
}

//glvertex
void glVertex2fv(GLfloat *v)
{
	glVertex4d(v[0],v[1],0.0,1.0);
}
void glVertex3fv(GLfloat *v)
{
	glVertex4d(v[0],v[1],v[2],1.0);
}
void glVertex4fv(GLfloat *v)
{
	glVertex4d(v[0],v[1],v[2],v[3]);
}
void glVertex2f(GLfloat x,GLfloat y)
{
	glVertex4d(x,y,0.0,1.0);
}
void glVertex3f(GLfloat x,GLfloat y,GLfloat z)
{
	glVertex4d(x,y,z,1.0);
}
void glVertex4f(GLfloat x,GLfloat y,GLfloat z,GLfloat h)
{
	glVertex4d(x,y,z,h);
}
void glVertex2iv(GLint *v)
{
	glVertex4d(v[0],v[1],0.0,1.0);
}
void glVertex3iv(GLint *v)
{
	glVertex4d(v[0],v[1],v[2],1.0);
}
void glVertex4iv(GLint *v)
{
	glVertex4d(v[0],v[1],v[2],v[3]);
}
void glVertex2i(GLint x,GLint y)
{
	glVertex4d(x,y,0.0,1.0);
}
void glVertex3i(GLint x,GLint y,GLint z)
{
	glVertex4d(x,y,z,1.0);
}
void glVertex4i(GLint x,GLint y,GLint z,GLint h)
{
	glVertex4d(x,y,z,h);
}
void glVertex2dv(GLdouble *v)
{
	glVertex4d(v[0],v[1],0.0,1.0);
}
void glVertex3dv(GLdouble *v)
{
	glVertex4d(v[0],v[1],v[2],1.0);
}
void glVertex4dv(GLdouble *v)
{
	glVertex4d(v[0],v[1],v[2],v[3]);
}
void glVertex2d(GLdouble x,GLdouble y)
{
	glVertex4d(x,y,0.0,1.0);
}
void glVertex3d(GLdouble x,GLdouble y,GLdouble z)
{
	glVertex4d(x,y,z,1.0);
}
void glVertex4d(GLdouble x,GLdouble y,GLdouble z,GLdouble h)
{	
	gl_context.current_vertex.coord[0]=x;gl_context.current_vertex.coord[1]=y;
	gl_context.current_vertex.coord[2]=z;gl_context.current_vertex.coord[3]=h;
	gl_context.modleview_matrix_stack.peek()->applyToVector(gl_context.current_vertex.coord);

	if(gl_context.light_enabled)
		calcLightColor(&gl_context.current_vertex);

	gl_context.vertexs[gl_context.vertex_cnt]=gl_context.current_vertex;
	gl_context.vertexs[gl_context.vertex_cnt].color=gl_context.vertexs[gl_context.vertex_cnt].front_color;
	++gl_context.vertex_cnt;
}

//glnormal
void glNormal3i(int x,int y,int z)
{
	glNormal3d(x,y,z);
}

void glNormal3iv(int* v)
{
	glNormal3d(v[0],v[1],v[2]);
}

void glNormal3f(float x,float y,float z)
{
	glNormal3d(x,y,z);
}

void glNormal3fv(float* v)
{
	glNormal3d(v[0],v[1],v[2]);
}

void glNormal3d(double x,double y,double z)
{	
	double m=sqrt(x*x+y*y+z*z);
	gl_context.current_vertex.normal[0]=x/m;
	gl_context.current_vertex.normal[1]=y/m;
	gl_context.current_vertex.normal[2]=z/m;

	//transform
	Matrix44* mat=gl_context.modleview_matrix_stack.peek();
	if(gl_context.modleview_matrix_inv_dirty)
	{
		mat->mvInverse(gl_context.modleview_matrix_inv);
		gl_context.modleview_matrix_inv_dirty=false;
	}
	gl_context.modleview_matrix_inv.applyToVectorWithTranspose(gl_context.current_vertex.normal);		
}

void glNormal3dv(double* v)
{
	glNormal3d(v[0],v[1],v[2]);
}

//gltexcoord
void glTexCoord1fv(GLfloat *v)
{
	glTexCoord4d(v[0],0.0,0.0,1.0);
}
void glTexCoord2fv(GLfloat *v)
{
	glTexCoord4d(v[0],v[1],0.0,1.0);
}
void glTexCoord3fv(GLfloat *v)
{
	glTexCoord4d(v[0],v[1],v[2],1.0);
}
void glTexCoord4fv(GLfloat *v)
{
	glTexCoord4d(v[0],v[1],v[2],v[3]);
}
void glTexCoord1f(GLfloat x)
{
	glTexCoord4d(x,0.0,0.0,1.0);
}
void glTexCoord2f(GLfloat x,GLfloat y)
{
	glTexCoord4d(x,y,0.0,1.0);
}
void glTexCoord3f(GLfloat x,GLfloat y,GLfloat z)
{
	glTexCoord4d(x,y,z,1.0);
}
void glTexCoord4f(GLfloat x,GLfloat y,GLfloat z,GLfloat h)
{
	glTexCoord4d(x,y,z,h);
}
void glTexCoord1iv(GLint *v)
{
	glTexCoord4d(v[0],0.0,0.0,1.0);
}
void glTexCoord2iv(GLint *v)
{
	glTexCoord4d(v[0],v[1],0.0,1.0);
}
void glTexCoord3iv(GLint *v)
{
	glTexCoord4d(v[0],v[1],v[2],1.0);
}
void glTexCoord4iv(GLint *v)
{
	glTexCoord4d(v[0],v[1],v[2],v[3]);
}
void glTexCoord1i(GLint x)
{
	glTexCoord4d(x,0.0,0.0,1.0);
}
void glTexCoord2i(GLint x,GLint y)
{
	glTexCoord4d(x,y,0.0,1.0);
}
void glTexCoord3i(GLint x,GLint y,GLint z)
{
	glTexCoord4d(x,y,z,1.0);
}
void glTexCoord4i(GLint x,GLint y,GLint z,GLint h)
{
	glTexCoord4d(x,y,z,h);
}
void glTexCoord1dv(GLdouble *v)
{
	glTexCoord4d(v[0],0.0,0.0,1.0);
}
void glTexCoord2dv(GLdouble *v)
{
	glTexCoord4d(v[0],v[1],0.0,1.0);
}
void glTexCoord3dv(GLdouble *v)
{
	glTexCoord4d(v[0],v[1],v[2],1.0);
}
void glTexCoord4dv(GLdouble *v)
{
	glTexCoord4d(v[0],v[1],v[2],v[3]);
}
void glTexCoord1d(GLdouble x)
{
	glTexCoord4d(x,0.0,0.0,1.0);
}
void glTexCoord2d(GLdouble x,GLdouble y)
{
	glTexCoord4d(x,y,0.0,1.0);
}
void glTexCoord3d(GLdouble x,GLdouble y,GLdouble z)
{
	glTexCoord4d(x,y,z,1.0);
}
void glTexCoord4d(GLdouble x,GLdouble y,GLdouble z,GLdouble h)
{
	gl_context.current_vertex.tex_coord[0]=x;
	gl_context.current_vertex.tex_coord[1]=y;
	gl_context.current_vertex.tex_coord[2]=z;
	gl_context.current_vertex.tex_coord[3]=h;
}

//glcolor
void glColor3i(GLint r,GLint g,GLint b)
{
	glColor4i(r,g,b,255);
}
void glColor3iv(GLint *v)
{
	glColor4i(v[0],v[1],v[2],255);
}
void glColor4iv(GLint *v)
{
	glColor4i(v[0],v[1],v[2],v[3]);
}
void glColor4i(GLint r,GLint g,GLint b, GLint a)
{
	if(r>255) r=255;if(g>255) g=255;if(b>255) b=255;if(a>255) a=255;
	if(r<0) r=0;if(g<0) g=0;if(b<0) b=0;if(a<0) a=0;
	glColor4d(r/255.0,g/255.0,b/255.0,a/255.0);
}
void glColor3f(GLfloat r,GLfloat g,GLfloat b)
{
	glColor4d(r,g,b,1.0);
}
void glColor3fv(GLfloat *v)
{
	glColor4d(v[0],v[1],v[2],1.0);
}
void glColor4f(GLfloat r,GLfloat g,GLfloat b, GLfloat a)
{
	glColor4d(r,g,b,a);
}
void glColor4fv(GLfloat *v)
{
	glColor4d(v[0],v[1],v[2],v[3]);
}
void glColor3d(GLdouble r,GLdouble g,GLdouble b)
{
	glColor4d(r,g,b,1.0);
}
void glColor3dv(GLdouble *v)
{
	glColor4d(v[0],v[1],v[2],1.0);
}
void glColor4dv(GLdouble *v)
{
	glColor4d(v[0],v[1],v[2],v[3]);
}
void glColor4d(GLdouble r,GLdouble g,GLdouble b,GLdouble a)
{
	gl_context.current_vertex.color[0]=r;
	gl_context.current_vertex.color[1]=g;
	gl_context.current_vertex.color[2]=b;
	gl_context.current_vertex.color[3]=a;
	if(gl_context.color_material_front_enabled)
	{
		switch(gl_context.color_material_front_mode)
		{
		case GL_AMBIENT:
			gl_context.current_vertex.material_front.material_ambient[0]=r;
			gl_context.current_vertex.material_front.material_ambient[1]=g;
			gl_context.current_vertex.material_front.material_ambient[2]=b;
			gl_context.current_vertex.material_front.material_ambient[3]=a;
			break;
		case GL_AMBIENT_AND_DIFFUSE:
			gl_context.current_vertex.material_front.material_ambient[0]=r;
			gl_context.current_vertex.material_front.material_ambient[1]=g;
			gl_context.current_vertex.material_front.material_ambient[2]=b;
			gl_context.current_vertex.material_front.material_ambient[3]=a;
		case GL_DIFFUSE:
			gl_context.current_vertex.material_front.material_diffuse[0]=r;
			gl_context.current_vertex.material_front.material_diffuse[1]=g;
			gl_context.current_vertex.material_front.material_diffuse[2]=b;
			gl_context.current_vertex.material_front.material_diffuse[3]=a;
			break;
		case GL_SPECULAR:
			gl_context.current_vertex.material_front.material_specular[0]=r;
			gl_context.current_vertex.material_front.material_specular[1]=g;
			gl_context.current_vertex.material_front.material_specular[2]=b;
			gl_context.current_vertex.material_front.material_specular[3]=a;
			break;
		case GL_EMISSION:
			gl_context.current_vertex.material_front.material_emission[0]=r;
			gl_context.current_vertex.material_front.material_emission[1]=g;
			gl_context.current_vertex.material_front.material_emission[2]=b;
			gl_context.current_vertex.material_front.material_emission[3]=a;
			break;
		}
	}
	if(gl_context.color_material_back_enabled)
	{
		switch(gl_context.color_material_back_mode)
		{
		case GL_AMBIENT:
			gl_context.current_vertex.material_back.material_ambient[0]=r;
			gl_context.current_vertex.material_back.material_ambient[1]=g;
			gl_context.current_vertex.material_back.material_ambient[2]=b;
			gl_context.current_vertex.material_back.material_ambient[3]=a;
			break;
		case GL_AMBIENT_AND_DIFFUSE:
			gl_context.current_vertex.material_back.material_ambient[0]=r;
			gl_context.current_vertex.material_back.material_ambient[1]=g;
			gl_context.current_vertex.material_back.material_ambient[2]=b;
			gl_context.current_vertex.material_back.material_ambient[3]=a;
		case GL_DIFFUSE:
			gl_context.current_vertex.material_back.material_diffuse[0]=r;
			gl_context.current_vertex.material_back.material_diffuse[1]=g;
			gl_context.current_vertex.material_back.material_diffuse[2]=b;
			gl_context.current_vertex.material_back.material_diffuse[3]=a;
			break;
		case GL_SPECULAR:
			gl_context.current_vertex.material_back.material_specular[0]=r;
			gl_context.current_vertex.material_back.material_specular[1]=g;
			gl_context.current_vertex.material_back.material_specular[2]=b;
			gl_context.current_vertex.material_back.material_specular[3]=a;
			break;
		case GL_EMISSION:
			gl_context.current_vertex.material_back.material_emission[0]=r;
			gl_context.current_vertex.material_back.material_emission[1]=g;
			gl_context.current_vertex.material_back.material_emission[2]=b;
			gl_context.current_vertex.material_back.material_emission[3]=a;
			break;
		}
	}	
}

void glClearColor(GLfloat r,GLfloat g,GLfloat b,GLfloat a)
{
	gl_context.clear_color[0]=r;
	gl_context.clear_color[1]=g;
	gl_context.clear_color[2]=b;
	gl_context.clear_color[3]=a;
}
void glClearDepth(GLdouble depth)
{
	gl_context.clear_depth=depth;
}
//project&&cam&&viewport
void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	double top=zNear*tan(GL_PI*fovy/360.0);
	double right=top*aspect;
	glFrustum(-right,right,-top,top,zNear,zFar);
}
void glFrustum(GLdouble left,GLdouble right,GLdouble bottom,GLdouble top,
			   GLdouble znear,GLdouble zfar)
{
	GLdouble m[16];
	m[0]=(znear+znear)/(right-left);
	m[1]=m[2]=m[3]=m[4]=m[6]=m[7]=m[12]=m[13]=m[15]=0.0;
	m[5]=(znear+znear)/(top-bottom);
	m[8]=(left+right)/(right-left);
	m[9]=(top+bottom)/(top-bottom);
	m[10]=(znear+zfar)/(znear-zfar);
	m[11]=-1.0;
	m[14]=(znear+znear)*zfar/(znear-zfar);

	gl_context.current_matrix->rightMul(m);
	gl_context.z_near=znear;gl_context.z_far=zfar;
}
void glOrtho (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, 
			  GLdouble znear, GLdouble zfar)
{
	GLdouble m[16];
	m[0]=2/(right-left);
	m[1]=m[2]=m[3]=m[4]=m[6]=m[7]=m[8]=m[9]=m[11]=0.0;
	m[5]=2/(top-bottom);
	m[10]=2/(znear-zfar);
	m[12]=(left+right)/(left-right);
	m[13]=(top+bottom)/(bottom-top);
	m[14]=(znear+zfar)/(znear-zfar);
	m[15]=1.0;

	gl_context.current_matrix->rightMul(m);
}
void gluLookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx, GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy, GLdouble upz)
{
	GLdouble m[16];
	Vector3 n,u(upx,upy,upz),v,eye(eyex,eyey,eyez);
	n.x=eyex-centerx;n.y=eyey-centery;n.z=eyez-centerz;
	u=u*n;
	v=n*u;
	n.normalize();u.normalize();v.normalize();

	GLdouble dx,dy,dz;
	dx=-eye.dot(u);dy=-eye.dot(v);dz=-eye.dot(n);

	m[0]=u.x;m[4]=u.y;m[8]=u.z;m[12]=dx;
	m[1]=v.x;m[5]=v.y;m[9]=v.z;m[13]=dy;
	m[2]=n.x;m[6]=n.y;m[10]=n.z;m[14]=dz;
	m[3]=0.0;m[7]=0.0;m[11]=0.0;m[15]=1.0;

	//camera,last mult
	gl_context.current_matrix->leftMul(m);
}
void glViewport(int x,int y,int width,int height)
{
	gl_context.view_x=x;gl_context.view_y=y;
	gl_context.view_width=width;gl_context.view_height=height;
	//if(gl_context.view_width!=width||gl_context.view_height!=height)
	{		
		glbufResize(x,y,x+width,y+height);
	}
}

//texture
void glGenTextures(int n, unsigned int *textures)
{
	gl_context.tex_names.getLowest0(n,textures);
}
void glDeleteTextures(int n, const unsigned int *textures)
{
	while(n--)
	{
		if(gl_context.tex_names.get(n))
		{
			delete gl_context.tex_objects[n];
			gl_context.tex_objects[n]=NULL;
			gl_context.tex_names.clear(n);
		}
	}	
}
void glBindTexture(int target,int texture)
{
	if(!gl_context.tex_names.get(texture))
	{
		gl_context.tex_objects[texture]=gl_context.current_tex_object=new GLTextureObject;
		gl_context.tex_names.set(texture);
	}
	else gl_context.current_tex_object=gl_context.tex_objects[texture];
	//what to do with target
}
void glTexImage2D( int target, int level, int components,
				  int width, int height, int border,
				  int format, int type, void *pixels)
{
	if(!gl_context.current_tex_object) return;

	//level=0
	//target must be GL_TEXTURE_2D
	if(target==GL_TEXTURE_2D)
	{
		if(level<MAX_TEXTURE_LEVEL)
		{
			GLTextureObject* obj=gl_context.current_tex_object;
			GLTextureImage* image=&obj->tex_image_2d[level];
			image->border=border;
			image->format=format;
			image->width=width;
			image->height=height;
			image->type=type;
			image->texels=pixels;	
			switch(format)
			{
			//only support the following two formats
			case GL_RGBA:
				obj->components_cnt=4;
				break;
			case GL_RGB:
				obj->components_cnt=3;
				break;
			}
			switch(type)
			{
			case GL_UNSIGNED_BYTE:
			case GL_BYTE:				
				obj->components_size=sizeof(byte);				
				break;
			case GL_UNSIGNED_SHORT:
			case GL_SHORT:
				obj->components_size=sizeof(byte);				
				break;
			case GL_UNSIGNED_INT:
			case GL_INT:
				obj->components_size=sizeof(int);
				break;
			case GL_FLOAT:
				obj->components_size=sizeof(float);
				break;
			}
			obj->units=obj->components_size*obj->components_cnt;
			obj->width_bytes=width*obj->units;
		}
		
	}
}
void glTexEnvfv(int target,int pname,float* param)
{
	//GL_TEXTURE_FILTER_CONTROL
	if(target==GL_TEXTURE_ENV)
	{
		switch(pname)
		{
		case GL_TEXTURE_ENV_COLOR:
			gl_context.tex_env_color[0]=param[0];
			gl_context.tex_env_color[1]=param[1];
			gl_context.tex_env_color[2]=param[2];
			gl_context.tex_env_color[3]=param[3];
			break;
		}
	}
}
void glTexEnviv(int target,int pname,int* param)
{
	//GL_TEXTURE_FILTER_CONTROL
	if(target==GL_TEXTURE_ENV)
	{
		switch(pname)
		{
		case GL_TEXTURE_ENV_MODE:
			gl_context.tex_env_mode=*param;
			break;
		case GL_TEXTURE_ENV_COLOR:
			gl_context.tex_env_color[0]=((float)param[0])/255.0;
			gl_context.tex_env_color[1]=((float)param[0])/255.0;
			gl_context.tex_env_color[2]=((float)param[0])/255.0;
			gl_context.tex_env_color[3]=((float)param[0])/255.0;
			break;
		}
	}
}
void glTexEnvi(int target,int pname,int param)
{
	//GL_TEXTURE_FILTER_CONTROL
	if(target==GL_TEXTURE_ENV)
	{
		switch(pname)
		{
		case GL_TEXTURE_ENV_MODE:
			gl_context.tex_env_mode=param;
			break;
		}
	}
}
void glTexParameteri(int target,int pname,int param)
{
	//only support 2d texture
	switch(target)
	{
// 	case GL_TEXTURE_1D:
// 		switch(pname)
// 		{
// 		case GL_TEXTURE_WRAP_S:
// 			gl_context.current_tex_object->wrap_s_1d=param;
// 			break;
// 		case GL_TEXTURE_MAG_FILTER:
// 			gl_context.current_tex_object->mag_filter_1d=param;
// 			break;
// 		case GL_TEXTURE_MIN_FILTER:
// 			gl_context.current_tex_object->min_filter_1d=param;
// 			break;
// 		}
// 		break;
	case GL_TEXTURE_2D:
		switch(pname)
		{
		case GL_TEXTURE_WRAP_S:
			gl_context.current_tex_object->wrap_s_2d=param;
			break;
		case GL_TEXTURE_WRAP_T:
			gl_context.current_tex_object->wrap_t_2d=param;
			break;
		case GL_TEXTURE_MAG_FILTER:			
		case GL_TEXTURE_MIN_FILTER://in this version we do not distinguish between
								   //minification and magnification filter
			gl_context.current_tex_object->filter_2d=param;
			break;
		}
		break;
	}
}
//lighting
void glMaterialfv(int mode,int type,float *v)
{
	switch(mode)
	{
	case GL_FRONT:
		switch(type)
		{
		case GL_AMBIENT:
			memcpy(gl_context.current_vertex.material_front.material_ambient,
				v,sizeof(float)*4);
			break;
		case GL_AMBIENT_AND_DIFFUSE:
			memcpy(gl_context.current_vertex.material_front.material_ambient,
				v,sizeof(float)*4);
		case GL_DIFFUSE:
			memcpy(gl_context.current_vertex.material_front.material_diffuse,
				v,sizeof(float)*4);
			break;
		case GL_SPECULAR:
			memcpy(gl_context.current_vertex.material_front.material_specular,
				v,sizeof(float)*4);
			break;
		case GL_EMISSION:
			memcpy(gl_context.current_vertex.material_front.material_emission,
				v,sizeof(float)*4);
			break;
		case GL_SHININESS:
			gl_context.current_vertex.material_front.material_shininess=*v;
			break;
		}
		break;
	case GL_FRONT_AND_BACK:
		switch(type)
		{
		case GL_AMBIENT:
			memcpy(gl_context.current_vertex.material_front.material_ambient,
				v,sizeof(float)*4);
			break;
		case GL_AMBIENT_AND_DIFFUSE:
			memcpy(gl_context.current_vertex.material_front.material_ambient,
				v,sizeof(float)*4);
		case GL_DIFFUSE:
			memcpy(gl_context.current_vertex.material_front.material_diffuse,
				v,sizeof(float)*4);
			break;
		case GL_SPECULAR:
			memcpy(gl_context.current_vertex.material_front.material_specular,
				v,sizeof(float)*4);
			break;
		case GL_EMISSION:
			memcpy(gl_context.current_vertex.material_front.material_emission,
				v,sizeof(float)*4);
			break;
		case GL_SHININESS:
			gl_context.current_vertex.material_front.material_shininess=*v;
			break;
		}
	case GL_BACK:
		switch(type)
		{
		case GL_AMBIENT:
			memcpy(gl_context.current_vertex.material_back.material_ambient,
				v,sizeof(float)*4);
			break;
		case GL_AMBIENT_AND_DIFFUSE:
			memcpy(gl_context.current_vertex.material_back.material_ambient,
				v,sizeof(float)*4);
		case GL_DIFFUSE:
			memcpy(gl_context.current_vertex.material_back.material_diffuse,
				v,sizeof(float)*4);
			break;
		case GL_SPECULAR:
			memcpy(gl_context.current_vertex.material_back.material_specular,
				v,sizeof(float)*4);
			break;
		case GL_EMISSION:
			memcpy(gl_context.current_vertex.material_back.material_emission,
				v,sizeof(float)*4);
			break;
		case GL_SHININESS:
			gl_context.current_vertex.material_back.material_shininess=*v;
			break;
		}
		break;
	}
}
void glMaterialf(int mode,int type,float v)
{
	if(type==GL_SHININESS)
	{
		switch(mode)
		{
		case GL_FRONT:
			gl_context.current_vertex.material_front.material_shininess=v;
			break;
		case GL_FRONT_AND_BACK:
			gl_context.current_vertex.material_front.material_shininess=v;
		case GL_BACK:
			gl_context.current_vertex.material_back.material_shininess=v;
			break;
		}		
	}
}
void glMaterialiv(int mode,int type,int *v)
{
	switch(mode)
	{
	case GL_FRONT:
		switch(type)
		{
		case GL_AMBIENT:
			gl_context.current_vertex.material_front.material_ambient[0]=((float)v[0])/255.0;
			gl_context.current_vertex.material_front.material_ambient[1]=((float)v[1])/255.0;
			gl_context.current_vertex.material_front.material_ambient[2]=((float)v[2])/255.0;
			gl_context.current_vertex.material_front.material_ambient[3]=((float)v[3])/255.0;
			break;
		case GL_AMBIENT_AND_DIFFUSE:
			gl_context.current_vertex.material_front.material_ambient[0]=((float)v[0])/255.0;
			gl_context.current_vertex.material_front.material_ambient[1]=((float)v[1])/255.0;
			gl_context.current_vertex.material_front.material_ambient[2]=((float)v[2])/255.0;
			gl_context.current_vertex.material_front.material_ambient[3]=((float)v[3])/255.0;
		case GL_DIFFUSE:
			gl_context.current_vertex.material_front.material_diffuse[0]=((float)v[0])/255.0;
			gl_context.current_vertex.material_front.material_diffuse[1]=((float)v[1])/255.0;
			gl_context.current_vertex.material_front.material_diffuse[2]=((float)v[2])/255.0;
			gl_context.current_vertex.material_front.material_diffuse[3]=((float)v[3])/255.0;
			break;
		case GL_SPECULAR:
			gl_context.current_vertex.material_front.material_specular[0]=((float)v[0])/255.0;
			gl_context.current_vertex.material_front.material_specular[1]=((float)v[1])/255.0;
			gl_context.current_vertex.material_front.material_specular[2]=((float)v[2])/255.0;
			gl_context.current_vertex.material_front.material_specular[3]=((float)v[3])/255.0;
			break;
		case GL_EMISSION:
			gl_context.current_vertex.material_front.material_emission[0]=((float)v[0])/255.0;
			gl_context.current_vertex.material_front.material_emission[1]=((float)v[1])/255.0;
			gl_context.current_vertex.material_front.material_emission[2]=((float)v[2])/255.0;
			gl_context.current_vertex.material_front.material_emission[3]=((float)v[3])/255.0;
			break;
		case GL_SHININESS:
			gl_context.current_vertex.material_front.material_shininess=((float)*v)/255.0;
			break;
		}
		break;
	case GL_FRONT_AND_BACK:
		switch(type)
		{
		case GL_AMBIENT:
			gl_context.current_vertex.material_front.material_ambient[0]=((float)v[0])/255.0;
			gl_context.current_vertex.material_front.material_ambient[1]=((float)v[1])/255.0;
			gl_context.current_vertex.material_front.material_ambient[2]=((float)v[2])/255.0;
			gl_context.current_vertex.material_front.material_ambient[3]=((float)v[3])/255.0;
			break;
		case GL_AMBIENT_AND_DIFFUSE:
			gl_context.current_vertex.material_front.material_ambient[0]=((float)v[0])/255.0;
			gl_context.current_vertex.material_front.material_ambient[1]=((float)v[1])/255.0;
			gl_context.current_vertex.material_front.material_ambient[2]=((float)v[2])/255.0;
			gl_context.current_vertex.material_front.material_ambient[3]=((float)v[3])/255.0;
		case GL_DIFFUSE:
			gl_context.current_vertex.material_front.material_diffuse[0]=((float)v[0])/255.0;
			gl_context.current_vertex.material_front.material_diffuse[1]=((float)v[1])/255.0;
			gl_context.current_vertex.material_front.material_diffuse[2]=((float)v[2])/255.0;
			gl_context.current_vertex.material_front.material_diffuse[3]=((float)v[3])/255.0;
			break;
		case GL_SPECULAR:
			gl_context.current_vertex.material_front.material_specular[0]=((float)v[0])/255.0;
			gl_context.current_vertex.material_front.material_specular[1]=((float)v[1])/255.0;
			gl_context.current_vertex.material_front.material_specular[2]=((float)v[2])/255.0;
			gl_context.current_vertex.material_front.material_specular[3]=((float)v[3])/255.0;
			break;
		case GL_EMISSION:
			gl_context.current_vertex.material_front.material_emission[0]=((float)v[0])/255.0;
			gl_context.current_vertex.material_front.material_emission[1]=((float)v[1])/255.0;
			gl_context.current_vertex.material_front.material_emission[2]=((float)v[2])/255.0;
			gl_context.current_vertex.material_front.material_emission[3]=((float)v[3])/255.0;
			break;
		case GL_SHININESS:
			gl_context.current_vertex.material_front.material_shininess=((float)*v)/255.0;
			break;
		}
	case GL_BACK:
		switch(type)
		{
		case GL_AMBIENT:
			gl_context.current_vertex.material_back.material_ambient[0]=((float)v[0])/255.0;
			gl_context.current_vertex.material_back.material_ambient[1]=((float)v[1])/255.0;
			gl_context.current_vertex.material_back.material_ambient[2]=((float)v[2])/255.0;
			gl_context.current_vertex.material_back.material_ambient[3]=((float)v[3])/255.0;
			break;
		case GL_AMBIENT_AND_DIFFUSE:
			gl_context.current_vertex.material_back.material_ambient[0]=((float)v[0])/255.0;
			gl_context.current_vertex.material_back.material_ambient[1]=((float)v[1])/255.0;
			gl_context.current_vertex.material_back.material_ambient[2]=((float)v[2])/255.0;
			gl_context.current_vertex.material_back.material_ambient[3]=((float)v[3])/255.0;
		case GL_DIFFUSE:
			gl_context.current_vertex.material_back.material_diffuse[0]=((float)v[0])/255.0;
			gl_context.current_vertex.material_back.material_diffuse[1]=((float)v[1])/255.0;
			gl_context.current_vertex.material_back.material_diffuse[2]=((float)v[2])/255.0;
			gl_context.current_vertex.material_back.material_diffuse[3]=((float)v[3])/255.0;
			break;
		case GL_SPECULAR:
			gl_context.current_vertex.material_back.material_specular[0]=((float)v[0])/255.0;
			gl_context.current_vertex.material_back.material_specular[1]=((float)v[1])/255.0;
			gl_context.current_vertex.material_back.material_specular[2]=((float)v[2])/255.0;
			gl_context.current_vertex.material_back.material_specular[3]=((float)v[3])/255.0;
			break;
		case GL_EMISSION:
			gl_context.current_vertex.material_back.material_emission[0]=((float)v[0])/255.0;
			gl_context.current_vertex.material_back.material_emission[1]=((float)v[1])/255.0;
			gl_context.current_vertex.material_back.material_emission[2]=((float)v[2])/255.0;
			gl_context.current_vertex.material_back.material_emission[3]=((float)v[3])/255.0;
			break;
		case GL_SHININESS:
			gl_context.current_vertex.material_back.material_shininess=((float)*v)/255.0;
			break;
		}
		break;
	}
}
void glMateriali(int mode,int type,int v)
{
	if(type==GL_SHININESS)
	{
		switch(mode)
		{
		case GL_FRONT:
			gl_context.current_vertex.material_front.material_shininess=v;
			break;
		case GL_FRONT_AND_BACK:
			gl_context.current_vertex.material_front.material_shininess=v;
		case GL_BACK:
			gl_context.current_vertex.material_back.material_shininess=v;
			break;
		}		
	}
}
void glColorMaterial(int mode,int type)
{
	switch(mode)
	{
	case GL_FRONT:
		gl_context.color_material_front_mode=type;
		break;
	case GL_FRONT_AND_BACK:
		gl_context.color_material_front_mode=type;
	case GL_BACK:
		gl_context.color_material_back_mode=type;
		break;
	}
}

void glLightfv(int light,int type,float *v)
{
	if(light<GL_LIGHT0||light>=GL_LIGHT0+GL_MAX_LIGHTS) return;
	light-=GL_LIGHT0;
	switch(type)
	{
	case GL_AMBIENT:
		memcpy(gl_context.lights[light].ambient_intensity,
			v,sizeof(float)*4);
		break;
	case GL_DIFFUSE:
		memcpy(gl_context.lights[light].diffuse_intensity,
			v,sizeof(float)*4);
		break;
	case GL_SPECULAR:
		memcpy(gl_context.lights[light].specular_intensity,
			v,sizeof(float)*4);
		break;
	case GL_POSITION:
		////////////////////////////////////////transform
		memcpy(gl_context.lights[light].position,
			v,sizeof(float)*4);
		gl_context.modleview_matrix_stack.peek()->applyToVector(gl_context.lights[light].position);
		break;
	case GL_SPOT_DIRECTION:
		memcpy(gl_context.lights[light].spot_direction,
			v,sizeof(float)*4);
		break;
	case GL_SPOT_EXPONENT:
		gl_context.lights[light].spot_exponent=*v;
		break;
	case GL_SPOT_CUTOFF:
		gl_context.lights[light].spot_cutoff=*v;
		break;
	case GL_CONSTANT_ATTENUATION:
		gl_context.lights[light].constant_attenuation=*v;
		break;
	case GL_LINEAR_ATTENUATION:
		gl_context.lights[light].linear_attenuation=*v;
		break;
	case GL_QUADRATIC_ATTENUATION:
		gl_context.lights[light].quadratic_attenuation=*v;
		break;
	}
}
void glLightf(int light,int type,float v)
{
	if(light<GL_LIGHT0||light>=GL_LIGHT0+GL_MAX_LIGHTS) return;
	light-=GL_LIGHT0;
	switch(type)
	{
	case GL_SPOT_EXPONENT:
		gl_context.lights[light].spot_exponent=v;
		break;
	case GL_SPOT_CUTOFF:
		gl_context.lights[light].spot_cutoff=v;
		break;
	case GL_CONSTANT_ATTENUATION:
		gl_context.lights[light].constant_attenuation=v;
		break;
	case GL_LINEAR_ATTENUATION:
		gl_context.lights[light].linear_attenuation=v;
		break;
	case GL_QUADRATIC_ATTENUATION:
		gl_context.lights[light].quadratic_attenuation=v;
		break;
	}
}
void glLightiv(int light,int type,int *v)
{
	if(light<GL_LIGHT0||light>=GL_LIGHT0+GL_MAX_LIGHTS) return;
	light-=GL_LIGHT0;
	switch(type)
	{
	case GL_AMBIENT:
		gl_context.lights[light].ambient_intensity[0]=((float)v[0])/255.0;
		gl_context.lights[light].ambient_intensity[1]=((float)v[1])/255.0;
		gl_context.lights[light].ambient_intensity[2]=((float)v[2])/255.0;
		gl_context.lights[light].ambient_intensity[3]=((float)v[3])/255.0;
		break;
	case GL_DIFFUSE:
		gl_context.lights[light].diffuse_intensity[0]=((float)v[0])/255.0;
		gl_context.lights[light].diffuse_intensity[1]=((float)v[1])/255.0;
		gl_context.lights[light].diffuse_intensity[2]=((float)v[2])/255.0;
		gl_context.lights[light].diffuse_intensity[3]=((float)v[3])/255.0;
		break;
	case GL_SPECULAR:
		gl_context.lights[light].specular_intensity[0]=((float)v[0])/255.0;
		gl_context.lights[light].specular_intensity[1]=((float)v[1])/255.0;
		gl_context.lights[light].specular_intensity[2]=((float)v[2])/255.0;
		gl_context.lights[light].specular_intensity[3]=((float)v[3])/255.0;
		break;
	case GL_POSITION:
		////////////////////////////////////////transform
		gl_context.lights[light].position[0]=v[0];
		gl_context.lights[light].position[1]=v[1];
		gl_context.lights[light].position[2]=v[2];
		gl_context.lights[light].position[3]=v[3];
		gl_context.modleview_matrix_stack.peek()->applyToVector(gl_context.lights[light].position);
		break;
	case GL_SPOT_DIRECTION:
		gl_context.lights[light].ambient_intensity[0]=v[0];
		gl_context.lights[light].ambient_intensity[1]=v[1];
		gl_context.lights[light].ambient_intensity[2]=v[2];
		gl_context.lights[light].ambient_intensity[3]=v[3];
		break;
	case GL_SPOT_EXPONENT:
		gl_context.lights[light].spot_exponent=*v;
		break;
	case GL_SPOT_CUTOFF:
		gl_context.lights[light].spot_cutoff=*v;
		break;
	case GL_CONSTANT_ATTENUATION:
		gl_context.lights[light].constant_attenuation=*v;
		break;
	case GL_LINEAR_ATTENUATION:
		gl_context.lights[light].linear_attenuation=*v;
		break;
	case GL_QUADRATIC_ATTENUATION:
		gl_context.lights[light].quadratic_attenuation=*v;
		break;
	}
}
void glLighti(int light,int type,float v)
{
	if(light<GL_LIGHT0||light>=GL_LIGHT0+GL_MAX_LIGHTS) return;
	light-=GL_LIGHT0;
	switch(type)
	{
	case GL_SPOT_EXPONENT:
		gl_context.lights[light].spot_exponent=v;
		break;
	case GL_SPOT_CUTOFF:
		gl_context.lights[light].spot_cutoff=v;
		break;
	case GL_CONSTANT_ATTENUATION:
		gl_context.lights[light].constant_attenuation=v;
		break;
	case GL_LINEAR_ATTENUATION:
		gl_context.lights[light].linear_attenuation=v;
		break;
	case GL_QUADRATIC_ATTENUATION:
		gl_context.lights[light].quadratic_attenuation=v;
		break;
	}
}
void glLightModeli(int pname,int param)
{
	switch(pname)
	{
	case GL_LIGHT_MODEL_LOCAL_VIEWER:
		if(param) gl_context.local_viewer=true;
		else gl_context.local_viewer=false;
		break;
	case GL_LIGHT_MODEL_TWO_SIDE:
		if(param) gl_context.two_side=true;
		else gl_context.two_side=false;
		break;
	case GL_LIGHT_MODEL_COLOR_CONTROL:
		gl_context.color_control=param;
		break;
	}
}
void glLightModelf(int pname,float param)
{
	if(pname==GL_LIGHT_MODEL_LOCAL_VIEWER)
	{
		if(param!=0.0) gl_context.local_viewer=true;
		else gl_context.local_viewer=false;
	}
	else if(pname==GL_LIGHT_MODEL_TWO_SIDE)
	{
		if(param!=0.0) gl_context.two_side=true;
		else gl_context.two_side=false;
	}
}
void glLightModeliv(int pname,int *param)
{
	switch(pname)
	{
	case GL_LIGHT_MODEL_LOCAL_VIEWER:
		if(*param) gl_context.local_viewer=true;
		else gl_context.local_viewer=false;
		break;
	case GL_LIGHT_MODEL_TWO_SIDE:
		if(*param) gl_context.two_side=true;
		else gl_context.two_side=false;
		break;
	case GL_LIGHT_MODEL_COLOR_CONTROL:
		gl_context.color_control=*param;
		break;
	case GL_LIGHT_MODEL_AMBIENT:		
		gl_context.global_ambient_intensity[0]=((float)param[0])/255.0;
		gl_context.global_ambient_intensity[0]=((float)param[0])/255.0;
		gl_context.global_ambient_intensity[0]=((float)param[0])/255.0;
		gl_context.global_ambient_intensity[0]=((float)param[0])/255.0;
		break;
	}
}
void glLightModelfv(int pname,float *param)
{
	if(pname==GL_LIGHT_MODEL_LOCAL_VIEWER)
	{
		if(*param!=0.0) gl_context.local_viewer=true;
		else gl_context.local_viewer=false;
	}
	else if(pname==GL_LIGHT_MODEL_TWO_SIDE)
	{
		if(*param!=0.0) gl_context.two_side=true;
		else gl_context.two_side=false;
	}
	else if(pname==GL_LIGHT_MODEL_AMBIENT)
	{
		memcpy(gl_context.global_ambient_intensity,param,sizeof(float)*4);
	}
}
//blending
void glBlendFunc(GLenum sfactor, GLenum dfactor)
{
	gl_context.blend_sfactor=sfactor;
	gl_context.blend_dfactor=dfactor;
}
void glDepthMask(int mask)
{
	gl_context.depth_mask=mask;
}
void glDepthFunc(int func)
{
	if(func>=GL_NEVER&&func<=GL_ALWAYS)
	{
		gl_context.depth_func=func;
		depthFunc=depthFuncs[func-GL_NEVER];
	}
}
//fog
void glFogi(int pname, int param)
{
	switch(pname)
	{
	case GL_FOG_MODE:
		gl_context.fog_mode=param;
		break;
	case GL_FOG_DENSITY:
		gl_context.fog_density=param;
		break;
	case GL_FOG_START:
		gl_context.fog_start=param;
		break;
	case GL_FOG_END:
		gl_context.fog_end=param;
		break;
	}
}
void glFogf(int pname, float param)
{
	switch(pname)
	{
	case GL_FOG_DENSITY:
		gl_context.fog_density=param;
		break;
	case GL_FOG_START:
		gl_context.fog_start=param;
		break;
	case GL_FOG_END:
		gl_context.fog_end=param;
		break;
	}
}
void glFogfv(int pname, const float* param)
{	
	switch(pname)
	{
	case GL_FOG_DENSITY:
		gl_context.fog_density=*param;
		break;
	case GL_FOG_START:
		gl_context.fog_start=*param;
		break;
	case GL_FOG_END:
		gl_context.fog_end=*param;
		break;
	case GL_FOG_COLOR:
		memcpy(gl_context.fog_color,param,4*sizeof(float));
		break;
	}	
}

void glFogiv(int pname, const int* param)
{	
	switch(pname)
	{
	case GL_FOG_MODE:
		gl_context.fog_mode=*param;
		break;
	case GL_FOG_DENSITY:
		gl_context.fog_density=*param;
		break;
	case GL_FOG_START:
		gl_context.fog_start=*param;
		break;
	case GL_FOG_END:
		gl_context.fog_end=*param;
		break;
	case GL_FOG_COLOR:
		//Óëmsdn²»Í¬,0-255
		gl_context.fog_color[0]=((float)(param[0]))/255.0;
		gl_context.fog_color[1]=((float)(param[1]))/255.0;
		gl_context.fog_color[2]=((float)(param[2]))/255.0;
		gl_context.fog_color[3]=((float)(param[3]))/255.0;
		break;
	}	
}

//cull
void glCullFace(int mode)
{
	gl_context.face_cull_mode=mode;
	if(mode==GL_FRONT)
		gl_context.face_cull_determine=gl_context.front_face;
	else
		gl_context.face_cull_determine=GL_CCW-1+(GL_CCW-gl_context.front_face);
}
void glFrontFace(int mode)
{
	gl_context.front_face=mode;
	if(mode==GL_FRONT)
		gl_context.face_cull_determine=mode;
	else
		gl_context.face_cull_determine=GL_CW+1-mode;
}
//stipple
void glLineStipple(int factor,unsigned short pattern)
{
	gl_context.line_stipple_factor=factor&0xff;
	gl_context.line_stipple_pattern=pattern;
}
//other
void glPolygonMode(int face,int mode)
{
	switch(face)
	{
	case GL_FRONT:
		gl_context.front_polygon_mode=mode;
		break;
	case GL_FRONT_AND_BACK:
		gl_context.front_polygon_mode=mode;
	case GL_BACK:
		gl_context.back_polygon_mode=mode;
	}
}
void glRectd(GLdouble x1,GLdouble y1,GLdouble x2,GLdouble y2)
{
	glBegin(GL_POLYGON); 
	glVertex2d(x1, y1);
	glVertex2d(x2, y1); 
	glVertex2d(x2, y2);
	glVertex2d(x1, y2); 
	glEnd();
}
void glRectf(GLfloat x1,GLfloat y1,GLfloat x2,GLfloat y2)
{
	glRectd(x1,y1,x2,y2);
}
void glRecti(GLint x1,GLint y1,GLint x2,GLint y2)
{
	glRectd(x1,y1,x2,y2);
}
void glRects(GLshort x1,GLshort y1,GLshort x2,GLshort y2)
{
	glRectd(x1,y1,x2,y2);
}
void glFlush()
{	
	//do nothing
}

void glEnable(int code)
{
	switch (code)
	{
	case GL_LIGHT0:
	case GL_LIGHT1:
	case GL_LIGHT2:
	case GL_LIGHT3:
	case GL_LIGHT4:
	case GL_LIGHT5:
	case GL_LIGHT6:
	case GL_LIGHT7:
	case GL_LIGHT8:
		gl_context.lights[code-GL_LIGHT0].enabled=true;
		break;
	case GL_LIGHTING:
		gl_context.light_enabled=true;
		break;
	case GL_TEXTURE_2D:
		if(gl_context.texture_enabled<2) gl_context.texture_enabled=2;
		break;
	case GL_BLEND:
		gl_context.blend_enabled=true;
		break;
	case GL_FOG:
		gl_context.fog_enabled=true;
		break;
	case GL_CULL_FACE:
		gl_context.face_cull_enabled=true;
		break;
	case GL_LINE_STIPPLE:
		gl_context.line_stipple_enabled=true;
		break;
	}	
}
void glDisable(int code)
{
	switch (code)
	{
	case GL_LIGHT0:
	case GL_LIGHT1:
	case GL_LIGHT2:
	case GL_LIGHT3:
	case GL_LIGHT4:
	case GL_LIGHT5:
	case GL_LIGHT6:
	case GL_LIGHT7:
	case GL_LIGHT8:
		gl_context.lights[code-GL_LIGHT0].enabled=false;
		break;
	case GL_LIGHTING:
		gl_context.light_enabled=false;
		break;
	//case GL_TEXTURE_1D:
	case GL_TEXTURE_2D:
	//case GL_TEXTURE_3D:
		gl_context.texture_enabled=0;
		break;
	case GL_BLEND:
		gl_context.blend_enabled=false;
		break;
	case GL_FOG:
		gl_context.fog_enabled=false;
		break;
	case GL_CULL_FACE:
		gl_context.face_cull_enabled=false;
		break;
	case GL_LINE_STIPPLE:
		gl_context.line_stipple_enabled=false;
		break;
	}	
}

void glHint(int target,int mode)
{
	switch(target)
	{
	case GL_FOG_HINT:
		//not implemented
		gl_context.fog_hint=mode;
		break;
	}
}