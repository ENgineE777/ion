#pragma once

#include "TrackPlayer.h"

class SubtitlesPlayer : public TrackPlayer
{		
public:
	SubtitlesPlayer(TrackDescBase* Desc):TrackPlayer(Desc){ owner = NULL; };
	virtual void Init();
	virtual void EndPlay();
	void ExecuteFunc(TrackStringPoint* func);	
};