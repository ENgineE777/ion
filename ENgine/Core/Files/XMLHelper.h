
#pragma once

#include "Common/Common.h"
#include "Common/libxml/tinyxml2.h"
#include "Core/Files/IFile.h"

bool XMLReadString(tinyxml2::XMLNode* node, const char* name, char* val, int val_len);
int  XMLGetStringSize(tinyxml2::XMLNode* node, const char* name);
bool XMLReadBool(tinyxml2::XMLNode* node, const char* name, bool& val);
bool XMLReadFloat(tinyxml2::XMLNode* node, const char* name, float& val);
bool XMLReadInt(tinyxml2::XMLNode* node, const char* name, int& val);
bool XMLReadInt64(tinyxml2::XMLNode* node, const char* name, int64& val);
bool XMLReadDword(tinyxml2::XMLNode* node, const char* name, dword& val);
bool XMLReadMatrix(tinyxml2::XMLNode* node, const char* name, Matrix& mat);
bool XMLReadVector(tinyxml2::XMLNode* node, const char* name, Vector& v);

void XMLSaveString(IFile* pFile, const char* name, const char* val);
void XMLSaveBool(IFile* pFile, const char* name, bool val);
void XMLSaveFloat(IFile* pFile, const char* name, float val);
void XMLSaveInt(IFile* pFile, const char* name, int val);
void XMLSaveInt(IFile* pFile, const char* name, int64 val);
void XMLSaveDword(IFile* pFile, const char* name, dword val);
void XMLSaveMatrix(IFile* pFile, const char* name, Matrix& mat);
void XMLSaveVector(IFile* pFile, const char* name, Vector& v);
