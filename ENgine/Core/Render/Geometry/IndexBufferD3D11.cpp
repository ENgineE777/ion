
#include "IndexBufferD3D11.h"
#include "Core/Core.h"

#include "d3d11.h"

IndexBufferD3D11::IndexBufferD3D11(int sz) : IndexBuffer(sz)
{
	if (RenderServiceD3D11::instance->IsUseDummyRender())
	{
		dummy_data = (byte*)malloc(size);		
	}
	else
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );

		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sz * 2;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		
		RenderServiceD3D11::instance->pd3dDevice->CreateBuffer( &bd, null, &indexBuffer );
	}
}

void* IndexBufferD3D11::Lock()
{
	if (RenderServiceD3D11::instance->IsUseDummyRender()) return dummy_data;

	D3D11_MAPPED_SUBRESOURCE res;	
	RenderServiceD3D11::instance->immediateContext->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);	

	return res.pData;
}

void IndexBufferD3D11::Unlock()
{
	if (RenderServiceD3D11::instance->IsUseDummyRender()) return;
	
	RenderServiceD3D11::instance->immediateContext->Unmap(indexBuffer, 0);
}

void* IndexBufferD3D11::GetData()
{
	return indexBuffer;
}

void IndexBufferD3D11::Release()
{
	if (!RenderServiceD3D11::instance->IsUseDummyRender())
	{		
		RenderServiceD3D11::instance->SetIndexBuffer(null);	

		if (indexBuffer)
		{
			indexBuffer->Release();
			indexBuffer = null;
		}		
	}
	else
	{
		free(dummy_data);
	}

	RenderServiceD3D11::instance->DeleteIBuffer(this);

	delete this;	
}
