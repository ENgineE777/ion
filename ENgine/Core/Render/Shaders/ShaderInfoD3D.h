
#pragma once

#ifdef PC
#define NOMINMAX
#pragma push_macro("new")
#undef new
#include "d3d9.h"
#include "d3dx9.h"
#pragma pop_macro("new")
#endif

#include "Shader.h"
#include "Common/Containers/HashMap.h"

class ShaderInfoD3D
{	
	friend class VertexShaderD3D;
	friend class PixelShaderD3D;

protected:

	struct ShaderParamInfo
	{
		dword reg;
		dword regCount;
		dword elementsCount;
		bool dirty;
		dword setted_reg;
	};

	Vector4* constants;
	int totalConstants;
	HashMap<ShaderParamInfo> constantsInfo;
	HashMap<ShaderParamInfo> samplersInfo;

public:

	ShaderInfoD3D();

	virtual bool ParamExist(const char* param);

	virtual bool SetVector4(const char* param, const Vector4& v);
    virtual bool SetVector4Array(const char* param, int num, float* v);	
    virtual bool SetMatrix(const char* param, const Matrix& mat);
    virtual bool SetMatrixArray(const char* param, int num, float* mat);

	void ParseShader(byte* buffer);
	void UpdateConstants(bool is_vs);		
	void SetConstants(bool is_vs, int index, float* ptr, int count);
};