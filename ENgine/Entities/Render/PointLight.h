#pragma once
#include "Core/ICore.h"

class PointLight : public SceneEntity
{	
public:
		
	PROP_START

	FLOAT_PROP(PointLight, radius, 5.0f, "Properties", "radius")
	COLOR_PROP(PointLight, color, COLOR_WHITE, "Properties", "color")	
	FLOAT_PROP(PointLight, power, 1.0f, "Properties", "power")

	ENTITY_DEFC(PointLight)

	virtual void RegisterProperties();
	virtual void Init();
};
