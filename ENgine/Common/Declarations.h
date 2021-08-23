#pragma once

#include <string>
#include <string.h>
#include <vector>

using namespace std;

typedef unsigned char	byte;
typedef signed short	int16;
typedef unsigned short	word;

#ifdef __LP64__
    typedef unsigned int  dword;    
#else
    typedef unsigned long dword;  
#endif

typedef signed long long int	int64;
typedef unsigned long long int	uint64;

#define null 0
#define _FL_ __FILE__ , __LINE__

#ifdef PC
	#ifdef DLL_EXPORT
		#define DEF_EXPORT __declspec(dllexport)
	#else
		#define DEF_EXPORT __declspec(dllimport)
	#endif
#else
	#define DEF_EXPORT
#endif

#define RGBA2COLOR(r,g,b,a) ( (((a)&0xff)<<24) | (((r)&0xff)<<16) | (((g)&0xff)<<8) | ((b)&0xff) )
#define RGBA2COLORF(r,g,b,a) ( (((byte)(a*255)&0xff)<<24) | (((byte)(r*255)&0xff)<<16) | (((byte)(g*255)&0xff)<<8) | ((byte)(b*255)&0xff) )

#define COLOR2RGBA(color,r,g,b,a) \
	a = ((color>>24)&0xff) / 255.0f;\
	r = ((color>>16)&0xff) / 255.0f;\
	g = ((color>>8)&0xff) / 255.0f;\
	b = ((color)&0xff) / 255.0f;	

#define	EPS 0.0001f

#define RELEASE(object)\
	if (object)\
{\
	object->Release();\
	object = null;\
}

#define DELETE(object)\
	if (object)\
{\
	delete object;\
	object = null;\
}

#define DELETE_ARRAY(object)\
	if (object)\
{\
	delete [] object;\
	object = null;\
}

#define DEBUG_BREAK { int k=0; k++; }

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
	((dword)(byte)(ch0) | ((dword)(byte)(ch1) << 8) |   \
	((dword)(byte)(ch2) << 16) | ((dword)(byte)(ch3) << 24 ))
#endif

enum ElemAlign
{
	up_left = 0,
	up_right = 1,
	down_right = 2,
	down_left = 3
};

enum TextAlign
{
	text_left = 0,
	text_center = 1,
	text_right = 2
};

enum Platform
{
	platform_pc,
	platform_ios,
	platform_android,
	platform_osx,
	platform_unix,
	platform_undefined
};

struct ClipRect
{
	float x1, y1;
	float x2, y2;
    ClipRect():x1(0),x2(0),y1(0),y2(0){}
    ClipRect(float _x1,float _y1,float _x2,float _y2):x1(_x1),x2(_x2),y1(_y1),y2(_y2){}
};

inline bool IsLittleEndian()
{
#ifdef PC 
	return true;
#endif

	return false;
}

inline void SwapWord(word& val)
{
	byte* buff = (byte*)&val;
	val = (buff[1] << 0) | (buff[0] << 8);	
}

inline void SwapDword(dword& val)
{
	unsigned int *ival = (unsigned int *)&val;
	*ival = ((*ival >> 24) & 0x000000ff) |
		((*ival >>  8) & 0x0000ff00) |
		((*ival <<  8) & 0x00ff0000) |
		((*ival << 24) & 0xff000000);
}

inline void SwapInt(int& val)
{
	SwapDword((dword&)val);	
}

inline void SwapFloat(float& val)
{
	SwapDword((dword&)val);	
}

inline unsigned int HashValue(const char* id)
{	
	const char* end_id = id + strlen(id);

	unsigned int val = 2166136261U;

	while(id != end_id)
		val = 16777619U * val ^ (unsigned int)*id++;

	return val;
}

inline unsigned int HashValue2(const char* id)
{	
	const char* end_id = id + strlen(id);

	unsigned int val = 1241344461U;

	while(id != end_id)
		val = 16777444U * val ^ (unsigned int)*id++;

	return val;
}