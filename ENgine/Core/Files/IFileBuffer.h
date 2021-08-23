
#pragma once

class IFileBuffer
{
public:	

	virtual bool Init(const char* name) = 0;
	
	virtual byte* GetBuffer() = 0;
	virtual unsigned long  GetSize() = 0;

	virtual void Release() = 0;
};