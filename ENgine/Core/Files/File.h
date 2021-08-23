
#pragma once

#include "Common/Common.h"
#include "IFile.h"
#include "stdio.h"

#ifdef ANDROID
#include "Common/libzip/zip.h"
#endif

class File : public IFile
{
	friend class FileService;

	byte* data_ptr;
	byte* ptr;

#ifdef ANDROID
	zip_file* zip_f;
#endif

	FILE* pFile;
	
	int size;
	int offset;

public:

	virtual bool Init(const char* name, mode_type mode);	

	virtual int  GetSize();
	virtual void Write(const void* data, int size);
	virtual void Print(const char *format, ...);
	virtual int  Read(void* data, int size);	
	virtual void Read(char* str);
	virtual int  Seek(dword offset, seek_type seek);
	virtual dword GetCurrentPos();

	virtual void  Release();

	FILE* GetHandle() { return pFile; };

private:

	bool Init(const char* name, int offset, int size, mode_type mode);
};


