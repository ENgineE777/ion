#pragma once

#include "uwentity.h"

#include "PhysEnums.h"
#include "PhysActor.h"

enum enumPhysicsGroups
{
	// this value is reserved in physics engine
	eStaticPhysicsGroup = 0,
	// this value is reserved in physics engine
	eDefaultPhysicsGroup = 1,

	enumPhysicsGroupsEnd
};

class CPhysicsEntity : public CEntity
{
public:
	
	//CLASS_PROTOTYPE( CPhysicsEntity );

	INT_PROP(CPhysicsEntity, phys_type, 0, "type")

	ENUM_PROP(CPhysicsEntity, phys_shape, 0, "shape")
		ENUM_ITEM("Mesh")
		ENUM_ITEM("Sphere")
		//ENUM_ITEM("Box")
	END_ENUM(CPhysicsEntity, phys_shape)

	FILENAME_PROP(CPhysicsEntity, m_PropPhysMesh, "", "mesh")
	FLOAT_PROP(CPhysicsEntity, phys_radius, 1.0f, "radius")
	FLOAT_PROP(CPhysicsEntity, phys_weight, 1.0f, "weight")
	FLOAT_PROP(CPhysicsEntity, phys_restitution, 0.8f, "Restitution")	

	CPhysicsEntity();
	virtual ~CPhysicsEntity();

	virtual void Init();
	virtual void ReInit();

	virtual void Start();
	virtual void Finish();

	virtual void Update(float dt, int level);

	virtual void OnContact(CPhysicsEntity* _physEnity2, enumPhysContactPairFlag _event);

	enum enumCPhysicsEnityType
	{
		eDynamyc,
		eStatic,
		eKinematic
	};


//protected:

	class PhysActor*	m_actor;
	enumCPhysicsEnityType m_type: 8;
};
