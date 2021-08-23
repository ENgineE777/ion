
#pragma once

#include "IPhysicsService.h"
#include "btBulletDynamicsCommon.h"

class PhysicsService : public IPhysicsService
{	
public:	
	
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btAxisSweep3* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
			
	PhysicsService();

	virtual bool Init();

	virtual IPhysObject* CreatePhysObject(Matrix transform);
	virtual IPhysController* CreatePhysConytoller();

	virtual float RayCast(Vector from, Vector to, Vector& cp, Vector& cn, Vector& vl,void* data);

	virtual void Work(float fDelta,int level);
	virtual void DebugDraw(float fDelta,int level);	

	virtual void Release();
};



