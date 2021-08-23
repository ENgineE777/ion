
#include "AnimationClipImp.h"

AnimationClipImp::AnimationClipImp()
{	
}

void AnimationClipImp::Prepare(AnimationInstance* anim)
{
	cur_keys.clear();		
	cur_keys.resize(anim->processor->GetNumBones());	
}

void AnimationClipImp::Init(PlayBackInfo* info)
{	
	playInfo = *info;
	elem = 0;

	ResetKeys();

	paused = false;

	if (playInfo.timeIn <= 0.01f)
	{
		cur_weight = target_weight = weight_delta = 1.0f;		
	}
	else
	{
		cur_weight = 0.0f;
		target_weight = 1.0f;
		weight_delta = 1.0f/playInfo.timeIn;	
	}

	need_delete = false;

	if (playInfo.playMode == 0)
	{
		cur_time = 0.0f;
	}
	else
	{
		cur_time = playInfo.clip->length;
		playInfo.playSpeed = -playInfo.playSpeed;
	}
}

bool AnimationClipImp::NeedDelete()
{
	return (need_delete && cur_weight<0.001f);
}

void AnimationClipImp::SetCustomData(void* data)
{
	elem = (AnimationInstance::PlayElement*)data;	
}

void AnimationClipImp::Pause(bool enbale)
{	
	paused = enbale;		
}

bool AnimationClipImp::IsPaused()
{
	return paused;
}

void AnimationClipImp::Stop(float time)
{
	if (need_delete) return;

	need_delete = true;
	target_weight = 0.0f;

	if (time>0.01f)
	{
		weight_delta = -cur_weight / time;	
	}
	else
	if (time>=0.0f)
	{
		cur_weight = 0.0f;
	}
	else
	{
		weight_delta = -cur_weight / playInfo.timeOut;	
	}
}

float AnimationClipImp::GetMaxTime()
{
	return playInfo.clip->length;
}

float AnimationClipImp::GetCurTime()
{
	if (playInfo.clip->length<0.001f) return 0.0f;

	return cur_time / playInfo.clip->length;
}

void AnimationClipImp::SetCurTime(float time)
{	
	if (need_delete) return;

	cur_time = time * playInfo.clip->length;	
}		

bool AnimationClipImp::SetWeight(float weight, float time)
{
	if (need_delete) return false;

	if (time<0.0f)
	{
		time = playInfo.timeIn;
	}

	if (time > 0.01f)
	{
		if (fabs(weight - target_weight) > 0.01f)
		{			
			target_weight = weight;
			weight_delta = (target_weight-cur_weight) / time;
		}
	}
	else
	{
		target_weight = weight;
		cur_weight = weight;
	}

	return true;
}

float AnimationClipImp::GetWeight()
{	
	return cur_weight;
}

void AnimationClipImp::SetPlaySpeed(float speed)
{
	playInfo.playSpeed = speed;
}

float AnimationClipImp::GetPlaySpeed()
{	
	return playInfo.playSpeed;
}

void AnimationClipImp::GetPlayData(PlayData& data)
{	
	data.cur_weight = cur_weight;
	data.target_weight = target_weight;
	data.weight_delta = weight_delta;	
	data.need_delete = need_delete;

	AnimationClip::GetPlayData(data);	
}

void AnimationClipImp::SetPlayData(PlayData data)
{
	cur_weight = data.cur_weight;
	target_weight = data.target_weight;
	weight_delta = data.weight_delta;	
	need_delete = data.need_delete;
	SetCurTime(data.time);
}

void AnimationClipImp::ResetKeys()
{
	for (int i=0; i<(int)cur_keys.size(); i++)
	{		
		cur_keys[i] = 0;
	}

	for (int i=0; i<5; i++)
	{		
		cur_bbkeys[i] = 0;				
	}	
}