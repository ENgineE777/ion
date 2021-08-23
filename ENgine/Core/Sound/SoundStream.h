
#pragma once

#include "Common/Common.h"
#include "Core/Sound/ISoundStream.h"
#include "Core/Files/IFileBuffer.h"

class SoundResource;
class SoundService;

class DEF_EXPORT SoundStream : public ISoundStream
{	
	dword stream;

	IFileBuffer* fl;

	bool autodelete;	
	bool looped;	

	int   volume;

	char cppFile[1024];	
	long cppLine;

public:

	void* scene;

	SoundResource* res;
	SoundService*  service;

	void Init(void* scn,SoundResource* rs, bool looped, bool autodelete, const char* sourceFile, long line);

	virtual bool IsPlaying();
	virtual void Pause(bool enable);

	virtual void AutoDelete(bool enable);
	virtual bool IsAutoDelete();

	virtual void SetLooped(bool loop);
	virtual void SetAttributes(int volume,int freq = -1);	

	virtual void Release();

	virtual bool IsActive();	
};