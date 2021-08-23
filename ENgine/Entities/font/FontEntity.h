#pragma once
#include "Core/ICore.h"

class FontEntity : public SceneEntity
{	
public:
		
	FLOAT_PROP(FontEntity, radius, 5.0f, "radius")
	COLOR_PROP(FontEntity, color, COLOR_WHITE, "color")	
	FLOAT_PROP(FontEntity, power, 1.0f, "power")

	ENTITY_DEFC(FontEntity)

	virtual void Init();
};
