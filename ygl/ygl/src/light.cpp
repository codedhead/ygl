#include "../include/light.h"
#include "../include/gltypes.h"
extern GLContext gl_context;

#define POW_APPROXIMATE

void calcLightColor(GLVertex* vertex)
{
	Vector3 v(0.0,0.0,1.0);

	if(gl_context.local_viewer)
		v=Vector3(0.0,0.0,0.0)-Vector3(vertex->coord);


	vertex->front_color[0]=vertex->material_front.material_emission[0]+
		vertex->material_front.material_ambient[0]*gl_context.global_ambient_intensity[0];
	vertex->front_color[1]=vertex->material_front.material_emission[1]+
		vertex->material_front.material_ambient[1]*gl_context.global_ambient_intensity[1];
	vertex->front_color[2]=vertex->material_front.material_emission[2]+
		vertex->material_front.material_ambient[2]*gl_context.global_ambient_intensity[2];

	if(gl_context.two_side)
	{
		vertex->back_color[0]=vertex->material_front.material_emission[0]+
			vertex->material_front.material_ambient[0]*gl_context.global_ambient_intensity[0];
		vertex->back_color[1]=vertex->material_front.material_emission[1]+
			vertex->material_front.material_ambient[1]*gl_context.global_ambient_intensity[1];
		vertex->back_color[2]=vertex->material_front.material_emission[2]+
			vertex->material_front.material_ambient[2]*gl_context.global_ambient_intensity[2];

		if(gl_context.color_control==GL_SINGLE_COLOR)
		{
			for(int i=0;i<GL_MAX_LIGHTS;++i)
				if(gl_context.lights[i].enabled)
				{
					gl_context.lights[i].accumLightColor2(vertex,&v);		
				}
		}
		else
		{
			for(int i=0;i<GL_MAX_LIGHTS;++i)
				if(gl_context.lights[i].enabled)
				{
					gl_context.lights[i].accumLightColor2Separate(vertex,&v);		
				}
		}
		if(vertex->front_color[0]>1.0) vertex->front_color[0]=1.0;
		if(vertex->front_color[1]>1.0) vertex->front_color[1]=1.0;
		if(vertex->front_color[2]>1.0) vertex->front_color[2]=1.0;
		vertex->front_color[3]=vertex->material_front.material_ambient[3];
		if(vertex->back_color[0]>1.0) vertex->back_color[0]=1.0;
		if(vertex->back_color[1]>1.0) vertex->back_color[1]=1.0;
		if(vertex->back_color[2]>1.0) vertex->back_color[2]=1.0;
		vertex->back_color[3]=vertex->material_front.material_ambient[3];
	}
	else
	{
		if(gl_context.color_control==GL_SINGLE_COLOR)
		{
			for(int i=0;i<GL_MAX_LIGHTS;++i)
				if(gl_context.lights[i].enabled)
				{
					gl_context.lights[i].accumLightColor(vertex,&v);		
				}
		}
		else
		{
			for(int i=0;i<GL_MAX_LIGHTS;++i)
				if(gl_context.lights[i].enabled)
				{
					gl_context.lights[i].accumLightColorSeparate(vertex,&v);		
				}
		}

		if(vertex->front_color[0]>1.0) vertex->front_color[0]=1.0;
		if(vertex->front_color[1]>1.0) vertex->front_color[1]=1.0;
		if(vertex->front_color[2]>1.0) vertex->front_color[2]=1.0;
		vertex->front_color[3]=vertex->material_front.material_ambient[3];
	}	
}

GLLightSource::GLLightSource()
{
	position[0]=0.0;
	position[1]=0.0;
	position[2]=1.0;
	position[3]=0.0;
	ambient_intensity[0]=0.0;//only light0? opengl 1.1 spec http://www.opengl.org/documentation/specs/version1.1/glspec1.1/node32.html#SECTION005131100000000000000
	ambient_intensity[1]=0.0;
	ambient_intensity[2]=0.0;
	ambient_intensity[3]=1.0;
	diffuse_intensity[0]=1.0;
	diffuse_intensity[1]=1.0;
	diffuse_intensity[2]=1.0;
	diffuse_intensity[3]=1.0;
	specular_intensity[0]=1.0;
	specular_intensity[1]=1.0;
	specular_intensity[2]=1.0;
	specular_intensity[3]=1.0;
	spot_cutoff=180.0;
	spot_exponent=0;
	spot_direction[0]=0.0;
	spot_direction[1]=0.0;
	spot_direction[2]=-1.0;
	spot_direction[3]=1.0;
	constant_attenuation=1;linear_attenuation=0;quadratic_attenuation=0;
	enabled=false;
}

void GLLightSource::accumLightColor(GLVertex* vertex,Vector3* v)
{
	double lambert,phong,atten,slen;
	Vector3 s(position),m(vertex->normal),h;
	m.normalize();
	if(position[3]!=0.0)
	{
		s-=Vector3(vertex->coord);
		slen=s.normalize();
		atten=1.0/(constant_attenuation+linear_attenuation*slen
			+quadratic_attenuation*slen*slen);
	}
	else
	{
		s.normalize();
		s.x=-s.x;s.y=-s.y;s.z=-s.z;
		atten=1.0;
	}
	lambert=s.dot(m);
	if(lambert<0.0) lambert=0.0;

	h=s+*v;h.normalize();
	phong=h.dot(m);
	if(phong<0.0) phong=0.0;
	else 
#ifdef POW_APPROXIMATE
		phong=phong/(vertex->material_front.material_shininess-vertex->material_front.material_shininess*phong+phong);
#else
		phong=pow(phong,(double)vertex->material_front.material_shininess);
#endif

	
	//*spot?
	if(position[3]!=0.0)
	{
		Vector3 dir(spot_direction);dir.normalize();
		double spot=dir.dot(s);
		spot/=spot_exponent-spot_exponent*spot+spot;
		atten*=spot;
	}

	vertex->front_color[0]+=atten*(
		ambient_intensity[0]*vertex->material_front.material_ambient[0]
	+diffuse_intensity[0]*vertex->material_front.material_diffuse[0]*lambert
		+specular_intensity[0]*vertex->material_front.material_specular[0]*phong);
	vertex->front_color[1]+=atten*(
		ambient_intensity[1]*vertex->material_front.material_ambient[1]
	+diffuse_intensity[1]*vertex->material_front.material_diffuse[1]*lambert
		+specular_intensity[1]*vertex->material_front.material_specular[1]*phong);
	vertex->front_color[2]+=atten*(
		ambient_intensity[2]*vertex->material_front.material_ambient[2]
	+diffuse_intensity[2]*vertex->material_front.material_diffuse[2]*lambert
		+specular_intensity[2]*vertex->material_front.material_specular[2]*phong);	
	
}

void GLLightSource::accumLightColorSeparate(GLVertex* vertex,Vector3* v)
{
	double lambert,phong,atten,slen;
	Vector3 s(position),m(vertex->normal),h;
	m.normalize();
	if(position[3]!=0.0)
	{
		s-=Vector3(vertex->coord);
		slen=s.normalize();
		atten=1.0/(constant_attenuation+linear_attenuation*slen
			+quadratic_attenuation*slen*slen);
	}
	else
	{
		s.normalize();
		s.x=-s.x;s.y=-s.y;s.z=-s.z;
		atten=1.0;
	}
	lambert=s.dot(m);
	if(lambert<0.0) lambert=0.0;

	h=s+*v;h.normalize();
	phong=h.dot(m);
	if(phong<0.0) phong=0.0;
	else
#ifdef POW_APPROXIMATE
		phong=phong/(vertex->material_front.material_shininess-vertex->material_front.material_shininess*phong+phong);
#else	
		phong=pow(phong,(double)vertex->material_front.material_shininess);
#endif


	//*spot?
	if(position[3]!=0.0)
	{
		Vector3 dir(spot_direction);dir.normalize();
		double spot=dir.dot(s);
		spot/=spot_exponent-spot_exponent*spot+spot;
		atten*=spot;
	}

	vertex->front_color[0]+=atten*(
		ambient_intensity[0]*vertex->material_front.material_ambient[0]
	+diffuse_intensity[0]*vertex->material_front.material_diffuse[0]*lambert);
	vertex->front_color[1]+=atten*(
		ambient_intensity[1]*vertex->material_front.material_ambient[1]
	+diffuse_intensity[1]*vertex->material_front.material_diffuse[1]*lambert);
	vertex->front_color[2]+=atten*(
		ambient_intensity[2]*vertex->material_front.material_ambient[2]
	+diffuse_intensity[2]*vertex->material_front.material_diffuse[2]*lambert);	

	vertex->specular_front_color[0]=atten*specular_intensity[0]*vertex->material_front.material_specular[0]*phong;
	vertex->specular_front_color[1]=atten*specular_intensity[0]*vertex->material_front.material_specular[0]*phong;
	vertex->specular_front_color[2]=atten*specular_intensity[0]*vertex->material_front.material_specular[0]*phong;
}
void GLLightSource::accumLightColor2(GLVertex* vertex,Vector3* v)
{
	double lambert1,phong1,lambert2,phong2,atten,slen;
	Vector3 s(position),m1(vertex->normal),m2,h;
	m1.normalize();m2=-m1;
	if(position[3]!=0.0)
	{
		s-=Vector3(vertex->coord);
		slen=s.normalize();
		atten=1.0/(constant_attenuation+linear_attenuation*slen
			+quadratic_attenuation*slen*slen);
	}
	else
	{
		s.normalize();
		s.x=-s.x;s.y=-s.y;s.z=-s.z;
		atten=1.0;
	}
	lambert1=s.dot(m1);
	if(lambert1<0.0) lambert1=0.0;
	lambert2=s.dot(m2);
	if(lambert2<0.0) lambert2=0.0;

	h=s+*v;h.normalize();
	phong1=h.dot(m1);
	if(phong1<0.0) phong1=0.0;
	else
#ifdef POW_APPROXIMATE
		phong1=phong1/(vertex->material_front.material_shininess-vertex->material_front.material_shininess*phong1+phong1);
#else
		phong1=pow(phong1,(double)vertex->material_front.material_shininess);
#endif

	phong2=h.dot(m2);
	if(phong2<0.0) phong2=0.0;
	else
#ifdef POW_APPROXIMATE
		phong2=phong2/(vertex->material_back.material_shininess-vertex->material_back.material_shininess*phong2+phong2);
#else 
		phong2=pow(phong2,(double)vertex->material_back.material_shininess);
#endif


	//*spot?
	if(position[3]!=0.0)
	{
		Vector3 dir(spot_direction);dir.normalize();
		double spot=dir.dot(s);
		spot/=spot_exponent-spot_exponent*spot+spot;
		atten*=spot;
	}

	vertex->front_color[0]+=atten*(
		ambient_intensity[0]*vertex->material_front.material_ambient[0]
	+diffuse_intensity[0]*vertex->material_front.material_diffuse[0]*lambert1
		+specular_intensity[0]*vertex->material_front.material_specular[0]*phong1);
	vertex->front_color[1]+=atten*(
		ambient_intensity[1]*vertex->material_front.material_ambient[1]
	+diffuse_intensity[1]*vertex->material_front.material_diffuse[1]*lambert1
		+specular_intensity[1]*vertex->material_front.material_specular[1]*phong1);
	vertex->front_color[2]+=atten*(
		ambient_intensity[2]*vertex->material_front.material_ambient[2]
	+diffuse_intensity[2]*vertex->material_front.material_diffuse[2]*lambert1
		+specular_intensity[2]*vertex->material_front.material_specular[2]*phong1);	

	vertex->back_color[0]+=atten*(
		ambient_intensity[0]*vertex->material_back.material_ambient[0]
	+diffuse_intensity[0]*vertex->material_back.material_diffuse[0]*lambert2
		+specular_intensity[0]*vertex->material_back.material_specular[0]*phong2);
	vertex->back_color[1]+=atten*(
		ambient_intensity[1]*vertex->material_back.material_ambient[1]
	+diffuse_intensity[1]*vertex->material_back.material_diffuse[1]*lambert2
		+specular_intensity[1]*vertex->material_back.material_specular[1]*phong2);
	vertex->back_color[2]+=atten*(
		ambient_intensity[2]*vertex->material_back.material_ambient[2]
	+diffuse_intensity[2]*vertex->material_back.material_diffuse[2]*lambert2
		+specular_intensity[2]*vertex->material_back.material_specular[2]*phong2);	
}

void GLLightSource::accumLightColor2Separate(GLVertex* vertex,Vector3* v)
{
	double lambert1,phong1,lambert2,phong2,atten,slen;
	Vector3 s(position),m1(vertex->normal),m2,h;
	m1.normalize();m2=-m1;
	if(position[3]!=0.0)
	{
		s-=Vector3(vertex->coord);
		slen=s.normalize();
		atten=1.0/(constant_attenuation+linear_attenuation*slen
			+quadratic_attenuation*slen*slen);
	}
	else
	{
		s.normalize();
		s.x=-s.x;s.y=-s.y;s.z=-s.z;
		atten=1.0;
	}
	lambert1=s.dot(m1);
	if(lambert1<0.0) lambert1=0.0;
	lambert2=s.dot(m2);
	if(lambert2<0.0) lambert2=0.0;

	h=s+*v;h.normalize();
	phong1=h.dot(m1);
	if(phong1<0.0) phong1=0.0;
	else
#ifdef POW_APPROXIMATE
		phong1=phong1/(vertex->material_front.material_shininess-vertex->material_front.material_shininess*phong1+phong1);
#else
		phong1=pow(phong1,(double)vertex->material_front.material_shininess);
#endif

	phong2=h.dot(m2);
	if(phong2<0.0) phong2=0.0;
	else 
#ifdef POW_APPROXIMATE
		phong2=phong2/(vertex->material_back.material_shininess-vertex->material_back.material_shininess*phong2+phong2);
#else
		phong2=pow(phong2,(double)vertex->material_back.material_shininess);
#endif


	//*spot?
	if(position[3]!=0.0)
	{
		Vector3 dir(spot_direction);dir.normalize();
		double spot=dir.dot(s);
		spot/=spot_exponent-spot_exponent*spot+spot;
		atten*=spot;
	}

	vertex->front_color[0]+=atten*(
		ambient_intensity[0]*vertex->material_front.material_ambient[0]
	+diffuse_intensity[0]*vertex->material_front.material_diffuse[0]*lambert1);
	vertex->front_color[1]+=atten*(
		ambient_intensity[1]*vertex->material_front.material_ambient[1]
	+diffuse_intensity[1]*vertex->material_front.material_diffuse[1]*lambert1);
	vertex->front_color[2]+=atten*(
		ambient_intensity[2]*vertex->material_front.material_ambient[2]
	+diffuse_intensity[2]*vertex->material_front.material_diffuse[2]*lambert1);	

	vertex->back_color[0]+=atten*(
		ambient_intensity[0]*vertex->material_back.material_ambient[0]
	+diffuse_intensity[0]*vertex->material_back.material_diffuse[0]*lambert2);
	vertex->back_color[1]+=atten*(
		ambient_intensity[1]*vertex->material_back.material_ambient[1]
	+diffuse_intensity[1]*vertex->material_back.material_diffuse[1]*lambert2);
	vertex->back_color[2]+=atten*(
		ambient_intensity[2]*vertex->material_back.material_ambient[2]
	+diffuse_intensity[2]*vertex->material_back.material_diffuse[2]*lambert2);	

	vertex->specular_front_color[0]=atten*specular_intensity[0]*vertex->material_front.material_specular[0]*phong1;
	vertex->specular_front_color[1]=atten*specular_intensity[0]*vertex->material_front.material_specular[0]*phong1;
	vertex->specular_front_color[2]=atten*specular_intensity[0]*vertex->material_front.material_specular[0]*phong1;

	vertex->specular_back_color[0]=atten*specular_intensity[0]*vertex->material_back.material_specular[0]*phong2;
	vertex->specular_back_color[1]=atten*specular_intensity[0]*vertex->material_back.material_specular[0]*phong2;
	vertex->specular_back_color[2]=atten*specular_intensity[0]*vertex->material_back.material_specular[0]*phong2;
}