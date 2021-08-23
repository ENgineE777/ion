
#include "RagDollAffector.h"

void RagDollAffector::TransformBone(int index, Matrix* parentMat, Matrix* curMat)
{
	for (int i=0;i<(int)bones.size();i++)
	{
		if (bones[i].index == index)
		{		
			Matrix curGl;

			if (parentMat)
			{
				curGl = (*curMat) * (*parentMat);	
			}
			else
			{
				curGl = (*curMat);	
			}

			Vector pos = curGl.Pos();

			*curMat = bones[i].mat;

			//curMat->Pos() = pos;

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

void RagDollAffector::SetBoneTransform(int index,const Matrix &mat)
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

void RagDollAffector::UnregisterBone(int index)
{
	for (int i=0;i<(int)bones.size();i++)
	{
		if (bones[i].index == index)
		{
			for (int j=0;j<(int)bones.size()-1;j++)			
			{
				bones[j] = bones[j+1];
			}

			bones.pop_back();			

			return;
		}
	}

}