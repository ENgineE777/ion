#pragma once

#include "IAnimationClip.h"
#include "Core/Model/Mesh.h"

class IAnimationProcessor
{	
public:	

	virtual ~IAnimationProcessor() {};
			
	virtual void BonesMaping(Mesh::Bone* gm_bones, int numBones) = 0;

	virtual int  GetNumBones() = 0;
	virtual int  GetBoneIndex(const char* name) = 0;
	virtual int  GetBoneParentIndex(int index) = 0;
	virtual Vector GetRootOffset() = 0;
	virtual Matrix* GetBoneMatrix(int index) = 0;
	virtual Matrix* GetBoneLocalMatrix(int index) = 0;
	virtual Matrix* GetBoneDelta(int index) = 0;
	virtual float* GetBBox() = 0;
	virtual void SetMovementBone(const char* bone) = 0;
	virtual void GetMovement(Matrix& mat) = 0;
	virtual void SetTransform(Matrix& transform) = 0;
	virtual void ResetDeltaPos(bool full) = 0;
	virtual void CalcGlobalMat(int bone_index,Matrix& mat) = 0;
	virtual void StartBlend(float blendTime, bool globalBlnd) = 0;
	virtual void ProcessBones() = 0;
	virtual void Update(float dt) = 0;
	virtual IAnimationClip* CreateClip() = 0;
	virtual bool InitClip(IAnimationClip* clip,void* ds, float inTime) = 0;	
	virtual bool UpdateClip(void* cl, float dt) { return true; };
	virtual void PrepareRenderData(Vector4* data, dword* cmp_bones, int num_complete_bones) = 0;
	virtual void Release() = 0;	
};
