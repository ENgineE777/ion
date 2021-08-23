
#pragma once

#include "Core/Sound/SoundResource.h"
#include "Core/Sound/SoundWav.h"
#include "Core/Sound/SoundStream.h"

#include "WavLoader.h"

#ifdef ANDROID

#include <AL/al.h>
#include <AL/alc.h>

#endif

#ifdef IOS

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#endif

#include "core/Files/FileBuffer.h"

class SoundResourceWav : public SoundResource
{
public:	
		
	unsigned long ulFrequency;
	unsigned long ulFormat;
	
	WavLoader wav_loader;   

public:	
		
	SoundResourceWav(const char* fl_name,SoundService* serv, bool is3d);

	virtual bool Load();	
	virtual void Release();
	
	ISound* CreateReference(void* scene, Vector* pos, bool looped, bool autodelete, const char* sourceFile, long line);
	ISoundStream* CreateReference(void* scene, bool looped, bool autodelete, const char* sourceFile, long line);

	int  Decode(int id, char* out_buffer, int buffer_len);
	void Reset(int id);
};