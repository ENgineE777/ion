
#include "SoundEntity.h"

ENTITY(SoundEntity)
ENTITY_PARAMS(SoundEntity,"SoundEntity","Sound", "Sound")

void SoundEntity::RegisterProperties()
{
	SceneEntity::RegisterProperties();

	PROP_REG(src_name)	
	PROP_REG(looped)
}

void SoundEntity::Init()
{			
	SetUpdate((TaskHolder::Task)&SoundEntity::Work, lvl_work1);

	volume = 0.0f;
	sound = null;
}

SoundEntity::~SoundEntity()
{
	if (sound)
	{
        sound->Release();
	}
}

void SoundEntity::Activate(bool set)
{
	SceneEntity::Activate(set);

	if (!sound && set)
	{
		if (en_core.Sound()->GetSFXVolume() > 2 && !looped)
		{
			Vector ps = 0.0f;
			sound = en_core.Sound()->CreateSound(Scene(), src_name.c_str(), &ps, _FL_, looped, false);
		}
	}
	
	if (set && sound)
	{
		if (en_core.Sound()->GetSFXVolume() > 2)
		{
			if (!sound->IsPlaying()) sound->Pause(false);
		}
	}
	else
	if (!set && sound)
	{
		RELEASE(sound)
	}
}

void SoundEntity::Finish()
{
	RELEASE(sound)	
}

void SoundEntity::Work(float dt, int level)
{
	if (Scene()->IsRunning())
	{
		if (sound)
		{
			if (sound->IsPlaying())
			{
				if (en_core.Sound()->GetSFXVolume() < 2)
				{
					if (looped)
					{
						sound->Pause(true);
					}
					else
					{
						sound->Release();
					}

				}
			}
			else
			{
				if (en_core.Sound()->GetSFXVolume() > 2)
				{
					sound->Pause(false);
				}
			}
		}
	}	
}