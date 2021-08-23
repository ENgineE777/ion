
#include "FontEntity.h"

#include "FontGlyph.h"

ENTITY(FontEntity)
ENTITY_PARAMS(FontEntity,"PointLight","Lighting", "Point Light")



void FontEntity::Init()
{	
	PROP_REG( radius )
	PROP_REG( color )
	PROP_REG( power )		
}