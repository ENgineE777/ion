
#include "PhysicBox.h"

ENTITY(PhysicBox)
ENTITY_PARAMS(PhysicBox,"PhysicBox","Physics", "Physic Box")

PhysicBox::~PhysicBox()
{
	if (Scene()) Scene()->UnregisterEntityInGroup(this, "PhysBox");
}

void PhysicBox::Init()
{
	pbox = null;

	PROP_REG(weight)
	PROP_REG(width)
	PROP_REG(height)
	PROP_REG(length)
	PROP_REG(color)

	SetUpdate((TaskHolder::Task)&PhysicBox::Render, lvl_work1);	

	if (Scene()) Scene()->RegisterEntityInGroup(this, "PhysBox");
}

void PhysicBox::ReInit()
{
}

void PhysicBox::Start()
{
	pbox = core->Physics()->CreatePhysObject(transform);

	//pbox->SetMass(weight);	
	pbox->AddBoxShape(Vector(width,height,length),Matrix());
	pbox->CompoudShape((weight<0.01f));	
}

void PhysicBox::Finish()
{
	pbox->Release();
	pbox = null;
}

void PhysicBox::ApplyImpulse(Vector& imp)
{
	if (pbox)
	{
		pbox->ApplyImpulse(imp);
	}
}

void PhysicBox::Render(float dt, int level)
{	
	if (Scene()->IsRunning() && pbox)
	{
		pbox->GetTransform(transform);
	}

	core->Render()->DrawSolidBox(Vector(width,height,length),transform,color);
	core->Render()->DrawBox(Vector(width,height,length),transform,0xffffff00);
}
