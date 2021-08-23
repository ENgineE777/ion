
#pragma once

#include "Common/Common.h"
#include "IFile.h"
#include "IFileBuffer.h"
#include "IDataFile.h"
#include "IStreamLoader.h"
#include "IStreamSaver.h"

class IFileService : public TaskHolder
{
public:

	virtual bool  Init() = 0;
	virtual IFileBuffer* LoadFile(const char* file_name) = 0;
	virtual IFile* OpenFile(const char* file_name, IFile::mode_type mode) = 0;
	virtual IStreamLoader* CreateStreamLoader(const char* file_name, const char* enterBlock, IStreamLoader::FileType type,bool bMemory = false) = 0;
	virtual IStreamSaver* CreateStreamSaver(const char* file_name, IStreamSaver::FileType type) = 0;
	virtual IDataFile* OpenXMLFile(const char* file_name, const char* enterBlock,bool bMemory = false) = 0;
	virtual IDataFile* OpenJSONFile(const char* file_name,bool bMemory = false) = 0;

	virtual void ActivateResourcePack(const char* pack_path, bool activate) = 0;

#ifdef ANDROID
	virtual void SetApkName(const char* apk_name) = 0;
	virtual bool IsLoadFromApk() = 0;
	virtual void SetLoadFromApk(bool set) = 0;
	virtual void SetBasePath(const char* base_path) = 0;
	virtual const char* GetBasePath() = 0;
#endif

	virtual void  Release() = 0;
};
