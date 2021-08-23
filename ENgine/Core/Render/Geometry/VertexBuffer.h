
#pragma once

#include "GeometryBuffer.h"

class VertexBuffer : public GeometryBuffer
{
protected:
	int stride;
public:
		
	enum ElemType
	{
		float2 = 0,
		float3,
		float4,
		tp_color,
		ubyte4,
		short4,
		dec4,
		short2
	};

	enum ElemUsage
	{
		position = 0,
		positiont,
		normal,
		texcoord,
		color,
		binormal,
		tangent,
		blendindex,
		blendweight
	};

	VertexBuffer(int sz,int strd) : GeometryBuffer(sz)
	{
		stride = strd;
	};

	virtual void StartDesc() = 0;
	virtual void AddDescElem(ElemUsage usage, ElemType type, const char* name) = 0;
	virtual void EndDesc() = 0;	

	virtual ~VertexBuffer() {};

	int GetStride() { return stride; };
};
