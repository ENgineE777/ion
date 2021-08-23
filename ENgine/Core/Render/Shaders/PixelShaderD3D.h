
#pragma once

#include "PixelShader.h"
#include "ShaderInfoD3D.h"

class PixelShaderD3D :  public PixelShader
{	
    class IDirect3DPixelShader9* shader;
    Texture*                     textures[16];
    bool texDirty;
	ShaderInfoD3D info;

public:
    PixelShaderD3D(const char* name);
	~PixelShaderD3D();

	virtual bool Load();

	virtual bool ParamExist(const char* param);

	virtual bool SetVector4(const char* param, const Vector4& v);
	virtual bool SetVector4Array(const char* param, int num, float* v);	
	virtual bool SetMatrix(const char* param, const Matrix& mat);
	virtual bool SetMatrixArray(const char* param, int num, float* mat);

    virtual bool SetTexture(const char* param, Texture* tex);    	
    
    virtual void UpdateConstants();
	virtual void UpdateTextures();
	virtual void* GetData();	
    virtual void Release();
};
