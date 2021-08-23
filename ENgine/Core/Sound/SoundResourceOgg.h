
#pragma once

#include "Core/Sound/SoundResource.h"

#ifdef PC

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#endif

#ifdef ANDROID

#include <AL/al.h>
#include <AL/alc.h>
#include <ivorbisfile.h>

#endif

#ifdef IOS

#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include "ivorbisfile.h"

#endif

#include "core/Files/FileBuffer.h"

class SoundResourceOgg : public SoundResource
{
public:

	IFileBuffer* fbuffer;		
	
	SoundResourceOgg(const char* fl_name, SoundService* serv, bool is3d);

	virtual bool Load();	
	virtual void Release();	

	virtual ISound* CreateReference(void* scene, Vector* pos, bool looped, bool autodelete, const char* sourceFile, long line);
	virtual ISoundStream* CreateReference(void* scene, bool looped, bool autodelete, const char* sourceFile, long line);

	int Decode(char* out_buffer, int buffer_len, OggVorbis_File* vf);
};