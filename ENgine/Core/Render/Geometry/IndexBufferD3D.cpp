
#include "IndexBufferD3D.h"
#include "Core/Core.h"

IndexBufferD3D::IndexBufferD3D(int sz) : IndexBuffer(sz)
{	
	if (RenderServiceD3D::instance->IsUseDummyRender())
	{
		dummy_data = (byte*)malloc(size);		
	}
	else
	{
		indexBuffer = NULL;
		if ( FAILED(RenderServiceD3D::instance->device->CreateIndexBuffer(size * 2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &indexBuffer, NULL) ) )
		{
			core->TraceTo( "Render", "create index buffer failed" );
		}
	}
}

void* IndexBufferD3D::Lock()
{
	if (RenderServiceD3D::instance->IsUseDummyRender()) return dummy_data;

	byte* pData;
	indexBuffer->Lock(0, 0, (VOID**)&pData, 0);

	return pData;
}

void IndexBufferD3D::Unlock()
{
	if (RenderServiceD3D::instance->IsUseDummyRender()) return;

	indexBuffer->Unlock();
}

void* IndexBufferD3D::GetData()
{
	return indexBuffer;
}

void IndexBufferD3D::Release()
{
	if (!RenderServiceD3D::instance->IsUseDummyRender())
	{		
		RenderServiceD3D::instance->SetIndexBuffer(null);	

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

	RenderServiceD3D::instance->DeleteIBuffer(this);

	delete this;	
}
