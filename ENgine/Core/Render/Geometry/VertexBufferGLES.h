#pragma once

#include "VertexBuffer.h"

#ifdef IOS
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#endif

#if defined(ANDROID) || defined(OSUNIX)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#ifdef OSX
#import	<OpenGL/gl.h>
#import	<OpenGL/glu.h>
#endif

class VertexShaderGLES;

class VertexBufferGLES : public VertexBuffer
{		
	int offset;

#if defined(ANDROID) || defined(IOS) || defined(OSUNIX)
	byte* data;
#endif

public:

	struct VertexComponent
	{
		char name[32];
		ElemType type;
		int offset;		
	};

#if /*defined(IOS) ||*/ defined(OSX)
	GLuint vbo;	
#endif

	int num_components;
	VertexComponent components[16];

	VertexBufferGLES(int sz,int strd);
	
	virtual void ReCreate();

	virtual void StartDesc();
	virtual void AddDescElem(ElemUsage usage, ElemType type, const char* name);
	virtual void EndDesc();

	virtual void* Lock();
	virtual void Unlock();

	virtual void SetComponents(VertexShaderGLES* vshader);

	virtual void* GetData();
	virtual void Release();
};