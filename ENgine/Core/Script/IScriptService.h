#pragma once

#include "Common/TaskHolder.h"
#include "IScriptModule.h"
#include "IScriptMachine.h"
namespace ScriptCodeParserData
{
    class ScriptNameSpace;
}
class ScriptModuleCore;
class IScriptService : public TaskHoldersOwner
{
public:	
	
	virtual bool Init() = 0;
	virtual bool ReInit() = 0;

#if defined(PC) || defined(OSUNIX)
	virtual void AddCodeToNameSpaces(const char* name) = 0;
    virtual void ParseCode(const char* name) = 0;      
    virtual void SaveParseData(const char* name) = 0;      
    virtual bool LoadParseData(const char* name) = 0;      
    virtual void AddNameSpace(ScriptCodeParserData::ScriptNameSpace* pSpace,const char* sFileName) = 0;
    virtual ScriptCodeParserData::ScriptNameSpace* GetNameSpace(const char* sName) = 0;
    virtual ScriptCodeParserData::ScriptNameSpace* GetNameSpace(int nLine,const char* sFileName,std::vector<string>& vPath,bool& bSpaceName) = 0;
    virtual int  FindPosition(int nLine,const char* sFileName,std::vector<string>& vPath,bool bNameSpace,string& sOutFile) = 0;
    virtual void GetNameSpaces(std::vector<string>& sNames,const string& sFileName) = 0;
    virtual void GetMembers(std::vector<string>& sNames,const string& sClass,bool bSearchInParents) = 0;
    virtual void GetFunctions(std::vector<string>& sNames,const string& sClass,bool bSearchInParents) = 0;
    virtual int  GetMemberPos(const string& sClass,const string& sMember) = 0;
    virtual int  GetFunctionPos(const string& sClass,const string& sFunction) = 0;
    virtual void ReParseCode(const char* sFileName,const string& sText) = 0;
    virtual void OnTextMofified(const char* sFileName,int nLine,int nLineAdded,int nPos,int nOffset) = 0; 
    virtual void GetFunctionName(string& sName,int nLine,const char* sFileName) = 0;
	virtual void GetScriptFilesList(std::vector<string>& vList) = 0;
    virtual ScriptModuleCore* GetCoreModule() = 0;
#endif

	virtual IScriptModule* LoadModule(const char* name) = 0;
	virtual IScriptMachine* Machine() = 0;

	virtual bool IsAllowedExecuteScript() = 0;
	virtual void SetAllowExecuteScript(bool set_allow) = 0;
	virtual bool ExportPrecompiledByteCode() = 0;
	virtual bool UsePrecompiledByteCode() = 0;
	virtual void SetPrintException(bool set) = 0;	
	virtual void Release() = 0;
};



