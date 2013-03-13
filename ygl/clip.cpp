#include "clip.h"
#include "glcontext.h"
#include "global.h"

#define LERP_COMPONENT_4(dst,comp,va,vb,f) \
	(dst)->##comp##[0]=lerp((va)->##comp##[0],(vb)->##comp##[0],f);\
	(dst)->##comp##[1]=lerp((va)->##comp##[1],(vb)->##comp##[1],f);\
	(dst)->##comp##[2]=lerp((va)->##comp##[2],(vb)->##comp##[2],f);\
	(dst)->##comp##[3]=lerp((va)->##comp##[3],(vb)->##comp##[3],f);

namespace clip
{
	// liang-barsky
	GLboolean line(const Vertex* a,const Vertex* b,Vertex* clip_buf)
	{
		GLfloat/*double*/ u1=0.0,u2=1.0,r;
		GLfloat/*double*/ aBC[6],bBC[6];

		//  1,-1
		//x
		//y
		//z
		aBC[0]=a->p[3]-a->p[0];aBC[1]=a->p[0]+a->p[3];
		aBC[2]=a->p[3]-a->p[1];aBC[3]=a->p[1]+a->p[3];
		aBC[4]=a->p[3]-a->p[2];aBC[5]=a->p[2]+a->p[3];
		bBC[0]=b->p[3]-b->p[0];bBC[1]=b->p[0]+b->p[3];
		bBC[2]=b->p[3]-b->p[1];bBC[3]=b->p[1]+b->p[3];
		bBC[4]=b->p[3]-b->p[2];bBC[5]=b->p[2]+b->p[3];

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
			//else		
		}

		// 	assert(a->coord[2]!=0);
		// 	r=b->coord[2]/a->coord[2];

		if(aOutCode!=0)
		{
			LERP_COMPONENT_4(clip_buf,p,a,b,u1);

			LERP_COMPONENT_4(clip_buf,col_front_pri,a,b,u1);
			//LERP_COMPONENT_4(clip_buf[0],texc,a,b,u1);

// 			t.color[0]=lerp(a->color[0],b->color[0],u1);
// 			t.color[1]=lerp(a->color[1],b->color[1],u1);
// 			t.color[2]=lerp(a->color[2],b->color[2],u1);
// 			t.color[3]=lerp(a->color[3],b->color[3],u1);
// 
// 			t.tex_coord[0]=lerp(a->tex_coord[0],b->tex_coord[0],u1);
// 			t.tex_coord[1]=lerp(a->tex_coord[1],b->tex_coord[1],u1);
// 			t.tex_coord[2]=lerp(a->tex_coord[2],b->tex_coord[2],u1);
// 			t.tex_coord[3]=lerp(a->tex_coord[3],b->tex_coord[3],u1);
		}
		else
		{
			clip_buf[0]=*a;
		}
		if(bOutCode!=0)
		{
			LERP_COMPONENT_4(clip_buf+1,p,a,b,u2);
			LERP_COMPONENT_4(clip_buf+1,col_front_pri,a,b,u1);
		}
		else
		{
			clip_buf[1]=*b;
		}

		return true;
	}

	GLboolean polygon(const Vertex** verts,GLint vcnt,Vertex* clip_buf,GLint* clip_cnt)
	{
		*clip_cnt=vcnt;
		for(int i=0;i<vcnt;++i)
			clip_buf[i]=*verts[i];
		return true;
	}
}