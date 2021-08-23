#include "Core/Core.h"

#ifdef OPENAL_ON

#include "Core/Sound/SoundResourceOgg.h"
#include "Core/Sound/SoundResourceWav.h"

#endif

SoundService::SoundService() : ISoundService(), snd_res(_FL_)
{
#ifdef OPENAL_ON
	device = null;
	context = null;
#endif

	Volume = 1.0f;

	music_volume = 100;
	sound_volume = 100;
}

void SoundService::SetMasterVolume(int volume)
{
	Volume = (float)volume / 100.0f;//(int)(volume * float(WndVol/100.0f));			

#ifdef OPENAL_ON
	alListenerf( AL_MAX_GAIN, Volume );
	alListenerf( AL_GAIN, Volume );
#endif
}

void SoundService::SetMusicVolume(int volume)
{
	music_volume = volume;
}

void SoundService::SetSFXVolume(int volume)
{
	sound_volume = volume;
}

int SoundService::GetMasterVolume()
{
	return Volume * 100;
}

int SoundService::GetMusicVolume()
{
	return music_volume;
}

int SoundService::GetSFXVolume()
{
	return sound_volume;
}

#ifdef IOS

void openALInterruptionListener ( void   *inClientData, UInt32 inInterruptionState)
{
    if (inInterruptionState == 1)//kAudioSessionBeginInterruption)
    {
        //alcMakeContextCurrent (NULL);
    }
}

#endif

bool SoundService::Init()
{		
#ifdef OPENAL_ON

    
	device = alcOpenDevice(NULL);
	
	if (!device)
	{
		core->TraceTo("Sound", "Can't OpenAl device");
		return true;
	}
	
	const ALint context_attribs[] = { ALC_FREQUENCY, 22050, 0 };

	context = alcCreateContext(device, null);
	if (!context)
	{
		core->TraceTo("Sound", "Can't Create Context");
		return true;		
	}

	alcMakeContextCurrent(context);
    
    /*AudioSessionInitialize (NULL, NULL, openALInterruptionListener, context);
    
    NSError *activationError = nil;
    [[AVAudioSession sharedInstance] setActive: YES error: &activationError];
    
    
    NSError *setCategoryError = nil;
    
    [[AVAudioSession sharedInstance] setCategory: AVAudioSessionCategoryAmbient error: &setCategoryError];*/
    
#endif

	Volume=100;	   	

	core->AddTask(this,this, (SoundService::Task)&SoundService::Work,lvl_work1, -1.0f);

	return true;
}

ISound* SoundService::CreateSound(void* scene, const char* id, Vector* pos, const char * sourceFile, long line, bool looped, bool autodelete)
{
#ifndef OPENAL_ON
	return null;
#endif

#ifdef OPENAL_ON

	char str[256];
	
	String::Copy(str,256, id);	

	bool is3D = false;
	if (pos)
	{
		String::Cat(str,256, "3D");
		is3D = true;
	}

	SoundResource* s_res = snd_res.Get(str);	

	if (!s_res)
	{
		char ext[16];

		String::ExtractExctention(id, ext, 15);

		if (String::IsEqual(ext, "ogg"))
		{
			s_res = new SoundResourceOgg(id,this, is3D);
		}
		else
		if (String::IsEqual(ext, "wav"))
		{
			s_res = new SoundResourceWav(id,this, is3D);
		}
		else
		{
			return null;
		}


		if (!s_res->Load())
		{
			delete s_res;

			return NULL;
		}

		snd_res.Add(str, s_res);		
	}

	ISound* s = s_res->CreateReference(scene, pos, looped, autodelete, sourceFile, line);	

	return s;
#endif
}

ISoundStream* SoundService::CreateSoundStream(void* scene, const char* id, const char * sourceFile, long line, bool looped, bool autodelete)
{	
#ifndef OPENAL_ON
	return null;
#endif

#ifdef OPENAL_ON

	SoundResource* s_res = snd_res.Get(id);	

	if (!s_res)
	{
		s_res = new SoundResourceOgg(id,this, false);

		/*if (!s_res->Load())
		{
			delete s_res;

			return NULL;
		}*/

		snd_res.Add(id, s_res);
	}

	ISoundStream* s = s_res->CreateReference(scene, looped, autodelete, sourceFile, line);	


	return s;
#endif
}

void SoundService::SetListiner(const Matrix& tranform)
{
	listiner_tr = tranform;
}

void SoundService::Work(float fDelta,int level)
{
#ifdef OPENAL_ON
	for (int j=0;j<scene_sounds.size();j++)
	{
		SceneSounds* sounds = &scene_sounds[j];		

		if (sounds)
		{	
			for (int i=0;i<sounds->sounds.size();i++)
			{		
				if (!sounds->sounds[i]->IsPlaying() && sounds->sounds[i]->IsAutoDelete())
				{
					sounds->sounds[i]->Release();
					i--;
					break;
				}

				sounds->sounds[i]->CalculateVolume(listiner_tr);		
			}
		}
	}
#endif
}

void SoundService::AddInstance(void* scene,ISound* sound)
{
#ifdef OPENAL_ON
	SceneSounds* sounds = GetSceneSounds(scene);

	if (!sounds)
	{
		scene_sounds.push_back(SceneSounds());
		sounds = &scene_sounds[scene_sounds.size()-1];
		sounds->scene = scene;
	}		

	sounds->sounds.push_back(sound);	
#endif
}

void SoundService::DelInstance(void* scene,ISound* sound)
{
#ifdef OPENAL_ON
	SceneSounds* sounds = GetSceneSounds(scene);

	if (sounds)
	{
		for (int i=0;i<sounds->sounds.size();i++)
		{
			if (sounds->sounds[i] == sound)
			{
				for (int j=i;j<sounds->sounds.size()-1;j++)
				{
					sounds->sounds[j] = sounds->sounds[j+1];
				}

				sounds->sounds.pop_back();

				return;
			}
		}
	}	
#endif
}

#ifdef OPENAL_ON
void SoundService::AddInstance(void* scene,SoundStream* stream)
{
	SceneSounds* sounds = GetSceneSounds(scene);

	if (!sounds)
	{
		scene_sounds.push_back(SceneSounds());
		sounds = &scene_sounds[scene_sounds.size()-1];
		sounds->scene = scene;
	}		

	sounds->streams.push_back(stream);	
}

void SoundService::DelInstance(void* scene,SoundStream* stream)
{
	SceneSounds* sounds = GetSceneSounds(scene);

	if (sounds)
	{
		for (int i=0;i<sounds->streams.size();i++)
		{
			if (sounds->streams[i] == stream)
			{
				for (int j=i;j<sounds->streams.size()-1;j++)
				{
					sounds->streams[j] = sounds->streams[j+1];
				}

				sounds->streams.pop_back();

				return;
			}
		}
	}	
}
#endif

void SoundService::Release()
{
#ifdef OPENAL_ON
	for (int j=0;j<scene_sounds.size();j++)
	{
		for (int i=0;i<scene_sounds[j].sounds.size();i++)
		{	#ifdef OPENAL_ON	
			scene_sounds[j].sounds[i]->Release();
			i--;		
		}

		for (int i=0;i<scene_sounds[j].streams.size();i++)
		{		
			scene_sounds[j].streams[i]->Release();
			i--;		
		}
	}
#endif

#ifdef OPENAL_ON
	if (context)
	{
		alcMakeContextCurrent(NULL);
		alcDestroyContext(context);
		context = null;
	}

	if (device)
	{
		alcCloseDevice(device);
		device = null;
	}
#endif
}

void SoundService::ChannelPause(dword channel,bool enable)
{	
}

void SoundService::ChannelSetLooped(dword channel, bool looped)
{	
}

void SoundService::ChannelSetAttributes(dword channel,int volume,int freq)
{
}

void SoundService::ChannelSet3DPosition(dword channel,const Vector& pos, const Vector& vel)
{
}

bool SoundService::ChannelIsPlaying( dword channel )
{
	return false;
}

bool SoundService::ChannelIsActive( dword channel )
{
	return false;
}


void SoundService::ChannelStop( dword channel )
{	
}

SoundService::SceneSounds* SoundService::GetSceneSounds(void* scene)
{
	static SceneSounds* SceneS = NULL;

	SceneS = NULL;

	for (int i=0;i<scene_sounds.size();i++)
	{
		if (scene_sounds[i].scene == scene)
		{
			SceneS = &scene_sounds[i];
		}
	}

	return SceneS;
}