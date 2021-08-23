#pragma once
#include "Core/Core.h"

class CacheTex : public SceneEntity
{
public:
	
	PROP_START
	FILENAME_PROP(CacheTex, file_name, "", "Properties", "file", "*.*")	

	ENTITY_DEFC(CacheTex)

	virtual void RegisterProperties();
	virtual void Init();
	virtual void ReInit();	
};
