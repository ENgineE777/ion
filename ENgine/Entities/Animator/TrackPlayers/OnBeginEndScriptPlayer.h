#pragma once

#include "TrackPlayer.h"
	
class OnBeginEndScriptPlayer : public TrackPlayer
{		
public:
	OnBeginEndScriptPlayer(TrackDescBase* Desc):TrackPlayer(Desc){ owner = NULL; };
	virtual void Init();
	virtual void StartPlay();
	virtual void EndPlay();

	void ExecuteFunc(const TrackStringPoint* func);	
};