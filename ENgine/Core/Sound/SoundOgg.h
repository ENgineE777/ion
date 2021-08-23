
#pragma once

#include "Core/Sound/ISound.h"
#include "Common/common.h"

class SoundResourceOgg;
class SoundService;

#ifdef PC

#include <al.h>
#include <alc.h>

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#endif

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


class DEF_EXPORT SoundOgg : public ISound
{		
	ALuint source;
	ALuint buffers[4];	

	bool autodelete;	
	bool looped;

	Vector pos;
	Vector vel;

	float min_d,max_d;
	int   out_volume;

	int   volume;

	char cppFile[1024];	
	long cppLine;

	bool is3D;

	bool playing;

	ALenum format;
	ALsizei freq;	

	OggVorbis_File vf; 

public:

	struct MemoryStream
	{
		unsigned char* buffer;
		size_t size;
		size_t offset;
	};

	MemoryStream stream;
	char temp_buffer[8 * 1024];

	void* scene;

	SoundResourceOgg* res;
	SoundService*  service;

	void Init(void* scn, SoundResourceOgg* rs, Vector* pos, bool looped, bool autodelete, byte* buffer, int buffer_len, const char* sourceFile, long line);

	virtual bool IsPlaying();
	virtual void Pause(bool enable);

	virtual void AutoDelete(bool enable);
	virtual bool IsAutoDelete();

	virtual void SetLooped(bool loop);
	virtual void SetAttributes(int volume,int freq = -1);
	virtual void Set3DAttributes(float min,float max,int outvol);
	virtual void SetPosition(const Vector& pos, const Vector& vel);

	virtual void Release();

	virtual bool IsActive();

	void PrepareBuffers();

	virtual void CalculateVolume(const Matrix& listiner_tr);
};