#pragma once

#include "TrackPlayer.h"

class AnimModelControllerPlayer : public TrackPlayer
{		
public:

	AnimModelControllerPlayer(TrackDescBase* Desc): TrackPlayer(Desc)
	{
		owner = NULL;
		m_AnimModelPlayer = NULL;
	};

	virtual void Init();
	virtual void ReInit();

private:
#ifdef EDITOR
	class AnimModelPlayerWidgets* m_propPanel;
#endif

	void ApplyTrans(TransformTrackPoint* tr);	
	void ApplyWeight(TrackFloatPoint* _value);

	class AnimModelPlayer* m_AnimModelPlayer;
	STRING_PROP(AnimModelControllerPlayer,m_AnimModelPlayerName, "", "AnimModelPlayer")
	STRING_PROP(AnimModelControllerPlayer,m_ControllerName, "", "Controller")
};