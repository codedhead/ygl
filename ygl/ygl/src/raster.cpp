#include <math.h>
#include "../include/ymath.h"
#include "../include/raster.h"
#include "../include/buf.h"
#include "../include/draw.h"

#undef min
#define min(a,b) ((a)<(b)?(a):(b))


int line_factor,line_pattern_bit;


extern GLContext gl_context;

struct BresInfo
{
	int x,y;
	int major_axis;
	int major_end;
	int incr0,incr1,i0,i1;
	int error;
	double z,dz;
};

class ETRecord
{
public:
	int x,y;
	int yupper;
	int m;
	double real_m;
	int incr0,incr1,i0,i1;
	int error;
	double z,dz;

	void incr()
	{
		if(error<0)
		{
			error+=incr0;
			x+=i0;
		}
		else
		{
			error+=incr1;
			x+=i1;
		}
		z+=dz;
	}

	ETRecord* next,*prev;
};

class ETRecordSmooth:public ETRecord
{
public:
	double color[4];
	double dcolor[4];

	void incr()
	{
		if(error<0)
		{
			error+=incr0;
			x+=i0;
		}
		else
		{
			error+=incr1;
			x+=i1;
		}
		z+=dz;
		color[0]+=dcolor[0];
		color[1]+=dcolor[1];
		color[2]+=dcolor[2];
		color[3]+=dcolor[3];
	}
};


class ETRecordTexture:public ETRecord
{
public:
	double _4,d_4;
	double s_4,t_4,ds_4,dt_4;

	void incr()
	{
		if(error<0)
		{
			error+=incr0;
			x+=i0;
		}
		else
		{
			error+=incr1;
			x+=i1;
		}
		z+=dz;
		_4+=d_4;
		s_4+=ds_4;
		t_4+=dt_4;
	}
};

class ETRecordBlend:public ETRecordTexture
{
public:
	double color[4];
	double dcolor[4];

	void incr()
	{
		if(error<0)
		{
			error+=incr0;
			x+=i0;
		}
		else
		{
			error+=incr1;
			x+=i1;
		}
		z+=dz;
		color[0]+=dcolor[0];
		color[1]+=dcolor[1];
		color[2]+=dcolor[2];
		color[3]+=dcolor[3];
		_4+=d_4;
		s_4+=ds_4;
		t_4+=dt_4;
	}
};
class _ET
{
public:
	_ET()
	{
		head=tail=NULL;size=0;
	}
	void merge(_ET& et)
	{
		ETRecord* p=et.head;et.head=NULL;
		ETRecord* q=head,*np;
		while(p&&q)
		{
			if(p->x<=q->x)
			{
				np=p->next;
				insertbefore(q,p);
				p=np;
			}
			else
				q=q->next;
		}
		while(p)
		{
			np=p->next;
			append(p);p=np;
		}
	}

	void erase(ETRecord* e)
	{
		if(e==head)
		{
			if(e==tail)
			{
				tail=head=NULL;
			}
			else
			{
				e->next->prev=NULL;
				head=e->next;
			}
		}
		else if(e==tail)
		{
			e->prev->next=NULL;
			tail=e->prev;
		}
		else
		{
			e->prev->next=e->next;
			e->next->prev=e->prev;
		}

		--size;
		delete e;		
	}
	void add(ETRecord* e)
	{
		if(!head)
		{
			e->prev=e->next=NULL;
			head=e;
			tail=e;
		}
		else
		{
			ETRecord* p=head;
			if(e->x<p->x||(e->x==p->x&&e->real_m<p->real_m))
			{
				e->prev=NULL;
				e->next=head;
				head=e;
			}
			else
			{
				while(p->next&&((e->x>=p->next->x)||(e->x==p->next->x&&e->real_m>p->real_m)))
					p=p->next;
				if(p->next)
				{
					e->prev=p;p->next->prev=e;
					e->next=p->next;p->next=e;
				}
				else
				{
					p->next=e;
					e->next=NULL;
					e->prev=p;
					tail=e;
				}				
			}
		}
		++size;
	}

	void insertbefore(ETRecord* pos,ETRecord* e)
	{
		if(pos==head)
		{
			e->next=pos;
			pos->prev=e;
			e->prev=NULL;
			head=e;
		}
		else
		{
			pos->prev->next=e;
			e->prev=pos->prev;
			e->next=pos;
			pos->prev=e;
		}
		++size;
	}
	void append(ETRecord* e)
	{
		if(head==NULL)
		{
			head=tail=e;
			e->prev=e->next=NULL;
		}
		else
		{
			e->prev=tail;
			e->next=NULL;
			tail->next=e;
			tail=e;
		}
		++size;
	}

	int size;
	ETRecord* head,*tail;
};


_ET ETs[MAX_EDGETABLE_SIZE+3];

//y0<y1
void BUILD_BRES_y0_lt_y1(int x0,int y0,double z0,int x1,int y1,double z1,BresInfo& bres_name)
{
	int dx=x1-x0,dy=y1-y0,_bres_macro_t;

	if(dx<0)
	{
		dx=-dx;		

		if(dx>=dy)
		{	
			bres_name.x=x1;
			bres_name.y=y1;
			bres_name.z=z1;
			bres_name.dz=(z0-z1)/((double)(x0-x1));

			bres_name.major_axis=0;
			bres_name.major_end=x0;

			bres_name.incr1=-dy-dy;
			bres_name.error=bres_name.incr1+dx;
			bres_name.incr0=bres_name.error+dx;
			bres_name.i0=-1;bres_name.i1=0;
		}
		else
		{
			bres_name.x=x0;
			bres_name.y=y0;
			bres_name.z=z0;
			bres_name.dz=(z0-z1)/((double)(y0-y1));

			bres_name.major_axis=1;
			bres_name.major_end=y1;

			bres_name.incr0=dx+dx;
			bres_name.error=bres_name.incr0-dy;
			bres_name.incr1=bres_name.error-dy;
			bres_name.i0=0;bres_name.i1=-1;
		}
	}
	else
	{
		bres_name.x=x0;
		bres_name.y=y0;
		bres_name.z=z0;

		if(dx>=dy)
		{			
			bres_name.major_axis=0;
			bres_name.major_end=x1;
			bres_name.dz=(z0-z1)/((double)(x0-x1));

			bres_name.incr0=dy+dy;
			bres_name.error=bres_name.incr0-dx;
			bres_name.incr1=bres_name.error-dx;
			bres_name.i0=0;bres_name.i1=1;
		}
		else
		{
			bres_name.major_axis=1;
			bres_name.major_end=y1;
			bres_name.dz=(z0-z1)/((double)(y0-y1));

			bres_name.incr1=-dx-dx;
			bres_name.error=bres_name.incr1+dy;
			bres_name.incr0=bres_name.error+dy;
			bres_name.i0=1;bres_name.i1=0;
		}	
	}
}

void BUILD_BRES(int x0,int y0,double z0,int x1,int y1,double z1,BresInfo& bres_name)
{
	if(y0<y1) BUILD_BRES_y0_lt_y1(x0,y0,z0,x1,y1,z1,bres_name);
	else BUILD_BRES_y0_lt_y1(x1,y1,z1,x0,y0,z0,bres_name);
}
void buildRecord_y0_lt_y1(int dx,int dy,ETRecord* bres_name)
{
		
	if(dx<0)
	{
		dx=-dx;			

		bres_name->incr0=dx+dx+(dy+dy)*bres_name->m;
		bres_name->error=bres_name->incr0-dy;
		bres_name->incr1=bres_name->error-dy;
		bres_name->i0=bres_name->m;bres_name->i1=bres_name->m-1;
	}
	else
	{
		bres_name->m=dx/dy;
		bres_name->incr1=-dx-dx+(dy+dy)*bres_name->m;
		bres_name->error=bres_name->incr1+dy;
		bres_name->incr0=bres_name->error+dy;
		bres_name->i0=bres_name->m+1;bres_name->i1=bres_name->m;
	}
}

void buildRecord(int x0,int y0,double z0,int x1,int y1,double z1,ETRecord* bres_name)
{
	int dx,dy;
	if(y0<y1)
	{
		dx=x1-x0;dy=y1-y0;
		bres_name->x=x0;
		bres_name->y=y0;
		bres_name->z=z0;
		bres_name->yupper=y1;
	}
	else
	{
		dx=x0-x1;dy=y0-y1;
		bres_name->x=x1;
		bres_name->y=y1;
		bres_name->z=z1;
		bres_name->yupper=y0;		
	}
	bres_name->dz=(z0-z1)/((double)(y0-y1));		
	bres_name->m=dx/dy;
	bres_name->real_m=((double)dx)/((double)dy);
	buildRecord_y0_lt_y1(dx,dy,bres_name);
}
void buildRecord_Smooth(int test,GLVertex* it,GLVertex* nextit,int x0,int y0,double z0,int x1,int y1,double z1,ETRecordSmooth* bres_name)
{
	int dx,dy;
	if(test<0)
	{
		dx=x1-x0;dy=y1-y0;
		bres_name->x=x0;
		bres_name->y=y0;
		bres_name->z=z0;
		bres_name->yupper=y1;

		memcpy(bres_name->color,it->color,4*sizeof(double));
		bres_name->dcolor[0]=nextit->color[0]-it->color[0];
		bres_name->dcolor[1]=nextit->color[1]-it->color[1];
		bres_name->dcolor[2]=nextit->color[2]-it->color[2];
		bres_name->dcolor[3]=nextit->color[3]-it->color[3];
	}
	else
	{
		dx=x0-x1;dy=y0-y1;
		bres_name->x=x1;
		bres_name->y=y1;
		bres_name->z=z1;
		bres_name->yupper=y0;	

		memcpy(bres_name->color,nextit->color,4*sizeof(double));
		bres_name->dcolor[0]=it->color[0]-nextit->color[0];
		bres_name->dcolor[1]=it->color[1]-nextit->color[1];
		bres_name->dcolor[2]=it->color[2]-nextit->color[2];
		bres_name->dcolor[3]=it->color[3]-nextit->color[3];
	}
	bres_name->dz=(z0-z1)/((double)(y0-y1));		
	bres_name->m=dx/dy;

	double df=1.0/((double)dy);
	bres_name->real_m=((double)dx)*df;	
	bres_name->dcolor[0]*=df;
	bres_name->dcolor[1]*=df;
	bres_name->dcolor[2]*=df;
	bres_name->dcolor[3]*=df;

	buildRecord_y0_lt_y1(dx,dy,bres_name);
}
void buildRecord_Texture(int test,GLVertex* it,GLVertex* nextit,int x0,int y0,double z0,int x1,int y1,double z1,ETRecordTexture* bres_name)
{
	int dx,dy;
	if(test<0)
	{
		dx=x1-x0;dy=y1-y0;
		bres_name->x=x0;
		bres_name->y=y0;
		bres_name->z=z0;
		bres_name->yupper=y1;

		bres_name->_4=it->w;
		bres_name->s_4=it->tex_coord[0];
		bres_name->t_4=it->tex_coord[1];
		bres_name->d_4=nextit->w-it->w;
		bres_name->ds_4=nextit->tex_coord[0]-it->tex_coord[0];
		bres_name->dt_4=nextit->tex_coord[1]-it->tex_coord[1];
	}
	else
	{
		dx=x0-x1;dy=y0-y1;
		bres_name->x=x1;
		bres_name->y=y1;
		bres_name->z=z1;
		bres_name->yupper=y0;

		bres_name->_4=nextit->w;
		bres_name->s_4=nextit->tex_coord[0];
		bres_name->t_4=nextit->tex_coord[1];
		bres_name->d_4=it->w-nextit->w;
		bres_name->ds_4=it->tex_coord[0]-nextit->tex_coord[0];
		bres_name->dt_4=it->tex_coord[1]-nextit->tex_coord[1];

	}
	bres_name->dz=(z0-z1)/((double)(y0-y1));		
	bres_name->m=dx/dy;

	double df=1.0/((double)dy);
	bres_name->real_m=((double)dx)*df;	
	bres_name->d_4*=df;
	bres_name->ds_4*=df;
	bres_name->dt_4*=df;

	buildRecord_y0_lt_y1(dx,dy,bres_name);
}
void buildRecord_Blend(int test,GLVertex* it,GLVertex* nextit,int x0,int y0,double z0,int x1,int y1,double z1,ETRecordBlend* bres_name)
{
	int dx,dy;
	if(y0<y1)
	{
		dx=x1-x0;dy=y1-y0;
		bres_name->x=x0;
		bres_name->y=y0;
		bres_name->z=z0;
		bres_name->yupper=y1;

		memcpy(bres_name->color,it->color,4*sizeof(double));
		bres_name->dcolor[0]=nextit->color[0]-it->color[0];
		bres_name->dcolor[1]=nextit->color[1]-it->color[1];
		bres_name->dcolor[2]=nextit->color[2]-it->color[2];
		bres_name->dcolor[3]=nextit->color[3]-it->color[3];
		bres_name->_4=it->w;
		bres_name->s_4=it->tex_coord[0];
		bres_name->t_4=it->tex_coord[1];
		bres_name->d_4=nextit->w-it->w;
		bres_name->ds_4=nextit->tex_coord[0]-it->tex_coord[0];
		bres_name->dt_4=nextit->tex_coord[1]-it->tex_coord[1];

	}
	else
	{
		dx=x0-x1;dy=y0-y1;
		bres_name->x=x1;
		bres_name->y=y1;
		bres_name->z=z1;
		bres_name->yupper=y0;		

		memcpy(bres_name->color,nextit->color,4*sizeof(double));
		bres_name->dcolor[0]=it->color[0]-nextit->color[0];
		bres_name->dcolor[1]=it->color[1]-nextit->color[1];
		bres_name->dcolor[2]=it->color[2]-nextit->color[2];
		bres_name->dcolor[3]=it->color[3]-nextit->color[3];
		bres_name->_4=nextit->w;
		bres_name->s_4=nextit->tex_coord[0];
		bres_name->t_4=nextit->tex_coord[1];
		bres_name->d_4=it->w-nextit->w;
		bres_name->ds_4=it->tex_coord[0]-nextit->tex_coord[0];
		bres_name->dt_4=it->tex_coord[1]-nextit->tex_coord[1];
	}
	bres_name->dz=(z0-z1)/((double)(y0-y1));		
	bres_name->m=dx/dy;

	double df=1.0/((double)dy);
	bres_name->real_m=((double)dx)*df;	
	bres_name->d_4*=df;
	bres_name->ds_4*=df;
	bres_name->dt_4*=df;
	bres_name->dcolor[0]*=df;
	bres_name->dcolor[1]*=df;
	bres_name->dcolor[2]*=df;
	bres_name->dcolor[3]*=df;

	buildRecord_y0_lt_y1(dx,dy,bres_name);
}
void line(GLVertex* p0,GLVertex* p1)
{
	int x,y;
	double z;
	double* color=p1->color;

	// 	if(((int)(p0->coord[0]+0.5))==((int)(p1->coord[0]+0.5)))
	// 	{
	// 		int end;
	// 		x=p0->coord[0]+0.5;
	// 		if(p0->coord[1]<p1->coord[1])
	// 		{
	// 			y=p0->coord[1]+0.5;
	// 			end=p1->coord[1]+0.5;			
	// 		}
	// 		else
	// 		{
	// 			y=p1->coord[1]+0.5;
	// 			end=p0->coord[1]+0.5;			
	// 		}
	// 
	// 		for(;y<=end;++y)
	// 			glbufSetPixelv(x,y,0.0,color);
	// 
	// 		return;
	// 	}
	// 	else if(((int)(p0->coord[1]+0.5))==((int)(p1->coord[1]+0.5)))
	// 	{
	// 		int end;
	// 		y=p0->coord[1]+0.5;
	// 		if(p0->coord[0]<p1->coord[0])
	// 		{
	// 			x=p0->coord[0]+0.5;
	// 			end=p1->coord[0]+0.5;			
	// 		}
	// 		else
	// 		{
	// 			x=p1->coord[0]+0.5;
	// 			end=p0->coord[0]+0.5;			
	// 		}
	// 
	// 		for(;x<=end;++x)
	// 			glbufSetPixelv(x,y,0.0,color);
	// 
	// 		return;
	// 	}

	BresInfo bresinfo;
	BUILD_BRES(p0->coord[0]+0.5,p0->coord[1]+0.5,p0->coord[2],p1->coord[0]+0.5,p1->coord[1]+0.5,p1->coord[2],bresinfo);

	if(bresinfo.major_axis==0)
	{
		for(x=bresinfo.x,y=bresinfo.y,z=bresinfo.z;x<=bresinfo.major_end;++x,z+=bresinfo.dz)
		{
			glbufSetPixelv(x,y,z,color);
			if(bresinfo.error>=0)
			{
				bresinfo.error+=bresinfo.incr1;
				y+=bresinfo.i1;
			}
			else
			{
				bresinfo.error+=bresinfo.incr0;
				y+=bresinfo.i0;
			}
		}
	}
	else
	{
		for(x=bresinfo.x,y=bresinfo.y,z=bresinfo.z;y<=bresinfo.major_end;++y,z+=bresinfo.dz)
		{
			glbufSetPixelv(x,y,z,color);
			if(bresinfo.error>=0)
			{
				bresinfo.error+=bresinfo.incr1;
				x+=bresinfo.i1;
			}
			else
			{
				bresinfo.error+=bresinfo.incr0;
				x+=bresinfo.i0;
			}
		}
	}
}

void linestipple(GLVertex* p0,GLVertex* p1)
{
	int x,y;
	double z;
	double* color=p1->color;

	BresInfo bresinfo;
	BUILD_BRES(p0->coord[0]+0.5,p0->coord[1]+0.5,p0->coord[2],p1->coord[0]+0.5,p1->coord[1]+0.5,p1->coord[2],bresinfo);

	if(bresinfo.major_axis==0)
	{
		for(x=bresinfo.x,y=bresinfo.y,z=bresinfo.z;x<=bresinfo.major_end;++x,z+=bresinfo.dz)
		{
			if(gl_context.line_stipple_pattern&(1<<line_pattern_bit))
				glbufSetPixelv(x,y,z,color);
			if(bresinfo.error>=0)
			{
				bresinfo.error+=bresinfo.incr1;
				y+=bresinfo.i1;
			}
			else
			{
				bresinfo.error+=bresinfo.incr0;
				y+=bresinfo.i0;
			}
			line_factor=(++line_factor)%gl_context.line_stipple_factor;
			if(!line_factor) line_pattern_bit=(++line_pattern_bit)&0xf;
		}
	}
	else
	{
		for(x=bresinfo.x,y=bresinfo.y,z=bresinfo.z;y<=bresinfo.major_end;++y,z+=bresinfo.dz)
		{
			if(gl_context.line_stipple_pattern&(1<<line_pattern_bit))
				glbufSetPixelv(x,y,z,color);
			if(bresinfo.error>=0)
			{
				bresinfo.error+=bresinfo.incr1;
				x+=bresinfo.i1;
			}
			else
			{
				bresinfo.error+=bresinfo.incr0;
				x+=bresinfo.i0;
			}
			line_factor=(++line_factor)%gl_context.line_stipple_factor;
			if(!line_factor) line_pattern_bit=(++line_pattern_bit)&0xf;
		}
	}
}






//no sorting,remember the deleting position
void fill(int ymin,int ymax)
{
	if(draw_vertex_cnt>2)
	{
		memset(ETs,0,sizeof(ETs));
		double* orgcolor=draw_vertex_buffer[draw_vertex_cnt-1].color,*color=orgcolor;
		double _color[4];
		
		bool depthtest=gl_context.depth_test;
		bool depthmask=gl_context.depth_mask;

		float* fogcolor=gl_context.fog_color;
		double* __m=gl_context.projection_matrix_stack.peek()->m;
		double m10=__m[10],m14=__m[14];
		double vz,ff,fdensity=gl_context.fog_density,fstart=gl_context.fog_start,fdelta=gl_context.fog_end-fstart;

		//添加第一个点
		draw_vertex_buffer[draw_vertex_cnt]=draw_vertex_buffer[0];

		int i=0;
		//初始化表
		for(GLVertex* it=draw_vertex_buffer,*nextit=draw_vertex_buffer+1;i<draw_vertex_cnt;++i,++it,++nextit)
		{
			if(((int)(it->coord[1]+0.5))==((int)(nextit->coord[1]+0.5))) continue;
			ETRecord* newET=new ETRecord;
			buildRecord(it->coord[0]+0.5,it->coord[1]+0.5,it->coord[2],nextit->coord[0]+0.5,nextit->coord[1]+0.5,nextit->coord[2],newET);
			
//			if() --newET->yupper;
	
			ETs[newET->y].add(newET);
		}


		_ET AET;		

		for(int scanline=ymin;scanline<=ymax;scanline++)
		{			
			if(ETs[scanline].size||AET.size)
			{	
				ETRecord* et,*nextet;
				
				AET.merge(ETs[scanline]);

				et=AET.head;
				nextet=et->next;

				int spanCnt=AET.size>>1;				

				while(spanCnt--)
				{						
					int left=et->x,right=nextet->x;
					double deltaz=(nextet->z-et->z)/(right-left);
					double startz=et->z;
					byte tcolor;

					byte* fp=GET_FRAME_BUFFER(scanline,left);
					double* zp=GET_Z_BUFFER(scanline,left);
					double* ap=GET_ALPHA_BUFFER(scanline,left);
					double alpha;
					for(;left<right;left++,startz+=deltaz)
					{
						//left>=0&&scanline>=0??
						if(!depthtest||depthFunc(startz,*zp))
						{
							color=orgcolor;
							//fog
							if(gl_context.fog_enabled)
							{
								if((vz=m14/(startz+startz-1.0+m10)-fstart)>0.0)
								{
									switch(gl_context.fog_mode)
									{
									case GL_EXP:
										ff=exp(-fdensity*vz);
										break;
									case GL_EXP2:
										ff=exp(-fdensity*fdensity*vz*vz);
										break;
									case GL_LINEAR:
										ff=(fdelta-vz)/fdelta;
										break;
									}
									color=_color;
									color[0]=lerp(fogcolor[0],orgcolor[0],ff);
									color[1]=lerp(fogcolor[1],orgcolor[1],ff);
									color[2]=lerp(fogcolor[2],orgcolor[2],ff);
									color[3]=orgcolor[3];
								}								
							}
							if(!gl_context.blend_enabled)
							{
								tcolor=255*color[RGB_CORRECT(0)];
								*fp++=min(tcolor,255);
								tcolor=255*color[RGB_CORRECT(1)];
								*fp++=min(tcolor,255);
								tcolor=255*color[RGB_CORRECT(2)];
								*fp++=min(tcolor,255);
								tcolor=255*color[3];
								*ap++=min(tcolor,255);								
							}
							else
							{
								double bcolor[4];
								switch(gl_context.blend_sfactor)
								{
								case GL_ZERO:
									bcolor[0]=bcolor[1]=bcolor[2]=bcolor[3]=0.0;
									break;
								case GL_ONE:
									bcolor[0]=color[0];
									bcolor[1]=color[1];
									bcolor[2]=color[2];
									bcolor[3]=color[3];
									break;
								case GL_DST_ALPHA:
									alpha=*ap;
									bcolor[0]=color[0]*alpha;
									bcolor[1]=color[1]*alpha;
									bcolor[2]=color[2]*alpha;
									bcolor[3]=color[3]*alpha;
									break;
								case GL_DST_COLOR:
									bcolor[RGB_CORRECT(0)]=color[RGB_CORRECT(0)]*((*fp)/255.0);
									bcolor[RGB_CORRECT(1)]=color[RGB_CORRECT(1)]*(*(fp+1)/255.0);
									bcolor[RGB_CORRECT(2)]=color[RGB_CORRECT(2)]*(*(fp+2)/255.0);
									bcolor[3]=color[3]*(*ap);
									break;								
								case GL_ONE_MINUS_DST_ALPHA:
									alpha=1.0-*ap;
									bcolor[0]=color[0]*alpha;
									bcolor[1]=color[1]*alpha;
									bcolor[2]=color[2]*alpha;
									bcolor[3]=color[3]*alpha;
									break;
								case GL_ONE_MINUS_DST_COLOR:
									bcolor[RGB_CORRECT(0)]=color[RGB_CORRECT(0)]*(1.0-(*fp/255.0));
									bcolor[RGB_CORRECT(1)]=color[RGB_CORRECT(1)]*(1.0-*(fp+1)/255.0);
									bcolor[RGB_CORRECT(2)]=color[RGB_CORRECT(2)]*(1.0-*(fp+2)/255.0);
									bcolor[3]=color[3]*(1.0-*ap);
									break;
								case GL_ONE_MINUS_SRC_ALPHA:
									alpha=1.0-color[3];
									bcolor[0]=color[0]*alpha;
									bcolor[1]=color[1]*alpha;
									bcolor[2]=color[2]*alpha;
									bcolor[3]=color[3]*alpha;
									break;
								case GL_SRC_ALPHA:
									alpha=color[3];
									bcolor[0]=color[0]*alpha;
									bcolor[1]=color[1]*alpha;
									bcolor[2]=color[2]*alpha;
									bcolor[3]=color[3]*alpha;
									break;
								case GL_SRC_ALPHA_SATURATE:
									alpha=min(1.0-*ap,color[3]);
									bcolor[0]=color[0]*alpha;
									bcolor[1]=color[1]*alpha;
									bcolor[2]=color[2]*alpha;
									bcolor[3]=color[3];
									break;
								}
								switch(gl_context.blend_dfactor)
								{
								case GL_ZERO:
									break;
								case GL_ONE:
									bcolor[RGB_CORRECT(0)]+=*fp/255.0;
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0;
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0;
									bcolor[3]+=*ap;
									break;
								case GL_DST_ALPHA:
									alpha=*ap;
									bcolor[RGB_CORRECT(0)]+=*fp/255.0*alpha;
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0*alpha;
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0*alpha;
									bcolor[3]+=alpha*alpha;
									break;								
								case GL_ONE_MINUS_DST_ALPHA:
									alpha=1.0-*ap;
									bcolor[RGB_CORRECT(0)]+=*fp/255.0*alpha;
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0*alpha;
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0*alpha;
									bcolor[3]+=*ap*alpha;
									break;
								case GL_ONE_MINUS_SRC_ALPHA:
									alpha=1.0-color[3];
									bcolor[RGB_CORRECT(0)]+=*fp/255.0*alpha;
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0*alpha;
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0*alpha;
									bcolor[3]+=*ap*alpha;
									break;
								case GL_ONE_MINUS_SRC_COLOR:
									bcolor[RGB_CORRECT(0)]+=*fp/255.0*(1.0-color[RGB_CORRECT(0)]);
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0*(1.0-color[RGB_CORRECT(1)]);
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0*(1.0-color[RGB_CORRECT(2)]);
									bcolor[3]+=*ap*(1.0-color[3]);
									break;
								case GL_SRC_ALPHA:
									alpha=color[3];
									bcolor[RGB_CORRECT(0)]+=*fp/255.0*alpha;
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0*alpha;
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0*alpha;
									bcolor[3]+=*ap*alpha;
									break;
								case GL_SRC_COLOR:
									bcolor[RGB_CORRECT(0)]+=*fp/255.0*color[RGB_CORRECT(0)];
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0*color[RGB_CORRECT(1)];
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0*color[RGB_CORRECT(2)];
									bcolor[3]+=*ap*color[3];
									break;
								}

								tcolor=255*bcolor[RGB_CORRECT(0)];
								*fp++=min(tcolor,255);
								tcolor=255*bcolor[RGB_CORRECT(1)];
								*fp++=min(tcolor,255);
								tcolor=255*bcolor[RGB_CORRECT(2)];
								*fp++=min(tcolor,255);
								tcolor=255*bcolor[3];
								*ap++=min(tcolor,255);	
							}
							if(depthmask) *zp=startz;	
							++zp;
						}
						else
						{
							++fp;++fp;++fp;++ap;++zp;
						}						
					}

					if(scanline+1>=et->yupper)
					{
						AET.erase(et);
						et=nextet->next;

						if(scanline+1>=nextet->yupper)
							AET.erase(nextet);
						else
							nextet->incr();						
					}
					else if(scanline+1>=nextet->yupper)
					{
						et->incr();

						et=nextet->next;
						AET.erase(nextet);
					}
					else
					{						
						et->incr();
						nextet->incr();

						et=nextet->next;					
					}
					if(et) nextet=et->next;
				}

			}
			else
				break;
		}

		//AET not empty?
		ETRecord* xx=AET.head,*yy;
		while(xx)
		{
			yy=xx;			
			xx=xx->next;
			delete yy;
		}
	}
	//ETS not clean?
}
void smoothfill(int ymin,int ymax)
{
	if(draw_vertex_cnt>2)
	{
		memset(ETs,0,sizeof(ETs));
		bool depthtest=gl_context.depth_test;
		bool depthmask=gl_context.depth_mask;

		float* fogcolor=gl_context.fog_color;
		double* __m=gl_context.projection_matrix_stack.peek()->m;
		double m10=__m[10],m14=__m[14];
		double vz,ff,fdensity=gl_context.fog_density,fstart=gl_context.fog_start,fdelta=gl_context.fog_end-fstart;

		//添加第一个点
		draw_vertex_buffer[draw_vertex_cnt]=draw_vertex_buffer[0];

		int i=0,testval;
		//初始化表
		for(GLVertex* it=draw_vertex_buffer,*nextit=draw_vertex_buffer+1;i<draw_vertex_cnt;++i,++it,++nextit)
		{
			
			if(!(testval=((int)(it->coord[1]+0.5))-((int)(nextit->coord[1]+0.5)))) continue;
			
			ETRecordSmooth* newET=new ETRecordSmooth;		
			buildRecord_Smooth(testval,it,nextit,it->coord[0]+0.5,it->coord[1]+0.5,it->coord[2],nextit->coord[0]+0.5,nextit->coord[1]+0.5,nextit->coord[2],newET);				

			//			if() --newET->yupper;

			ETs[newET->y].add(newET);
		}


		_ET AET;

		for(int scanline=ymin;scanline<=ymax;scanline++)
		{			
			if(ETs[scanline].size||AET.size)
			{	
				ETRecordSmooth* et,*nextet;

				AET.merge(ETs[scanline]);

				et=(ETRecordSmooth*)AET.head;
				nextet=(ETRecordSmooth*)et->next;

				int spanCnt=AET.size>>1;				

				while(spanCnt--)
				{						
					int left=et->x,right=nextet->x;
					double deltaf=1.0/((double)(right-left));
					double deltaz=(nextet->z-et->z)*deltaf;
					double startz=et->z;

					double _color[4],dcolor[4],xcolor[4],*color=_color;
					color[0]=et->color[0];
					color[1]=et->color[1];
					color[2]=et->color[2];
					color[3]=et->color[3];
					dcolor[0]=(nextet->color[0]-et->color[0])*deltaf;
					dcolor[1]=(nextet->color[1]-et->color[1])*deltaf;
					dcolor[2]=(nextet->color[2]-et->color[2])*deltaf;
					dcolor[3]=(nextet->color[3]-et->color[3])*deltaf;

					byte tcolor;double alpha;
					double* ap=GET_ALPHA_BUFFER(scanline,left);
					byte* fp=GET_FRAME_BUFFER(scanline,left);
					double* zp=GET_Z_BUFFER(scanline,left);					
					for(;left<right;left++,startz+=deltaz)
					{		
						if(!depthtest||depthFunc(startz,*zp))
						{						
//						bool df;
// 						if(depthtest)
// 						{
// 							switch(gl_context.depth_func)
// 							{
// 							case GL_NEVER:df=false;break;
// 							case GL_LESS:df=startz<*zp;break;
// 							case GL_LEQUAL:  df=startz<=*zp;break;
// 							case GL_EQUAL: df=startz==*zp;break;
// 							case GL_GREATER:df=startz>*zp;break;
// 							case GL_NOTEQUAL:df=startz!=*zp;break;
// 							case GL_GEQUAL: df=startz>=*zp;break;
// 							case GL_ALWAYS:df=true;break;
// 							}
// 						}
// 						else df=true;						
//						if(df)
//						{
							//fog
							if(gl_context.fog_enabled)
							{
								if((vz=m14/(startz+startz-1.0+m10)-fstart)>0.0)
								{
									color=xcolor;
									switch(gl_context.fog_mode)
									{
									case GL_EXP:
										ff=exp(-fdensity*vz);
										break;
									case GL_EXP2:
										ff=exp(-fdensity*fdensity*vz*vz);
										break;
									case GL_LINEAR:
										ff=(fdelta-vz)/fdelta;
										break;
									}
									color[0]=lerp(fogcolor[0],_color[0],ff);
									color[1]=lerp(fogcolor[1],_color[1],ff);
									color[2]=lerp(fogcolor[2],_color[2],ff);
									color[3]=_color[3];
								}	
							}

							if(!gl_context.blend_enabled)
							{
								tcolor=255*color[RGB_CORRECT(0)];
								*fp++=min(tcolor,255);
								tcolor=255*color[RGB_CORRECT(1)];
								*fp++=min(tcolor,255);
								tcolor=255*color[RGB_CORRECT(2)];
								*fp++=min(tcolor,255);
								tcolor=255*color[3];
								*ap++=min(tcolor,255);								
							}
							else
							{
								double bcolor[4];
								switch(gl_context.blend_sfactor)
								{
								case GL_ZERO:
									bcolor[0]=bcolor[1]=bcolor[2]=bcolor[3]=0.0;
									break;
								case GL_ONE:
									bcolor[0]=color[0];
									bcolor[1]=color[1];
									bcolor[2]=color[2];
									bcolor[3]=color[3];
									break;
								case GL_DST_ALPHA:
									alpha=*ap;
									bcolor[0]=color[0]*alpha;
									bcolor[1]=color[1]*alpha;
									bcolor[2]=color[2]*alpha;
									bcolor[3]=color[3]*alpha;
									break;
								case GL_DST_COLOR:
									bcolor[RGB_CORRECT(0)]=color[RGB_CORRECT(0)]*((*fp)/255.0);
									bcolor[RGB_CORRECT(1)]=color[RGB_CORRECT(1)]*(*(fp+1)/255.0);
									bcolor[RGB_CORRECT(2)]=color[RGB_CORRECT(2)]*(*(fp+2)/255.0);
									bcolor[3]=color[3]*(*ap);
									break;								
								case GL_ONE_MINUS_DST_ALPHA:
									alpha=1.0-*ap;
									bcolor[0]=color[0]*alpha;
									bcolor[1]=color[1]*alpha;
									bcolor[2]=color[2]*alpha;
									bcolor[3]=color[3]*alpha;
									break;
								case GL_ONE_MINUS_DST_COLOR:
									bcolor[RGB_CORRECT(0)]=color[RGB_CORRECT(0)]*(1.0-(*fp/255.0));
									bcolor[RGB_CORRECT(1)]=color[RGB_CORRECT(1)]*(1.0-*(fp+1)/255.0);
									bcolor[RGB_CORRECT(2)]=color[RGB_CORRECT(2)]*(1.0-*(fp+2)/255.0);
									bcolor[3]=color[3]*(1.0-*ap);
									break;
								case GL_ONE_MINUS_SRC_ALPHA:
									alpha=1.0-color[3];
									bcolor[0]=color[0]*alpha;
									bcolor[1]=color[1]*alpha;
									bcolor[2]=color[2]*alpha;
									bcolor[3]=color[3]*alpha;
									break;
								case GL_SRC_ALPHA:
									alpha=color[3];
									bcolor[0]=color[0]*alpha;
									bcolor[1]=color[1]*alpha;
									bcolor[2]=color[2]*alpha;
									bcolor[3]=color[3]*alpha;
									break;
								case GL_SRC_ALPHA_SATURATE:
									alpha=min(1.0-*ap,color[3]);
									bcolor[0]=color[0]*alpha;
									bcolor[1]=color[1]*alpha;
									bcolor[2]=color[2]*alpha;
									bcolor[3]=color[3];
									break;
								}
								switch(gl_context.blend_dfactor)
								{
								case GL_ZERO:
									break;
								case GL_ONE:
									bcolor[RGB_CORRECT(0)]+=*fp/255.0;
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0;
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0;
									bcolor[3]+=*ap;
									break;
								case GL_DST_ALPHA:
									alpha=*ap;
									bcolor[RGB_CORRECT(0)]+=*fp/255.0*alpha;
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0*alpha;
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0*alpha;
									bcolor[3]+=alpha*alpha;
									break;								
								case GL_ONE_MINUS_DST_ALPHA:
									alpha=1.0-*ap;
									bcolor[RGB_CORRECT(0)]+=*fp/255.0*alpha;
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0*alpha;
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0*alpha;
									bcolor[3]+=*ap*alpha;
									break;
								case GL_ONE_MINUS_SRC_ALPHA:
									alpha=1.0-color[3];
									bcolor[RGB_CORRECT(0)]+=*fp/255.0*alpha;
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0*alpha;
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0*alpha;
									bcolor[3]+=*ap*alpha;
									break;
								case GL_ONE_MINUS_SRC_COLOR:
									bcolor[RGB_CORRECT(0)]+=*fp/255.0*(1.0-color[RGB_CORRECT(0)]);
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0*(1.0-color[RGB_CORRECT(1)]);
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0*(1.0-color[RGB_CORRECT(2)]);
									bcolor[3]+=*ap*(1.0-color[3]);
									break;
								case GL_SRC_ALPHA:
									alpha=color[3];
									bcolor[RGB_CORRECT(0)]+=*fp/255.0*alpha;
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0*alpha;
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0*alpha;
									bcolor[3]+=*ap*alpha;
									break;
								case GL_SRC_COLOR:
									bcolor[RGB_CORRECT(0)]+=*fp/255.0*color[RGB_CORRECT(0)];
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0*color[RGB_CORRECT(1)];
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0*color[RGB_CORRECT(2)];
									bcolor[3]+=*ap*color[3];
									break;
								}

								tcolor=255*bcolor[RGB_CORRECT(0)];
								*fp++=min(tcolor,255);
								tcolor=255*bcolor[RGB_CORRECT(1)];
								*fp++=min(tcolor,255);
								tcolor=255*bcolor[RGB_CORRECT(2)];
								*fp++=min(tcolor,255);
								tcolor=255*bcolor[3];
								*ap++=min(tcolor,255);
							}
							if(depthmask) *zp=startz;	
							++zp;
						}
						else
						{
							++fp;++fp;++fp;++zp;++ap;
						}
itend:
						color=_color;
						color[0]+=dcolor[0];
						color[1]+=dcolor[1];
						color[2]+=dcolor[2];
						color[3]+=dcolor[3];
					}

					if(scanline+1>=et->yupper)
					{
						AET.erase(et);
						et=(ETRecordSmooth*)nextet->next;

						if(scanline+1>=nextet->yupper)
							AET.erase(nextet);
						else
							nextet->incr();						
					}
					else if(scanline+1>=nextet->yupper)
					{
						et->incr();

						et=(ETRecordSmooth*)nextet->next;
						AET.erase(nextet);
					}
					else
					{						
						et->incr();
						nextet->incr();

						et=(ETRecordSmooth*)nextet->next;					
					}
					if(et) nextet=(ETRecordSmooth*)et->next;
				}

			}
			else
				break;
		}

		//AET not empty?
		ETRecord* xx=AET.head,*yy;
		while(xx)
		{
			yy=xx;			
			xx=xx->next;
			delete yy;
		}
	}
	//ETS not clean?
}
//replace and no blend
void texfill(int ymin,int ymax)
{
	if(draw_vertex_cnt>2)
	{
		memset(ETs,0,sizeof(ETs));
		byte color[4];		
		bool depthtest=gl_context.depth_test;
		bool depthmask=gl_context.depth_mask;

		byte fogcolor[3];
		fogcolor[0]=gl_context.fog_color[0]*255;
		fogcolor[1]=gl_context.fog_color[1]*255;
		fogcolor[2]=gl_context.fog_color[2]*255;
		double* __m=gl_context.projection_matrix_stack.peek()->m;
		double m10=__m[10],m14=__m[14];
		double vz,ff,fdensity=gl_context.fog_density,fstart=gl_context.fog_start,fdelta=gl_context.fog_end-fstart;

		TEXFUNC texfunc=tex2DFuncs[gl_context.current_tex_object->filter_2d-GL_NEAREST];	
		
		//添加第一个点
		draw_vertex_buffer[draw_vertex_cnt]=draw_vertex_buffer[0];

		int i=0,testval;
		//初始化表
		for(GLVertex* it=draw_vertex_buffer,*nextit=draw_vertex_buffer+1;i<draw_vertex_cnt;++i,++it,++nextit)
		{
			if(!(testval=((int)(it->coord[1]+0.5))-((int)(nextit->coord[1]+0.5)))) continue;
			ETRecordTexture* newET=new ETRecordTexture;
			buildRecord_Texture(testval,it,nextit,it->coord[0]+0.5,it->coord[1]+0.5,it->coord[2],nextit->coord[0]+0.5,nextit->coord[1]+0.5,nextit->coord[2],newET);				
			
			//			if() --newET->yupper;

			ETs[newET->y].add(newET);
		}


		_ET AET;

		for(int scanline=ymin;scanline<=ymax;scanline++)
		{			
			if(ETs[scanline].size||AET.size)
			{	
				ETRecordTexture* et,*nextet;

				AET.merge(ETs[scanline]);

				et=(ETRecordTexture*)AET.head;
				nextet=(ETRecordTexture*)et->next;

				int spanCnt=AET.size>>1;				

				while(spanCnt--)
				{						
					int left=et->x,right=nextet->x;
					double deltaf=1.0/((double)(right-left));
					double deltaz=(nextet->z-et->z)*deltaf;
					double startz=et->z;
					double g,s,t;

					double alpha;
					double* ap=GET_ALPHA_BUFFER(scanline,left);
					byte* fp=GET_FRAME_BUFFER(scanline,left);
					double* zp=GET_Z_BUFFER(scanline,left);
					for(double f=0.0;left<right;left++,startz+=deltaz,f+=deltaf)
					{
						if(!depthtest||depthFunc(startz,*zp))
						{
							g=1.0/lerp(et->_4,nextet->_4,f);
							s=lerp(et->s_4,nextet->s_4,f)*g;
							t=lerp(et->t_4,nextet->t_4,f)*g;							

							texfunc(s,t,color);

							//fog
							if(gl_context.fog_enabled)
							{
								if((vz=m14/(startz+startz-1.0+m10)-fstart)>0.0)
								{
									switch(gl_context.fog_mode)
									{
									case GL_EXP:
										ff=exp(-fdensity*vz);
										break;
									case GL_EXP2:
										ff=exp(-fdensity*fdensity*vz*vz);
										break;
									case GL_LINEAR:
										ff=(fdelta-vz)/fdelta;
										break;
									}
									color[0]=lerp(fogcolor[0],color[0],ff);
									color[1]=lerp(fogcolor[1],color[1],ff);
									color[2]=lerp(fogcolor[2],color[2],ff);
								}
							}

							*fp++=min(color[RGB_CORRECT(TEX_CORRECT(0))],255);
							*fp++=min(color[RGB_CORRECT(TEX_CORRECT(1))],255);
							*fp++=min(color[RGB_CORRECT(TEX_CORRECT(2))],255);
							*ap++=255;//constant??								
							if(depthmask) *zp=startz;	
							++zp;
						}
						else
						{
							++fp;++fp;++fp;++zp;++ap;
						}
					}

					if(scanline+1>=et->yupper)
					{
						AET.erase(et);
						et=(ETRecordTexture*)nextet->next;

						if(scanline+1>=nextet->yupper)
							AET.erase(nextet);
						else
							nextet->incr();						
					}
					else if(scanline+1>=nextet->yupper)
					{
						et->incr();

						et=(ETRecordTexture*)nextet->next;
						AET.erase(nextet);
					}
					else
					{						
						et->incr();
						nextet->incr();

						et=(ETRecordTexture*)nextet->next;					
					}
					if(et) nextet=(ETRecordTexture*)et->next;
				}

			}
			else
				break;
		}

		//AET not empty?
		ETRecord* xx=AET.head,*yy;
		while(xx)
		{
			yy=xx;			
			xx=xx->next;
			delete yy;
		}
	}
	//ETS not clean?
}

void blendfill(int ymin,int ymax)
{
	if(draw_vertex_cnt>2)
	{
		memset(ETs,0,sizeof(ETs));
		float tex_color[4],*env_color=gl_context.tex_env_color;
		bool depthtest=gl_context.depth_test;
		bool depthmask=gl_context.depth_mask;

		float* fogcolor=gl_context.fog_color;
		double* __m=gl_context.projection_matrix_stack.peek()->m;
		double m10=__m[10],m14=__m[14];
		double vz,ff,fdensity=gl_context.fog_density,fstart=gl_context.fog_start,fdelta=gl_context.fog_end-fstart;

		TEXFUNCf texfunc=tex2DFuncsf[gl_context.current_tex_object->filter_2d-GL_NEAREST];
		

		//添加第一个点
		draw_vertex_buffer[draw_vertex_cnt]=draw_vertex_buffer[0];

		int i=0,testval;
		//初始化表
		for(GLVertex* it=draw_vertex_buffer,*nextit=draw_vertex_buffer+1;i<draw_vertex_cnt;++i,++it,++nextit)
		{
			if(!(testval=((int)(it->coord[1]+0.5))-((int)(nextit->coord[1]+0.5)))) continue;

			ETRecordBlend* newET=new ETRecordBlend;
			buildRecord_Blend(testval,it,nextit,it->coord[0]+0.5,it->coord[1]+0.5,it->coord[2],nextit->coord[0]+0.5,nextit->coord[1]+0.5,nextit->coord[2],newET);
			//			if() --newET->yupper;

			ETs[newET->y].add(newET);
		}


		_ET AET;

		for(int scanline=ymin;scanline<=ymax;scanline++)
		{			
			if(ETs[scanline].size||AET.size)
			{	
				ETRecordBlend* et,*nextet;

				AET.merge(ETs[scanline]);

				et=(ETRecordBlend*)AET.head;
				nextet=(ETRecordBlend*)et->next;

				int spanCnt=AET.size>>1;				

				while(spanCnt--)
				{						
					int left=et->x,right=nextet->x;
					double deltaf=1.0/((double)(right-left));
					double deltaz=(nextet->z-et->z)*deltaf;
					double startz=et->z;
					double g,s,t;

					double color[4],dcolor[4];
					color[0]=et->color[0];
					color[1]=et->color[1];
					color[2]=et->color[2];
					color[3]=et->color[3];
					dcolor[0]=(nextet->color[0]-et->color[0])*deltaf;
					dcolor[1]=(nextet->color[1]-et->color[1])*deltaf;
					dcolor[2]=(nextet->color[2]-et->color[2])*deltaf;
					dcolor[3]=(nextet->color[3]-et->color[3])*deltaf;

					byte tcolor;double alpha;double src_color[4];
					double* ap=GET_ALPHA_BUFFER(scanline,left);
					byte* fp=GET_FRAME_BUFFER(scanline,left);
					double* zp=GET_Z_BUFFER(scanline,left);
					for(double f=0.0;left<right;left++,startz+=deltaz,f+=deltaf)
					{
						if(!depthtest||depthFunc(startz,*zp))
						{
							g=1.0/lerp(et->_4,nextet->_4,f);
							s=lerp(et->s_4,nextet->s_4,f)*g;
							t=lerp(et->t_4,nextet->t_4,f)*g;							

							texfunc(s,t,tex_color);
							switch(gl_context.tex_env_mode)
							{
							//ignore alpha?
							case GL_MODULATE:
								tex_color[RGB_CORRECT(TEX_CORRECT(0))]*=color[RGB_CORRECT(0)];
								tex_color[RGB_CORRECT(TEX_CORRECT(1))]*=color[RGB_CORRECT(1)];
								tex_color[RGB_CORRECT(TEX_CORRECT(2))]*=color[RGB_CORRECT(2)];
								tex_color[3]*=color[3];
								break;
							case GL_DECAL:
								tex_color[RGB_CORRECT(TEX_CORRECT(0))]=lerp(color[RGB_CORRECT(0)],env_color[RGB_CORRECT(0)],tex_color[3]);
								tex_color[RGB_CORRECT(TEX_CORRECT(1))]=lerp(color[RGB_CORRECT(1)],env_color[RGB_CORRECT(1)],tex_color[3]);
								tex_color[RGB_CORRECT(TEX_CORRECT(2))]=lerp(color[RGB_CORRECT(2)],env_color[RGB_CORRECT(2)],tex_color[3]);
								tex_color[3]=color[3];
								break;
							case GL_BLEND:
								tex_color[RGB_CORRECT(TEX_CORRECT(0))]=lerp(color[RGB_CORRECT(0)],env_color[RGB_CORRECT(0)],tex_color[RGB_CORRECT(TEX_CORRECT(0))]);
								tex_color[RGB_CORRECT(TEX_CORRECT(1))]=lerp(color[RGB_CORRECT(1)],env_color[RGB_CORRECT(1)],tex_color[RGB_CORRECT(TEX_CORRECT(1))]);
								tex_color[RGB_CORRECT(TEX_CORRECT(2))]=lerp(color[RGB_CORRECT(2)],env_color[RGB_CORRECT(2)],tex_color[RGB_CORRECT(TEX_CORRECT(2))]);
								tex_color[3]*=color[3];
								break;
							}

							//fog
							if(gl_context.fog_enabled)
							{
								if((vz=m14/(startz+startz-1.0+m10)-fstart)>0.0)
								{
									switch(gl_context.fog_mode)
									{
									case GL_EXP:
										ff=exp(-fdensity*vz);
										break;
									case GL_EXP2:
										ff=exp(-fdensity*fdensity*vz*vz);
										break;
									case GL_LINEAR:
										ff=(fdelta-vz)/fdelta;
										break;
									}
									tex_color[TEX_CORRECT(0)]=lerp(fogcolor[0],tex_color[TEX_CORRECT(0)],ff);
									tex_color[TEX_CORRECT(1)]=lerp(fogcolor[1],tex_color[TEX_CORRECT(1)],ff);
									tex_color[TEX_CORRECT(2)]=lerp(fogcolor[2],tex_color[TEX_CORRECT(2)],ff);
								}								
							}

							//tex_color as src
							if(!gl_context.blend_enabled)
							{
								tcolor=255*tex_color[RGB_CORRECT(TEX_CORRECT(0))];
								*fp++=min(tcolor,255);
								tcolor=255*tex_color[RGB_CORRECT(TEX_CORRECT(1))];
								*fp++=min(tcolor,255);
								tcolor=255*tex_color[RGB_CORRECT(TEX_CORRECT(1))];
								*fp++=min(tcolor,255);
								tcolor=255*tex_color[3];
								*ap++=min(tcolor,255);								
							}
							else
							{
								double bcolor[4];
								
								switch(gl_context.blend_sfactor)
								{
								case GL_ZERO:
									bcolor[0]=bcolor[1]=bcolor[2]=bcolor[3]=0.0;
									break;
								case GL_ONE:
									bcolor[0]=tex_color[TEX_CORRECT(0)];
									bcolor[1]=tex_color[TEX_CORRECT(1)];
									bcolor[2]=tex_color[TEX_CORRECT(2)];
									bcolor[3]=tex_color[3];
									break;
								case GL_DST_ALPHA:
									alpha=*ap;
									bcolor[0]=tex_color[TEX_CORRECT(0)]*alpha;
									bcolor[1]=tex_color[TEX_CORRECT(1)]*alpha;
									bcolor[2]=tex_color[TEX_CORRECT(2)]*alpha;
									bcolor[3]=tex_color[3]*alpha;
									break;
								case GL_DST_COLOR:
									bcolor[RGB_CORRECT(0)]=tex_color[RGB_CORRECT(TEX_CORRECT(0))]*(*fp/255.0);
									bcolor[RGB_CORRECT(1)]=tex_color[RGB_CORRECT(TEX_CORRECT(1))]*(*(fp+1)/255.0);
									bcolor[RGB_CORRECT(2)]=tex_color[RGB_CORRECT(TEX_CORRECT(2))]*(*(fp+2)/255.0);
									bcolor[3]=tex_color[3]*(*ap);
									break;								
								case GL_ONE_MINUS_DST_ALPHA:
									alpha=1.0-*ap;
									bcolor[0]=tex_color[TEX_CORRECT(0)]*alpha;
									bcolor[1]=tex_color[TEX_CORRECT(1)]*alpha;
									bcolor[2]=tex_color[TEX_CORRECT(2)]*alpha;
									bcolor[3]=tex_color[3]*alpha;
									break;
								case GL_ONE_MINUS_DST_COLOR:
									bcolor[RGB_CORRECT(0)]=tex_color[RGB_CORRECT(TEX_CORRECT(0))]*(1.0-*fp/255.0);
									bcolor[RGB_CORRECT(1)]=tex_color[RGB_CORRECT(TEX_CORRECT(1))]*(1.0-*(fp+1)/255.0);
									bcolor[RGB_CORRECT(2)]=tex_color[RGB_CORRECT(TEX_CORRECT(2))]*(1.0-*(fp+2)/255.0);
									bcolor[3]=tex_color[3]*(1.0-*ap);
									break;
								case GL_ONE_MINUS_SRC_ALPHA:
									alpha=1.0-tex_color[3];
									bcolor[0]=tex_color[TEX_CORRECT(0)]*alpha;
									bcolor[1]=tex_color[TEX_CORRECT(1)]*alpha;
									bcolor[2]=tex_color[TEX_CORRECT(2)]*alpha;
									bcolor[3]=tex_color[3]*alpha;
									break;
								case GL_SRC_ALPHA:
									alpha=tex_color[3];
									bcolor[0]=tex_color[TEX_CORRECT(0)]*alpha;
									bcolor[1]=tex_color[TEX_CORRECT(1)]*alpha;
									bcolor[2]=tex_color[TEX_CORRECT(2)]*alpha;
									bcolor[3]=tex_color[3]*alpha;
									break;
								case GL_SRC_ALPHA_SATURATE:
									alpha=min(1.0-*ap,tex_color[3]);
									bcolor[0]=tex_color[TEX_CORRECT(0)]*alpha;
									bcolor[1]=tex_color[TEX_CORRECT(1)]*alpha;
									bcolor[2]=tex_color[TEX_CORRECT(2)]*alpha;
									bcolor[3]=tex_color[3];
									break;
								}
								switch(gl_context.blend_dfactor)
								{
								case GL_ZERO:
									break;
								case GL_ONE:
									bcolor[RGB_CORRECT(0)]+=*fp/255.0;
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0;
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0;
									bcolor[3]+=*ap;
									break;
								case GL_DST_ALPHA:
									alpha=*ap;
									bcolor[RGB_CORRECT(0)]+=*fp/255.0*alpha;
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0*alpha;
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0*alpha;
									bcolor[3]+=alpha*alpha;
									break;								
								case GL_ONE_MINUS_DST_ALPHA:
									alpha=1.0-*ap;
									bcolor[RGB_CORRECT(0)]+=*fp/255.0*alpha;
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0*alpha;
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0*alpha;
									bcolor[3]+=*ap*alpha;
									break;
								case GL_ONE_MINUS_SRC_ALPHA:
									alpha=1.0-tex_color[3];
									bcolor[RGB_CORRECT(0)]+=*fp/255.0*alpha;
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0*alpha;
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0*alpha;
									bcolor[3]+=*ap*alpha;
									break;
								case GL_ONE_MINUS_SRC_COLOR:
									bcolor[RGB_CORRECT(0)]+=*fp/255.0*(1.0-tex_color[RGB_CORRECT(TEX_CORRECT(0))]);
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0*(1.0-tex_color[RGB_CORRECT(TEX_CORRECT(1))]);
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0*(1.0-tex_color[RGB_CORRECT(TEX_CORRECT(2))]);
									bcolor[3]+=*ap*(1.0-tex_color[3]);
									break;
								case GL_SRC_ALPHA:
									alpha=tex_color[3];
									bcolor[RGB_CORRECT(0)]+=*fp/255.0*alpha;
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0*alpha;
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0*alpha;
									bcolor[3]+=*ap*alpha;
									break;
								case GL_SRC_COLOR:
									bcolor[RGB_CORRECT(0)]+=*fp/255.0*tex_color[RGB_CORRECT(TEX_CORRECT(0))];
									bcolor[RGB_CORRECT(1)]+=*(fp+1)/255.0*tex_color[RGB_CORRECT(TEX_CORRECT(1))];
									bcolor[RGB_CORRECT(2)]+=*(fp+2)/255.0*tex_color[RGB_CORRECT(TEX_CORRECT(2))];
									bcolor[3]+=*ap*tex_color[3];
									break;
								}

								tcolor=255*bcolor[RGB_CORRECT(0)];
								*fp++=min(tcolor,255);
								tcolor=255*bcolor[RGB_CORRECT(1)];
								*fp++=min(tcolor,255);
								tcolor=255*bcolor[RGB_CORRECT(2)];
								*fp++=min(tcolor,255);
								tcolor=255*bcolor[3];
								*ap++=min(tcolor,255);	
							}
							if(depthmask) *zp=startz;	
							++zp;
						}
						else
						{
							++fp;++fp;++fp;++zp;++ap;
						}

						color[0]+=dcolor[0];
						color[1]+=dcolor[1];
						color[2]+=dcolor[2];
						color[3]+=dcolor[3];
					}

					if(scanline+1>=et->yupper)
					{
						AET.erase(et);
						et=(ETRecordBlend*)nextet->next;

						if(scanline+1>=nextet->yupper)
							AET.erase(nextet);
						else
							nextet->incr();						
					}
					else if(scanline+1>=nextet->yupper)
					{
						et->incr();

						et=(ETRecordBlend*)nextet->next;
						AET.erase(nextet);
					}
					else
					{						
						et->incr();
						nextet->incr();

						et=(ETRecordBlend*)nextet->next;					
					}
					if(et) nextet=(ETRecordBlend*)et->next;
				}

			}
			else
				break;
		}

		//AET not empty?
		ETRecord* xx=AET.head,*yy;
		while(xx)
		{
			yy=xx;			
			xx=xx->next;
			delete yy;
		}
	}
	//ETS not clean?
}




// polygon line mode
void polyline(int ymin,int ymax)
{
	if(draw_vertex_cnt>2)
	{
		memset(ETs,0,sizeof(ETs));

		bool depthtest=gl_context.depth_test;
		bool depthmask=gl_context.depth_mask;

		void (*linefunc)(GLVertex*,GLVertex*);
		if(gl_context.line_stipple_enabled)
			linefunc=linestipple;
		else linefunc=line;

		//添加第一个点
		draw_vertex_buffer[draw_vertex_cnt]=draw_vertex_buffer[0];

		int i=0;
		//初始化表
		for(GLVertex* it=draw_vertex_buffer,*nextit=draw_vertex_buffer+1;i<draw_vertex_cnt;++i,++it,++nextit)
		{
			linefunc(it,nextit);

			if(((int)(it->coord[1]+0.5))==((int)(nextit->coord[1]+0.5))) continue;
			ETRecord* newET=new ETRecord;
			buildRecord(it->coord[0]+0.5,it->coord[1]+0.5,it->coord[2],nextit->coord[0]+0.5,nextit->coord[1]+0.5,nextit->coord[2],newET);

			//			if() --newET->yupper;

			ETs[newET->y].add(newET);
		}


		_ET AET;		

		for(int scanline=ymin;scanline<=ymax;scanline++)
		{			
			if(ETs[scanline].size||AET.size)
			{	
				ETRecord* et,*nextet;

				AET.merge(ETs[scanline]);

				et=AET.head;
				nextet=et->next;

				int spanCnt=AET.size>>1;				

				while(spanCnt--)
				{			
					int left=et->x,right=nextet->x;
					double deltaz=(nextet->z-et->z)/(right-left);
					double startz=et->z;

					double* zp=GET_Z_BUFFER(scanline,left);
					for(;left<right;left++,startz+=deltaz)
					{
						if(!depthtest||depthFunc(startz,*zp))
						{							
							if(depthmask) *zp=startz;							
						}
						++zp;					
					}

					if(scanline+1>=et->yupper)
					{
						AET.erase(et);
						et=nextet->next;

						if(scanline+1>=nextet->yupper)
							AET.erase(nextet);
						else
							nextet->incr();						
					}
					else if(scanline+1>=nextet->yupper)
					{
						et->incr();

						et=nextet->next;
						AET.erase(nextet);
					}
					else
					{						
						et->incr();
						nextet->incr();

						et=nextet->next;					
					}
					if(et) nextet=et->next;
				}

			}
			else
				break;
		}

		//AET not empty?
		ETRecord* xx=AET.head,*yy;
		while(xx)
		{
			yy=xx;			
			xx=xx->next;
			delete yy;
		}
	}
	//ETS not clean?
}
void polypoints(int ymin,int ymax)
{
	if(draw_vertex_cnt>2)
	{
		memset(ETs,0,sizeof(ETs));

		bool depthtest=gl_context.depth_test;
		bool depthmask=gl_context.depth_mask;

		//添加第一个点
		draw_vertex_buffer[draw_vertex_cnt]=draw_vertex_buffer[0];

		int i=0;
		//初始化表
		for(GLVertex* it=draw_vertex_buffer,*nextit=draw_vertex_buffer+1;i<draw_vertex_cnt;++i,++it,++nextit)
		{
			glbufSetPixelv(it->coord[0],it->coord[1],it->coord[2],it->color);

			if(((int)(it->coord[1]+0.5))==((int)(nextit->coord[1]+0.5))) continue;
			ETRecord* newET=new ETRecord;
			buildRecord(it->coord[0]+0.5,it->coord[1]+0.5,it->coord[2],nextit->coord[0]+0.5,nextit->coord[1]+0.5,nextit->coord[2],newET);

			//			if() --newET->yupper;

			ETs[newET->y].add(newET);
		}


		_ET AET;		

		for(int scanline=ymin;scanline<=ymax;scanline++)
		{			
			if(ETs[scanline].size||AET.size)
			{	
				ETRecord* et,*nextet;

				AET.merge(ETs[scanline]);

				et=AET.head;
				nextet=et->next;

				int spanCnt=AET.size>>1;				

				while(spanCnt--)
				{			
					int left=et->x,right=nextet->x;
					double deltaz=(nextet->z-et->z)/(right-left);
					double startz=et->z;

					double* zp=GET_Z_BUFFER(scanline,left);
					for(;left<right;left++,startz+=deltaz)
					{
						if(!depthtest||depthFunc(startz,*zp))
						{							
							if(depthmask) *zp=startz;							
						}
						++zp;					
					}

					if(scanline+1>=et->yupper)
					{
						AET.erase(et);
						et=nextet->next;

						if(scanline+1>=nextet->yupper)
							AET.erase(nextet);
						else
							nextet->incr();						
					}
					else if(scanline+1>=nextet->yupper)
					{
						et->incr();

						et=nextet->next;
						AET.erase(nextet);
					}
					else
					{						
						et->incr();
						nextet->incr();

						et=nextet->next;					
					}
					if(et) nextet=et->next;
				}

			}
			else
				break;
		}

		//AET not empty?
		ETRecord* xx=AET.head,*yy;
		while(xx)
		{
			yy=xx;			
			xx=xx->next;
			delete yy;
		}
	}
	//ETS not clean?
}
/*
int discretePixels[MAX_FRAMEBUF_SIZE];
// y xL xR
int Harray[MAX_HSLS_CNT][3];
void calcDiscretePixels()
{
	if(draw_vertex_cnt>2)
	{
		double* color=draw_vertex_buffer[draw_vertex_cnt-1].color;		
		BresInfo bresinfo;

		//添加第一个点
		draw_vertex_buffer[draw_vertex_cnt]=draw_vertex_buffer[0];

		int i=0;
		//初始化表
		for(GLVertex* it=draw_vertex_buffer,*nextit=draw_vertex_buffer+1;i<draw_vertex_cnt;++i,++it,++nextit)
		{
			BUILD_BRES(it->coord[0]+0.5,it->coord[1]+0.5,it->coord[2],nextit->coord[0]+0.5,nextit->coord[1]+0.5,nextit->coord[2],bresinfo);
			
			if(bresinfo.major_axis==0)
			{
				for(x=bresinfo.x,y=bresinfo.y,z=bresinfo.z;x<=bresinfo.major_end;++x,z+=bresinfo.dz)
				{
					glbufSetPixelv(x,y,z,color);
					if(bresinfo.error>=0)
					{
						bresinfo.error+=bresinfo.incr1;
						y+=bresinfo.i1;
					}
					else
					{
						bresinfo.error+=bresinfo.incr0;
						y+=bresinfo.i0;
					}
				}
			}
			else
			{
				for(x=bresinfo.x,y=bresinfo.y,z=bresinfo.z;y<=bresinfo.major_end;++y,z+=bresinfo.dz)
				{
					glbufSetPixelv(x,y,z,color);
					if(bresinfo.error>=0)
					{
						bresinfo.error+=bresinfo.incr1;
						x+=bresinfo.i1;
					}
					else
					{
						bresinfo.error+=bresinfo.incr0;
						x+=bresinfo.i0;
					}
				}
			}


			ETs[newET->bresinfo.y].add(newET);
		}
	}
}

void calcHSLS()
{

}

void buildNbuckets()
{

}

void fill(int ymin,int ymax)
{

}
*/