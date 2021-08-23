
#pragma once

#include "IndexBuffer.h"

class DEF_EXPORT IndexBufferD3D11 : public IndexBuffer
{			
	class ID3D11Buffer* indexBuffer;
	byte* dummy_data;

public:

	IndexBufferD3D11(int sz);
	
	virtual void* Lock();
	virtual void Unlock();
	virtual void* GetData();
	virtual void Release();
};
