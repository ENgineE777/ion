
#include "Resources.h"
#include "Core/Core.h"

ResourceManager* res_manager = null;

BaseResource::BaseResource(const char* fl_name)
{
	String::Copy(fileName,1023,fl_name);
	ref_count = 0;

#ifdef PC
    if(fileName[0])
	    stat(fileName,&fileInfo);	
#endif

	if (res_manager && fileName[0]) res_manager->Add(this);
}

void BaseResource::CheckModify()
{
#ifdef PC
	struct stat flInfo;

	if(!stat(fileName,&flInfo))
	{        
		if (fileInfo.st_mtime != flInfo.st_mtime ||
			fileInfo.st_atime != flInfo.st_atime ||
			fileInfo.st_ctime != flInfo.st_ctime)
		{
			Load();
			stat(fileName,&fileInfo);
		}			
	}
#endif
}

void BaseResource::Release()
{
	if (res_manager) res_manager->Del(this);
}

ResourceManager::ResourceManager()
{
	res_manager = this;
	enabled = false;
	locker.Init();
}

void ResourceManager::Init()
{
#ifdef PC
	enabled = ReadINI("settings.ini", 1, "Editor", "scan_res");
#endif
}

void ResourceManager::Add(BaseResource* res)
{
	locker.Lock();
	
	resources.push_back(res);
	
	locker.UnLock();
}

void ResourceManager::Del(BaseResource* res)
{
	locker.Lock();

	for (int j=0;j<resources.size();j++)
	{			
		if (resources[j] == res)
		{
			resources.erase(resources.begin() + j);
			locker.UnLock();

			return;
		}		
	}

	locker.UnLock();
}

void ResourceManager::CheckModify()
{	
#ifdef PC
	if (!enabled)
	{
		return;
	}
		
	if (resources.size() == 0) return;

	locker.Lock();

	static int counter = 0;

	int index = 0;

	while (index<5)
	{
		if (counter >= resources.size())
		{		
			counter = 0;
		}

		resources[counter]->CheckModify();		

		index++;
		counter++;
	}

	locker.UnLock();

#endif
}

