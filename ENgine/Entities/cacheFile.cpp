
#include "cacheFile.h"

#ifdef OSUNIX
#include <dirent.h>
#endif

ENTITY(CacheFile)
ENTITY_PARAMS(CacheFile,"CacheFile","Other", "CacheFile")

void CacheFile::RegisterProperties()
{
	SceneEntity::RegisterProperties();

	PROP_REG( file_name )
	PROP_REG( scan_dir )
}

void CacheFile::Init()
{		
}

void CacheFile::ReInit()
{
#if defined(PC) || defined(OSUNIX)

	if (scan_dir)
	{
		if (file_name[0])
		{
			char local_dir[256];
			String::ExtractPath(file_name.c_str(), local_dir, true);
			String::LowerCase(local_dir);
		
			char dir[256];
			en_core.EN_Files()->GetAppDirectory();
		
			String::Printf(dir, 256, "%s%s", en_core.EN_Files()->GetAppDirectory(), local_dir);
	
			ScanFolder(dir, local_dir);
		}
	}
	else
	{
		IFile* file = core->Files()->OpenFile(file_name.c_str(), IFile::read);
	
		if (file)
		{
			file->Release();		
		}
	}
#endif
}

#if defined(PC) || defined(OSUNIX)

void CacheFile::ScanFolder(const char* path, const char* local_path)
{
#ifdef PC
	char cur_dir[521];
	String::Printf(cur_dir, 512, "%s/*.*", path);

	WIN32_FIND_DATA data;
	HANDLE h = FindFirstFile(cur_dir, &data);

	if (h!=INVALID_HANDLE_VALUE) 
	{
		do
		{
			if (!String::IsEqual(data.cFileName, ".") && !String::IsEqual(data.cFileName, ".."))
			{				
				char file_name[256];
				String::Printf(file_name, 256, "%s/%s", path, data.cFileName);

				char local_file_name[256];
				String::Printf(local_file_name, 256, "%s/%s", local_path, data.cFileName);

				if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{				
					ScanFolder(file_name, local_file_name);
				}
				else
				{										
					IFile* file = core->Files()->OpenFile(local_file_name, IFile::read);

					if (file)
					{
						file->Release();		
					}
				}
			}			
		}

		while(FindNextFile(h,&data));
	}
#endif

#ifdef OSUNIX
	struct dirent *de=NULL;
	DIR *d=NULL;
	
	d=opendir(path);
  
	if(d == NULL)
	{
		return;
	}

	while(de = readdir(d))
	{
		if (!String::IsEqual(".", de->d_name) && !String::IsEqual("..", de->d_name) && !String::IsEqual(".svn", de->d_name))
		{
			char file_name[256];
			String::Printf(file_name, 256, "%s/%s", path, de->d_name);

			char local_file_name[256];
			String::Printf(local_file_name, 256, "%s/%s", local_path, de->d_name);

			if (de->d_type == DT_DIR)
			{				
				ScanFolder(file_name, local_file_name);
			}
			else
			{										
				IFile* file = core->Files()->OpenFile(local_file_name, IFile::read);

				if (file)
				{
					file->Release();		
				}
			}
		}						
	}

	closedir(d);
#endif  
}

#endif 
