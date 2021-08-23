
#pragma once

#include "IPhysController.h"
#include "PhysicsService.h"

class PhysController : public IPhysController
{
	PhysicsService* service;
	
	btRigidBody* btBody;
	btCollisionShape* colShape;

	//btConvexShape* m_shape;
	//btCollisionObject* m_collisionObject;
	//btVector3 m_currentPosition;
	//btVector3 m_targetPosition;

	bool bActive;
	float radius;
	float height;	

public:

	Vector move_dir;
	
	PhysController(PhysicsService* srv);
	virtual void Activate(bool act, Matrix mat);

	virtual void SetTransform(Matrix transform);
	virtual void GetTransform(Matrix& transform);
	virtual void MoveTo(Vector dir);
	virtual void GetPos(Vector& pos);

	virtual void SetRadius(float r); 
	virtual void SetHeight(float h); 

	virtual void* GetData();

	virtual void Release();
	
	void updateTargetPositionBasedOnCollision (const btVector3& hitNormal, btScalar tangentMag, btScalar normalMag);
	void ReleaseController();
};