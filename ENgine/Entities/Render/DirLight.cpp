
#include "DirLight.h"

ENTITY(DirLight)
ENTITY_PARAMS(DirLight,"DirLight","Lighting", "Directional Light")

void DirLight::RegisterProperties()
{
	SceneEntity::RegisterProperties();

	PROP_REG( color )	
	PROP_REG( power )	
}

void DirLight::Init()
{			
	if (Scene()) Scene()->RegisterEntityInGroup(this, "DirLight");
}