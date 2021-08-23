
#pragma once

#include "Core/Sound/ISound.h"
#include "Common/common.h"

class SoundResourceWav;
class SoundService;

#ifdef PC

#include <al.h>
#include <alc.h>

#endif

#ifdef ANDROID

#include <AL/al.h>
#include <AL/alc.h>

#endif

#ifdef IOS

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#endif

class DEF_EXPORT SoundWav : public ISound
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
	
	int  buffer_size;	
	char buffer[32 * 1024];

public:

	int  wave_id;
	void* scene;

	SoundResourceWav* res;
	SoundService*  service;

	void Init(void* scn,SoundResourceWav* rs, Vector* pos, bool looped, bool autodelete, int set_buffer_size, int id, const char* sourceFile, long line);

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