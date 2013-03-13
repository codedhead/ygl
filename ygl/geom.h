#ifndef _YGL_GEOMETRY_
#define _YGL_GEOMETRY_

#include "global.h"

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
	void do_negate()
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
/*
inline void coordSystem(const Vector& v1,Vector *v2,Vector *v3)
{
	// assume normalized
	//v1.normalize();
	if (fabsf(v1.x) > fabsf(v1.y)) {
		float invLen = 1.f / sqrtf(v1.x*v1.x + v1.z*v1.z);
		*v2 = Vector(-v1.z * invLen, 0.f, v1.x * invLen);
	}
	else {
		float invLen = 1.f / sqrtf(v1.y*v1.y + v1.z*v1.z);
		*v2 = Vector(0.f, v1.z * invLen, -v1.y * invLen);
	}
	*v3 = v1^*v2;
}

inline Vector spherical2cartesian(float sintheta,
								 float costheta, float phi) {
	return Vector(sintheta * cosf(phi),
		sintheta * sinf(phi),
		costheta);
}

inline Vector spherical2cartesian(float sintheta, float costheta,
								 float phi, const Vector &x,
								 const Vector &y, const Vector &z) {
	return (sintheta * cosf(phi)) * x +
		(sintheta * sinf(phi)) * y + costheta * z;
}

inline float sphericalTheta(const Vector &v) {
	return acosf(clamp(v.z, -1.f, 1.f));
}

inline float sphericalPhi(const Vector &v) {
	float p = atan2f(v.y, v.x);
	return (p < 0.f) ? p + 2.f*M_PI : p;
}

inline float w2costheta(const Vector& w)
{
	return w.z;
}
inline float w2abscostheta(const Vector& w)
{
	return fabsf(w.z);
}
inline float spherical_uv(const Vector& w,float* theta,float* phi)
{
	*theta=acosf(clamp(w.z,-1.f,1.f));
	*phi=atan2f(w.y,w.x);
	if(*phi<0.f) *phi+=2.f*M_PI;
}
*/
#endif