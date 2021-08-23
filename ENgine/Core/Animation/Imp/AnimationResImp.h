#pragma once

#include "Core/Animation/Base/AnimationRes.h"
#include "Core/Files/FileBuffer.h"

class AnimationResImp : public AnimationRes
{	

public:
	
	struct Clip
	{
		float length;		
		Mesh::Animation* clip;
		std::vector<Mesh::BoneAnimTrack*> mapping;		
	};

	Mesh::Animation* master_clip;

	HashMap<Clip> ls_clips;	

	AnimationResImp(AnimationService* mngr);	

	virtual bool PreLoad(const char* name);
	virtual void LoadClip(const char* name, ClipDesc* desc);	
	virtual bool ClipsLoaded();
	virtual void Release();
};