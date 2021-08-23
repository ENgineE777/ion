#pragma once

#include "Core/Animation/IAnimationService.h"
#include "Core/Animation/Base/AnimationRes.h"
#include "Core/Files/FileBuffer.h"

class AnimationInstance;

class AnimationService : public IAnimationService
{			
protected:

	bool debugOn;
	bool drawSkeletonOn;

	class Clip
	{
	public:

		int ref;
		Mesh::Animation* anim;
		FileBuffer* buffer;

		Clip()
		{
			ref = 0;
			anim = null;
			buffer = null;
		};
	};
	
	HashMap<AnimationRes> animations;
	HashMap<Clip> clips;
	std::vector<AnimationInstance*> anim_instances;	

public:

	AnimationService();	
		
	virtual void Update(float dt);	

	virtual void SetDebug(bool enable);
	virtual bool IsDebug();
	
	virtual void SetDrawSkeleton(bool enable);
	virtual bool IsDrawSkeleton();

	virtual IAnimationController* CreateController(AnimationInstance* instance,
												   IAnimationController::ControllerType type,
												   const char* boneName, const char* boneOffsetName,
												   const Vector& min,const Vector& max, float speed) = 0;
	virtual IAnimationProcessor* CreateAnimationProcessor(AnimationInstance* instance) = 0;

	virtual void Trace(const char* formated, ...);

	AnimationInstance* AddAnim(AnimationRes* anim);
	void ReleaseAnim(AnimationInstance* anim);	

	virtual Mesh::Animation* LoadClip(const char* name);
	virtual void DeleteClip(Mesh::Animation* clip);
};
