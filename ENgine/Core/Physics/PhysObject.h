
#pragma once

#include "IPhysObject.h"
#include "PhysicsService.h"

#include "btBulletDynamicsCommon.h"

class PhysObject : public IPhysObject
{
	PhysicsService* service;	
	
	int num_collisions;	

	Matrix transform;

	btRigidBody* btBody;
	btCompoundShape* compShape;
	btCollisionShape* colShape[10];

	btVector3*	gVertices;
	int* gIndices;
	btTriangleIndexVertexArray* indexVertexArrays;
	btBvhTriangleMeshShape* triMesh;

public:

	PhysObject(PhysicsService* srv, Matrix trans);

	virtual void AddBoxShape(Vector size,Matrix offset);
	virtual void AddSphereShape(float radius,Matrix offset);
	virtual void AddCapsuleShape(float height, float radius,Matrix offset);
	
	//virtual void AddCollisionTree(IModel* model);

	virtual void CompoudShape(bool isStatic);

	virtual void SetMass(float mass);	

	virtual void SetTransform(Matrix transform);
	virtual void GetTransform(Matrix& transform);

	virtual void ApplyImpulse(Vector& imp);

	virtual void Release();
};



