#ifndef _YGL_STACK_H_
#define _YGL_STACK_H_

template <class T,size_t STACK_SIZE>
class stack
{
public:
	stack():top(-1),stack_size(STACK_SIZE)
	{
		//initially has one element??
	}
	inline int cnt() const{return top+1;}
	inline bool full() const{return top>=stack_size-1;}
	inline T* peek() const
	{
		if(top<0) return 0;
		else return (T*)(ele+top);
	}

	// strange here
	inline T* pop()
	{
		//xx//can't be empty,return new top
		if(top>=0) return ele+(--top);
		//else if(top==0) return ele;
		else return 0;
	}

	// return top
	inline T* push(const T& m)
	{
		if(top<stack_size-1)
		{
			ele[++top]=m;
		}
		return top<0?0:ele+top;
	}

	int top,stack_size;
	T ele[STACK_SIZE];
};

#endif