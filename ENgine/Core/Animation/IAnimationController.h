#pragma once

#include "IAnimationProcessor.h"

class IAnimationController
{	
public:	

	enum ControllerType
	{
		lookat = 0,
		ik = 1
	};
	
	virtual ~IAnimationController() {};

	virtual bool  Init(IAnimationProcessor* anim_processor, ControllerType type,
					   const char* boneName,const char* boneOffsetName,
					   const Vector& min, const Vector& max, float speed) = 0;

	virtual void  Activate(bool act) = 0;
	virtual bool  IsActive() = 0;
	virtual void  SetWeight(float weight, float time) = 0;	
	virtual float GetWeight() = 0;
	virtual float GetCurWeight() = 0;
	virtual void  SetGoal(const Vector& goal) = 0;	
	virtual void  SetAdditionalParameter(const void* userData) = 0;	
	virtual void  SetConstraints(const Vector& min,const Vector& max) = 0;	
	virtual void  SetPreRotation(Matrix& transform) = 0;
	virtual void  SetBendDirection(const Vector& dir) = 0;	
	virtual void  SetSpeed(float speed) = 0;

	virtual void  SetOffset(const Vector& offset) = 0;

	#ifdef ANIM_STANDALONE
	virtual void  GetEDMatrix(Matrix& mat) = 0;
	#endif

	virtual void  Update(float dt) = 0;	
	virtual void  Release() = 0;	
};
