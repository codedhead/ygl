#include "yglu.h"
#include "global.h"
#include "geom.h"

#include <math.h>

namespace ygl{

#define CHECK_VALUE

	void gluOrtho2D(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top)
	{
		ygl::glOrtho(left,right,bottom,top,-1.0,1.0);
	}

	void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
	{
		CHECK_VALUE
		
		GLdouble h=zNear*tan(0.5*D2R(fovy)),w=aspect*h;
		ygl::glFrustum(-w,w,-h,h,zNear,zFar);
	}
	void gluLookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx, GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy, GLdouble upz)
	{
		GLfloat m[16];
		// todo: sigularity case??
		Vector n,u(upx,upy,upz),v,eye(eyex,eyey,eyez);
		n.x=eyex-centerx;n.y=eyey-centery;n.z=eyez-centerz;
		n.normalize();
		u=u^n;u.normalize();
		v=n^u;
		Assert(!n.hasNaNs()&&!u.hasNaNs()&&!v.hasNaNs());

		GLdouble dx,dy,dz;
		dx=-(eye*u);dy=-(eye*v);dz=-(eye*n);

		m[0]=u.x;m[4]=u.y;m[8]=u.z;m[12]=dx;
		m[1]=v.x;m[5]=v.y;m[9]=v.z;m[13]=dy;
		m[2]=n.x;m[6]=n.y;m[10]=n.z;m[14]=dz;
		m[3]=0.0;m[7]=0.0;m[11]=0.0;m[15]=1.0;

		ygl::glMultMatrixf(m);
	}

	void gluSphere(GLdouble radius, GLint slices, GLint stacks)
	{

	}
}