#ifndef _YGL_STACK_H_
#define _YGL_STACK_H_

template <size_t matrix_size>
class stack
{
public:
	stack()
	{
		//initially has one element
		top=0;
	}
	Matrix44* peek()
	{
		if(top<0) return NULL;
		else return matrixs+top;
	}

	Matrix44* pop()
	{
		//can't be empty,return new top
		if(top>0) return matrixs+(--top);
		else if(top==0) return matrixs;
		else return NULL;
	}

	void push(Matrix44& m)
	{
		if(top<matrix_size-1)
		{
			matrixs[++top]=m;
		}
	}

	int top;
	Matrix44 matrixs[matrix_size];
};

#endif