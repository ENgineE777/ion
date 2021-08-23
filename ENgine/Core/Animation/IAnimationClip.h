#pragma once

#include "Common/Common.h"
#include "Common/Containers/HashMap.h"
//#include "Core/Model/Mesh.h"

class IAnimationClip
{
public:

	struct PlayData
	{
		float time;		

		float cur_weight;
		float target_weight;
		float weight_delta;	

		bool need_delete;
	};

	virtual ~IAnimationClip() {};

	virtual bool  NeedDelete() = 0;	
	virtual void  SetCustomData(void* data) = 0;
	virtual void  Pause(bool enbale) = 0;	
	virtual bool  IsPaused() = 0;
	virtual void  Stop(float time = -1.0f) = 0;
	virtual float GetMaxTime() = 0;	
	virtual float GetCurTime() = 0;
	virtual void  SetCurTime(float time) = 0;	
	virtual bool  SetWeight(float weight, float time) = 0;
	virtual float GetWeight() = 0;
	virtual void  SetPlaySpeed(float speed) = 0;
	virtual float GetPlaySpeed() = 0;
	virtual void  GetPlayData(PlayData& data) = 0;	
	virtual void  SetPlayData(PlayData data) = 0;	
};
