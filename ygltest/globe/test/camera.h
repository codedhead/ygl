#pragma once

#include "../ygl/ygl.h"

class Camera
{
private:
	Vector3 eye,u,v,n;
	void setModelViewMatrix();
public:
	Camera(void);
	~Camera(void);

	void setDefaultCamera();
	void set(Vector3& eye,Vector3& look,Vector3& up);
	void roll(GLfloat angle);
	void pitch(GLfloat angle);
	void yaw(GLfloat angle);
	void slide(GLfloat du, GLfloat dv, GLfloat dn);
};
