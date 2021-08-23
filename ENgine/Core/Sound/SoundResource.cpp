
#include "Core/Sound/SoundResource.h"
#include "Core/Sound/SoundService.h"
#include "Core/Core.h"

SoundResource::SoundResource(const char* fl_name,SoundService* serv, bool is3d):BaseResource(fl_name)
{	
	is3D = is3d;	

	service = serv;
}

void SoundResource::AddRef(ISound* snd)
{
	service->AddInstance(snd->scene,snd);
	BaseResource::AddRef();

}

void SoundResource::DecRef(ISound* snd)
{
	service->DelInstance(snd->scene,snd);
	BaseResource::DecRef();
}

void SoundResource::AddRef(SoundStream* snd)
{
	//service->AddInstance(snd->scene,snd);
	BaseResource::AddRef();

}

void SoundResource::DecRef(SoundStream* snd)
{
	//service->DelInstance(snd->scene,snd);
	BaseResource::DecRef();
}