
#include "VertexBufferD3D11.h"
#include "Core/Core.h"

#include "d3d11.h"

VertexBufferD3D11::VertexBufferD3D11(int sz, int strd) : VertexBuffer(sz, strd)
{	
	vertexBuffer = NULL;

	if (RenderServiceD3D11::instance->IsUseDummyRender())
	{
		dummy_data = (byte*)malloc(size);		
	}
	else
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = size;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;						

		RenderServiceD3D11::instance->pd3dDevice->CreateBuffer( &bd, null, &vertexBuffer );		
	}
}

void* VertexBufferD3D11::Lock()
{
	if (RenderServiceD3D11::instance->IsUseDummyRender()) return dummy_data;

	D3D11_MAPPED_SUBRESOURCE res;	
	RenderServiceD3D11::instance->immediateContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);	

	return res.pData;
}

void VertexBufferD3D11::Unlock()
{
	if (RenderServiceD3D11::instance->IsUseDummyRender()) return;

	RenderServiceD3D11::instance->immediateContext->Unmap(vertexBuffer, 0);
}

void VertexBufferD3D11::StartDesc()
{	
}

void VertexBufferD3D11::AddDescElem(ElemUsage usage, ElemType type, const char* name)
{
}

void VertexBufferD3D11::EndDesc()
{
}

void* VertexBufferD3D11::GetData()
{	
	return vertexBuffer;
}

void VertexBufferD3D11::Release()
{
	if (!RenderServiceD3D11::instance->IsUseDummyRender())
	{		
		RenderServiceD3D11::instance->SetVertexBuffer(0, null);	

		if (vertexBuffer)
		{
			vertexBuffer->Release();
			vertexBuffer = null;
		}
	
		RenderServiceD3D11::instance->SetVertexDeclaration(null);		
	}
	else
	{
		free(dummy_data);
	}

	RenderServiceD3D11::instance->DeleteVBuffer(this);

	delete this;
}