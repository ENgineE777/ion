
#include "RotateAffector.h"

void RotateMotorAffector::TransformBone(int index, Matrix* parentMat, Matrix* curMat)
{
	for (int i=0;i<(int)bones.size();i++)
	{
		if (bones[i].index == index)
		{		
			(*curMat) = bones[i].rotation * (*curMat);
			return;
		}
	}

}

void RotateMotorAffector::RotateBone(int index, const Matrix& rotation)
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

	if (bn_index < 0)
	{
		bones.push_back(boneMat());
		bn_index = (int)bones.size()-1;		
		bones[bn_index].index = index;
	}

	bones[bn_index].rotation = rotation;

}

void RotateMotorAffector::UnregisterBone(int index)
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