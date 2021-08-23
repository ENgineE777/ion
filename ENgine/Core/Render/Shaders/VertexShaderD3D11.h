
#pragma once

#include "VertexShader.h"
#include "ShaderInfoD3D11.h"

class VertexShaderD3D11 :  public VertexShader
{	
	class ID3D11InputLayout*  layout;
    class ID3D11VertexShader* shader;
	ShaderInfoD3D11           info;
    
public:
    VertexShaderD3D11(const char* name);  
	~VertexShaderD3D11();

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