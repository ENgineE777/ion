#pragma once

#include "Common/Common.h"
#include "angelscript.h"


#define CALLWRAPPERNAME(method) method##_Generic

#define CALLWRAPPERNAMEAS(method) asFUNCTION(method##_Generic)

#define CALLWRAPPERRET(rettype) *(rettype*)(gen->GetAddressOfReturnLocation()) = 

#define CALLWRAPPERNORET()

#define CALLWRAPPERGETADRESS(argtype, argName) \
argtype* argName = (argtype*)(gen->GetArgAddress(argIndex));\
argIndex++;

#define CALLWRAPPERGETPARAM(argtype, argName) \
argtype* argName = (argtype*)(gen->GetAddressOfArg(argIndex));\
argIndex++;


#define CALLWRAPPER(classname, method, ret)\
void CALLWRAPPERNAME(method)(asIScriptGeneric *gen)\
{\
    classname* obj = (classname*)gen->GetObject();\
    ret obj->method();\
}


#define CALLWRAPPERARG1(classname, method, getarg1, ret)\
void CALLWRAPPERNAME(method)(asIScriptGeneric *gen)\
{\
    classname* obj = (classname*)gen->GetObject();\
	int argIndex = 0;\
	getarg1\
    ret obj->method(*arg1);\
}

#define CALLWRAPPERARG2(classname, method, getarg1, getarg2, ret)\
void CALLWRAPPERNAME(method)(asIScriptGeneric *gen)\
{\
    classname* obj = (classname*)gen->GetObject();\
	int argIndex = 0;\
	getarg1\
	getarg2\
    ret obj->method(*arg1, *arg2);\
}

#define CALLWRAPPERARG3(classname, method, getarg1, getarg2, getarg3, ret)\
void CALLWRAPPERNAME(method)(asIScriptGeneric *gen)\
{\
    classname* obj = (classname*)gen->GetObject();\
	int argIndex = 0;\
	getarg1\
	getarg2\
	getarg3\
    ret obj->method(*arg1, *arg2, *arg3);\
}

#define CALLWRAPPERARG4(classname, method, getarg1, getarg2, getarg3, getarg4, ret)\
void CALLWRAPPERNAME(method)(asIScriptGeneric *gen)\
{\
    classname* obj = (classname*)gen->GetObject();\
	int argIndex = 0;\
	getarg1\
	getarg2\
	getarg3\
	getarg4\
    ret obj->method(*arg1, *arg2, *arg3, *arg4);\
}

#define CALLWRAPPERARG5(classname, method, getarg1, getarg2, getarg3, getarg4, getarg5, ret)\
void CALLWRAPPERNAME(method)(asIScriptGeneric *gen)\
{\
    classname* obj = (classname*)gen->GetObject();\
	int argIndex = 0;\
	getarg1\
	getarg2\
	getarg3\
	getarg4\
	getarg5\
    ret obj->method(*arg1, *arg2, *arg3, *arg4, *arg5);\
}

#define CALLWRAPPERARG6(classname, method, getarg1, getarg2, getarg3, getarg4, getarg5, getarg6, ret)\
void CALLWRAPPERNAME(method)(asIScriptGeneric *gen)\
{\
    classname* obj = (classname*)gen->GetObject();\
	int argIndex = 0;\
	getarg1\
	getarg2\
	getarg3\
	getarg4\
	getarg5\
	getarg6\
    ret obj->method(*arg1, *arg2, *arg3, *arg4, *arg5, *arg6);\
}

#define CALLWRAPPERARG7(classname, method, getarg1, getarg2, getarg3, getarg4, getarg5, getarg6, getarg7, ret)\
void CALLWRAPPERNAME(method)(asIScriptGeneric *gen)\
{\
    classname* obj = (classname*)gen->GetObject();\
	int argIndex = 0;\
	getarg1\
	getarg2\
	getarg3\
	getarg4\
	getarg5\
	getarg6\
	getarg7\
    ret obj->method(*arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7);\
}

#define CALLWRAPPERARG8(classname, method, getarg1, getarg2, getarg3, getarg4, getarg5, getarg6, getarg7, getarg8, ret)\
void CALLWRAPPERNAME(method)(asIScriptGeneric *gen)\
{\
    classname* obj = (classname*)gen->GetObject();\
	int argIndex = 0;\
	getarg1\
	getarg2\
	getarg3\
	getarg4\
	getarg5\
	getarg6\
	getarg7\
	getarg8\
    ret obj->method(*arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7, *arg8);\
}

#define CALLWRAPPERARG9(classname, method, getarg1, getarg2, getarg3, getarg4, getarg5, getarg6, getarg7, getarg8, getarg9, ret)\
void CALLWRAPPERNAME(method)(asIScriptGeneric *gen)\
{\
    classname* obj = (classname*)gen->GetObject();\
	int argIndex = 0;\
	getarg1\
	getarg2\
	getarg3\
	getarg4\
	getarg5\
	getarg6\
	getarg7\
	getarg8\
	getarg9\
    ret obj->method(*arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7, *arg8, *arg9);\
}

#define CALLWRAPPERARG10(classname, method, getarg1, getarg2, getarg3, getarg4, getarg5, getarg6, getarg7, getarg8, getarg9, getarg10, ret)\
void CALLWRAPPERNAME(method)(asIScriptGeneric *gen)\
{\
    classname* obj = (classname*)gen->GetObject();\
	int argIndex = 0;\
	getarg1\
	getarg2\
	getarg3\
	getarg4\
	getarg5\
	getarg6\
	getarg7\
	getarg8\
	getarg9\
	getarg10\
    ret obj->method(*arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7, *arg8, *arg9, *arg10);\
}

#define CALLWRAPPERARG11(classname, method, getarg1, getarg2, getarg3, getarg4, getarg5, getarg6, getarg7, getarg8, getarg9, getarg10, getarg11, ret)\
void CALLWRAPPERNAME(method)(asIScriptGeneric *gen)\
{\
    classname* obj = (classname*)gen->GetObject();\
	int argIndex = 0;\
	getarg1\
	getarg2\
	getarg3\
	getarg4\
	getarg5\
	getarg6\
	getarg7\
	getarg8\
	getarg9\
	getarg10\
	getarg11\
    ret obj->method(*arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7, *arg8, *arg9, *arg10, *arg11);\
}


class IScriptFunction;

class IScriptMachine
{
public:	
	
	virtual void AddIntArg(int arg) = 0;
	virtual void AddFloatArg(float arg) = 0;
	virtual void AddPtrArg(void* arg) = 0;
	virtual void AddStrArg(const char* arg) = 0;

	virtual void Call(IScriptFunction* sf) = 0;				

	virtual void  RegisterObjectType(const char* obj, int byteSize, dword flags) = 0;
	virtual void  RegisterFunctionDef(const char* decl) = 0;
	virtual void  RegisterObjectProperty(const char* obj, const char* declaration, int byteOffset) = 0;
	virtual void  RegisterObjectMethod(const char* obj, const char* declaration, const asSFuncPtr& funcPointer) = 0;
	virtual void  RegisterObjectBehaviour(const char *obj, asEBehaviours behaviour, const char *declaration, const asSFuncPtr &funcPointer) = 0;

	virtual void  RegisterGlobalProperty(const char* declaration, void* pointer) = 0;
	virtual void  RegisterGlobalFunction(const char* declaration, const asSFuncPtr& funcPointer, dword callConv) = 0;

	virtual int   GetTypeIdByDecl(const char* decl) = 0;
#if defined(PC) || defined(OSUNIX)
    virtual void  GetMembers(const std::vector<string>& vPath,std::vector<string>& vMembers) = 0;
    virtual void  GetSignature(const std::vector<string>& vPath,std::vector<string>& vSignatures) = 0;
    virtual void  GetMembers(const string& sObjectType,std::vector<string>& vMembers) = 0;
    virtual void  GetSignature(const string& sObjectType,const string& sMethod,std::vector<string>& vSignatures) = 0;
#endif
	virtual void Release() = 0;
};



