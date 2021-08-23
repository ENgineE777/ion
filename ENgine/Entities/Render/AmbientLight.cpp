
#include "AmbientLight.h"

ENTITY(AmbientLight)
ENTITY_PARAMS(AmbientLight,"AmbientLight","Lighting", "Ambient Light")

void AmbientLight::RegisterProperties()
{
	SceneEntity::RegisterProperties();

	PROP_REG( color )	
}

void AmbientLight::Init()
{		
	if (Scene()) Scene()->RegisterEntityInGroup(this, "AmbientLight");
}