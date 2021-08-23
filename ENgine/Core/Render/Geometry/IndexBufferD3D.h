
#pragma once

#include "IndexBuffer.h"

class DEF_EXPORT IndexBufferD3D : public IndexBuffer
{		
	class IDirect3DIndexBuffer9* indexBuffer;
	byte* dummy_data;

public:

	IndexBufferD3D(int sz);
	
	virtual void* Lock();
	virtual void Unlock();
	virtual void* GetData();
	virtual void Release();
};
