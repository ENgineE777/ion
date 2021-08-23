#pragma once
#include "Core/ICore.h"

class AmbientLight : public SceneEntity
{	
public:
		
	PROP_START
	COLOR_PROP(AmbientLight, color, COLOR_WHITE, "Properties", "color")	

	ENTITY_DEFC(AmbientLight)

	virtual void RegisterProperties();
	virtual void Init();
};
