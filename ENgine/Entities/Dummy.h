#pragma once
#include "Core/Core.h"

class Dummy : public SceneEntity
{
	SceneEntity* attach;
	Matrix relative_matrix;

public:
	
	PROP_START
	STRING_PROP(Dummy, attach_name, "", "Properties", "attach")	

	ENTITY_DEFC(Dummy)

	virtual void RegisterProperties();
	virtual void Init();

	virtual bool Start();	

	virtual void Work(float dt, int level);
};
