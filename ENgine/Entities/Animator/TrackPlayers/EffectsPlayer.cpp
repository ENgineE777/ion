
#include "EffectsPlayer.h"
#include "DeepBlack/World.h"
#include "DeepBlack/Cutscene/CutSceneManager.h"
#include "Core/Effects/NgFxEntity.h"
#include "DeepBlack/gui/gameGuiScenes.h"

TRACKDESC(EffectsPlayer, "EffectsPlayer", 0)

void EffectsPlayer::Init()
{	
	GameBaseEntityPlayer::Init();

	AddTransformDataTrack( Transform ,5, false,  (TrackPlayer::KeyFunc)&EffectsPlayer::ApplyTrans, NULL)
	AddStringDataTrackNoBlend( Effect , NULL, (TrackPlayer::ChangeKeyFunc)&EffectsPlayer::CreateEffect)	



}

void EffectsPlayer::Load(StreamLoader* stream)
{
	GameBaseEntityPlayer::Load(stream);

#if PC
	if (!g_GameProxy.PackMode)
		return;

	StringDataTrack* effectTrack = static_cast<StringDataTrack*>(GetDataTrack("Effect"));
	const std::vector<TrackStringPoint>& points = effectTrack->GetPoints();

	UINT size = points.size();
	for (UINT i = 0; i < size; ++i)
	{
		core.Files()->RegisterFile(points[i].val, "fx" );
	}
#endif
	
}

CGameBaseEntity* EffectsPlayer::GetEntity()
{
	return m_Effect;
}

void EffectsPlayer::Release()
{
	if (m_Effect)
	{
		m_Effect->setFlag(true, CBaseEntity::eDeleted);
		m_Effect = NULL;
	}
	GameBaseEntityPlayer::Release();
}

void EffectsPlayer::EndPlay()
{
	if (m_Effect && !m_isEntityUsedAfterEndPlay)
	{
		m_Effect->setFlag(true, CBaseEntity::eDeleted);
		m_Effect = NULL;
	}
}


void EffectsPlayer::ApplyTrans(TransformTrackPoint* _value)
{	
	if (GetEntity() && GetEntity()->getParent())
		return;

		IMatrix transform;
		_value->rot.GetMatrix(transform);
		transform.Pos() = _value->pos;

		NxMat34 nxtransform;
		transform.Get(nxtransform);

		if (m_Effect)
		{
			m_Effect->setTransform(nxtransform);
		}
}

void EffectsPlayer::CreateEffect(TrackStringPoint* func)
{	
	if ( !CWorld::instance()->getUpdateScene() )
		return;

	IMatrix transform;
	NxMat34 nxtransform;
	
	//TransformDataTrack* transformTrack = (TransformDataTrack*)GetDataTrack("Transform");
	//transformTrack->GetCurValue().rot.GetMatrix(transform);
	//transform.Pos() = transformTrack->GetCurPoint().pos;
	//transform.Get(nxtransform);
	

	CBaseEntity* parent = NULL;
	sHook hook(sHook::g_IdentityHook);

	if (m_Effect)
	{
		if (m_Effect->getParent())
		{
			parent = m_Effect->getParent();
			hook = m_Effect->getHook();
			m_Effect->getParent()->removeChild(m_Effect);
		}

		m_Effect->setFlag(true, CBaseEntity::eDeleted);
		m_Effect = NULL;
	}

#ifdef EDITOR
	if (g_GameProxy.EditorMode)
	{
		if (CWorld::instance()->getUpdateScene()->getCutSceneManager()->ed_played && !CWorld::instance()->getUpdateScene()->getCutSceneManager()->ed_playBack)
		{
			m_Effect = (FxEntity*)CWorld::instance()->getUpdateScene()->
				createLocalEntity(eEFFECT, nxtransform, "", false,
				&FxEntity::SConstructData(func->val,false,false,false,false,true));
		}
	}
	else 
	{
		m_Effect = (FxEntity*)CWorld::instance()->getUpdateScene()->
			createLocalEntity(eEFFECT, nxtransform, "", false,
			&FxEntity::SConstructData(func->val,false,false,false,false,true));
	}

#else
	m_Effect = (FxEntity*)CWorld::instance()->getUpdateScene()->
		createLocalEntity(eEFFECT, nxtransform, "", false,
		&FxEntity::SConstructData(func->val,false,false,false,false,true));
#endif

	

	if (parent)
		parent->addChild(m_Effect, hook);
	else if (strlen(m_PropPlayerName))
	{
		ReInit();
	}
			
}