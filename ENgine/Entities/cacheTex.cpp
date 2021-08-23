
#include "cacheTex.h"

ENTITY(CacheTex)
ENTITY_PARAMS(CacheTex,"CacheTex","Other", "CacheTex")

void CacheTex::RegisterProperties()
{
	SceneEntity::RegisterProperties();

	PROP_REG( file_name )		
}

void CacheTex::Init()
{		
}

void CacheTex::ReInit()
{
#if defined(PC) || defined(OSUNIX)
	TextureInstance* tex = en_core.Render()->LoadTexture(file_name.c_str(), false, _FL_);	
	
	if (tex)
	{
        tex->Release();
	}
#endif
}