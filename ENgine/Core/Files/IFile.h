
#pragma once

#include "Common/Common.h"

class IFile
{
public:

	enum mode_type
	{
		read = 0,
		write,
		writeText,
		append,
		appendText,
	};	

	enum seek_type
	{
		seek_set = 0,
		seek_cur = 1,
		seek_end = 2
	};

	virtual bool Init(const char* name, mode_type mode) = 0;	

	virtual int  GetSize() = 0;
	virtual void Write(const void* data, int size) = 0;
	virtual void Print(const char *format, ...) = 0;
	virtual int  Read(void* data, int size) = 0;	
	virtual void Read(char* str) = 0;
	virtual int  Seek(dword offset, seek_type seek) = 0;
	virtual dword GetCurrentPos() = 0;
	virtual void  Release() = 0;
};


