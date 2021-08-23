#pragma once

#include "IAnimationInstance.h"

class IAnimationService
{
public:	

	virtual ~IAnimationService() {};	

	virtual bool Init() = 0;
	virtual void Update(float dt) = 0;
	virtual IAnimationInstance* LoadAnimation(const char* name, const char* file, int line) = 0;

	virtual void SetDebug(bool enable) = 0;
	virtual bool IsDebug() = 0;

	virtual void SetDrawSkeleton(bool enable) = 0;
	virtual bool IsDrawSkeleton() = 0;

	virtual void Trace(const char* formated, ...) = 0;
	
	virtual const char* Name() = 0;

	virtual void ReleaseRes() = 0;

	//virtual Mesh::Animation* LoadClip(const char* name) = 0;

	//virtual void DeleteClip(Mesh::Animation* clip) = 0;
};
