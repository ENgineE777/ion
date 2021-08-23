#pragma once

#include "PixelShader.h"
#include "ShaderGLES.h"

class PixelShaderGLES :  public PixelShader
{		
	friend class RenderTechnique;
	ShaderGLES shader;

public:
	
	PixelShaderGLES(const char* name);
	
	virtual bool Load();
	virtual void* GetData();

	virtual bool SetVector4(const char* param, const Vector4& v);
	virtual bool SetVector4Array(const char* param, int num, float* v);	
	virtual bool SetMatrix(const char* param, const Matrix& mat);
	virtual bool SetMatrixArray(const char* param, int num, float* mat);	

	virtual bool SetTexture(const char* param, Texture* tex);
	virtual bool SetTexture(const char* param, int slot, Texture* tex);

	virtual void Release();
};
