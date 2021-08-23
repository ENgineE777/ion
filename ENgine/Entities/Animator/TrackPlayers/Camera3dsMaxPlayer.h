#pragma once

#include "CameraPlayer.h"
#include "core/Model/Mesh.h"
	
class Camera3dsMaxPlayer : public CameraPlayer
{	
	struct SCameraAnimationRes
	{
		uint NumKeys;
		Mesh::AnimKey* Keys;

		SCameraAnimationRes();			
		void Release();				
		bool Load(const char* animFileName);		
	};

public:
	
	Camera3dsMaxPlayer(TrackDescBase* Desc):CameraPlayer(Desc) { owner = NULL; };
	
	virtual void Init();
	void ReInit();
	void Release();		

	FILENAME_PROP(Camera3dsMaxPlayer, m_FileNameProp, "", "*.anim file")

private:

	SCameraAnimationRes m_animationRes;
};