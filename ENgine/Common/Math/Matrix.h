
#pragma once

#include "Vector4.h"

class Matrix
{
public:
	
	union
	{
		float matrix[16];
		float m[4][4];
		struct
		{
			float vx_x, vx_y, vx_z;
			float wx;
			float vy_x, vy_y, vy_z;
			float wy;
			float vz_x, vz_y, vz_z;
			float wz;
			float pos_x, pos_y, pos_z;
			float w;
		};
		struct
		{			
			float _11,_12,_13,_14;
			float _21,_22,_23,_24;
			float _31,_32,_33,_34;
			float _41,_42,_43,_44;			
		};
	};

	Matrix();
	Matrix(bool empty);
	Matrix(const float* data);
	Matrix(float angX, float angY, float angZ, float x, float y, float z);
	Matrix(float angX, float angY, float angZ);
	Matrix(const Vector& ang, const Vector& pos);
	Matrix(const Vector& ang);
	Matrix(const Matrix& Matrix);
	Matrix(const Matrix& m1, const Matrix& m2);

	Matrix(float m11, float m12, float m13,
			float m21, float m22, float m23,
			float m31, float m32, float m33,
			float x, float y, float z);	

	
	Vector& Vx() { return (Vector&)matrix[0]; };
	Vector& Vy() { return (Vector&)matrix[4]; };
	Vector& Vz() { return (Vector&)matrix[8]; };
	Vector& Pos() { return (Vector&)matrix[12]; };

public:

	Matrix& operator = (const Matrix& mtx);
	Matrix& operator = (float f);
	Matrix& operator = (double d);
	Matrix& operator = (const Vector& v);
	Matrix& operator *= (const Matrix& mtx);

	
public:

	Matrix& Identity();
	Matrix& Identity3x3();
	Matrix& SetZero();
	Matrix& Set(const Matrix & Matrix);	
	Matrix& Build(float angX, float angY, float angZ, float x, float y, float z);
	Matrix& Build(float angX, float angY, float angZ);
	Matrix& Build(const Vector & ang, const Vector & pos);
	Matrix& Build(const Vector & ang);
	Matrix& BuildXYZ(float angX, float angY, float angZ, float x, float y, float z);
	Matrix& BuildRotateX(float ang);
	Matrix& BuildRotateY(float ang);
	Matrix& BuildRotateZ(float ang);
	bool    BuildRotate(const Vector & axis, float ang);
	Matrix& BuildPosition(float x, float y, float z);
	Matrix& BuildPosition(const Vector & pos);
	Matrix& BuildScale(float scale);
	Matrix& BuildScale(float scaleX, float scaleY, float scaleZ);
	Matrix& BuildScale(const Vector & scale);
	Matrix& BuildProjection(float viewAngle, float aspect, float zNear, float zFar);
	Matrix& BuildProjection(float viewAngle, float vpWidth, float vpHeight, float zNear, float zFar);

	bool    BuildView(Vector lookFrom, Vector lookTo, Vector upVector);
	Matrix& BuildViewFromObject();
	bool    BuildOrient(Vector zAxisDirection, Vector upVector);
	bool    BuildOriented(Vector position, Vector lookTo, Vector upVector);
	Matrix& BuildMirror(float Nx, float Ny, float Nz, float D);
	Matrix& BuildOrtoProjection(float vpWidth, float vpHeight, float zNear, float zFar);
	Matrix& BuildShadowProjection(float viewAngle, float vpWidth, float vpHeight, float zNear, float zFar);

	Matrix& RotateX(float ang);
	Matrix& RotateY(float ang);
	Matrix& RotateZ(float ang);
	Matrix& Rotate(float angX, float angY, float angZ);
	Matrix& Rotate(const Vector & ang);
	Matrix& Move(float dX, float dY, float dZ);
	Matrix& Move(const Vector & pos);
	Matrix& Scale(float scale);
	Matrix& Scale3x3(float scale);
	Matrix& Scale(float scaleX, float scaleY, float scaleZ);
	Matrix& Scale3x3(float scaleX, float scaleY, float scaleZ);
	Matrix& Scale(const Vector & scale);
	Matrix& Scale3x3(const Vector & scale);
	Matrix& Inverse();
	Matrix& Inverse(Matrix & mtx);
	bool    InverseComplette();
	Matrix& Transposition();
	Matrix& Transposition3X3();


	Matrix& SetRotate(const Matrix & mtr);
	Matrix& EqMultiply(const Matrix & m1, const Matrix & m2);
	Matrix& EqMultiplyFast(const Matrix & m1, const Matrix & m2);

	Vector4 MulVertex4(const Vector & v) const;
	Vector  MulVertex(const Vector & v) const;
	Vector  MulNormal(const Vector & v) const;
	Vector  MulVertexByInverse(const Vector & v) const;
	Vector  MulNormalByInverse(const Vector & v) const;

	Vector  GetCamPos();
	bool    IsIdentity() const;
	bool    IsScale();
	bool    IsParity();

	Vector4 Projection(Vector vertex, float vphWidth05 = 1.0f, float vphHeight05 = 1.0f) const;
	void    Projection(Vector4 * dstArray, Vector * srcArray, long num, float vphWidth05 = 1.0f, float vphHeight05 = 1.0f, long srcSize = sizeof(Vector), long dstSize = sizeof(Vector4)) const;

	void    GetAngles(float & ax, float & ay, float & az);
	void    GetAngles(Vector & ang);
	Vector  GetAngles();

	Vector  GetScale();
	Matrix& Normalize();

	Matrix& Mul(float k);
	Matrix& Add(const Matrix & m);
	Matrix& Mad(const Matrix & m, float k);

	float&  operator () (long i, long j);
};

inline Matrix::Matrix()
{
	Identity();
}

inline Matrix::Matrix(bool empty)
{
	if (empty) Identity();
}

inline Matrix::Matrix(const float* data)
{
	for (int i=0;i<16;i++) matrix[i] = data[i];
}

inline Matrix::Matrix(float angX, float angY, float angZ, float x, float y, float z)
{
	Build(angX, angY, angZ, x, y, z);
}

inline Matrix::Matrix(float angX, float angY, float angZ)
{
	Build(angX, angY, angZ);
}

inline Matrix::Matrix(const Vector & ang, const Vector & pos)
{
	Build(ang, pos);
}

inline Matrix::Matrix(const Vector & ang)
{
	Build(ang);
}

inline Matrix::Matrix(const Matrix & Matrix)
{
	Set(Matrix);
}

inline Matrix::Matrix(const Matrix & m1, const Matrix & m2)
{
	EqMultiply(m1, m2);
}

inline Matrix::Matrix(float m11, float m12, float m13,
					  float m21, float m22, float m23,
					  float m31, float m32, float m33,
					  float x, float y, float z)
{
	_11 = m11; _12 = m12; _13 = m13; _14 = 0;
	_21 = m21; _22 = m22; _23 = m23; _24 = 0;
	_31 = m31; _32 = m32; _33 = m33; _34 = 0;
	_41 =  x ; _42 =  y ; _43 =  z ; _44 = 1;
}

inline Matrix& Matrix::operator = (const Matrix & mtx)
{
	Set(mtx);
	return *this;
}

inline Matrix& Matrix::operator = (float f)
{
	Pos().x = Pos().y = Pos().z = f;
	return *this;
}

inline Matrix& Matrix::operator = (double d)
{
	Pos().x = Pos().y = Pos().z = float(d);
	return *this;
}

inline Matrix& Matrix::operator = (const Vector & v)
{
	Pos().x = v.x;
	Pos().y = v.y;
	Pos().z = v.z;

	return *this;
}

inline Matrix& Matrix::operator *= (const Matrix & mtx)
{
	EqMultiply(*this, mtx);
	return *this;
}

inline Matrix operator * (const Matrix & m1, const Matrix & m2)
{
	Matrix m;
	m.EqMultiplyFast(m1, m2);
	return m;
}

inline Vector operator * (const Matrix & mtx, const Vector & v)
{
	return mtx.MulVertex(v);
}

inline Vector operator * (const Vector & v, const Matrix & mtx)
{
	return mtx.MulVertex(v);
}

inline Matrix & Matrix::Identity()
{
	matrix[0] = 1.0f;
	matrix[1] = 0.0f;
	matrix[2] = 0.0f;
	matrix[3] = 0.0f;

	matrix[4] = 0.0f;
	matrix[5] = 1.0f;
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;

	matrix[8] = 0.0f;
	matrix[9] = 0.0f;
	matrix[10] = 1.0f;
	matrix[11] = 0.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;

	return *this;

}

inline Matrix & Matrix::Identity3x3()
{
	matrix[0] = 1.0f;
	matrix[1] = 0.0f;
	matrix[2] = 0.0f;

	matrix[4] = 0.0f;
	matrix[5] = 1.0f;
	matrix[6] = 0.0f;

	matrix[8] = 0.0f;
	matrix[9] = 0.0f;
	matrix[10] = 1.0f;

	return *this;

}

inline Matrix & Matrix::SetZero()
{
	matrix[0] = 0.0f;
	matrix[1] = 0.0f;
	matrix[2] = 0.0f;
	matrix[3] = 0.0f;

	matrix[4] = 0.0f;
	matrix[5] = 0.0f;
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;

	matrix[8] = 0.0f;
	matrix[9] = 0.0f;
	matrix[10] = 0.0f;
	matrix[11] = 0.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 0.0f;

	return *this;

}

inline Matrix & Matrix::Set(const Matrix & matrix)
{
	this->matrix[0] = matrix.matrix[0]; 
	this->matrix[1] = matrix.matrix[1];
	this->matrix[2] = matrix.matrix[2];
	this->matrix[3] = matrix.matrix[3];

	this->matrix[4] = matrix.matrix[4]; 
	this->matrix[5] = matrix.matrix[5];
	this->matrix[6] = matrix.matrix[6];
	this->matrix[7] = matrix.matrix[7];

	this->matrix[8] = matrix.matrix[8];  
	this->matrix[9] = matrix.matrix[9]; 
	this->matrix[10] = matrix.matrix[10];
	this->matrix[11] = matrix.matrix[11];

	this->matrix[12] = matrix.matrix[12]; 
	this->matrix[13] = matrix.matrix[13];
	this->matrix[14] = matrix.matrix[14];
	this->matrix[15] = matrix.matrix[15];

	return *this;

}

inline Matrix & Matrix::Build(float angX, float angY, float angZ, float x, float y, float z)
{
	float sinAx = sinf(angX);
	float cosAx = cosf(angX);
	float sinAy = sinf(angY);
	float cosAy = cosf(angY);
	float sinAz = sinf(angZ);
	float cosAz = cosf(angZ);	
	
	m[0][0] = cosAz*cosAy + sinAz*sinAx*sinAy;		
	m[0][1] = sinAz*cosAx;							
	m[0][2] = cosAz*-sinAy + sinAz*sinAx*cosAy;		
	m[0][3] = 0.0f;
	m[1][0] = -sinAz*cosAy + cosAz*sinAx*sinAy;		
	m[1][1] = cosAz*cosAx;							
	m[1][2] = -sinAz*-sinAy + cosAz*sinAx*cosAy;	
	m[1][3] = 0.0f;
	m[2][0] = cosAx*sinAy;							
	m[2][1] = -sinAx;								
	m[2][2] = cosAx*cosAy;							
	m[2][3] = 0.0f;
	m[3][0] = x;									
	m[3][1] = y;									
	m[3][2] = z;									
	m[3][3] = 1.0f;
	return *this;
}

inline Matrix & Matrix::Build(float angX, float angY, float angZ)
{
	Build(angX, angY, angZ, 0.0f, 0.0f, 0.0f);
	return *this;
}

inline Matrix & Matrix::Build(const Vector & ang, const Vector & pos)
{
	Build(ang.x, ang.y, ang.z, pos.x, pos.y, pos.z);
	return *this;
}

inline Matrix & Matrix::Build(const Vector & ang)
{
	Build(ang.x, ang.y, ang.z, 0.0f, 0.0f, 0.0f);
	return *this;
}


inline Matrix & Matrix::BuildXYZ(float angX, float angY, float angZ, float x, float y, float z)
{
	float sinAx = sinf(angX);
	float cosAx = cosf(angX);
	float sinAy = sinf(angY);
	float cosAy = cosf(angY);
	float sinAz = sinf(angZ);
	float cosAz = cosf(angZ);
	
	m[0][0] = cosAy*cosAz;
	m[0][1] = cosAy*sinAz;
	m[0][2] = -sinAy;
	m[0][3] = 0.0f;
	m[1][0] = sinAx*sinAy*cosAz-cosAx*sinAz;
	m[1][1] = sinAx*sinAy*sinAz+cosAx*cosAz;
	m[1][2] = sinAx*cosAy;
	m[1][3] = 0.0f;
	m[2][0] = cosAx*sinAy*cosAz+sinAx*sinAz;
	m[2][1] = cosAx*sinAy*sinAz-sinAx*cosAz;
	m[2][2] = cosAx*cosAy;
	m[2][3] = 0.0f;
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
	m[3][3] = 1.0f;
	return *this;
}

inline Matrix & Matrix::BuildRotateX(float ang)
{
	Identity();
	m[1][1] = cosf(ang);
	m[1][2] = sinf(ang);
	m[2][1] = -sinf(ang);	
	m[2][2] = cosf(ang);
	return *this;
}

inline Matrix & Matrix::BuildRotateY(float ang)
{
	Identity();
	m[0][0] = cosf(ang);	
	m[0][2] = -sinf(ang);
	m[2][0] = sinf(ang);
	m[2][2] = cosf(ang);
	return *this;
}

inline Matrix & Matrix::BuildRotateZ(float ang)
{
	Identity();
	m[0][0] = cosf(ang);	
	m[0][1] = sinf(ang);
	m[1][0] = -sinf(ang);
	m[1][1] = cosf(ang);
	return *this;
}

inline bool Matrix::BuildRotate(const Vector & axis, float ang)
{	
	Matrix m;
	m.Vz() = axis;
	if(m.Vz().Normalize() < 1e-20f)
	{
		Identity();
		return false;
	}
	
	m.Vy() = m.Vz() ^ Vector(1.0f, 0.0f, 0.0f);
	if (m.Vy().Normalize() < 1e-20f)
	{		
		m.Vy() = m.Vz() ^ Vector(0.0f, 1.0f, 0.0f);
		if (m.Vy().Normalize() < 1e-20f)
		{			
			m.Vy() = m.Vz() ^ Vector(0.0f, 0.0f, 1.0f);
			
			if (m.Vy().Normalize() < 1e-20f)
			{			
				Identity();
				return false;
			}
		}
	}
	m.Vx() = m.Vy() ^ m.Vz();
	*this = Matrix(m).Inverse()*Matrix().BuildRotateZ(ang)*m;
	return true;
}

inline Matrix & Matrix::BuildPosition(float x, float y, float z)
{
	Identity();
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
	return *this;
}

inline Matrix & Matrix::BuildPosition(const Vector & pos)
{
	BuildPosition(Pos().x, Pos().y, Pos().z);
	return *this;
}

inline Matrix & Matrix::BuildScale(float scale)
{
	Identity();
	m[0][0] = scale;
	m[1][1] = scale;
	m[2][2] = scale;
	return *this;
}

inline Matrix & Matrix::BuildScale(float scaleX, float scaleY, float scaleZ)
{
	Identity();
	m[0][0] = scaleX;
	m[1][1] = scaleY;
	m[2][2] = scaleZ;
	return *this;
}

inline Matrix & Matrix::BuildScale(const Vector & scale)
{
	BuildScale(scale.x, scale.y, scale.z);
	return *this;
}

inline Matrix & Matrix::BuildProjection(float viewAngle, float aspect, float zNear, float zFar)
{	
	SetZero();	

	double Q = double(zFar)/double(zFar - zNear);
	m[1][1] = float(1.0/tan(viewAngle*0.5));
	m[0][0] = m[1][1] * aspect;
	m[2][2] = float(Q);
	m[2][3] = 1.0f;
	m[3][2] = float(-Q*zNear);

	return *this;
}

inline Matrix & Matrix::BuildProjection(float viewAngle, float vpWidth, float vpHeight, float zNear, float zFar)
{
	return BuildProjection(viewAngle, vpHeight/vpWidth, zNear, zFar);
}

inline Matrix & Matrix::BuildOrtoProjection(float vpWidth, float vpHeight, float zNear, float zFar)
{	
	SetZero();

	double Q = 1.0/double(zFar - zNear);
	m[0][0] = 2.0f/vpWidth;
	m[1][1] = 2.0f/vpHeight;
	m[2][2] = float(Q);
	m[3][2] = float(-Q*zNear);
	m[3][3] = 1.0f;

	return *this;
}

inline Matrix & Matrix::BuildShadowProjection(float viewAngle, float vpWidth, float vpHeight, float zNear, float zFar)
{	
	SetZero();

	double Q = 1.0/double(zFar - zNear);
	m[0][0] = float(1.0/tan(viewAngle*0.5));
	m[1][1] = float(1.0/tan((vpHeight/vpWidth)*viewAngle*0.5));
	m[2][2] = float(Q);
	m[2][3] = 1.0f;
	m[3][2] = float(-Q*zNear);
	return *this;
}

inline bool Matrix::BuildView(Vector lookFrom, Vector lookTo, Vector upVector)
{
	Identity();
	
	lookTo -= lookFrom;
	if(lookTo.Normalize() == 0.0f)
	{
		
		Pos() = -lookFrom;
		return false;
	}
	
	upVector -= lookTo*(lookTo | upVector);
	
	if(upVector.Normalize() == 0.0f) upVector.y = 1.0f;
	
	Vector v = upVector ^ lookTo;
	if(v.Normalize() != 0.0f)
	{		
		m[0][0] = v.x;
		m[1][0] = v.y;
		m[2][0] = v.z;
		m[0][1] = upVector.x;
		m[1][1] = upVector.y;
		m[2][1] = upVector.z;
		m[0][2] = lookTo.x;
		m[1][2] = lookTo.y;
		m[2][2] = lookTo.z;	
	}
	else
	{		
		Pos() = -lookFrom;
		return false;
	}
	
	Pos() = -MulNormal(lookFrom);
	return true;
}

inline Matrix & Matrix::BuildViewFromObject()
{	
	Inverse();
	return *this;
}

inline bool Matrix::BuildOrient(Vector zAxisDirection, Vector upVector)
{	
	if(zAxisDirection.Normalize() < 1e-37f || upVector.Normalize() < 1e-37f)
	{
		Vx().Set(1.0f, 0.0f , 0.0f);
		Vy().Set(0.0f, 1.0f , 0.0f);
		Vz().Set(0.0f, 0.0f , 1.0f);
		return false;
	}
	
	Vx() = upVector ^ zAxisDirection;
	if(Vx().Normalize() == 0.0f)
	{
		Vx().Set(1.0f, 0.0f , 0.0f);
		Vy().Set(0.0f, 1.0f , 0.0f);
		Vz().Set(0.0f, 0.0f , 1.0f);
		return false;
	}
	
	Vy() = zAxisDirection ^ Vx();
	Vz() = zAxisDirection;
	return true;
}

inline bool Matrix::BuildOriented(Vector position, Vector lookTo, Vector upVector)
{	
	lookTo -= position;
	
	if(lookTo.Normalize() == 0.0f || upVector.Normalize() == 0.0f)
	{
		Vx().Set(1.0f, 0.0f , 0.0f); wx = 0.0f;
		Vy().Set(0.0f, 1.0f , 0.0f); wy = 0.0f;
		Vz().Set(0.0f, 0.0f , 1.0f); wz = 0.0f;
		Pos() = position; w = 1.0f;
		return false;
	}
	
	Vx() = lookTo ^ upVector; wx = 0.0f;
	if(Vx().Normalize() == 0.0f)
	{
		Vx().Set(1.0f, 0.0f , 0.0f); wx = 0.0f;
		Vy().Set(0.0f, 1.0f , 0.0f); wy = 0.0f;
		Vz().Set(0.0f, 0.0f , 1.0f); wz = 0.0f;
		Pos() = position; w = 1.0f;
		return false;		
	}
	Vy() = lookTo ^ Vx(); wy = 0.0f;
	Vz() = lookTo; wz = 0.0f;
	Pos() = position; w = 1.0f;
	return true;
}

inline Matrix & Matrix::BuildMirror(float Nx, float Ny, float Nz, float D)
{
	m[0][0] = -Nx*2.0f*Nx + 1.0f;
	m[0][1] = -Ny*2.0f*Nx;
	m[0][2] = -Nz*2.0f*Nx;
	m[0][3] = 0.0f;
	m[1][0] = -Nx*2.0f*Ny;
	m[1][1] = -Ny*2.0f*Ny + 1.0f;
	m[1][2] = -Nz*2.0f*Ny;
	m[1][3] = 0.0f;
	m[2][0] = -Nx*2.0f*Nz;
	m[2][1] = -Ny*2.0f*Nz;
	m[2][2] = -Nz*2.0f*Nz + 1.0f;
	m[2][3] = 0.0f;
	m[3][0] = -Nx*2.0f*-D;
	m[3][1] = -Ny*2.0f*-D;
	m[3][2] = -Nz*2.0f*-D;	
	m[3][3] = 1.0f;
	return *this;
}

inline Matrix & Matrix::RotateX(float ang)
{
	Matrix m;
	m.BuildRotateX(ang);
	EqMultiply(Matrix(*this), m);
	return *this;
}

inline Matrix & Matrix::RotateY(float ang)
{
	Matrix m;
	m.BuildRotateY(ang);
	EqMultiply(Matrix(*this), m);
	return *this;
}

inline Matrix & Matrix::RotateZ(float ang)
{
	Matrix m;
	m.BuildRotateZ(ang);
	EqMultiply(Matrix(*this), m);
	return *this;
}

inline Matrix & Matrix::Rotate(float angX, float angY, float angZ)
{
	Matrix m;
	m.Build(angX, angY, angZ);
	EqMultiply(Matrix(*this), m);
	return *this;
}

inline Matrix & Matrix::Rotate(const Vector & ang)
{
	Matrix m;
	m.Build(ang.x, ang.y, ang.z);
	EqMultiply(Matrix(*this), m);
	return *this;
}

inline Matrix & Matrix::Move(float dX, float dY, float dZ)
{
	Pos().x += dX;
	Pos().y += dY;
	Pos().z += dZ;
	return *this;
}

inline Matrix & Matrix::Move(const Vector & pos)
{
	this->Pos().x += pos.x;
	this->Pos().y += pos.y;
	this->Pos().z += pos.z;
	return *this;
}

inline Matrix & Matrix::Scale(float scale)
{
	Scale(scale, scale, scale);
	return *this;
}

inline Matrix & Matrix::Scale3x3(float scale)
{
	Scale3x3(scale, scale, scale);
	return *this;
}

inline Matrix & Matrix::Scale(float scaleX, float scaleY, float scaleZ)
{
	m[0][0] *= scaleX;
	m[1][0] *= scaleX;
	m[2][0] *= scaleX;
	m[3][0] *= scaleX;
	m[0][1] *= scaleY;
	m[1][1] *= scaleY;
	m[2][1] *= scaleY;
	m[3][1] *= scaleY;
	m[0][2] *= scaleZ;
	m[1][2] *= scaleZ;
	m[2][2] *= scaleZ;
	m[3][2] *= scaleZ;
	return *this;
}

inline Matrix & Matrix::Scale3x3(float scaleX, float scaleY, float scaleZ)
{
	m[0][0] *= scaleX;
	m[1][0] *= scaleX;
	m[2][0] *= scaleX;
	m[0][1] *= scaleY;
	m[1][1] *= scaleY;
	m[2][1] *= scaleY;
	m[0][2] *= scaleZ;
	m[1][2] *= scaleZ;
	m[2][2] *= scaleZ;
	return *this;
}

inline Matrix & Matrix::Scale(const Vector & scale)
{
	Scale(scale.x, scale.y, scale.z);
	return *this;
}

inline Matrix & Matrix::Scale3x3(const Vector & scale)
{
	Scale3x3(scale.x, scale.y, scale.z);
	return *this;
}

inline Matrix & Matrix::Inverse()
{
	Pos().Set(-(Pos() | Vx()), -(Pos() | Vy()), -(Pos() | Vz()));
	Transposition3X3();
	return *this;
}

inline Matrix & Matrix::Inverse(Matrix & mtx)
{
	Pos().Set(-(mtx.Pos() | mtx.Vx()), -(mtx.Pos() | mtx.Vy()), -(mtx.Pos() | mtx.Vz()));

	m[0][0] = mtx.m[0][0];
	m[1][0] = mtx.m[0][1];
	m[2][0] = mtx.m[0][2];

	m[0][1] = mtx.m[1][0];
	m[1][1] = mtx.m[1][1];
	m[2][1] = mtx.m[1][2];

	m[0][2] = mtx.m[2][0];
	m[1][2] = mtx.m[2][1];
	m[2][2] = mtx.m[2][2];

	m[0][3] = 0.0f;  
	m[1][3] = 0.0f;  
	m[2][3] = 0.0f;  
	m[3][3] = 1.0f;  

	return *this;

}

inline bool Matrix::InverseComplette()
{
	double mtmp[3][3];
	mtmp[0][0] = m[1][1]*m[2][2] - m[1][2]*m[2][1];
	mtmp[0][1] = m[0][2]*m[2][1] - m[0][1]*m[2][2];
	mtmp[0][2] = m[0][1]*m[1][2] - m[0][2]*m[1][1];
	mtmp[1][0] = m[1][2]*m[2][0] - m[1][0]*m[2][2];
	mtmp[1][1] = m[0][0]*m[2][2] - m[0][2]*m[2][0];
	mtmp[1][2] = m[0][2]*m[1][0] - m[0][0]*m[1][2];
	mtmp[2][0] = m[1][0]*m[2][1] - m[1][1]*m[2][0];
	mtmp[2][1] = m[0][1]*m[2][0] - m[0][0]*m[2][1];
	mtmp[2][2] = m[0][0]*m[1][1] - m[0][1]*m[1][0];
	double det = m[0][0]*mtmp[0][0] + m[0][1]*mtmp[1][0] + m[0][2]*mtmp[2][0];
	if(fabs(det) > 1e-200)
	{
		det = 1.0/det;
		m[0][0] = float(det*mtmp[0][0]);
		m[0][1] = float(det*mtmp[0][1]);
		m[0][2] = float(det*mtmp[0][2]);
		m[1][0] = float(det*mtmp[1][0]);
		m[1][1] = float(det*mtmp[1][1]);
		m[1][2] = float(det*mtmp[1][2]);
		m[2][0] = float(det*mtmp[2][0]);
		m[2][1] = float(det*mtmp[2][1]);
		m[2][2] = float(det*mtmp[2][2]);
	}
	else
	{
		return false;
	}

	Pos() = -(MulNormal(Pos()));
	return true;
}

inline Matrix & Matrix::Transposition()
{
	float tmp;
	tmp = m[0][1]; m[0][1] = m[1][0]; m[1][0] = tmp;
	tmp = m[0][2]; m[0][2] = m[2][0]; m[2][0] = tmp;
	tmp = m[0][3]; m[0][3] = m[3][0]; m[3][0] = tmp;
	tmp = m[1][2]; m[1][2] = m[2][1]; m[2][1] = tmp;
	tmp = m[1][3]; m[1][3] = m[3][1]; m[3][1] = tmp;
	tmp = m[2][3]; m[2][3] = m[3][2]; m[3][2] = tmp;
	return *this;


}

inline Matrix & Matrix::Transposition3X3()
{
	float tmp;
	tmp = m[0][1]; m[0][1] = m[1][0]; m[1][0] = tmp;
	tmp = m[0][2]; m[0][2] = m[2][0]; m[2][0] = tmp;
	tmp = m[1][2]; m[1][2] = m[2][1]; m[2][1] = tmp;

	return *this;

}

inline Matrix & Matrix::SetRotate(const Matrix & mtx)
{
	m[0][0] = mtx.m[0][0];
	m[0][1] = mtx.m[0][1];
	m[0][2] = mtx.m[0][2];

	m[1][0] = mtx.m[1][0];
	m[1][1] = mtx.m[1][1];
	m[1][2] = mtx.m[1][2];

	m[2][0] = mtx.m[2][0];
	m[2][1] = mtx.m[2][1];
	m[2][2] = mtx.m[2][2];

	return *this;

}

inline Matrix & Matrix::EqMultiply(const Matrix & m1, const Matrix & m2)
{
	Matrix m;
	m.EqMultiplyFast(m1, m2);
	Set(m);
	return *this;
}
	
inline Matrix & Matrix::EqMultiplyFast(const Matrix & m1, const Matrix & m2)
{
	m[0][0] = m2.m[0][0]*m1.m[0][0] + m2.m[1][0]*m1.m[0][1] + m2.m[2][0]*m1.m[0][2] + m2.m[3][0]*m1.m[0][3];
	m[0][1] = m2.m[0][1]*m1.m[0][0] + m2.m[1][1]*m1.m[0][1] + m2.m[2][1]*m1.m[0][2] + m2.m[3][1]*m1.m[0][3];
	m[0][2] = m2.m[0][2]*m1.m[0][0] + m2.m[1][2]*m1.m[0][1] + m2.m[2][2]*m1.m[0][2] + m2.m[3][2]*m1.m[0][3];
	m[0][3] = m2.m[0][3]*m1.m[0][0] + m2.m[1][3]*m1.m[0][1] + m2.m[2][3]*m1.m[0][2] + m2.m[3][3]*m1.m[0][3];
	m[1][0] = m2.m[0][0]*m1.m[1][0] + m2.m[1][0]*m1.m[1][1] + m2.m[2][0]*m1.m[1][2] + m2.m[3][0]*m1.m[1][3];
	m[1][1] = m2.m[0][1]*m1.m[1][0] + m2.m[1][1]*m1.m[1][1] + m2.m[2][1]*m1.m[1][2] + m2.m[3][1]*m1.m[1][3];
	m[1][2] = m2.m[0][2]*m1.m[1][0] + m2.m[1][2]*m1.m[1][1] + m2.m[2][2]*m1.m[1][2] + m2.m[3][2]*m1.m[1][3];
	m[1][3] = m2.m[0][3]*m1.m[1][0] + m2.m[1][3]*m1.m[1][1] + m2.m[2][3]*m1.m[1][2] + m2.m[3][3]*m1.m[1][3];
	m[2][0] = m2.m[0][0]*m1.m[2][0] + m2.m[1][0]*m1.m[2][1] + m2.m[2][0]*m1.m[2][2] + m2.m[3][0]*m1.m[2][3];
	m[2][1] = m2.m[0][1]*m1.m[2][0] + m2.m[1][1]*m1.m[2][1] + m2.m[2][1]*m1.m[2][2] + m2.m[3][1]*m1.m[2][3];
	m[2][2] = m2.m[0][2]*m1.m[2][0] + m2.m[1][2]*m1.m[2][1] + m2.m[2][2]*m1.m[2][2] + m2.m[3][2]*m1.m[2][3];
	m[2][3] = m2.m[0][3]*m1.m[2][0] + m2.m[1][3]*m1.m[2][1] + m2.m[2][3]*m1.m[2][2] + m2.m[3][3]*m1.m[2][3];
	m[3][0] = m2.m[0][0]*m1.m[3][0] + m2.m[1][0]*m1.m[3][1] + m2.m[2][0]*m1.m[3][2] + m2.m[3][0]*m1.m[3][3];
	m[3][1] = m2.m[0][1]*m1.m[3][0] + m2.m[1][1]*m1.m[3][1] + m2.m[2][1]*m1.m[3][2] + m2.m[3][1]*m1.m[3][3];
	m[3][2] = m2.m[0][2]*m1.m[3][0] + m2.m[1][2]*m1.m[3][1] + m2.m[2][2]*m1.m[3][2] + m2.m[3][2]*m1.m[3][3];
	m[3][3] = m2.m[0][3]*m1.m[3][0] + m2.m[1][3]*m1.m[3][1] + m2.m[2][3]*m1.m[3][2] + m2.m[3][3]*m1.m[3][3];
	return *this;
}

inline Vector4 Matrix::MulVertex4(const Vector & v) const
{	
	Vector4 tv;
	tv.x = m[0][0]*v.x + m[1][0]*v.y + m[2][0]*v.z + m[3][0];
	tv.y = m[0][1]*v.x + m[1][1]*v.y + m[2][1]*v.z + m[3][1];
	tv.z = m[0][2]*v.x + m[1][2]*v.y + m[2][2]*v.z + m[3][2];
	tv.w = m[0][3]*v.x + m[1][3]*v.y + m[2][3]*v.z + m[3][3];

	return tv;
}

inline Vector Matrix::MulVertex(const Vector & v) const
{
	Vector tv;
	tv.x = m[0][0]*v.x + m[1][0]*v.y + m[2][0]*v.z + m[3][0];
	tv.y = m[0][1]*v.x + m[1][1]*v.y + m[2][1]*v.z + m[3][1];
	tv.z = m[0][2]*v.x + m[1][2]*v.y + m[2][2]*v.z + m[3][2];
	return tv;
}

inline Vector Matrix::MulNormal(const Vector & v) const
{
	Vector tv;
	tv.x = m[0][0]*v.x + m[1][0]*v.y + m[2][0]*v.z;
	tv.y = m[0][1]*v.x + m[1][1]*v.y + m[2][1]*v.z;
	tv.z = m[0][2]*v.x + m[1][2]*v.y + m[2][2]*v.z;
	return tv;
}

inline Vector Matrix::MulVertexByInverse(const Vector & v) const
{
	Vector tv;
	tv.x = m[0][0]*(v.x - m[3][0]) + m[0][1]*(v.y - m[3][1]) + m[0][2]*(v.z - m[3][2]);
	tv.y = m[1][0]*(v.x - m[3][0]) + m[1][1]*(v.y - m[3][1]) + m[1][2]*(v.z - m[3][2]);
	tv.z = m[2][0]*(v.x - m[3][0]) + m[2][1]*(v.y - m[3][1]) + m[2][2]*(v.z - m[3][2]);
	return tv;
}

inline Vector Matrix::MulNormalByInverse(const Vector & v) const
{
	Vector tv;
	tv.x = m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z;
	tv.y = m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z;
	tv.z = m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z;
	return tv;
}

inline Vector Matrix::GetCamPos()
{
	return -MulNormalByInverse(Pos());
}

inline bool Matrix::IsIdentity() const
{
	const float eps = 1e-4f;
	if(fabs(m[0][0] - 1.0f) > eps) return false;
	if(fabs(m[0][1] - 0.0f) > eps) return false;
	if(fabs(m[0][2] - 0.0f) > eps) return false;
	if(fabs(m[0][3] - 0.0f) > eps) return false;
	if(fabs(m[1][0] - 0.0f) > eps) return false;
	if(fabs(m[1][1] - 1.0f) > eps) return false;
	if(fabs(m[1][2] - 0.0f) > eps) return false;
	if(fabs(m[1][3] - 0.0f) > eps) return false;
	if(fabs(m[2][0] - 0.0f) > eps) return false;
	if(fabs(m[2][1] - 0.0f) > eps) return false;
	if(fabs(m[2][2] - 1.0f) > eps) return false;
	if(fabs(m[2][3] - 0.0f) > eps) return false;
	if(fabs(m[3][0] - 0.0f) > eps) return false;
	if(fabs(m[3][1] - 0.0f) > eps) return false;
	if(fabs(m[3][2] - 0.0f) > eps) return false;
	if(fabs(m[3][3] - 1.0f) > eps) return false;
	return true;
}

inline bool Matrix::IsScale()
{
	const float eps = 1e-4f;
	if(fabsf(~Vx() - 1.0f) > eps) return true;
	if(fabsf(~Vy() - 1.0f) > eps) return true;
	if(fabsf(~Vz() - 1.0f) > eps) return true;
	if(fabsf(w - 1.0f) > eps) return true;
	return false;
}

inline bool Matrix::IsParity()
{
	return ((Vx() ^ Vy()) | Vz()) >= 0.0f;
}

inline Vector4 Matrix::Projection(Vector vertex, float vphWidth05, float vphHeight05) const
{
	Vector4 res;
	
	res.x = m[0][0]*vertex.x + m[1][0]*vertex.y + m[2][0]*vertex.z + m[3][0];
	res.y = m[0][1]*vertex.x + m[1][1]*vertex.y + m[2][1]*vertex.z + m[3][1];
	res.z = m[0][2]*vertex.x + m[1][2]*vertex.y + m[2][2]*vertex.z + m[3][2];
	res.w = m[0][3]*vertex.x + m[1][3]*vertex.y + m[2][3]*vertex.z + m[3][3];
	
	float w = 1.0f/res.w;
	
	res.x = (1.0f + res.x*w)*vphWidth05;
	res.y = (1.0f - res.y*w)*vphHeight05;
	res.z *= w;
	res.w = w;
	return res;
}

inline void Matrix::Projection(Vector4 * dstArray, Vector * srcArray, long num, float vphWidth05, float vphHeight05, long srcSize, long dstSize) const
{
	for(; num > 0; num--)
	{		
		dstArray->x = m[0][0]*srcArray->x + m[1][0]*srcArray->y + m[2][0]*srcArray->z + m[3][0];
		dstArray->y = m[0][1]*srcArray->x + m[1][1]*srcArray->y + m[2][1]*srcArray->z + m[3][1];
		dstArray->z = m[0][2]*srcArray->x + m[1][2]*srcArray->y + m[2][2]*srcArray->z + m[3][2];
		dstArray->w = m[0][3]*srcArray->x + m[1][3]*srcArray->y + m[2][3]*srcArray->z + m[3][3];
		
		float w = 1.0f/dstArray->w;
		
		dstArray->x = (1.0f + dstArray->x*w)*vphWidth05;
		dstArray->y = (1.0f - dstArray->y*w)*vphHeight05;
		dstArray->z *= w;
		dstArray->w = w;
		
		srcArray = (Vector *)((char *)srcArray + srcSize);
		dstArray = (Vector4 *)((char *)dstArray + dstSize);
	}
}

inline void Matrix::GetAngles(float & ax, float & ay, float & az)
{	
	if(Vz().y < 1.0f)
	{
		if(Vz().y > -1.0f)
		{
			ax = (float)asin(-Vz().y);
			ay = (float)atan2(Vz().x, Vz().z);
			az = (float)atan2(Vx().y, Vy().y);
			return;
		}else{
			ax = 3.141592654f*0.5f;
			ay = 0.0f;
			az = (float)atan2(Vx().z, Vx().x);
		}
	}else{
		ax = -3.141592654f*0.5f;
		ay = 0.0f;
		az = (float)-atan2(Vx().z, Vx().x);
	}
}

inline Vector Matrix::GetScale()
{	
	return Vector(Vx().Length(), Vy().Length(), Vz().Length());
}

inline Matrix & Matrix::Normalize()
{
	Vx().Normalize();
	Vy().Normalize();
	Vz().Normalize();
	return *this;
}

inline Matrix & Matrix::Mul(float k)
{
	matrix[0] *= k;  matrix[1] *= k;  matrix[2] *= k;  matrix[3] *= k;
	matrix[4] *= k;  matrix[5] *= k;  matrix[6] *= k;  matrix[7] *= k;
	matrix[8] *= k;  matrix[9] *= k;  matrix[10] *= k; matrix[11] *= k;
	matrix[12] *= k; matrix[13] *= k; matrix[14] *= k; matrix[15] *= k;
	return *this;
}

inline Matrix & Matrix::Add(const Matrix & m)
{
	matrix[0] += m.matrix[0];   matrix[1] += m.matrix[1];   matrix[2] += m.matrix[2];   matrix[3] += m.matrix[3];
	matrix[4] += m.matrix[4];   matrix[5] += m.matrix[5];   matrix[6] += m.matrix[6];   matrix[7] += m.matrix[7];
	matrix[8] += m.matrix[8];   matrix[9] += m.matrix[9];   matrix[10] += m.matrix[10]; matrix[11] += m.matrix[11];
	matrix[12] += m.matrix[12]; matrix[13] += m.matrix[13]; matrix[14] += m.matrix[14]; matrix[15] += m.matrix[15];
	return *this;
}

inline Matrix & Matrix::Mad(const Matrix & m, float k)
{
	matrix[0] += m.matrix[0]*k;   matrix[1] += m.matrix[1]*k;   matrix[2] += m.matrix[2]*k;   matrix[3] += m.matrix[3]*k;
	matrix[4] += m.matrix[4]*k;   matrix[5] += m.matrix[5]*k;   matrix[6] += m.matrix[6]*k;   matrix[7] += m.matrix[7]*k;
	matrix[8] += m.matrix[8]*k;   matrix[9] += m.matrix[9]*k;   matrix[10] += m.matrix[10]*k; matrix[11] += m.matrix[11]*k;
	matrix[12] += m.matrix[12]*k; matrix[13] += m.matrix[13]*k; matrix[14] += m.matrix[14]*k; matrix[15] += m.matrix[15]*k;
	return *this;
}

inline void Matrix::GetAngles(Vector & ang)
{
	GetAngles(ang.x, ang.y, ang.z);
}

inline Vector Matrix::GetAngles()
{
	Vector tmp;
	GetAngles(tmp);
	return tmp;
}

inline float & Matrix::operator () (long i, long j)
{
	return m[i][j];
}