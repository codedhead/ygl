#include "../include/gltypes.h"
#include "../include/clip.h"
#include "../include/draw.h"


#define IONONE	-1
#define IN_IN	3
#define IN_OUT	1
#define OUT_IN	2
#define OUT_OUT	0

#define lerp(a,b,f) ((a)+(f)*((b)-(a)))

bool lineClipLiangBarsky(GLVertex* a,GLVertex* b)
{	
	double u1=0.0,u2=1.0,r;
	double aBC[6],bBC[6];

	//  1,-1
	//x
	//y
	//z
	aBC[0]=a->coord[3]-a->coord[0];aBC[1]=a->coord[0]+a->coord[3];
	aBC[2]=a->coord[3]-a->coord[1];aBC[3]=a->coord[1]+a->coord[3];
	aBC[4]=a->coord[3]-a->coord[2];aBC[5]=a->coord[2]+a->coord[3];
	bBC[0]=b->coord[3]-b->coord[0];bBC[1]=b->coord[0]+b->coord[3];
	bBC[2]=b->coord[3]-b->coord[1];bBC[3]=b->coord[1]+b->coord[3];
	bBC[4]=b->coord[3]-b->coord[2];bBC[5]=b->coord[2]+b->coord[3];

	int aOutCode=((aBC[0]<0)<<5)|((aBC[1]<0)<<4)|((aBC[2]<0)<<3)|((aBC[3]<0)<<2)|
	((aBC[4]<0)<<1)|((aBC[5]<0));
	int bOutCode=((bBC[0]<0)<<5)|((bBC[1]<0)<<4)|((bBC[2]<0)<<3)|((bBC[3]<0)<<2)|
	((bBC[4]<0)<<1)|((bBC[5]<0));

	if((aOutCode&bOutCode)!=0) return false;//reject
	if((aOutCode|bOutCode)==0) return true;//accept

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

	GLVertex t;
	if(aOutCode!=0)
	{
		t.coord[0]=lerp(a->coord[0],b->coord[0],u1);
		t.coord[1]=lerp(a->coord[1],b->coord[1],u1);
		t.coord[2]=lerp(a->coord[2],b->coord[2],u1);
		t.coord[3]=lerp(a->coord[3],b->coord[3],u1);

		t.color[0]=lerp(a->color[0],b->color[0],u1);
		t.color[1]=lerp(a->color[1],b->color[1],u1);
		t.color[2]=lerp(a->color[2],b->color[2],u1);
		t.color[3]=lerp(a->color[3],b->color[3],u1);

		t.tex_coord[0]=lerp(a->tex_coord[0],b->tex_coord[0],u1);
		t.tex_coord[1]=lerp(a->tex_coord[1],b->tex_coord[1],u1);
		t.tex_coord[2]=lerp(a->tex_coord[2],b->tex_coord[2],u1);
		t.tex_coord[3]=lerp(a->tex_coord[3],b->tex_coord[3],u1);
	}
	if(bOutCode!=0)
	{
		b->coord[0]=lerp(a->coord[0],b->coord[0],u2);
		b->coord[1]=lerp(a->coord[1],b->coord[1],u2);
		b->coord[2]=lerp(a->coord[2],b->coord[2],u2);
		b->coord[3]=lerp(a->coord[3],b->coord[3],u2);

		b->color[0]=lerp(a->color[0],b->color[0],u2);
		b->color[1]=lerp(a->color[1],b->color[1],u2);
		b->color[2]=lerp(a->color[2],b->color[2],u2);
		b->color[3]=lerp(a->color[3],b->color[3],u2);

		b->tex_coord[0]=lerp(a->tex_coord[0],b->tex_coord[0],u2);
		b->tex_coord[1]=lerp(a->tex_coord[1],b->tex_coord[1],u2);
		b->tex_coord[2]=lerp(a->tex_coord[2],b->tex_coord[2],u2);
		b->tex_coord[3]=lerp(a->tex_coord[3],b->tex_coord[3],u2);

	}
	if(aOutCode!=0)
	{
		*a=t;
	}

	return true;
}

bool clipByPlane(GLVertex* pgn,int pcnt,int axis,bool bPositive)
{
	GLVertex* p1=pgn,*p2=pgn+1;
	double u,z;

	GLVertex savefirst=*pgn;

	--pcnt;
	draw_vertex_cnt=0;

	//ÔÚ±ßÉÏ£¬·µ»Øin?out?

	for(int i=0;i<pcnt;i++)
	{
		double bc1,bc2;
		if(bPositive)
		{
			bc1=p1->coord[3]-p1->coord[axis],bc2=p2->coord[3]-p2->coord[axis];
		}
		else
		{
			bc1=p1->coord[3]+p1->coord[axis],bc2=p2->coord[3]+p2->coord[axis];
		}


		switch((bc1>0)|((bc2>0)<<1))
		{
		case IN_IN:
			draw_vertex_buffer[draw_vertex_cnt++]=*p2;
			break;
		case IN_OUT:
			{				
				u=bc1/(bc1-bc2);
	
				p1->coord[0]=lerp(p1->coord[0],p2->coord[0],u);
				p1->coord[1]=lerp(p1->coord[1],p2->coord[1],u);
				p1->coord[2]=lerp(p1->coord[2],p2->coord[2],u);
				p1->coord[3]=lerp(p1->coord[3],p2->coord[3],u);
				p1->color[0]=lerp(p1->color[0],p2->color[0],u);
				p1->color[1]=lerp(p1->color[1],p2->color[1],u);
				p1->color[2]=lerp(p1->color[2],p2->color[2],u);
				p1->color[3]=lerp(p1->color[3],p2->color[3],u);
				p1->tex_coord[0]=lerp(p1->tex_coord[0],p2->tex_coord[0],u);
				p1->tex_coord[1]=lerp(p1->tex_coord[1],p2->tex_coord[1],u);
				p1->tex_coord[2]=lerp(p1->tex_coord[2],p2->tex_coord[2],u);
				p1->tex_coord[3]=lerp(p1->tex_coord[3],p2->tex_coord[3],u);	
				
				draw_vertex_buffer[draw_vertex_cnt++]=*p1;
			}			
			break;
		case OUT_OUT:
			break;
		case OUT_IN:
			{
				u=bc1/(bc1-bc2);

				p1->coord[0]=lerp(p1->coord[0],p2->coord[0],u);
				p1->coord[1]=lerp(p1->coord[1],p2->coord[1],u);
				p1->coord[2]=lerp(p1->coord[2],p2->coord[2],u);
				p1->coord[3]=lerp(p1->coord[3],p2->coord[3],u);
				p1->color[0]=lerp(p1->color[0],p2->color[0],u);
				p1->color[1]=lerp(p1->color[1],p2->color[1],u);
				p1->color[2]=lerp(p1->color[2],p2->color[2],u);
				p1->color[3]=lerp(p1->color[3],p2->color[3],u);
				p1->tex_coord[0]=lerp(p1->tex_coord[0],p2->tex_coord[0],u);
				p1->tex_coord[1]=lerp(p1->tex_coord[1],p2->tex_coord[1],u);
				p1->tex_coord[2]=lerp(p1->tex_coord[2],p2->tex_coord[2],u);
				p1->tex_coord[3]=lerp(p1->tex_coord[3],p2->tex_coord[3],u);	

				draw_vertex_buffer[draw_vertex_cnt++]=*p1;
				draw_vertex_buffer[draw_vertex_cnt++]=*p2;
			}		
			break;
		}
		p1=p2++;
	}

	double bc1,bc2;
	if(bPositive)
	{
		bc1=p1->coord[3]-p1->coord[axis],bc2=savefirst.coord[3]-savefirst.coord[axis];
	}
	else
	{
		bc1=p1->coord[3]+p1->coord[axis],bc2=savefirst.coord[3]+savefirst.coord[axis];
	}
	
	switch((bc1>0)|((bc2>0)<<1))
	{
	case IN_IN:
		draw_vertex_buffer[draw_vertex_cnt++]=savefirst;
		break;
	case IN_OUT:
		{				
			u=bc1/(bc1-bc2);

			p1->coord[0]=lerp(p1->coord[0],savefirst.coord[0],u);
			p1->coord[1]=lerp(p1->coord[1],savefirst.coord[1],u);
			p1->coord[2]=lerp(p1->coord[2],savefirst.coord[2],u);
			p1->coord[3]=lerp(p1->coord[3],savefirst.coord[3],u);
			p1->color[0]=lerp(p1->color[0],savefirst.color[0],u);
			p1->color[1]=lerp(p1->color[1],savefirst.color[1],u);
			p1->color[2]=lerp(p1->color[2],savefirst.color[2],u);
			p1->color[3]=lerp(p1->color[3],savefirst.color[3],u);
			p1->tex_coord[0]=lerp(p1->tex_coord[0],savefirst.tex_coord[0],u);
			p1->tex_coord[1]=lerp(p1->tex_coord[1],savefirst.tex_coord[1],u);
			p1->tex_coord[2]=lerp(p1->tex_coord[2],savefirst.tex_coord[2],u);
			p1->tex_coord[3]=lerp(p1->tex_coord[3],savefirst.tex_coord[3],u);

			draw_vertex_buffer[draw_vertex_cnt++]=*p1;		
		}			
		break;
	case OUT_OUT:
		break;
	case OUT_IN:
		{
			u=bc1/(bc1-bc2);

			p1->coord[0]=lerp(p1->coord[0],savefirst.coord[0],u);
			p1->coord[1]=lerp(p1->coord[1],savefirst.coord[1],u);
			p1->coord[2]=lerp(p1->coord[2],savefirst.coord[2],u);
			p1->coord[3]=lerp(p1->coord[3],savefirst.coord[3],u);
			p1->color[0]=lerp(p1->color[0],savefirst.color[0],u);
			p1->color[1]=lerp(p1->color[1],savefirst.color[1],u);
			p1->color[2]=lerp(p1->color[2],savefirst.color[2],u);
			p1->color[3]=lerp(p1->color[3],savefirst.color[3],u);
			p1->tex_coord[0]=lerp(p1->tex_coord[0],savefirst.tex_coord[0],u);
			p1->tex_coord[1]=lerp(p1->tex_coord[1],savefirst.tex_coord[1],u);
			p1->tex_coord[2]=lerp(p1->tex_coord[2],savefirst.tex_coord[2],u);
			p1->tex_coord[3]=lerp(p1->tex_coord[3],savefirst.tex_coord[3],u);

			draw_vertex_buffer[draw_vertex_cnt++]=*p1;
			draw_vertex_buffer[draw_vertex_cnt++]=savefirst;
		}		
		break;
	}

	return draw_vertex_cnt>2;
}

#define SWAP_V_BUFFER {t=backbuffer;backbuffer=draw_vertex_buffer;draw_vertex_buffer=t;}

bool polygonClipSutherlandHodgman(GLVertex* initbuffer,int vcnt)
{
	if(vcnt<=2) return false;

	GLVertex* backbuffer=draw_vertex_buffer1,*t;
	draw_vertex_buffer=draw_vertex_buffer2;

	if(!clipByPlane(initbuffer,vcnt,2,true)) return false;
	SWAP_V_BUFFER;
	if(!clipByPlane(backbuffer,draw_vertex_cnt,2,false)) return false;
	SWAP_V_BUFFER;
	if(!clipByPlane(backbuffer,draw_vertex_cnt,0,true)) return false;
	SWAP_V_BUFFER;
	if(!clipByPlane(backbuffer,draw_vertex_cnt,0,false)) return false;
	SWAP_V_BUFFER;
	if(!clipByPlane(backbuffer,draw_vertex_cnt,1,true)) return false;
	SWAP_V_BUFFER;
	if(!clipByPlane(backbuffer,draw_vertex_cnt,1,false)) return false;

	return true;
}

void doClip(int type,GLVertex* vertexs,int pcnt)
{
	GLVertex* it=vertexs;	
	draw_vertex_cnt=0;
	
	switch(type)
	{
	case GL_POINTS:
		for(int i=0;i<pcnt;++i,++it)
		{
			double h=fabs(it->coord[3]);
			if(!((h==0)||
				(h+it->coord[0]<0)||
				(h-it->coord[0]<0)||
				(h+it->coord[1]<0)||
				(h-it->coord[1]<0)||
				(h+it->coord[2]<0)||
				(h-it->coord[2]<0)))
				draw_vertex_buffer[draw_vertex_cnt++]=*it;
		}
		break;
	case GL_LINES:
		{		
			pcnt>>=1;
			for(int i=0;i<pcnt;i++,it+=2)
			{
				if(lineClipLiangBarsky(it,it+1))
				{
					draw_vertex_buffer[draw_vertex_cnt++]=*it;
					draw_vertex_buffer[draw_vertex_cnt++]=*(it+1);
				}
			}	
		}
		break;
	case GL_LINE_LOOP:
		if(pcnt>1)
		{
			GLVertex firstvertex=*it,newvertex;
			for(int i=1;i<pcnt;++i,++it)
			{				
				newvertex=*(it+1);
				if(lineClipLiangBarsky(it,&newvertex))
				{
					draw_vertex_buffer[draw_vertex_cnt++]=*it;
					draw_vertex_buffer[draw_vertex_cnt++]=newvertex;
				}
			}	
			if(lineClipLiangBarsky(it,&firstvertex))
			{
				draw_vertex_buffer[draw_vertex_cnt++]=*it;
				draw_vertex_buffer[draw_vertex_cnt++]=firstvertex;
			}
		}
		break;
	case GL_LINE_STRIP:
		if(pcnt>1)
		{
			GLVertex newvertex;
			for(int i=1;i<pcnt;++i,++it)
			{				
				newvertex=*(it+1);
				if(lineClipLiangBarsky(it,&newvertex))
				{
					draw_vertex_buffer[draw_vertex_cnt++]=*it;
					draw_vertex_buffer[draw_vertex_cnt++]=newvertex;
				}
			}	
		}	
		break;	
	case GL_POLYGON:
		polygonClipSutherlandHodgman(vertexs,pcnt);
		break;
	}
}