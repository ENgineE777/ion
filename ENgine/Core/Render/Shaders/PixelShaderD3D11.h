
#pragma once

#include "PixelShader.h"
#include "ShaderInfoD3D11.h"

class PixelShaderD3D11 :  public PixelShader
{	
    class ID3D11PixelShader* shader;    
	Texture*                 textures[16];
    bool texDirty;
	ShaderInfoD3D11          info;

public:

    PixelShaderD3D11(const char* name);
	~PixelShaderD3D11();

	virtual bool Load();

    virtual bool SetTexture(const char* param, Texture* tex);    
	    
	virtual bool ParamExist(const char* param);

	virtual bool SetVector4(const char* param, const Vector4& v);
	virtual bool SetVector4Array(const char* param, int num, float* v);	
	virtual bool SetMatrix(const char* param, const Matrix& mat);
	virtual bool SetMatrixArray(const char* param, int num, float* mat);	

    virtual void UpdateConstants();
	virtual void UpdateTextures();
	virtual void* GetData();	
    virtual void Release();
};
