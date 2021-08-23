
#pragma once

#include "IDataFile.h"
#include "Core/Files/XMLHelper.h"

class XMLFile : public IDataFile
{
	tinyxml2::XMLDocument* doc;
	int cur_depth;
	tinyxml2::XMLNode* nodes[50];
	tinyxml2::XMLNode* cur_node;
	char fl_name[256];

public:

	virtual bool Init(const char* name, const char* enterBlock,bool bMemory = false);

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
};



