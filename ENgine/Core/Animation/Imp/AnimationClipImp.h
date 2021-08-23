#pragma once

#include "Core/Animation/Base/AnimationClip.h"
#include "Core/Animation/Base/AnimationInstance.h"
#include "Core/Animation/Imp/AnimationResImp.h"

class AnimationClipImp : public AnimationClip
{					
public:	

	bool  paused;

	float cur_weight;
	float target_weight;
	float weight_delta;	

	struct PlayBackInfo
	{
		bool  mix;
		float playSpeed;
		float timeIn;
		float timeOut;
		int   playMode;	
		bool  additiveBlend;		

		AnimationResImp::Clip* clip;
	};

	std::vector<int> cur_keys;
	int cur_bbkeys[5];

	PlayBackInfo playInfo;

	AnimationInstance::PlayElement* elem;

	float cur_time;

	AnimationClipImp();

	void Prepare(AnimationInstance* anim);
	void Init(PlayBackInfo* info);

	virtual bool  NeedDelete();	
	virtual void  SetCustomData(void* data);
	virtual void  Pause(bool enbale);	
	virtual bool  IsPaused();
	virtual void  Stop(float time = -1.0f);
	virtual float GetMaxTime();	
	virtual float GetCurTime();
	virtual void  SetCurTime(float time);	
	virtual bool  SetWeight(float weight, float time);
	virtual float GetWeight();
	virtual void  SetPlaySpeed(float speed);
	virtual float GetPlaySpeed();
	virtual void  GetPlayData(PlayData& data);	
	virtual void  SetPlayData(PlayData data);
	
	void ResetKeys();
};