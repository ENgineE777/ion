
#pragma once

#include "Core/Sound/ISound.h"
#include "Core/Sound/ISoundStream.h"
#include "Common/TaskHolder.h"

class ISoundService : public TaskHoldersOwner
{
public:	
	
	ISoundService() : TaskHoldersOwner() {};
	virtual ISound* CreateSound(void* scene, const char* id, Vector* pos, const char* sourceFile, long line, bool looped = false, bool autodelete = true) = 0;	
	virtual ISoundStream* CreateSoundStream(void* scene, const char* id, const char* sourceFile, long line, bool looped = false, bool autodelete = true) = 0;
	virtual void SetListiner(const Matrix& tranform) = 0;	
	
	virtual void SetMasterVolume(int volume) = 0;
	virtual void SetMusicVolume(int volume) = 0;
	virtual void SetSFXVolume(int volume) = 0;
};