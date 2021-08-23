#pragma once
#include "Core/Core.h"

class DirLight : public SceneEntity
{	
public:
		
	PROP_START
	COLOR_PROP(DirLight, color, COLOR_WHITE, "Properties", "color")	
	FLOAT_PROP(DirLight, power, 1.0f, "Properties", "power")	

	ENTITY_DEFC(DirLight)

	virtual void RegisterProperties();
	virtual void Init();
};
