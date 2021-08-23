
#pragma once

#include "Core/Resources/Resources.h"
#include "Core/Sound/ISound.h"
#include "Core/Sound/SoundStream.h"

#include "core/Files/FileBuffer.h"

class SoundResource : public BaseResource
{
public:

	bool  is3D;			  

public:	

	SoundService* service;
	
	SoundResource(const char* fl_name,SoundService* serv, bool is3d);
	
	virtual void AddRef(ISound* snd);
	virtual void DecRef(ISound* snd);

	virtual void AddRef(SoundStream* snd);
	virtual void DecRef(SoundStream* snd);		

	virtual ISound* CreateReference(void* scene, Vector* pos, bool looped, bool autodelete, const char* sourceFile, long line) = 0;
	virtual ISoundStream* CreateReference(void* scene, bool looped, bool autodelete, const char* sourceFile, long line) = 0;

	virtual void Release() = 0;
};