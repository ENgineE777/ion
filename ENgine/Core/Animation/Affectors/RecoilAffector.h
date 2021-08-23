#pragma once

#include "Core/Animation/IAnimationAffector.h"

class RecoilAffector: public IAnimationAffector
{	
	struct boneMat
	{
		int index;
		float curTime;
		float strenght;
		float speed;

		Quaternion rot;		
	};

	std::vector<boneMat> bones;

public:
	RecoilAffector(affecorDeclaratorBase*  Desc):IAnimationAffector(Desc)
	{}
	virtual void TransformBone(int index, Matrix* parentMat, Matrix* curMat);
	void RecoilBone(int index, float time, float strenght, Vector* offset = NULL );
	virtual void Update(float dt);
};