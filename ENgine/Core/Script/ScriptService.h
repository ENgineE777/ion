#pragma once

#include "IScriptService.h"
#include "ScriptModulecore.h"
#include "ScriptMachine.h"

class ScriptMachine;

#if defined(PC) || defined(OSUNIX)
//#define LOG_SCRIPT_PARSER

namespace  ScriptCodeParserData
{
    struct Object
    {
    protected:
        string sName;
        string sClass;
        string sNameSpace;
        string sFileName; 
        int    nPos;
        bool   bArray;
        bool   bPrivate;
        int    nRefCount;

        void   LoadString(string& sBuffer,const char* sName,IStreamLoader* pLoader);
    public:
        Object():nPos(-1),bArray(false),bPrivate(false),nRefCount(1){++g_nCreatedCount;}
        Object(const string& file):nPos(-1),bArray(false),bPrivate(false),sFileName(file),nRefCount(1){++g_nCreatedCount;}
        Object(const string& file,const string& name,int pos,bool  _bPrivate):
            sFileName(file),sName(name),nPos(pos),bArray(false),bPrivate(_bPrivate),nRefCount(1)
        {++g_nCreatedCount;}
        Object(const string& file,const string& name,const string& _class,int pos,bool _bArray,bool  _bPrivate):
            sFileName(file),sName(name),sClass(_class),nPos(pos),bArray(_bArray),bPrivate(_bPrivate),nRefCount(1)
        {++g_nCreatedCount;}
        Object(const string& file,const string& name,const string& _class,const string& nameSpace,int pos,bool _bArray,bool  _bPrivate):
            sFileName(file),sName(name),sClass(_class),sNameSpace(nameSpace),nPos(pos),bArray(_bArray),bPrivate(_bPrivate),nRefCount(1)
        {++g_nCreatedCount;}
        virtual ~Object(){}
        inline const string& Name() const {return sName;}
        inline const string& Class() const {return sClass;}
        inline const string& File() const {return sFileName;}
        inline const string& NameSpace() const {return sNameSpace;}
        inline int Pos() const {return nPos;}
        inline bool IsArray() const {return bArray;}
        inline bool IsPrivate() const {return bPrivate;}
        inline void IncRef(){ ++nRefCount; }
        inline void Delete(){ --nRefCount;if(nRefCount == 0){--g_nCreatedCount;delete this;}}
        virtual void Save(IStreamSaver* pSaver);
        virtual void Load(IStreamLoader* pLoader);

        static int g_nCreatedCount;
    };

    struct Reference:public Object
    {

    public:
        Reference():Object(){}
        Reference(const string& file):Object(file){}
        Reference(const string& file,const string& name,const string& _class,int pos,bool _bArray,bool  _bPrivate);
        Reference(const string& file,const string& name,const string& _class,const string& nameSpace,int pos,bool _bArray,bool  _bPrivate);
        Reference(const Reference& other);
        Reference& operator = (const Reference& other );
        Reference& operator += (int nOffset){nPos += nOffset;return *this;}

    };
    typedef HashTable<string,Reference*> TReferences;
    struct Function:public Object
    {
    private:
        string sSignature;
        int    nParamsCount;
    public:
        Function():Object(),nParamsCount(-1){}
        Function(const string& file):Object(file),nParamsCount(-1){}
        Function(const string& file,const string& name,const string& signature,int pos,bool  _bPrivate);
        Function(const Function& other);
        inline const string& Signature() const {return sSignature;}
        inline int ParamsCount() const {return nParamsCount;}
        Function& operator = (const Function& other );
        Function& operator += (int nOffset){nPos += nOffset;return *this;}
        virtual void Save(IStreamSaver* pSaver);
        virtual void Load(IStreamLoader* pLoader);


    };
    typedef HashTable<string,Function*> TFunctions;

    class ScriptNameSpace
    {
    public:
        enum ETypes{eClass,eFunction,eNameSpace,eBrackets,eFile,eNone};
    private:
        string              sName;
        string              sFileName;
        int                 nStartLine;
        int                 nStartPos; 
        int                 nEndLine;
        int                 nEndPos; 
        ETypes              eType; 
        std::vector<string> vParents;
        TReferences         vReferences;
        TFunctions          vFunctions;
                
        std::vector<ScriptNameSpace*> vClasses;

        bool  GetMemberPos(const string& sMember,int& nPos);         
        bool  GetFunctionPos(const string& sFunction,int& nPos);   
        bool  IsNamePresents(std::vector<string>& vNames,const string& sName);
        void  GetFunctionNames(std::vector<string>& vMembers,bool bPrivate,bool bSearchInParents);
        void  GetFunctions(std::vector<Function*>& vList,const string& sName,bool bPrivate,bool bSearchInParents);
        void  Clear();

    public:
		ScriptNameSpace():vReferences(512),vFunctions(512),nStartLine(-1),nStartPos(-1),nEndLine(-1),nEndPos(-1),eType(eNone)
		{}
        ScriptNameSpace(const string& file):vReferences(512),vFunctions(512),sFileName(file),nStartLine(-1),nStartPos(-1),nEndLine(-1),nEndPos(-1),eType(eNone)
        {}
        ScriptNameSpace(const string& file,const string& name,ETypes type,int line,int pos):
           vReferences(512),vFunctions(512),sName(name),sFileName(file),nStartLine(line),nStartPos(pos),nEndLine(-1),nEndPos(-1),eType(type)
        {}
        ~ScriptNameSpace();
        void AddParent(const string& name);
        void AddReference(const string& name,const std::vector<string>& vWordsStack,int nPos);
        void Merge(ScriptNameSpace* pOther);
        void Cut(ScriptNameSpace* pOther);

        void AddPublicFunction(const string& name,const string& signature,int nPos);
        void AddPrivateFunction(const string& name,const string& signature,int nPos);

        void GetMembers(std::vector<string>& vMembers,bool bPrivate,bool bSearchInParents);
        void GetMembers(std::vector<string>& vMembers,int nLine,bool bSearchInParents );

        void GetSignatures(std::vector<string>& vSignatures,int nLine,const string& sName,bool bSearchInParents);
        void GetSignatures(std::vector<string>& vSignatures,bool bPrivate,bool bSearchInParents);


        void AddClass(ScriptNameSpace*  pClass){vClasses.push_back(pClass);}

        bool IsEmpty() const;
        inline ETypes Type() const {return eType;}
        inline const string& Name() const {return sName;}
        inline const string& File() const {return sFileName;}
        inline int StartLine() const {return nStartLine;}
        inline int EndLine() const {return nEndLine;}
        inline int StartPos() const {return nStartPos;}
        inline int EndPos() const {return nEndPos;}
        inline void  SetEnd(int line,int pos){nEndLine = line;nEndPos = pos; }
        inline void  SetStart(int line,int pos){nStartLine = line;nStartPos = pos;}
        int   GetMemberPos(const string& sMember);
        int   GetFunctionPos(const string& sFunction);
        int   GetChildNameSpacePos(const string& sNameSpace);
        bool  IsPresents(const string& sName,ScriptNameSpace** pSpaceOut);

        Function*  GetFunction(const string& sName,bool bPrivate);
        Reference* GetReference(const string& sName,bool bPrivate);
        void       GetNameSpace(const string& sName,int nLine,const char* sFileName,std::vector<string>& vNameSpaces,bool& bNameSpace);
        void       OnTextMofified(int nPos,int nOffset);

        void       Save(IStreamSaver* pSaver);
        void       Load(IStreamLoader* pLoader);

    };

};


typedef HashTable<string,ScriptCodeParserData::ScriptNameSpace*> TScriptNameSpaces;
typedef std::vector<ScriptCodeParserData::ScriptNameSpace*>      TScriptNameSpacesList;

#endif

class ScriptService : public IScriptService
{	
	ScriptModuleCore moduleCore;
	ScriptMachine* machine;
	bool allowedExecuteScript;
	bool printException;

   	struct Exception
	{
		vector<string> callstack;
		float fade_time;
	};

	vector<Exception> exceptions;

#if defined(PC) || defined(OSUNIX)
private:

    TScriptNameSpaces                       vScriptNameSpaces;
    TScriptNameSpaces                       vScriptClasses;
    HashTable<string,TScriptNameSpacesList> vFileNamesData;

    ScriptCodeParserData::ScriptNameSpace* FindNameSpace(int nLine,const char* sFileName,const string& sName);
    ScriptCodeParserData::ScriptNameSpace* GetNameSpace(ScriptCodeParserData::ScriptNameSpace* pParent,int nLine,const char* sFileName,const string& sName,std::vector<string>& vNameSpace,bool& bNameSpace);
    ScriptCodeParserData::ScriptNameSpace* GetFirstSpace(int nLine,const char* sFileName,std::vector<string>& vPath,bool bNameSpace);
    char* ScriptParseDataFileName(const char* name);
    void  ClearScriptParseData();

public:
    virtual ScriptModuleCore* GetCoreModule(){return &moduleCore;}   

    virtual void AddNameSpace(ScriptCodeParserData::ScriptNameSpace* pSpace,const char* sFileName);
	virtual void AddCodeToNameSpaces(const char* name);
    virtual void ParseCode(const char* name);
    virtual void SaveParseData(const char* name);      
    virtual bool LoadParseData(const char* name);      

    virtual ScriptCodeParserData::ScriptNameSpace* GetNameSpace(const char* sName);
    virtual ScriptCodeParserData::ScriptNameSpace* GetNameSpace(int nLine,const char* sFileName,std::vector<string>& vPath,bool& bNameSpace);

    virtual int  FindPosition(int nLine,const char* sFileName,std::vector<string>& vPath,bool bNameSpace,string& sOutFile);

    virtual void GetNameSpaces(std::vector<string>& sNames,const string& sFileName);
    virtual void GetMembers(std::vector<string>& sNames,const string& sClass,bool bSearchInParents);
    virtual void GetFunctions(std::vector<string>& sNames,const string& sClass,bool bSearchInParents);
    virtual int  GetMemberPos(const string& sClass,const string& sMember);
    virtual int  GetFunctionPos(const string& sClass,const string& sFunction);
	virtual void ReParseCode(const char* sFileName,const string& sText);
    virtual void OnTextMofified(const char* sFileName,int nLine,int nLineAdded,int nPos,int nOffset);
    virtual void GetFunctionName(string& sName,int nLine,const char* sFileName);
	virtual void GetScriptFilesList(std::vector<string>& vList);

	bool skip_help;
#endif
public:	

	ScriptService();

	virtual bool Init();
	virtual bool ReInit();
	virtual IScriptModule* LoadModule(const char* name);
	virtual IScriptMachine* Machine() { return machine; };
	virtual bool IsAllowedExecuteScript();
	virtual void SetAllowExecuteScript(bool set_allow);
	virtual bool ExportPrecompiledByteCode();
	virtual bool UsePrecompiledByteCode();
	virtual void SetPrintException(bool set);
	virtual void Work(float dt, int level);	
	virtual void Release();

    void Exceptioncallback(asIScriptContext *context);

	static const char* GetScriptFilePath(const char* sName);

};



