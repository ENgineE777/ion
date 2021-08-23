#pragma once

#include "IAnimationEvents.h"
#include "IAnimationAffector.h"
#include "IAnimationController.h"

class IAnimationInstance
{
public:	

	virtual ~IAnimationInstance() {};
	
	virtual void ActivateLink(const char* name, bool ignoreActivated = false) = 0;
	virtual void ActivateLink(unsigned int hash, bool ignoreActivated = false) = 0;
	
	virtual bool TestActivateLink(const char* name) = 0;
	virtual bool TestActivateLink(unsigned int hash) = 0;
	
	virtual const char* CurrentNodeName() = 0;

	virtual const char* GetNodeConstant(const char* name, const char* nodeName = 0) = 0;

	virtual void Pause(bool enable) = 0;
	virtual bool IsPaused() = 0;

	virtual void Restart() = 0;	

	virtual bool ActivateTableNames(const char* name) = 0;

	virtual void ResetNode(float blendTime = 0.2f) = 0;

	virtual void GotoNode(const char* name, float blendTime = 0.2f) = 0;
	virtual void GotoNode(unsigned int hash, float blendTime = 0.2f) = 0;
		
	virtual bool PlayLayer(const char* name) = 0;
	virtual bool PlayLayer(unsigned int hash) = 0;

	virtual bool IsLayerPlaying(const char* name) = 0;
	virtual bool IsLayerPlaying(unsigned int hash) = 0;

	virtual void StopLayer(const char* name, float time = -1) = 0;
	virtual void StopLayer(unsigned int hash, float time = -1) = 0;

	virtual bool SetLayerWeight(const char* name, float weight, float time = -1.0f) = 0;
	virtual bool SetLayerWeight(unsigned int hash, float weight, float time = -1.0f) = 0;

	virtual float GetLayerWeight(const char* name) = 0;
	virtual float GetLayerWeight(unsigned int hash) = 0;

	virtual	void SetLayerPlaySpeed(const char* name, float speed) = 0;
	virtual void SetLayerPlaySpeed(unsigned int hash, float speed) = 0;

	virtual	void SetLayerPosition(const char* name, float pos) = 0;
	virtual void SetLayerPosition(unsigned int hash, float pos) = 0;

	virtual void GetMovement(Matrix& mat) = 0;
	virtual bool IsNodeHasMovement() = 0;

	virtual void BonesMaping(Mesh::Bone* bones, int numBones) = 0;
	
	virtual int  GetNumBones() = 0;
	virtual int  GetBoneIndex(const char* name) = 0;
	virtual int  GetBoneParentIndex(int index) = 0;
	virtual Matrix* GetBoneMatrix(int index) = 0;	
	virtual Matrix* GetBoneLocalMatrix(int index) = 0;
	virtual Matrix* GetBoneMatrixMulGlTrans(int index) = 0;	
	virtual Matrix* GetBoneDelta(int index) = 0;

	virtual float* GetBBox() = 0;

	virtual void SetEvents(IAnimationEvents* events) = 0;

	virtual void SetTransform(Matrix& transform) = 0;	
	virtual void GetTransform(Matrix& transform) = 0;	

	virtual void AddController(IAnimationController::ControllerType type,
							   const char* name, const char* boneName,const char* boneOffsetName,
							   Vector& min, Vector& max, float speed) = 0;

	virtual const char* GetControllerName(int index) = 0;
	virtual int  GetControllersCount() = 0;

	virtual void SetMovementBone(const char* bone) = 0;

	#ifdef ANIM_STANDALONE
	virtual void  GetControllerEDMatrix(const char* id, Matrix& mat) = 0;
	#endif	

	virtual void SetControllerWeight(int index,float weight,float time) = 0;
	virtual void SetControllerWeight(const char* name,float weight,float time) = 0;
	virtual void SetControllersWeight(float weight,float time) = 0;

	virtual float GetControllerWeight(int index) = 0;
	virtual float GetControllerWeight(const char* name) = 0;

	virtual void SetControllerGoal(int index, const Vector& goal) = 0;
	virtual void SetControllerGoal(const char* name, const Vector& goal) = 0;	
	virtual void SetControllersGoal(const Vector& goal) = 0;	
	
	virtual void SetControllerAdditionalParameter(int index, void* userData) = 0;
	virtual void SetControllerAdditionalParameter(const char* name, void* userData) = 0;
	virtual void SetControllersAdditionalParameter(void* userData) = 0;

	virtual void SetControllerConstraints(const char* name, const Vector& min, const Vector& max) = 0;

	virtual void SetControllerPreRotation(const char* name, Matrix& transform) = 0;
	
	virtual void SetControllerBendDirection(const char* name, const Vector& dir) = 0;	

	virtual void SetControllerOffset(const char* name, const Vector& offset) = 0;

	virtual void SetControllersSpeed(float speed) = 0;	
	virtual void SetControllerSpeed(const char* name,float speed) = 0;	

	virtual void RemoveController(const char* boneName) = 0;

	virtual void AddAffector(IAnimationAffector* affector, int level) = 0;
	virtual void DeleteAffector(IAnimationAffector* affector) = 0;

	// Chache
	virtual void StartRecord() = 0;
	virtual void RecordAction(int cmd, const char* name, float time) = 0;	
	virtual void ProcessRecord(float dt) = 0;
	virtual void FinishRecord() = 0;	
	virtual void SetTime(float time) = 0;

	virtual void Release() = 0;

	virtual void DrawDebug() = 0;

	virtual bool BonesReady() = 0;

	virtual void PrepareRenderData(Vector4* data, dword* cmp_bones, int num_complete_bones) = 0;
};

