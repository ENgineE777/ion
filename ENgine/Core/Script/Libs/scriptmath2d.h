#ifndef SCRIPTMATH2D_H
#define SCRIPTMATH2D_H

#include "angelscript.h"

BEGIN_AS_NAMESPACE

// This is not a complete 3D math library. It's only meant as a sample
// for registering value types. The application developers will most
// likely want to register their own math library instead.

struct Vector2S
{
	Vector2S();
	Vector2S(const Vector2S &other);
	Vector2S(float x, float y);

	Vector2S &operator=(const Vector2S &other);
	Vector2S &operator+=(const Vector2S &other);
	Vector2S &operator-=(const Vector2S &other);
	Vector2S &operator*=(float scalar);
	Vector2S &operator/=(float scalar);

	float Length();
	void  Normalize();
	void  Lerp(Vector2S& p1,Vector2S& p2, float k);	

	friend bool operator==(const Vector2S &a, const Vector2S &b);
	friend bool operator!=(const Vector2S &a, const Vector2S &b);
	friend Vector2S operator+(const Vector2S &a, const Vector2S &b);
	friend Vector2S operator-(const Vector2S &a, const Vector2S &b);
	friend Vector2S operator*(float s, const Vector2S &v);
	friend Vector2S operator*(const Vector2S &v, float s);
	friend Vector2S operator/(const Vector2S &v, float s);

	float x;
	float y;
};

// This function will determine the configuration of the engine
// and use one of the two functions below to register the string type
void RegisterScriptMath2D(asIScriptEngine *engine);

// Call this function to register the math functions using native calling conventions
void RegisterScriptMath2D_Native(asIScriptEngine *engine);

// Use this one instead if native calling conventions
// are not supported on the target platform
void RegisterScriptMath2D_Generic(asIScriptEngine *engine);

END_AS_NAMESPACE

#endif
