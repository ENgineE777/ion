#pragma once

#include "TrackPlayer.h"
	
class CameraPlayer : public TrackPlayer
{		
public:
   
	virtual void Init();

	void ApplyTrans(TrackPoint* tr);
	void ApplyFov(TrackPoint* fov);

#ifdef EDITOR
	void OnStopEdit();
#endif	
};