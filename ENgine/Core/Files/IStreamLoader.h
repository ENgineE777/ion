
#pragma once

#include "Common/Common.h"

class IStreamLoader
{
public:

	enum FileType
	{
		xml = 0,
		binary,
		json
	};

	virtual bool Init(const char* name, const char* enterBlock, FileType type,bool bMemory = false) = 0;

#ifdef PC
	virtual void InitSaver(const char* name) = 0;
#endif

	virtual bool EnterBlock(const char* name) = 0;
	virtual void LeaveBlock() = 0;

	virtual bool Read(const char* name, char* val, int val_len) = 0;	
	virtual bool Read(const char* name, bool& val) = 0;
	virtual bool Read(const char* name, float& val) = 0;
	virtual bool Read(const char* name, int& val) = 0;	
	virtual bool Read(const char* name, int64& val) = 0;
	virtual bool Read(const char* name, dword& val) = 0;	
	virtual bool Read(const char* name, uint64& val) = 0;	
	virtual bool Read(const char* name, Matrix& mat) = 0;
	virtual bool Read(const char* name, Vector& val) = 0;	

	virtual void SetNeedLogWarnings(bool set) = 0;

	virtual int  GetStringSize(const char* name) = 0;

	virtual void Release() = 0;

    virtual FileType GetType() = 0;
};
