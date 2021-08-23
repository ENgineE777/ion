
#pragma once

#include "Shader.h"
#include "Common/Containers/HashMap.h"

class ShaderInfoD3D11
{	
	friend class VertexShaderD3D11;
	friend class PixelShaderD3D11;

protected:

	struct ConstantBuffer
	{
		byte*		  rawdata;
		int           size;
		int			  slot;
		class ID3D11Buffer* buffer;
		bool		  dirty;

		ConstantBuffer()
		{
			rawdata = null;
			slot = 0;
			size = 0;
			buffer = null;
			dirty = false;
		};
	};

	struct ShaderParamInfo
	{
		int slot; 
		int offset;
		int size;		

		ShaderParamInfo()
		{
			slot = 0; 
			offset = 0;
			size = 0;			
		};
	};

	vector<ConstantBuffer>   buffers;
	HashMap<ShaderParamInfo> constantsInfo;
	HashMap<ShaderParamInfo> samplersInfo;

public:

	ShaderInfoD3D11();
	
	void ParseConstants(class ID3D11ShaderReflection* pVertexShaderReflection);

	virtual bool ParamExist(const char* param);

	virtual bool SetVector4(const char* param, const Vector4& v);
    virtual bool SetVector4Array(const char* param, int num, float* v);	
    virtual bool SetMatrix(const char* param, const Matrix& mat);
    virtual bool SetMatrixArray(const char* param, int num, float* mat);
	
	void UpdateConstants(bool is_vs);	
};