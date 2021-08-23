
#pragma once

#include "Common/Common.h"
#include "Common/Containers/HashMap.h"
#include "IFileService.h"

class FileService : public IFileService
{		
	static dword cachedMagicID;

#if defined(PC)
	char cache_dir[256];	
	HashMap<struct stat> cached_file_names;	
#endif
	
#if defined(PC) || defined(OSUNIX)
	struct Pool
	{		
		HashMap<int> file_names;

		Pool() : file_names(_FL_)
		{
		};
	};

	HashMap<Pool> pools;
	Pool* active_pool;
	Pool* saved_pool;
#endif

public:

	static dword packMagicID;

#ifdef ANDROID
	char apk_name[256];
	bool load_from_apk;
	char base_path[256];
#endif

#if defined(PC) || defined(OSUNIX)
	char appPath[1024];	
#endif

	struct FileRecord
	{
		dword hash;
		char  filename[256];
		dword offset;
		dword size;
	};

	struct ResourcePack
	{		
		int num_records;
		FileRecord* records;
	};

	HashMap<ResourcePack> resource_packs;

	FileService();
	virtual bool  Init();
	virtual IFileBuffer* LoadFile(const char* file_name);
	virtual IFile* OpenFile(const char* file_name, IFile::mode_type mode);
	virtual IStreamLoader* CreateStreamLoader(const char* file_name, const char* enterBlock, IStreamLoader::FileType type,bool bMemory = false);
	virtual IStreamSaver* CreateStreamSaver(const char* file_name, IStreamSaver::FileType type);
	virtual IDataFile* OpenXMLFile(const char* file_name, const char* enterBlock,bool bMemory = false);
	virtual IDataFile* OpenJSONFile(const char* file_name,bool bMemory = false);

	virtual void ActivateResourcePack(const char* pack_path, bool activate);

#if defined(PC) || defined(OSUNIX)

	typedef void (*ConverterProgress)(const char* file_name, int current, int total);

	typedef void (*Converter)(const char* in, const char* out, const char* ext, Platform platform);

	ConverterProgress converterProgress;

	struct ExtConvertor
	{
		char ext[8];
		Converter converter;
	};

	struct PlatformConverters
	{
		Platform platform;
		vector<ExtConvertor> converters;
	};

	Converter post_converter;
	vector<PlatformConverters> platform_converters;

	void     SetConverterProgress(ConverterProgress converterProgress);
	void     RegisterPostConverter(Converter converter);
	void     RegisterConverter(Platform platform, const char* ext, Converter converter);
	void     ApplyConverter(Platform platform, const char* in, const char* out, bool fake);
		
	void     RegFile(const char* path);
	void	 RelativePath(char* path);
	void	 ReportFileError(const char * fileName);
	
	void     AddPool(const char* name);
	void     ClearPool(const char* name);
	void     ActivatePool(const char* name);
	void     SavePool(const char* name, const char* path, Platform platform);	

	void     LoadCached(const char * fileName);
	void     SaveCached(const char * fileName);

	bool load_from_res;
	void SetLoadFromRes(bool set) {load_from_res = set;};
	bool IsLoadFromRes() { return load_from_res; };
	const char* GetAppDirectory() { return appPath; };
	void	 CorrectPath(char* path);
#endif

#ifdef ANDROID
	virtual void SetApkName(const char* apk_name);
	virtual bool IsLoadFromApk();
	virtual void SetLoadFromApk(bool set);
	virtual void SetBasePath(const char* base_path);
	virtual const char* GetBasePath();
#endif

	virtual void  Release();
};
