#pragma once

#include "IScriptMachine.h"

class ScriptService;

class ScriptMachine : public IScriptMachine
{
	asIScriptEngine* machine;
	ScriptService*   service;	
	
	struct FuncArg		
	{
		int type;

		union
		{
			int   iarg;
			float farg;				
			void* parg;
		};			

		string sarg;
	};	

	int prev_num_args;
	vector<FuncArg> args;

public:

	int cur_context;
	asIScriptContext* ctxs[5];	

#if defined(PC) || defined(OSUNIX)
     asITypeInfo* GetTypeById(int id);
     asITypeInfo* GetTypeByType(const string& sObjectType);
     void           GetMembers(asITypeInfo* pType,std::vector<string>& vMembers);
     void           GetSignature(asITypeInfo* pType,const string& sMethod,std::vector<string>& vSignatures);
     asITypeInfo* GetReturnType(const string& sMethod,std::vector<string>& vSignatures);
#endif
public:	

	ScriptMachine(ScriptService* srv);	

	virtual void AddIntArg(int arg);
	virtual void AddFloatArg(float arg);
	virtual void AddPtrArg(void* arg);
	virtual void AddStrArg(const char* arg);

	virtual void Call(IScriptFunction* sf);	

	virtual void  RegisterFunctionDef(const char* decl);
	virtual void  RegisterObjectType(const char* obj, int byteSize, dword flags);
	virtual void  RegisterObjectProperty(const char* obj, const char* declaration, int byteOffset);
	virtual void  RegisterObjectMethod(const char* obj, const char* declaration, const asSFuncPtr& funcPointer);
	virtual void  RegisterObjectBehaviour(const char *obj, asEBehaviours behaviour, const char *declaration, const asSFuncPtr &funcPointer);

	virtual void  RegisterGlobalProperty(const char* declaration, void* pointer);
	virtual void  RegisterGlobalFunction(const char* declaration, const asSFuncPtr& funcPointer, dword callConv);

	virtual int   GetTypeIdByDecl(const char* decl);
	
	asIScriptContext* GrabContext(const char* modul_name);
	void FreeContext();

	virtual void Release();
#if defined(PC) || defined(OSUNIX)
    virtual void  GetMembers(const std::vector<string>& vPath,std::vector<string>& vMembers);
    virtual void  GetMembers(const string& sObjectType,std::vector<string>& vMembers);
    virtual void  GetSignature(const std::vector<string>& vPath,std::vector<string>& vSignatures);
    virtual void  GetSignature(const string& sObjectType,const string& sMethod,std::vector<string>& vSignatures);
#endif
	asIScriptEngine* Machine() { return machine; };

};



