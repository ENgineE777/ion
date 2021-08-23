#pragma once

#ifdef FBX

#include "core/Model/ModelRes.h"
#include <fbxsdk.h>

class AnimationFbxLoader
{
public:	
		
	static KFbxSdkManager* sdkManager;
	static KFbxScene* scene;
	static float grab_time;

	static void CountBones(KFbxNode* pNode, int& num_bones);
	static void ExtractBones(Mesh::Animation* res, KFbxNode* pNode, int& cur_bone);

	static void ExtractBonesTrack(Mesh::Animation* res, KFbxNode* pNode, KTime& time, int& cur_bone, int cur_key);

	static bool Load(Mesh::Animation* res, const char* name);
};

#endif