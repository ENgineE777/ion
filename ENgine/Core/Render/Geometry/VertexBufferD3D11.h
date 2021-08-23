
#pragma once

#include "VertexBuffer.h"

class DEF_EXPORT VertexBufferD3D11 : public VertexBuffer
{	
	class ID3D11Buffer* vertexBuffer;
		
	byte* dummy_data;

public:
	
	VertexBufferD3D11(int sz, int strd);
	
	virtual void StartDesc();
	virtual void AddDescElem(ElemUsage usage, ElemType type, const char* name);
	virtual void EndDesc();	

	virtual void* Lock();
	virtual void Unlock();
	virtual void* GetData();
	virtual void Release();
};
