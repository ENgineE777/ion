#pragma once

#include "TrackPlayer.h"
	
class ScriptPlayer : public TrackPlayer
{		
public:
	ScriptPlayer(TrackDescBase* Desc):TrackPlayer(Desc){ owner = NULL; };
	virtual void Init();
	void ExecuteFunc(TrackStringPoint* func);	
};