
#pragma once

#ifdef PC
#include <sys/stat.h>
#endif

#include "Common/Common.h"

class DEF_EXPORT BaseResource : public TaskHoldersOwner
{	

#ifdef PC
	struct stat  fileInfo;
#endif

	char fileName[1024];
	int  ref_count;

public:
	
	BaseResource(const char* fl_name);	

	virtual void SetFileName(const char* fl_name) { String::Copy(fileName,1023,fl_name); };
	virtual const char* GetFileName() { return fileName; };

	virtual bool Load() = 0;
	virtual void AddRef()
	{
		ref_count++;
	};

	virtual void DecRef()
	{
		ref_count--;
	
		if (ref_count == 0)
		{
			Release();
		}
	};	

	virtual void CheckModify();	

	virtual void Release();
};

class ResourceManager
{
	bool enabled;
	ThreadLocker locker;
	std::vector<BaseResource*> resources;		

public:

	ResourceManager();
	void Init();
	void Add(BaseResource* res);	
	void Del(BaseResource* res);
	void CheckModify();	
};
