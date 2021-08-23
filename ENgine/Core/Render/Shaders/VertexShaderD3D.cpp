
#include "VertexShaderD3D.h"
#include "Core/Core.h"

VertexShaderD3D::VertexShaderD3D(const char* name) : VertexShader(name)
{	
    shader = null;
}

VertexShaderD3D::~VertexShaderD3D()
{
	Release();
}

bool VertexShaderD3D::Load()
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
		RenderServiceD3D::instance->device->CreateVertexShader((const DWORD*)fb->GetBuffer(), &shader);    		

		info.ParseShader(fb->GetBuffer());

		fb->Release();

		return true;
	}

	return false;
}

void* VertexShaderD3D::GetData()
{
    return shader;
}

bool VertexShaderD3D::ParamExist(const char* param)
{
	return info.ParamExist(param);
}

bool VertexShaderD3D::SetVector4(const char* param, const Vector4& v)
{
	return info.SetVector4(param, v);
}

bool VertexShaderD3D::SetVector4Array(const char* param, int num, float* v)
{
	return info.SetVector4Array(param, num, v);
}

bool VertexShaderD3D::SetMatrix(const char* param, const Matrix& mat)
{
	return info.SetMatrix(param, mat);
}

bool VertexShaderD3D::SetMatrixArray(const char* param, int num, float* mat)
{
	return info.SetMatrixArray(param, num, mat);
}

void VertexShaderD3D::Release()
{
    info.constantsInfo.Clear();   	

	RenderServiceD3D::instance->SetVertexShader(null);

	RELEASE(shader)	
}

void VertexShaderD3D::UpdateConstants() 
{
	info.UpdateConstants(true);
}
