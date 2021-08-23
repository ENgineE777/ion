
#pragma once

#include "IDataFile.h"
#include "Common/lib_json/json.h"

class JSONFile : public IDataFile
{
	block_allocator allocator;
	json_value* root;
	int cur_depth;
	json_value* nodes[50];	
	json_value* cur_node;	
	char fl_name[256];
    char*  data;
    IFileBuffer* fb;
public:

	JSONFile();
	virtual bool Init(const char* name,bool bMemory = false);

	virtual bool EnterBlock(const char* name);
	virtual void LeaveBlock();

	virtual bool ReadString(const char* name, char* val, int val_len);
	virtual bool ReadBool(const char* name, bool& val);
	virtual bool ReadFloat(const char* name, float& val);
	virtual bool ReadInt(const char* name, int& val);
	virtual bool ReadInt64(const char* name, int64& val);
	virtual bool ReadDword(const char* name, dword& val);
	virtual bool ReadMatrix(const char* name, Matrix& mat);
	virtual bool ReadVector(const char* name, Vector& mat);

	virtual int  GetStringSize(const char* name);

	virtual void Release();

protected:

	json_value* FindValue(const char* name);	
};



