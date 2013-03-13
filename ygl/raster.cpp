#include "raster.h"
#include "yglstructs.h"
#include "buffer.h"
#include "global.h"
#include "yglconstants.h"

//#define USE_MEM_FACTORY

#ifdef USE_MEM_FACTORY
#include "memory.h"
#endif

namespace raster
{
#ifdef USE_MEM_FACTORY
	struct ETable;
	struct ETRecord;
	extern TFactory<ETable> Factory_ETable;
	extern TFactory<ETRecord> Factory_ETRecord;
	
#define NEW_ETABLE (Factory_ETable.alloc())
#define DELETE_ETABLE(et) Factory_ETable.recycle(et)

#define NEW_ETRECORD (Factory_ETRecord.alloc())
#define DELETE_ETRECORD(rec) Factory_ETRecord.recycle(rec)

#else

#define NEW_ETABLE (new ETable)
#define DELETE_ETABLE(et) delete (et)

#define NEW_ETRECORD (new ETRecord)
#define DELETE_ETRECORD(rec) delete (rec)

#endif
	

	void line(Vertex* v,GLfloat* color)
	{
		// assume x0<x1,y0<y1,k<1

		int x0=ROUND(v[0].p[0]),
			y0=ROUND(v[0].p[1]),
			x1=ROUND(v[1].p[0]),
			y1=ROUND(v[1].p[1]);

		// trival case
		if(x0==x1)
		{
			if(y1<y0)
				XOR_SWAP(y0,y1);

			for(int y=y0;y<=y1;++y)
				buffer::plot(x0,y,color);

			return;
		}
		else if(y0==y1)
		{
			if(x1<x0)
				XOR_SWAP(x0,x1);

			for(int x=x0;x<=x1;++x)
				buffer::plot(x,y0,color);

			return;
		}

		int steep=abs(x1-x0)-abs(y0-y1);
		if(steep==0)
		{
			if(x1<x0)
			{
				XOR_SWAP(x0,x1);
				XOR_SWAP(y0,y1);
			}

			if(y0<y1)
			{
				for(int x=x0,y=y0;x<=x1;++x,++y)
					buffer::plot(x,y,color);
			}
			else
			{
				for(int x=x0,y=y0;x<=x1;++x,--y)
					buffer::plot(x,y,color);
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
				XOR_SWAP(x0,x1);
				XOR_SWAP(y0,y1);
			}

			const int inc_y=y0<y1?1:-1;

			// x0<=x1, dx>0
			int dx=x1-x0,dy=abs(y1-y0); // check!!
			int D=dy+dy-dx;
			int delta1=dy+dy,delta2=dy+dy-dx-dx;

			for(int x=x0,y=y0;x<=x1;++x)
			{
				if(D<=0) // use bottom
				{
					D+=delta1;
					if(steep<0) buffer::plot(y,x,color);
					else buffer::plot(x,y,color);
				}
				else
				{
					D+=delta2;
					if(steep<0) buffer::plot((y+=inc_y),x,color);
					else buffer::plot(x,(y+=inc_y),color);
				}
			}
		}

	}

	// todo: handle exceptions? single vert isect, horizontal line no add to etable
	struct ETRecord
	{
		// bresenham vertical
		//int D,delta1,delta2,inc_x;
		//GRow vertical or horizonal

		// edge info
		int ymin,ymax;
		GLfloat cur_x;
		GLfloat m_recip; // 1/m = dx/dy


		ETRecord* prev,*next;

#ifdef USE_MEM_FACTORY
		ETRecord(){ctor();}
		void ctor(){prev=next=0;}
#ifdef _YGL_DEBUG_
		void class_name(){printf("ETRecord:\t");}
#endif
#else
		ETRecord():prev(0),next(0){}
#endif
		void build(Vertex* v1,Vertex* v2)
		{
			prev=next=0;
			// we start from ymin so ymin always < ymax (bresenham)
			if(Y_OF(v1->p)<Y_OF(v2->p))
			{
				// check: ROUND?? fill convention
				ymin=ROUND(Y_OF(v1->p));
				ymax=ROUND(Y_OF(v2->p));
				cur_x=ROUND(X_OF(v1->p));

				m_recip=((GLfloat)(ROUND(X_OF(v2->p))-cur_x))/((GLfloat)(ymax-ymin));
			}
			else
			{
				ymin=ROUND(Y_OF(v2->p));
				ymax=ROUND(Y_OF(v1->p));
				cur_x=ROUND(X_OF(v2->p));

				m_recip=((GLfloat)(ROUND(X_OF(v1->p))-cur_x))/((GLfloat)(ymax-ymin));
			}
			//int dx=,dy=ymax-ymin;

		}

		inline void incre()
		{
			cur_x+=m_recip;
		}
	};

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
				if(p->cur_x<=q->cur_x)
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
					(p->cur_x<e->cur_x||
						(p->cur_x==e->cur_x&&p->m_recip<e->m_recip)
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
	TFactory<ETRecord> Factory_ETRecord;
#endif

	ETable* et_ptrs[YGL_MAX_BUFFER_HEIGHT];
	ETable aet;
#define GET_ET_PTR(y) (et_ptrs[y]?et_ptrs[y]:(et_ptrs[y]=(NEW_ETABLE)))

	// edge by bresenham, other attribs increment
	void scanline(Vertex* v,int vcnt)
	{
		// clear?

		//GLfloat* color=v[0].col_front_pri;
		GLint color[4]={YGL_COLOR_F2I(v[0].col_front_pri[0]),
			YGL_COLOR_F2I(v[0].col_front_pri[1]),
			YGL_COLOR_F2I(v[0].col_front_pri[2]),
			255
		};

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
			if(ROUND(Y_OF(v[i].p))!=ROUND(Y_OF(v[i+1].p)))
			{
				ETRecord* newRec=NEW_ETRECORD;
				newRec->build(v+i,v+(i+1));
				GET_ET_PTR(newRec->ymin)->add(newRec);
			}
		}

		if(ROUND(Y_OF(v[0].p))!=ROUND(Y_OF(v[vcnt-1].p)))
		{
			ETRecord* newRec=NEW_ETRECORD;
			newRec->build(v+(vcnt-1),v);
			GET_ET_PTR(newRec->ymin)->add(newRec);
		}

		aet.head=0;

		// scan
		ETRecord* lefte,*righte,*nexte;
		int left,right;
		GLubyte* cbuf;
		for(int y=ymin;y<=ymax;++y)
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
					left=ROUND(lefte->cur_x);right=ROUND(righte->cur_x);
					cbuf=buffer::framebuf_ptr(left,y);
					// fill span
					for(int x=left;x<=right;++x)
					{
						*cbuf++=color[0];
						*cbuf++=color[1];
						*cbuf++=color[2];
					}

					nexte=righte->next;
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
		}

		// cleanup
		aet.free();
	}
}