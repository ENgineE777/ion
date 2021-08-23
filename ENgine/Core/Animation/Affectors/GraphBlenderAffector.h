#pragma once

#include "Core/Animation/IAnimationAffector.h"

class GraphBlenderAffector: public IAnimationAffector
{	
	struct boneMat
	{
		int index;		
		Quaternion blendRot;
		Vector     blendPos;
	};

	std::vector<boneMat> bones;

	float blend_time;
	float blend_speed;

public:	
	
	GraphBlenderAffector(affecorDeclaratorBase*  Desc);
	void StartBlend(IAnimationInstance* from, float time);
	virtual void TransformBone(int index, Matrix* parentMat, Matrix* curMat);	
	virtual void Update(float dt);
};