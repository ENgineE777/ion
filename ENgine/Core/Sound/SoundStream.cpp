
#include "Core/Sound/SoundStream.h"
#include "Core/Sound/SoundResource.h"
#include "Core/Sound/SoundService.h"
#include "Core/Core.h"

void SoundStream::Init(void* scn,SoundResource* rs, bool set_looped, bool set_autodelete, const char* sourceFile, long line)
{	
	strcpy(cppFile,sourceFile);	
	cppLine = line;

	scene = scn;

	res = rs;

	service = res->service;

	fl = core->Files()->LoadFile(res->GetFileName());

	if (fl)
	{
		//stream=service->LoadStream(fl->GetBuffer(),fl->GetSize());		
	}
	
	AutoDelete(set_autodelete);	
	SetLooped(set_looped);
	
	volume = 100;
}

bool SoundStream::IsPlaying()
{
	if (!stream) return false;	

	return service->ChannelIsPlaying( stream );
}

void SoundStream::Pause(bool enable)
{
	service->ChannelPause(stream,enable);	
}

void SoundStream::AutoDelete(bool enable)
{
	autodelete = enable;
}

bool SoundStream::IsAutoDelete()
{
	return autodelete;
}

void SoundStream::SetLooped(bool loop)
{
	looped = loop;

	service->ChannelSetLooped(stream, looped);	
}


void SoundStream::SetAttributes(int set_volume,int freq)
{
	volume = set_volume;
	service->ChannelSetAttributes(stream,volume,freq);
}

void SoundStream::Release()
{
	if (fl)
	{
		fl->Release();
		fl = NULL;
	}		
		
	service->ChannelStop(stream);
	res->DecRef(this);
	delete this;
}

bool SoundStream::IsActive()
{
	if (!stream) return false;	

	return service->ChannelIsActive( stream );
}