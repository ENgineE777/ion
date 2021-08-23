#pragma once

#include "Core/Animation/IAnimationAffector.h"

class RagDollAffector: public IAnimationAffector
{	
	struct boneMat
	{
		int index;
		Matrix mat;
	};

	std::vector<boneMat> bones;

public:	
	RagDollAffector()
	{}
	RagDollAffector(affecorDeclaratorBase*  Desc):IAnimationAffector(Desc)
	{}
	virtual void TransformBone(int index, Matrix* parentMat, Matrix* curMat);
	void SetBoneTransform(int index,const Matrix &mat);
	void UnregisterBone(int index);	
};