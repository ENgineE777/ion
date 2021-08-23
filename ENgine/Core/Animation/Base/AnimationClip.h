#pragma once

#include "Core/Animation/IAnimationClip.h"

class AnimationInstance;

class AnimationClip : public IAnimationClip
{
public:

	virtual ~AnimationClip() {}

	bool need_delete;	

	virtual void  Prepare(AnimationInstance* anim) {};

	virtual void  GetPlayData(PlayData& data)
	{
		data.time = GetCurTime();
		if (data.time>1.0f) data.time = 1.0f;
	};

	virtual void  SetPlayData(PlayData data) {};
};
