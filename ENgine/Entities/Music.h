#pragma once
#include "Core/Core.h"

class Music : public SceneEntity
{
	float volume;
	ISound* sound;

public:
	
	PROP_START
	FILENAME_PROP(Music, src_name, "", "Sound", "Properties", "*.*")	

	ENTITY_DEFC(Music)
	virtual ~Music();

	virtual void RegisterProperties();
	virtual void Init();
	virtual void Activate(bool set);

	virtual bool Start();	
	virtual void Finish();

	virtual void Work(float dt, int level);
};
