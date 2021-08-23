
#pragma once

#include <stdlib.h>
#include <math.h>

struct Vector
{
public:

	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		
		float v[3];
	};

	Vector();
	Vector(float f);
	Vector(double d);
	Vector(float x, float y);
	Vector(float x, float y, float z);
	Vector(const float f[3]);
	Vector(const double d[3]);
	Vector(const Vector & v);
			
	void Set(float f);
	void Set(double d);	
	void Set(float x, float y);		
	void Set(float x, float y, float z);		
	void Set(const float f[3]);	
	void Set(const double d[3]);	
	void Set(const Vector& v);

public:

	float operator ~ () const;

	Vector operator ! () const;

	Vector operator - () const;

	Vector & operator = (float f);
	
	Vector & operator = (double d);
	
	Vector & operator = (const Vector & v);
	
	Vector & operator += (float f);
	
	Vector & operator += (double d);
	
	Vector & operator += (const Vector & v);
	
	Vector & operator -= (float f);
	
	Vector & operator -= (double d);
	
	Vector & operator -= (const Vector & v);
	
	Vector & operator *= (float f);
	
	Vector & operator *= (double d);
	
	Vector & operator *= (const Vector & v);
	
	Vector & operator /= (float f);
	
	Vector & operator /= (double d);
	
	Vector & operator /= (const Vector & v);
	
	Vector & operator |= (const Vector & v);

	Vector & operator ^= (const Vector & v);


public:

	float Normalize();
	float NormalizeXZ();

	inline Vector& Inverse();
	inline Vector& InverseX();
	inline Vector& InverseY();
	inline Vector& InverseZ();
	inline Vector& InverseXZ();
	inline Vector InverseCopy();

	float ClampLength(float clampValue);
	
	Vector & ClampX(float min, float max);
	Vector & ClampY(float min, float max);
	Vector & ClampZ(float min, float max);
	Vector & Clamp(float min, float max);

	Vector & Min(const Vector & v);
	Vector & Max(const Vector & v);
	Vector & Min(const Vector & v1, const Vector & v2);
	Vector & Max(const Vector & v1, const Vector & v2);

	Vector & Abs();
	
	float GetAngle(const Vector & v) const;
	float GetAngleXZ(const Vector & v) const;
	float GetAY(float defAngle = 0.0f) const;
	
	float SinXZ(const Vector & v) const;
	float CosXZ(const Vector & v) const;
	float CrossXZ(const Vector & v) const;

	Vector GetXZ() const;
	Vector GetXZ(float y) const;
	Vector & MakeXZ(float ay);


	float Length() const;
	float Length2() const;
	float LengthXZ() const;
	float LengthXZ2() const;
	
	Vector & Rotate(float angle);
	
	Vector & BuildRotate(float alpha,float betta);

	Vector & Rotate(float vcos, float vsin);
	Vector & Rotate_PI2_CW();
	Vector & Rotate_PI2_CCW();

	Vector & Lerp(const Vector & from, const Vector & to, float kBlend);

	float GetAttenuation(float minDist, float kLen);
	float GetAttenuation2(float minDist, float kLen);
	float GetAttenuation3(float minDist, float kLen);
	float GetAttenuation4(float minDist, float kLen);
	float GetAttenuationXZ(float minDist, float kLen);
	float GetAttenuation2XZ(float minDist, float kLen);
	float GetAttenuation3XZ(float minDist, float kLen);
	float GetAttenuation4XZ(float minDist, float kLen);

	Vector & Reflection(const Vector & normal);

	Vector & Rand();
	Vector & RandXZ();
	Vector & Rand(const Vector & min, const Vector & max);
	Vector & Rand(const Vector & pos, float radius);

	bool MoveByStep(const Vector & to, float step);
	bool MoveByStepXZ(const Vector & to, float step);

	bool InBox(const Vector & min, const Vector & max) const;
	bool InSphere(const Vector & pos, float rad) const;

	Vector & Friction(float k, float power);
	Vector & FrictionXZ(float k, float power);

	bool Equals( const Vector & pos, float delta );
};


inline Vector::Vector(){}
inline Vector::Vector(float f)
{
	x = f;
	y = f;
	z = f;
}

inline Vector::Vector(double d)
{
	x = float(d);
	y = float(d);
	z = float(d);
}

inline Vector::Vector(float x, float y)
{
	this->x = x;
	this->y = y;
	this->z = 0.0f;
}

inline Vector::Vector(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

inline Vector::Vector(const float f[3])
{
	x = f[0];
	y = f[1];
	z = f[2];
}

inline Vector::Vector(const double d[3])
{
	x = float(d[0]);
	y = float(d[1]);
	z = float(d[2]);
}

inline Vector::Vector(const Vector & v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

inline void Vector::Set(float f)
{
	x = f;
	y = f;
	z = f;
}

inline void Vector::Set(double d)
{
	x = float(d);
	y = float(d);
	z = float(d);
}

inline void Vector::Set(float x, float y)
{
	this->x = x;
	this->y = y;
	this->z = 0.0f;
}

inline void Vector::Set(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

inline void Vector::Set(const float f[3])
{
	x = f[0];
	y = f[1];
	z = f[2];
}

inline void Vector::Set(const double d[3])
{
	x = float(d[0]);
	y = float(d[1]);
	z = float(d[2]);
}

inline void Vector::Set(const Vector & v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

#ifdef PC

/*inline void Vector::Set(const D3DXVECTOR3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

inline void Vector::Set(const NxVec3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}*/

#endif

inline float Vector::operator ~ () const
{
	return x*x + y*y + z*z;
}

inline Vector Vector::operator ! () const
{
	Vector v(*this);
	v.Normalize();
	return v;
}

inline Vector Vector::operator - () const
{
	Vector v(*this);
	v.x = -v.x;
	v.y = -v.y;
	v.z = -v.z;
	return v;
}

inline Vector & Vector::operator = (float f)
{
	x = f;
	y = f;
	z = f;
	return *this;
}

inline Vector & Vector::operator = (double d)
{
	x = float(d);
	y = float(d);
	z = float(d);
	return *this;
}

inline Vector & Vector::operator = (const Vector & v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

inline Vector & Vector::operator += (float f)
{
	x += f;
	y += f;
	z += f;
	return *this;
}

inline Vector & Vector::operator += (double d)
{
	x += float(d);
	y += float(d);
	z += float(d);
	return *this;
}

inline Vector & Vector::operator += (const Vector & v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

inline Vector & Vector::operator -= (float f)
{
	x -= f;
	y -= f;
	z -= f;
	return *this;
}

inline Vector & Vector::operator -= (double d)
{
	x -= float(d);
	y -= float(d);
	z -= float(d);
	return *this;
}

inline Vector & Vector::operator -= (const Vector & v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

inline Vector & Vector::operator *= (float f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

inline Vector & Vector::operator *= (double d)
{
	x *= float(d);
	y *= float(d);
	z *= float(d);
	return *this;
}

inline Vector & Vector::operator *= (const Vector & v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	return *this;
}

inline Vector & Vector::operator /= (float f)
{
	double d = 1.0/f;
	x = float(x*d);
	y = float(y*d);
	z = float(z*d);
	return *this;
}

inline Vector & Vector::operator /= (double d)
{
	d = 1.0/d;
	x = float(x*d);
	y = float(y*d);
	z = float(z*d);
	return *this;
}

inline Vector & Vector::operator /= (const Vector & v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
	return *this;
}

inline Vector & Vector::operator |= (const Vector & v)
{
	x = y = z = v.x*x + v.y*y + v.z*z;
	return *this;
}

inline Vector & Vector::operator ^= (const Vector & v)
{
	float tx = y * v.z - z * v.y;
	float ty = z * v.x - x * v.z;
	float tz = x * v.y - y * v.x;
	x = tx;
	y = ty;
	z = tz;
	return *this;
}

inline Vector operator + (const Vector & v, float f)
{
	Vector tv(v);
	tv += f;
	return tv;
}

inline Vector operator + (float f, const Vector & v)
{
	Vector tv(v);
	tv += f;
	return tv;
}

inline Vector operator + (const Vector & v, double d)
{
	Vector tv(v);
	tv += d;
	return tv;
}

inline Vector operator + (double d, const Vector & v)
{
	Vector tv(v);
	tv += d;
	return tv;
}

inline Vector operator + (const Vector & v1, const Vector & v2)
{
	Vector tv(v1);
	tv += v2;
	return tv;
}

inline Vector operator - (const Vector & v, float f)
{
	Vector tv(v);
	tv -= f;
	return tv;
}

inline Vector operator - (float f, const Vector & v)
{
	Vector tv(v);
	tv -= f;
	return tv;
}

inline Vector operator - (const Vector & v, double d)
{
	Vector tv(v);
	tv -= d;
	return tv;
}

inline Vector operator - (double d, const Vector & v)
{
	Vector tv(v);
	tv -= d;
	return tv;
}

inline Vector operator - (const Vector & v1, const Vector & v2)
{
	Vector tv(v1);
	tv -= v2;
	return tv;
}

inline Vector operator * (const Vector & v, float f)
{
	Vector tv(v);
	tv *= f;
	return tv;
}

inline Vector operator * (float f, const Vector & v)
{
	Vector tv(v);
	tv *= f;
	return tv;
}

inline Vector operator * (const Vector & v, double d)
{
	Vector tv(v);
	tv *= d;
	return tv;
}

inline Vector operator * (double d, const Vector & v)
{
	Vector tv(v);
	tv *= d;
	return tv;
}

inline Vector operator * (const Vector & v1, const Vector & v2)
{
	Vector tv(v1);
	tv *= v2;
	return tv;
}

inline Vector operator / (const Vector & v, float f)
{
	Vector tv(v);
	tv /= f;
	return tv;
}

inline Vector operator / (float f, const Vector & v)
{
	Vector tv;
	tv.Set(f);
	tv /= v;
	return tv;
}

inline Vector operator / (const Vector & v, double d)
{
	Vector tv;
	tv.Set(v);
	tv /= d;
	return tv;
}

inline Vector operator / (double d, const Vector & v)
{
	Vector tv;
	tv.Set(d);
	tv /= v;
	return tv;
}

inline Vector operator / (const Vector & v1, const Vector & v2)
{
	Vector tv;
	tv.Set(v1);
	tv /= v2;
	return tv;
}

inline float operator | (const Vector & v1, const Vector & v2)
{	
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

inline Vector operator ^ (const Vector & v1, const Vector & v2)
{
	Vector v;
	v.x = v1.y*v2.z - v1.z*v2.y;
	v.y = v1.z*v2.x - v1.x*v2.z;
	v.z = v1.x*v2.y - v1.y*v2.x;
	return v;
}

inline bool operator > (const Vector & v1, const Vector & v2)
{
	if(v1.x > v2.x && v1.y > v2.y && v1.z > v2.z) return true;	
	return false;
}

inline bool operator >= (const Vector & v1, const Vector & v2)
{
	if(v1.x >= v2.x && v1.y >= v2.y && v1.z >= v2.z) return true;	
	return false;
}

inline bool operator < (const Vector & v1, const Vector & v2)
{
	if(v1.x < v2.x && v1.y < v2.y && v1.z < v2.z) return true;	
	return false;
}

inline bool operator <= (const Vector & v1, const Vector & v2)
{
	if(v1.x <= v2.x && v1.y <= v2.y && v1.z <= v2.z) return true;	
	return false;
}

inline bool operator == (const Vector & v1, const Vector & v2)
{
	if(fabsf(v1.x - v2.x) < 1e-34f && fabsf(v1.y - v2.y) < 1e-34f && fabsf(v1.z - v2.z) < 1e-34f) return true;
	return false;
}

inline bool operator != (const Vector & v1, const Vector & v2)
{	
	return !(v1 == v2);
}

inline float Vector::Normalize()
{
	float len = sqrtf(x*x + y*y + z*z);
	if(len > 0.0)
	{
		float k = 1.0f/len;
		x = float(x*k);
		y = float(y*k);
		z = float(z*k);
	}
	return float(len);
}

inline Vector& Vector::Inverse()
{
	x = -x;
	y = -y;
	z = -z;

	return *this;
}

inline Vector& Vector::InverseXZ()
{
	x = -x;
	z = -z;

	return *this;
}

inline Vector& Vector::InverseX()
{
	x = -x;

	return *this;
}

inline Vector& Vector::InverseY()
{
	y = -y;

	return *this;
}

inline Vector& Vector::InverseZ()
{
	z = -z;

	return *this;
}

inline Vector Vector::InverseCopy()
{
	return Vector(-x, -y, -z);
}

inline float Vector::NormalizeXZ()
{
	y = 0.0f;
	float len = sqrtf(x*x + z*z);
	if(len > 0.0)
	{
		float k = 1.0f/len;
		x = float(x*k);
		z = float(z*k);
	}
	return float(len);
}

inline float Vector::ClampLength(float clampValue)
{
	if(clampValue <= 0.0f)
	{
		x = y = z = 0.0f;
		return 0.0f;
	}
	float len = x*x + y*y + z*z;
	if(len > clampValue*clampValue)
	{
		float k = clampValue/sqrtf(len);
		x = float(x*k);
		y = float(y*k);
		z = float(z*k);
		return clampValue;
	}
	return float(len);
}

inline Vector & Vector::ClampX(float min, float max)
{
	if(x < min) x = min;
	if(x > max) x = max;
	return *this;
}

inline Vector & Vector::ClampY(float min, float max)
{
	if(y < min) y = min;
	if(y > max) y = max;
	return *this;
}

inline Vector & Vector::ClampZ(float min, float max)
{
	if(z < min) z = min;
	if(z > max) z = max;
	return *this;
}

inline Vector & Vector::Clamp(float min, float max)
{
	if(x < min) x = min;
	if(x > max) x = max;
	if(y < min) y = min;
	if(y > max) y = max;
	if(z < min) z = min;
	if(z > max) z = max;
	return *this;
}

inline Vector & Vector::Min(const Vector & v)
{
	if(x > v.x) x = v.x;
	if(y > v.y) y = v.y;
	if(z > v.z) z = v.z;
	return *this;
}

inline Vector & Vector::Max(const Vector & v)
{
	if(x < v.x) x = v.x;
	if(y < v.y) y = v.y;
	if(z < v.z) z = v.z;
	return *this;
}

inline Vector & Vector::Min(const Vector & v1, const Vector & v2)
{
	x = (v1.x < v2.x) ? v1.x : v2.x;
	y = (v1.y < v2.y) ? v1.y : v2.y;
	z = (v1.z < v2.z) ? v1.z : v2.z;
	return *this;
}

inline Vector & Vector::Max(const Vector & v1, const Vector & v2)
{
	x = (v1.x > v2.x) ? v1.x : v2.x;
	y = (v1.y > v2.y) ? v1.y : v2.y;
	z = (v1.z > v2.z) ? v1.z : v2.z;
	return *this;
}

inline Vector & Vector::Abs()
{
	x = fabsf(x);
	y = fabsf(y);
	z = fabsf(z);
	return *this;
}

inline float Vector::GetAngle(const Vector & v) const
{
	double len = double(x)*double(x) + double(y)*double(y) + double(z)*double(z);
	len *= double(x)*double(x) + double(y)*double(y) + double(z)*double(z);
	if(len <= 0.0) return 0.0f;
	double cs = (x*v.x + y*v.y + z*v.z)/sqrt(len);
	if(cs > 1.0) cs = 1.0;
	if(cs < -1.0) cs = -1.0;
	return float(acos(cs));
}

inline float Vector::GetAngleXZ(const Vector & v) const
{
	double len = double(x)*double(x) + double(z)*double(z);
	len *= double(v.x)*double(v.x) + double(v.z)*double(v.z);
	if(len <= 0.0) return 0.0f;
	len = (x*v.x + z*v.z)/sqrt(len);
	if(len > 1.0) len = 1.0;
	if(len < -1.0) len = -1.0;
	len = acos(len);
	if(z*v.x - x*v.z < 0) len = -len;
	return float(len);
}

inline float Vector::GetAY(float defAngle) const
{
	double len = double(x)*double(x) + double(z)*double(z);
	if(len > 0.00000000001)
	{
		len = z/sqrt(len);
		if(len > 1.0) len = 1.0;
		if(len < -1.0) len = -1.0;
		len = acos(len);
	}else return defAngle;
	if(x < 0) len = -len;
	return float(len);
}

inline float Vector::SinXZ(const Vector & v) const
{
	double len = double(x)*double(x) + double(z)*double(z);
	len *= double(v.x)*double(v.x) + double(v.z)*double(v.z);
	if(len <= 0.0) return 0.0f;
	len = (z*v.x - x*v.z)/sqrt(len);
	return float(len);
}

inline float Vector::CosXZ(const Vector & v) const
{
	double len = double(x)*double(x) + double(z)*double(z);
	len *= double(v.x)*double(v.x) + double(v.z)*double(v.z);
	if(len <= 0.0) return 1.0f;
	len = (x*v.x + z*v.z)/sqrt(len);
	return float(len);
}

inline float Vector::CrossXZ(const Vector & v) const
{
	return z*v.x - x*v.z;
}

inline Vector Vector::GetXZ() const
{
	Vector vc;
	vc.Set(x, 0.0f, z);
	return vc;
}

inline Vector Vector::GetXZ(float y) const
{
	Vector vc;
	vc.Set(x, y, z);
	return vc;
}

inline Vector & Vector::MakeXZ(float ay)
{
	x = sinf(ay);
	y = 0.0f;
	z = cosf(ay);
	return *this;
}

inline float Vector::Length() const
{
	return float(sqrtf(x*x + y*y + z*z));
}

inline float Vector::Length2() const
{
	return x*x + y*y + z*z;
}

inline float Vector::LengthXZ() const
{
	return float(sqrtf(x*x + z*z));
}

inline float Vector::LengthXZ2() const
{
	return x*x + z*z;
}

inline Vector & Vector::Rotate(float angle)
{
	return Rotate(cosf(angle), sinf(angle));
}

inline Vector & Vector::BuildRotate(float alpha,float betta)
{
	x=cosf(betta);
	y=sinf(betta);
	z=sinf(alpha)*x;
	
	x*=cosf(alpha);
		
	return *this;
}

inline Vector & Vector::Rotate(float vcos, float vsin)
{
	float tx = x*vcos + z*vsin;
	float tz = z*vcos - x*vsin;
	x = tx; z = tz;
	return *this;
}

inline Vector & Vector::Rotate_PI2_CW()
{
	float t = x;
	x = z;
	z = -t;
	return *this;
}

inline Vector & Vector::Rotate_PI2_CCW()
{
	float t = x;
	x = -z;
	z = t;
	return *this;
}

inline Vector & Vector::Lerp(const Vector & from, const Vector & to, float kBlend)
{
	x = from.x + (to.x - from.x)*kBlend;
	y = from.y + (to.y - from.y)*kBlend;
	z = from.z + (to.z - from.z)*kBlend;
	return *this;
}

// 1.0f - (this.Length2() - minDist)*(kLen = 1.0f/(maxDist - minDist))
inline float Vector::GetAttenuation(float minDist, float kLen)
{
	float v = x*x + y*y + z*z;
	v = 1.0f - (v - minDist)*kLen;	
	if(v < 0.0f) v = 0.0f;
	if(v > 1.0f) v = 1.0f;
	return v;
}

inline float Vector::GetAttenuation2(float minDist, float kLen)
{
	float v = GetAttenuation(minDist, kLen);
	return v*v;
}

inline float Vector::GetAttenuation3(float minDist, float kLen)
{
	float v = GetAttenuation(minDist, kLen);
	return v*v*v;
}

inline float Vector::GetAttenuation4(float minDist, float kLen)
{
	float v = GetAttenuation(minDist, kLen);
	v *= v;
	return v*v;
}

// 1.0f - (this.Length2D2() - minDist)*(kLen = 1.0f/(maxDist - minDist))
inline float Vector::GetAttenuationXZ(float minDist, float kLen)
{
	float v = x*x + z*z;
	return (v - minDist)*kLen;		
}

// pow(this.GetAttenuation(), 2)
inline float Vector::GetAttenuation2XZ(float minDist, float kLen)
{
	float v = GetAttenuationXZ(minDist, kLen);
	return v*v;
}

// pow(this.GetAttenuation(), 3)
inline float Vector::GetAttenuation3XZ(float minDist, float kLen)
{
	float v = GetAttenuationXZ(minDist, kLen);
	return v*v*v;
}

///pow(this.GetAttenuation(), 4)
inline float Vector::GetAttenuation4XZ(float minDist, float kLen)
{
	float v = GetAttenuationXZ(minDist, kLen);
	v *= v;
	return v*v;
}

inline Vector & Vector::Reflection(const Vector & normal)
{
	float k = -2.0f*(x*normal.x + y*normal.y + z*normal.z);
	x += normal.x*k;
	y += normal.y*k;
	z += normal.z*k;
	return *this;
}

inline Vector & Vector::Rand()
{
	MakeXZ(rand()*((2.0f*3.141592654f)/RAND_MAX));
	y = z; z = 0.0f;
	return Rotate(rand()*((2.0f*3.141592654f)/RAND_MAX));
}

inline Vector & Vector::RandXZ()
{
	return MakeXZ(rand()*((2.0f*3.141592654f)/RAND_MAX));
}

inline Vector & Vector::Rand(const Vector & min, const Vector & max)
{
	x = min.x + rand()*((max.x - min.x)*(1.0f/RAND_MAX));
	y = min.y + rand()*((max.y - min.y)*(1.0f/RAND_MAX));
	z = min.z + rand()*((max.z - min.z)*(1.0f/RAND_MAX));
	return *this;
}

inline Vector & Vector::Rand(const Vector & pos, float radius)
{
	Rand();
	*this *= rand()*(radius*(1.0f/RAND_MAX));
	return *this;
}

inline bool Vector::MoveByStep(const Vector & to, float step)
{
	if(step <= 0.0f) return false;
	
	float dx = to.x - x;
	float dy = to.y - y;
	float dz = to.z - z;
	double dist = dx*dx + dy*dy + dz*dz;
	if(dist < 1e-30)
	{
		x = to.x;
		y = to.y;
		z = to.z;
		return false;
	}
	dist = sqrt(dist);
	
	if(step >= dist)
	{
		x = to.x;
		y = to.y;
		z = to.z;
		return true;
	}
	double k = step/dist;
	x += float(dx*k);
	y += float(dy*k);
	z += float(dz*k);
	return true;
}

inline bool Vector::MoveByStepXZ(const Vector & to, float step)
{
	if(step <= 0.0f) return false;
	
	float dx = to.x - x;
	float dz = to.z - z;
	double dist = dx*dx + dz*dz;
	if(dist < 1e-30)
	{
		x = to.x;
		z = to.z;
		return false;
	}
	dist = sqrt(dist);
	
	if(step >= dist)
	{
		x = to.x;
		z = to.z;
		return true;
	}
	double k = step/dist;
	x += float(dx*k);
	z += float(dz*k);
	return true;
}

inline bool Vector::InBox(const Vector & min, const Vector & max) const
{
	if(y < min.y || y > max.y) return false;
	if(x < min.x || x > max.x) return false;
	if(z < min.z || z > max.z) return false;
	return true;
}

inline bool Vector::InSphere(const Vector & pos, float rad) const
{
	double d = (x - pos.x)*(x - pos.x);
	d += (y - pos.y)*(y - pos.y);
	d += (z - pos.z)*(z - pos.z);
	return d < double(rad)*double(rad);
}

inline Vector & Vector::Friction(float k, float power)
{
	Vector sign;
	Vector v;
	if(x >= 0.0f)
	{
		sign.x = 1.0f;
		v.x = x;
	}else{
		sign.x = -1.0f;
		v.x = -x;
	}
	if(y >= 0.0f)
	{
		sign.y = 1.0f;
		v.y = y;
	}else{
		sign.y = -1.0f;
		v.y = -y;
	}
	if(z >= 0.0f)
	{
		sign.z = 1.0f;
		v.z = z;
	}else{
		sign.z = -1.0f;
		v.z = -z;
	}
	
	if(power < 1e-10f)
	{
		power = 1e-10f;
	}
	x = v.x - powf(v.x, power)*k;
	y = v.y - powf(v.y, power)*k;
	z = v.z - powf(v.z, power)*k;
	if(x < 0.0f) x = 0.0f;
	if(y < 0.0f) y = 0.0f;
	if(z < 0.0f) z = 0.0f;
	x *= sign.x;
	y *= sign.y;
	z *= sign.z;
	return *this;
}

inline Vector & Vector::FrictionXZ(float k, float power)
{
	Vector sign;
	Vector v;
	
	if(x >= 0.0f)
	{
		sign.x = 1.0f;
		v.x = x;
	}
	else
	{
		sign.x = -1.0f;
		v.x = -x;
	}
	
	v.y = 0.0f;
	sign.y = 0.0f;
	
	if(z >= 0.0f)
	{
		sign.z = 1.0f;
		v.z = z;
	}
	else
	{
		sign.z = -1.0f;
		v.z = -z;
	}

	if(power < 1e-10f)
	{
		power = 1e-10f;
	}
	
	x = v.x - powf(v.x, power)*k;
	z = v.z - powf(v.z, power)*k;
	if(x < 0.0f) x = 0.0f;
	if(z < 0.0f) z = 0.0f;
	x *= sign.x;
	z *= sign.z;
	return *this;
}

inline bool Vector::Equals( const Vector & pos, float delta )
{
	return ((fabs(x - pos.x)<delta) && (fabs(y - pos.y)<delta) && (fabs(y - pos.y)<delta));
}
