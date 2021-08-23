#pragma once

#include "Core/core.h"

class TimeBomb : public SceneEntity
{
public:
	
	FLOAT_PROP(TimeBomb, power, 5.0f, "Power")
	FLOAT_PROP(TimeBomb, radius, 0.5f, "Radius")
	FLOAT_PROP(TimeBomb, delayTime, 0.0f, "Delay")
	FILENAME_PROP(TimeBomb, file_name, "", "file", "*.*")	

	ENTITY_DEFC(TimeBomb)

	float time_to_blow;
	bool  blowed;

	TextureInstance* tex;

	virtual void Init();
	virtual void ReInit();
	virtual void Start();
	virtual void Finish();
	virtual void Work(float dt, int level);	
};
