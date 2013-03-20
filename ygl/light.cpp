#include "light.h"
#include "glcontext.h"
#include "geom.h"
#include "global.h"

#include <stdio.h>

void lit2(Vertex& vert,const Material& front_mat,const Material& back_mat,const Vector& view,GLfloat* color)
{
	_NOT_IMPLEMENTED_(lit2);
}
void lit2_spec(Vertex& vert,const Material& front_mat,const Material& back_mat,const Vector& view,GLfloat* color,GLfloat* spec_color)
{
	_NOT_IMPLEMENTED_(lit2_spec);
}

#define VEC3_ADD_DOT(res,a,b) \
	(res)[0]+=(a)[0]*(b)[0];\
	(res)[1]+=(a)[1]*(b)[1];\
	(res)[2]+=(a)[2]*(b)[2];
#define VEC4_ADD_DOT(res,a,b) \
	VEC3_ADD_DOT(res,a,b) \
	(res)[3]+=(a)[3]*(b)[3];

#define VEC4_addby_ka_dot_b(res,k,a,b) \
	(res)[0]+=(k)*(a)[0]*(b)[0];\
	(res)[1]+=(k)*(a)[1]*(b)[1];\
	(res)[2]+=(k)*(a)[2]*(b)[2];\
	(res)[3]+=(k)*(a)[3]*(b)[3];

void LightSource::lit(Vertex* vert,const Normal& nrm,const Vector& v_eye,const Material& front_mat)
{
	GLdouble lamb,spot=1.,phong,atten=0.;
	// if directional, assume normalized
	Vector v_li(position[0],position[1],position[2]),h;

// 	vert->col_front_sec=(0,0,0,1);
// 	vert->col_back_sec=(0,0,0,1);

	if(spot_cutoff_cos!=-1.f)
	{
		Assert(W_OF(position)!=0.f);
		v_li-=Vector(vert->p[0],vert->p[1],vert->p[2]);
		GLfloat slen=v_li.normalize();
		GLfloat dotv=-(v_li*spot_direction);
		spot=dotv<spot_cutoff_cos?0.:YGL_POW(dotv,spot_exponent);
		if(spot>0.)
			atten=spot/(constant_attenuation+linear_attenuation*slen
				+quadratic_attenuation*slen*slen);
	}
	else if(W_OF(position)==0.f) // directional
	{
		atten=1.;
	}
	else
	{
		v_li-=Vector(vert->p[0],vert->p[1],vert->p[2]);
		GLfloat slen=v_li.normalize();
		atten=1./(constant_attenuation+linear_attenuation*slen
			+quadratic_attenuation*slen*slen);
	}

	if(atten>0.f)
	{

		//vert->col_front_pri+=atten*ambient*front_mat.ambient;
		VEC4_addby_ka_dot_b(vert->col_front_pri,atten,this->ambient,front_mat.ambient);

		lamb=v_li*nrm;
		lamb=YGL_CLAMP_MIN(lamb,0.f)*spot;
		if(lamb>0.f)
		{
			h=v_li+v_eye;h.normalize();
			phong=h*nrm;
			if(phong<0.0) phong=0.0;
			else
				phong=YGL_POW(phong,front_mat.shininess);

			lamb*=atten;phong*=atten;
			vert->col_front_pri[0]+=(lamb*diffuse[0]*front_mat.diffuse[0]
				+phong*specular[0]*front_mat.specular[0]);
			vert->col_front_pri[1]+=(lamb*diffuse[1]*front_mat.diffuse[1]
				+phong*specular[1]*front_mat.specular[1]);
			vert->col_front_pri[2]+=(lamb*diffuse[2]*front_mat.diffuse[2]
				+phong*specular[2]*front_mat.specular[2]);
			vert->col_front_pri[3]+=(lamb*diffuse[3]*front_mat.diffuse[3]
				+phong*specular[3]*front_mat.specular[3]);
// 			vert->col_front_pri[0]+=atten*(lamb*diffuse[0]*front_mat.diffuse[0]
// 				+phong*specular[0]*front_mat.specular[0]);
// 			vert->col_front_pri[1]+=atten*(lamb*diffuse[1]*front_mat.diffuse[1]
// 				+phong*specular[1]*front_mat.specular[1]);
// 			vert->col_front_pri[2]+=atten*(lamb*diffuse[2]*front_mat.diffuse[2]
// 				+phong*specular[2]*front_mat.specular[2]);
// 			vert->col_front_pri[3]+=atten*(lamb*diffuse[3]*front_mat.diffuse[3]
// 				+phong*specular[3]*front_mat.specular[3]);
		}
	}

	//vert->col_back_pri=vert->col_front_pri;
}
void LightSource::lit_spec(Vertex* vert,const Normal& nrm,const Vector& v_eye,const Material& front_mat)
{
	/*
	GLdouble lamb,spot=1.,phong,atten;
	Vector v_li(position),h;

	if(spot_cutoff_cos!=-1.f)
	{
		GLfloat dotv=-v_li*spot_direction;
		spot=dotv<spot_cutoff_cos?0.:YGL_POW(dotv,spot_exponent);
	}
	if(spot>0.f)
	{
		if(position[3]==0.f) // directional
		{
			v_li.do_negate();
			atten=spot;
		}
		else
		{
			v_li-=vert->p;
			GLfloat slen=v_li.normalize();
			atten=spot/(constant_attenuation+linear_attenuation*slen
				+quadratic_attenuation*slen*slen);
		}

		vert->col_front_pri+=atten*ambient*front_mat.ambient;

		lamb=v_li*nrm;
		lamb=YGL_CLAMP_MIN(lamb,0.f)*spot;
		if(lamb>0.f)
		{
			h=v_li+v_eye;h.normalize();
			phong=h*nrm;
			if(phong<0.0) phong=0.0;
			else
				phong=YGL_POW(phong,front_mat.shininess);

			vert->col_front_pri+=atten*lamb*diffuse*front_mat.diffuse;
			vert->col_front_sec+=atten*phong*specular*front_mat.specular;
		}
	}
*/
// 	vert->col_back_pri=vert->col_front_pri;
// 	vert->col_back_sec=vert->col_front_sec;
}

void do_lighting(Vertex* vert,const Normal& nrm)
{
	Vector view(0.f,0.f,1.f);
	if(glctx.light_model.local_viewer)
		view=(Point(0.f)-Point(vert->p)).hat();

	vert->col_front_pri[0]=glctx.material_front.emission[0]+
		glctx.material_front.ambient[0]*glctx.light_model.ambient_scene[0];
	vert->col_front_pri[1]=glctx.material_front.emission[1]+
		glctx.material_front.ambient[1]*glctx.light_model.ambient_scene[1];
	vert->col_front_pri[2]=glctx.material_front.emission[2]+
		glctx.material_front.ambient[2]*glctx.light_model.ambient_scene[2];

	vert->col_front_pri[3]=glctx.material_front.diffuse[3];
	ASSIGN_F4_WITH(vert->col_front_sec,0.f,0.f,0.f,1.f);

	if(glctx.light_model.two_sided)
	{
		vert->col_back_pri[3]=glctx.material_back.diffuse[3];
		ASSIGN_F4_WITH(vert->col_back_sec,0.f,0.f,0.f,1.f);
		/*
		vert->col_back_pri[0]=glctx.material_back.emission[0]+
			glctx.material_back.ambient[0]*glctx.light_model.ambient_scene[0];
		vert->col_back_pri[1]=glctx.material_back.emission[1]+
			glctx.material_back.ambient[1]*glctx.light_model.ambient_scene[1];
		vert->col_back_pri[2]=glctx.material_back.emission[2]+
			glctx.material_back.ambient[2]*glctx.light_model.ambient_scene[2];

		if(glctx.light_model.color_control==GL_SINGLE_COLOR)
		{
			for(int i=0;i<YGL_MAX_LIGHTS;++i)
				if(YGL_IS_LIGHT_ENABLED(i))
				{
					glctx.lights[i].lit2(vert,glctx.material_front,glctx.material_back,v);
				}
		}
		else
		{
			for(int i=0;i<YGL_MAX_LIGHTS;++i)
				if(YGL_IS_LIGHT_ENABLED(i))
				{
					glctx.lights[i].lit2_spec(vert,glctx.material_front,glctx.material_back,v);
				}
		}*/
	}
	else
	{
		//if(glctx.light_model.color_control==GL_SINGLE_COLOR)
		{
			
			for(int i=0;i<YGL_MAX_LIGHTS;++i)
				if(YGL_IS_LIGHT_ENABLED(i))
				{
					glctx.lights[i].lit(vert,nrm,view,glctx.material_front);
				}
		}
		/*else
		{
			for(int i=0;i<YGL_MAX_LIGHTS;++i)
				if(YGL_IS_LIGHT_ENABLED(i))
				{
					glctx.lights[i].lit_spec(vert,nrm,view,glctx.material_front);
				}
			ASSIGN_F4(vert->col_back_sec,vert->col_front_sec);
			
		}*/
	}

}