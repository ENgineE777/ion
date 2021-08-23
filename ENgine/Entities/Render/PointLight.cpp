
#include "PointLight.h"

ENTITY(PointLight)
ENTITY_PARAMS(PointLight,"PointLight","Lighting", "Point Light")

void PointLight::RegisterProperties()
{
	SceneEntity::RegisterProperties();

	PROP_REG( radius )
	PROP_REG( color )
	PROP_REG( power )	
}

void PointLight::Init()
{		
	if (Scene()) Scene()->RegisterEntityInGroup(this, "PointLight");
}