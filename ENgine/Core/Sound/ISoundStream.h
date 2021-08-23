
#pragma once

#include "Common/Common.h"

class ISoundStream
{
public:

	virtual bool IsPlaying() = 0;
	virtual void Pause(bool enable) = 0;

	virtual void AutoDelete(bool enable) = 0;
	virtual bool IsAutoDelete() = 0;

	virtual void SetLooped(bool loop) = 0;
	virtual void SetAttributes(int volume,int freq = -1) = 0;	

	virtual bool IsActive() = 0;

	virtual void Release() = 0;
};