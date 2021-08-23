
#pragma once

#include "Shader.h"

class VertexShader : public Shader
{	
public:

	VertexShader(const char* file) : Shader(file) {};
	
	virtual ~VertexShader() {};
	

#if defined(ANDROID) || defined(IOS)
	virtual bool SetArray(const char* param, int num, int type, int stride, byte* offset) { return true; };	
#endif

#if /*defined(IOS) ||*/ defined(OSX)
	virtual bool SetArray(const char* param, int num, int type, int stride, int offset) { return true; };	
#endif	
};
