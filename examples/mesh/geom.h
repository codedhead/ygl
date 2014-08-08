#ifndef _YGL_GEOMETRY_
#define _YGL_GEOMETRY_

//#include "global.h"
#include <assert.h>
#define Assert assert
#include <float.h>
#define isnan _isnan
#define isinf(f) (!_finite((f)))
#include <math.h>

#include <algorithm>
using std::min;
using std::max;

#ifndef INFINITY
#define INFINITY FLT_MAX
#endif

inline float lerp(float t, float v1, float v2) {
	return (1.f - t) * v1 + t * v2;
}

inline void asc_order(float a,float b,float& f1,float& f2)
{
	if(a<b){f1=a;f2=b;}
	else{f2=a;f1=b;}
}
inline void desc_order(float a,float b,float& f1,float& f2)
{
	if(b<a){f1=a;f2=b;}
	else{f2=a;f1=b;}
}

// todo: cross handedness ??
// point addition ( lerp )

class Point;
class Normal;

class Vector {
public:
	// Vector Public Methods
	Vector() { x = y = z = 0.f; }
	explicit Vector(float f) { x = y = z = f; }
	Vector(float xx, float yy, float zz)
		: x(xx), y(yy), z(zz) {
			Assert(!hasNaNs());
	}
	bool hasZeros() const { return x==0.f||y==0.f||z==0.f; }
	bool hasNaNs() const { return isnan(x) || isnan(y) || isnan(z); }
	explicit Vector(const Point &p);
	explicit Vector(const Normal &n);

#ifndef NDEBUG
	// The default versions of these are fine for release builds; for debug
	// we define them so that we can add the Assert checks.
	Vector(const Vector &v) {
		Assert(!v.hasNaNs());
		x = v.x; y = v.y; z = v.z;
	}

	Vector &operator=(const Vector &v) {
		Assert(!v.hasNaNs());
		x = v.x; y = v.y; z = v.z;
		return *this;
	}
#endif // !NDEBUG

	Vector reciprocal()const
	{
		Assert(!hasZeros());
		return Vector(1.f/x, 1.f/y, 1.f/z);
	}
	void do_reciprocal()
	{
		Assert(!hasZeros());
		x=1.f/x; y=1.f/y; z=1.f/z;	
	}
	Vector negate()const
	{
		return Vector(-x,-y,-z);
	}
	inline void do_negate()
	{
		x=-x;y=-y;z=-z;
	}

	// unary -
	Vector operator-() const { return Vector(-x, -y, -z); }
	// +
	Vector operator+(const Vector &v) const {
		Assert(!v.hasNaNs());
		return Vector(x + v.x, y + v.y, z + v.z);
	}
	Vector& operator+=(const Vector &v) {
		Assert(!v.hasNaNs());
		x += v.x; y += v.y; z += v.z;
		return *this;
	}
	// -
	Vector operator-(const Vector &v) const {
		Assert(!v.hasNaNs());
		return Vector(x - v.x, y - v.y, z - v.z);
	}
	Vector& operator-=(const Vector &v) {
		Assert(!v.hasNaNs());
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}
	// *
	Vector operator*(float f) const {
		Assert(!isnan(f));
		return Vector(f*x, f*y, f*z);
	}
	Vector &operator*=(float f) {
		Assert(!isnan(f));
		x *= f; y *= f; z *= f;
		return *this;
	}
	friend inline Vector operator*(float f,const Vector& v)
	{
		return v*f;
	}
	// /
	Vector operator/(float f) const {
		Assert(f != 0);
		float inv = 1.f / f;
		return Vector(x * inv, y * inv, z * inv);
	}
	Vector &operator/=(float f) {
		Assert(f != 0);
		float inv = 1.f / f;
		x *= inv; y *= inv; z *= inv;
		return *this;
	}
	// dot
	float operator*(const Vector& v) const
	{
		Assert(!v.hasNaNs());
		return x*v.x+y*v.y+z*v.z;
	}
	float operator*(const Normal& v) const;
	float absDot(const Vector& v) const
	{
		return fabsf(*this*v);
	}
	float absDot(const Normal& v) const;
	// cross
	Vector operator^(const Vector& v) const
	{
		Assert(!v.hasNaNs());
		return (Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x));
	}
	Vector operator^(const Normal& v) const;
	// face
	Vector faceForward(const Vector& v) const
	{
		return (*this*v<0.f)?-*this:*this;
	}
	Vector faceForward(const Normal& v) const;
	// index
	float operator[](int i) const {
		Assert(i >= 0 && i <= 2);
		return (&x)[i];
	}
	float &operator[](int i) {
		Assert(i >= 0 && i <= 2);
		return (&x)[i];
	}
	// normalize
	float length2() const { Assert(!hasNaNs()); return x*x + y*y + z*z; }
	float length() const { Assert(!hasNaNs()); return sqrtf(length2()); }
	Vector& hat()
	{
		return (*this/=length());
	}
	float normalize()
	{
		float ret=length();
		*this/=ret;
		return ret;
	}
	static inline float normalize(float* v)
	{
		// todo: error checking??
		float len=sqrtf(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
		Assert(!isnan(len)&&len!=0.f);
		float inv = 1.f / len;
		v[0]*=inv;v[1]*=inv;v[2]*=inv;
		return len;
	}
	float normalize2()
	{
		float ret=length2();
		*this/=sqrtf(ret);
		return ret;
	}
	// equal
	bool operator==(const Vector &v) const {
		return x == v.x && y == v.y && z == v.z;
	}
	bool operator!=(const Vector &v) const {
		return x != v.x || y != v.y || z != v.z;
	}

	// Vector Public Data
	float x, y, z;
};

class Point {
public:
	// Point Public Methods
	Point() { x = y = z = 0.f; }
	explicit Point(float f) { x = y = z = f; }
	explicit Point(float* f) { x=f[0];y=f[1];z=f[2]; }
	explicit Point(double* f) { x=(float)f[0];y=(float)f[1];z=(float)f[2]; }
	Point(float xx, float yy, float zz)
		: x(xx), y(yy), z(zz) {
			Assert(!hasNaNs());
	}
#ifndef NDEBUG
	Point(const Point &p) {
		Assert(!p.hasNaNs());
		x = p.x; y = p.y; z = p.z;
	}

	Point &operator=(const Point &p) {
		Assert(!p.hasNaNs());
		x = p.x; y = p.y; z = p.z;
		return *this;
	}
#endif // !NDEBUG

	bool hasNaNs() const {
		return isnan(x) || isnan(y) || isnan(z);
	}

	// +
	Point operator+(const Vector &v) const {
		Assert(!v.hasNaNs());
		return Point(x + v.x, y + v.y, z + v.z);
	}
	Point &operator+=(const Vector &v) {
		Assert(!v.hasNaNs());
		x += v.x; y += v.y; z += v.z;
		return *this;
	}
	// -
	Vector operator-(const Point &p) const {
		Assert(!p.hasNaNs());
		return Vector(x - p.x, y - p.y, z - p.z);
	}
	Point operator-(const Vector &v) const {
		Assert(!v.hasNaNs());
		return Point(x - v.x, y - v.y, z - v.z);
	}
	Point &operator-=(const Vector &v) {
		Assert(!v.hasNaNs());
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}
	// lerp
	Point &operator+=(const Point &p) {
		Assert(!p.hasNaNs());
		x += p.x; y += p.y; z += p.z;
		return *this;
	}
	Point operator+(const Point &p) const {
		Assert(!p.hasNaNs());
		return Point(x + p.x, y + p.y, z + p.z);
	}
	// *
	Point operator* (float f) const
	{
		Assert(!isnan(f));
		return Point(f*x, f*y, f*z);
	}
	Point &operator*=(float f) {
		Assert(!isnan(f));
		x *= f; y *= f; z *= f;
		return *this;
	}
	friend inline Point operator*(float f,const Point& p) {
		return p*f;
	}
	// /
	Point operator/ (float f) const {
		Assert(f!=0.f&&!isnan(f));
		float inv = 1.f/f;
		return Point(inv*x, inv*y, inv*z);
	}
	Point &operator/=(float f) {
		Assert(f!=0.f&&!isnan(f));
		float inv = 1.f/f;
		x *= inv; y *= inv; z *= inv;
		return *this;
	}
	// index
	float operator[](int i) const {
		Assert(i >= 0 && i <= 2);
		return (&x)[i];
	}
	float &operator[](int i) {
		Assert(i >= 0 && i <= 2);
		return (&x)[i];
	}
	// distance
	float distance(const Point& p) const
	{
		return sqrtf((x-p.x)*(x-p.x)+(y-p.y)*(y-p.y)+(z-p.z)*(z-p.z));
	}
	float distance2(const Point& p) const
	{
		return ((x-p.x)*(x-p.x)+(y-p.y)*(y-p.y)+(z-p.z)*(z-p.z));
	}
	// equal
	bool operator==(const Point &p) const {
		return x == p.x && y == p.y && z == p.z;
	}
	bool operator!=(const Point &p) const {
		return x != p.x || y != p.y || z != p.z;
	}

	// Point Public Data
	float x, y, z;
};

class Normal {
public:
	// Normal Public Methods
	Normal() { x = y = z = 0.f; }
	explicit Normal(float f) { x = y = z = f; }
	explicit Normal(float* f) { x=f[0];y=f[1];z=f[2]; }
	explicit Normal(double* f) { x=(float)f[0];y=(float)f[1];z=(float)f[2]; }
	Normal(float xx, float yy, float zz)
		: x(xx), y(yy), z(zz) {
			Assert(!hasNaNs());
	}
#ifndef NDEBUG
	Normal(const Normal &n) {
		Assert(!n.hasNaNs());
		x = n.x; y = n.y; z = n.z;
	}

	Normal &operator=(const Normal &n) {
		Assert(!n.hasNaNs());
		x = n.x; y = n.y; z = n.z;
		return *this;
	}
#endif // !NDEBUG
	explicit Normal(const Vector &v)
		: x(v.x), y(v.y), z(v.z) {
			Assert(!v.hasNaNs());
	}
	explicit Normal(const Point &v)
		: x(v.x), y(v.y), z(v.z) {
			Assert(!v.hasNaNs());
	}

	bool hasNaNs() const {
		return isnan(x) || isnan(y) || isnan(z);
	}
	// unary -
	Normal operator-() const {
		return Normal(-x, -y, -z);
	}
	// +
	Normal operator+ (const Normal &n) const {
		Assert(!n.hasNaNs());
		return Normal(x + n.x, y + n.y, z + n.z);
	}
	Normal& operator+=(const Normal &n) {
		Assert(!n.hasNaNs());
		x += n.x; y += n.y; z += n.z;
		return *this;
	}
	// -
	Normal operator- (const Normal &n) const {
		Assert(!n.hasNaNs());
		return Normal(x - n.x, y - n.y, z - n.z);
	}
	Normal& operator-=(const Normal &n) {
		Assert(!n.hasNaNs());
		x -= n.x; y -= n.y; z -= n.z;
		return *this;
	}
	// *
	Normal operator*(float f) const {
		return Normal(f*x, f*y, f*z);
	}
	Normal &operator*=(float f) {
		x *= f; y *= f; z *= f;
		return *this;
	}
	friend inline Normal operator*(float f,const Normal& v)
	{
		return v*f;
	}
	// /
	Normal operator/(float f) const {
		Assert(f!=0&&!isnan(f));
		float inv = 1.f/f;
		return Normal(x * inv, y * inv, z * inv);
	}
	Normal &operator/=(float f) {
		Assert(f!=0&&!isnan(f));
		float inv = 1.f/f;
		x *= inv; y *= inv; z *= inv;
		return *this;
	}
	// dot
	float operator*(const Vector& v) const
	{
		Assert(!v.hasNaNs());
		return x*v.x+y*v.y+z*v.z;
	}
	float operator*(const Normal& v) const
	{
		Assert(!v.hasNaNs());
		return x*v.x+y*v.y+z*v.z;
	}
	float absDot(const Vector& v) const
	{
		return fabsf(*this*v);
	}
	float absDot(const Normal& v) const
	{
		return fabsf(*this*v);
	}
	// cross
	Vector operator^(const Vector& v) const
	{
		Assert(!v.hasNaNs());
		return (Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x));
	}
	Vector operator^(const Normal& v) const
	{
		Assert(!v.hasNaNs());
		return (Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x));
	}
	// face
	Normal faceForward(const Vector& v) const
	{
		return (*this*v<0.f)?-*this:*this;
	}
	Normal faceForward(const Normal& v) const
	{
		return (*this*v<0.f)?-*this:*this;
	}
	// equal
	bool operator==(const Normal &n) const {
		return x == n.x && y == n.y && z == n.z;
	}
	bool operator!=(const Normal &n) const {
		return x != n.x || y != n.y || z != n.z;
	}
	// normalize
	float length2() const { return x*x + y*y + z*z; }
	float length() const { return sqrtf(length2()); }
	Normal& hat()
	{
		return (*this/=length());
	}
	float normalize()
	{
		float ret=length();
		*this/=ret;
		return ret;
	}
	inline void set(float x,float y,float z)
	{
		this->x=x;this->y=y;this->z=z;
	}
	inline void set_and_normalize(float x,float y,float z)
	{
		this->x=x;this->y=y;this->z=z;
		normalize();
	}
	float normalize2()
	{
		float ret=length2();
		*this/=sqrtf(ret);
		return ret;
	}
	// index
	float operator[](int i) const {
		Assert(i >= 0 && i <= 2);
		return (&x)[i];
	}
	float &operator[](int i) {
		Assert(i >= 0 && i <= 2);
		return (&x)[i];
	}

	// Normal Public Data
	float x, y, z;
};


inline Vector::Vector(const Point &p)
: x(p.x), y(p.y), z(p.z) {
	Assert(!hasNaNs());
}
inline Vector::Vector(const Normal &n)
: x(n.x), y(n.y), z(n.z) {
	Assert(!hasNaNs());
}
inline float Vector::operator*(const Normal& v) const
{
	Assert(!v.hasNaNs());
	return x*v.x+y*v.y+z*v.z;
}
inline float Vector::absDot(const Normal& v) const
{
	return fabsf(*this*v);
}
inline Vector Vector::operator^(const Normal& v) const
{
	Assert(!v.hasNaNs());
	return (Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x));
}
inline Vector Vector::faceForward(const Normal& v) const
{
	return (*this*v<0.f)?-*this:*this;
}


class BBox
{
public:
	BBox():p_min(INFINITY),p_max(-INFINITY){}
	BBox(const Point& p):p_min(p),p_max(p){}
	BBox(const Point& p1,const Point& p2)
	{
		asc_order(p1.x,p2.x,p_min.x,p_max.x);
		asc_order(p1.y,p2.y,p_min.y,p_max.y);
		asc_order(p1.z,p2.z,p_min.z,p_max.z);
	}
	BBox operator+(const BBox& b) const
	{
		BBox ret;
		ret.p_min.x=min(p_min.x,b.p_min.x);
		ret.p_min.y=min(p_min.y,b.p_min.y);
		ret.p_min.z=min(p_min.z,b.p_min.z);

		ret.p_max.x=min(p_max.x,b.p_max.x);
		ret.p_max.y=min(p_max.y,b.p_max.y);
		ret.p_max.z=min(p_max.z,b.p_max.z);
		
		return ret;
	}
	friend inline BBox operator+(const Point& p,const BBox& b)
	{
		return b+p;
	}
	BBox operator+(const Point& b) const
	{
		BBox ret;
		ret.p_min.x=min(p_min.x,b.x);
		ret.p_min.y=min(p_min.y,b.y);
		ret.p_min.z=min(p_min.z,b.z);

		ret.p_max.x=min(p_max.x,b.x);
		ret.p_max.y=min(p_max.y,b.y);
		ret.p_max.z=min(p_max.z,b.z);

		return ret;
	}
	BBox& operator+=(const BBox& b)
	{
		if(b.p_min.x<p_min.x) p_min.x=b.p_min.x;
		if(b.p_min.y<p_min.y) p_min.y=b.p_min.y;
		if(b.p_min.z<p_min.z) p_min.z=b.p_min.z;

		if(b.p_max.x>p_max.x) p_max.x=b.p_max.x;
		if(b.p_max.y>p_max.y) p_max.y=b.p_max.y;
		if(b.p_max.z>p_max.z) p_max.z=b.p_max.z;
		return *this;
	}
	BBox& operator+=(const Point& b)
	{
		if(b.x<p_min.x) p_min.x=b.x;
		else if(b.x>p_max.x) p_max.x=b.x;

		if(b.y<p_min.y) p_min.y=b.y;
		else if(b.y>p_max.y) p_max.y=b.y;

		if(b.z<p_min.z) p_min.z=b.z;
		else if(b.z>p_max.z) p_max.z=b.z;
		return *this;
	}
	bool overlap(const BBox& b) const
	{
		return (p_max.x >= b.p_min.x) && (p_min.x <= b.p_max.x) &&
			(p_max.y >= b.p_min.y) && (p_min.y <= b.p_max.y) &&
			(p_max.z >= b.p_min.z) && (p_min.z <= b.p_max.z);
	}
	void expand(float d)
	{
		Vector v(d);
		p_min-=v;
		p_max+=v;
	}
	int maxAxis() const
	{
		Vector d=diagonal();
		return d.x>d.y?(d.x>d.z?0:2):(d.y>d.z?1:2);
	}
	float volume() const
	{
		Vector d=diagonal();
		return d.x*d.y*d.z;
	}
	float area() const
	{
		Vector d=diagonal();
		return 2.f*(d.x*(d.y+d.z)+d.y*d.z);
	}
	Vector diagonal() const
	{
		return p_max-p_min;
	}
	// lerp
	Point operator()(float tx,float ty,float tz) const
	{
		return Point(lerp(tx,p_min.x,p_max.x),
					lerp(ty,p_min.y,p_max.y),
					lerp(tz,p_min.z,p_max.z));
	}
	Vector offset(const Point& p) const
	{
		return Vector((p.x - p_min.x) / (p_max.x - p_min.x),
			(p.y - p_min.y) / (p_max.y - p_min.y),
			(p.z - p_min.z) / (p_max.z - p_min.z));
	}
	bool inside(const Point& pt) const
	{
		return pt.x>=p_min.x&&pt.x<=p_max.x&&
			pt.y>=p_min.y&&pt.y<=p_max.y&&
			pt.z>=p_min.z&&pt.z<=p_max.z;
	}

	void boundingSphere(Point *c, float *rad) const
	{
		*c =  .5f * (p_min+p_max);
		*rad = inside(*c) ? c->distance(p_max) : 0.f;
	}
	
	bool operator==(const BBox &b) const {
		return b.p_min == p_min && b.p_max == p_max;
	}
	bool operator!=(const BBox &b) const {
		return b.p_min != p_min || b.p_max != p_max;
	}

	const Point &operator[](int i) const
	{
		Assert(i == 0 || i == 1);
		return (&p_min)[i];
	}
	Point &operator[](int i)
	{
		Assert(i == 0 || i == 1);
		return (&p_max)[i];
	}
	Point p_min,p_max;
};



#endif