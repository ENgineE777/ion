
#include "FxResEdit.h"
#include "Core/Fx/Fx.h"
#include "Core/Fx/FxRes.h"

ENTITY(FxResEdit)
ENTITY_PARAMS(FxResEdit,"FxResEdit","FX", "Emiter Editor")

void FxResEdit::Init()
{
	m_fx = null;

	PROP_REG( propResPath )	
	PROP_REG( numEmiters )	
	PROP_REG( curEditedEmiter )	

	PROP_REG( TextureName )	
	PROP_REG( Looped )	
	PROP_REG( LifeTime )	
	PROP_REG( ParticlesCount )	
	PROP_REG( SpawnRate )	
	PROP_REG( SpawnTime )	
	PROP_REG( SizeMaxX )	
	PROP_REG( SizeMinX )	
	PROP_REG( SizeMaxY )	
	PROP_REG( SizeMinY )	
	PROP_REG( LifeTimeMin )	
	PROP_REG( LifeTimeMax )	
	PROP_REG( PositionMin )	
	PROP_REG( PositionMax )	
	PROP_REG( VelocityMin )	
	PROP_REG( VelocityMax )	
	PROP_REG( AccelerationMin )	
	PROP_REG( AccelerationMax )	
	PROP_REG( AccelerationMax )
	PROP_REG( Fade )
	PROP_REG( Color )
	PROP_REG( scaleSizePerSec )

	SetUpdate((TaskHolder::Task)&FxResEdit::Work, lvl_work1);
	core->Render()->RegisterCallback("debug_render", this, Scene(), (TaskHolder::Task)&FxResEdit::Draw, 0);


	resPathEdited[0] = 0;
}

void FxResEdit::ReInit()
{
	if (m_fx && !String::IsEqual(resPathEdited, propResPath) )
	{
		const_cast<CFxRes*>(m_fx->m_Res)->Save(resPathEdited);
		core->Fx()->ReleaseFx(m_fx);
		m_fx = core->Fx()->CreateFx(propResPath);

		PROP_REG( TextureName )	
	PROP_REG( Looped )	
	PROP_REG( LifeTime )	
	PROP_REG( ParticlesCount )	
	PROP_REG( SpawnRate )	
	PROP_REG( SpawnTime )	
	PROP_REG( SizeMaxX )	
	PROP_REG( SizeMinX )	
	PROP_REG( SizeMaxY )	
	PROP_REG( SizeMinY )	
	PROP_REG( LifeTimeMin )	
	PROP_REG( LifeTimeMax )	
	PROP_REG( PositionMin )	
	PROP_REG( PositionMax )	
	PROP_REG( VelocityMin )	
	PROP_REG( VelocityMax )	
	PROP_REG( AccelerationMin )	
	PROP_REG( AccelerationMax )	
	PROP_REG( AccelerationMax )
	PROP_REG( Fade )
	PROP_REG( Color )
	PROP_REG( scaleSizePerSec )

	}

	m_fx = core->Fx()->CreateFx(propResPath);
}

void FxResEdit::Start()
{

}

void FxResEdit::Work(float dt, int level)
{
	if (Scene()->IsRunning())
	{
	}

	if (m_fx)
	{
		if (m_fx->m_Res->GetType() == eEmiterType::type2d)
		{
			transform.Pos().x = 0.5f;
			transform.Pos().y = core->Render()->GetAspectRatio()/2.f;
		}
		else
			transform.Identity();
		
		m_fx->SetTransform(transform);
	}
}

void FxResEdit::Draw(float dt, int level)
{
	if (Scene()->IsRunning())
	{
	}

	if (m_fx)
	{
		if (m_fx->m_Res->GetType() == eEmiterType::type2d)
		{
			m_fx->Draw();
		}
	}
}