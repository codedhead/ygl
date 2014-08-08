#include "raster.h"
#include "yglstructs.h"
#include "buffer.h"
#include "global.h"
#include "yglconstants.h"
#include "texture.h"
#include "glcontext.h"
#include <stdio.h>
//#define FILL_TOP_LEFT

// top -> max
// bottom -> min

// don't open this macro, mem_factory not tested
//#define USE_MEM_FACTORY

#ifdef USE_MEM_FACTORY
#include "memory.h"
#endif
namespace ygl{
namespace raster
{

#define DECLARE_FACTORY(cla_name) TFactory<cla_name> Factory_##cla_name
// 	struct ETable;
// 	struct ETRecord;
// 	struct ETRecord_PColor;
// 	struct ETRecord_PColor_PTex;
	

#ifdef USE_MEM_FACTORY
	extern DECLARE_FACTORY(ETable);
	extern DECLARE_FACTORY(ETRecord);
	extern DECLARE_FACTORY(ETRecord_PColor);
	extern DECLARE_FACTORY(ETRecord_PColor_PTex);
	
#define NEW_ETABLE (Factory_ETable.alloc())
#define DELETE_ETABLE(et) Factory_ETable.recycle(et)

#define NEW_REC (Factory_ETRecord.alloc())
#define DELETE_REC(rec) (rec)->recycle_me()

#define NEW_REC_COLOR (Factory_ETRecordPSmooth.alloc())
#define NEW_REC_COLOR (Factory_ETRecordPSmoothTex.alloc())

#else

#define NEW_ETABLE (new ETable)
#define DELETE_ETABLE(et) delete (et)

#define NEW_FLAT (new FragShaderFlat)
#define DELETE_SHADER(rec) delete (rec)

#define NEW_COLOR (new FragShader_Color)
#define NEW_TEX (new FragShader_Tex)
#define NEW_FLAT_TEX (new FragShader_Flat_Tex)
#define NEW_COLOR_TEX (new FragShader_Color_Tex)

#define NEW_FLAT_FOG (new FragShader_Flat_Fog)
#define NEW_COLOR_FOG (new FragShader_Color_Fog)
#define NEW_TEX_FOG (new FragShader_Tex_Fog)

#define NEW_FLAT_TEX_FOG (new FragShader_Flat_Tex_Fog)
#define NEW_COLOR_TEX_FOG (new FragShader_Color_Tex_Fog)

#define NEW_P_COLOR (new FragShader_P_Color)
#define NEW_P_TEX (new FragShader_P_Tex)
#define NEW_P_FLAT_TEX (new FragShader_P_Flat_Tex)
#define NEW_P_COLOR_TEX (new FragShader_P_Color_Tex)

#define NEW_P_FLAT_FOG (new FragShader_P_Flat_Fog)
#define NEW_P_COLOR_FOG (new FragShader_P_Color_Fog)
#define NEW_P_TEX_FOG (new FragShader_P_Tex_Fog)

#define NEW_P_FLAT_TEX_FOG (new FragShader_P_Flat_Tex_Fog)
#define NEW_P_COLOR_TEX_FOG (new FragShader_P_Color_Tex_Fog)

#endif
	
	void point(Vertex* v)
	{
		cbuf_type color[4]={
			YGL_COLOR_F2I(v->col_front_pri[0]),
			YGL_COLOR_F2I(v->col_front_pri[1]),
			YGL_COLOR_F2I(v->col_front_pri[2]),
			YGL_COLOR_F2I(v->col_front_pri[3])};

		zbuf_type z=FLOAT_AS_UINT(Z_OF(v->p));
		buffer::plot(ROUND(X_OF(v->p)),ROUND(Y_OF(v->p)),z,color);
	}

	void line(Vertex* v)
	{
		// line clipping would preserve vertex order, so use second vertex's color here
		GLubyte color[4]={
			YGL_COLOR_F2I(v[1].col_front_pri[0]),
			YGL_COLOR_F2I(v[1].col_front_pri[1]),
			YGL_COLOR_F2I(v[1].col_front_pri[2]),
			255
		};

// 		buffer::plot_ubyte_func plot_func=(buffer::z_test_mask==FLOAT_ALWAYS?
// 			buffer::plot_ubyte_no_ztest:
// 			buffer::plot_ubyte);

		int x0=ROUND(v[0].p[0]),
			y0=ROUND(v[0].p[1]),
			x1=ROUND(v[1].p[0]),
			y1=ROUND(v[1].p[1]);

#define SET_Z_DZ(from,to,_dd) z=Z_OF(from);dz=(Z_OF(to)-z)/(GLfloat(_dd));
		GLfloat z,dz; // need correction??


		// trival case
		if(x0==x1)
		{
			if(y1<y0)
			{
				SET_Z_DZ(v[1].p,v[0].p,y0-y1);

				XOR_SWAP(y0,y1);
			}
			else
			{
				SET_Z_DZ(v[0].p,v[1].p,y1-y0);
			}

			for(int y=y0;y<=y1;++y,z+=dz)
				buffer::plot(x0,y,(zbuf_type)z,color);

			return;
		}
		else if(y0==y1)
		{
			if(x1<x0)
			{
				SET_Z_DZ(v[1].p,v[0].p,x0-x1);
				XOR_SWAP(x0,x1);
			}
			else
			{
				SET_Z_DZ(v[0].p,v[1].p,x1-x0);
			}

			for(int x=x0;x<=x1;++x,z+=dz)
				buffer::plot(x,y0,z,color);

			return;
		}

		int steep=abs(x1-x0)-abs(y0-y1);
		if(steep==0)
		{
			if(x1<x0)
			{
				SET_Z_DZ(v[1].p,v[0].p,x0-x1);

				XOR_SWAP(x0,x1);
				XOR_SWAP(y0,y1);
			}
			else
			{
				SET_Z_DZ(v[0].p,v[1].p,x1-x0);
			}

			if(y0<y1)
			{
				for(int x=x0,y=y0;x<=x1;++x,++y,z+=dz)
					buffer::plot(x,y,z,color);
			}
			else
			{
				for(int x=x0,y=y0;x<=x1;++x,--y,z+=dz)
					buffer::plot(x,y,z,color);
			}
		}
		else
		{
			// steep
			if(steep<0)
			{
				XOR_SWAP(x0,y0);
				XOR_SWAP(x1,y1);
			}
			if(x1<x0)
			{
				SET_Z_DZ(v[1].p,v[0].p,x0-x1);

				XOR_SWAP(x0,x1);
				XOR_SWAP(y0,y1);
			}
			else
			{
				SET_Z_DZ(v[0].p,v[1].p,x1-x0);
			}

			const int inc_y=y0<y1?1:-1;

			// x0<=x1, dx>0
			int dx=x1-x0,dy=abs(y1-y0); // check!!
			int D=dy+dy-dx;
			int delta1=dy+dy,delta2=dy+dy-dx-dx;

// 			for(int x=x0,y=y0;x<=x1;++x)
// 			{
// 				if(D<=0) // use bottom
// 				{
// 					D+=delta1;
// 					if(steep<0) plot_func(y,x,color);
// 					else plot_func(x,y,color);
// 				}
// 				else
// 				{
// 					D+=delta2;
// 					if(steep<0) plot_func((y+=inc_y),x,color);
// 					else plot_func(x,(y+=inc_y),color);
// 				}
// 			}
			if(steep<0)
			{
				for(int x=x0,y=y0;x<=x1;++x,z+=dz)
				{
					if(D<=0) // use bottom
					{
						D+=delta1;
						buffer::plot(y,x,z,color);
					}
					else
					{
						D+=delta2;
						buffer::plot((y+=inc_y),x,z,color);
					}
				}
			}
			else
			{
				for(int x=x0,y=y0;x<=x1;++x,z+=dz)
				{
					if(D<=0) // use bottom
					{
						D+=delta1;
						buffer::plot(x,y,z,color);
					}
					else
					{
						D+=delta2;
						buffer::plot(x,(y+=inc_y),z,color);
					}
				}
			}
		}

	}

///////////////////////////////////////////////////////////////////////////////////////////
// polygon
typedef void (*FragmentInitFunc)(Vertex* v,GLfloat* att);
typedef void (*FragmentShadeFunc)(GLfloat* attribs,GLfloat* ddx,GLfloat* ddy,cbuf_type* flat_color,TextureObject* texobj,cbuf_type* res_color);
//typedef void (*FragmentShadeFunc_NoPersp)(GLfloat* attribs,GLfloat* ddx,GLfloat* ddy,cbuf_type* flat_color,TextureObject* texobj,cbuf_type* res_color);
//typedef void (*FragmentShadeFunc_Persp)(GLfloat* attribs,GLfloat* ddx_w,GLfloat* ddy_w,GLfloat w,cbuf_type* flat_color,TextureObject* texobj,cbuf_type* res_color);

namespace fragment_shader
{
#define ___R 0
#define ___G 1
#define ___B 2
#define ___A 3
	inline void init_color(Vertex* v,GLfloat* att)
	{
#define _INIT_COLOR \
	att[___R]=YGL_COLOR_F2IF(v->col_front_pri[0]);\
	att[___G]=YGL_COLOR_F2IF(v->col_front_pri[1]);\
	att[___B]=YGL_COLOR_F2IF(v->col_front_pri[2]);\
	att[___A]=YGL_COLOR_F2IF(v->col_front_pri[3]);

		_INIT_COLOR
	}
	inline void shade_color(GLfloat* attribs,GLfloat* ddx,GLfloat* ddy,cbuf_type* flat_color,TextureObject* texobj,cbuf_type* res_color)
	{
		res_color[0]=(cbuf_type)(attribs[___R]);
		res_color[1]=(cbuf_type)(attribs[___G]);
		res_color[2]=(cbuf_type)(attribs[___B]);
		res_color[3]=(cbuf_type)(attribs[___A]);
	}
#undef ___R
#undef ___G
#undef ___B
#undef ___A

#define ___S 0
#define ___T 1
	inline void init_tex(Vertex* v,GLfloat* att)
	{
#define _INIT_TEX \
	att[___S]=v->tex_coords[0];\
	att[___T]=v->tex_coords[1];

		_INIT_TEX
	}
	// tex replace mode
	inline void shade_tex(GLfloat* attribs,GLfloat* ddx,GLfloat* ddy,cbuf_type* flat_color,TextureObject* texobj,cbuf_type* res_color)
	{
#define TEX_COMPUTE_RHO \
	GLfloat dudx=ddx[___S]*texobj->width(),dudy=ddy[___S]*texobj->width(),\
		dvdx=ddx[___T]*texobj->height(),dvdy=ddy[___T]*texobj->height();\
	GLfloat rho=max(sqrtf(dudx*dudx+dvdx*dvdx),sqrtf(dudy*dudy+dvdy*dvdy));

		TEX_COMPUTE_RHO
		cbuf_type tex_color[4];
		texobj->fetch255(rho,attribs[___S],attribs[___T],tex_color);
		ASSIGN_V4(res_color,tex_color);
	}
#undef ___S
#undef ___T

#define ___S 0
#define ___T 1
	// modulate mode? on flat_color
	inline void shade_flat_tex(GLfloat* attribs,GLfloat* ddx,GLfloat* ddy,cbuf_type* flat_color,TextureObject* texobj,cbuf_type* res_color)
	{
#define R_FRAG flat_color[0]
#define G_FRAG flat_color[1]
#define B_FRAG flat_color[2]
#define A_FRAG flat_color[3]

#define _SHADE_FRAG_TEX \
	GLfloat tex_color[4]; \
	TEX_COMPUTE_RHO;\
	texobj->fetch(rho,attribs[___S],attribs[___T],tex_color);\
	switch(glctx.tex_env_mode)\
	{\
	case GL_MODULATE:\
		res_color[0]=(cbuf_type)(tex_color[0]*R_FRAG);\
		res_color[1]=(cbuf_type)(tex_color[1]*G_FRAG);\
		res_color[2]=(cbuf_type)(tex_color[2]*B_FRAG);\
		res_color[3]=(cbuf_type)(tex_color[3]*A_FRAG);\
		break;\
	case GL_DECAL:\
		res_color[0]=lerp(R_FRAG,255.f*tex_color[0],tex_color[3]);\
		res_color[1]=lerp(G_FRAG,255.f*tex_color[1],tex_color[3]);\
		res_color[2]=lerp(B_FRAG,255.f*tex_color[2],tex_color[3]);\
		res_color[3]=A_FRAG;\
		break;\
	case GL_BLEND:\
		res_color[0]=lerp(R_FRAG,glctx.tex_env_color[0],tex_color[0]);\
		res_color[1]=lerp(G_FRAG,glctx.tex_env_color[1],tex_color[1]);\
		res_color[2]=lerp(B_FRAG,glctx.tex_env_color[2],tex_color[2]);\
		res_color[3]=A_FRAG*tex_color[2];\
		break;\
	}

		_SHADE_FRAG_TEX

#undef R_FRAG
#undef G_FRAG
#undef B_FRAG
#undef A_FRAG
	}
#undef ___S
#undef ___T

#define ___R 0
#define ___G 1
#define ___B 2
#define ___A 3
#define ___S 4
#define ___T 5
	inline void init_color_tex(Vertex* v,GLfloat* att)
	{
		_INIT_COLOR
		_INIT_TEX
	}

	inline void shade_color_tex(GLfloat* attribs,GLfloat* ddx,GLfloat* ddy,cbuf_type* flat_color,TextureObject* texobj,cbuf_type* res_color)
	{
#define R_FRAG attribs[___R]
#define G_FRAG attribs[___G]
#define B_FRAG attribs[___B]
#define A_FRAG attribs[___A]

		_SHADE_FRAG_TEX

#undef R_FRAG
#undef G_FRAG
#undef B_FRAG
#undef A_FRAG
	}
#undef ___R
#undef ___G
#undef ___B
#undef ___A
#undef ___S
#undef ___T

#define ___F 0
	inline void init_fog(Vertex* v,GLfloat* att)
	{
#define _INIT_FOG \
	GLfloat _tmp;\
	att[___F]=1.f;\
	switch(glctx.fog_mode)\
	{\
	case GL_EXP:\
		att[___F]=exp(-glctx.fog_density*v->fog_coord);\
		break;\
	case GL_EXP2:\
		_tmp=glctx.fog_density*v->fog_coord;\
		att[___F]=exp(-_tmp*_tmp);\
		break;\
	case GL_LINEAR:\
		att[___F]=(glctx.fog_end-v->fog_coord)*glctx.cached_fog_e_s;\
		break;\
	}\
	att[___F]=YGL_CLAMP_MIN_MAX(att[___F],0.f,1.f);

		_INIT_FOG
	}
	void shade_flat_fog(GLfloat* attribs,GLfloat* ddx,GLfloat* ddy,cbuf_type* flat_color,TextureObject* texobj,cbuf_type* res_color)
	{
		res_color[0]=lerp(glctx.fog_color[0],flat_color[0],attribs[___F]);
		res_color[1]=lerp(glctx.fog_color[1],flat_color[1],attribs[___F]);
		res_color[2]=lerp(glctx.fog_color[2],flat_color[2],attribs[___F]);
		res_color[3]=lerp(glctx.fog_color[3],flat_color[3],attribs[___F]);
	}
#undef ___F

#define ___R 0
#define ___G 1
#define ___B 2
#define ___A 3
#define ___F 4
	inline void init_color_fog(Vertex* v,GLfloat* att)
	{
		_INIT_COLOR
		_INIT_FOG
	}
	void shade_color_fog(GLfloat* attribs,GLfloat* ddx,GLfloat* ddy,cbuf_type* flat_color,TextureObject* texobj,cbuf_type* res_color)
	{
		res_color[0]=lerp(glctx.fog_color[0],attribs[___R],attribs[___F]);
		res_color[1]=lerp(glctx.fog_color[1],attribs[___G],attribs[___F]);
		res_color[2]=lerp(glctx.fog_color[2],attribs[___B],attribs[___F]);
		res_color[3]=lerp(glctx.fog_color[3],attribs[___A],attribs[___F]);
	}
#undef ___R
#undef ___G
#undef ___B
#undef ___A
#undef ___F

#define ___S 0
#define ___T 1
#define ___F 2
	inline void init_tex_fog(Vertex* v,GLfloat* att)
	{
		_INIT_TEX
		_INIT_FOG
	}
	void shade_tex_fog(GLfloat* attribs,GLfloat* ddx,GLfloat* ddy,cbuf_type* flat_color,TextureObject* texobj,cbuf_type* res_color)
	{
		TEX_COMPUTE_RHO;
		cbuf_type tex_color[4];
		texobj->fetch255(rho,attribs[___S],attribs[___T],tex_color);

		res_color[0]=lerp(glctx.fog_color[0],tex_color[0],attribs[___F]);
		res_color[1]=lerp(glctx.fog_color[1],tex_color[1],attribs[___F]);
		res_color[2]=lerp(glctx.fog_color[2],tex_color[2],attribs[___F]);
		res_color[3]=lerp(glctx.fog_color[3],tex_color[3],attribs[___F]);
	}
	void shade_flat_tex_fog(GLfloat* attribs,GLfloat* ddx,GLfloat* ddy,cbuf_type* flat_color,TextureObject* texobj,cbuf_type* res_color)
	{
#define R_FRAG flat_color[0]
#define G_FRAG flat_color[1]
#define B_FRAG flat_color[2]
#define A_FRAG flat_color[3]

		_SHADE_FRAG_TEX

			res_color[0]=lerp(glctx.fog_color[0],res_color[0],attribs[___F]);
		res_color[1]=lerp(glctx.fog_color[1],res_color[1],attribs[___F]);
		res_color[2]=lerp(glctx.fog_color[2],res_color[2],attribs[___F]);
		res_color[3]=lerp(glctx.fog_color[3],res_color[3],attribs[___F]);

#undef R_FRAG
#undef G_FRAG
#undef B_FRAG
#undef A_FRAG
	}
#undef ___S
#undef ___T
#undef ___F

#define ___R 0
#define ___G 1
#define ___B 2
#define ___A 3
#define ___S 4
#define ___T 5
#define ___F 6
	inline void init_color_tex_fog(Vertex* v,GLfloat* att)
	{
		_INIT_COLOR
		_INIT_TEX
		_INIT_FOG
	}
	void shade_color_tex_fog(GLfloat* attribs,GLfloat* ddx,GLfloat* ddy,cbuf_type* flat_color,TextureObject* texobj,cbuf_type* res_color)
	{
#define R_FRAG attribs[___R]
#define G_FRAG attribs[___G]
#define B_FRAG attribs[___B]
#define A_FRAG attribs[___A]

		_SHADE_FRAG_TEX

			res_color[0]=lerp(glctx.fog_color[0],res_color[0],attribs[___F]);
		res_color[1]=lerp(glctx.fog_color[1],res_color[1],attribs[___F]);
		res_color[2]=lerp(glctx.fog_color[2],res_color[2],attribs[___F]);
		res_color[3]=lerp(glctx.fog_color[3],res_color[3],attribs[___F]);

#undef R_FRAG
#undef G_FRAG
#undef B_FRAG
#undef A_FRAG
	}
#undef ___R
#undef ___G
#undef ___B
#undef ___A
#undef ___S
#undef ___T
#undef ___F
}


#ifdef DO_TRIANGULATION

	// a/b = floor(a/b) + (a%b)/b
	// return floor=floor(a/b), mod=a%b>0=0
	inline void floor_mod(long a,long b,long& floor,long& mod)
	{
		// b>0
		if(a>=0){floor=a/b;mod=a%b;}
		else
		{
			floor=-((-a)/b);
			mod=(-a)%b; // >=0
			if(mod)
			{
				--floor;
				mod=b-mod;
			}
		}
	}


	inline fixed_real toFixed(GLfloat v){return v*FIXED_F;}
	inline fixed_real toFixed(long v){return v*FIXED_F;}
	inline GLfloat toFloat(fixed_real v){return v/FIXED_FF;}
	inline fixed_real mulFixed(fixed_real a,fixed_real b){return a*b/FIXED_F;}
	// ceil fixed to integer
	inline long ceilFixed(fixed_real f)
	{
		Assert(f>=0);
		return (f>>FIXED_BITS)+((f&FIXED_MASK)!=0);
	}
	// floor fixed to integer
	inline long floorFixed(fixed_real f)
	{
		Assert(f>=0);
		return (f>>FIXED_BITS);
	}

// #ifdef FILL_TOP_LEFT
// #define SNAP_YMIN floorFixed
// #define SNAP_YMAX floorFixed
// #else
// #define SNAP_YMIN ceilFixed
// #define SNAP_YMAX floorFixed
// #endif
//#define SNAP_X ceilFixed

#define WINDOW_COORD_TO_FIXED(v) (v).p_fixed[0]=toFixed((v).p[0]);\
	(v).p_fixed[1]=toFixed((v).p[1]);

	// this is the class for right edge
	struct TriEdge
	{
		long x,xstep,error_denom,error,error_step;
		
#ifdef USE_MEM_FACTORY
		virtual void recycle_me()
		{
			//Factory_ETRecord.recycle(this);
		}
#ifdef _YGL_DEBUG_
		void class_name(){printf("ETRecord:\t");}
#endif
#endif

		inline void dda_left(Vertex* v1,Vertex* v2,fixed_real fixed_snapped_y)
		{
#define m1 X_OF(v1->p_fixed)
#define m2 X_OF(v2->p_fixed)
#define n1 Y_OF(v1->p_fixed)
#define n2 Y_OF(v2->p_fixed)
			long Dm=m2-m1,Dn=n2-n1;
			//fixed_real fixed_snapped_y=toFixed(SNAP_Y(n1));
// #ifdef FILL_TOP_LEFT
// 			long R=Dm*(fixed_snapped_y-n1)+Dn*m1;
// #else
			if(Dn==0)
			{
				error_denom=1;
				xstep=error=error_step=0;
				//x=ceilFixed(MIN(v1->p_fixed[0],v2->p_fixed[0]));
				x=ceilFixed(m1); // already sorted, X1<X2
			}
			else
			{
				long R=Dm*(fixed_snapped_y-n1)+Dn*m1
					-1+Dn*FIXED_F;

				error_denom=Dn*FIXED_F;
				floor_mod(R,error_denom,x,error);
				floor_mod(Dm*FIXED_F,error_denom,xstep,error_step); // same denom
			}
			
			
#undef m1
#undef m2
#undef n1
#undef n2
		}
		
		virtual void build(Vertex* v0,Vertex* v1,  Vertex* v2,fixed_real fixed_snapped_y)
		{
			if(Y_OF(v0->p_fixed)==Y_OF(v1->p_fixed))
			{
				error_denom=1;
				xstep=error=error_step=0;
				x=ceilFixed(max(v0->p_fixed[0],v1->p_fixed[0]))-1;
			}
			else
			{
				dda_left(v0,v1,fixed_snapped_y);
				--x;
			}
		}
		virtual void fill_span(GLint y,TriEdge* that,cbuf_type* flat_color,TextureObject* texobj)
		{
		}
		virtual void step()
		{
			x+=xstep;
			if((error+=error_step)>=error_denom)
			{
				++x;
				error-=error_denom;
			}
		}
	};

	struct TriEdge_Left:public TriEdge
	{
		GLdouble z,zstep/*,zstep_extra*/; // dz/d(edge)
		GLfloat dz_dy,dz_dx;

#ifdef USE_MEM_FACTORY
		virtual void recycle_me()
		{
			//Factory_ETRecord.recycle(this);
		}
#ifdef _YGL_DEBUG_
		void class_name(){printf("ETRecord:\t");}
#endif
#endif

		//			fixed_real fixed_snapped_y=toFixed(SNAP_Y(Y_OF(v0->p_fixed)));\

#define DcDx_Numer(C0,C1,C2) ( ((C1)-(C2))*Y0_Y2 - ((C0)-(C2))*Y1_Y2 )
#define DcDy_Numer(C0,C1,C2) ( ((C1)-(C2))*X0_X2 - ((C0)-(C2))*X1_X2 )
#define INIT_Z \
			GLfloat dx_denom,dy_denom;\
			GLfloat Y0_Y2,Y1_Y2,X0_X2,X1_X2;\
			GLfloat y_prestep=0.f,x_prestep;\
			if(Y0==Y1)\
			{\
				int min_i,max_i;\
				dz_dy=0.f;\
				dx_denom=1.f/toFloat(X2-X0);\
				dz_dx=dx_denom*(DECODE_Z(Z_OF(v2->p))-DECODE_Z(Z_OF(v0->p)));\
			}\
			else\
			{\
				dx_denom=1.f/toFloat(mulFixed(X1-X2,Y0-Y2)-mulFixed(X0-X2,Y1-Y2));\
				dy_denom=-dx_denom;\
				Y0_Y2=toFloat(Y0-Y2);Y1_Y2=toFloat(Y1-Y2);\
				X0_X2=toFloat(X0-X2);X1_X2=toFloat(X1-X2);\
				dz_dx=dx_denom*DcDx_Numer(DECODE_Z(Z_OF(v0->p)),DECODE_Z(Z_OF(v1->p)),DECODE_Z(Z_OF(v2->p)));\
				dz_dy=dy_denom*DcDy_Numer(DECODE_Z(Z_OF(v0->p)),DECODE_Z(Z_OF(v1->p)),DECODE_Z(Z_OF(v2->p)));\
				y_prestep=toFloat(fixed_snapped_y-Y_OF(v0->p_fixed));\
			}\
			x_prestep=toFloat(toFixed(x)-X_OF(v0->p_fixed));\
			z=DECODE_Z(Z_OF(v0->p))\
				+y_prestep*dz_dy\
				+x_prestep*dz_dx;\
			zstep=xstep*dz_dx+dz_dy;
		
		// v0->v1 is the edge, v2 is the other vertex
		virtual void build(Vertex* v0,Vertex* v1,   Vertex* v2,fixed_real fixed_snapped_y)
		{
			dda_left(v0,v1,fixed_snapped_y);
#define X0 X_OF(v0->p_fixed)
#define X1 X_OF(v1->p_fixed)
#define X2 X_OF(v2->p_fixed)
#define Y0 Y_OF(v0->p_fixed)
#define Y1 Y_OF(v1->p_fixed)
#define Y2 Y_OF(v2->p_fixed)
			
			INIT_Z

#undef X0
#undef X1
#undef X2
#undef Y0
#undef Y1
#undef Y2
		}
		// GLubyte* cbuf,GLuint* zbuf,
		virtual void fill_span(GLint y,TriEdge* that,cbuf_type* flat_color,TextureObject* texobj)
		{
			GLdouble spanz=this->z;

			// fill span
			for(long px=this->x;px<=that->x;++px)
			{
				buffer::plot(px,y,(zbuf_type)spanz,flat_color);

				spanz+=dz_dx;
			}
		}
		virtual void step()
		{
			x+=xstep;
			z+=zstep;

			if((error+=error_step)>=error_denom)
			{
				++x;
				error-=error_denom;
				z+=dz_dx;
			}
		}
	};

	
	template <size_t npAttribNum, FragmentInitFunc INIT_FUNC, FragmentShadeFunc SHADE_FUNC>
	struct TriEdge_NoPersp:public TriEdge_Left
	{
		GLfloat att[npAttribNum], att_step[npAttribNum];
		GLfloat datt_dy[npAttribNum],datt_dx[npAttribNum];

		void build(Vertex* v0,Vertex* v1,   Vertex* v2,fixed_real fixed_snapped_y)
		{
			dda_left(v0,v1,fixed_snapped_y);
#define X0 X_OF(v0->p_fixed)
#define X1 X_OF(v1->p_fixed)
#define X2 X_OF(v2->p_fixed)
#define Y0 Y_OF(v0->p_fixed)
#define Y1 Y_OF(v1->p_fixed)
#define Y2 Y_OF(v2->p_fixed)

			INIT_Z;

			GLfloat att1[npAttribNum],att2[npAttribNum];
			INIT_FUNC(v0,att);
			INIT_FUNC(v1,att1);
			INIT_FUNC(v2,att2);

			if(Y0==Y1)
			{
				// todo: interpolation btwn v0,2 might be wrong
				for(int i=0;i<npAttribNum;++i)
				{
					// calculate two or three times??
					datt_dx[i]=dx_denom*(att2[i]-att[i]);
					datt_dy[i]=0.f;

					att[i]+=x_prestep*datt_dx[i];
					att_step[i]=xstep*datt_dx[i];
				}
			}
			else
			{
				for(int i=0;i<npAttribNum;++i)
				{
					// calculate two or three times??
					datt_dx[i]=dx_denom*DcDx_Numer(att[i],att1[i],att2[i]);
					datt_dy[i]=dy_denom*DcDy_Numer(att[i],att1[i],att2[i]);

					att[i]+=x_prestep*datt_dx[i]+y_prestep*datt_dy[i];
					att_step[i]=xstep*datt_dx[i]+datt_dy[i];
				}
			}

			
#undef X0
#undef X1
#undef X2
#undef Y0
#undef Y1
#undef Y2
		}
		void fill_span(GLint y,TriEdge* that,cbuf_type* flat_color,TextureObject* texobj)
		{
			GLdouble spanz=this->z;
			GLfloat span_att[npAttribNum];
			cbuf_type fragment_color[4];

			for(int i=0;i<npAttribNum;++i)
				span_att[i]=att[i];

			// fill span
			for(long px=this->x;px<=that->x;++px)
			{
				SHADE_FUNC(span_att,datt_dx,datt_dy,flat_color,texobj,fragment_color);
				buffer::plot(px,y,(zbuf_type)spanz,fragment_color);

				spanz+=dz_dx;
				for(int i=0;i<npAttribNum;++i)
					span_att[i]+=datt_dx[i];
			}
		}
		void step()
		{
			x+=xstep;
			z+=zstep;
			for(int i=0;i<npAttribNum;++i)
				att[i]+=att_step[i];

			if((error+=error_step)>=error_denom)
			{
				++x;
				error-=error_denom;
				z+=dz_dx;

				for(int i=0;i<npAttribNum;++i)
					att[i]+=datt_dx[i];
			}
		}
	};

	template <size_t pAttribNum, bool CALC_DXDY, FragmentInitFunc INIT_FUNC, FragmentShadeFunc SHADE_FUNC>
	struct TriEdge_Persp:public TriEdge_Left
	{
		GLfloat _w,d_w_dx,d_w_dy,_w_step;
		GLfloat att_w[pAttribNum], att_w_step[pAttribNum];
		GLfloat datt_w_dy[pAttribNum],datt_w_dx[pAttribNum];

		void build(Vertex* v0,Vertex* v1,   Vertex* v2,fixed_real fixed_snapped_y)
		{
			dda_left(v0,v1,fixed_snapped_y);
#define X0 X_OF(v0->p_fixed)
#define X1 X_OF(v1->p_fixed)
#define X2 X_OF(v2->p_fixed)
#define Y0 Y_OF(v0->p_fixed)
#define Y1 Y_OF(v1->p_fixed)
#define Y2 Y_OF(v2->p_fixed)

			INIT_Z;

			

			GLfloat att1_w[pAttribNum],att2_w[pAttribNum];
			INIT_FUNC(v0,att_w);
			INIT_FUNC(v1,att1_w);
			INIT_FUNC(v2,att2_w);

			if(Y0==Y1)
			{
				// already reciprocal
				d_w_dx=dx_denom*(W_OF(v2->p)-W_OF(v0->p));
				_w=W_OF(v0->p)+x_prestep*d_w_dx;
				_w_step=xstep*d_w_dx;

				for(int i=0;i<pAttribNum;++i)
				{
					// w without prestep
					att_w[i]*=W_OF(v0->p);
					//att1_w[i]*=W_OF(v1->p);
					att2_w[i]*=W_OF(v2->p);

					// calculate two or three times??
					datt_w_dx[i]=dx_denom*(att2_w[i]-att_w[i]);
					datt_w_dy[i]=0.f;

					att_w[i]+=x_prestep*datt_w_dx[i];
					att_w_step[i]=xstep*datt_w_dx[i];
				}
			}
			else
			{
				// already reciprocal
				d_w_dx=dx_denom*DcDx_Numer(W_OF(v0->p),W_OF(v1->p),W_OF(v2->p));
				d_w_dy=dy_denom*DcDy_Numer(W_OF(v0->p),W_OF(v1->p),W_OF(v2->p));
				_w=W_OF(v0->p)+x_prestep*d_w_dx+y_prestep*d_w_dy;
				_w_step=xstep*d_w_dx+d_w_dy;

				for(int i=0;i<pAttribNum;++i)
				{
					// w without prestep
					att_w[i]*=W_OF(v0->p);
					att1_w[i]*=W_OF(v1->p);
					att2_w[i]*=W_OF(v2->p);

					// calculate two or three times??
					datt_w_dx[i]=dx_denom*DcDx_Numer(att_w[i],att1_w[i],att2_w[i]);
					datt_w_dy[i]=dy_denom*DcDy_Numer(att_w[i],att1_w[i],att2_w[i]);

					att_w[i]+=x_prestep*datt_w_dx[i]+y_prestep*datt_w_dy[i];
					att_w_step[i]=xstep*datt_w_dx[i]+datt_w_dy[i];
				}
			}
			
#undef X0
#undef X1
#undef X2
#undef Y0
#undef Y1
#undef Y2
		}
		void fill_span(GLint y,TriEdge* that,cbuf_type* flat_color,TextureObject* texobj)
		{
			GLdouble spanz=this->z;
			GLfloat span_w=this->_w,w;
			GLfloat span_att_w[pAttribNum],span_att[pAttribNum];
			cbuf_type fragment_color[4];

			for(int i=0;i<pAttribNum;++i)
				span_att_w[i]=att_w[i];

			GLfloat ddx[pAttribNum],ddy[pAttribNum];

			// fill span
			for(long px=this->x;px<=that->x;++px)
			{
				w=1.f/span_w;
				for(int i=0;i<pAttribNum;++i)
				{
					span_att[i]=span_att_w[i]*w;
				}
				
				if(CALC_DXDY)
				{
					for(int i=0;i<pAttribNum;++i)
					{
						ddx[i]=datt_w_dx[i]*w;
						ddy[i]=datt_w_dy[i]*w;
					}
				}

				SHADE_FUNC(span_att,ddx,ddy,flat_color,texobj,fragment_color);
				buffer::plot(px,y,(zbuf_type)spanz,fragment_color);

				spanz+=dz_dx;
				span_w+=d_w_dx;
				for(int i=0;i<pAttribNum;++i)
					span_att_w[i]+=datt_w_dx[i];
			}
		}
		void step()
		{
			x+=xstep;
			z+=zstep;
			_w+=_w_step;
			for(int i=0;i<pAttribNum;++i)
				att_w[i]+=att_w_step[i];

			if((error+=error_step)>=error_denom)
			{
				++x;
				error-=error_denom;
				z+=dz_dx;
				_w+=d_w_dx;

				for(int i=0;i<pAttribNum;++i)
					att_w[i]+=datt_w_dx[i];
			}
		}
	};

	typedef TriEdge_Left FragShaderFlat;
	typedef FragShaderFlat FragShader;

	typedef TriEdge_NoPersp<4,fragment_shader::init_color,fragment_shader::shade_color> FragShader_Color;

	typedef TriEdge_NoPersp<2,fragment_shader::init_tex,fragment_shader::shade_tex> FragShader_Tex;
	typedef TriEdge_NoPersp<2,fragment_shader::init_tex,fragment_shader::shade_flat_tex> FragShader_Flat_Tex;
	typedef TriEdge_NoPersp<6,fragment_shader::init_color_tex,fragment_shader::shade_color_tex> FragShader_Color_Tex;

	typedef TriEdge_NoPersp<1,fragment_shader::init_fog,fragment_shader::shade_flat_fog> FragShader_Flat_Fog;
	typedef TriEdge_NoPersp<5,fragment_shader::init_color_fog,fragment_shader::shade_color_fog> FragShader_Color_Fog;
	typedef TriEdge_NoPersp<3,fragment_shader::init_tex_fog,fragment_shader::shade_tex_fog> FragShader_Tex_Fog;
	typedef TriEdge_NoPersp<3,fragment_shader::init_tex_fog,fragment_shader::shade_flat_tex_fog> FragShader_Flat_Tex_Fog;
	typedef TriEdge_NoPersp<7,fragment_shader::init_color_tex_fog,fragment_shader::shade_color_tex_fog> FragShader_Color_Tex_Fog;

	typedef TriEdge_Persp<4,false,fragment_shader::init_color,fragment_shader::shade_color> FragShader_P_Color;

	typedef TriEdge_Persp<2,true,fragment_shader::init_tex,fragment_shader::shade_tex> FragShader_P_Tex;
	typedef TriEdge_Persp<2,true,fragment_shader::init_tex,fragment_shader::shade_flat_tex> FragShader_P_Flat_Tex;
	typedef TriEdge_Persp<6,true,fragment_shader::init_color_tex,fragment_shader::shade_color_tex> FragShader_P_Color_Tex;

	typedef TriEdge_Persp<1,false,fragment_shader::init_fog,fragment_shader::shade_flat_fog> FragShader_P_Flat_Fog;
	typedef TriEdge_Persp<5,false,fragment_shader::init_color_fog,fragment_shader::shade_color_fog> FragShader_P_Color_Fog;
	typedef TriEdge_Persp<3,true,fragment_shader::init_tex_fog,fragment_shader::shade_tex_fog> FragShader_P_Tex_Fog;
	typedef TriEdge_Persp<3,true,fragment_shader::init_tex_fog,fragment_shader::shade_flat_tex_fog> FragShader_P_Flat_Tex_Fog;
	typedef TriEdge_Persp<7,true,fragment_shader::init_color_tex_fog,fragment_shader::shade_color_tex_fog> FragShader_P_Color_Tex_Fog;

	inline FragShader* new_frag_shader(GLenum option)
	{
		FragShader* ret=0;
		if(option&FragPersp)
		{
			option&=(~FragPersp);
			switch(option)
			{
			case FragSmooth:
				ret=NEW_P_COLOR;
				break;
			case FragTex:
				ret=NEW_P_TEX;
				break;
			case FragFog:
				ret=NEW_P_FLAT_FOG;
				break;

			case (FragFlat|FragTex):
				ret=NEW_P_FLAT_TEX;
				break;
			case (FragSmooth|FragTex):
				ret=NEW_P_COLOR_TEX;
				break;
			case (FragFlat|FragFog):
				ret=NEW_P_FLAT_FOG;
				break;
			case (FragSmooth|FragFog):
				ret=NEW_P_COLOR_FOG;
				break;
			case (FragFog|FragTex):
				ret=NEW_P_TEX_FOG;
				break;

			case (FragFog|FragFlat|FragTex):
				ret=NEW_P_FLAT_TEX_FOG;
				break;
			case (FragFog|FragSmooth|FragTex):
				ret=NEW_P_COLOR_TEX_FOG;
				break;

				//case FragFlat:
			default:
				ret=NEW_FLAT;
				break;
			}
		}
		else
		{
			switch(option)
			{
			case FragSmooth:
				ret=NEW_COLOR;
				break;
			case FragTex:
				ret=NEW_TEX;
				break;
			case FragFog:
				ret=NEW_FLAT_FOG;
				break;

			case (FragFlat|FragTex):
				ret=NEW_FLAT_TEX;
				break;
			case (FragSmooth|FragTex):
				ret=NEW_COLOR_TEX;
				break;
			case (FragFlat|FragFog):
				ret=NEW_FLAT_FOG;
				break;
			case (FragSmooth|FragFog):
				ret=NEW_COLOR_FOG;
				break;
			case (FragFog|FragTex):
				ret=NEW_TEX_FOG;
				break;

			case (FragFog|FragFlat|FragTex):
				ret=NEW_FLAT_TEX_FOG;
				break;
			case (FragFog|FragSmooth|FragTex):
				ret=NEW_COLOR_TEX_FOG;
				break;

				//case FragFlat:
			default:
				ret=NEW_FLAT;
				break;
			}
		}
		
		return ret;
	}

	void scanline(Vertex* v,int vcnt,GLenum option,GLubyte* flat_color,TextureObject* texobj)
	{
		Assert(vcnt>=3);
		if(!texobj)
			option&=~FragTex;
		if((option&FragTex)&&glctx.tex_env_mode==GL_REPLACE)
			option&=~(FragSmooth|FragFlat);
		if(!option) return;

		Vertex *v1,*v2,*v3;

		WINDOW_COORD_TO_FIXED(v[0]);
		WINDOW_COORD_TO_FIXED(v[1]);

		// Fan triangulation
		GLint tri_cnt=vcnt-2;
		GLboolean edge_flag_last=v[vcnt-1].edge_flag; // first edge flag is destroyed
		for(GLint tri_i=1;tri_i<=tri_cnt;++tri_i)
		{
#define I1 (0)
#define I2 (tri_i)
#define I3 (tri_i+1)
#define Y1 Y_OF(v[I1].p)
#define Y2 Y_OF(v[I2].p)
#define Y3 Y_OF(v[I3].p)

			WINDOW_COORD_TO_FIXED(v[I3]);

			if(I2!=1) v[I1].edge_flag=false;
			v[I3].edge_flag=((I3!=vcnt-1)?false:edge_flag_last);

			// set edge_flag to the vertex that starts v1->v3 or v3->v1
			// v1->y <= v2->y <= v3->y
			if(Y1<=Y2)
			{
				// 1 -> 2 -> 3
				if(Y2<=Y3)
				{
					v1=v+I1;
					v2=v+I2;
					v3=v+I3;
				}
				// 1 -> 3 -> 2
				else if(Y1<=Y3)
				{
					v1=v+I1;
					v2=v+I3;
					v3=v+I2;
				}
				// 3 -> 1 -> 2
				else
				{
					v1=v+I3;
					v2=v+I1;
					v3=v+I2;
				}
			}
			else
			{
				// 3 -> 2 -> 1
				if(Y3<=Y2)
				{
					v1=v+I3;
					v2=v+I2;
					v3=v+I1;
				}
				// 2 -> 1 -> 3
				else if(Y1<=Y3)
				{
					v1=v+I2;
					v2=v+I1;
					v3=v+I3;
				}
				// 2 -> 3 -> 1
				else
				{
					v1=v+I2;
					v2=v+I3;
					v3=v+I1;
				}
			}
#undef Y1
#undef Y2
#undef Y3
#undef I1
#undef I2
#undef I3

#define Y1 Y_OF(v1->p_fixed)
#define Y2 Y_OF(v2->p_fixed)
#define Y3 Y_OF(v3->p_fixed)
#define X1 X_OF(v1->p_fixed)
#define X2 X_OF(v2->p_fixed)
#define X3 X_OF(v3->p_fixed)

			//Gradients grad(v1,v2,v3);
			long ymin,ymid,ymax;
			fixed_real fixed_snapped_ymin;
			TriEdge rightedge,*righte=&rightedge;
			FragShader* lefte=new_frag_shader(option);

			// bottom-flat
			if(Y1==Y2)
			{
				if(Y2==Y3) // special case
				{
					/*
#ifdef FILL_TOP_LEFT
					ymin=ymax=floorFixed(Y1);
#else
					ymin=ymax=floorFixed(Y1);
#endif

					// todo: call point(), line()
					if(X1==X2&&X2==X3)
					{
						printf("skip\n");
						continue;
					}

					fixed_snapped_ymin=Y1; // no matter??
					if(X1<X2)
					{
						if(X2<X3) // 1<2<3
						{
							lefte->build(v1,v2,  v3,fixed_snapped_ymin);
							righte->build(v2,v3,  v1,fixed_snapped_ymin);
						}
						else if(X1<X3) // 1<3<2
						{
							lefte->build(v1,v3,  v2,fixed_snapped_ymin);
							righte->build(v3,v2,  v1,fixed_snapped_ymin);
						}
						else // 3<1<2
						{
							lefte->build(v3,v1,  v2,fixed_snapped_ymin);
							righte->build(v1,v2,  v3,fixed_snapped_ymin);
						}
					}
					else
					{
						if(X2>X3) // 3<2<1
						{
							lefte->build(v3,v2,  v1,fixed_snapped_ymin);
							righte->build(v2,v1,  v3,fixed_snapped_ymin);
						}
						else if(X1>X3) // 2<3<1
						{
							lefte->build(v2,v3,  v1,fixed_snapped_ymin);
							righte->build(v3,v1,  v2,fixed_snapped_ymin);
						}
						else // 2<1<3
						{
							lefte->build(v2,v1,  v3,fixed_snapped_ymin);
							righte->build(v1,v3,  v2,fixed_snapped_ymin);
						}
					}

					if(righte->x<lefte->x) righte->x=lefte->x;
					lefte->fill_span(ymin,righte,flat_color,texobj);
					*/
					continue;
					// both top and bottom, reject it, wrong!!!!
				}
				else
				{
					ymax=floorFixed(Y3);

#ifdef FILL_TOP_LEFT
					ymin=floorFixed(Y1)+1;
#else
					ymin=ceilFixed(Y1);
#endif
				}

				if(ymin>ymax) continue;

				fixed_snapped_ymin=toFixed(ymin);
				if(X1<X2)
				{
					lefte->build(v1,v3,  v2,fixed_snapped_ymin);
					righte->build(v2,v3,  0,fixed_snapped_ymin);
				}
				else
				{
					lefte->build(v2,v3,  v1,fixed_snapped_ymin);
					righte->build(v1,v3,  0,fixed_snapped_ymin);
				}

				// [ymin, ymax)
				for(long y=ymin;y<=ymax;++y)
				{
					lefte->fill_span(y,righte,flat_color,texobj);
					lefte->step();righte->step();
				}
			}
			// top-flat
			else if(Y2==Y3)
			{
				ymin=ceilFixed(Y1);
				
#ifdef FILL_TOP_LEFT
				ymax=floorFixed(Y3);
#else
				ymax=ceilFixed(Y3)-1;
#endif
				if(ymin>ymax) continue;

				fixed_snapped_ymin=toFixed(ymin);
				if(X2<X3)
				{
					lefte->build(v1,v2,  v3,fixed_snapped_ymin);
					righte->build(v1,v3,  0,fixed_snapped_ymin);
				}
				else
				{
					lefte->build(v1,v3,  v2,fixed_snapped_ymin);
					righte->build(v1,v2,  0,fixed_snapped_ymin);
				}

				for(long y=ymin;y<=ymax;++y)
				{
					lefte->fill_span(y,righte,flat_color,texobj);
					lefte->step();righte->step();
				}
			}
			else
			{
				GLfloat _slope_b2m=(X2-X1)/(GLfloat)(Y2-Y1),_slope_b2t=(X3-X1)/(GLfloat)(Y3-Y1);
				ymin=ceilFixed(Y1);
				ymax=floorFixed(Y3);

				fixed_snapped_ymin=toFixed(ymin);

#ifdef FILL_TOP_LEFT
				ymid=floorFixed(Y2);
#else
				ymid=ceilFixed(Y2)-1;
#endif
				
				// bot2mid, mid2top as left edge
				if(_slope_b2m<_slope_b2t)
				{
					// bot2mid
					lefte->build(v1,v2,  v3,fixed_snapped_ymin);
					// bot2top
					righte->build(v1,v3,  0,fixed_snapped_ymin);

					for(long y=ymin;y<=ymid;++y)
					{
						lefte->fill_span(y,righte,flat_color,texobj);
						lefte->step();righte->step();
					}

					fixed_snapped_ymin=toFixed(ymid+1);
					// mid2top
					lefte->build(v2,v3,  v1,fixed_snapped_ymin);

					for(long y=ymid+1;y<=ymax;++y)
					{
						lefte->fill_span(y,righte,flat_color,texobj);
						lefte->step();righte->step();
					}
				}
				// bot2top as left edge
				else if(_slope_b2m>_slope_b2t)
				{
					// bot2top
					lefte->build(v1,v3,  v2,fixed_snapped_ymin);
					// bot2mid
					righte->build(v1,v2,  0,fixed_snapped_ymin);

					for(long y=ymin;y<=ymid;++y)
					{
						lefte->fill_span(y,righte,flat_color,texobj);
						lefte->step();righte->step();
					}

					fixed_snapped_ymin=toFixed(ymid+1);
					// mid2top
					righte->build(v2,v3,  0,fixed_snapped_ymin);

					for(long y=ymid+1;y<=ymax;++y)
					{
						lefte->fill_span(y,righte,flat_color,texobj);
						lefte->step();righte->step();
					}
				}
			}
			
			DELETE_SHADER(lefte);
#undef Y1
#undef Y2
#undef Y3
#undef X1
#undef X2
#undef X3
		}

	
	}
#else

	// todo: handle exceptions? single vert isect, horizontal line no add to etable
	struct ETRecord
	{
		// bresenham vertical
		//int D,delta1,delta2,inc_x;
		//GRow vertical or horizonal

		// edge info
		int ymin,ymax;
		GLfloat curx;
		GLfloat dx; // dx/dy
		GLdouble z,dz; // dz/dy


		ETRecord* prev,*next;

#ifdef USE_MEM_FACTORY
		ETRecord(){ctor();}
		void ctor(){prev=next=0;}
		virtual void recycle_me()
		{
			Factory_ETRecord.recycle(this);
		}
#ifdef _YGL_DEBUG_
		void class_name(){printf("ETRecord:\t");}
#endif
#else
		ETRecord():prev(0),next(0){}
#endif
		// return true: start from v1, false: start from v2
		virtual void build(Vertex* v1,Vertex* v2)
		{
			prev=next=0;
			// we start from ymin so ymin always < ymax (bresenham)

			if(Y_OF(v1->p)>Y_OF(v2->p))
			{
				//XOR_SWAP(v1,v2);
				Vertex* vt=v1;
				v1=v2;
				v2=vt;
			}

			//start from v1
			ymin=COMPUTE_YMIN(Y_OF(v1->p));
			ymax=COMPUTE_YMAX(Y_OF(v2->p));
			curx=/*ROUND(*/X_OF(v1->p);
			z=YGL_DEPTH_BUFFER_MAX_RES*Z_OF(v1->p);

			GLfloat _dy=1.f/((GLfloat)(ymax-ymin));
			dx=((GLfloat)(X_OF(v2->p)-curx))*_dy;
			dz=(YGL_DEPTH_BUFFER_MAX_RES*Z_OF(v2->p)-z)*_dy;

			//int dx=,dy=ymax-ymin;
		}

		virtual void fill_span(GLint y,ETRecord* that,/*GLubyte* cbuf,GLuint* zbuf,*/cbuf_type* flat_color,TextureObject* texobj)
		{
			int left=COMPUTE_XMIN(this->curx),right=COMPUTE_XMAX(that->curx);
			GLdouble spanz=this->z,d_spanz=(that->z-spanz)/((GLfloat)(right-left)); // dx
			
			// fill span
			for(int x=left;x<right;++x)
			{
				buffer::plot(x,y,(zbuf_type)spanz,flat_color);
				//if(d_span_z<0&&span_z<that->z) span_z=that->z;
// 				if(SIGN_OF_DOUBLE(spanz-*zbuf)&depthtest)
// 				{
// 					*cbuf++=color[0];
// 					*cbuf++=color[1];
// 					*cbuf++=color[2];
// 					*zbuf=spanz;
// 				}
// 				else
// 				{
// 					cbuf+=3;
// 				}
//				
//				++zbuf;
				spanz+=d_spanz;
			}
		}

		virtual void incre()
		{
			curx+=dx;
			z+=dz;
		}
	};
	
	template <size_t pAttribNum, FragmentInitFunc INIT_FUNC, FragmentShadeFunc SHADE_FUNC>
	struct ETRecord_Persp : public ETRecord
	{
#ifdef USE_MEM_FACTORY
		TFactory< ETRecord_Persp<pAttribNum,INIT_FUNC,SHADE_FUNC> > Factory;
		virtual void recycle_me()
		{
			Factory.recycle(this);
		}
#endif
		GLfloat att_w[pAttribNum],d_att_w_dy[pAttribNum],
			_w,d_w; // dy implicit

		//ETRecord_Persp():att_num(AttribNum){}

		/*virtual */void build(Vertex* v1,Vertex* v2)
		{
			prev=next=0;

			if(Y_OF(v1->p)>Y_OF(v2->p))
			{
				//XOR_SWAP(v1,v2);
				Vertex* vt=v1;
				v1=v2;
				v2=vt;
			}

			//start from v1
			GLfloat theother_w,_dy;

			// todo: 

			ymin=COMPUTE_YMIN(Y_OF(v1->p));
			ymax=COMPUTE_YMAX(Y_OF(v2->p));
			curx=/*ROUND(*/X_OF(v1->p); // -> todo: add offset
			z=YGL_DEPTH_BUFFER_MAX_RES*Z_OF(v1->p);

			_dy=1.f/((GLfloat)(ymax-ymin));
			dx=((GLfloat)(/*ROUND(*/X_OF(v2->p)-curx))*_dy;
			dz=(YGL_DEPTH_BUFFER_MAX_RES*Z_OF(v2->p)-z)*_dy;

			// already reciprocal
			_w=W_OF(v1->p);theother_w=W_OF(v2->p);
			d_w=(theother_w-_w)*_dy;

			INIT_FUNC(v1,att_w);
			INIT_FUNC(v2,d_att_w_dy);

			for(int i=0;i<pAttribNum;++i)
			{
				att_w[i]*=_w;
				d_att_w_dy[i]=_dy*(d_att_w_dy[i]*theother_w-att_w[i]);
			}
		}

		/*virtual */void incre()
		{
			curx+=dx;
			z+=dz;
			_w+=d_w;
			
			for(int i=0;i<pAttribNum;++i)
				att_w[i]+=d_att_w_dy[i];
		}

		/*virtual */void fill_span(GLint y,ETRecord* spanto,cbuf_type* flat_color,TextureObject* texobj)
		{
			ETRecord_Persp<pAttribNum,INIT_FUNC,SHADE_FUNC>* that=(ETRecord_Persp<pAttribNum,INIT_FUNC,SHADE_FUNC>*)spanto;
			int left=COMPUTE_XMIN(this->curx),right=COMPUTE_XMAX(that->curx);

			GLfloat _dx=1.f/((GLfloat)(right-left));
			GLfloat span_w=this->_w,d_w_dx=(that->_w-span_w)*_dx;
			GLfloat spanz=this->z,d_spanz=(that->z-spanz)*_dx;

			GLfloat span_att_w[pAttribNum],d_att_w_dx[pAttribNum],
				span_attribs[pAttribNum];
			
			for(int i=0;i<pAttribNum;++i)
			{
				span_att_w[i]=this->att_w[i];
				d_att_w_dx[i]=(that->att_w[i]-span_att_w[i])*_dx;
			}

			cbuf_type fragment_color[4];
			
// 			cbuf_type* cbuf=buffer::framebuf_ptr(left,y);
// 			zbuf_type* zbuf=buffer::depthbuf_ptr(left,y);

			GLfloat w;
			// fill span
			for(int x=left;x<right;++x)
			{
				w=1.f/span_w;
				for(int i=0;i<pAttribNum;++i)
				{
					span_attribs[i]=span_att_w[i]*w;
				}

				SHADE_FUNC(span_attribs,0,0,flat_color,texobj,fragment_color);
				buffer::plot(x,y,(zbuf_type)spanz,fragment_color);
				//buffer::plot(x,y,(zbuf_type)spanz,cbuf,zbuf,fragment_color);
				
				//++zbuf;cbuf+=3;
// 				*cbuf++=fragment_color[0];
// 				*cbuf++=fragment_color[1];
// 				*cbuf++=fragment_color[2];
// 				*cbuf++=fragment_color[3];

				for(int i=0;i<pAttribNum;++i)
				{
					span_att_w[i]+=d_att_w_dx[i];
				}
				span_w+=d_w_dx;
				spanz+=d_spanz;

				//++zbuf;cbuf+=4;
			}
		}
	};
	
	template <size_t npAttribNum, FragmentInitFunc INIT_FUNC, FragmentShadeFunc SHADE_FUNC>
	struct ETRecord_NoPersp : public ETRecord
	{
#ifdef USE_MEM_FACTORY
		TFactory< ETRecord_NoPersp<npAttribNum,INIT_FUNC,SHADE_FUNC> > Factory;
		virtual void recycle_me()
		{
			Factory.recycle(this);
		}
#endif

		//GLint att_num;
		GLfloat att[npAttribNum],d_att_dy[npAttribNum];

		//ETRecordPerspCorrection():att_num(AttribNum){}

		/*virtual */void build(Vertex* v1,Vertex* v2)
		{
			prev=next=0;

			if(Y_OF(v1->p)>Y_OF(v2->p))
			{
				//XOR_SWAP(v1,v2);
				Vertex* vt=v1;
				v1=v2;
				v2=vt;
			}

			//start from v1
			GLfloat /*theother_w,*/_dy;

			ymin=COMPUTE_YMIN(Y_OF(v1->p));
			ymax=COMPUTE_YMAX(Y_OF(v2->p));
			curx=/*ROUND(*/X_OF(v1->p);
			z=YGL_DEPTH_BUFFER_MAX_RES*Z_OF(v1->p);

			_dy=1.f/((GLfloat)(ymax-ymin));
			dx=((GLfloat)(X_OF(v2->p)-curx))*_dy;
			dz=(YGL_DEPTH_BUFFER_MAX_RES*Z_OF(v2->p)-z)*_dy;

			INIT_FUNC(v1,att);
			INIT_FUNC(v2,d_att_w_dy);

			for(int i=0;i<npAttribNum;++i)
				d_att_dy[i]=(d_att_dy[i]-att[i])*_dy;
		}

		/*virtual */void incre()
		{
			curx+=dx;
			z+=dz;
			// !!check att_num or AttribNum??
			for(int i=0;i<npAttribNum;++i)
				att[i]+=d_att_dy[i];
		}

		/*virtual */void fill_span(GLint y,ETRecord* spanto,cbuf_type* flat_color,TextureObject* texobj)
		{
			ETRecord_NoPersp<npAttribNum,INIT_FUNC,SHADE_FUNC>* that=(ETRecord_NoPersp<npAttribNum,INIT_FUNC,SHADE_FUNC>*)spanto;
			int left=COMPUTE_XMIN(this->curx),right=COMPUTE_XMAX(that->curx);

			GLfloat _dx=1.f/((GLfloat)(right-left));
			//GLfloat span_w=this->_w,d_w_dx=(that->_w-span_w)*_dx;
			GLfloat spanz=this->z,d_spanz=(that->z-spanz)*_dx;

			GLfloat span_att[npAttribNum],d_att_dx[npAttribNum];
			// !!check use att_num??
			for(int i=0;i<npAttribNum;++i)
			{
				span_att[i]=this->att[i];
				d_att_dx[i]=(that->att[i]-span_att[i])*_dx;
			}

			cbuf_type fragment_color[4];
			// fill span
			for(int x=left;x<right;++x)
			{
				SHADE_FUNC(span_att,flat_color,texobj,fragment_color);
				buffer::plot(x,y,(zbuf_type)spanz,fragment_color);

				//++zbuf;cbuf+=3;

				for(int i=0;i<npAttribNum;++i)
				{
					span_att[i]+=d_att_dx[i];
				}
				spanz+=d_spanz;
			}
		}
	};

	typedef ETRecord FragShaderFlat;
	typedef FragShaderFlat FragShader;
	
	typedef ETRecord_NoPersp<4,fragment_shader::init_color,fragment_shader::shade_color> FragShader_Color;

	typedef ETRecord_NoPersp<2,fragment_shader::init_tex,fragment_shader::shade_tex> FragShader_Tex;
	typedef ETRecord_NoPersp<2,fragment_shader::init_tex,fragment_shader::shade_flat_tex> FragShader_Flat_Tex;
	typedef ETRecord_NoPersp<6,fragment_shader::init_color_tex,fragment_shader::shade_color_tex> FragShader_Color_Tex;

	typedef ETRecord_NoPersp<1,fragment_shader::init_fog,fragment_shader::shade_flat_fog> FragShader_Flat_Fog;
	typedef ETRecord_NoPersp<5,fragment_shader::init_color_fog,fragment_shader::shade_color_fog> FragShader_Color_Fog;
	typedef ETRecord_NoPersp<3,fragment_shader::init_tex_fog,fragment_shader::shade_tex_fog> FragShader_Tex_Fog;
	typedef ETRecord_NoPersp<3,fragment_shader::init_tex_fog,fragment_shader::shade_flat_tex_fog> FragShader_Flat_Tex_Fog;
	typedef ETRecord_NoPersp<7,fragment_shader::init_color_tex_fog,fragment_shader::shade_color_tex_fog> FragShader_Color_Tex_Fog;

	// perspective correction
	typedef ETRecord_Persp<4,fragment_shader::init_color,fragment_shader::shade_color> FragShader_P_Color;

	typedef ETRecord_Persp<2,fragment_shader::init_tex,fragment_shader::shade_tex> FragShader_P_Tex;
	typedef ETRecord_Persp<2,fragment_shader::init_tex,fragment_shader::shade_flat_tex> FragShader_P_Flat_Tex;
	typedef ETRecord_Persp<6,fragment_shader::init_color_tex,fragment_shader::shade_color_tex> FragShader_P_Color_Tex;

	typedef ETRecord_Persp<1,fragment_shader::init_fog,fragment_shader::shade_flat_fog> FragShader_P_Flat_Fog;
	typedef ETRecord_Persp<5,fragment_shader::init_color_fog,fragment_shader::shade_color_fog> FragShader_P_Color_Fog;
	typedef ETRecord_Persp<3,fragment_shader::init_tex_fog,fragment_shader::shade_tex_fog> FragShader_P_Tex_Fog;
	typedef ETRecord_Persp<3,fragment_shader::init_tex_fog,fragment_shader::shade_flat_tex_fog> FragShader_P_Flat_Tex_Fog;
	typedef ETRecord_Persp<7,fragment_shader::init_color_tex_fog,fragment_shader::shade_color_tex_fog> FragShader_P_Color_Tex_Fog;

	inline FragShader* new_frag_shader(GLenum option)
	{
		FragShader* ret=0;
		switch(option)
		{
		case FragSmooth:
			ret=NEW_P_COLOR;
			break;
		case FragTex:
			ret=NEW_P_TEX;
			break;
		case FragFog:
			ret=NEW_P_FLAT_FOG;
			break;

		case (FragFlat|FragTex):
			ret=NEW_P_FLAT_TEX;
			break;
		case (FragSmooth|FragTex):
			ret=NEW_P_COLOR_TEX;
			break;
		case (FragFlat|FragFog):
			ret=NEW_P_FLAT_FOG;
			break;
		case (FragSmooth|FragFog):
			ret=NEW_P_COLOR_FOG;
			break;
		case (FragFog|FragTex):
			ret=NEW_P_TEX_FOG;
			break;

		case (FragFog|FragFlat|FragTex):
			ret=NEW_P_FLAT_TEX_FOG;
			break;
		case (FragFog|FragSmooth|FragTex):
			ret=NEW_P_COLOR_TEX_FOG;
			break;

			//case FragFlat:
		default:
			ret=NEW_FLAT;
			break;
		}
		return ret;
	}


	struct ETable
	{
		ETRecord* head,*tail;

#ifdef USE_MEM_FACTORY
		ETable(){ctor();}
		void ctor(){head=tail=0;}
#ifdef _YGL_DEBUG_
		void class_name(){printf("ETable:\t");}
#endif
#else
		ETable():head(0),tail(0){}
#endif
		void free()
		{
			ETRecord* pnext=head,*p;
			while(p=pnext)
			{
				pnext=p->next;
				DELETE_SHADER(p);
			}
		}

		ETRecord* merge(ETable* et)
		{
			Assert(et);
			if(!this->head)
			{
				this->head=et->head;
				this->tail=et->tail;
				DELETE_ETABLE(et);
				return this->head;
			}

			ETRecord* p=et->head;/*et.head=NULL;*/
			ETRecord* q=head,*pnext;
			while(p&&q)
			{
				// =??
				if(p->curx<=q->curx)
				{
					pnext=p->next;
					insertbefore(q,p);
					p=pnext;
				}
				else q=q->next;
			}
// 			while(p)
// 			{
// 				pnext=p->next;
// 				append(p);p=pnext;
// 			}
			if(p)
			{
				p->prev=this->tail;
				this->tail->next=p;
				this->tail=et->tail;
			}

			DELETE_ETABLE(et);

			return this->head;
		}

		void erase(ETRecord* e)
		{
			if(e==head)
			{
				if(head==tail)
					tail=head=0;
				else
					(head=e->next)->prev=0;
			}
			else if(e==tail) // >=2
			{
				(tail=e->prev)->next=0;
			}
			else
			{
				e->prev->next=e->next;
				e->next->prev=e->prev;
			}

			--size;
			DELETE_SHADER(e); // recycle		
		}
		void add(ETRecord* e)
		{
			if(!head)
			{
				(head=e)->prev=
					(tail=e)->next=0;
			}
			else
			{
				// todo: test cur_x==, m_recip==?
				ETRecord* p=head;
				while(p&&
					(p->curx<e->curx||
						(p->curx==e->curx&&p->dx<e->dx)
					))
					p=p->next;

				if(p)
				{
					insertbefore(p,e);
				}
				else // append to tail
				{
					e->prev=this->tail;
					e->next=0;
					this->tail->next=e;
					this->tail=e;
				}
			}
			++size;
		}
	private:
		inline void insertbefore(ETRecord* pos,ETRecord* e)
		{
			if(pos==head) head=e;
			else pos->prev->next=e;

			e->next=pos;
			e->prev=pos->prev;
			pos->prev=e;

			++size;
		}
// 		void append(ETRecord* e)
// 		{
// 			if(head==NULL)
// 			{
// 				head=tail=e;
// 				e->prev=e->next=NULL;
// 			}
// 			else
// 			{
// 				e->prev=tail;
// 				e->next=NULL;
// 				tail->next=e;
// 				tail=e;
// 			}
// 			++size;
// 		}

		int size;
	};

#ifdef USE_MEM_FACTORY
	TFactory<ETable> Factory_ETable(10);
	DECLARE_FACTORY(ETRecord);
	DECLARE_FACTORY(ETRecord_PColor);
	DECLARE_FACTORY(ETRecord_PColor_PTex);
#endif

	ETable* et_ptrs[YGL_MAX_BUFFER_HEIGHT];
	ETable aet;
#define GET_ET_PTR(y) (et_ptrs[y]?et_ptrs[y]:(et_ptrs[y]=(NEW_ETABLE)))

	

	// top-bottom fill??
	// edge by bresenham, other attribs increment
	void scanline(Vertex* v,int vcnt,GLenum option,GLubyte* flat_color,TextureObject* texobj)
	{
		if(!texobj)
			option&=~FragTex;
		if((option&FragTex)&&glctx.tex_env_mode==GL_REPLACE)
			option&=~(FragSmooth|FragFlat);
		if(!option) return;
		// clear?

		// ?? round, top left fill
		int ymin=COMPUTE_YMIN(Y_OF(v[vcnt-1].p)),
			ymax=COMPUTE_YMAX(Y_OF(v[vcnt-1].p));
		// build ETs
		for(int i=0;i<vcnt-1;++i)
		{
			if(ROUND(Y_OF(v[i].p))<ymin)
				ymin=COMPUTE_YMIN(Y_OF(v[i].p));
			else if(ROUND(Y_OF(v[i].p))>ymax)
				ymax=COMPUTE_YMAX(Y_OF(v[i].p));

			// ignore horizontal lines?
			ETRecord* newRec=0;
			if(ROUND(Y_OF(v[i].p))!=ROUND(Y_OF(v[i+1].p)))
			{
				newRec=new_frag_shader(option);
				newRec->build(v+i,v+(i+1));
				GET_ET_PTR(newRec->ymin)->add(newRec);
			}
		}

		if(ROUND(Y_OF(v[0].p))!=ROUND(Y_OF(v[vcnt-1].p)))
		{
			ETRecord*newRec=new_frag_shader(option);

			newRec->build(v+(vcnt-1),v);
			GET_ET_PTR(newRec->ymin)->add(newRec);
		}

		aet.head=0;

		// scan
		ETRecord* lefte,*righte,*nexte;
		// todo: dont test on et_ptrs[y] for every scanline? (linklist)
		for(int y=ymin;y</*=*/ymax;++y)
		{
			if(et_ptrs[y])
			{
				aet.merge(et_ptrs[y]);
				et_ptrs[y]=0; // already freed in merge
			}

			if(lefte=aet.head)
			{
				while(lefte&&(righte=lefte->next))
				{
					// !!!!!check
					//cbuf=buffer::framebuf_ptr(ROUND(lefte->cur_x),y);

// 					lefte->fill_span(righte,buffer::framebuf_ptr(ROUND(lefte->curx),y),
// 						buffer::depthbuf_ptr(ROUND(lefte->curx),y),
// 						flat_color,texobj);
					lefte->fill_span(y,righte,flat_color,texobj);

					nexte=righte->next;

					// todo: expand the incre code into fill_span? will waste time if incre then erase, but only happens one time??

					// raster until top line
					// incre and update
					if(y+1>=lefte->ymax)
						aet.erase(lefte);
					else lefte->incre();

					if(y+1>=righte->ymax)
						aet.erase(righte);
					else righte->incre();

					lefte=nexte;
				}
			}
// 			if(et_ptrs[y])
// 			{
// 				// dont raster bottom line, incre to next
// 				ETRecord* rec=et_ptrs[y]->head;
// 				while(rec)
// 				{
// 					rec->incre();rec=rec->next;
// 				}
// 				aet.merge(et_ptrs[y]);
// 				et_ptrs[y]=0; // already freed in merge
// 			}
		}

		// cleanup
		aet.free();
	}

	


#endif

}
}