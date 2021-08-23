#pragma once

#include "Core/core.h"

class FxEntity : public SceneEntity
{
	Fx* m_fx;

public:
	
	FILENAME_PROP(FxEntity, propResPath, "", "resPath", "*.fx");	

	ENTITY_DEFC(FxEntity)

	virtual void Init();

	virtual void ReInit();

	virtual void Start();	

	virtual void Work(float dt, int level);
};
