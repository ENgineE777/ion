#pragma once

#include "Core/Animation/Base/AnimationService.h"
#include "Core/Animation/Imp/AnimationResImp.h"

class AnimationServiceImp : public AnimationService
{		
public:		

	AnimationServiceImp();
	virtual ~AnimationServiceImp();

	virtual bool Init();
	virtual IAnimationInstance* LoadAnimation(const char* name, const char* file, int line);	
	virtual void ReleaseRes();		

	virtual const char* Name() { return "Legacy"; };

	virtual IAnimationController* CreateController(AnimationInstance* instance,
												   IAnimationController::ControllerType type,
												   const char* boneName, const char* boneOffsetName,
												   const Vector& min,const Vector& max, float speed);
	virtual IAnimationProcessor* CreateAnimationProcessor(AnimationInstance* instance);	

	virtual void Update(float dt);
	virtual void Trace(const char* formated, ...);

#ifdef PC
	void Save(const char* mdl, const char* path, Platform platform);
#endif
};