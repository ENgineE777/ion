#pragma once
#include "Core/Core.h"

class SoundEntity : public SceneEntity
{
	float volume;
	ISound* sound;

public:
	
	PROP_START
	FILENAME_PROP(SoundEntity, src_name, "", "Properties", "Source", "*.*")	
	BOOL_PROP(SoundEntity, looped, false, "Properties", "Looped")

	ENTITY_DEFC(SoundEntity)
	virtual ~SoundEntity();

	virtual void RegisterProperties();
	virtual void Init();

	virtual void Activate(bool set);
	virtual void Finish();

	virtual void Work(float dt, int level);
};
