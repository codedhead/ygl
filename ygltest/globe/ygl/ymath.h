#ifndef _YGL_MATH_H_
#define _YGL_MATH_H_

#include <string.h>
#include <math.h>

class Vector3
{
public:
	double x,y,z;
	Vector3():x(0.0),y(0.0),z(0.0){}
	Vector3(float* v):x(v[0]),y(v[1]),z(v[2]){}
	Vector3(double* v):x(v[0]),y(v[1]),z(v[2]){}
	Vector3(double xx,double yy,double zz):x(xx),y(yy),z(zz){}
	double dot(Vector3& v)
	{
		return x*v.x+y*v.y+z*v.z;
	}

	double length()
	{
		return sqrt(x*x+y*y+z*z);
	}

	double normalize()
	{
		double m=sqrt(x*x+y*y+z*z);
		if(m==0.0) return 0.0;
		x/=m;y/=m;z/=m;
		return m;
	}

	Vector3 operator-()
	{
		return Vector3(-x,-y,-z);;
	}

	Vector3 operator-(Vector3& v)
	{
		return Vector3(x-v.x,y-v.y,z-v.z);
	}

	Vector3 operator+(Vector3& v)
	{
		return Vector3(x+v.x,y+v.y,z+v.z);
	}

	Vector3 operator+=(Vector3& v)
	{
		x+=v.x;y+=v.y;z+=v.z;
		return *this;
	}

	Vector3 operator-=(Vector3& v)
	{
		x-=v.x;y-=v.y;z-=v.z;
		return *this;
	}

	Vector3 operator*(Vector3& v)
	{
		return Vector3(y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x);
	}

	Vector3 operator*(double k)
	{
		return Vector3(k*x,k*y,k*z);
	}
};


class Matrix44
{
public:
	double m[16];

	Matrix44()
	{
		loadIdentity();
	}

	Matrix44(const Matrix44& n)
	{
		memcpy(m,n.m,sizeof(double)*16);
	}

	void loadIdentity()
	{
		m[0]=1.0;m[4]=0.0;m[8]=0.0;m[12]=0.0;
		m[1]=0.0;m[5]=1.0;m[9]=0.0;m[13]=0.0;
		m[2]=0.0;m[6]=0.0;m[10]=1.0;m[14]=0.0;
		m[3]=0.0;m[7]=0.0;m[11]=0.0;m[15]=1.0;
	}
	void loadMatrixd(const double* n)
	{
		memcpy(m,n,sizeof(double)*16);
	}
	void loadMatrixf(const float* n)
	{
		for(int i=0;i<16;i++) m[i]=n[i];
	}
	void leftMul(Matrix44& n)
	{
		leftMul(n.m);
	}

	void rightMul(Matrix44& n)
	{
		rightMul(n.m);
	}

	void leftMul(const double* n)
	{
		double res[16];
		res[0]=n[0]*m[0]+n[4]*m[1]+n[8]*m[2]+n[12]*m[3];
		res[4]=n[0]*m[4]+n[4]*m[5]+n[8]*m[6]+n[12]*m[7];
		res[8]=n[0]*m[8]+n[4]*m[9]+n[8]*m[10]+n[12]*m[11];
		res[12]=n[0]*m[12]+n[4]*m[13]+n[8]*m[14]+n[12]*m[15];
		res[1]=n[1]*m[0]+n[5]*m[1]+n[9]*m[2]+n[13]*m[3];
		res[5]=n[1]*m[4]+n[5]*m[5]+n[9]*m[6]+n[13]*m[7];
		res[9]=n[1]*m[8]+n[5]*m[9]+n[9]*m[10]+n[13]*m[11];
		res[13]=n[1]*m[12]+n[5]*m[13]+n[9]*m[14]+n[13]*m[15];
		res[2]=n[2]*m[0]+n[6]*m[1]+n[10]*m[2]+n[14]*m[3];
		res[6]=n[2]*m[4]+n[6]*m[5]+n[10]*m[6]+n[14]*m[7];
		res[10]=n[2]*m[8]+n[6]*m[9]+n[10]*m[10]+n[14]*m[11];
		res[14]=n[2]*m[12]+n[6]*m[13]+n[10]*m[14]+n[14]*m[15];
		res[3]=n[3]*m[0]+n[7]*m[1]+n[11]*m[2]+n[15]*m[3];
		res[7]=n[3]*m[4]+n[7]*m[5]+n[11]*m[6]+n[15]*m[7];
		res[11]=n[3]*m[8]+n[7]*m[9]+n[11]*m[10]+n[15]*m[11];
		res[15]=n[3]*m[12]+n[7]*m[13]+n[11]*m[14]+n[15]*m[15];
		memcpy(m,res,sizeof(double)*16);
	}

	void rightMul(const double* n)
	{
		double res[16];
		res[0]=m[0]*n[0]+m[4]*n[1]+m[8]*n[2]+m[12]*n[3];
		res[4]=m[0]*n[4]+m[4]*n[5]+m[8]*n[6]+m[12]*n[7];
		res[8]=m[0]*n[8]+m[4]*n[9]+m[8]*n[10]+m[12]*n[11];
		res[12]=m[0]*n[12]+m[4]*n[13]+m[8]*n[14]+m[12]*n[15];
		res[1]=m[1]*n[0]+m[5]*n[1]+m[9]*n[2]+m[13]*n[3];
		res[5]=m[1]*n[4]+m[5]*n[5]+m[9]*n[6]+m[13]*n[7];
		res[9]=m[1]*n[8]+m[5]*n[9]+m[9]*n[10]+m[13]*n[11];
		res[13]=m[1]*n[12]+m[5]*n[13]+m[9]*n[14]+m[13]*n[15];
		res[2]=m[2]*n[0]+m[6]*n[1]+m[10]*n[2]+m[14]*n[3];
		res[6]=m[2]*n[4]+m[6]*n[5]+m[10]*n[6]+m[14]*n[7];
		res[10]=m[2]*n[8]+m[6]*n[9]+m[10]*n[10]+m[14]*n[11];
		res[14]=m[2]*n[12]+m[6]*n[13]+m[10]*n[14]+m[14]*n[15];
		res[3]=m[3]*n[0]+m[7]*n[1]+m[11]*n[2]+m[15]*n[3];
		res[7]=m[3]*n[4]+m[7]*n[5]+m[11]*n[6]+m[15]*n[7];
		res[11]=m[3]*n[8]+m[7]*n[9]+m[11]*n[10]+m[15]*n[11];
		res[15]=m[3]*n[12]+m[7]*n[13]+m[11]*n[14]+m[15]*n[15];
		memcpy(m,res,sizeof(double)*16);
	}

	void leftMul(const float* n)
	{
		double res[16];
		res[0]=n[0]*m[0]+n[4]*m[1]+n[8]*m[2]+n[12]*m[3];
		res[4]=n[0]*m[4]+n[4]*m[5]+n[8]*m[6]+n[12]*m[7];
		res[8]=n[0]*m[8]+n[4]*m[9]+n[8]*m[10]+n[12]*m[11];
		res[12]=n[0]*m[12]+n[4]*m[13]+n[8]*m[14]+n[12]*m[15];
		res[1]=n[1]*m[0]+n[5]*m[1]+n[9]*m[2]+n[13]*m[3];
		res[5]=n[1]*m[4]+n[5]*m[5]+n[9]*m[6]+n[13]*m[7];
		res[9]=n[1]*m[8]+n[5]*m[9]+n[9]*m[10]+n[13]*m[11];
		res[13]=n[1]*m[12]+n[5]*m[13]+n[9]*m[14]+n[13]*m[15];
		res[2]=n[2]*m[0]+n[6]*m[1]+n[10]*m[2]+n[14]*m[3];
		res[6]=n[2]*m[4]+n[6]*m[5]+n[10]*m[6]+n[14]*m[7];
		res[10]=n[2]*m[8]+n[6]*m[9]+n[10]*m[10]+n[14]*m[11];
		res[14]=n[2]*m[12]+n[6]*m[13]+n[10]*m[14]+n[14]*m[15];
		res[3]=n[3]*m[0]+n[7]*m[1]+n[11]*m[2]+n[15]*m[3];
		res[7]=n[3]*m[4]+n[7]*m[5]+n[11]*m[6]+n[15]*m[7];
		res[11]=n[3]*m[8]+n[7]*m[9]+n[11]*m[10]+n[15]*m[11];
		res[15]=n[3]*m[12]+n[7]*m[13]+n[11]*m[14]+n[15]*m[15];
		memcpy(m,res,sizeof(double)*16);
	}

	void rightMul(const float* n)
	{
		double res[16];
		res[0]=m[0]*n[0]+m[4]*n[1]+m[8]*n[2]+m[12]*n[3];
		res[4]=m[0]*n[4]+m[4]*n[5]+m[8]*n[6]+m[12]*n[7];
		res[8]=m[0]*n[8]+m[4]*n[9]+m[8]*n[10]+m[12]*n[11];
		res[12]=m[0]*n[12]+m[4]*n[13]+m[8]*n[14]+m[12]*n[15];
		res[1]=m[1]*n[0]+m[5]*n[1]+m[9]*n[2]+m[13]*n[3];
		res[5]=m[1]*n[4]+m[5]*n[5]+m[9]*n[6]+m[13]*n[7];
		res[9]=m[1]*n[8]+m[5]*n[9]+m[9]*n[10]+m[13]*n[11];
		res[13]=m[1]*n[12]+m[5]*n[13]+m[9]*n[14]+m[13]*n[15];
		res[2]=m[2]*n[0]+m[6]*n[1]+m[10]*n[2]+m[14]*n[3];
		res[6]=m[2]*n[4]+m[6]*n[5]+m[10]*n[6]+m[14]*n[7];
		res[10]=m[2]*n[8]+m[6]*n[9]+m[10]*n[10]+m[14]*n[11];
		res[14]=m[2]*n[12]+m[6]*n[13]+m[10]*n[14]+m[14]*n[15];
		res[3]=m[3]*n[0]+m[7]*n[1]+m[11]*n[2]+m[15]*n[3];
		res[7]=m[3]*n[4]+m[7]*n[5]+m[11]*n[6]+m[15]*n[7];
		res[11]=m[3]*n[8]+m[7]*n[9]+m[11]*n[10]+m[15]*n[11];
		res[15]=m[3]*n[12]+m[7]*n[13]+m[11]*n[14]+m[15]*n[15];
		memcpy(m,res,sizeof(double)*16);
	}

	void applyToVector(double* v)
	{
		double res[4];
		res[0]=m[0]*v[0]+m[4]*v[1]+m[8]*v[2]+m[12]*v[3];
		res[1]=m[1]*v[0]+m[5]*v[1]+m[9]*v[2]+m[13]*v[3];
		res[2]=m[2]*v[0]+m[6]*v[1]+m[10]*v[2]+m[14]*v[3];
		res[3]=m[3]*v[0]+m[7]*v[1]+m[11]*v[2]+m[15]*v[3];
		v[0]=res[0];v[1]=res[1];v[2]=res[2];v[3]=res[3];
	}
	void applyToVector(float* v)
	{
		float res[4];
		res[0]=m[0]*v[0]+m[4]*v[1]+m[8]*v[2]+m[12]*v[3];
		res[1]=m[1]*v[0]+m[5]*v[1]+m[9]*v[2]+m[13]*v[3];
		res[2]=m[2]*v[0]+m[6]*v[1]+m[10]*v[2]+m[14]*v[3];
		res[3]=m[3]*v[0]+m[7]*v[1]+m[11]*v[2]+m[15]*v[3];
		v[0]=res[0];v[1]=res[1];v[2]=res[2];v[3]=res[3];
	}
	void applyToVectorWithTranspose(double* v)
	{
		double res[4];
		res[0]=m[0]*v[0]+m[1]*v[1]+m[2]*v[2]+m[3]*v[3];
		res[1]=m[4]*v[0]+m[5]*v[1]+m[6]*v[2]+m[7]*v[3];
		res[2]=m[8]*v[0]+m[9]*v[1]+m[10]*v[2]+m[11]*v[3];
		res[3]=m[12]*v[0]+m[13]*v[1]+m[14]*v[2]+m[15]*v[3];
		v[0]=res[0];v[1]=res[1];v[2]=res[2];v[3]=res[3];
	}
	Matrix44& operator=(Matrix44& n)
	{
		memcpy(m,n.m,sizeof(double)*16);

		return *this;
	}


	Matrix44 operator*(Matrix44& n)
	{
		Matrix44 res;
		res.m[0]=m[0]*n.m[0]+m[4]*n.m[1]+m[8]*n.m[2]+m[12]*n.m[3];
		res.m[4]=m[0]*n.m[4]+m[4]*n.m[5]+m[8]*n.m[6]+m[12]*n.m[7];
		res.m[8]=m[0]*n.m[8]+m[4]*n.m[9]+m[8]*n.m[10]+m[12]*n.m[11];
		res.m[12]=m[0]*n.m[12]+m[4]*n.m[13]+m[8]*n.m[14]+m[12]*n.m[15];
		res.m[1]=m[1]*n.m[0]+m[5]*n.m[1]+m[9]*n.m[2]+m[13]*n.m[3];
		res.m[5]=m[1]*n.m[4]+m[5]*n.m[5]+m[9]*n.m[6]+m[13]*n.m[7];
		res.m[9]=m[1]*n.m[8]+m[5]*n.m[9]+m[9]*n.m[10]+m[13]*n.m[11];
		res.m[13]=m[1]*n.m[12]+m[5]*n.m[13]+m[9]*n.m[14]+m[13]*n.m[15];
		res.m[2]=m[2]*n.m[0]+m[6]*n.m[1]+m[10]*n.m[2]+m[14]*n.m[3];
		res.m[6]=m[2]*n.m[4]+m[6]*n.m[5]+m[10]*n.m[6]+m[14]*n.m[7];
		res.m[10]=m[2]*n.m[8]+m[6]*n.m[9]+m[10]*n.m[10]+m[14]*n.m[11];
		res.m[14]=m[2]*n.m[12]+m[6]*n.m[13]+m[10]*n.m[14]+m[14]*n.m[15];
		res.m[3]=m[3]*n.m[0]+m[7]*n.m[1]+m[11]*n.m[2]+m[15]*n.m[3];
		res.m[7]=m[3]*n.m[4]+m[7]*n.m[5]+m[11]*n.m[6]+m[15]*n.m[7];
		res.m[11]=m[3]*n.m[8]+m[7]*n.m[9]+m[11]*n.m[10]+m[15]*n.m[11];
		res.m[15]=m[3]*n.m[12]+m[7]*n.m[13]+m[11]*n.m[14]+m[15]*n.m[15];

		return res;
	}
	/*void inverse(CLAYMATRIX& mOut, const CLAYMATRIX& rhs)
	{
		CLAYMATRIX m(rhs);
		int is[4];
		int js[4];
		int f = 1;

		for (int k = 0; k < 4; k ++)
		{
			// 第一步，全选主元
			float fMax = 0.0f;
			for (int i = k; i < 4; i ++)
			{
				for (int j = k; j < 4; j ++)
				{
					const float f = abs(m(i,j));
					if (f > fMax)
					{
						fMax	= f;
						is[k]	= i;
						js[k]	= j;
					}
				}
			}
			if (Abs(fMax) < 0.0001f)
				return 0;

			if (is[k] != k)
			{
				f = -f;
				swap(m(k, 0), m(is[k], 0));
				swap(m(k, 1), m(is[k], 1));
				swap(m(k, 2), m(is[k], 2));
				swap(m(k, 3), m(is[k], 3));
			}
			if (js[k] != k)
			{
				f = -f;
				swap(m(0, k), m(0, js[k]));
				swap(m(1, k), m(1, js[k]));
				swap(m(2, k), m(2, js[k]));
				swap(m(3, k), m(3, js[k]));
			}
			// 计算逆矩阵

			// 第二步
			m(k, k) = 1.0f / m(k, k);	
			// 第三步
			for (int j = 0; j < 4; j ++)
			{
				if (j != k)
					m(k, j) *= m(k, k);
			}
			// 第四步
			for (int i = 0; i < 4; i ++)
			{
				if (i != k)
				{
					for	(j = 0; j < 4; j ++)
					{
						if (j != k)
							m(i, j) = m(i, j) - m(i, k) * m(k, j);
					}
				}
			}
			// 第五步
			for (i = 0; i < 4; i ++)
			{
				if (i != k)
					m(i, k) *= -m(k, k);
			}
		}

		for	(k = 3; k >= 0; k --)
		{
			if (js[k] != k)
			{
				swap(m(k, 0), m(js[k], 0));
				swap(m(k, 1), m(js[k], 1));
				swap(m(k, 2), m(js[k], 2));
				swap(m(k, 3), m(js[k], 3));
			}
			if (is[k] != k)
			{
				swap(m(0, k), m(0, is[k]));
				swap(m(1, k), m(1, is[k]));
				swap(m(2, k), m(2, is[k]));
				swap(m(3, k), m(3, is[k]));
			}
		}

		mOut = m;
	}*/

	double mvDet()
	{
		return m[0]*m[5]*m[10]+m[1]*m[6]*m[8]+m[2]*m[4]*m[9]
					-m[2]*m[5]*m[8]-m[0]*m[6]*m[9]-m[1]*m[4]*m[10];
	}
	void mvInverse(Matrix44& dst)
	{
		double det=1.0/mvDet();
		dst.m[0]=det*(m[5]*m[10]-m[6]*m[9]);
		dst.m[5]=det*(m[0]*m[10]-m[2]*m[8]);
		dst.m[10]=det*(m[5]*m[0]-m[1]*m[4]);
		dst.m[1]=-det*(m[1]*m[10]-m[2]*m[9]);
		dst.m[2]=det*(m[1]*m[6]-m[2]*m[5]);
		dst.m[4]=-det*(m[4]*m[10]-m[6]*m[8]);
		dst.m[6]=-det*(m[0]*m[6]-m[2]*m[4]);
		dst.m[8]=det*(m[4]*m[9]-m[5]*m[8]);
		dst.m[9]=-det*(m[0]*m[9]-m[1]*m[8]);
		
		dst.m[3]=dst.m[7]=dst.m[11]=0.0;dst.m[15]=1.0;

		dst.m[12]=-(dst.m[0]*m[12]+dst.m[4]*m[13]+dst.m[8]*m[14]);
		dst.m[13]=-(dst.m[1]*m[12]+dst.m[5]*m[13]+dst.m[9]*m[14]);
		dst.m[14]=-(dst.m[2]*m[12]+dst.m[6]*m[13]+dst.m[10]*m[14]);
	}

};

#endif