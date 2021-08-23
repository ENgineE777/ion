#pragma once

#include "TrackPlayer.h"
	
class EventProcessorPlayer : public TrackPlayer
{		
public:
	EventProcessorPlayer(TrackDescBase* Desc):TrackPlayer(Desc){ owner = NULL; };
	virtual void Init();
	void ExecuteFunc(TrackStringPoint* func);	
};