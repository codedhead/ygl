#include "light.h"
#include "glcontext.h"
#include "geom.h"

void LightSource::lit(const Vertex& vert,const Normal& nrm,const Vector& v_eye,const Material& front_mat)
{
	GLdouble lamb,spot=1.,phong,atten;
	Vector v_li(position),h;

// 	vert.col_front_sec=(0,0,0,1);
// 	vert.col_back_sec=(0,0,0,1);

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
			v_li-=vert.p;
			GLfloat slen=v_li.normalize();
			atten=spot/(constant_attenuation+linear_attenuation*slen
				+quadratic_attenuation*slen*slen);
		}

		vert.col_front_pri+=atten*ambient*front_mat.ambient;

		lamb=v_li*nrm;
		lamb=YGL_CLAMP_MIN(lamb,0.f)*spot;
		if(lamb>0.f)
		{
			h=v_li+v_eye;h.normalize();
			phong=h*nrm;
			if(phong<0.0) phong=0.0;
			else
				phong=YGL_POW(phong,front_mat.shininess);

			vert.col_front_pri+=atten*(lamb*diffuse*front_mat.diffuse
				+phong*specular*front_mat.specular);
		}
	}

	//vert.col_back_pri=vert.col_front_pri;
}
void LightSource::lit_spec(const Vertex& vert,const Normal& nrm,const Vector& v_eye,const Material& front_mat)
{
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
			v_li-=vert.p;
			GLfloat slen=v_li.normalize();
			atten=spot/(constant_attenuation+linear_attenuation*slen
				+quadratic_attenuation*slen*slen);
		}

		vert.col_front_pri+=atten*ambient*front_mat.ambient;

		lamb=v_li*nrm;
		lamb=YGL_CLAMP_MIN(lamb,0.f)*spot;
		if(lamb>0.f)
		{
			h=v_li+v_eye;h.normalize();
			phong=h*nrm;
			if(phong<0.0) phong=0.0;
			else
				phong=YGL_POW(phong,front_mat.shininess);

			vert.col_front_pri+=atten*lamb*diffuse*front_mat.diffuse;
			vert.col_front_sec+=atten*phong*specular*front_mat.specular;
		}
	}

// 	vert.col_back_pri=vert.col_front_pri;
// 	vert.col_back_sec=vert.col_front_sec;
}

void do_lighting(const Vertex& vert)
{
	Vector view(0.f,0.f,1.f);
	if(glctx.light_model.local_viewer)
		view=Vector(0.0,0.0,0.0)-vert.p;

	vert.col_front_pri[0]=glctx.material_front.emission[0]+
		glctx.material_front.ambient[0]*glctx.light_model.ambient_scene[0];
	vert.col_front_pri[1]=glctx.material_front.emission[1]+
		glctx.material_front.ambient[1]*glctx.light_model.ambient_scene[1];
	vert.col_front_pri[2]=glctx.material_front.emission[2]+
		glctx.material_front.ambient[2]*glctx.light_model.ambient_scene[2];

	vert.col_front_pri[3]=glctx.material_front.diffuse[3];
	vert.col_back_pri[3]=glctx.material_back.diffuse[3];
	vert.col_front_sec=(0,0,0,1);
	vert.col_back_sec=(0,0,0,1);

	if(glctx.light_model.two_sided)
	{
		vert.col_back_pri[0]=glctx.material_back.emission[0]+
			glctx.material_back.ambient[0]*glctx.light_model.ambient_scene[0];
		vert.col_back_pri[1]=glctx.material_back.emission[1]+
			glctx.material_back.ambient[1]*glctx.light_model.ambient_scene[1];
		vert.col_back_pri[2]=glctx.material_back.emission[2]+
			glctx.material_back.ambient[2]*glctx.light_model.ambient_scene[2];

		if(glctx.light_model.color_control==GL_SINGLE_COLOR)
		{
			for(int i=0;i<GL_MAX_LIGHTS;++i)
				if(glctx.lights[i].enabled)
				{
					glctx.lights[i].lit2(vert,glctx.material_front,glctx.material_back,v);
				}
		}
		else
		{
			for(int i=0;i<GL_MAX_LIGHTS;++i)
				if(glctx.lights[i].enabled)
				{
					glctx.lights[i].lit2_spec(vert,glctx.material_front,glctx.material_back,v);
				}
		}
	}
	else
	{
		if(glctx.light_model.color_control==GL_SINGLE_COLOR)
		{
			
			for(int i=0;i<GL_MAX_LIGHTS;++i)
				if(glctx.lights[i].enabled)
				{
					glctx.lights[i].lit(vert,nrm,view,glctx.material_front);
				}
		}
		else
		{
			for(int i=0;i<GL_MAX_LIGHTS;++i)
				if(glctx.lights[i].enabled)
				{
					glctx.lights[i].lit_spec(vert,nrm,view,glctx.material_front);
				}
			vert.col_back_sec=vert.col_front_sec;
			
		}
		vert.col_back_pri=vert.col_front_pri;
	}

}