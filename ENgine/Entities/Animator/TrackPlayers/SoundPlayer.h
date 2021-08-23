#pragma once

#include "GameBaseEntityPlayer.h"

class SoundPlayer : public GameBaseEntityPlayer
{		
public:
	SoundPlayer(TrackDescBase* Desc):GameBaseEntityPlayer(Desc)
	{ 
		m_sound = NULL;
		GameBaseEntityPlayer::ms_AttachedToTypes.insert("SoundPlayer");
	};

	virtual class CGameBaseEntity* GetEntity();

	virtual void Init();
	virtual void Release();
	virtual void EndPlay();
	void ExecuteFunc(TrackStringPoint* func);	
	void ApplyTrans(TransformTrackPoint* tr);

private:

	CGameBaseEntity* m_sound;

};