
#pragma once

#include "GeometryBuffer.h"

class IndexBuffer : public GeometryBuffer
{	
public:
	IndexBuffer(int sz) : GeometryBuffer(sz) {};
	virtual ~IndexBuffer() {};	
};
