
#include "Core/Sound/Sound.h"
#include "../CutSceneManager.h"

#include "SoundPlayer.h"
#include "DeepBlack/World.h"

TRACKDESC(SoundPlayer, "SoundPlayer", 0)

void SoundPlayer::Init()
{	
	GameBaseEntityPlayer::Init();
	//remove BoolDataTrack "Visibility" from base class
	//tracks.pop_back(); // leak
	AddTransformDataTrack( Transform ,5, false,(TrackPlayer::KeyFunc)&SoundPlayer::ApplyTrans, NULL)
	AddStringDataTrack( Name , NULL, (TrackPlayer::ChangeKeyFunc)&SoundPlayer::ExecuteFunc)	

}

CGameBaseEntity* SoundPlayer::GetEntity()
{
	return m_sound;
}


void SoundPlayer::Release()
{
	if (m_sound)
	{
		m_sound->setFlag(true, CBaseEntity::eDeleted);
		m_sound = NULL;
	}

	GameBaseEntityPlayer::Release();
}

void SoundPlayer::EndPlay()
{
	if (m_sound && !m_isEntityUsedAfterEndPlay)
	{
		m_sound->setFlag(true, CBaseEntity::eDeleted);
		m_sound = NULL;
	}
}


void SoundPlayer::ApplyTrans(TransformTrackPoint* tr)
{
	if (m_sound)
	{
		NxMat34 mat;
		IMatrix imat;
		tr->rot.GetMatrix(imat);
		imat.Pos() = tr->pos;
		imat.Get(mat);
		m_sound->setTransform(mat);
	}
}

void SoundPlayer::ExecuteFunc(TrackStringPoint* func)
{	
	if ( !CWorld::instance()->getUpdateScene() )
		return;

	CBaseEntity* parent = NULL;
	sHook hook(sHook::g_IdentityHook);

		if (m_sound)
		{
			if (m_sound->getParent())
			{
				parent = m_sound->getParent();
				hook = m_sound->getHook();
				m_sound->getParent()->removeChild(m_sound);
			}

			m_sound->setFlag(true, CBaseEntity::eDeleted);
			m_sound = NULL;
		}


		TransformDataTrack* transTrack = (TransformDataTrack*)tracks[0];
		
		NxMat34 mat;
		IMatrix imat;
		transTrack->GetCurValue().rot.GetMatrix(imat);
		imat.Pos() = transTrack->GetCurValue().pos;
		imat.Get(mat);


#ifdef EDITOR
	if (g_GameProxy.EditorMode)
	{
		if (CWorld::instance()->getUpdateScene()->getCutSceneManager()->ed_played && !CWorld::instance()->getUpdateScene()->getCutSceneManager()->ed_playBack)
			m_sound = CWorld::instance()->getUpdateScene()->createLocalEntity(  eSOUNDSOURCE, mat, "", false, &CSoundSource::SConstructData( func->val, false, false, false, false, true ) );
	}
	else 
		m_sound = CWorld::instance()->getUpdateScene()->createLocalEntity(  eSOUNDSOURCE, mat, "", false, &CSoundSource::SConstructData( func->val, false, false, false, false, true ) );

#else
	m_sound = CWorld::instance()->getUpdateScene()->createLocalEntity(  eSOUNDSOURCE, mat, "", false, &CSoundSource::SConstructData( func->val, false, false, false, false, true ) );
#endif

	if (parent)
		parent->addChild(m_sound, hook);
	else if ( strlen(m_PropPlayerName))
	{
		ReInit();
	}

			
}