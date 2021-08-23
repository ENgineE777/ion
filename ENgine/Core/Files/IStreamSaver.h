
#pragma once

#include "Common/Common.h"

class IStreamSaver
{	
public :

	enum FileType
	{
		xml = 0,
		binary,
	};

	virtual bool Init(const char* name, FileType type) = 0;	
	virtual FileType GetType() = 0;	

	virtual void MarkBeginBlock(const char* name) = 0; 
	virtual void MarkEndBlock(const char* name) = 0; 

	virtual void Write(const char* name, bool val) = 0;
	virtual void Write(const char* name, const char* val) = 0;
	virtual void Write(const char* name, float val) = 0;
	virtual void Write(const char* name, int val) = 0;
	virtual void Write(const char* name, int64 val) = 0;
	virtual void Write(const char* name, dword val) = 0;
	virtual void Write(const char* name, Matrix& mat) = 0;
	virtual void Write(const char* name, Vector& v) = 0;	

	virtual void Release() = 0;

    virtual FileType Type() = 0;
};
