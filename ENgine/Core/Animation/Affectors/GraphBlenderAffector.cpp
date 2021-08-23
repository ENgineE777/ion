
#include "GraphBlenderAffector.h"
#include "Core/Animation/IAnimationInstance.h"
#include "Core/Animation/Base/AnimationProcessor.h"


GraphBlenderAffector::GraphBlenderAffector(affecorDeclaratorBase*  Desc):IAnimationAffector(Desc)
{
	blend_time = -1.0f;
}

void GraphBlenderAffector::StartBlend(IAnimationInstance* from, float time)
{
	bones.clear();

	int num = from->GetNumBones();

	if (num>0)
	{
		for (int i=0;i<num;i++)
		{
			Matrix* mat = from->GetBoneLocalMatrix(i);

			boneMat bone;

			bone.blendRot.Set(*mat);
			bone.blendPos = mat->Pos();

			bones.push_back(bone);
		}	

		blend_time = 1.0f;
		blend_speed = 1.0f/time;		
	}
	else
	{
		blend_time = -1.0f;
	}
}

void GraphBlenderAffector::TransformBone(int index, Matrix* parentMat, Matrix* curMat)
{
	if (blend_time>0.0f)
	{		
		AnimationProcessor::BlendMatrix(curMat, &bones[index].blendRot, &bones[index].blendPos, blend_time);		
	}
}

void GraphBlenderAffector::Update(float dt)
{
	if (blend_time>0.0f)
	{
		blend_time -= dt * blend_speed;
		
		if (blend_time<0.0f)
		{
			blend_time = -1.0f;
		}
	}
}