
#include "PixelShaderD3D.h"
#include "Core/Core.h"

PixelShaderD3D::PixelShaderD3D(const char* name) : PixelShader(name)
{
    shader = null;        

    for(int i = 0; i < 16; i++)
    {
        textures[i] = 0;
    }	
}

bool PixelShaderD3D::Load()
{
	Release();

	char file_path[512];

#ifdef PC
	//Hack
	String::Copy(file_path, 511, "#platform/Shaders/");	
	String::Cat(file_path, 511, GetFileName());

	en_core.EN_Files()->RegFile(file_path);
#endif

	String::Copy(file_path, 511, "#platform/Shaders/DX9/");	
	String::Cat(file_path, 511, GetFileName());

	IFileBuffer* fb = core->Files()->LoadFile(file_path);

	if (fb && !RenderServiceD3D::instance->IsUseDummyRender())
	{				
		RenderServiceD3D::instance->device->CreatePixelShader((const DWORD*)fb->GetBuffer(), &shader);   		

		info.ParseShader(fb->GetBuffer());

		fb->Release();

		return true;
	}

	return false;
}

PixelShaderD3D::~PixelShaderD3D()
{
	Release();
}

void* PixelShaderD3D::GetData()
{
    return shader;
}

void PixelShaderD3D::Release()
{
    info.constantsInfo.Clear();
    info.samplersInfo.Clear();
    
	RenderServiceD3D::instance->SetPixelShader(null);

	RELEASE(shader)	
}

bool PixelShaderD3D::ParamExist(const char* param)
{
	return info.ParamExist(param);
}

bool PixelShaderD3D::SetVector4(const char* param, const Vector4& v)
{
	return info.SetVector4(param, v);
}

bool PixelShaderD3D::SetVector4Array(const char* param, int num, float* v)
{
	return info.SetVector4Array(param, num, v);
}

bool PixelShaderD3D::SetMatrix(const char* param, const Matrix& mat)
{
	return info.SetMatrix(param, mat);
}

bool PixelShaderD3D::SetMatrixArray(const char* param, int num, float* mat)
{
	return info.SetMatrixArray(param, num, mat);
}

bool PixelShaderD3D::SetTexture(const char* param, Texture* tex)
{
    ShaderInfoD3D::ShaderParamInfo* spInfo = info.samplersInfo.Get(param);

    if(spInfo == 0)
        return false;

    textures[spInfo->reg] = tex;
    texDirty = true;

    return true;
}

void PixelShaderD3D::UpdateConstants() 
{    
	info.UpdateConstants(false);	
}

void PixelShaderD3D::UpdateTextures() 
{
    if(texDirty)
    {
		for (int i=0;i<info.samplersInfo.Size();i++)
		{
			ShaderInfoD3D::ShaderParamInfo* spInfo = info.samplersInfo.Get(i);

			RenderServiceD3D::instance->SetTexture(spInfo->reg, textures[spInfo->reg]);
			if (textures[spInfo->reg])
			{
				textures[spInfo->reg]->ApplyStates(spInfo->reg);
			}
		}
        
        texDirty = false;
    }
}





