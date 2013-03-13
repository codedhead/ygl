#ifndef _YGL_MATRIX_
#define _YGL_MATRIX_

class Matrix44
{
public:
	float m[16];

	Matrix44()
	{
		loadI();
	}

	Matrix44(const float* mm)
	{
		memcpy(m,mm,sizeof(float)*16);
	}

	void loadI()
	{
		m[0]=1.0f;m[4]=0.0f;m[8]=0.0f;m[12]=0.0f;
		m[1]=0.0f;m[5]=1.0f;m[9]=0.0f;m[13]=0.0f;
		m[2]=0.0f;m[6]=0.0f;m[10]=1.0f;m[14]=0.0f;
		m[3]=0.0f;m[7]=0.0f;m[11]=0.0f;m[15]=1.0f;
	}
	// todo: add a inverse function
	void loadSystem(const Vector& u,const Vector& v,const Vector& n,
		const Point& p)
	{
		m[0]=u.x;m[4]=v.x;m[8]=n.x;m[12]=p.x;
		m[1]=u.y;m[5]=v.y;m[9]=n.y;m[13]=p.y;
		m[2]=u.z;m[6]=v.z;m[10]=n.z;m[14]=p.z;
		m[3]=0.0f;m[7]=0.0f;m[11]=0.0f;m[15]=1.0f;
	}
	void loadMatrixf(const float* n)
	{
		memcpy(m,n,sizeof(float)*16);
	}

	bool isAffine() const
	{
		return m[3]==0.f&&m[7]==0.f&&m[11]==0.f&&m[15]==1.f;
	}

	// right mul
	void scale_(const Vector& s)
	{
		m[0]*=s.x;m[4]*=s.y;m[8]*=s.z;
		m[1]*=s.x;m[5]*=s.y;m[9]*=s.z;
		m[2]*=s.x;m[6]*=s.y;m[10]*=s.z;
		m[3]*=s.x;m[7]*=s.y;m[11]*=s.z;
	}
	void translate_(const Vector& p)
	{
		m[12]+=m[0]*p.x+m[4]*p.y+m[8]*p.z;
		m[13]+=m[1]*p.x+m[5]*p.y+m[9]*p.z;
		m[14]+=m[2]*p.x+m[6]*p.y+m[10]*p.z;
		m[15]+=m[3]*p.x+m[7]*p.y+m[11]*p.z;
	}
	void rotate_(){}
	// left mul
	void _scale(const Vector& s)
	{
		m[0]*=s.x;m[4]*=s.x;m[8]*=s.x;m[12]*=s.x;
		m[1]*=s.y;m[5]*=s.y;m[9]*=s.y;m[13]*=s.y;
		m[2]*=s.z;m[6]*=s.z;m[10]*=s.z;m[14]*=s.z;
	}
	void _translate(const Vector& p)
	{
		m[0]+=m[3]*p.x;m[4]+=m[7]*p.x;m[8]+=m[11]*p.x;m[12]+=m[15]*p.x;
		m[1]+=m[3]*p.y;m[5]+=m[7]*p.y;m[9]+=m[11]*p.y;m[13]+=m[15]*p.y;
		m[2]+=m[3]*p.z;m[6]+=m[7]*p.z;m[10]+=m[11]*p.z;m[14]+=m[15]*p.z;
	}
	void _rotate(){}

	Matrix4x4 operator*(const Matrix4x4& b)const
	{
		Matrix4x4 ret;
		mul(this->m,b.m,ret.m);
		return ret;
	}
	Matrix4x4& operator*=(const Matrix4x4& b)
	{
		mul(this->m,b.m,this->m);
		return *this;
	}

	void onP(GLfloat* v) const
	{

	}
	void onV(GLfloat* v) const
	{

	}
	void onVwithTranspose(GLfloat* v) const
	{

	}


	// 	void leftMul(const double* n)
	// 	{
	// 		double res[16];
	// 		res[0]=n[0]*m[0]+n[4]*m[1]+n[8]*m[2]+n[12]*m[3];
	// 		res[4]=n[0]*m[4]+n[4]*m[5]+n[8]*m[6]+n[12]*m[7];
	// 		res[8]=n[0]*m[8]+n[4]*m[9]+n[8]*m[10]+n[12]*m[11];
	// 		res[12]=n[0]*m[12]+n[4]*m[13]+n[8]*m[14]+n[12]*m[15];
	// 		res[1]=n[1]*m[0]+n[5]*m[1]+n[9]*m[2]+n[13]*m[3];
	// 		res[5]=n[1]*m[4]+n[5]*m[5]+n[9]*m[6]+n[13]*m[7];
	// 		res[9]=n[1]*m[8]+n[5]*m[9]+n[9]*m[10]+n[13]*m[11];
	// 		res[13]=n[1]*m[12]+n[5]*m[13]+n[9]*m[14]+n[13]*m[15];
	// 		res[2]=n[2]*m[0]+n[6]*m[1]+n[10]*m[2]+n[14]*m[3];
	// 		res[6]=n[2]*m[4]+n[6]*m[5]+n[10]*m[6]+n[14]*m[7];
	// 		res[10]=n[2]*m[8]+n[6]*m[9]+n[10]*m[10]+n[14]*m[11];
	// 		res[14]=n[2]*m[12]+n[6]*m[13]+n[10]*m[14]+n[14]*m[15];
	// 		res[3]=n[3]*m[0]+n[7]*m[1]+n[11]*m[2]+n[15]*m[3];
	// 		res[7]=n[3]*m[4]+n[7]*m[5]+n[11]*m[6]+n[15]*m[7];
	// 		res[11]=n[3]*m[8]+n[7]*m[9]+n[11]*m[10]+n[15]*m[11];
	// 		res[15]=n[3]*m[12]+n[7]*m[13]+n[11]*m[14]+n[15]*m[15];
	// 		memcpy(m,res,sizeof(double)*16);
	// 	}

	static void mul(const float* m,const float* n,float* dst)
	{
		float res[16];
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
		memcpy(dst,res,sizeof(float)*16);
	}

// 	Matrix4x4 transpose() const
// 	{
// 		Matrix4x4 ret(*this);
// 		ret.do_transpose();
// 		return ret;
// 	}
// 	Matrix4x4 inverse() const
// 	{
// 		Matrix4x4 ret;
// 		Assert(isAffine());
// 		affine_inv(ret.m);
// 		return ret;
// 	}
// 	void do_transpose()
// 	{
// 		::swap(m[1],m[4]);
// 		::swap(m[2],m[8]);
// 		::swap(m[3],m[12]);
// 
// 		::swap(m[6],m[9]);
// 		::swap(m[7],m[13]);
// 
// 		::swap(m[11],m[14]);
// 	}
// 
// 	float affine_det() const
// 	{
// 		return m[0]*m[5]*m[10]+m[1]*m[6]*m[8]+m[2]*m[4]*m[9]
// 		-m[2]*m[5]*m[8]-m[0]*m[6]*m[9]-m[1]*m[4]*m[10];
// 	}
// 	void affine_inv(float* dst) const // possible change
// 	{
// 		Assert(isAffine());
// 		float det=affine_det();
// 		Assert(det!=0.f&&!isnan(det));
// 		float invDet=1.f/det;
// 		float dstm[16];
// 
// 		dstm[0]=invDet*(m[5]*m[10]-m[6]*m[9]);
// 		dstm[5]=invDet*(m[0]*m[10]-m[2]*m[8]);
// 		dstm[10]=invDet*(m[5]*m[0]-m[1]*m[4]);
// 		dstm[1]=-invDet*(m[1]*m[10]-m[2]*m[9]);
// 		dstm[2]=invDet*(m[1]*m[6]-m[2]*m[5]);
// 		dstm[4]=-invDet*(m[4]*m[10]-m[6]*m[8]);
// 		dstm[6]=-invDet*(m[0]*m[6]-m[2]*m[4]);
// 		dstm[8]=invDet*(m[4]*m[9]-m[5]*m[8]);
// 		dstm[9]=-invDet*(m[0]*m[9]-m[1]*m[8]);
// 
// 		dstm[3]=dstm[7]=dstm[11]=0.0;dstm[15]=1.0;
// 
// 		dstm[12]=-(dstm[0]*m[12]+dstm[4]*m[13]+dstm[8]*m[14]);
// 		dstm[13]=-(dstm[1]*m[12]+dstm[5]*m[13]+dstm[9]*m[14]);
// 		dstm[14]=-(dstm[2]*m[12]+dstm[6]*m[13]+dstm[10]*m[14]);
// 		memcpy(dst,dstm,sizeof(float)*16);
// 	}

};



#endif