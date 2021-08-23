
#pragma once

#include "Core/Sound/ISoundService.h"

#ifdef OPENAL_ON

#include "Core/Sound/SoundStream.h"
#include "Common/Containers/HashMap.h"

#endif

class SoundResource;

//#define OPENAL_ON

#ifdef PC

#ifdef OPENAL_ON

#include <al.h>
#include <alc.h>

#endif

#endif

#ifdef OPENAL_ON

#ifdef ANDROID

#include <AL/al.h>
#include <AL/alc.h>

#endif

#ifdef IOS

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#endif

#endif

class SoundService : public ISoundService
{
#ifdef OPENAL_ON
	ALCdevice* device;
	ALCcontext* context;
#endif
	
	float Volume;
	dword WndVol;

	int music_volume;
	int sound_volume;

	struct SceneSounds
	{
		void* scene;
		vector<ISound*> sounds;
		
#ifdef OPENAL_ON
		vector<SoundStream*> streams;
#endif
	};
	
	vector<SceneSounds> scene_sounds;
	HashMap<SoundResource> snd_res;

	Matrix listiner_tr;

public:	

	SoundService();	

	virtual bool Init();	

	virtual ISound* CreateSound(void* scene, const char* id, Vector* pos, const char* sourceFile, long line, bool looped = false, bool autodelete = true);
	virtual ISoundStream* CreateSoundStream(void* scene, const char* id, const char* sourceFile, long line, bool looped = false, bool autodelete = true);

	virtual void SetListiner(const Matrix& tranform);	

	virtual void Work(float fDelta,int level);	

	void SetMasterVolume(int volume);
	void SetMusicVolume(int volume);
	void SetSFXVolume(int volume);

	int  GetMasterVolume();
	int  GetMusicVolume();
	int  GetSFXVolume();

	void AddInstance(void* scene,ISound* sound);
	void DelInstance(void* scene,ISound* sound);	

#ifdef OPENAL_ON
	void AddInstance(void* scene,SoundStream* sound);
	void DelInstance(void* scene,SoundStream* sound);	
#endif		
	
	void  ChannelPause(dword channel,bool enable);	
	void  ChannelSetLooped(dword channel, bool looped);
	void  ChannelSetAttributes(dword channel,int volume,int freq = -1);
	void  ChannelSet3DPosition(dword channel,const Vector& pos, const Vector& vel);
	bool  ChannelIsPlaying( dword channel );
	bool  ChannelIsActive( dword channel );
	void  ChannelStop( dword channel );

	virtual void Release();	

	SceneSounds* GetSceneSounds(void* scene);	
};