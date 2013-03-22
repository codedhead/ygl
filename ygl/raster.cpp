#include "raster.h"
#include "yglstructs.h"
#include "buffer.h"
#include "global.h"
#include "yglconstants.h"
#include "texture.h"
#include "glcontext.h"

//#define USE_MEM_FACTORY

#ifdef USE_MEM_FACTORY
#include "memory.h"
#endif
namespace ygl{
namespace raster
{

#define DECLARE_FACTORY(cla_name) TFactory<cla_name> Factory_##cla_name
	struct ETable;
	struct ETRecord;
	struct ETRecord_PColor;
	struct ETRecord_PColor_PTex;

#ifdef USE_MEM_FACTORY
	extern DECLARE_FACTORY(ETable);
	extern DECLARE_FACTORY(ETRecord);
	extern DECLARE_FACTORY(ETRecord_PColor);
	extern DECLARE_FACTORY(ETRecord_PColor_PTex);
	
#define NEW_ETABLE (Factory_ETable.alloc())
#define DELETE_ETABLE(et) Factory_ETable.recycle(et)

#define NEW_ETRECORD (Factory_ETRecord.alloc())
#define DELETE_ETRECORD(rec) (rec)->recycle_me()

#define NEW_ETRECORD_PSMOOTH (Factory_ETRecordPSmooth.alloc())
#define NEW_ETRECORD_PSMOOTHTEX (Factory_ETRecordPSmoothTex.alloc())

#else

#define NEW_ETABLE (new ETable)
#define DELETE_ETABLE(et) delete (et)

#define NEW_ETRECORD (new ETRecord)
#define DELETE_ETRECORD(rec) delete (rec)

#define NEW_ETRECORD_PSMOOTH (new ETRecord_PColor)
#define NEW_ETRECORD_PSMOOTHTEX (new ETRecord_PColor_PTex)

#endif
	

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

	// no support for smooth, tex line
	/*
	// no perspective correction
	void line_smooth(Vertex* v)
	{
#define INC_LINE_ATRRIB \
		z+=dz;\
		color[0]+=d_color[0];\
		color[1]+=d_color[1];\
		color[2]+=d_color[2];\
		color[3]+=d_color[3];

#define SETUP_LINE_ATTRIB_AND_DATTRIB \
		z=Z_OF(v1->p);\
		dz=_dd*(Z_OF(v2->p)-z);\
		color[0]=YGL_COLOR_F2IF(v1->col_front_pri[0]);\
		color[1]=YGL_COLOR_F2IF(v1->col_front_pri[1]);\
		color[2]=YGL_COLOR_F2IF(v1->col_front_pri[2]);\
		color[3]=YGL_COLOR_F2IF(v1->col_front_pri[3]);\
		d_color[0]=(YGL_COLOR_F2IF(v2->col_front_pri[0])-color[0])*_dd;\
		d_color[1]=(YGL_COLOR_F2IF(v2->col_front_pri[1])-color[1])*_dd;\
		d_color[2]=(YGL_COLOR_F2IF(v2->col_front_pri[2])-color[2])*_dd;\
		d_color[3]=(YGL_COLOR_F2IF(v2->col_front_pri[3])-color[3])*_dd;

		buffer::plot_ubytef_func plot_func=(buffer::z_test_mask==FLOAT_ALWAYS?
			buffer::plot_ubytef_no_ztest:
			buffer::plot_ubytef);

		// draw from v1->v2
		Vertex* v1=v,*v2=v+1;
		// float [0,255.0]
		GLfloat color[4],d_color[4],z,dz;
		GLfloat _dd;

		int x0=ROUND(v[0].p[0]),
			y0=ROUND(v[0].p[1]),
			x1=ROUND(v[1].p[0]),
			y1=ROUND(v[1].p[1]);

		// trival case
		if(x0==x1)
		{
			if(y1<y0) // start from y1
			{
				v1=v+1;v2=v;
				XOR_SWAP(y0,y1);
			}

			_dd=1.f/(y1-y0);

			SETUP_LINE_ATTRIB_AND_DATTRIB;

			for(int y=y0;y<=y1;++y)
			{
				plot_func(x0,y,z,color);
				INC_LINE_ATRRIB;
			}

			return;
		}
		else if(y0==y1)
		{
			if(x1<x0)
			{
				v1=v+1;v2=v;
				XOR_SWAP(x0,x1);
			}

			_dd=1.f/(x1-x0);

			SETUP_LINE_ATTRIB_AND_DATTRIB;

			for(int x=x0;x<=x1;++x)
			{
				plot_func(x,y0,z,color);
				INC_LINE_ATRRIB;
			}

			return;
		}

		int steep=abs(x1-x0)-abs(y0-y1);
		if(steep==0)
		{
			if(x1<x0)
			{
				v1=v+1;v2=v;

				XOR_SWAP(x0,x1);
				XOR_SWAP(y0,y1);
			}

			_dd=1.f/(x1-x0);
			SETUP_LINE_ATTRIB_AND_DATTRIB;

			if(y0<y1)
			{
				for(int x=x0,y=y0;x<=x1;++x,++y)
				{
					plot_func(x,y,z,color);
					INC_LINE_ATRRIB;
				}
			}
			else
			{
				for(int x=x0,y=y0;x<=x1;++x,--y)
				{
					plot_func(x,y,z,color);
					INC_LINE_ATRRIB;
				}
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
				v1=v+1;v2=v;

				XOR_SWAP(x0,x1);
				XOR_SWAP(y0,y1);
			}

			const int inc_y=y0<y1?1:-1;

			// x0<=x1, dx>0
			int dx=x1-x0,dy=abs(y1-y0); // check!!
			int D=dy+dy-dx;
			int delta1=dy+dy,delta2=dy+dy-dx-dx;

			_dd=1.f/dx;
			SETUP_LINE_ATTRIB_AND_DATTRIB;

			for(int x=x0,y=y0;x<=x1;++x)
			{
				if(D<=0) // use bottom
				{
					D+=delta1;
					if(steep<0) plot_func(y,x,z,color);
					else plot_func(x,y,z,color);
				}
				else
				{
					D+=delta2;
					if(steep<0) plot_func((y+=inc_y),x,z,color);
					else plot_func(x,(y+=inc_y),z,color);
				}

				INC_LINE_ATRRIB;
			}
		}

	}
	*/

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
			ymin=ROUND(Y_OF(v1->p));
			ymax=ROUND(Y_OF(v2->p));
			curx=ROUND(X_OF(v1->p));
			z=YGL_DEPTH_BUFFER_MAX_RES*Z_OF(v1->p);

			GLfloat _dy=1.f/((GLfloat)(ymax-ymin));
			dx=((GLfloat)(ROUND(X_OF(v2->p))-curx))*_dy;
			dz=(YGL_DEPTH_BUFFER_MAX_RES*Z_OF(v2->p)-z)*_dy;

			//int dx=,dy=ymax-ymin;
		}

		virtual void fill_span(GLint y,ETRecord* that,/*GLubyte* cbuf,GLuint* zbuf,*/cbuf_type* flat_color,TextureObject* texobj)
		{
			int left=ROUND(this->curx),right=ROUND(that->curx);
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
	
	template <size_t pAttribNum>
	struct ETRecord_Persp : public ETRecord
	{
		GLint att_num;
		GLfloat att_w[pAttribNum],d_att_w_dy[pAttribNum],
			z,dz,_w,d_w; // dy implicit

		//ETRecord_Persp():att_num(AttribNum){}

		//start from v1
		virtual void init_attribs(Vertex* v1,Vertex* v2,GLfloat _dy,GLfloat theother_w)=0;
		virtual void fragment(GLfloat w,GLfloat* att_p_w,cbuf_type* flat_color,TextureObject* texobj,cbuf_type* res_color)=0;

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

			ymin=ROUND(Y_OF(v1->p));
			ymax=ROUND(Y_OF(v2->p));
			curx=ROUND(X_OF(v1->p));
			z=YGL_DEPTH_BUFFER_MAX_RES*Z_OF(v1->p);

			_dy=1.f/((GLfloat)(ymax-ymin));
			dx=((GLfloat)(ROUND(X_OF(v2->p))-curx))*_dy;
			dz=(YGL_DEPTH_BUFFER_MAX_RES*Z_OF(v2->p)-z)*_dy;

			// already reciprocal
			_w=W_OF(v1->p);theother_w=W_OF(v2->p);
			d_w=(theother_w-_w)*_dy;

			init_attribs(v1,v2,_dy,theother_w);
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
			ETRecord_Persp<pAttribNum>* that=(ETRecord_Persp<pAttribNum>*)spanto;
			int left=ROUND(this->curx),right=ROUND(that->curx);

			GLfloat _dx=1.f/((GLfloat)(right-left));
			GLfloat span_w=this->_w,d_w_dx=(that->_w-span_w)*_dx;
			GLfloat spanz=this->z,d_spanz=(that->z-spanz)*_dx;

			GLfloat span_att_w[pAttribNum],d_att_w_dx[pAttribNum];
			
			for(int i=0;i<pAttribNum;++i)
			{
				span_att_w[i]=this->att_w[i];
				d_att_w_dx[i]=(that->att_w[i]-span_att_w[i])*_dx;
			}

			cbuf_type fragment_color[4];
			
// 			cbuf_type* cbuf=buffer::framebuf_ptr(left,y);
// 			zbuf_type* zbuf=buffer::depthbuf_ptr(left,y);

			// fill span
			for(int x=left;x<right;++x)
			{
				fragment(1.f/span_w,span_att_w,flat_color,texobj,fragment_color);
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
	
	template <size_t npAttribNum>
	struct ETRecord_NoPersp : public ETRecord
	{
		//GLint att_num;
		GLfloat att[npAttribNum],d_att_dy[npAttribNum],
			z,dz; // dy implicit

		//ETRecordPerspCorrection():att_num(AttribNum){}

		//start from v1
		virtual void init_attribs(Vertex* v1,Vertex* v2,GLfloat _dy)=0;
		virtual void fragment(GLfloat* att_np,cbuf_type* flat_color,TextureObject* texobj,cbuf_type* res_color)=0;

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

			ymin=ROUND(Y_OF(v1->p));
			ymax=ROUND(Y_OF(v2->p));
			curx=ROUND(X_OF(v1->p));
			z=YGL_DEPTH_BUFFER_MAX_RES*Z_OF(v1->p);

			_dy=1.f/((GLfloat)(ymax-ymin));
			dx=((GLfloat)(ROUND(X_OF(v2->p))-curx))*_dy;
			dz=(YGL_DEPTH_BUFFER_MAX_RES*Z_OF(v2->p)-z)*_dy;

			init_attribs(v1,v2,_dy/*theother_w*/);
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
			ETRecord_NoPersp<npAttribNum>* that=(ETRecord_NoPersp<npAttribNum>*)spanto;
			int left=ROUND(this->curx),right=ROUND(that->curx);

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
				fragment(span_att,flat_color,texobj,fragment_color);
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

	template <size_t npAttribNum,size_t pAttribNum>
	struct ETRecord_Mix : public ETRecord
	{
		//GLint att_num;
		GLfloat att_w[pAttribNum],d_att_w_dy[pAttribNum],
			att[npAttribNum],d_att_dy[npAttribNum],
			z,dz,_w,d_w; // dy implicit

		//ETRecord_Persp():att_num(AttribNum){}

		//start from v1
		virtual void init_attribs(Vertex* v1,Vertex* v2,GLfloat _dy,GLfloat theother_w)=0;
		virtual void fragment(GLfloat w,GLfloat* att_np,GLfloat* att_p_w,cbuf_type* flat_color,TextureObject* texobj,cbuf_type* res_color)=0;

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

			ymin=ROUND(Y_OF(v1->p));
			ymax=ROUND(Y_OF(v2->p));
			curx=ROUND(X_OF(v1->p));
			z=YGL_DEPTH_BUFFER_MAX_RES*Z_OF(v1->p);

			_dy=1.f/((GLfloat)(ymax-ymin));
			dx=((GLfloat)(ROUND(X_OF(v2->p))-curx))*_dy;
			dz=(YGL_DEPTH_BUFFER_MAX_RES*Z_OF(v2->p)-z)*_dy;

			// already reciprocal
			_w=W_OF(v1->p);theother_w=W_OF(v2->p);
			d_w=(theother_w-_w)*_dy;

			init_attribs(v1,v2,_dy,theother_w);
		}

		/*virtual */void incre()
		{
			curx+=dx;
			z+=dz;
			_w+=d_w;

			for(int i=0;i<npAttribNum;++i)
				att[i]+=d_att_dy[i];
			for(int i=0;i<pAttribNum;++i)
				att_w[i]+=d_att_w_dy[i];
		}

		/*virtual */void fill_span(GLint y,ETRecord* spanto,cbuf_type* flat_color,TextureObject* texobj)
		{
			ETRecord_Mix<npAttribNum,pAttribNum>* that=(ETRecord_Mix<npAttribNum,pAttribNum>*)spanto;
			int left=ROUND(this->curx),right=ROUND(that->curx);

			GLfloat _dx=1.f/((GLfloat)(right-left));
			GLfloat span_w=this->_w,d_w_dx=(that->_w-span_w)*_dx;
			GLfloat spanz=this->z,d_spanz=(that->z-spanz)*_dx;

			GLfloat span_att_w[pAttribNum],d_att_w_dx[pAttribNum],
				span_att[npAttribNum],d_att_dx[npAttribNum];
			
			for(int i=0;i<npAttribNum;++i)
			{
				span_att[i]=this->att[i];
				d_att_dx[i]=(that->att[i]-span_att[i])*_dx;
			}
			for(int i=0;i<pAttribNum;++i)
			{
				span_att_w[i]=this->att_w[i];
				d_att_w_dx[i]=(that->att_w[i]-span_att_w[i])*_dx;
			}

			cbuf_type fragment_color[4];
			// fill span
			for(int x=left;x<right;++x)
			{
				fragment(1.f/span_w,span_att_w,flat_color,texobj,fragment_color);
				buffer::plot(x,y,(zbuf_type)spanz,fragment_color);

				//++zbuf;cbuf+=3;

				for(int i=0;i<npAttribNum;++i)
				{
					span_att[i]+=d_att_dx[i];
				}
				for(int i=0;i<pAttribNum;++i)
				{
					span_att_w[i]+=d_att_w_dx[i];
				}
				span_w+=d_w_dx;
				spanz+=d_spanz;
			}
		}
	};
	
	struct ETRecord_Color: public ETRecord_NoPersp<4>
	{
#define ___R 0
#define ___G 1
#define ___B 2
#define ___A 3

#ifdef USE_MEM_FACTORY
		virtual void recycle_me()
		{
			Factory_ETRecordSmooth.recycle(this);
		}
#ifdef _YGL_DEBUG_
		void class_name(){printf("ETRecordSmooth:\t");}
#endif
#endif
		void init_attribs(Vertex* v1,Vertex* v2,GLfloat _dy)
		{
			SETUP_ATTRIB_NOP(___R,YGL_COLOR_F2IF(v1->col_front_pri[0]));
			SETUP_ATTRIB_NOP(___G,YGL_COLOR_F2IF(v1->col_front_pri[1]));
			SETUP_ATTRIB_NOP(___B,YGL_COLOR_F2IF(v1->col_front_pri[2]));
			SETUP_ATTRIB_NOP(___A,YGL_COLOR_F2IF(v1->col_front_pri[3]));

			SETUP_DATTRIB_NOP(___R,YGL_COLOR_F2IF(v2->col_front_pri[0]));
			SETUP_DATTRIB_NOP(___G,YGL_COLOR_F2IF(v2->col_front_pri[1]));
			SETUP_DATTRIB_NOP(___B,YGL_COLOR_F2IF(v2->col_front_pri[2]));
			SETUP_DATTRIB_NOP(___A,YGL_COLOR_F2IF(v2->col_front_pri[3]));
		}
		void fragment(GLfloat* att_np,cbuf_type* flat_color,TextureObject* texobj,cbuf_type* res_color)
		{
			res_color[0]=(cbuf_type)GET_ATTRIB_NOP(___R);
			res_color[1]=(cbuf_type)GET_ATTRIB_NOP(___G);
			res_color[2]=(cbuf_type)GET_ATTRIB_NOP(___B);
			res_color[3]=(cbuf_type)GET_ATTRIB_NOP(___A);
			// Alpha??
		}
#undef ___R
#undef ___G
#undef ___B
#undef ___A
	};
	struct ETRecord_PColor: public ETRecord_Persp<4>
	{
#define ___R 0
#define ___G 1
#define ___B 2
#define ___A 3

#ifdef USE_MEM_FACTORY
		virtual void recycle_me()
		{
			Factory_ETRecordPSmooth.recycle(this);
		}
#ifdef _YGL_DEBUG_
		void class_name(){printf("ETRecordPSmooth:\t");}
#endif
#endif
		void init_attribs(Vertex* v1,Vertex* v2,GLfloat _dy,GLfloat theother_w)
		{
			SETUP_ATTRIB_P(___R,YGL_COLOR_F2IF(v1->col_front_pri[0]));
			SETUP_ATTRIB_P(___G,YGL_COLOR_F2IF(v1->col_front_pri[1]));
			SETUP_ATTRIB_P(___B,YGL_COLOR_F2IF(v1->col_front_pri[2]));
			SETUP_ATTRIB_P(___A,YGL_COLOR_F2IF(v1->col_front_pri[3]));
			
			SETUP_DATTRIB_P(___R,YGL_COLOR_F2IF(v2->col_front_pri[0]));
			SETUP_DATTRIB_P(___G,YGL_COLOR_F2IF(v2->col_front_pri[1]));
			SETUP_DATTRIB_P(___B,YGL_COLOR_F2IF(v2->col_front_pri[2]));
			SETUP_DATTRIB_P(___A,YGL_COLOR_F2IF(v2->col_front_pri[3]));
		}
		void fragment(GLfloat w,GLfloat* att_p_w,cbuf_type* flat_color,TextureObject* texobj,cbuf_type* res_color)
		{
			res_color[0]=(cbuf_type)GET_ATTRIB_P(___R);
			res_color[1]=(cbuf_type)GET_ATTRIB_P(___G);
			res_color[2]=(cbuf_type)GET_ATTRIB_P(___B);
			res_color[3]=(cbuf_type)GET_ATTRIB_P(___A);
			// Alpha??
		}
#undef ___R
#undef ___G
#undef ___B
#undef ___A
	};

	struct ETRecord_PColor_PTex: public ETRecord_Persp<6>
	{
#define ___R 0
#define ___G 1
#define ___B 2
#define ___A 3
#define ___S 4
#define ___T 5

#ifdef USE_MEM_FACTORY
		virtual void recycle_me()
		{
			Factory_ETRecordPSmoothTex.recycle(this);
		}
#ifdef _YGL_DEBUG_
		void class_name(){printf("ETRecordPSmoothTex:\t");}
#endif
#endif
		void init_attribs(Vertex* v1,Vertex* v2,GLfloat _dy,GLfloat theother_w)
		{
			SETUP_ATTRIB_P(___R,YGL_COLOR_F2IF(v1->col_front_pri[0]));
			SETUP_ATTRIB_P(___G,YGL_COLOR_F2IF(v1->col_front_pri[1]));
			SETUP_ATTRIB_P(___B,YGL_COLOR_F2IF(v1->col_front_pri[2]));
			SETUP_ATTRIB_P(___A,YGL_COLOR_F2IF(v1->col_front_pri[3]));

			SETUP_DATTRIB_P(___R,YGL_COLOR_F2IF(v2->col_front_pri[0]));
			SETUP_DATTRIB_P(___G,YGL_COLOR_F2IF(v2->col_front_pri[1]));
			SETUP_DATTRIB_P(___B,YGL_COLOR_F2IF(v2->col_front_pri[2]));
			SETUP_DATTRIB_P(___A,YGL_COLOR_F2IF(v2->col_front_pri[3]));

			SETUP_ATTRIB_P(___S,v1->tex_coords[0]);
			SETUP_ATTRIB_P(___T,v1->tex_coords[1]);
			SETUP_DATTRIB_P(___S,v2->tex_coords[0]);
			SETUP_DATTRIB_P(___T,v2->tex_coords[1]);
		}
		void fragment(GLfloat w,GLfloat* att_p_w,cbuf_type* flat_color,TextureObject* texobj,cbuf_type* res_color)
		{
			GLfloat s,t;
			s=GET_ATTRIB_P(___S);
			t=GET_ATTRIB_P(___T);
			GLfloat tex_color[4]; // 0,1
			texobj->fetch(s,t,tex_color);

			// GL_MODULATE?
// 			*cbuf++=(int)(tex_color[0]*GET_ATTRIB_P(___R));
// 			*cbuf++=(int)(tex_color[1]*GET_ATTRIB_P(___G));
// 			*cbuf++=(int)(tex_color[2]*GET_ATTRIB_P(___B));
			
			// GL_REPLACE
			res_color[0]=(cbuf_type)(tex_color[0]*255.f);
			res_color[1]=(cbuf_type)(tex_color[1]*255.f);
			res_color[2]=(cbuf_type)(tex_color[2]*255.f);
			res_color[3]=(cbuf_type)(tex_color[3]*255.f);

// 			*cbuf++=(int)(tex_color[0]*255.f);
// 			*cbuf++=(int)(tex_color[1]*255.f);
// 			*cbuf++=(int)(tex_color[2]*255.f);
		}
#undef ___R
#undef ___G
#undef ___B
#undef ___A
#undef ___S
#undef ___T
	};

	
	struct ETRecord_PColor_PTex_PFog:public ETRecord_Persp<7>
	{
#define ___R 0
#define ___G 1
#define ___B 2
#define ___A 3
#define ___S 4
#define ___T 5
#define ___F 6

#ifdef USE_MEM_FACTORY
		virtual void recycle_me()
		{
			Factory_ETRecordPSmoothTex.recycle(this);
		}
#ifdef _YGL_DEBUG_
		void class_name(){printf("ETRecord_PColor_PTex_PFog:\t");}
#endif
#endif
		void init_attribs(Vertex* v1,Vertex* v2,GLfloat _dy,GLfloat theother_w)
		{
			SETUP_ATTRIB_P(___R,YGL_COLOR_F2IF(v1->col_front_pri[0]));
			SETUP_ATTRIB_P(___G,YGL_COLOR_F2IF(v1->col_front_pri[1]));
			SETUP_ATTRIB_P(___B,YGL_COLOR_F2IF(v1->col_front_pri[2]));
			SETUP_ATTRIB_P(___A,YGL_COLOR_F2IF(v1->col_front_pri[3]));

			SETUP_DATTRIB_P(___R,YGL_COLOR_F2IF(v2->col_front_pri[0]));
			SETUP_DATTRIB_P(___G,YGL_COLOR_F2IF(v2->col_front_pri[1]));
			SETUP_DATTRIB_P(___B,YGL_COLOR_F2IF(v2->col_front_pri[2]));
			SETUP_DATTRIB_P(___A,YGL_COLOR_F2IF(v2->col_front_pri[3]));

			SETUP_ATTRIB_P(___S,v1->tex_coords[0]);
			SETUP_ATTRIB_P(___T,v1->tex_coords[1]);
			SETUP_DATTRIB_P(___S,v2->tex_coords[0]);
			SETUP_DATTRIB_P(___T,v2->tex_coords[1]);

			GLfloat f1=1.f,f2=1.f,_tmp;
			switch(glctx.fog_mode)
			{
			case GL_EXP:
				f1=exp(-glctx.fog_density*v1->fog_coord);
				f2=exp(-glctx.fog_density*v2->fog_coord);
				break;
			case GL_EXP2:
				_tmp=glctx.fog_density*v1->fog_coord;
				f1=exp(_tmp*_tmp);
				_tmp=glctx.fog_density*v2->fog_coord;
				f2=exp(_tmp*_tmp);
				break;
			case GL_LINEAR:
				f1=(glctx.fog_end-v1->fog_coord)*glctx.cached_fog_e_s;
				f2=(glctx.fog_end-v2->fog_coord)*glctx.cached_fog_e_s;
				break;
			}

			SETUP_ATTRIB_P(___F,f1);
			SETUP_DATTRIB_P(___F,f2);
		}
		void fragment(GLfloat w,GLfloat* att_p_w,cbuf_type* flat_color,TextureObject* texobj,cbuf_type* res_color)
		{
			GLfloat s,t,f;
			s=GET_ATTRIB_P(___S);
			t=GET_ATTRIB_P(___T);
			f=GET_ATTRIB_P(___F);

			GLfloat tex_color[4]; // 0,1
			texobj->fetch(s,t,tex_color);

			GLfloat factor=f*255.f;

			// GL_MODULATE?
			// 			*cbuf++=(int)(tex_color[0]*GET_ATTRIB_P(___R));
			// 			*cbuf++=(int)(tex_color[1]*GET_ATTRIB_P(___G));
			// 			*cbuf++=(int)(tex_color[2]*GET_ATTRIB_P(___B));

			// GL_REPLACE
			res_color[0]=(cbuf_type)(tex_color[0]*factor);
			res_color[1]=(cbuf_type)(tex_color[1]*factor);
			res_color[2]=(cbuf_type)(tex_color[2]*factor);
			res_color[3]=(cbuf_type)(tex_color[3]*factor);
		}
#undef ___R
#undef ___G
#undef ___B
#undef ___A
#undef ___S
#undef ___T
#undef ___F
	};
	/*struct ETRecordPSmoothTex: public ETRecord_NoPersp<2>
	{
#define ___S 0
#define ___T 1

	#ifdef USE_MEM_FACTORY
		virtual void recycle_me()
		{
			Factory_ETRecordPSmoothTex.recycle(this);
		}
	#ifdef _YGL_DEBUG_
		void class_name(){printf("ETRecordPSmoothTex:\t");}
	#endif
	#endif
		void init_attribs(Vertex* v1,Vertex* v2,GLfloat _dy)
		{
			SETUP_ATTRIB_NOP(___S,v1->tex_coords[0]);
			SETUP_ATTRIB_NOP(___T,v1->tex_coords[1]);
			SETUP_DATTRIB_NOP(___S,v2->tex_coords[0]);
			SETUP_DATTRIB_NOP(___T,v2->tex_coords[1]);
		}
		void plot(GLubyte* cbuf,GLuint* zbuf,GLfloat* att_np,TextureObject* texobj)
		{
			GLfloat s,t;
			s=GET_ATTRIB_NOP(___S);
			t=GET_ATTRIB_NOP(___T);
			GLfloat tex_color[4]; // 0,1
			texobj->fetch(s,t,tex_color);

			// GL_MODULATE?
			// 			*cbuf++=(int)(tex_color[0]*GET_ATTRIB_P(___R));
			// 			*cbuf++=(int)(tex_color[1]*GET_ATTRIB_P(___G));
			// 			*cbuf++=(int)(tex_color[2]*GET_ATTRIB_P(___B));

			// GL_REPLACE
			*cbuf++=(int)(tex_color[0]*255.f);
			*cbuf++=(int)(tex_color[1]*255.f);
			*cbuf++=(int)(tex_color[2]*255.f);
		}
#undef ___S
#undef ___T
	};*/

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
				DELETE_ETRECORD(p);
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
			DELETE_ETRECORD(e); // recycle		
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
			option&=~ETRecTex;
		// clear?

		// ?? round, top left fill
		int ymin=ROUND(Y_OF(v[vcnt-1].p)),ymax=ymin;
		// build ETs
		for(int i=0;i<vcnt-1;++i)
		{
			if(ROUND(Y_OF(v[i].p))<ymin)
				ymin=ROUND(Y_OF(v[i].p));
			else if(ROUND(Y_OF(v[i].p))>ymax)
				ymax=ROUND(Y_OF(v[i].p));

			// ignore horizontal lines?
			ETRecord* newRec=0;
			if(ROUND(Y_OF(v[i].p))!=ROUND(Y_OF(v[i+1].p)))
			{
				switch(option)
				{
				case ETRecSmooth:
					newRec=NEW_ETRECORD_PSMOOTH;
					break;
// 				case ETRecTex:
// 					newRec=NEW_ETRECORD_PTEX;
// 					break;
				case (ETRecSmooth|ETRecTex):
					newRec=NEW_ETRECORD_PSMOOTHTEX;
					break;
				default:
					newRec=NEW_ETRECORD;
					break;
				}

				newRec->build(v+i,v+(i+1));
				GET_ET_PTR(newRec->ymin)->add(newRec);
			}
		}

		if(ROUND(Y_OF(v[0].p))!=ROUND(Y_OF(v[vcnt-1].p)))
		{
			ETRecord* newRec=0;
			switch(option)
			{
			case ETRecSmooth:
				newRec=NEW_ETRECORD_PSMOOTH;
				break;
				// 				case ETRecTex:
				// 					newRec=NEW_ETRECORD_PTEX;
				// 					break;
			case (ETRecSmooth|ETRecTex):
				newRec=NEW_ETRECORD_PSMOOTHTEX;
				break;
			default:
				newRec=NEW_ETRECORD;
				break;
			}

			newRec->build(v+(vcnt-1),v);
			GET_ET_PTR(newRec->ymin)->add(newRec);
		}

		aet.head=0;

		// scan
		ETRecord* lefte,*righte,*nexte;
		// todo: dont test on et_ptrs[y] for every scanline? (linklist)
		for(int y=ymin;y<=ymax;++y)
		{
			
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
					if(y/*+1*/>=lefte->ymax)
						aet.erase(lefte);
					else lefte->incre();

					if(y/*+1*/>=righte->ymax)
						aet.erase(righte);
					else righte->incre();

					lefte=nexte;
				}
			}
			if(et_ptrs[y])
			{
				// dont raster bottom line, incre to next
				ETRecord* rec=et_ptrs[y]->head;
				while(rec)
				{
					rec->incre();rec=rec->next;
				}
				aet.merge(et_ptrs[y]);
				et_ptrs[y]=0; // already freed in merge
			}
		}

		// cleanup
		aet.free();
	}

	
}
}