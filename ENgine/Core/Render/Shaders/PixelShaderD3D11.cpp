
#include "PixelShaderD3D11.h"
#include "Core/Core.h"

#include "d3d11.h"
#include "d3dcompiler.h"

PixelShaderD3D11::PixelShaderD3D11(const char* name) : PixelShader(name)
{
    shader = null;        

    for(int i = 0; i < 16; i++)
    {
        textures[i] = 0;
    }	
}

bool PixelShaderD3D11::Load()
{
	Release();

	char file_path[512];

#ifdef PC
	//Hack
	String::Copy(file_path, 511, "#platform/Shaders/");	
	String::Cat(file_path, 511, GetFileName());

	en_core.EN_Files()->RegFile(file_path);
#endif

	String::Copy(file_path, 511, "#platform/Shaders/");	
	String::Cat(file_path, 511, GetFileName());

	IFileBuffer* fb = core->Files()->LoadFile(file_path);

	if (fb && !RenderServiceD3D11::instance->IsUseDummyRender())
	{		
		RenderServiceD3D11::instance->pd3dDevice->CreatePixelShader( fb->GetBuffer(), fb->GetSize(), NULL, &shader );		
		
		ID3D11ShaderReflection* pVertexShaderReflection;
		D3DReflect(fb->GetBuffer(), fb->GetSize(), IID_ID3D11ShaderReflection, (void**) &pVertexShaderReflection);

		info.ParseConstants(pVertexShaderReflection);

		D3D11_SHADER_DESC shaderDesc;
		pVertexShaderReflection->GetDesc(&shaderDesc);		
		
		for (int i=0;i<shaderDesc.BoundResources;i++)
		{
			D3D11_SHADER_INPUT_BIND_DESC bindDesc;
			pVertexShaderReflection->GetResourceBindingDesc(i, &bindDesc);

			if (bindDesc.Type == D3D_SIT_TEXTURE)
			{
				ShaderInfoD3D11::ShaderParamInfo* param = info.samplersInfo.Add(bindDesc.Name);

				param->slot = bindDesc.BindPoint;				
			}
		}		

		fb->Release();

		return true;
	}

	return false;
}

PixelShaderD3D11::~PixelShaderD3D11()
{
	Release();
}

void* PixelShaderD3D11::GetData()
{
    return shader;
}

void PixelShaderD3D11::Release()
{
    info.constantsInfo.Clear();
    info.samplersInfo.Clear();
	info.buffers.clear();

	RenderServiceD3D11::instance->SetPixelShader(null);

	if (shader)
	{
		shader->Release();
		shader = null;
	}
}

bool PixelShaderD3D11::SetTexture(const char* param, Texture* tex)
{
    ShaderInfoD3D11::ShaderParamInfo* spInfo = info.samplersInfo.Get(param);

    if (spInfo == 0)
	{
        return false;
	}

    textures[spInfo->slot] = tex;
    texDirty = true;

    return true;
}

bool PixelShaderD3D11::ParamExist(const char* param)
{
	return info.ParamExist(param);
}

bool PixelShaderD3D11::SetVector4(const char* param, const Vector4& v)
{
	return info.SetVector4(param, v);
}

bool PixelShaderD3D11::SetVector4Array(const char* param, int num, float* v)
{
	return info.SetVector4Array(param, num, v); 
}

bool PixelShaderD3D11::SetMatrix(const char* param, const Matrix& mat)
{
	return info.SetMatrix(param, mat); 
}

bool PixelShaderD3D11::SetMatrixArray(const char* param, int num, float* mat)
{
	return info.SetMatrixArray(param, num, mat); 
}

void PixelShaderD3D11::UpdateConstants() 
{    
	if (dirty)
	{
		info.UpdateConstants(false);	
	}
}

void PixelShaderD3D11::UpdateTextures() 
{
	dirty = true;

    if (texDirty)
    {
		for (int i=0;i<info.samplersInfo.Size();i++)
		{
			ShaderInfoD3D11::ShaderParamInfo* spInfo = info.samplersInfo.Get(i);
			
			if (textures[spInfo->slot])
			{
				ID3D11ShaderResourceView* sh_view = (ID3D11ShaderResourceView*)textures[spInfo->slot]->GetHackData();

				textures[spInfo->slot]->ApplyStates(spInfo->slot);
				RenderServiceD3D11::instance->immediateContext->PSSetShaderResources(spInfo->slot, 1, &sh_view);
			}
		}
        
        texDirty = false;
    }
}





