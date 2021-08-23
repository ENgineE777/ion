
#include "AnimationInstance.h"
#include "AnimationProcessor.h"
#include "Core/Core.h"


AnimationProcessor::AnimationProcessor(AnimationInstance* instance) : bonesIndices(_FL_)
{
	anim = instance;	

	bbox[0] = bbox[1] = bbox[2] = -2.0f;
	bbox[3] = bbox[4] = bbox[5] = 2.0f;

	cur_blend_time = 0.0f;
	blend_speed = 1.0f;	

	root_bone = -1;
	colapse_to = -1;

	globalBlend = false;
	
	globalMatrix.Identity();
	inv_globalMatrix.Identity();

	allow_blend = false;

	delta_pos.Set(0.0f);
	last_pos.Set(0.0f);

	cur_pos.Set(0.0f);
}

int AnimationProcessor::GetNumBones()
{
	return (int)bones.size();

}

int  AnimationProcessor::GetBoneIndex(const char* name)
{
	const int* index = bonesIndices.Get(name);

	if (index)
	{
		return *index;
	}

	return -1;
}

int AnimationProcessor::GetBoneParentIndex(int index)
{
	return bones[index].parent;
}

Vector AnimationProcessor::GetRootOffset()
{	
	return globalMatrix.MulNormal(cur_pos);
}

Matrix* AnimationProcessor::GetBoneMatrix(int index)
{
	if (index >= (int)bones.size()) return NULL;
	
	return &bones[index].curMat;
}

Matrix* AnimationProcessor::GetBoneLocalMatrix(int index)
{
	if (index >= (int)bones.size()) return NULL;

	return &bones[index].curLocMat;
}

Matrix* AnimationProcessor::GetBoneDelta(int index)
{
	if (index >= (int)bones.size()) return NULL;

	res_mat = bones[index].initInvMat * bones[index].curMat * globalMatrix;

	return &res_mat;	
}

float* AnimationProcessor::GetBBox()
{
	return bbox;
}

void AnimationProcessor::SetMovementBone(const char* bone)
{
	int* cl_bn = bonesIndices.Get(bone);

	if (cl_bn)
	{
		colapse_to = *cl_bn;
	}
}

void AnimationProcessor::GetMovement(Matrix& mat)
{
	mat.Identity();
	mat.Pos() = delta_pos;
}

void AnimationProcessor::SetTransform(Matrix& transform)
{
	globalMatrix = transform;
	inv_globalMatrix = transform;	
	inv_globalMatrix.Inverse();	
}

void AnimationProcessor::ResetDeltaPos(bool full)
{
	if (full)
	{
		delta_pos.Set(0.0f);
	}

	last_pos.Set(0.0f);

	delta_was_set = false;
}

void AnimationProcessor::CalcGlobalMat(int bone_index,Matrix& mat)
{
	if (bones[bone_index].parent != -1)
	{		
		CalcGlobalMat(bones[bone_index].parent, mat);
		mat =  bones[bone_index].curLocMat * mat;
	}
	else
	{
		mat = bones[bone_index].curLocMat;
	}
}

void AnimationProcessor::StartBlend(float blendTime, bool globalBlnd)
{
	if (!allow_blend)
	{
		cur_blend_time = 0.0f;
		return;
	}

	if (blendTime>0.0f)
	{
		for (int i=0;i<(int)bones.size();i++)
		{
			BoneMat* bone_mat = &bones[i];			

			Matrix* mat = &bone_mat->curLocMat;

			if (globalBlnd)
			{
				mat = &bone_mat->curMat;
			}

			bone_mat->blendRot.Set( *mat );
			bone_mat->blendPos = mat->Pos();
		}

		cur_blend_time = 1.0f;
		blend_speed = 1.0f/blendTime;
		globalBlend = globalBlnd;
	}
	else
	{
		cur_blend_time = 0.0f;
		blend_speed = -1.0f;
	}

	//cur_blend_time = 0.0f;
}

void AnimationProcessor::ProcessBones()
{		
	if (colapse_to>=0) // && anim->cur_node->getMovement>0)
	{
		Matrix mt(true);		
		CalcGlobalMat(colapse_to,mt);

		cur_pos = mt.Pos();

		boxOffset -= cur_pos;		
		bones[0].curLocMat.Pos() = bones[0].curLocMat.Pos() - cur_pos;
	}
	else
	{
		cur_pos.Set(0.0f);
	}

	if (root_bone>=0 && anim->cur_node->getMovement>0)
	{		
		if (delta_was_set)
		{
			delta_pos = cur_pos - last_pos;					
		}
		else
		{
			delta_was_set = true;					
		}

		last_pos = cur_pos;
	}

	cntrl_s_depth = 0;
	cntrl_s[0].mat.Identity();
	cntrl_s[0].ang = 0.0f;
	cntrl_s[0].rot = 0.0f;
	cntrl_s[0].index = -1;

	for (int i=0; i<(int)bones.size(); i++)
	{
		BoneMat* boneMat = &bones[i];
		
		bool hasParent = false;

		Matrix* parentMat = NULL;

		if (boneMat->parent != -1)
		{
			hasParent = true;
			parentMat = &bones[boneMat->parent].curMat;
		}

		for (int j=0; j<(int)anim->affectors.size();j++)
		{
			anim->affectors[j].affector->TransformBone(i, parentMat, &boneMat->curLocMat);
		}

		if (!globalBlend && cur_blend_time>0.0f)
		{
			BlendMatrix(&boneMat->curLocMat, &boneMat->blendRot, &boneMat->blendPos, cur_blend_time);
		}

		/*if (hasParent)
		{
			boneMat->curMat = boneMat->curLocMat * (*parentMat);
		}
		else*/
		{
			boneMat->curMat = boneMat->curLocMat;
		}

		//boneMat->initInvMat.MultiplyTransposed(boneMat->curMat, boneMat->to_render);
	}

	if (globalBlend && cur_blend_time>0.0f)
	{
		for (int i=0; i<(int)bones.size(); i++)
		{			
			BoneMat* boneMat = &bones[i];
			BlendMatrix(&boneMat->curMat, &boneMat->blendRot, &boneMat->blendPos, cur_blend_time);
		}
	}

	if (!allow_blend)
	{
		allow_blend = true;
	}
}

void AnimationProcessor::Update(float dt)
{
	if (cur_blend_time>0.0f)
	{
		cur_blend_time -= dt * blend_speed;

		if (cur_blend_time<0.0f)
		{
			cur_blend_time = 0.0f;
		}
	}
}

void AnimationProcessor::PrepareRenderData(Vector4* data, dword* cmp_bones, int num_complete_bones)
{
	for (int index = 0; index < num_complete_bones; index++)
	{
		memcpy(&data[3*index], bones[cmp_bones[index]].to_render, sizeof(float) * 12);
	}
}

void AnimationProcessor::Release()
{
	bones.clear();	
	bonesIndices.Clear();
	
	delete this;
}
