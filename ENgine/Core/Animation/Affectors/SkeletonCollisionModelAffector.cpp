
#include "SkeletonCollisionModelAffector.h"

void CSkeletonCollisionModelAffector::TransformBone(int index, Matrix* parentMat, Matrix* curMat)
{
	for (int i=0;i<(int)bones.size();i++)
	{
		if (bones[i].index == index)
		{	
			*curMat = bones[i].mat;
			if (parentMat)
			{
				Matrix invParent = *parentMat;
				invParent.Inverse();

				(*curMat) = (*curMat) * invParent;
			}
			return;
		}
	}
}

void CSkeletonCollisionModelAffector::SetBoneTransform(int index,const Matrix &mat)
{
	int bn_index = -1;

	for (int i=0;i<(int)bones.size();i++)
	{
		if (bones[i].index == index)
		{
			bn_index = i;
			break;
		}
	}

	if (bn_index<0)
	{
		bones.push_back(boneMat());
		bn_index = (int)bones.size()-1;		
		bones[bn_index].index = index;
	}

	bones[bn_index].mat = mat;
}
