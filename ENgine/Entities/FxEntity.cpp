
#include "FxEntity.h"
#include "Core/Fx/Fx.h"
#include "Core/Fx/FxRes.h"

ENTITY(FxEntity)
ENTITY_PARAMS(FxEntity,"FxEntity","FX", "Emiter")

void FxEntity::Init()
{
	m_fx = null;

	PROP_REG( propResPath )	

	SetUpdate((TaskHolder::Task)&FxEntity::Work, lvl_work1);

	if (Scene()) Scene()->RegisterEntityInGroup(this, "Fx");
}

void FxEntity::ReInit()
{
	if (m_fx)
		core->Fx()->ReleaseFx(m_fx);

	m_fx = core->Fx()->CreateFx(propResPath);
}

void FxEntity::Start()
{

}

void FxEntity::Work(float dt, int level)
{
	if (Scene()->IsRunning())
	{
	}

	if (m_fx)
		m_fx->SetTransform(transform);
}