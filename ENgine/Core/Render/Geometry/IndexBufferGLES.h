#pragma once

#include "IndexBuffer.h"

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

class IndexBufferGLES : public IndexBuffer
{	
#if defined(ANDROID) || defined(OSUNIX)
	byte* data;
#endif

	GLuint indexVBO;

public:

	IndexBufferGLES(int sz);
	
	virtual void ReCreate();
	virtual void* Lock();
	virtual void Unlock();
	virtual void* GetData();
	virtual void Release();
};