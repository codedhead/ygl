#ifndef _YGL_MEMORY_
#define _YGL_MEMORY_

#include <vector>

template <class T>
struct TWrap
{
	T m;
	TWrap<T>* n;
};

template <class T>
class TFactory
{
public:
	TFactory(int b=128):free_(0),batch_num_(b){}
	~TFactory()
	{
		for(int i=0,iLen=heads_.size();i<iLen;++i)
			free(heads_[i]);
	}

	T* alloc()
	{
		if(!free_)
		{
			TWrap<T>* h=(TWrap<T>*)malloc(sizeof(TWrap<T>)*batch_num_);
			if(!h) return 0;//error
			heads_.push_back(h);

			for(int i=0;i<batch_num_;++i)
				h[i].n=h+(i+1);
			h[batch_num_-1].n=0;
			free_=h;
#ifdef _YGL_DEBUG_
			free_->m.class_name();
			printf("alloc another batchnum %d\n",batch_num_);
#endif
		}
		T* ret=&(free_->m);
		// ctor??
		ret->ctor();
		free_=free_->n;
		return ret;
	}
	void recycle(T* t)
	{
		// dtor??
		t->~T();
		TWrap<T>* w=(TWrap<T>*)t;
		w->n=free_;
		free_=w;
	}

private:
	TWrap<T>* free_;
	std::vector<TWrap<T>*> heads_;
	int batch_num_;
};

#endif