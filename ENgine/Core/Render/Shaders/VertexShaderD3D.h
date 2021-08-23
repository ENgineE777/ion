
#pragma once

#include "VertexShader.h"
#include "ShaderInfoD3D.h"

class VertexShaderD3D :  public VertexShader
{	
    class IDirect3DVertexShader9* shader;
	ShaderInfoD3D info;
    
public:
    VertexShaderD3D(const char* name);  
	~VertexShaderD3D();

	virtual bool Load();

	virtual bool ParamExist(const char* param);

	virtual bool SetVector4(const char* param, const Vector4& v);
	virtual bool SetVector4Array(const char* param, int num, float* v);	
	virtual bool SetMatrix(const char* param, const Matrix& mat);
	virtual bool SetMatrixArray(const char* param, int num, float* mat);
    
    virtual void UpdateConstants();
    virtual void* GetData();	
    virtual void Release();
};