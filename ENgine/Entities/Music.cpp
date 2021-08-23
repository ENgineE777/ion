
#include "Music.h"

ENTITY(Music)
ENTITY_PARAMS(Music,"Music","Sound", "Music")

void Music::RegisterProperties()
{
	SceneEntity::RegisterProperties();

	PROP_REG(src_name)	
}

void Music::Init()
{	
	SetUpdate((TaskHolder::Task)&Music::Work, lvl_work1);

	volume = 0.0f;
	sound = null;
}

Music::~Music()
{
	if (sound)
	{
		sound->Release();
	}
}

void Music::Activate(bool set)
{
	SceneEntity::Activate(set);

	if (!sound && set)
	{
		Vector ps = 0.0f;
		sound = en_core.Sound()->CreateSound(Scene(), src_name.c_str(), &ps, _FL_, true, false);	
	}
	
	if (set && sound)
	{
		if (en_core.Sound()->GetMusicVolume() > 2)
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

bool Music::Start()
{
	if (sound)
	{
		sound->Pause(false);
	}

	return true;
}

void Music::Finish()
{
	if (sound)
	{
		sound->Pause(true);
	}
}

void Music::Work(float dt, int level)
{
	if (Scene()->IsRunning())
	{
		if (sound)
		{
			if (sound->IsPlaying())
			{
				if (en_core.Sound()->GetMusicVolume() < 2)
				{
					sound->Pause(true);
				}
			}
			else
			{
				if (en_core.Sound()->GetMusicVolume() > 2)
				{
					sound->Pause(false);
				}
			}
		}		
	}	
}