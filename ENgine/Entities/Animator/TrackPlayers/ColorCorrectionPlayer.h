#pragma once

#include "TrackPlayer.h"

class ColorCorrectionPlayer : public TrackPlayer
{		
public:
	ColorCorrectionPlayer(TrackDescBase* Desc):TrackPlayer(Desc){ owner = NULL; };
	virtual void Init();
	void ExecuteFunc(class Vector4DataTrackPoint* _curvalue);
};