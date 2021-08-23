#include "ScriptModule.h"
#include "Core/Core.h"
#include "ScriptService.h"

ScriptModule* ScriptModule::cur_exec_module = null;

void ScriptModuleExceptioncallback(asIScriptContext *context)  
{  	
	((ScriptService*)(core->Script()))->Exceptioncallback(context);	
}


/*
void LineCallback(asIScriptContext *ctx, DWORD *timeOut)
{
	// If the time out is reached we abort the script
	if( *timeOut < timeGetTime() )
		ctx->Abort();

	// It would also be possible to only suspend the script,
	// instead of aborting it. That would allow the application
	// to resume the execution where it left of at a later 
	// time, by simply calling Execute() again.
}*/

ScriptFunction::ScriptFunction()
{
	sf = 0;
	module = 0;
	object = 0;
}

void ScriptFunction::Set(asIScriptFunction* set_sf, void* set_object)
{	
	if (sf)
	{
		sf->Release();
	}
	
	object = null;

	sf = set_sf;
	if (sf)
	{
		sf->AddRef();
		module = sf->GetModule();

		object = set_object;

		if( sf->GetFuncType() == asFUNC_DELEGATE )
		{			
			module = sf->GetDelegateFunction()->GetModule();
		}
	}
}

asIScriptFunction* ScriptFunction::Get()
{
	return sf;
}

void* ScriptFunction::GetObject()
{
	return object;
}

void ScriptFunction::Reset()
{
	sf = 0;
	module = 0;
}

void ScriptFunction::Find(IScriptFunction* sf, IScriptModule* module, const char* func)
{
	if (!func[0]) return;

	char str[512];
	String::Printf(str, 512, "void %s()", func);	

	module->Find(sf, str);
}

void ScriptFunction::Find1F(IScriptFunction* sf, IScriptModule* module, const char* func)
{
	if (!func[0]) return;

	char str[512];
	String::Printf(str, 512, "void %s(float)", func);	

	module->Find(sf, str);
}

void ScriptFunction::Find1S(IScriptFunction* sf, IScriptModule* module, const char* func)
{
	if (!func[0]) return;

	char str[512];
	String::Printf(str, 512, "void %s(string&in)", func);	

	module->Find(sf, str);
}

void ScriptFunction::Find1P(IScriptFunction* sf, IScriptModule* module, const char* func, const char* obj)
{
	if (!func[0]) return;

	char str[512];
	String::Printf(str, 512, "void %s(%s@)", func, obj);	

	module->Find(sf, str);
}

void ScriptFunction::Find2PI(IScriptFunction* sf, IScriptModule* module, const char* func, const char* obj)
{
	if (!func[0]) return;

	char str[512];
	String::Printf(str, 512, "void %s(%s@, int)", func, obj);	

	module->Find(sf, str);
}

void ScriptFunction::Find2PF(IScriptFunction* sf, IScriptModule* module, const char* func, const char* obj)
{
	if (!func[0]) return;

	char str[512];
	String::Printf(str, 512, "void %s(%s@, float)", func, obj);	

	module->Find(sf, str);
}

void ScriptFunction::Find2PS(IScriptFunction* sf, IScriptModule* module, const char* func, const char* obj)
{
	if (!func[0]) return;

	char str[512];
	String::Printf(str, 512, "void %s(%s@, string&in)", func, obj);		

	module->Find(sf, str);
}

void ScriptFunction::Find3PFF(IScriptFunction* sf, IScriptModule* module, const char* func, const char* obj)
{
	if (!func[0]) return;

	char str[512];
	String::Printf(str, 512, "void %s(%s@, float, float)", func, obj);	

	module->Find(sf, str);
}

void ScriptFunction::Find4PIFF(IScriptFunction* sf, IScriptModule* module, const char* func, const char* obj)
{
	if (!func[0]) return;

	char str[512];
	String::Printf(str, 512, "void %s(%s@, int, float, float)", func, obj);

	module->Find(sf, str);
}

#if defined(PC) || defined(OSUNIX)

#include "Editor/Editor.h"

ScriptModule::BytecodeStreamSaver::BytecodeStreamSaver(IFile* fp)
{
	f = fp;
}

void ScriptModule::BytecodeStreamSaver::Write(const void *ptr, asUINT size) 
{
	if( size == 0 ) return; 
	f->Write(ptr, size); 
}

void ScriptModule::BytecodeStreamSaver::Read(void *ptr, asUINT size) 
{
}

#endif

ScriptModule::BytecodeStreamLoader::BytecodeStreamLoader(byte* ptr)
{
	buffer = ptr;
}

void ScriptModule::BytecodeStreamLoader::Write(const void *ptr, asUINT size)
{

}

void ScriptModule::BytecodeStreamLoader::Read(void *ptr, asUINT size)
{
	memcpy(ptr, buffer, size);
	buffer += size;

	//core->Trace("Read bc %i", size);
}
#if defined(PC) || defined(OSUNIX)
ScriptModule::ScriptModule(ScriptMachine* _machine):vDefines(1000)
{
	machine = _machine;
	sScriptName[0] = 0;	
}
#else
ScriptModule::ScriptModule(ScriptMachine* _machine)
{
    machine = _machine;
    sScriptName[0] = 0;
}
#endif
void ScriptModule::Release()
{    
    if (module)
	{
        machine->Machine()->DiscardModule(sScriptName);
	}

#if defined(PC) || defined(OSUNIX)
    vDefines.Clear();
#endif
    delete this;

}

#if defined(PC) || defined(OSUNIX)
bool ScriptModule::BuildScript(const char *sSectionName,EBuildStep eStep)
{
     vIncludedScriptsNames.clear();
     string sText;string include = "system_declations";
     if(LoadIncludeFile(include,sText))
     {
         if(BuildScript(include.c_str(),sText,eStep))
         {
             sText.clear();
             if(LoadScript(sSectionName,sText))
             {
                return BuildScript(sSectionName,sText,eStep);
             }
         }
     }
     return false;
}
#endif
bool ScriptModule::Load(const char* name)
{	
	String::Copy(sScriptName, 256, name);
	module = machine->Machine()->GetModule(sScriptName, asGM_ALWAYS_CREATE);
	
	int ret = 0;
	if (!core->Script()->UsePrecompiledByteCode())
	{
#if defined(PC) || defined(OSUNIX)
        ret = -1;
        if(BuildScript(name,ePreProcessor))
        {
            if(BuildScript(name,eBuild))
            {
                ScriptModule::cur_exec_module = this;
                ret = 0;
            }
        }
#endif
	}
	else
	{
		const char* filename = en_core.EN_Scene()->GetSceneFileName(name);

        if (!filename) return false;

		char fl_name[256]; 
        String::Copy(fl_name, 256, filename);

        int len = strlen(fl_name);
		fl_name[len - 3] = 's';
		fl_name[len - 2] = 'r';
		fl_name[len - 1] = 't';
	
		IFileBuffer* data = core->Files()->LoadFile(fl_name);

		BytecodeStreamLoader straem(data->GetBuffer());
		int ret = module->LoadByteCode(&straem);		

		data->Release();	
	}

	if( ret < 0 )return false;
	else
	{
		if (!core->Script()->UsePrecompiledByteCode())
			ret = module->Build();
		if( ret < 0 )
		{			
			core->TraceTo("ScriptErr", "Build() failed");
			return false;
		}		
	}
	return true;
}

bool ScriptModule::Find(IScriptFunction* sf, const char* desc)
{
	asIScriptFunction* script_func = module->GetFunctionByDecl(desc);

	if (!script_func)
	{
		core->TraceTo("ScriptErr", "Warring in '%s': The function '%s' was not found.", sScriptName, desc);
		return false;
	}

	((ScriptFunction*)sf)->Set(script_func);
	
	return true;
}

void ScriptModule::BindImportFunctions()
{
	if (module)
	{
		int ret = module->BindAllImportedFunctions();
	}
}

#if defined(PC) || defined(OSUNIX)

void ScriptModule::SaveByteCode(const char* name)
{
    IFile* file = core->Files()->OpenFile(name, IFile::write);

    if (file)
    {
        BytecodeStreamSaver stream(file);

        module->SaveByteCode(&stream);

        file->Release();
    }
}

void ScriptModule::OverwriteCode(char *code, int len)
{
    // Overwrite the preprocessor code with space characters to avoid compiler error
    for( int n = 0; n < len; n++ )
    {
        if( *code != '\n' && *code != '\r')
            *code = ' ';
        code++;
    }
}

bool ScriptModule::LoadScript(const char* name,string& out)
{
	const char* filename =  ScriptService::GetScriptFilePath(name);
	IFileBuffer* data = core->Files()->LoadFile(filename);
	if (!data)
    {
        core->TraceTo("ScriptErr", "LoadScript: Wrong script file name %s ",filename);
		return false;
    }

	if (data->GetSize() > 0)
	{
        out = (const char*)data->GetBuffer();
		data->Release();
		return true;
	}

	return true;	
}

int ScriptModule::CalculateLines(const string& sText)
{
    unsigned pos = 0;
    int lines = 0;
    while( pos < sText.size() )
    {
        if(sText[pos] == 10 || sText[pos] == '\n')
            ++lines;
        ++pos;
    }
    return lines;
}

asETokenClass ScriptModule::ParseToken(const string& sScriptText,unsigned& pos,asUINT* len)
{
    asETokenClass t = machine->Machine()->ParseToken(&sScriptText[pos],sScriptText.size() - pos, len);
    if( t == asTC_WHITESPACE )
    {
        pos += *len;
        t = machine->Machine()->ParseToken(&sScriptText[pos], sScriptText.size() - pos, len);
    }
    return t;
}


//////////////////////////////////////////////////////////////////////////
// Includes
//////////////////////////////////////////////////////////////////////////
bool ScriptModule::LoadIncludeFile(string& includefile,string& out)
{
    if( IncludeIfNotAlreadyIncluded(includefile.c_str()) )
          return LoadScript(includefile.c_str(),out);
    return true;
}

bool ScriptModule::IncludeIfNotAlreadyIncluded(const char *name)
{
    string scriptFile = name;
    if( vIncludedScriptsNames.find(scriptFile) != vIncludedScriptsNames.end() )
    {
        // Already included
        return false;
    }
    // Add the file to the set of included sections
    vIncludedScriptsNames.insert(scriptFile);
    return true;
}

bool  ScriptModule::BuildScript(const char *sSectionName,std::string& sScriptText,EBuildStep eStep)
{
    unsigned pos = 0;
    bool bRet  = true;
    while( pos < sScriptText.size() )
    {
        asUINT len;
        asETokenClass t = machine->Machine()->ParseToken(&sScriptText[pos], sScriptText.size() - pos, &len);
        if( t == asTC_COMMENT || t == asTC_WHITESPACE )
        {
            pos += len;
            continue;
        }
        if( sScriptText[pos] == '#' )// Is this a preprocessor directive?
        {
            int start = pos++;
            asETokenClass t = machine->Machine()->ParseToken(&sScriptText[pos], sScriptText.size() - pos, &len);
            if (t == asTC_IDENTIFIER)
            {
                string token;
                token.assign(&sScriptText[pos], len);
                if( token == "include" )
                {
                    pos += len;
                    asETokenClass t = machine->Machine()->ParseToken(&sScriptText[pos], sScriptText.size() - pos,&len);
                    if( t == asTC_WHITESPACE )
                    {
                        pos += len;
                        t = machine->Machine()->ParseToken(&sScriptText[pos], sScriptText.size() - pos, &len);
                    }
                    if( t == asTC_VALUE && len > 2 && (sScriptText[pos] == '"' || sScriptText[pos] == '\'') )
                    {
                        // Get the include file
                        string sIncludeName;
                        sIncludeName.assign(&sScriptText[pos+1], len-2);
                        pos += len;
                        OverwriteCode(&sScriptText[start], pos-start);
                        string sIncludeText;
                        if(LoadIncludeFile(sIncludeName,sIncludeText))
                        {
                            if(!sIncludeText.empty())
                            {
                               if(!BuildScript(sIncludeName.c_str(),sIncludeText,eStep))
                                   return false;
                            }
                        }
                        else 
                            return false;
                    }
                }
              
            }
            else pos += len;
        }
        else pos += len;
    }
    // First perform the checks for #if directives to exclude code that shouldn't be compiled
    ProcessDefines(sSectionName,sScriptText,eStep);
    if(eStep == eBuild)
    {
        // Build the actual script
        machine->Machine()->SetEngineProperty(asEP_COPY_SCRIPT_SECTIONS, true);
        module->AddScriptSection(sSectionName, sScriptText.c_str(), sScriptText.size());
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////
// Defines
//////////////////////////////////////////////////////////////////////////
void ScriptModule::Define::Replace(ScriptMachine*  machine,std::vector<string>& params,string& out)
{
    out = sValue;
    if(params.size() == vParams.size())
    {
        int  pos = 0;
        while( pos < (int)out.size() )
        {
            asUINT len;
            asETokenClass t = machine->Machine()->ParseToken(&out[pos], out.size() - pos, &len);
            if(t == asTC_IDENTIFIER)
            {
                string word;
                word.assign(&out[pos],len);
                unsigned index = 0;
                for(;index < vParams.size();index++)
                    if(word == vParams[index])break;
                if(index < params.size())
                {
                    out.erase(pos,len);
                    out = out.insert(pos,params[index]);
                    pos += params[index].size();
                }
                else
                    pos += len;
            }
            else
                pos += len;
        }
    }
}

void ScriptModule::DeletePreProcessorBlock(string& sScriptText,unsigned& pos)
{
    int start = pos;
    int stack = 0;
    while( pos < (int)sScriptText.size() )
    {
        asUINT len;
        asETokenClass t = machine->Machine()->ParseToken(&sScriptText[pos], sScriptText.size() - pos, &len);
       	if( t == asTC_UNKNOWN && sScriptText[pos] == '#' )
		{
            ++pos;
            asETokenClass t = machine->Machine()->ParseToken(&sScriptText[pos], sScriptText.size() - pos, &len);
            string token;
            token.assign(&sScriptText[pos], len);
            pos += len;
            if(token == "ifdef" || token == "ifndef")++stack;
            else
               if(token == "endif" || token == "elseif")
               {
                   if(stack)--stack;
                   else
                   {
                       OverwriteCode(&sScriptText[start], pos-start);
                       return;
                   }
               }
        }
        else
            pos += len;
    }
}

void ScriptModule::GetDefineParams(const string& sScriptText,unsigned& pos,std::vector<string>& vParams)
{ 
    asUINT len;
    asETokenClass t = ParseToken(sScriptText,pos,&len);
    if( t == asTC_KEYWORD && sScriptText[pos] == '(')
    {
        pos += len;
        int start = pos;int brackets = 0;
        while( pos < (int)sScriptText.size())
        {
            t = ParseToken(sScriptText,pos,&len);
            if( t == asTC_KEYWORD )
            {
                if(sScriptText[pos] == '(')
                {
                    ++brackets;
                    pos += len;
                }
                else
                    if(sScriptText[pos] == ',')
                    {
                        vParams.push_back("");
                        vParams.back().assign(&sScriptText[start], pos - start);
                        pos += len;
                        start = pos;
                    }
                    else
                        if(sScriptText[pos] == ')')
                        {
                            if(brackets == 0)
                            {
                                vParams.push_back("");
                                vParams.back().assign(&sScriptText[start], pos - start);
                                pos += len;
                                break;
                            }
                            else
                            {
                              pos += len;
                            --brackets;
                            }
                        }
                        else 
                            pos += len;
            }
            else 
                pos += len;
        }
    }
}

void ScriptModule::ParseDefines(string& sScriptText,unsigned& pos,bool& bPreProcessorIF,EBuildStep eStep)
{
    asUINT len = 0;
    int start = pos++;
    asETokenClass t = machine->Machine()->ParseToken(&sScriptText[pos], sScriptText.size() - pos, &len);
    string token;
    token.assign(&sScriptText[pos], len);
    if( token == "define" )
    {
        pos += len;
        t = ParseToken(sScriptText,pos,&len);
        string sName,sValue;
        std::vector<string> vParams;
        if( t == asTC_IDENTIFIER )
        {
            sName.assign(&sScriptText[pos], len);
            pos += len;
            if(sScriptText[pos] != 13 && sScriptText[pos] != 10 && sScriptText[pos] != '\n')
            {
                GetDefineParams(sScriptText,pos,vParams);
                while( pos < (int)sScriptText.size() && sScriptText[pos] != 13 && sScriptText[pos] != 10 && sScriptText[pos] != '\n')
                {
                    if(sScriptText[pos] == '\\')
                    {
                        ++pos;
                        while( pos < (int)sScriptText.size() && (sScriptText[pos] == 13 || sScriptText[pos] == 10 || sScriptText[pos] == '\n'))++pos;
                    }
                    sValue += sScriptText[pos];
                    ++pos;
                }
            }
        }
        if(!sName.empty())
        {
            OverwriteCode(&sScriptText[start], pos-start);
            Define& rDefine = vDefines[sName];
            if(!vParams.empty())
                rDefine.vParams = vParams;
            rDefine.sValue.assign(sValue.c_str(),sValue.size());
        }
    }
    else
    {
        if(token == "ifdef" || token == "ifndef")
        {
            pos += len;
            t = ParseToken(sScriptText,pos,&len);
            if( t == asTC_IDENTIFIER )
            {
                string sName;
                sName.assign(&sScriptText[pos], len);
                pos += len;
                OverwriteCode(&sScriptText[start], pos-start);
                start = pos;
                bool operation =  vDefines.Find(sName) == null;
                if(token == "ifndef")operation = !operation;
                if(operation)
                    DeletePreProcessorBlock(sScriptText,pos);
                bPreProcessorIF = true;
            }
        }
        else
        {
            if(token == "elseif")
            {
                pos += len;
                if(bPreProcessorIF)
                {
                    bPreProcessorIF = false;
                    OverwriteCode(&sScriptText[start], pos-start);
                    DeletePreProcessorBlock(sScriptText,pos);
                }
            }
            else
            {
                if(token == "endif")
                {
                    pos += len;
                    if(bPreProcessorIF)
                    {
                        bPreProcessorIF = false;
                        OverwriteCode(&sScriptText[start], pos-start);
                    }
                }
                else pos += len;
            }
        }

    }
}

void ScriptModule::ReplaceDefineParams(string& sScriptText,Define* pDefine,unsigned& pos,asUINT len)
{
    std::vector<string> vParams;
    int start = pos;
    pos += len;
    if(!pDefine->sValue.empty())
    {
        GetDefineParams(sScriptText,pos,vParams);
        if(vParams.size() == pDefine->vParams.size() )
        {
            string sValue;
            if(!vParams.empty())
            {
                len = pos - start;
                pDefine->Replace(machine,vParams,sValue);
            }
            else
                sValue = pDefine->sValue;
            sScriptText.erase(start,len);
            sScriptText = sScriptText.insert(start,sValue);
            pos = start;
            pos += sValue.size();
        }
    }
    else
    {
        GetDefineParams(sScriptText,pos,vParams);
        if(!vParams.empty())
            len = pos - start;
        sScriptText.erase(start,len);
        pos = start;

    }
}

void ScriptModule::ReplaceString(string& sScriptText,const string& sText,unsigned& pos,asUINT len)
{
     sScriptText.erase(pos,len);
     sScriptText = sScriptText.insert(pos,sText);
     pos += sText.size();
}

void ScriptModule::ProcessDefines(const char* sScriptName,string& sScriptText,EBuildStep eStep)
{
    unsigned  nPos = 0;
    bool bPreProcessorIF = false;
    int nLine = 0;
    while( nPos < (int)sScriptText.size() )
    {
        asUINT len;
        asETokenClass t = machine->Machine()->ParseToken(&sScriptText[nPos], sScriptText.size() - nPos, &len);
        switch(t)
        {
        case asTC_UNKNOWN:
            {
                if(sScriptText[nPos] == '#' )ParseDefines(sScriptText,nPos,bPreProcessorIF,eStep);
                else nPos += len;
                break;
            }
        case asTC_IDENTIFIER:
            {
                if(eStep == eBuild)
                {
                    string word;
                    word.assign(&sScriptText[nPos],len);
                    Define* pDefine = vDefines.Find(word);
                    if(pDefine)ReplaceDefineParams(sScriptText,pDefine,nPos,len);
                    else
                    {
                        if(word == "__FILE__")
                        {
                            ReplaceString(sScriptText,"\"" + string(sScriptName) + "\"",nPos,len);
                            break;
                        }
                        else
                            if(word == "__LINE__")
                            {
                                char vBuffer[128];
                                memset(vBuffer,0,128);
                                String::Printf(vBuffer,128,"%d",nLine + 1);
                                ReplaceString(sScriptText,"\"" + string(vBuffer) + "\"",nPos,len);
                            }
                            else
                                if(word == "__FUNCTION__")
                                {
                                    string sName;
                                    core->Script()->GetFunctionName(sName,nLine,sScriptName);
                                    ReplaceString(sScriptText,"\"" + sName + "\"",nPos,len);
                                }
                                else  nPos += len;
                    }
                }
                else  nPos += len;
                break;
            }
        case asTC_COMMENT:
        case asTC_WHITESPACE:
            {
                 for(unsigned i = nPos;i < nPos + len;++i)
                    if(sScriptText[i] == 10 || sScriptText[i] == '\n')
                        ++nLine;
                nPos += len;
                break;
            }
        default:nPos += len;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// Script Parser
//////////////////////////////////////////////////////////////////////////
void ScriptModule::AddCodeToNameSpaces(const char* name)
{
	string sText;
	if(LoadScript(name,sText))
	{
		BuildNameData(name,sText);
		core->TraceTo("Status","Parse script ends.");
	}
};
void ScriptModule::ParseCode(const char* name)
{
    string sText;
    if(LoadScript(name,sText))
    {
        string include = "system_declations";
        string sIncludeText;
        string sSpaceName = include  +"_Global";
        if(LoadScript(include.c_str(),sIncludeText) && !core->Script()->GetNameSpace(sSpaceName.c_str()))
            BuildNameData(include.c_str(),sIncludeText);
         BuildNameData(name,sText);
         core->TraceTo("Status","Parse script ends.");
    }

}

void ScriptModule::AddReference(std::vector<string>& vWordsStack,ScriptCodeParserData::ScriptNameSpace* pSpace,int nPos)
{
    if(vWordsStack.size() > 1 && pSpace)
    {
        string name = vWordsStack.back();
        vWordsStack.pop_back();
        pSpace->AddReference(name,vWordsStack,nPos);
    }
}

void ScriptModule::AddEnumReference(ScriptCodeParserData::ScriptNameSpace* pLastNameSpace,std::vector<string>& vWordsStack,string sName,int pos)
{
    vWordsStack.clear();
    vWordsStack.push_back("int");
    vWordsStack.push_back(sName);
    AddReference(vWordsStack,pLastNameSpace,pos);
}
void ScriptModule::BuildNameData(const char* sScriptName,const string& sText,bool bParseIncludes)
{
    unsigned  nPos = 0;unsigned nWordsStackDepth = 6;
    std::vector<ParserStates> vStack;
    std::vector<string> vWordsStack;
    vStack.push_back(eGlobalState);
    ScriptCodeParserData::ScriptNameSpace* pLastNameSpace = new ScriptCodeParserData::ScriptNameSpace(sScriptName,string(sScriptName) + "_Global",ScriptCodeParserData::ScriptNameSpace::eFile,0,0); 
    std::vector<ScriptCodeParserData::ScriptNameSpace*> vNameSpacesStack;
    int nLine = 0;
    string sSignature;string sFunctionName;
    int  nBracketsCount = 0;
    while( nPos < (int)sText.size() )
    {
        asUINT len;
        if( sText[nPos] == '#' )// Is this a preprocessor directive?
        {
            int start = nPos++;
            asETokenClass t = machine->Machine()->ParseToken(&sText[nPos], sText.size() - nPos, &len);
            if (t == asTC_IDENTIFIER)
            {
                string token;
                token.assign(&sText[nPos], len);
                if( token == "include" )
                {
                    nPos += len;
                    asETokenClass t = machine->Machine()->ParseToken(&sText[nPos], sText.size() - nPos,&len);
                    if( t == asTC_WHITESPACE )
                    {
                        nPos += len;
                        t = machine->Machine()->ParseToken(&sText[nPos], sText.size() - nPos, &len);
                    }
                    if( t == asTC_VALUE && len > 2 && (sText[nPos] == '"' || sText[nPos] == '\'') )
                    {
                        // Get the include file
                        string sIncludeFile,sIncludeText;
                        sIncludeFile.assign(&sText[nPos+1], len-2);
                        nPos += len;
						if(!bParseIncludes)
							bParseIncludes = en_core.EN_Scene()->GetSceneFileName(sIncludeFile.c_str()) == NULL;
					
                        if(bParseIncludes)
                        {
                            if(LoadIncludeFile(sIncludeFile,sIncludeText))
                                if(!sIncludeText.empty())
									BuildNameData(sIncludeFile.c_str(),sIncludeText);
						}
                    }
                    else 
                        nPos += len;
                }
                else
                {
                    while( nPos < (int)sText.size() && sText[nPos] != 13 && sText[nPos] != 10 && sText[nPos] != '\n')
                    {
                        if(sText[nPos] == '\\')
                        {
                            ++nPos;
                            while( nPos < (int)sText.size() && (sText[nPos] == 13 || sText[nPos] == 10 || sText[nPos] == '\n'))++nPos;
                        }
                        ++nPos;
                    }
                }

            }
            else 
                nPos += len;
        }
        asETokenClass t = machine->Machine()->ParseToken(&sText[nPos], sText.size() - nPos, &len);
        string word;
        word.assign(&sText[nPos],len);
        switch(t)
        {
        case asTC_KEYWORD:
            {
                if((vStack.back() == eFunctionDeclarationState || vStack.back() == ePrivateFunctionDeclarationState) &&  sText[nPos] != '{')sSignature += word;
                if(word.size() == 1)
                {
                    switch(word[0])
                    {
                        case '}':
                            {
                                if(vStack.back() == eValueState &&  vStack.size() > 1 && vStack[vStack.size() - 2] == eEnumBodyState)PopParserState(vStack);
                                switch(vStack.back())
                                {
                                    case eEnumBodyState:
                                        {
                                            if(vWordsStack.size() > 0)
                                                AddEnumReference(pLastNameSpace,vWordsStack,vWordsStack.back(),nPos);
                                        }
                                    case eBracketsBodyState:
                                    case eFunctionBodyState:
                                    case eClassBodyState:
                                        {
                                            if(pLastNameSpace)
                                            {
                                                if(!pLastNameSpace->IsEmpty())
                                                {
													pLastNameSpace->SetEnd(nLine + 1,nPos);
													core->Script()->AddNameSpace(pLastNameSpace,sScriptName);
													#ifdef EDITOR													
													//FIX ME!!!  Crash
                                                    //core->TraceTo("Status","Parse file %s: end scan class %s ",sScriptName,pLastNameSpace->Name().c_str());
													#endif
                                                }
                                                else
                                                {
                                                    delete pLastNameSpace;
                                                    pLastNameSpace = NULL;
                                                }
                                            }
                                            else
											{
                                                core->TraceTo("ScriptErr", "Script Parser error in file: %s -> line: %d ",sScriptName,nLine);
											}

                                            if (!vNameSpacesStack.empty())
											{
                                                pLastNameSpace = vNameSpacesStack.back();
											}

                                            if (vNameSpacesStack.size() > 1)
											{
                                                vNameSpacesStack.pop_back();
											}

                                            break;
                                        }
                                }
                                if(vStack.back() != eValueState &&  vStack.back() != eParserPauseState)PopParserState(vStack);
                                break;
                            }
                        case '{':
                            {
                                switch(vStack.back())
                                {
                                    case eClassBodyState:
                                    case eBracketsBodyState:
                                    case eFunctionBodyState:
                                    case eGlobalState:
                                        { 
                                            if(vStack.back() == eClassBodyState)
                                                   AddReference(vWordsStack,pLastNameSpace,nPos);
                                            string sName = string(sScriptName) + "BracketsSpace";
                                            if(nBracketsCount > 0)
                                            {
                                                static char buff[1024];
												sprintf(buff, "%i", nBracketsCount);
                                                sName += buff;
                                            }
                                            if(pLastNameSpace)
                                                vNameSpacesStack.push_back(pLastNameSpace);
                                            pLastNameSpace = new ScriptCodeParserData::ScriptNameSpace(sScriptName,sName,ScriptCodeParserData::ScriptNameSpace::eBrackets,nLine,nPos);
                                            ++nBracketsCount;
                                            vStack.push_back(eBracketsBodyState);
                                            break;
                                        }
                                    case eEnumDeclarationState:
                                        {
                                            vStack.pop_back();
                                            vStack.push_back(eEnumBodyState);
                                            break;
                                        }
                                    case eNameSpaceDeclaration:
                                    case eClassDeclarationState:
                                        {
                                            vStack.pop_back();
                                            vStack.push_back(eClassBodyState);
                                            break;
                                        }
                                    case eClassParentsDeclarationState:
                                        {
                                            if(vWordsStack.size() > 0)
                                                pLastNameSpace->AddParent(vWordsStack.back());
                                            vStack.pop_back();
                                            vStack.push_back(eClassBodyState);
                                            break;
                                        }
                                    case ePrivateFunctionDeclarationState:
                                    case eFunctionDeclarationState: 
                                        {
                                            vStack.pop_back();
                                            vStack.push_back(eFunctionBodyState);
                                            break;
                                        }
                                }
                                vWordsStack.clear();
                                break;
                            }
                        case ',':
                            {
                                switch(vStack.back())
                                {
                                    case eClassParentsDeclarationState:
                                        {
                                            if(vWordsStack.size() > 0)
                                                pLastNameSpace->AddParent(vWordsStack.back());
                                            break;
                                        }
                                    case eEnumBodyState:
                                        {
                                            AddEnumReference(pLastNameSpace,vWordsStack,vWordsStack.back(),nPos);
                                            break;
                                        }
                                    case eValueState:
                                        {
                                            if(vStack.size() > 1 && vStack[vStack.size() - 2] == eEnumBodyState)PopParserState(vStack);
                                            break;
                                        }
                                    case ePrivateFunctionDeclarationState:
                                    case eFunctionDeclarationState:AddReference(vWordsStack,pLastNameSpace,nPos);break;
                                }
                                vWordsStack.clear();
                                break;
                            }
                        case ':':
                            {
                                switch(vStack.back())
                                {
                                    case eClassDeclarationState:
                                    {
                                        vStack.pop_back();
                                        vStack.push_back(eClassParentsDeclarationState);
                                        break;
                                    }

                                }
                                break;
                            }
                        case '=':
                        case ';':
                            {
                                switch(vStack.back())
                                {
                                case eParserPauseState:break;
                                case eValueState:PopParserState(vStack);break;
                                case ePrivateFunctionDeclarationState:
                                case eFunctionDeclarationState:
                                    {
                                        if(word[0] == '=')
                                            AddReference(vWordsStack,pLastNameSpace,nPos);
                                        else
                                        {
                                            PopParserState(vStack);
                                            if(pLastNameSpace)
                                            {
                                                if(pLastNameSpace->Type() == ScriptCodeParserData::ScriptNameSpace::eFunction)
                                                {
                                                    delete pLastNameSpace;
                                                    pLastNameSpace = NULL;
                                                }
                                                else core->TraceTo("ScriptErr", "Script Parser error in file: %s -> line: %d ",sScriptName,nLine);
                                            }
                                            else
                                                core->TraceTo("ScriptErr", "Script Parser error in file: %s -> line: %d ",sScriptName,nLine);
                                            if(!vNameSpacesStack.empty())
                                                pLastNameSpace = vNameSpacesStack.back();
                                            if(vNameSpacesStack.size() > 1)
                                               vNameSpacesStack.pop_back();
                                        }
                                        break;
                                    }
                                case eEnumBodyState:
                                    {
                                        if(word[0] == '=')
                                        {
                                            AddEnumReference(pLastNameSpace,vWordsStack,vWordsStack.back(),nPos);
                                            vStack.push_back(eValueState);
                                        }
                                        else 
                                            PopParserState(vStack);
                                        break;
                                    }
                                case eGlobalState:
                                case eClassBodyState:
                                case eFunctionBodyState:
                                case eBracketsBodyState:
                                    {
                                        AddReference(vWordsStack,pLastNameSpace,nPos);
                                        if(word[0] == '=')
                                            vStack.push_back(eValueState);
                                        break;
                                    }
                                default:
                                    {
                                        if(word[0] == '=')
                                            vStack.push_back(eValueState);
                                    }
                                }
                                vWordsStack.clear();
                                break;
                            }
                        case ']':
                        case '[':
                        case '.':vWordsStack.clear();break;
                        case '@':break;
                        case '&':
                            {
                                nPos += len;
                                machine->Machine()->ParseToken(&sText[nPos], sText.size() - nPos, &len);
                                word.assign(&sText[nPos],len);
                                if((vStack.back() == eFunctionDeclarationState || vStack.back() == ePrivateFunctionDeclarationState))sSignature += word;
                                break;
                            }
                        case ')':
                            {
                                if(vStack.back() == eParserPauseState)
                                {
                                    vWordsStack.clear();
                                    PopParserState(vStack);
                                    break;
                                }
                                if(!vNameSpacesStack.empty())
                                {
                                    ScriptCodeParserData::ScriptNameSpace* pNameSpace = vNameSpacesStack.back();
                                    switch(vStack.back())
                                    {
                                       case eFunctionDeclarationState:
                                       case ePrivateFunctionDeclarationState:
                                           {
                                               AddReference(vWordsStack,pLastNameSpace,nPos);
                                               if(vStack.back() == ePrivateFunctionDeclarationState)
                                                   pNameSpace->AddPrivateFunction(sFunctionName,sSignature,nPos + 1);
                                               else
                                                   pNameSpace->AddPublicFunction(sFunctionName,sSignature,nPos + 1);
                                               vWordsStack.clear();
                                               break;
                                           }
                                    }
                                }
                                break;
                            }
                        case '(':
                            {
                                switch(vStack.back())
                                {
                                    case eClassBodyState:
                                    case eGlobalState:
                                        {
                                            sSignature.clear();
                                            int state = eFunctionDeclarationState;
                                            unsigned nIter = nPos;
                                            while(nIter >  0 && (sText[nIter - 1] != 10 || sText[nIter - 1] != '\n'))--nIter;
                                            while(nIter < nPos)
                                            {
                                               asUINT len;
                                               asETokenClass t = machine->Machine()->ParseToken(&sText[nIter], sText.size() - nIter, &len);
                                               if( t != asTC_COMMENT && t != asTC_WHITESPACE)break;
                                               else nIter += len;
                                            }            
                                            while(nIter < nPos)
                                            {
                                                asUINT len;
                                                asETokenClass t = machine->Machine()->ParseToken(&sText[nIter], sText.size() - nIter, &len);
                                                string word;
                                                word.assign(&sText[nIter],len);
                                                if(word == "private")state = ePrivateFunctionDeclarationState;
                                                nIter += len;
                                                sSignature += word;
                                            }
                                            sSignature += "(";
                                            if(vWordsStack.size() > 0)
                                            {
                                                sFunctionName = vWordsStack.back();
                                                vStack.push_back((ParserStates)state);
                                                if(pLastNameSpace)
                                                    vNameSpacesStack.push_back(pLastNameSpace);
                                                pLastNameSpace = new ScriptCodeParserData::ScriptNameSpace(sScriptName,sFunctionName,ScriptCodeParserData::ScriptNameSpace::eFunction,nLine,nPos);
												#ifdef EDITOR	
                                                core->TraceTo("Status","Parse file %s: scan function %s::%s ",sScriptName,vNameSpacesStack.back()->Name().c_str(),sFunctionName.c_str());
												#endif
                                            }
                                            vWordsStack.clear();
                                            break;
                                        }
                                    case eValueState:
                                    case eParserPauseState:
                                    case eBracketsBodyState:
                                    case eFunctionBodyState:
                                        {
                                            vStack.push_back(eParserPauseState);
                                            vWordsStack.clear();
                                            break;
                                        }
                                }
                                break;
                            }
                    }
                }
                else
                {
                    if(word == "+=" || word == "-=" ||  word == "/=" ||  word == "*=")
                    {
                        vStack.push_back(eValueState);
                        nPos += len;
                        break;
                    }
                    else
                    {
                        if(word != "++" && word != "--" && word != "is" && word != "!is")
                        {
                            if(word == "class" || word == "interface")
                            {
                                vStack.push_back(eClassDeclarationState);
                            }
                            else
                                if(word == "enum")
                                {
                                    vStack.push_back(eEnumDeclarationState);
                                }
                                else
                                    if(word == "namespace")
                                    {
                                        vStack.push_back(eNameSpaceDeclaration);
                                    }
                                    else
                                        if(word != "return")
                                        {
                                            if(vWordsStack.size() >= nWordsStackDepth)
                                            {
                                                for(unsigned i = 0;i < vWordsStack.size() - 1;++i)vWordsStack[i] = vWordsStack[i + 1];
                                                vWordsStack.pop_back();
                                            }
                                            vWordsStack.push_back(word);
                                        }
                        } 
                        else vWordsStack.clear();
                    }
                }
                nPos += len;
                break;
            }
        case asTC_IDENTIFIER:
            {
                ScriptCodeParserData::ScriptNameSpace::ETypes type = ScriptCodeParserData::ScriptNameSpace::eClass;
                switch(vStack.back())
                {
                        case eFunctionDeclarationState:
                        case ePrivateFunctionDeclarationState:sSignature += word;break;
                        case eNameSpaceDeclaration:type = ScriptCodeParserData::ScriptNameSpace::eNameSpace;
                        case eEnumDeclarationState:
                        case eClassDeclarationState:
                        {
                            ScriptCodeParserData::ScriptNameSpace* pParent = NULL;
                            if(pLastNameSpace)
                            {
                                vNameSpacesStack.push_back(pLastNameSpace);
                                if(pLastNameSpace->Type() == ScriptCodeParserData::ScriptNameSpace::eNameSpace)
                                {
                                    pParent = pLastNameSpace;
                                    string sNewName = pLastNameSpace->Name() +"::" + word;
                                    word.swap(sNewName);
                                }
                            }
                            pLastNameSpace = new ScriptCodeParserData::ScriptNameSpace(sScriptName,word,type,nLine,nPos);
                            if(pParent)
                                pParent->AddClass(pLastNameSpace);
                            core->TraceTo("Status","Parse file %s: scan class %s ",sScriptName,word.c_str());
                            break;
                        }
                }
                if(vWordsStack.size() >= nWordsStackDepth)
                {
                    for(unsigned i = 0;i < vWordsStack.size() - 1;++i)vWordsStack[i] = vWordsStack[i + 1];
                    vWordsStack.pop_back();
                }
                vWordsStack.push_back(word);
                nPos += len;
                break;
            }
        case asTC_COMMENT:
        case asTC_WHITESPACE:
                     {
                     
                         if(vStack.back() == eFunctionDeclarationState || vStack.back() == ePrivateFunctionDeclarationState)sSignature += word;
                         for(unsigned i = nPos;i < nPos + len;++i)
                             if(sText[i] == 10 || sText[i] == '\n')
                             {
                                 switch(vStack.back())
                                 {
                                    case eClassParentsDeclarationState:
                                    case eClassBodyState:
                                    case eEnumBodyState:break;
                                    default:vWordsStack.clear();
                                 }
                                 ++nLine;
                             }
                         nPos += len;
                         break;
                     }
        default:nPos += len;
        }
    }
    pLastNameSpace->SetEnd(nLine + 1,nPos);
    core->Script()->AddNameSpace(pLastNameSpace,sScriptName);
}

void ScriptModule::ReParseCode(const char* name,const string& sText)
{
    BuildNameData(name,sText,false);
    core->TraceTo("Status","Parse script ends.");
}
#endif













