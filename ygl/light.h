#ifndef _YGL_LIGHT_
#define _YGL_LIGHT_

#include "ygltypes.h"
#include "yglstructs.h"
using namespace ygl;

struct Material
{
	Material(){
		ambient[0]=ambient[1]=ambient[2]=0.2f;ambient[3]=1.f;
		diffuse[0]=diffuse[1]=diffuse[2]=0.8f;diffuse[3]=1.f;
		specular[0]=specular[1]=specular[2]=0.f;specular[3]=1.f;
		emission[0]=emission[1]=emission[2]=0.f;emission[3]=1.f;
		shininess=0.f;
	}
	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat emission[4];
	GLfloat shininess; // specular exponet
};

struct LightSource
{
	LightSource(){
		position[0]=position[1]=position[3]=0.f;position[2]=1.f;
		spot_direction[0]=spot_direction[1]=0.f;spot_direction[2]=-1.f;
		ambient[0]=ambient[1]=ambient[2]=0.f;ambient[3]=1.f; // 1.f for light[0]
		diffuse[0]=diffuse[1]=diffuse[2]=0.f;diffuse[3]=1.f;
		specular[0]=specular[1]=specular[2]=0.f;specular[3]=1.f; // 1.f for light[0]

		spot_cutoff_cos=-1.f;
		spot_exponent=0.f;
		constant_attenuation=1.f;
		linear_attenuation=0.f;
		quadratic_attenuation=0.f;
	}

	void lit(Vertex* vert,const Normal& nrm,const Vector& v_eye,const Material& front_mat);
	void lit_spec(Vertex* vert,const Normal& nrm,const Vector& v_eye,const Material& front_mat);
	void lit2(Vertex* vert,const Material& front_mat,const Material& back_mat,const Vector& view,GLfloat* color);
	void lit2_spec(Vertex* vert,const Material& front_mat,const Material& back_mat,const Vector& view,GLfloat* color,GLfloat* spec_color);


	GLfloat position[4];
	// intensity
	GLfloat ambient[4],diffuse[4],specular[4];
	GLfloat spot_cutoff_cos,spot_exponent;//,spot_direction[3];
	Vector spot_direction;
	GLfloat constant_attenuation,linear_attenuation,quadratic_attenuation;
	
};

void do_lighting(Vertex* vert,const Normal& nrm);


#endif