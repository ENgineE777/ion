#pragma once

#include "VertexShader.h"
#include "ShaderGLES.h"

class VertexShaderGLES :  public VertexShader
{		
	friend class RenderTechnique;
	ShaderGLES shader;

public:
	
	VertexShaderGLES(const char* name);
	
	virtual bool Load();

	virtual void* GetData();

	virtual bool SetVector4(const char* param, const Vector4& v);
	virtual bool SetVector4Array(const char* param, int num, float* v);	
	virtual bool SetMatrix(const char* param, const Matrix& mat);
	virtual bool SetMatrixArray(const char* param, int num, float* mat);	

#if defined(ANDROID) || defined(IOS) || defined(OSUNIX)
	virtual bool SetArray(const char* param, int num, int type, int stride, byte* offset);	
#endif

#if /*defined(IOS) ||*/ defined(OSX)
	virtual bool SetArray(const char* param, int num, int type, int stride, int offset);	
#endif

	virtual void Release();
};