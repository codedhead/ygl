#include "../include/ygl.h"
#include "../include/draw.h"
#include "../include/gltypes.h"
#include "../include/buf.h"
#include "../include/raster.h"
#include "../include/clip.h"
#include "../include/light.h"

extern GLContext gl_context;

GLVertex draw_vertex_buffer1[GPU_MAX_VERTEX_BUF];
GLVertex draw_vertex_buffer2[GPU_MAX_VERTEX_BUF];
GLVertex* draw_vertex_buffer=draw_vertex_buffer1;
int draw_vertex_cnt;

#define WHICHFACE(x) ((x==gl_context.front_face)?GL_FRONT:GL_BACK)

void drawPoints()
{
	GLVertex* it=draw_vertex_buffer;
	for(int i=0;i<draw_vertex_cnt;++i,++it)
	{
		gl_context.mapToViewPort(it);
		glbufSetPixelv(it->coord[0],it->coord[1],it->coord[2],it->color);
	}
}

void drawLines()
{
	draw_vertex_cnt>>=1;if(!draw_vertex_cnt) return;

	GLVertex* it=draw_vertex_buffer;

	if(gl_context.line_stipple_enabled)
	{
		for(int i=0;i<draw_vertex_cnt;++i)
		{
			gl_context.mapToViewPort(it);
			gl_context.mapToViewPort(it+1);
			RESET_LINE_STIPPLE;
			linestipple(it,it+1);
			++it;++it;
		}
	}
	else
	{
		for(int i=0;i<draw_vertex_cnt;++i)
		{
			gl_context.mapToViewPort(it);
			gl_context.mapToViewPort(it+1);
			line(it,it+1);
			++it;++it;
		}
	}
}

void drawPolygons(int face)
{
	//which face???
	if(draw_vertex_cnt<=2) return;

	int pmode=face==GL_FRONT?gl_context.front_polygon_mode:gl_context.back_polygon_mode;

	GLVertex* it=draw_vertex_buffer;
	gl_context.mapToViewPort(it);
	double ymin=it->coord[1],ymax=it->coord[1];
	++it;

	for(int i=1;i<draw_vertex_cnt;++i,++it)
	{
		gl_context.mapToViewPort(it);
		if(it->coord[1]>ymax) ymax=it->coord[1];
		if(it->coord[1]<ymin) ymin=it->coord[1];
	}


	switch(pmode)
	{
	case GL_POINT:
		polypoints(ymin+0.5,ymax+0.5);
		break;
	case GL_LINE:
		polyline(ymin+0.5,ymax+0.5);
		break;
	case GL_FILL:
		if(gl_context.texture_enabled)
		{
			if(gl_context.tex_env_mode==GL_REPLACE&&!gl_context.blend_enabled)
				texfill(ymin+0.5,ymax+0.5);
			else blendfill(ymin+0.5,ymax+0.5);
		}
		else if(gl_context.shading_mode==GL_FLAT)
			fill(ymin+0.5,ymax+0.5);
		else if(gl_context.shading_mode==GL_SMOOTH)
			smoothfill(ymin+0.5,ymax+0.5);	
		break;
	}	
}

GLVertex ___v[5];

/* interact with vertexs in gl_context */
/*
front ccw,cull front  ccw
cull back   cw
front cw ,cull front cw
cull back  ccw

other cull all
*/
void draw(int type,GLVertex* vertexs,int pcnt)
{
	Matrix44* mat=gl_context.projection_matrix_stack.peek();
	GLVertex* it;

	double faceval;int facing,determine=gl_context.face_cull_determine;
	GLVertex* p1,*p2,*p3,*p4;

	int i,cnt=0,face;

	for(i=0,it=vertexs;i<pcnt;++i,++it)
		mat->applyToVector(it->coord);	

	it=vertexs;

	switch(type)
	{
	case GL_POINTS:
		doClip(type,vertexs,pcnt);
		drawPoints();
		break;
	case GL_LINES:
	case GL_LINE_STRIP:
	case GL_LINE_LOOP:
		doClip(type,vertexs,pcnt);
		drawLines();
		break;	
	case GL_POLYGON:
		if(gl_context.face_cull_mode==GL_FRONT_AND_BACK) return;

		faceval=(vertexs[pcnt-1].coord[0]*vertexs[0].coord[1]-vertexs[0].coord[0]*vertexs[pcnt-1].coord[1])/(vertexs[pcnt-1].coord[3]*vertexs[0].coord[3]);
		for(i=0;i<pcnt-1;++i)
			faceval+=(vertexs[i].coord[0]*vertexs[i+1].coord[1]-vertexs[i+1].coord[0]*vertexs[i].coord[1])/(vertexs[i].coord[3]*vertexs[i+1].coord[3]);

		if(faceval==0.0) break;
		facing=faceval>0.0?GL_CCW:GL_CW;

		face=WHICHFACE(facing);

		if(gl_context.two_side)
		{
			int coloroffset=face==GL_FRONT?0:4;
			for(i=0;i<pcnt;++i)
				vertexs[i].color=vertexs[i].front_color+coloroffset;
		}

		if(!gl_context.face_cull_enabled||facing!=determine)
		{
			doClip(GL_POLYGON,vertexs,pcnt);
			drawPolygons(face);
		}

		break;
	case GL_TRIANGLES:	
		if(gl_context.face_cull_mode==GL_FRONT_AND_BACK) return;

		cnt=pcnt/3;

		while(cnt--)
		{
			p1=vertexs;p2=vertexs+1;p3=vertexs+2;
			//faceval=(p2->coord[0]-p1->coord[0])*(p3->coord[1]-p1->coord[1]) - (p2->coord[1]-p1->coord[1])*(p3->coord[0]-p1->coord[0]);

			faceval=(p1->coord[0]*p2->coord[1]-p2->coord[0]*p1->coord[1])/(p1->coord[3]*p2->coord[3])
				+(p2->coord[0]*p3->coord[1]-p3->coord[0]*p2->coord[1])/(p2->coord[3]*p3->coord[3])
				+(p3->coord[0]*p1->coord[1]-p1->coord[0]*p3->coord[1])/(p3->coord[3]*p1->coord[3]);

			if(faceval==0.0) continue;
			facing=faceval>0.0?GL_CCW:GL_CW;

			face=WHICHFACE(facing);

			if(gl_context.two_side)
			{
				int coloroffset=face==GL_FRONT?0:4;			
				p1->color=p1->front_color+coloroffset;
				p2->color=p2->front_color+coloroffset;
				p3->color=p3->front_color+coloroffset;

			}

			if(!gl_context.face_cull_enabled||facing!=determine)
			{
				doClip(GL_POLYGON,vertexs,3);
				drawPolygons(face);
			}
			vertexs+=3;
		}
		break;
	case GL_TRIANGLE_STRIP:
		if(gl_context.face_cull_mode==GL_FRONT_AND_BACK) return;
		cnt=pcnt-2;
		if(cnt>0)
		{
			bool bOdd=true;
			p1=___v;p2=___v+1;p3=___v+2;
			while(cnt--)
			{
				if(bOdd)
				{
					___v[0]=*(it++);
					___v[1]=*it;
					___v[2]=*(it+1);
				}
				else
				{
					___v[1]=*(it++);
					___v[0]=*it;
					___v[2]=*(it+1);
				}

				faceval=(p1->coord[0]*p2->coord[1]-p2->coord[0]*p1->coord[1])/(p1->coord[3]*p2->coord[3])
					+(p2->coord[0]*p3->coord[1]-p3->coord[0]*p2->coord[1])/(p2->coord[3]*p3->coord[3])
					+(p3->coord[0]*p1->coord[1]-p1->coord[0]*p3->coord[1])/(p3->coord[3]*p1->coord[3]);				if(faceval==0.0) continue;
				facing=faceval>0.0?GL_CCW:GL_CW;

				face=WHICHFACE(facing);

				if(gl_context.two_side)
				{
					int coloroffset=face==GL_FRONT?0:4;			
					p1->color=p1->front_color+coloroffset;
					p2->color=p2->front_color+coloroffset;
					p3->color=p3->front_color+coloroffset;					
				}

				if(!gl_context.face_cull_enabled||facing!=determine)
				{
					doClip(GL_POLYGON,___v,3);
					drawPolygons(face);
				}

				bOdd=!bOdd;
			}			
		}
		break;
	case GL_TRIANGLE_FAN:
		if(gl_context.face_cull_mode==GL_FRONT_AND_BACK) return;
		cnt=pcnt-2;
		if(cnt>0)
		{
			GLVertex firstvertex=*it++;
			p1=___v;p2=___v+1;p3=___v+2;
			while(cnt--)
			{				
				___v[0]=firstvertex;
				___v[1]=*it;
				___v[2]=*(++it);			

				faceval=(p1->coord[0]*p2->coord[1]-p2->coord[0]*p1->coord[1])/(p1->coord[3]*p2->coord[3])
					+(p2->coord[0]*p3->coord[1]-p3->coord[0]*p2->coord[1])/(p2->coord[3]*p3->coord[3])
					+(p3->coord[0]*p1->coord[1]-p1->coord[0]*p3->coord[1])/(p3->coord[3]*p1->coord[3]);				if(faceval==0.0) continue;
				facing=faceval>0.0?GL_CCW:GL_CW;

				face=WHICHFACE(facing);

				if(gl_context.two_side)
				{
					int coloroffset=face==GL_FRONT?0:4;			
					p1->color=p1->front_color+coloroffset;
					p2->color=p2->front_color+coloroffset;
					p3->color=p3->front_color+coloroffset;					
				}

				if(!gl_context.face_cull_enabled||facing!=determine)
				{
					doClip(GL_POLYGON,___v,3);
					drawPolygons(face);
				}			
			}			
		}
		break;
	case GL_QUADS:
		if(gl_context.face_cull_mode==GL_FRONT_AND_BACK) return;
		cnt=pcnt>>2;
		while(cnt--)
		{
			p1=vertexs;p2=vertexs+1;p3=vertexs+2;p4=vertexs+3;

			faceval=(p1->coord[0]*p2->coord[1]-p2->coord[0]*p1->coord[1])/(p1->coord[3]*p2->coord[3])
				+(p2->coord[0]*p3->coord[1]-p3->coord[0]*p2->coord[1])/(p2->coord[3]*p3->coord[3])
				+(p3->coord[0]*p4->coord[1]-p4->coord[0]*p3->coord[1])/(p3->coord[3]*p4->coord[3])
				+(p4->coord[0]*p1->coord[1]-p1->coord[0]*p4->coord[1])/(p4->coord[3]*p1->coord[3]);
			if(faceval==0.0) continue;
			facing=faceval>0.0?GL_CCW:GL_CW;

			face=WHICHFACE(facing);

			if(gl_context.two_side)
			{
				int coloroffset=face==GL_FRONT?0:4;			
				p1->color=p1->front_color+coloroffset;
				p2->color=p2->front_color+coloroffset;
				p3->color=p3->front_color+coloroffset;					
				p4->color=p4->front_color+coloroffset;
			}

			if(!gl_context.face_cull_enabled||facing!=determine)
			{
				doClip(GL_POLYGON,vertexs,4);
				drawPolygons(face);
			}	
			vertexs+=4;
		}
		break;
	case GL_QUAD_STRIP:
		if(gl_context.face_cull_mode==GL_FRONT_AND_BACK) return;
		cnt=(pcnt>>1)-1;
		p1=___v;p2=___v+1;p3=___v+2;p4=___v+3;
		if(cnt>0)
		{
			while(cnt--)
			{	
				___v[0]=*(it++);
				___v[1]=*(it++);
				___v[2]=*(it+1);
				___v[3]=*(it);

				faceval=(p1->coord[0]*p2->coord[1]-p2->coord[0]*p1->coord[1])/(p1->coord[3]*p2->coord[3])
					+(p2->coord[0]*p3->coord[1]-p3->coord[0]*p2->coord[1])/(p2->coord[3]*p3->coord[3])
					+(p3->coord[0]*p4->coord[1]-p4->coord[0]*p3->coord[1])/(p3->coord[3]*p4->coord[3])
					+(p4->coord[0]*p1->coord[1]-p1->coord[0]*p4->coord[1])/(p4->coord[3]*p1->coord[3]);

				if(faceval==0.0) continue;
				facing=faceval>0.0?GL_CCW:GL_CW;
				face=WHICHFACE(facing);

				if(gl_context.two_side)
				{
					int coloroffset=face==GL_FRONT?0:4;			
					p1->color=p1->front_color+coloroffset;
					p2->color=p2->front_color+coloroffset;
					p3->color=p3->front_color+coloroffset;					
					p4->color=p4->front_color+coloroffset;
				}

				if(!gl_context.face_cull_enabled||facing!=determine)
				{
					doClip(GL_POLYGON,___v,4);
					drawPolygons(face);
				}	
			}			
		}
		break;
	}
}

