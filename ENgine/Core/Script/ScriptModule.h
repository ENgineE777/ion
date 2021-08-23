
#pragma once

#include "IScriptModule.h"
#include "ScriptMachine.h"

#include <set>

class IFile;
class IFileBuffer;
namespace ScriptCodeParserData
{
    class ScriptNameSpace;
}

class ScriptFunction : public IScriptFunction
{		
	asIScriptFunction* sf;	
	void*              object;

public:		

	asIScriptModule* module;

	ScriptFunction();
	void Set(asIScriptFunction* set_sf, void* set_object = null);
	asIScriptFunction* Get();		
	void* GetObject();

	virtual void Reset();

	static void Find(IScriptFunction* sf, IScriptModule* module, const char* func);

	static void Find1F(IScriptFunction* sf, IScriptModule* module, const char* func);
	static void Find1S(IScriptFunction* sf, IScriptModule* module, const char* func);
	static void Find1P(IScriptFunction* sf, IScriptModule* module, const char* func, const char* obj);

	static void Find2PI(IScriptFunction* sf, IScriptModule* module, const char* func, const char* obj);		
	static void Find2PF(IScriptFunction* sf, IScriptModule* module, const char* func, const char* obj);		
	static void Find2PS(IScriptFunction* sf, IScriptModule* module, const char* func, const char* obj);		

	static void Find3PFF(IScriptFunction* sf, IScriptModule* module, const char* func, const char* obj);		
	static void Find4PIFF(IScriptFunction* sf, IScriptModule* module, const char* func, const char* obj);				
};

class ScriptModule : public IScriptModule
{
private:

#if defined(PC) || defined(OSUNIX)
    enum ParserStates
    {
        eUnknownState,
        eGlobalState,
        eClassDeclarationState,
        eClassParentsDeclarationState,
        eClassBodyState,
        eFunctionDeclarationState,
        ePrivateFunctionDeclarationState,
        eFunctionBodyState,
        eBracketsBodyState,
        eValueState,
        eParserPauseState,
        eEnumDeclarationState,
        eEnumBodyState,
        eNameSpaceDeclaration,
    };

    struct Define
    {
        string              sValue;
        std::vector<string> vParams;
        void Replace(ScriptMachine*  machine,std::vector<string>& params,string& out);
    };
    typedef HashTable<string,Define>  TDefines;

    TDefines                vDefines;
    
    std::set<std::string>   vIncludedScriptsNames;

    enum EBuildStep {ePreProcessor = 0,eBuild}; 


    void          OverwriteCode(char *code, int len);
    bool          LoadScript(const char *name,string& out);
    int           CalculateLines(const string& sText);
    asETokenClass ParseToken(const string& sScriptText,unsigned& pos,asUINT* len);

    void          ParseDefines(string& sScriptText,unsigned& pos,bool& bPreProcessorIF,EBuildStep eStep);
    void          ReplaceDefineParams(string& sScriptText,Define* pDefine,unsigned& pos,asUINT len);
    void          DeletePreProcessorBlock(string& sScriptText,unsigned& pos);
    void          GetDefineParams(const string& sScriptText,unsigned& pos,std::vector<string>& vParams);    
    void          ProcessDefines(const char* sScriptName,string& sScriptText,EBuildStep eStep);
    void          ReplaceString(string& sScriptText,const string& sText,unsigned& pos,asUINT len);

    bool          LoadIncludeFile(string& includefile,string& out);
    bool          IncludeIfNotAlreadyIncluded(const char *name);
    bool          BuildScript(const char *sSectionName,std::string& sScriptText,EBuildStep eStep);
    bool          BuildScript(const char *sSectionName,EBuildStep eStep);

    void          BuildNameData(const char* sScriptName,const string& sText,bool bParseIncludes = true);
    void          AddReference(std::vector<string>& vWordStack,ScriptCodeParserData::ScriptNameSpace* pSpace,int nPos);
    void          AddEnumReference(ScriptCodeParserData::ScriptNameSpace* pLastNameSpace,std::vector<string>& vWordsStack,string sName,int pos);
    inline void   PopParserState(std::vector<ParserStates>& vStack)
    {
        if(vStack.size() > 1)
        {
#ifdef _DEBUG
            vStack.back() = eUnknownState;
#endif
            vStack.pop_back();
        }
    }

#endif	

public:	

	static ScriptModule* cur_exec_module;

	char sScriptName[256];
	ScriptMachine*    machine;
	asIScriptModule*  module;
	

#if defined(PC) || defined(OSUNIX)
	class BytecodeStreamSaver : public asIBinaryStream
	{			
	public:
		
		BytecodeStreamSaver(IFile *fp);

		virtual void Write(const void *ptr, asUINT size);				
		virtual void Read(void *ptr, asUINT size);		

	protected:
		IFile* f;
	};
#endif

	class BytecodeStreamLoader : public asIBinaryStream
	{			
	public:

		BytecodeStreamLoader(byte* ptr);

		virtual void Write(const void *ptr, asUINT size);				
		virtual void Read(void *ptr, asUINT size);		

	protected:
		byte* buffer;
	};


public:	

	ScriptModule(ScriptMachine* _machine);
	bool Load(const char* name);
	
	virtual bool Find(IScriptFunction* sf, const char* desc);				

	virtual void BindImportFunctions();	

#if defined(PC) || defined(OSUNIX)
	void AddCodeToNameSpaces(const char* name);
    void ParseCode(const char* name);
    void ReParseCode(const char* name,const string& sText);
	virtual void SaveByteCode(const char* name);
#endif

	virtual void Release();
};