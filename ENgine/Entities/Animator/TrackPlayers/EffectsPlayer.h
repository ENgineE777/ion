#pragma once

#include "GameBaseEntityPlayer.h"

class EffectsPlayer : public GameBaseEntityPlayer
{		
public:
	EffectsPlayer(TrackDescBase* Desc):GameBaseEntityPlayer(Desc)
	{
		m_Effect = NULL; 
		GameBaseEntityPlayer::ms_AttachedToTypes.insert("EffectsPlayer");
	};

	virtual class CGameBaseEntity* GetEntity();
	virtual void Init();
	virtual void Load(StreamLoader* stream);
	void CreateEffect(TrackStringPoint* func);	
	void ApplyTrans(TransformTrackPoint* _value);

	void Release();
	void EndPlay();

private:
	class FxEntity* m_Effect;
};