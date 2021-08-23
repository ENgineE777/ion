
#pragma once

#include "VertexBuffer.h"

class DEF_EXPORT VertexBufferD3D : public VertexBuffer
{	
	class IDirect3DVertexDeclaration9* decl;
	class IDirect3DVertexBuffer9* vertexBuffer;
	
	int elem_index;
	int elem_offset;
	int elem_ps_index;
	int elem_tx_index;
    int elem_clr_index;

	byte* dummy_data;

public:
	
	VertexBufferD3D(int sz, int strd);
	
	virtual void StartDesc();
	virtual void AddDescElem(ElemUsage usage, ElemType type, const char* name);
	virtual void EndDesc();	

	virtual void* Lock();
	virtual void Unlock();
	virtual void* GetData();
	virtual void Release();
};
