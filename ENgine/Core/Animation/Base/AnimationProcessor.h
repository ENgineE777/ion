#pragma once

#include "Core/Animation/IAnimationProcessor.h"
#include "Core/Animation/IAnimationClip.h"
#include "Common/Containers/HashMap.h"

class AnimationInstance;

class AnimationProcessor : public IAnimationProcessor
{		
	friend class AnimationProcessorImp;						
	
	int root_bone;	
	int colapse_to;	

	float bbox[6];	
	
	bool     delta_was_set;
	Vector  delta_pos;	
	Vector  last_pos;

	bool globalBlend;	

	Vector  cur_pos;
	Vector  boxOffset;	

	Matrix res_mat;	

public:			

	struct BoneMat
	{
		int			   index;
		int            parent;

		Matrix		   initMat;
		Matrix		   initInvMat;
		Matrix		   curMat;
		Matrix        curInvMat;
		Matrix        curLocMat;

		Quaternion	   blendRot;
		Vector	       blendPos;
		Matrix        blendMat;

		Quaternion	   mc_rot;
		Vector	       mc_pos;	
		bool           mc;

		Vector4       to_render[3];
		

		BoneMat()
		{
			index = -1;
			parent = -1;			
			mc = false;
		}
	};

	std::vector<BoneMat> bones;
	HashMap<int> bonesIndices;


	AnimationInstance* anim;	
	Matrix globalMatrix;
	Matrix inv_globalMatrix;

	bool allow_blend;

	float cur_blend_time;
	float blend_speed;

	struct cntrl_stack
	{
		Matrix mat;
		float ang;
		float rot;
		int index;
	};

	int cntrl_s_depth;
	cntrl_stack cntrl_s[10];	

	Matrix StackMat()
	{
		return cntrl_s[cntrl_s_depth].mat;
	};

	float StackAng()
	{		
		return cntrl_s[cntrl_s_depth].ang;
	};

	float StackRot()
	{		
		return cntrl_s[cntrl_s_depth].rot;
	};

	void UpdateStack(int index)
	{		
		if (cntrl_s_depth>0)
		{
			int prnt = GetBoneParentIndex(index);

			if (cntrl_s[cntrl_s_depth].index<prnt)
			{			
				while (cntrl_s[cntrl_s_depth].index<prnt)
				{
					index = prnt;
					prnt = GetBoneParentIndex(index);
				}
			}

			while (cntrl_s[cntrl_s_depth].index>prnt)
			{
				cntrl_s_depth--;
			}
		}

		cntrl_s[cntrl_s_depth+1].index = index;
	}

	void StackSetMat(Matrix mat)
	{
		cntrl_s[cntrl_s_depth+1].mat = cntrl_s[cntrl_s_depth].mat * mat;
	}

	void StackSetAng(float da)
	{
		cntrl_s[cntrl_s_depth+1].ang = cntrl_s[cntrl_s_depth].ang + da;
	}

	void StackSetRot(float dr)
	{
		cntrl_s[cntrl_s_depth+1].rot = cntrl_s[cntrl_s_depth].rot + dr;
	}

	void RiseDepth()
	{
		cntrl_s_depth++;
	}

	AnimationProcessor(AnimationInstance* instance);

	static void BlendMatrix(Matrix* mat, Quaternion* rotBlend, Vector* posBlend, float blendTime)
	{
		Quaternion rotation;
		rotation.Set(*mat);

		Vector translation = mat->Pos();

		Vector pos = 0.0f;
		pos.Lerp(translation, *posBlend, blendTime);	

		Quaternion rot;
		rot.SLerp( rotation, *rotBlend, blendTime);

		rot.GetMatrix(*mat);
		mat->Pos() = pos;
	};


	virtual void BonesMaping(Mesh::Bone* gm_bones, int numBones) = 0;
	virtual int  GetNumBones();
	virtual int  GetBoneIndex(const char* name);
	virtual int  GetBoneParentIndex(int index);
	virtual Vector GetRootOffset();
	virtual Matrix* GetBoneMatrix(int index);
	virtual Matrix* GetBoneLocalMatrix(int index);	
	virtual Matrix* GetBoneDelta(int index);
	virtual float* GetBBox();
	virtual void SetMovementBone(const char* bone);
	virtual void GetMovement(Matrix& mat);
	virtual void SetTransform(Matrix& transform);
	virtual void ResetDeltaPos(bool full);
	virtual void CalcGlobalMat(int bone_index,Matrix& mat);
	virtual void StartBlend(float blendTime, bool globalBlnd);
	virtual void ProcessBones();
	virtual void Update(float dt);
	virtual void Release();	

	virtual IAnimationClip* CreateClip() = 0;
	virtual bool InitClip(IAnimationClip* clip,void* ds, float inTime) = 0;	

	virtual void PrepareRenderData(Vector4* data, dword* cmp_bones, int num_complete_bones);
};
