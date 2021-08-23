#include "Core/Core.h"

#include "StreamLoader.h"
#include "StreamSaver.h"
#include "File.h"
#include "FileBuffer.h"
#include "XMLFile.h"
#include "JSONFile.h"

#ifdef PC
#include <windows.h>
#include "shlobj.h"
#endif

#ifdef OSUNIX

#include <unistd.h>

#endif

#ifdef ANDROID
#include "Common/libzip/zip.h"
zip* zip_arch = null;
#endif

dword FileService::cachedMagicID = MAKEFOURCC('C', 'F', '0', '3');
dword FileService::packMagicID = MAKEFOURCC('P', 'K', '0', '1');

FileService::FileService() : resource_packs(_FL_)
#if defined(PC) || defined(OSUNIX)
, pools(_FL_)
#endif
#if defined(PC)
,  cached_file_names(_FL_)
#endif
{
#if defined(PC) || defined(OSUNIX)
	load_from_res = false;
	active_pool = null; 
	saved_pool = null;
	converterProgress = 0;
	post_converter = 0;
#endif

#ifdef ANDROID
	apk_name[0] = 0;
	load_from_apk = false;
	String::Copy(base_path, 256, "/sdcard/android/data/ion/");
#endif
}

bool FileService::Init()
{	
#if defined(PC) || defined(OSUNIX)

	GetCurDirectory(appPath, 1023);
	String::Cat(appPath,1023,"/");
	
#endif
		   
#ifdef ANDROID
	core->TraceTo("Files", "apk_name - %s", apk_name);

	int err = 0;
	if (load_from_apk && !zip_arch)
	{
		zip_arch = zip_open(apk_name, 0, &err);
		core->TraceTo("Files", "zip_open, er - %i", err);
	}
	
#endif	

#ifdef PC
	
	TCHAR windir[MAX_PATH];
	GetWindowsDirectory(windir, MAX_PATH);		

	/*XMLFile* xml = OpenXMLFile("files.xml","res");

	if (xml)
	{
		
		xml->ReadString("path", res_path);
		xml->Release();

		core->Trace("Path to MediaData set to - %s", res_path);
	}
	else
	{
		res_path[0] = 0;
		
		core->Trace("Warring, path to MediaData not set!!!");
	}*/

	if (!load_from_res)
	{
		/*FILE * file = fopen("files.txt", "w");
		if(file)
		{		
			fwrite("\n", 1, 1, file);
			fclose(file);
		}*/
	}

#endif

	return true;
}

IFileBuffer* FileService::LoadFile(const char* file_name)
{
	if (!file_name[0]) return null;

	FileBuffer* file = new FileBuffer();

	if (file->Init(file_name))
	{
		return file;
	}

	file->Release();
	return null;
}

IFile* FileService::OpenFile(const char* file_name, IFile::mode_type mode)
{
	if (!file_name[0]) return null;

#if defined(PC) || defined (OSUNIX)
	char correct_name[512];

	/*char correct_name[512];
	if (!String::ParsePlatform(file_name, correct_name, 512, core->GetCurrentPlatform()))
	{
		String::Copy(correct_name, 512, file_name);
	}
	String::LowerCase(correct_name);*/

	String::Copy(correct_name, 512, file_name);
			
#ifdef OSUNIX
	if ((mode == IFile::read) &&
		(correct_name[0] != '/' || correct_name[1] != 'h' || correct_name[2] != 'o' ||
		 correct_name[3] != 'm' || correct_name[4] != 'e' || correct_name[5] != '/' ||  correct_name[6] != 'u'))
#endif
	{
		String::LowerCase(correct_name);
	}

#else
	char correct_name[512];
	
	if (!String::ParsePlatform(file_name, correct_name, 512, core->GetCurrentPlatform()))
	{
		String::Copy(correct_name, 512, file_name);
	}
    
    if (correct_name[0] != '/' && correct_name[1] != 'v')
    {
        String::LowerCase(correct_name);
    }
#endif
			
	File* file = new File();


#ifndef PC

#ifdef ANDROID
	//if (mode == File::write || IsLoadFromApk())
#else
	//if (mode == File::write)
#endif

#else
	//if (mode == File::write)
#endif
	{		
		if (file->Init(correct_name,mode))
		{
			return file;
		}		
	}

	if (mode == File::read)
	{				
		for (int i=0; i<resource_packs.Size();i++)
		{
			ResourcePack* pack = resource_packs.Get(i);			

			int hash = HashValue(correct_name);

			for (int j=0;j<pack->num_records;j++)
			{				
				if (pack->records[j].hash == hash)				
				{										
					if (file->Init(resource_packs.GetName(i), pack->records[j].offset, pack->records[j].size, File::read))
					{
						return file;
					}
				}
			}
		}
	}	
	
#ifdef ANDROID
	if (file->Init(correct_name,mode))
	{
		return file;
	}			
#endif

	file->Release();

	core->TraceTo("Files", "Can't open \"%s\"", correct_name);

	return null;
}

IStreamLoader* FileService::CreateStreamLoader(const char* file_name, const char* enterBlock, IStreamLoader::FileType type,bool bMemory)
{
	StreamLoader* loader = new StreamLoader();

	if (loader->Init(file_name,enterBlock,type))
	{
		return loader;
	}

	loader->Release();
	return null;
}

IStreamSaver* FileService::CreateStreamSaver(const char* file_name, StreamSaver::FileType type)
{
	StreamSaver* saver = new StreamSaver();

	if (saver->Init(file_name,type))
	{
		return saver;
	}

	saver->Release();
	return null;
}

IDataFile* FileService::OpenXMLFile(const char* file_name, const char* enterBlock,bool bMemory)
{
	XMLFile* file = new XMLFile();
	if (file->Init(file_name, enterBlock,bMemory))
		return file;
	file->Release();
	return null;
}

IDataFile* FileService::OpenJSONFile(const char* file_name,bool bMemory)
{
	JSONFile* file = new JSONFile();
	if (file->Init(file_name,bMemory))
		return file;
	file->Release();
	return null;
}

void FileService::ActivateResourcePack(const char* pack_path, bool activate)
{
	char pack_name[128];
	String::ExtractFileName(pack_path, pack_name);
	
	ResourcePack* pack = resource_packs.Get(pack_name);

	if (activate && !pack)
	{
#ifdef PC
		Pool* prev_active_pool = active_pool;		
		active_pool = null;
#endif

		File* file = new File();

		if (file->Init(pack_path, File::read))
		{			
			dword magic_id;
			file->Read(&magic_id, sizeof(magic_id));

			if (magic_id != packMagicID)
			{
				core->TraceTo("Files", "ERROR: %s are not valid resource pack", pack_path);				
			}
			else
			{
				pack = resource_packs.Add(pack_name);

				file->Read(&pack->num_records, sizeof(pack->num_records));

				pack->records = new FileRecord[pack->num_records];
	
				file->Read(pack->records, sizeof(FileRecord) * pack->num_records);

				core->TraceTo("Files", "%s was mounted with %i files", pack_path, pack->num_records);			
			}

			file->Release();
		}	

#ifdef PC
		active_pool = prev_active_pool;
#endif
	}
	else
	if (!activate && pack)
	{
		delete[] pack->records;

		resource_packs.Delete(pack);
	}
}

#if defined(PC) || defined(OSUNIX)

void FileService::SetConverterProgress(ConverterProgress set_converterProgress)
{
	converterProgress = set_converterProgress;
}

void FileService::RegisterPostConverter(Converter converter)
{
	post_converter = converter;
}

void FileService::RegisterConverter(Platform platform, const char* ext, Converter converter)
{
	PlatformConverters* pl_converter = null;

	for (int i=0;i<platform_converters.size();i++)
	{
		if (platform_converters[i].platform == platform)
		{
			pl_converter = &platform_converters[i];
		}
	}

	if (!pl_converter)
	{
		platform_converters.push_back(PlatformConverters());

		pl_converter = &platform_converters[platform_converters.size()-1];

		pl_converter->platform = platform;
	}

	pl_converter->converters.push_back(ExtConvertor());

	ExtConvertor* ext_cnv = &pl_converter->converters[pl_converter->converters.size()-1];

	String::Copy(ext_cnv->ext, 8, ext);
	ext_cnv->converter = converter;
}

void FileService::ApplyConverter(Platform platform, const char* in, const char* out, bool fake)
{
	char ext[8];
	String::ExtractExctention(out, ext, 7);

	char full_out[1024];
	sprintf(full_out, "%s%s", appPath, out);

	if (!fake)
	{
		for (int i=0;i<platform_converters.size();i++)
		{
			if (platform_converters[i].platform == platform)
			{
				for (int j=0;j<platform_converters[i].converters.size();j++)
				{
					if (String::IsEqual(platform_converters[i].converters[j].ext, ext))
					{					
						platform_converters[i].converters[j].converter(in, full_out, ext, platform);
					
						break;
					}
				}

				break;
			}
		}
	}

	if (post_converter) post_converter(in, full_out, ext, platform);
}

void FileService::RegFile(const char* path)
{	
	if (!active_pool) return;
	
	char tmp[512];
	String::Copy(tmp, 512, path);
	String::LowerCase(tmp);
	String::FixSlashes(tmp);	

	if (!active_pool->file_names.Get(tmp))
	{		
		active_pool->file_names.Add(tmp);
	}
}

void FileService::RelativePath(char* path)
{
	int res_len = strlen(appPath);
	int len = strlen(path) - res_len;

	int i=0;

	for (i=0;i<len;i++)
	{
		path[i] = path[i+res_len];
	}

	path[i] = 0;
}

void FileService::ReportFileError(const char * fileName)
{
	/*FILE * file = fopen("files.txt", "a");
	if(file)
	{
		fseek(file, 0, SEEK_END);

		char buf[1024];
		String::Printf(buf, 1024, "Can't find file - \"%s\"", fileName);
		
		long len = strlen(buf);
		fwrite(buf, len, 1, file);
		fwrite("\n", 1, 1, file);
		fclose(file);
	}*/
}

void FileService::AddPool(const char* name)
{
	Pool* pool = pools.Get(name);

	if (!pool)
	{
		pools.Add(name);
	}
}

void FileService::ClearPool(const char* name)
{
	Pool* pool = pools.Get(name);

	if (pool)
	{
		pool->file_names.Clear();
	}
}

void FileService::ActivatePool(const char* name)
{
	active_pool = pools.Get(name);
}

void FileService::SavePool(const char* name, const char* path, Platform platform)
{
	saved_pool = pools.Get(name);

	if (!saved_pool)
	{
		return;
	}

	int counter;

	class Saver: public ThreadWorker
	{
		public:

		int from, to;
		FileService* fs;
		Platform platform;
		const char* path;
		int counter;
		virtual void Prepare()
		{
			counter = 0;
		};
		
		virtual void Loop()
		{		
			for (int i=from; i<to;i++)
			{		
				//if (fs->converterProgress) fs->converterProgress(fs->file_names.GetName(i), i, fs->file_names.Size());				
					
				char correct_path[1024];				

				if (strstr(fs->saved_pool->file_names.GetName(i), "comment_icon.png"))
				{
					DEBUG_BREAK;
				}

				if (!String::ParsePlatform(fs->saved_pool->file_names.GetName(i), correct_path, 1024, platform))
				{
					String::Copy(correct_path,1023, fs->saved_pool->file_names.GetName(i));
				}
				
				bool skip = false;
				
#ifdef PC	
				struct stat* st = fs->cached_file_names.Get(fs->saved_pool->file_names.GetName(i));				
				
				char cached_out[1024];
				String::Copy(cached_out,1023, fs->cache_dir);
				String::Cat(cached_out,1023, correct_path);				

				struct stat cur_st;
				if (!stat(correct_path, &cur_st))
				{
					if (st)
					{
						if (st->st_mtime == cur_st.st_mtime &&
							st->st_atime == cur_st.st_atime &&
							st->st_ctime == cur_st.st_ctime)
						{
							skip = true;
						}
					}
					else
					{
						struct stat* st = fs->cached_file_names.Add(fs->saved_pool->file_names.GetName(i));

						st->st_mtime = cur_st.st_mtime;
						st->st_atime = cur_st.st_atime;
						st->st_ctime = cur_st.st_ctime;
					}
				}
#endif

				char in[1024];
				String::Copy(in,1023, correct_path);		
		
				char out[1024];
				String::Copy(out,1023, path);
				String::Cat(out,1023, correct_path);		

				if (!skip)
				{
					CopyFileEX(in, out);
					fs->ApplyConverter(platform, in, out, false);

#ifdef PC
					CopyFileEX(out, cached_out);
#endif
				}
				else
				{
#ifdef PC					
					CopyFileEX(cached_out, out);
#endif
					fs->ApplyConverter(platform, in, out, true);
				}
													
				counter++;
			}
		};
	};
	
	Saver saver[16];

#ifdef PC
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );

	int num_threads = sysinfo.dwNumberOfProcessors;
	num_threads = 1;
#endif

#ifdef OSUNIX
	int num_threads = 1;
#endif

	int gap = (int)(saved_pool->file_names.Size() / num_threads);

	for (int i=0;i<num_threads;i++)
	{
		saver[i].from = gap * i;
		
		saver[i].to = gap * (i+1);
		if (i == num_threads - 1) saver[i].to = saved_pool->file_names.Size();

		saver[i].platform = platform;
		saver[i].fs = this;
		saver[i].path = path;	
		saver[i].Start(ThreadWorker::normal);
	}

	bool running = true;

	while (running)
	{
		int counter = 0;

		for (int i=0;i<num_threads;i++)
		{
			counter += saver[i].counter;
		}
		
		if (converterProgress) converterProgress("", counter, saved_pool->file_names.Size());
		ThreadWorker::Sleep(100);

		running = false;

		for (int i=0;i<num_threads;i++)
		{
			if (saver[i].IsWorking())
			{
				running = true;
				break;
			}
		}
		
	}

	/*for (int i=0; i<file_names.Size();i++)
	{		
		if (converterProgress) converterProgress(file_names.GetName(i), i, file_names.Size());

		char correct_path[1024];
		if (!String::ParsePlatform(file_names.GetName(i), correct_path, 1024, platform))
		{
			String::Copy(correct_path,1023, file_names.GetName(i));
		}

		char in[1024];
		String::Copy(in,1023, correct_path);		
		
		char out[1024];
		String::Copy(out,1023, path);
		String::Cat(out,1023, correct_path);		

		CopyFileEX(in, out);
		ApplyConverter(platform, in, out);		
	}*/
}

void FileService::LoadCached(const char * fileName)
{
#ifdef PC
	Pool* prev_active_pool = active_pool;		
	active_pool = null;		

	cached_file_names.Clear();

	char str[512];
	String::ExtractPath(fileName, str, true);
	String::GetCropPath(appPath, str, cache_dir, 256);

	IFileBuffer* fb = LoadFile(fileName);
	active_pool = prev_active_pool;

	if (fb)
	{
		byte* ptr = fb->GetBuffer();
		
		dword magic_id = *((dword*)ptr);
		ptr += 4;

		if (magic_id == cachedMagicID)
		{
			int count = *((dword*)ptr);
			ptr += 4;

			for (int i=0;i<count;i++)
			{
				int len = *((dword*)ptr);
				ptr += 4;

				struct stat* st = cached_file_names.Add((char*)ptr);
				ptr += len;

				memcpy(st, ptr, sizeof(struct stat));
				ptr += sizeof(struct stat);
			}
		}

		fb->Release();
	}
#endif
}

void FileService::SaveCached(const char * fileName)
{
#ifdef PC
	Pool* prev_active_pool = active_pool;		
	active_pool = null;	

	IFile* file = OpenFile(fileName, File::write);

	if (!file) return;

	active_pool = prev_active_pool;

	file->Write(&cachedMagicID, 4);

	int count = cached_file_names.Size();
	file->Write(&count, 4);

	for (int i=0;i<count;i++)
	{
		int len = strlen(cached_file_names.GetName(i)) + 1;
		file->Write(&len, 4);

		file->Write(cached_file_names.GetName(i), len - 1);
		byte zero = 0;
		file->Write(&zero, 1);

		struct stat* st = cached_file_names.Get(i);
		file->Write(st, sizeof(struct stat));
	}

	file->Release();
#endif
}

void FileService::CorrectPath(char* path)
{
	char buff[2048];
	
	if (!String::ParsePlatform(path, buff, 2048, core->GetCurrentPlatform()))
	{
		strcpy(buff, path);
		//return;
	}
	
#ifdef PC
	if (path[1] != ':')
#endif
#ifdef OSUNIX
	if (path[0] != '/' /* || path[1] != 'h' || path[2] != 'o' ||
		path[3] != 'm' || path[4] != 'e' || path[5] != '/' ||  path[6] != 'u'*/)
#endif
	{		
		strcpy(path, appPath);
		strcat(path, buff);
	}
	else
	{
		strcpy(path, buff);
	}			
}
#endif

#ifdef ANDROID
void FileService::SetApkName(const char* set_apk_name)
{
	core->TraceTo("Files", "apk name - %s", set_apk_name);
	String::Copy(apk_name, 256, set_apk_name);
}

bool FileService::IsLoadFromApk()
{
	return load_from_apk;
}

void FileService::SetLoadFromApk(bool set)
{
	load_from_apk = set;
}

void FileService::SetBasePath(const char* set_base_path)
{
	String::Copy(base_path, 256, set_base_path);
}

const char* FileService::GetBasePath()
{
	return base_path;
}

#endif

void FileService::Release()
{
#ifdef ANDROID 	
	zip_close(zip_arch);
#endif
}
