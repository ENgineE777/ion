
#include "PhysicsEntity.h"
#include "PhysicsEngine.h"
#include <algorithm>
#include "PhysActorDesc.h"

//CLASS_DECLARATION( CEntity, CPhysicsEntity )

ENTITY_DECL(CPhysicsEntity, "CPhysicsEntity", 0)


CPhysicsEntity::CPhysicsEntity():CEntity()
{
	m_actor = NULL;	
}

CPhysicsEntity::~CPhysicsEntity()
{
	if (m_actor)
	{
		m_actor->m_userData = NULL;
		//PhysicsEngine::Instance.ReleaseActor(m_actor);
	}
}

typedef struct {
	float			uv[2];
	unsigned short	color;
	float			normal[3];
	float			xyz[3];
} vertex_t;



void CreateMd3FromMd1(byte* buf, char *fullPath);

void CPhysicsEntity::Init()
{
	CEntity::Init();

	m_actor = null;

	PROP_REG(phys_type)
	PROP_REG(phys_shape)
	PROP_REG(m_PropPhysMesh)
	PROP_REG(phys_radius)
	PROP_REG(phys_weight)
	PROP_REG(phys_restitution)	

	m_type = (enumCPhysicsEnityType)phys_type;
}

void CPhysicsEntity::ReInit()
{
	if (model)
	{
		model->Release();
	}

	model = core.Models()->LoadModel(model_name);	


	// actor reinit
	if (m_actor)
	{
		m_actor->m_userData = NULL;
		PhysicsEngine::instance()->ReleaseActor(m_actor);
	}

	switch(phys_shape)
	{
	case eAnimatedMesh:
	case eSphere:
		{

			m_actor = core.Physics()->CreatePhysicalSphere(
				(Vector3&)m_state.origin, 
				(Quaternion&)m_state.quat, 
				phys_radius, OtherMaterial);
		}

		break;

	case eMesh:
		{
			if (CrossString::IsEqual(m_PropPhysMesh, "" ))
			{
				CrossString::Copy(m_PropPhysMesh, 512, "Models/debugCube/debugCube.md1" );
			}

			m_actor = core.Physics()->CreatePhysicalMesh(
			(Vector3&)m_state.origin, 
			(Quaternion&)m_state.quat, 
			m_PropPhysMesh, OtherMaterial);
		}
		break;

	//case eLine:
	//	{
	//		PhysLineDesc line;
	//		PhysMaterialSingle material;
	//		
	//		actorDesc.m_shapeDesc = &line;
	//		actorDesc.m_material = &material;

	//		line.dir.MakeZero();

	//		m_createArgs.GetFloat("length","1", line.dir.z);
	//		
	//		line.dir.z *= -1;

	//		material.m_materialID = OtherMaterial;

	//		m_actor = &PhysicsEngine::instance()->AddActor( actorDesc);

	//	}

	//	break;
	default:
		{
			m_actor = core.Physics()->CreatePhysicalSphere(
				(Vector3&)m_state.origin, 
				(Quaternion&)m_state.quat, 
				phys_radius, OtherMaterial);
		}
			
	}

	m_actor->GetWeight() = phys_weight;
	m_actor->GetRestitution() = phys_restitution;
	m_actor->SetGravity(true);
	m_actor->getCanCollide() = true;
	m_actor->m_userData = this;
	m_actor->GetActorType() = eKinematicActor;
	//m_actor->GetIsActive() = false;

	m_type = eKinematic;	
}

void CPhysicsEntity::Start()
{
	CEntity::Start();

	m_actor->GetActorType() = Other;
	//m_actor->GetIsActive() = false;

	m_type = eDynamyc;

}

void CPhysicsEntity::Finish()
{	
	//if (m_actor)
	//{
	//	m_actor->m_userData = NULL;
	//	PhysicsEngine::instance()->ReleaseActor(m_actor);
	//}

	m_actor->GetActorType() = eKinematicActor;
	//m_actor->GetIsActive() = false;

	m_type = eKinematic;
}

void CPhysicsEntity::Update(float dt, int level)
{

	CEntity::Update ( dt, level );	

	if (core.Scene()->IsRunning() && m_actor)
	{
		if (m_type == eDynamyc)
		{
			m_state.origin = (IVector&)m_actor->GetPosition();
			m_state.quat = (IQuaternion&)m_actor->GetOrientation();
		}
		else
		{
			m_actor->setPosition((Vector3&)m_state.origin);
			m_actor->setOrientation((Quaternion&)m_state.quat);
		}
	}
	else
	{
			m_state.origin = transform.Pos();
			m_state.quat.Set(transform);

			m_actor->setPosition((Vector3&)m_state.origin);
			m_actor->setOrientation((Quaternion&)m_state.quat);

	}

	
}

void CPhysicsEntity::OnContact(CPhysicsEntity* _physEnity2, enumPhysContactPairFlag _event)
{
	

}






