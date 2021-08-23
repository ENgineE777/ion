#pragma once

#include "Core/Animation/IAnimationAffector.h"

class CSkeletonCollisionModelAffector: public IAnimationAffector
{	
	struct boneMat
	{
		int		index;
		Matrix mat;
	};

	std::vector<boneMat> bones;

public:	
	CSkeletonCollisionModelAffector():IAnimationAffector()
	{}
	CSkeletonCollisionModelAffector(affecorDeclaratorBase*  Desc):IAnimationAffector(Desc)
	{}
	virtual void TransformBone(int index, Matrix* parentMat, Matrix* curMat);
	void SetBoneTransform(int index,const Matrix &mat);

};