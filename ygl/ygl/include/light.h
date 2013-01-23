#ifndef _YGL_LIGHT_H_
#define _YGL_LIGHT_H_

#include "ygl.h"

struct GLVertex;
struct GLMaterial
{
	GLfloat material_ambient[4];
	GLfloat material_diffuse[4];
	GLfloat material_specular[4];
	GLfloat material_emission[4];
	GLfloat material_shininess;
};

class GLLightSource
{
public:
	GLfloat position[4];
	GLfloat ambient_intensity[4],diffuse_intensity[4],specular_intensity[4];
	GLfloat spot_cutoff,spot_exponent,spot_direction[4];
	GLfloat constant_attenuation,linear_attenuation,quadratic_attenuation;
	bool enabled;

	GLLightSource();
	void accumLightColor(GLVertex* vertex,Vector3* v);
	void accumLightColorSeparate(GLVertex* vertex,Vector3* v);
	void accumLightColor2(GLVertex* vertex,Vector3* v);
	void accumLightColor2Separate(GLVertex* vertex,Vector3* v);
};

void calcLightColor(GLVertex* vertex);

#endif