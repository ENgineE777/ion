
#include "FxEntity2D.h"

ENTITY(FxEntity2D)
	ENTITY_IS_2D_OBJECT
	ENTITY_PARAMS(FxEntity2D,"FxEntity2D","FX", "Emiter 2D")

	void FxEntity2D::Init()
{
	
	m_fx = null;

	PROP_REG( propResPath )	

	SetUpdate((TaskHolder::Task)&FxEntity2D::Work, lvl_work1);

	if (Scene()) Scene()->RegisterEntityInGroup(this, "Fx");

	GUILabel::Init();
}

void FxEntity2D::ReInit()
{
	GUILabel::ReInit();

	if (m_fx)
	{
		core->Fx()->ReleaseFx(m_fx);
	}

	m_fx = core->Fx()->CreateFx(propResPath);
}

void FxEntity2D::Start()
{
	GUILabel::Start();
}

void FxEntity2D::Work(float dt, int level)
{
	if (Scene()->IsRunning())
	{
	}

	transform.Pos().x = scr_x/core->Render()->GetHeight();
	transform.Pos().y = scr_y/core->Render()->GetHeight();

	if (m_fx)
		m_fx->SetTransform(transform);
}


void  FxEntity2D::RenderWidget()
{
	if (m_fx)
		m_fx->Draw();

}