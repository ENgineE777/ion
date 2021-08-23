
#pragma once

#include "Common/Common.h"
#include "IStreamSaver.h"
#include "IFile.h"

#include "Core/Files/XMLHelper.h"

class StreamSaver : public IStreamSaver
{
	IFile* file;
	FileType type;

    static string sBuffer;
	const char*  ReplaceXmlControlSymbols(const char* name);

public:

	virtual bool Init(const char* name, FileType type);
	virtual FileType GetType();	

	virtual void MarkBeginBlock(const char* name); 
	virtual void MarkEndBlock(const char* name); 

	virtual void Write(const char* name, bool val);
	virtual void Write(const char* name, const char* val);
	virtual void Write(const char* name, float val);
	virtual void Write(const char* name, int val);
	virtual void Write(const char* name, int64 val);
	virtual void Write(const char* name, dword val);
	virtual void Write(const char* name, Matrix& mat);
	virtual void Write(const char* name, Vector& v);	

	virtual void Release();

    virtual FileType Type(){ return type;}
};
