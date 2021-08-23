#pragma once

#include "Core/Animation/Base/AnimationProcessor.h"

class AnimationResImp;

class AnimationProcessorImp : public AnimationProcessor
{	
public:	
	
	AnimationResImp* res;	

	AnimationProcessorImp(AnimationInstance* instance);

	virtual void BonesMaping(Mesh::Bone* gm_bones, int numBones);	
	virtual void ProcessBones();	
	virtual void Update(float dt);
	
	virtual IAnimationClip* CreateClip();
	virtual bool InitClip(IAnimationClip* clip,void* ds, float inTime);

	virtual bool UpdateClip(void* cl, float dt);
};