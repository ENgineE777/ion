
#pragma once

#include "IPhysObject.h"
#include "IPhysController.h"
#include "Common/TaskHolder.h"

class IPhysicsService : public TaskHoldersOwner
{
public:	

	IPhysicsService() : TaskHoldersOwner() {};
	virtual IPhysObject* CreatePhysObject(Matrix transform) = 0;
	virtual IPhysController* CreatePhysConytoller() = 0;
	virtual float RayCast(Vector pos, Vector to, Vector& cp, Vector& cn, Vector& vl, void* data) = 0;
};



