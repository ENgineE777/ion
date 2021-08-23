
#pragma once

#include "Declarations.h"

class Color
{	
public:

	float r,g,b,a;

	Color()
	{
		r = g = b = a = 1.0f;
	};

	Color(float R, float G, float B)
	{
       Set(R,G,B,1.0f);
	};

    Color(const Color& other)
    {
       Set(other.r,other.g,other.b,other.a);
    }

	Color(dword color)
	{		
        Set(color);
	};

	Color(float R, float G, float B, float A)
	{
		Set(R,G,B,A);		
	};

	void Set(float R, float G, float B, float A)
	{
		r = R;
		g = G;
		b = B;
		a = A;
	};

    void Set(dword color)
    {
        a = (color >> 24) / 255.0f;
        b = ((color >> 16) & 0xff) / 255.0f;
        g = ((color >> 8) & 0xff) / 255.0f;
        r = ((color) & 0xff) / 255.0f;	
    };

	dword Get(bool revert = false)
	{		
#if defined(PC)
		return (((byte)(a*255))<<24) | (((byte)(r*255))<<16) | (((byte)(g*255))<<8) | ((byte)(b*255));		
#endif
		
#if defined(OPGL)
		return (((byte)(a*255))<<24) | (((byte)(r*255))<<16) | (((byte)(g*255))<<8) | ((byte)(b*255));		
#endif
	};
	
	Color& operator *= (Color& color)
	{
		r *= color.r;
		g *= color.g;
		b *= color.b;
		a *= color.a;

		return *this;
	};

    Color& operator = (const Color& other)
    {
        Set(other.r,other.g,other.b,other.a);
        return *this;
    };

    Color& operator = (dword color)
    {
        Set(color);
        return *this;
    };
};

#define COLOR_WHITE Color()
#define COLOR_BLACK Color(0.0f, 0.0f, 0.0f)
#define COLOR_GRAY Color(0.4f, 0.4f, 0.4f)
#define COLOR_LIGHT_GRAY Color(0.65f, 0.65f, 0.65f)

#define COLOR_RED Color(1.0f, 0.0f, 0.0f)
#define COLOR_GREEN Color(0.0f, 1.0f, 0.0f)
#define COLOR_BLUE Color(0.0f, 0.0f, 1.0f)

#define COLOR_YELLOW Color(1.0f, 1.0f, 0.0f)
#define COLOR_MAGNETA Color(1.0f, 0.0f, 1.0f)
#define COLOR_CYAN Color(0.0f, 1.0f, 1.0f)

#define COLOR_EMPTY Color(0.0f, 0.0f, 0.0f, 0.0f)
