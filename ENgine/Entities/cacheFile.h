#pragma once
#include "Core/Core.h"

class CacheFile : public SceneEntity
{
public:
	
	PROP_START
	FILENAME_PROP(CacheFile, file_name, "", "Properties", "file", "*.*")
	BOOL_PROP(CacheFile, scan_dir, false, "Properties", "scan_dir")

	ENTITY_DEFC(CacheFile)

	virtual void RegisterProperties();
	virtual void Init();

#if defined(PC) || defined(OSUNIX)
	void ScanFolder(const char* path, const char* local_path);
#endif

	virtual void ReInit();	
};
