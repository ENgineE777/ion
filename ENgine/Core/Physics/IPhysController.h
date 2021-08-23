
#pragma once

#include "Common/Common.h"

class IPhysController
{
public:
	virtual void Activate(bool act, Matrix mat) = 0;	
		
	virtual void SetTransform(Matrix transform) = 0;
	virtual void GetTransform(Matrix& transform) = 0;

	virtual void MoveTo(Vector dir) = 0; 
	virtual void GetPos(Vector& pos) = 0; 

	virtual void SetRadius(float r) = 0; 
	virtual void SetHeight(float h) = 0; 

	virtual void* GetData() = 0;

	virtual void Release() = 0;	
};



