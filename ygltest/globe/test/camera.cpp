#include "camera.h"

Camera::Camera(void)
{
	setDefaultCamera();
}

Camera::~Camera(void)
{
}

void Camera::setDefaultCamera()
{
	eye.x=eye.y=eye.z=0.0;
	n.x=n.y=0.0;n.z=1.0;
	u.x=1.0;u.y=u.z=0.0;
	v.x=v.z=0;v.y=1.0;
	setModelViewMatrix();
}

void Camera::setModelViewMatrix()
{
	GLfloat m[16],dx,dy,dz;
	dx=-eye.dot(u);dy=-eye.dot(v);dz=-eye.dot(n);

	m[0]=u.x;m[4]=u.y;m[8]=u.z;m[12]=dx;
	m[1]=v.x;m[5]=v.y;m[9]=v.z;m[13]=dy;
	m[2]=n.x;m[6]=n.y;m[10]=n.z;m[14]=dz;
	m[3]=0.0;m[7]=0.0;m[11]=0.0;m[15]=1.0;

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m);
}

void Camera::set(Vector3& eye,Vector3& look,Vector3& up)
{
	this->eye=eye;
	n=eye-look;
	u=up*n;
	v=n*u;
	n.normalize();u.normalize();v.normalize();
	setModelViewMatrix();
}
//around u
void Camera::roll(GLfloat angle)
{
	Vector3 oldn=n,oldv=v;
	v=v*cos(angle)+n*sin(angle);
	n=oldn*cos(angle)-oldv*sin(angle);
	setModelViewMatrix();
}
//around n
void Camera::pitch(GLfloat angle)
{
	Vector3 oldu=u,oldv=v;
	v=v*cos(angle)-u*sin(angle);
	u=oldu*cos(angle)+oldv*sin(angle);
	setModelViewMatrix();
}
//around v
void Camera::yaw(GLfloat angle)
{
	Vector3 oldn=n,oldu=u;
	u=u*cos(angle)-n*sin(angle);
	n=oldn*cos(angle)+oldu*sin(angle);
	setModelViewMatrix();
}
void Camera::slide(GLfloat du, GLfloat dv, GLfloat dn)
{
	eye+=u*du+v*dv+n*dn;
	setModelViewMatrix();
}