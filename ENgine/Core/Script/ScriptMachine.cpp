
#include "ScriptService.h"
#include "ScriptMachine.h"
#include "ScriptModule.h"
#include "Core/Core.h"
#include "Libs/scriptstdstring.h"
#include "Libs/scriptmath.h"
#include "Libs/scriptmath2d.h"
#include "Libs/scriptmath3d.h"
#include "Libs/scriptarray.h"
#include "Libs/scriptdictionary.h"
#include "Libs/scripthandle.h"

void MessageCallback(const asSMessageInfo *msg, void *param)
{
	const char *type = "ERROR";
	if( msg->type == asMSGTYPE_WARNING ) 
		type = "WARNING";
	else if( msg->type == asMSGTYPE_INFORMATION )
		type = "INFO";
    int row = msg->row;
    const char *scriptSection = msg->section;
	if (msg->type == asMSGTYPE_ERROR)
		core->TraceTo("ScriptErr", "%s : %s in file: %s (%d, %d)",type,msg->message,scriptSection,row - 1, msg->col);
    else
        core->TraceTo("Script", "%s : %s in file: %s (%d, %d)",type,msg->message,scriptSection,row - 1, msg->col);

}

ScriptMachine::ScriptMachine(ScriptService* srv)
{
	service = srv;

	machine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	machine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);	

	machine->SetEngineProperty(asEP_ALLOW_UNSAFE_REFERENCES, true);	

	//machine->SetEngineProperty(asEP_INIT_GLOBAL_VARS_AFTER_BUILD, 0);

	RegisterStdString(machine);	
	RegisterScriptMath(machine);
	RegisterScriptMath2D(machine);
	RegisterScriptMath3D(machine);
	RegisterScriptArray(machine, true);	
	RegisterScriptDictionary(machine);
	RegisterStdStringUtils(machine);
	RegisterScriptHandle(machine);

	cur_context = 0;
	
	for (int i=0;i<5;i++)
	{
		ctxs[i] = 0;
	}

	prev_num_args = 0;

#if defined(EDITOR) && !defined(_DEBUG)

	EntityDedeclarator* decl_array[500];
	EntityDedeclarator* decl_array_gui[500];

	const char* decl_groups[500];
	int groups_count = 0; 
	
	int decl_count = 0;
	for (EntityDedeclarator* decl = EntityDedeclarator::GetFirst(); decl != NULL; decl = decl->GetNext())
	{
		if (!String::IsEqual(decl->GetName(), "NGUISymbolInstance") && 
			!String::IsEqual(decl->GetName(), "NGUIWidget"))			
		{
			decl_array[decl_count] = decl;
			decl_groups[decl_count] = decl->GetNodePanelSheetName();
			decl_count++;
		}
	}	

	for (int i=0;i<decl_count-1;i++)
	{
		for (int j=i+1;j<decl_count;j++)
		{
			if (!String::CompareAlphapedOrder(decl_array[i]->GetName(), decl_array[j]->GetName()))
			{
				EntityDedeclarator* tmp = decl_array[i];
				decl_array[i] = decl_array[j];
				decl_array[j] = tmp;
			}
		}
	}

	groups_count = decl_count;
	
	for (int i=0;i<groups_count-1;i++)
	{
		for (int j=i+1;j<groups_count;j++)
		{
			if (!String::CompareAlphapedOrder(decl_groups[i], decl_groups[j]))
			{
				const char* tmp = decl_groups[i];
				decl_groups[i] = decl_groups[j];
				decl_groups[j] = tmp;
			}
		}
	}

	for (int i=0;i<groups_count-1;i++)
	{
		if (String::IsEqual(decl_groups[i], decl_groups[i+1]))
		{
			for (int j=i;j<groups_count-1;j++)
			{
				const char* tmp = decl_groups[j];
				decl_groups[j] = decl_groups[j+1];
				decl_groups[j+1] = tmp;
			}

			groups_count--;
			i--;
		}
	}

	IFile* out_script_hlp_fl = core->Files()->OpenFile("Help/script/entities_groups.h", IFile::writeText);

	out_script_hlp_fl->Print("/**");
	
	out_script_hlp_fl->Print(" * @defgroup gr_scriptcore SceneScriptCore");
	out_script_hlp_fl->Print(" * @{");
	out_script_hlp_fl->Print(" *   @defgroup gr_scriptcore_parts Components");
	out_script_hlp_fl->Print(" *   @{");
	out_script_hlp_fl->Print(" *   @}");
	out_script_hlp_fl->Print(" *   @defgroup gr_callback Callbacks");
	out_script_hlp_fl->Print(" *   @{");
	out_script_hlp_fl->Print(" *   @}");
	out_script_hlp_fl->Print(" * @}");
	out_script_hlp_fl->Print(" * @}");

	out_script_hlp_fl->Print(" * @defgroup gr_entities Scene Entities");
	out_script_hlp_fl->Print(" * @{");
	
	for (int i=0;i<groups_count-1;i++)
	{
		out_script_hlp_fl->Print(" *   @defgroup gr_%s %s", decl_groups[i], decl_groups[i]);
		out_script_hlp_fl->Print(" *   @{");
		out_script_hlp_fl->Print(" *   @}");
	}

	out_script_hlp_fl->Print(" * @}");
	out_script_hlp_fl->Print(" */");

	RELEASE(out_script_hlp_fl)

	IFile* out_script_fl = core->Files()->OpenFile("Help/script/callbacks.h", IFile::writeText);
							
	out_script_fl->Print("/**");
	out_script_fl->Print(" * @ingroup gr_callback");
	out_script_fl->Print(" */");

	out_script_fl->Print("class Callback");
	out_script_fl->Print("{");
	
	RELEASE(out_script_fl)

#endif
}

char out_script_hlp[128];
IFile* out_script_hlp_fl = null;

void HackFinishScriptHlp()
{
	if (out_script_hlp_fl)
	{
		out_script_hlp_fl->Print("};");
		RELEASE(out_script_hlp_fl);
	}

#if defined(EDITOR) && !defined(_DEBUG)
	IFile* out_script_fl = core->Files()->OpenFile("Help/script/callbacks.h", IFile::appendText);
    
	out_script_fl->Print("};");
	
	RELEASE(out_script_fl)
#endif
}

void ScriptMachine::AddIntArg(int arg)
{
	args.push_back(FuncArg());
	FuncArg* farg = &args[args.size()-1];

	farg->type = 0;
	farg->iarg = arg;
}

void ScriptMachine::AddFloatArg(float arg)
{
	args.push_back(FuncArg());
	FuncArg* farg = &args[args.size()-1];

	farg->type = 1;
	farg->farg = arg;
}

void ScriptMachine::AddPtrArg(void* arg)
{
	args.push_back(FuncArg());
	FuncArg* farg = &args[args.size()-1];

	farg->type = 2;
	farg->parg = arg;
}

void ScriptMachine::AddStrArg(const char* arg)
{
	args.push_back(FuncArg());
	FuncArg* farg = &args[args.size()-1];

	farg->type = 3;
	farg->sarg = arg;
}

void ScriptMachine::Call(IScriptFunction* isf)
{
	ScriptFunction* sf = (ScriptFunction*)isf;	

	bool sf_valid = false;
	
	std::vector<SceneInstance*>& scenes = en_core.EN_Scene()->scenes;

	for (int i = 0; i < scenes.size(); i++)
	{
		if ((ScriptModule*)scenes[i]->scriptModule &&  ((ScriptModule*)scenes[i]->scriptModule)->module == sf->module)
		{
			sf_valid = true;
			break;
		}
	}	

	if (!sf_valid)
	{
		sf->Reset();
	}

	if (!core->Script()->IsAllowedExecuteScript() || !sf->Get())
	{
		args.resize(prev_num_args);
		return;
	}

	asIScriptContext* ctx = GrabContext(sf->module->GetName());
	if (!ctx)
	{
		args.resize(prev_num_args);
		return;
	}
	
	int r = ctx->Prepare(sf->Get());

	if (sf->GetObject())
	{
		ctx->SetObject(sf->GetObject());
	}

	if( r < 0 ) 
	{
		core->TraceTo("ScriptErr", "Error in '%s': Failed to prepare the context for %s.", sf->module->GetName(), sf->Get()->GetName());
		args.resize(prev_num_args);
	}
	else
	{	
		for (int i=prev_num_args;i <args.size();i++)
		{
			int index = i-prev_num_args;
			FuncArg* arg = &args[i];

			if (arg->type == 0)
			{
				ctx->SetArgDWord(index, arg->iarg);
			}
			else
			if (arg->type == 1)
			{
				ctx->SetArgFloat(index, arg->farg);
			}
			else
			if (arg->type == 2)
			{
				ctx->SetArgObject(index, arg->parg);
			}
			else
			if (arg->type == 3)
			{
				ctx->SetArgObject(index, &arg->sarg);
			}
		}		

		int num = args.size() - prev_num_args;

		prev_num_args += num;

		if (ctx->Execute() < 0)
		{
			core->TraceTo("ScriptErr", "Error in '%s': Failed to execute %s.", sf->module->GetName(), sf->Get()->GetName());						
		}

		prev_num_args -= num;
	}
	
	args.resize(prev_num_args);
	FreeContext();	
}

void ScriptMachine::RegisterFunctionDef(const char* decl)
{
#if defined(EDITOR) && !defined(_DEBUG)
	if (!service->skip_help)
	{		
		IFile* out_script_fl = core->Files()->OpenFile("Help/script/callbacks.h", IFile::appendText);
									
		string hlp_decl = decl;

		int k = hlp_decl.find("(");	
		hlp_decl.insert(k,")");

		k = hlp_decl.find(" ");	
		hlp_decl.insert(k+1,"(*");

		out_script_fl->Print("typedef %s;", hlp_decl.c_str());		

		RELEASE(out_script_fl)
	}
#endif

	int err = machine->RegisterFuncdef(decl);

	if (err<0)
	{
		const char* err_str = "unknown error";		
		
		switch (err)
		{
			case asINVALID_ARG: err_str = "The flags are invalid.";
			break;
			case asINVALID_DECLARATION: err_str = "The declaration is invalid.";
			break;
			case asNAME_TAKEN: err_str = "Name already taken";
			break;		
		}											

		core->TraceTo("Script", "Error in RegisterObjectType, decl - %s: %s", decl, err_str);
	}	
}

void ScriptMachine::RegisterObjectType(const char* obj, int byteSize, dword flags)
{

#if defined(EDITOR) && !defined(_DEBUG)
	if (!service->skip_help)
	{
		if (out_script_hlp_fl)
		{
			out_script_hlp_fl->Print("};");
			RELEASE(out_script_hlp_fl);
		}

		if (!String::IsEqual(obj, "NGUISymbolInstance") && 
			!String::IsEqual(obj, "NGUIWidget"))
		{
			sprintf(out_script_hlp, "Help/script/%s.h", obj);

			out_script_hlp_fl = core->Files()->OpenFile(out_script_hlp, IFile::writeText);

			const char* gr_name = null;
		
			if (String::IsEqual("SceneScriptCore", obj))
			{		
				gr_name = "scriptcore";
			}

			if (!gr_name)
			{
				for (EntityDedeclarator* decl = EntityDedeclarator::GetFirst(); decl != NULL; decl = decl->GetNext())
				{
					if (!String::IsEqual(decl->GetName(), "NGUISymbolInstance") && 
						!String::IsEqual(decl->GetName(), "NGUIWidget"))			
					{
						if (String::IsEqual(decl->GetName(), obj))
						{
							gr_name = decl->GetNodePanelSheetName();
							break;
						}				
					}
				}	
			}

			if (!gr_name)
			{
				gr_name = "scriptcore_parts";
			}

			out_script_hlp_fl->Print("/**");
			out_script_hlp_fl->Print(" * @ingroup gr_%s", gr_name);
			out_script_hlp_fl->Print(" */");


			out_script_hlp_fl->Print("class %s", obj);
			out_script_hlp_fl->Print("{");
			out_script_hlp_fl->Print("  public:");			
		}
	}
#endif

	int err = machine->RegisterObjectType(obj, byteSize, flags);

	if (err<0)
	{
		const char* err_str = "unknown error";		

		switch (err)
		{
			case asINVALID_ARG: err_str = "The flags are invalid.";
			break;
			case asINVALID_NAME: err_str = "The name is invalid.";
			break;
			case asALREADY_REGISTERED: err_str = "Another type of the same name already exists.";
			break;
			case asNAME_TAKEN: err_str = "The name conflicts with other symbol names.";
			break;
			case asLOWER_ARRAY_DIMENSION_NOT_REGISTERED: err_str = "When registering an array type the array element must be a primitive or a registered type.";
			break;
			case asINVALID_TYPE: err_str = "The array type was not properly formed.";
			break;
			case asNOT_SUPPORTED  : err_str = "The array type is not supported, or already in use preventing it from being overloaded.";
			break;
		}											
		
		core->TraceTo("Script", "Error in RegisterObjectType, obj - %s: %s", obj, err_str);
	}	
}

void ScriptMachine::RegisterObjectProperty(const char* obj, const char* declaration, int byteOffset)
{
#if defined(EDITOR) && !defined(_DEBUG)
	if (!service->skip_help)
	{
		if (out_script_hlp_fl)
		{
			out_script_hlp_fl->Print("   %s;", declaration);
		}		
	}
#endif

	int err = machine->RegisterObjectProperty(obj, declaration, byteOffset);

	if (err<0)
	{
		const char* err_str = "unknown error";

		switch (err)
		{
			case asWRONG_CONFIG_GROUP: err_str = "The object type was registered in a different configuration group.";
			break;
			case asINVALID_OBJECT: err_str = "The obj does not specify an object type.";
			break;
			case asINVALID_TYPE: err_str = "The obj parameter has invalid syntax.";
			break;
			case asINVALID_DECLARATION: err_str = "The declaration is invalid.";
			break;
			case asNAME_TAKEN: err_str = "The name conflicts with other members.";
			break;		
		}

		core->TraceTo("Script", "Error in RegisterObjectProperty, obj - %s, decl - %s: %s", obj, declaration,err_str);
	}	
}

void ScriptMachine::RegisterObjectMethod(const char* obj, const char* declaration, const asSFuncPtr& funcPointer)
{
#if defined(EDITOR) && !defined(_DEBUG)
	if (!service->skip_help)
	{
		if (out_script_hlp_fl)
		{
			out_script_hlp_fl->Print("   %s;", declaration);
		}		
	}
#endif

	int err = machine->RegisterObjectMethod(obj, declaration, funcPointer, asCALL_GENERIC);

	if (err<0)
	{		
		const char* err_str = "unknown error";		

		switch (err)
		{
			case asWRONG_CONFIG_GROUP: err_str = "The object type was registered in a different configuration group.";
			break;
			case asNOT_SUPPORTED: err_str = "The calling convention is not supported.";
			break;
			case asINVALID_TYPE: err_str = "The obj parameter is not a valid object name.";
			break;
			case asINVALID_DECLARATION: err_str = "The declaration is invalid.";
			break;
			case asNAME_TAKEN: err_str = "The name conflicts with other members.";
			break;
			case asWRONG_CALLING_CONV: err_str = "The function's calling convention isn't compatible with callConv.";
			break;	
		}			

		core->TraceTo("Script", "Error in RegisterObjectMethod, obj - %s, decl - %s: %s", obj, declaration,err_str);
	}	
}

void ScriptMachine::RegisterObjectBehaviour(const char *obj, asEBehaviours behaviour, const char *declaration, const asSFuncPtr &funcPointer)
{
	int err = machine->RegisterObjectBehaviour(obj, behaviour, declaration, funcPointer, asCALL_GENERIC);

	if (err<0)
	{		
		const char* err_str = "unknown error";		

		switch (err)
		{
			case asWRONG_CONFIG_GROUP: err_str = "The object type was registered in a different configuration group.";
				break;
			case asINVALID_ARG: err_str = "obj is not set, or a global behaviour is given in behaviour.";
				break;
			case asWRONG_CALLING_CONV: err_str = "The function's calling convention isn't compatible with callConv.";
				break;
			case asNOT_SUPPORTED: err_str = "The calling convention is not supported.";
				break;
			case asINVALID_TYPE: err_str = "The obj parameter is not a valid object name.";
				break;
			case asINVALID_DECLARATION: err_str = "The declaration is invalid.";
				break;
			case asILLEGAL_BEHAVIOUR_FOR_TYPE: err_str = "The behaviour is not allowed for this type."; 
				break;
			case asALREADY_REGISTERED: err_str = "The behaviour is already registered with the same signature.";
				break;
		}

		core->TraceTo("Script", "Error in RegisterObjectBehaviour, obj - %s, decl - %s: %s", obj, declaration,err_str);
	}	
}

void ScriptMachine::RegisterGlobalProperty(const char* declaration, void* pointer)
{
	int err = machine->RegisterGlobalProperty(declaration, pointer);

	if (err<0)
	{
		const char* err_str = "unknown error";		

		switch (err)
		{
			case asINVALID_DECLARATION: err_str = "The declaration has invalid syntax.";
			break;
			case asINVALID_TYPE: err_str = "The declaration is a reference.";
			break;
			case asNAME_TAKEN: err_str = "The name is already taken.";
			break;		
		}

		core->TraceTo("Script", "Error in RegisterGlobalProperty, decl - %s^ %s", declaration,err_str);
	}	
}

void ScriptMachine::RegisterGlobalFunction(const char* declaration, const asSFuncPtr& funcPointer, dword callConv)
{
	int err = machine->RegisterGlobalFunction(declaration, funcPointer, callConv);

	if (err<0)
	{
		const char* err_str = "unknown error";		

		switch (err)
		{
			case asNOT_SUPPORTED: err_str = "The calling convention is not supported.";
			break;
			case asWRONG_CALLING_CONV: err_str = "The function's calling convention doesn't match callConv.";
			break;
			case asINVALID_DECLARATION: err_str = "The function declaration is invalid.";
			break;		
			case asNAME_TAKEN: err_str = "The function name is already used elsewhere.";
			break;
		}

		core->TraceTo("Script", "Error in RegisterGlobalFunction, decl - %s: %s", declaration,err_str);
	}	
}

int ScriptMachine::GetTypeIdByDecl(const char* decl)
{
	return machine->GetTypeIdByDecl(decl);
}

asIScriptContext* ScriptMachine::GrabContext(const char* modul_name)
{
	if (cur_context == 5)
	{
		core->TraceTo("ScriptErr", "Error: number of free context exceded");
		return null;
	}

	if (!ctxs[cur_context])
	{
		ctxs[cur_context] = machine->CreateContext();

		if( ctxs[cur_context] == 0 )
		{
			core->TraceTo("ScriptErr", "Error: Failed to create the context");
			return null;
		}
		else
		{			
			ctxs[cur_context]->SetExceptionCallback(asMETHOD(ScriptService, Exceptioncallback), core->Script(), asCALL_THISCALL);			
		}
	}

	ctxs[cur_context]->SetUserData((void*)modul_name);

	cur_context++;

	return ctxs[cur_context - 1];
}

void ScriptMachine::FreeContext()
{
	cur_context--;
}

void ScriptMachine::Release()
{
	for (int i=0;i<5;i++)
	{
		RELEASE(ctxs[i]);
	}

	machine->Release();
	delete this;
}
#if defined(PC) || defined(OSUNIX)
void ScriptMachine::GetMembers(const std::vector<string>& vPath,std::vector<string>& vMembers)
{
    if(!vPath.empty())
    {
        int index = machine->GetGlobalPropertyIndexByName(vPath[0].c_str());
        const char* sMemberName;const char* sNameSpace;int nTypeId;
        machine->GetGlobalPropertyByIndex(index,&sMemberName,&sNameSpace,&nTypeId);
        asITypeInfo* pType = GetTypeById(nTypeId);
        if(!pType)
           pType = GetTypeByType(vPath[0]);
        if(vPath.size() > 1  && pType)
        {
            asITypeInfo* pParent = pType;
            pType = null;
            for(unsigned j = 1;j < vPath.size();++j)
            {
                int nPos = vPath[j].find("(");
                if(nPos != string::npos)
                {
                    string sMethod;
                    sMethod.assign(&vPath[j][0],nPos);
                    std::vector<string> vSignaturesBuff;
                    for(unsigned i = 0;i < pParent->GetMethodCount();++i)
                    {
                        asIScriptFunction *pMethod =  pParent->GetMethodByIndex(i);
                        if(String::IsEqual(sMethod.c_str(),pMethod->GetName()))
                            vSignaturesBuff.push_back(pMethod->GetDeclaration(false,false,true));
                    }
                    pType = GetReturnType(vPath[j],vSignaturesBuff);
                }
                else
                    for(unsigned i = 0;i < pParent->GetPropertyCount();++i)
                    {
                        pParent->GetProperty(i,&sMemberName,&nTypeId);
                        if(String::IsEqual(sMemberName,vPath[j].c_str()))
                            pType = GetTypeById(nTypeId);
                    }
                if(!pType)break;
                pParent = pType;
            }
        }
        GetMembers(pType,vMembers);
    }
}

void ScriptMachine::GetMembers(const string& sObjectType,std::vector<string>& vMembers)
{
    GetMembers(GetTypeByType(sObjectType),vMembers);
}

asITypeInfo* ScriptMachine::GetTypeByType(const string& sObjectType)
{
    asITypeInfo* pType = null;
    for(unsigned i = 0;i < machine->GetObjectTypeCount();++i)
    {
        asITypeInfo* pBuff = machine->GetObjectTypeByIndex(i);
        if(pBuff->GetName() == sObjectType)
        {
            pType = pBuff; 
            break;
        }
    }
    return pType;
}

void ScriptMachine::GetMembers(asITypeInfo* pType,std::vector<string>& vMembers)
{
    if(pType)
    {
        for(unsigned i = 0;i < pType->GetPropertyCount();++i)
        {
            const char* sMemberName;
            pType->GetProperty(i,&sMemberName);
            vMembers.push_back(sMemberName);
        }
        for(unsigned i = 0;i < pType->GetMethodCount();++i)
        {
            asIScriptFunction *pMethod =  pType->GetMethodByIndex(i);
            vMembers.push_back(pMethod->GetName());
        }
    }
}

asITypeInfo* ScriptMachine::GetTypeById(int id)
{
    asITypeInfo* pType = null;
    for(unsigned i = 0;i < machine->GetObjectTypeCount();++i)
    {
        asITypeInfo* pBuff = machine->GetObjectTypeByIndex(i);
        if(pBuff->GetTypeId() == id)
        {
            pType = pBuff; 
            break;
        }
    }
    return pType;
}

void ScriptMachine::GetSignature(const std::vector<string>& vPath,std::vector<string>& vSignatures)
{
    if(!vPath.empty())
    {
        int index = machine->GetGlobalPropertyIndexByName(vPath[0].c_str());
        const char* sMemberName;const char* sNameSpace;int nTypeId;
        machine->GetGlobalPropertyByIndex(index,&sMemberName,&sNameSpace,&nTypeId);
        asITypeInfo* pType = GetTypeById(nTypeId);
        if(!pType)
            pType = GetTypeByType(vPath[0]);
        if(vPath.size() > 2  && pType)
        {
            asITypeInfo* pParent = pType;
            pType = null;
            for(unsigned j = 1;j < vPath.size()  - 1;++j)
            {
                int nPos = vPath[j].find("(");
                if(nPos != string::npos)
                {
                    string sMethod;
                    sMethod.assign(&vPath[j][0],nPos);
                    std::vector<string> vSignaturesBuff;
                    for(unsigned i = 0;i < pParent->GetMethodCount();++i)
                    {
                        asIScriptFunction *pMethod =  pParent->GetMethodByIndex(i);
                        if(String::IsEqual(sMethod.c_str(),pMethod->GetName()))
                            vSignaturesBuff.push_back(pMethod->GetDeclaration(false,false,true));
                    }
                    pType = GetReturnType(vPath[j],vSignaturesBuff);
                }
                else
                    for(unsigned i = 0;i < pParent->GetPropertyCount();++i)
                    {
                        pParent->GetProperty(i,&sMemberName,&nTypeId);
                        if(String::IsEqual(sMemberName,vPath[j].c_str()))
                            pType = GetTypeById(nTypeId);
                    }
                if(!pType)break;
                pParent = pType;
            }
        }
        GetSignature(pType,vPath.back(),vSignatures);
    }
}

asITypeInfo* ScriptMachine::GetReturnType(const string& sMethod,std::vector<string>& vSignatures)
{
    string sSignature;
    unsigned paramsCount = 0;
    for(unsigned i = sMethod.find("(",0) + 1;i < sMethod.size() - 1;++i)
    {
        if(paramsCount == 0 && (sMethod[i] != ' ' || sMethod[i] != '\t'))paramsCount = 1;
        else
            if(sMethod[i] == ',')++paramsCount;
    }
    if(vSignatures.size() > 0)
        for(unsigned i = 0; i < vSignatures.size();++i)
        {
            unsigned params = 0;
            int pos = vSignatures[i].find("(",0);
            for(unsigned j = pos;j < vSignatures[i].size() - 1;++j)
            {
                if(params == 0 && (vSignatures[i][j] != ' ' || vSignatures[i][j] != '\t'))params = 1;
                else
                    if(vSignatures[i][j] == ',')++params;
            }
            if(params == paramsCount)
            {
                sSignature = vSignatures[i];
                break;
            }
        }
    if(sSignature.empty())return NULL;
    else
    {
        string sRetClass;
        for(unsigned i = 0;i < sSignature.size();++i)
        {
            char ch = sSignature[i];
            if(isalpha(ch) || isdigit(ch) || ch == '_')sRetClass += ch;
            else break;
        }
        return GetTypeByType(sRetClass);
    }
}

void ScriptMachine::GetSignature(const string& sObjectType,const string& sMethod,std::vector<string>& vSignatures)
{
    GetSignature(GetTypeByType(sObjectType),sMethod,vSignatures);
}

void ScriptMachine::GetSignature(asITypeInfo* pType,const string& sMethod,std::vector<string>& vSignatures)
{
    if(pType)
    {
        for(unsigned i = 0;i < pType->GetMethodCount();++i)
        {
            asIScriptFunction *pMethod =  pType->GetMethodByIndex(i);
            if(String::IsEqual(sMethod.c_str(),pMethod->GetName()))
                vSignatures.push_back(pMethod->GetDeclaration(false,false,true));
        }
    }
}
#endif