
#pragma once

#include "IFileBuffer.h"
#include <stdio.h>

class FileBuffer : public IFileBuffer
{
	byte* pBuffer;
	unsigned long  lenght;

public:	

	FileBuffer();

	virtual bool Init(const char* name);
	
	virtual byte* GetBuffer();
	virtual unsigned long  GetSize();

	virtual void Release();
};