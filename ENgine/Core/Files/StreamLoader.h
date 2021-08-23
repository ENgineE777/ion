
#pragma once

#include "Common/Common.h"
#include "IStreamLoader.h"
#include "IDataFile.h"
#include "IFileBuffer.h"

class StreamLoader : public IStreamLoader
{
	IDataFile* data_file;	

#ifdef PC
	class IStreamSaver* saver;
	char sv_enterBlock[64];
#endif

	int   readed;
	IFileBuffer* file;
	byte* data;
	byte* ptr;		
	FileType type;

    static string sBuffer;
    void  ReplaceXmlControlSymbols(char* val, int val_len);

public:

	virtual bool Init(const char* name, const char* enterBlock, FileType type,bool bMemory = false);

#ifdef PC
	virtual void InitSaver(const char* name);
#endif

	virtual bool EnterBlock(const char* name);
	virtual void LeaveBlock();

	virtual bool Read(const char* name, char* val, int val_len);	
	virtual bool Read(const char* name, bool& val);
	virtual bool Read(const char* name, float& val);
	virtual bool Read(const char* name, int& val);	
	virtual bool Read(const char* name, int64& val);
	virtual bool Read(const char* name, dword& val);	
	virtual bool Read(const char* name, uint64& val);	
	virtual bool Read(const char* name, Matrix& mat);
	virtual bool Read(const char* name, Vector& val);	

	virtual int  GetStringSize(const char* name);

	virtual void SetNeedLogWarnings(bool set);

	virtual void Release();
    virtual FileType GetType() { return type;}
};
