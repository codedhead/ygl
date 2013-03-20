#include "clip.h"
#include "glcontext.h"
#include "global.h"

#define LERP_COMPONENT_4(dst,comp,va,vb,f) \
	(dst)->##comp##[0]=lerp((va)->##comp##[0],(vb)->##comp##[0],f);\
	(dst)->##comp##[1]=lerp((va)->##comp##[1],(vb)->##comp##[1],f);\
	(dst)->##comp##[2]=lerp((va)->##comp##[2],(vb)->##comp##[2],f);\
	(dst)->##comp##[3]=lerp((va)->##comp##[3],(vb)->##comp##[3],f);

//#define IMAGE_PLANE_LERP_TO_WORLD_LERP(u,z1,z2) ((u)*(z1)/((z2)+(u)*((z1)-(z2))))
#define IMAGE_PLANE_LERP_TO_WORLD_LERP(u,z1,z2) (u)

namespace clip
{
	// liang-barsky blinn
	GLboolean line(const Vertex* a,const Vertex* b,Vertex* clip_buf)
	{
		GLfloat/*double*/ u1=0.0,u2=1.0,r;
		GLfloat/*double*/ aBC[6],bBC[6];

		//  1,-1
		//x
		//y
		//z
		aBC[0]=W_OF(a->p)-X_OF(a->p);aBC[1]=W_OF(a->p)+X_OF(a->p);
		aBC[2]=W_OF(a->p)-Y_OF(a->p);aBC[3]=W_OF(a->p)+Y_OF(a->p);
		aBC[4]=W_OF(a->p)-Z_OF(a->p);aBC[5]=W_OF(a->p)+Z_OF(a->p);

		bBC[0]=W_OF(b->p)-X_OF(b->p);bBC[1]=W_OF(b->p)+X_OF(b->p);
		bBC[2]=W_OF(b->p)-Y_OF(b->p);bBC[3]=W_OF(b->p)+Y_OF(b->p);
		bBC[4]=W_OF(b->p)-Z_OF(b->p);bBC[5]=W_OF(b->p)+Z_OF(b->p);

		int aOutCode=((aBC[0]<0)<<5)|((aBC[1]<0)<<4)|((aBC[2]<0)<<3)|((aBC[3]<0)<<2)|
			((aBC[4]<0)<<1)|((aBC[5]<0));
		int bOutCode=((bBC[0]<0)<<5)|((bBC[1]<0)<<4)|((bBC[2]<0)<<3)|((bBC[3]<0)<<2)|
			((bBC[4]<0)<<1)|((bBC[5]<0));

		if((aOutCode&bOutCode)!=0) return false;//reject
		if((aOutCode|bOutCode)==0) //accept
		{
			clip_buf[0]=*a;
			clip_buf[1]=*b;
			return true;
		}

		for(int i=0;i<6;i++)
		{
			if(bBC[i]<0.0)
			{
				r=aBC[i]/(aBC[i]-bBC[i]);

				if(r<u1) return false;
				else if(r<u2) u2=r;
			}
			else if(aBC[i]<0.0)
			{
				r=aBC[i]/(aBC[i]-bBC[i]);

				if(r>u2) return false;
				else if(r>u1) u1=r;
			}
			//else both >=0.0
		}

		// need to perspective correct? u1 u2

		// 	assert(a->coord[2]!=0);
		// 	r=b->coord[2]/a->coord[2];

		if(aOutCode!=0)
		{
			LERP_COMPONENT_4(clip_buf,p,a,b,u1);

			// persp correction??
			u1=IMAGE_PLANE_LERP_TO_WORLD_LERP(u1,W_OF(a->p),W_OF(a->p));
			LERP_COMPONENT_4(clip_buf,col_front_pri,a,b,u1);
			LERP_COMPONENT_4(clip_buf,tex_coords,a,b,u1);
		}
		else
		{
			clip_buf[0]=*a;
		}
		if(bOutCode!=0)
		{
			LERP_COMPONENT_4(clip_buf+1,p,a,b,u2);

			// persp correction??
			u2=IMAGE_PLANE_LERP_TO_WORLD_LERP(u2,W_OF(a->p),W_OF(a->p));
			LERP_COMPONENT_4(clip_buf+1,col_front_pri,a,b,u2);
			LERP_COMPONENT_4(clip_buf+1,tex_coords,a,b,u2);
		}
		else
		{
			clip_buf[1]=*b;
		}

		return true;
	}

	// p1 -> p2
	//					output:
	// 1) in, in:		p2
	// 2) in, out:		isect
	// 3) out, in:		isect, p2
	// 4) out, out:	

#define IN_IN 3
#define IN_OUT 1
#define OUT_IN 2
#define OUT_OUT 0

#define NEGATIVE_MASK 0x80
#define _NEGATIVE_MASK 0x7f


	GLboolean ply_by_plane(int axis,Vertex* verts,GLint vcnt,Vertex* clip_buf,GLint* clip_cnt)
	{
		Vertex* p1=verts,*p2=p1+1,*clip_buf_start=clip_buf;
		Vertex pisect;
		GLdouble bc1,bc2,u;
		bool axis_negate=(axis&NEGATIVE_MASK);
		axis&=_NEGATIVE_MASK;
		
		*clip_cnt=0;
		
		// vcnt-1
		while(--vcnt>0)
		{
clip_plane_start:
			if(axis_negate)
			{
				bc1=W_OF(p1->p)+p1->p[axis];
				bc2=W_OF(p2->p)+p2->p[axis];
			}
			else
			{
				bc1=W_OF(p1->p)-p1->p[axis];
				bc2=W_OF(p2->p)-p2->p[axis];
			}

			switch((bc1>0.0)|((bc2>0.0)<<1))
			{
			case IN_IN:
				*(clip_buf++)=*p2;
				break;
			case IN_OUT:
				{
					Assert(bc1!=bc2);
					u=bc1/(bc1-bc2);

					LERP_COMPONENT_4(&pisect,p,p1,p2,u);

					// persp correction??
					u=IMAGE_PLANE_LERP_TO_WORLD_LERP(u,W_OF(p1->p),W_OF(p2->p));
					LERP_COMPONENT_4(&pisect,col_front_pri,p1,p2,u);
					LERP_COMPONENT_4(&pisect,tex_coords,p1,p2,u);

					*(clip_buf++)=pisect;
				}
				break;
			case OUT_IN:
				{
					Assert(bc1!=bc2);
					u=bc1/(bc1-bc2);

					LERP_COMPONENT_4(&pisect,p,p1,p2,u);

					// persp correction??
					u=IMAGE_PLANE_LERP_TO_WORLD_LERP(u,W_OF(p1->p),W_OF(p2->p));
					LERP_COMPONENT_4(&pisect,col_front_pri,p1,p2,u);
					LERP_COMPONENT_4(&pisect,tex_coords,p1,p2,u);

					*(clip_buf++)=pisect;
					*(clip_buf++)=*p2;
				}
				break;
			}
			p1=p2++;
		}
		// loop, last point and first point
		if(vcnt==0)
		{
			p2=verts;
			goto clip_plane_start;
		}

		//Assert(!ret||ret>=2);
		//Assert(ret!=1);

		return (*clip_cnt=(clip_buf-clip_buf_start))>0;
	}

	// Sutherland-Hodgman
	GLboolean polygon(Vertex* clip_buf,GLint vcnt,GLint* clip_cnt)
	{
		Vertex v2[10];
		*clip_cnt=vcnt;

#define CLIP_1TO2 clip_buf,*clip_cnt,v2,&vcnt
#define CLIP_2TO1 v2,vcnt,clip_buf,clip_cnt
// #define CLIP_1TO2(v1,c1,v2,c2) v1,c1,v2,&c2
// #define CLIP_2TO1(v1,c1,v2,c2) v2,c2,v1,&c1

		// 0x80 indicates negative
		return ply_by_plane(AXIS_Z|NEGATIVE_MASK,CLIP_1TO2)&&
			ply_by_plane(AXIS_Z,CLIP_2TO1)&&
			ply_by_plane(AXIS_X|NEGATIVE_MASK,CLIP_1TO2)&&
			ply_by_plane(AXIS_X,CLIP_2TO1)&&
			ply_by_plane(AXIS_Y|NEGATIVE_MASK,CLIP_1TO2)&&
			ply_by_plane(AXIS_Y,CLIP_2TO1);

// 		*clip_cnt=vcnt;
// 		for(int i=0;i<vcnt;++i)
// 			clip_buf[i]=*verts[i];
// 		return true;
	}
}