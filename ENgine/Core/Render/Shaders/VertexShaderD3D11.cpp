
#include "VertexShaderD3D11.h"
#include "Core/Core.h"

#include "d3d11.h"
#include "d3dcompiler.h"

VertexShaderD3D11::VertexShaderD3D11(const char* name) : VertexShader(name)
{	
    shader = null;
	layout = null;
}

VertexShaderD3D11::~VertexShaderD3D11()
{
	Release();
}

bool VertexShaderD3D11::Load()
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
		RenderServiceD3D11::instance->pd3dDevice->CreateVertexShader( fb->GetBuffer(), fb->GetSize(), NULL, &shader );
				
		ID3D11ShaderReflection* pVertexShaderReflection;
		D3DReflect(fb->GetBuffer(), fb->GetSize(), IID_ID3D11ShaderReflection, (void**) &pVertexShaderReflection);

		info.ParseConstants(pVertexShaderReflection);

		D3D11_SHADER_DESC shaderDesc;
		pVertexShaderReflection->GetDesc(&shaderDesc);

		unsigned int byteOffset = 0;
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
		for ( unsigned int i=0; i< shaderDesc.InputParameters; ++i )
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc;      
			pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc );

			// create input element desc
			D3D11_INPUT_ELEMENT_DESC elementDesc;  
			elementDesc.SemanticName = paramDesc.SemanticName;      
			elementDesc.SemanticIndex = paramDesc.SemanticIndex;
			elementDesc.InputSlot = 0;
			elementDesc.AlignedByteOffset = byteOffset;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;  	
				
			if (String::IsEqual(paramDesc.SemanticName, "COLOR"))
			{
				elementDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;				
				byteOffset += 4;				
			}
			else
			// determine DXGI format
			if ( paramDesc.Mask == 1 )
			{
				if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32_UINT;
				else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32_SINT;
				else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
				byteOffset += 4;
			}
			else if ( paramDesc.Mask <= 3 )
			{
				if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
				else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
				else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
				byteOffset += 8;
			}
			else if ( paramDesc.Mask <= 7 )
			{
				if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
				byteOffset += 12;
			}
			else if ( paramDesc.Mask <= 15 )
			{
				if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 ) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				byteOffset += 16;
			}			

			//save element desc
			inputLayoutDesc.push_back(elementDesc);
		}      
		
		RenderServiceD3D11::instance->pd3dDevice->CreateInputLayout( &inputLayoutDesc[0], inputLayoutDesc.size(), fb->GetBuffer(), fb->GetSize(), &layout);						
		
		pVertexShaderReflection->Release();

		fb->Release();

		return true;
	}

	return false;
}

void* VertexShaderD3D11::GetData()
{
	if (layout) RenderServiceD3D11::instance->SetVertexDeclaration(layout);

    return shader;
}

void VertexShaderD3D11::Release()
{
    info.constantsInfo.Clear();
	info.buffers.clear();

	RenderServiceD3D11::instance->SetVertexShader(null);

	RELEASE(shader)	
}

bool VertexShaderD3D11::ParamExist(const char* param)
{
	return info.ParamExist(param);
}

bool VertexShaderD3D11::SetVector4(const char* param, const Vector4& v)
{
	return info.SetVector4(param, v);
}

bool VertexShaderD3D11::SetVector4Array(const char* param, int num, float* v)
{
	return info.SetVector4Array(param, num, v); 
}

bool VertexShaderD3D11::SetMatrix(const char* param, const Matrix& mat)
{
	return info.SetMatrix(param, mat); 
}

bool VertexShaderD3D11::SetMatrixArray(const char* param, int num, float* mat)
{
	return info.SetMatrixArray(param, num, mat); 
}

void VertexShaderD3D11::UpdateConstants() 
{
	if (dirty)
	{
		info.UpdateConstants(true);	
	}
}
