
#include "ShaderInfoD3D11.h"
#include "Core/Core.h"

#include "d3d11.h"
#include "d3dcompiler.h"

ShaderInfoD3D11::ShaderInfoD3D11() : constantsInfo(_FL_), samplersInfo(_FL_)
{	
}

void ShaderInfoD3D11::ParseConstants(ID3D11ShaderReflection* pVertexShaderReflection)
{
	D3D11_SHADER_DESC shaderDesc;
	pVertexShaderReflection->GetDesc(&shaderDesc);

	for (unsigned int i = 0; i < shaderDesc.ConstantBuffers; i++)
	{			
		D3D11_SHADER_BUFFER_DESC shaderBuffer;
		ID3D11ShaderReflectionConstantBuffer* pConstBuffer = pVertexShaderReflection->GetConstantBufferByIndex(i);

		pConstBuffer->GetDesc(&shaderBuffer);

		D3D11_SHADER_INPUT_BIND_DESC bindingDesc;
		pVertexShaderReflection->GetResourceBindingDescByName(shaderBuffer.Name, &bindingDesc);

		buffers.push_back(ConstantBuffer());

		ConstantBuffer& buffer = buffers.back();	
		buffer.size = shaderBuffer.Size;
		buffer.rawdata = (byte*)malloc(buffer.size);

		buffer.slot = bindingDesc.BindPoint;

		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = buffer.size;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		RenderServiceD3D11::instance->pd3dDevice->CreateBuffer( &bd, NULL, &buffer.buffer );

		for (unsigned int j = 0; j < shaderBuffer.Variables; j++)
		{
			ID3D11ShaderReflectionVariable * pVariable = pConstBuffer->GetVariableByIndex(j);

			D3D11_SHADER_VARIABLE_DESC varDesc;
			pVariable->GetDesc(&varDesc);

			D3D11_SHADER_TYPE_DESC varType;
			ID3D11ShaderReflectionType * pType = pVariable->GetType();

			pType->GetDesc(&varType);

			ShaderParamInfo* param = constantsInfo.Add(varDesc.Name);

			param->slot = i;
			param->offset = varDesc.StartOffset;
			param->size = varDesc.Size;
		}			
	}
}

bool ShaderInfoD3D11::ParamExist(const char* param)
{
	return (constantsInfo.Get(param) != null);
}

bool ShaderInfoD3D11::SetVector4(const char* param, const Vector4& v) 
{
    ShaderParamInfo* spInfo = constantsInfo.Get(param);

    if(spInfo == 0)
	{
		return false;
	}

	ConstantBuffer* buffer = &buffers[spInfo->slot];	
	memcpy((void*)&buffer->rawdata[spInfo->offset], &v.x, spInfo->size);

	buffer->dirty = true;

    return true; 
}

bool ShaderInfoD3D11::SetVector4Array(const char* param, int num, float* v) 
{
    ShaderParamInfo* spInfo = constantsInfo.Get(param);

    if (spInfo == 0)
	{
		return false;
	}

	ConstantBuffer* buffer = &buffers[spInfo->slot];		
	memcpy((void*)&buffer->rawdata[spInfo->offset], v, MathMin(spInfo->size, sizeof(float) * 4 * num));

	buffer->dirty = true;

    return true;
}

bool ShaderInfoD3D11::SetMatrix(const char* param, const Matrix& mat)
{ 
    ShaderParamInfo* spInfo = constantsInfo.Get(param);

    if(spInfo == 0)
	{
		return false;
	}

	ConstantBuffer* buffer = &buffers[spInfo->slot];

	Matrix tmp = mat;
	tmp.Transposition();

	memcpy((void*)&buffer->rawdata[spInfo->offset], &tmp, spInfo->size);
	
    buffer->dirty = true;	

    return true; 
}

bool ShaderInfoD3D11::SetMatrixArray(const char* param, int num, float* mat)
{
    ShaderParamInfo* spInfo = constantsInfo.Get(param);

    if (spInfo == 0)
	{
		return false;
	}

    //spInfo->setted_reg = (spInfo->regCount/spInfo->elementsCount) * MathMin(num, spInfo->elementsCount);
    //memcpy((void*)&constants[spInfo->reg], mat, sizeof(float) * 4 * spInfo->setted_reg);    

    return true; 
}

void ShaderInfoD3D11::UpdateConstants(bool is_vs)
{		
	for (int i=0;i<buffers.size();i++)
	{
		ConstantBuffer* buffer = &buffers[i];

		if (buffer->dirty)
		{				
			RenderServiceD3D11::instance->immediateContext->UpdateSubresource( buffer->buffer, 0, NULL, buffer->rawdata, 0, 0 );						

			buffer->dirty = false;		
		}

		if (is_vs)
		{
			RenderServiceD3D11::instance->immediateContext->VSSetConstantBuffers(i, 1, &buffer->buffer );	
		}
		else
		{
			RenderServiceD3D11::instance->immediateContext->PSSetConstantBuffers(i, 1, &buffer->buffer );	
		}		
    }	
}