
#pragma once

#include "Common/Common.h"

class IDataFile
{
#ifdef EDITOR

protected:

	bool need_log_warnings;

#endif

public:

	virtual bool EnterBlock(const char* name) = 0;
	virtual void LeaveBlock() = 0;

	virtual bool ReadString(const char* name, char* val, int val_len) = 0;
	virtual bool ReadBool(const char* name, bool& val) = 0;
	virtual bool ReadFloat(const char* name, float& val) = 0;
	virtual bool ReadInt(const char* name, int& val) = 0;
	virtual bool ReadInt64(const char* name, int64& val) = 0;
	virtual bool ReadDword(const char* name, dword& val) = 0;
	virtual bool ReadMatrix(const char* name, Matrix& mat) = 0;
	virtual bool ReadVector(const char* name, Vector& mat) = 0;

	virtual int  GetStringSize(const char* name) = 0;

#ifdef EDITOR
	virtual void SetNeedLogWarnings(bool set)
	{
		need_log_warnings = set;
	}
#endif

	virtual void Release() = 0;
};



