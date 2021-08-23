
#pragma once

#include "Common/Common.h"

class IPhysObject
{
public:
	virtual void AddBoxShape(Vector size,Matrix offset) = 0;
	virtual void AddSphereShape(float radius,Matrix offset) = 0;
	virtual void AddCapsuleShape(float height, float radius,Matrix offset) = 0;

	//virtual void AddCollisionTree(IModel* model) = 0;

	virtual void CompoudShape(bool isStatic) = 0;

	virtual void SetMass(float mass) = 0;	

	virtual void SetTransform(Matrix transform) = 0;
	virtual void GetTransform(Matrix& transform) = 0;

	virtual void ApplyImpulse(Vector& imp) = 0;

	virtual void Release() = 0;
};




