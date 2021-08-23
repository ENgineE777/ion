
#include "ScriptService.h"
#include "ScriptMachine.h"
#include "ScriptModule.h"
#include "Core/Core.h"
#include "Core/Scene/SceneInstance.h"


ScriptService::ScriptService()
{
#ifdef EDITOR
	skip_help = false;
#endif

	allowedExecuteScript = true;
	printException = true;

	exceptions.reserve(8);
}

bool ScriptService::Init()
{
	machine = new ScriptMachine(this);
	

	for (int i=0;i<en_core.EN_Scene()->services.Size();i++)
	{
		en_core.EN_Scene()->services.Get(i)->MethodsToScript();
	}

	SceneInstance* scene = new SceneInstance(en_core.EN_Scene(), "fake");

	for (EntityDedeclarator* decl = EntityDedeclarator::GetFirst(); decl; decl = decl->GetNext())
	{			
		decl->SetCheck(false);

		SceneEntity* entity = scene->AddSceneEntity(decl->GetName());
        
#if defined(PC) || defined(OSUNIX)
        entity->Properties().clear();
#endif
        entity->RegisterProperties();
        entity->EmbedingToScript();
	}

	scene->Release();	

	moduleCore.Init(machine);	

	core->AddTask(this, this, (TaskHolder::Task)&ScriptService::Work, lvl_camera1, -1.f);
    
	return true;
}

bool ScriptService::ReInit()
{
	moduleCore.Reset();

	if (machine) delete machine;
	return Init();
}

IScriptModule* ScriptService::LoadModule(const char* name)
{
	ScriptModule* module = new ScriptModule((ScriptMachine*)machine);
	if (!module->Load(name))
		return NULL;
	return module;
}

bool ScriptService::IsAllowedExecuteScript()
{
	return allowedExecuteScript;
}

void ScriptService::SetAllowExecuteScript(bool set_allow)
{
	allowedExecuteScript = set_allow;
}

bool ScriptService::ExportPrecompiledByteCode()
{
	return true;
}

bool ScriptService::UsePrecompiledByteCode()
{
#if defined(PC) || defined(OSUNIX)
	return false;
#else
	return true;
#endif
}

void ScriptService::SetPrintException(bool set)
{
	printException = set;
}

void ScriptService::Exceptioncallback(asIScriptContext *context)
{
	Exception* excp = null;

	if (exceptions.size() < 4 && printException)
	{
		exceptions.push_back(Exception());
		excp = &exceptions[exceptions.size() - 1];
		excp->fade_time = 5.0f;
	}
	char str[1024];
	// Show the call stack
	for( asUINT n = 0; n < context->GetCallstackSize(); n++ )
	{
		asIScriptFunction *func;
		const char *scriptSection;
		int line, column;
		func = context->GetFunction(n);
        line = context->GetLineNumber(n, &column, &scriptSection);
        if(n == 0)
        {
            String::Printf(str, 1024, "A script exception occurred: %s in file: %s (%i,%i)", context->GetExceptionString(),scriptSection, line, column);

            core->TraceTo("Script", str);

            if (excp)
            {
                excp->callstack.push_back(str);
                excp->callstack.push_back("Call stack:");
            }

            core->TraceTo("Script", "Call stack:");
        }
		String::Printf(str, 1024, "Function %s in file: %s (%i,%i)\n",func->GetDeclaration(), scriptSection, line, column);
		core->TraceTo("Script", str);

		if (excp)
		{
			excp->callstack.push_back(str);
		}
	}	
}

void ScriptService::Work(float dt, int level)
{
	if (printException && exceptions.size() > 0)
	{
		Exception* excp = &exceptions[0];

		excp->fade_time -= dt;

		if (excp->fade_time < 0.0f)
		{
			exceptions.erase(exceptions.begin());
		}
		else
		{
			for (int i = 0; i<excp->callstack.size(); i++)
			{
				en_core.Render()->Debug()->PrintText(10, i * 20, up_left, COLOR_YELLOW, excp->callstack[i].c_str());
			}
		}
	}
}

void ScriptService::Release()
{
	delete machine;
#if defined(PC) || defined(OSUNIX)
    ClearScriptParseData();
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(PC) || defined(OSUNIX)

ScriptCodeParserData::ScriptNameSpace* ScriptService::GetNameSpace(const char* sName)
{
    ScriptCodeParserData::ScriptNameSpace** ppSpace = vScriptNameSpaces.Find(sName);
    if(ppSpace)
        return *ppSpace;
    ppSpace = vScriptClasses.Find(sName); 
    if(ppSpace)
        return *ppSpace;
    return NULL;
}

ScriptCodeParserData::ScriptNameSpace* ScriptService::GetNameSpace(ScriptCodeParserData::ScriptNameSpace* pParent,int nLine,const char* sFileName,const string& sName,std::vector<string>& vNameSpace,bool& bNameSpace)
{
    vNameSpace.clear();
    pParent->GetNameSpace(sName,nLine,sFileName,vNameSpace,bNameSpace);
    if(vNameSpace.empty())
    {
        if(sName == "this")return pParent;
        bool bPrivate = String::IsEqual(sFileName,pParent->File().c_str()) && nLine > pParent->StartLine() && nLine < pParent->EndLine();
        ScriptCodeParserData::Function* pRef = pParent->GetFunction(sName,bPrivate);
        if(pRef)
            return pParent;
        return NULL;
    }
    if(vNameSpace.size() == 2)
    {
        ScriptCodeParserData::ScriptNameSpace* pNextSpace = GetNameSpace(vNameSpace[0].c_str());
        if(pNextSpace)
            return  GetNameSpace(pNextSpace,nLine,sFileName,vNameSpace[1],vNameSpace,bNameSpace);
        return pNextSpace;
    }
    else
        return  FindNameSpace(nLine,sFileName,vNameSpace[0]);
}
ScriptCodeParserData::ScriptNameSpace* ScriptService::GetFirstSpace(int nLine,const char* sFileName,std::vector<string>& vPath,bool bNameSpace)
{
    ScriptCodeParserData::ScriptNameSpace* pSpace = NULL;
    if(bNameSpace)
    {
        ScriptCodeParserData::ScriptNameSpace** ppSpace = vScriptNameSpaces.Find(vPath[0]);
        if(ppSpace)
            pSpace = (*ppSpace);
    }
    else
        if(vPath[0] == "this")
        {
            TScriptNameSpacesList vStack;
            TScriptNameSpacesList* pList = vFileNamesData.Find(sFileName);
            if(pList)
                for(unsigned i = 0;i < pList->size();++i)
                {
                    ScriptCodeParserData::ScriptNameSpace* pSpace = pList->at(i);
                    if(pSpace &&  nLine > pSpace->StartLine() && nLine < pSpace->EndLine())
                        vStack.push_back(pSpace);
                }
                for(int i = vStack.size() - 1;i >= 0;--i)
                    if(vStack[i]->Type() == ScriptCodeParserData::ScriptNameSpace::eClass)
                    {
                        pSpace = vStack[i];
                        break;
                    }
        }
        else  
            pSpace = FindNameSpace(nLine,sFileName,vPath[0]);
    return pSpace;
}
ScriptCodeParserData::ScriptNameSpace* ScriptService::GetNameSpace(int nLine,const char* sFileName,std::vector<string>& vPath,bool& bNameSpace)
{
    ScriptCodeParserData::ScriptNameSpace* pSpace = GetFirstSpace(nLine,sFileName,vPath,bNameSpace);
    if(pSpace)
    {
        int nStart = 0;
        if(bNameSpace)nStart = 1;
        for(unsigned i = nStart;i < vPath.size();++i)
        {
            std::vector<string> vSpaceName;
            ScriptCodeParserData::ScriptNameSpace* pNextSpace = GetNameSpace(pSpace,nLine,sFileName,vPath[i],vSpaceName,bNameSpace);
            if(!pNextSpace)
            {
                for(unsigned j = i + 1;j <  vPath.size();++j)vSpaceName.push_back(vPath[j]);
                vPath.swap(vSpaceName);
                return NULL;
            }
            else
                pSpace = pNextSpace;
        }
        return pSpace;
    }
    return NULL;
}

int ScriptService::FindPosition(int nLine,const char* sFileName,std::vector<string>& vPath,bool bNameSpace,string& sOutFile)
{
    ScriptCodeParserData::ScriptNameSpace* pSpace = GetFirstSpace(nLine,sFileName,vPath,bNameSpace);
    if(pSpace)
    {
        for(unsigned i = 0;i < vPath.size() - 1;++i)
        {
            std::vector<string> vSpaceName; bool flag  = bNameSpace;
            ScriptCodeParserData::ScriptNameSpace* pNextSpace = GetNameSpace(pSpace,nLine,sFileName,vPath[i],vSpaceName,flag);
            if(!pNextSpace)
                break;
            pSpace = pNextSpace;
        }
        sOutFile = pSpace->File();
        string& sLast = vPath.back();
        if(sLast[sLast.size() - 1] == ')')
        {
            string sName;
            sName.assign(&sLast[0],sLast.find("("));
            return  pSpace->GetFunctionPos(sName);
        }
        else
        {
            if(vPath.size() == 1 && pSpace->Name() == sLast)
                return pSpace->StartPos();
            return pSpace->GetMemberPos(sLast);
        }
    }
    return -1;
}


void ScriptService::OnTextMofified(const char* sFileName,int nLine,int nLineAdded,int nPos,int nOffset)
{
    TScriptNameSpacesList* pList = vFileNamesData.Find(sFileName);
    if(pList)
    {
        for(unsigned i = 0;i < pList->size();++i)
        {
            ScriptCodeParserData::ScriptNameSpace* pSpace = pList->at(i);
            if(pSpace)
            {
                int nStartLine = pSpace->StartLine();
                int nStartPos = pSpace->StartPos();
                int nEndPos = pSpace->EndPos();
                int nEndLine = pSpace->EndLine();
                if(nStartPos >= nPos)nStartPos += nOffset;
                if(nEndPos >= nPos)nEndPos += nOffset;
                if(nLineAdded != 0)
                {
                    if(nLine <= nStartLine)nStartLine += nLineAdded;
                    if(nLine <= nEndLine)nEndLine += nLineAdded;
                }
                pSpace->SetStart(nStartLine,nStartPos);
                pSpace->SetEnd(nEndLine,nEndPos);
                pSpace->OnTextMofified(nPos,nOffset);
            }
        }
    }
}

void ScriptService::GetFunctionName(string& sName,int nLine,const char* sFileName)
{
    TScriptNameSpacesList* pList = vFileNamesData.Find(sFileName);
    if(pList)
    {
        TScriptNameSpacesList vStack;
        for(unsigned i = 0;i < pList->size();++i)
        {
            ScriptCodeParserData::ScriptNameSpace* pSpace = pList->at(i);
            if(pSpace && pSpace->Type() ==ScriptCodeParserData::ScriptNameSpace::eFunction &&  nLine > pSpace->StartLine() && nLine < pSpace->EndLine())
            {
              sName = pSpace->Name();
              return;
            }
        }
    }
}


ScriptCodeParserData::ScriptNameSpace* ScriptService::FindNameSpace(int nLine,const char* sFileName,const string& sName)
{
    string sMember = sName;
    if(sName.find(")") != string::npos)
	{
        sMember.assign(&sName[0],sName.find("("));
	}
    else
	{
        if(sName.size() > 0 && sName[sName.size() - 1] == ']')
		{
            sMember.assign(&sName[0],sName.find("["));
		}
	}

    
    TScriptNameSpacesList* pList = vFileNamesData.Find(sFileName);
    if(pList)
    {
        TScriptNameSpacesList vStack;
        for(unsigned i = 0;i < pList->size();++i)
        {
            ScriptCodeParserData::ScriptNameSpace* pSpace = pList->at(i);
            if(pSpace &&  nLine > pSpace->StartLine() && nLine < pSpace->EndLine())
                vStack.push_back(pSpace);
        }

        for(int i = 0;i < vStack.size();++i)
        {
            ScriptCodeParserData::ScriptNameSpace* pSpace = vStack[i];
            if(vStack[i]->IsPresents(sMember,&pSpace))
                return pSpace;
        }
    }

   ScriptCodeParserData::ScriptNameSpace** ppSpace = vScriptNameSpaces.Find(sMember);
   if(ppSpace)
       return *ppSpace;

   TScriptNameSpaces::Iterator it = vScriptNameSpaces.Begin();
   while(it != vScriptNameSpaces.End())
   {
       ScriptCodeParserData::ScriptNameSpace* pSpace = *(*it);
       if(pSpace->IsPresents(sMember,&pSpace))
           return pSpace;
       ++it;
   }

   ppSpace = vScriptClasses.Find(sMember);
   if(ppSpace)
       return *ppSpace;
   
   {
       TScriptNameSpaces::Iterator it = vScriptClasses.Begin();
       while(it != vScriptClasses.End())
       {
           ScriptCodeParserData::ScriptNameSpace* pSpace = *(*it);
           if(pSpace->IsPresents(sMember,&pSpace))
               return pSpace;
           ++it;
       }
   }

    return NULL;
}
void ScriptService::AddCodeToNameSpaces(const char* name)
{
	ScriptModule* module = new ScriptModule((ScriptMachine*)machine);
	module->AddCodeToNameSpaces(name);
	delete module;
}

void ScriptService::ParseCode(const char* name)
{
    ScriptModule* module = new ScriptModule((ScriptMachine*)machine);
    module->ParseCode(name);
    delete module;
}

void ScriptService::AddNameSpace(ScriptCodeParserData::ScriptNameSpace* pSpace,const char* sFileName)
{
	if(pSpace->Type() == ScriptCodeParserData::ScriptNameSpace::eClass )
	{
		ScriptCodeParserData::ScriptNameSpace** ppSpace = vScriptClasses.Find(pSpace->Name());
		if(ppSpace)
		{
			(*ppSpace)->Merge(pSpace);
			delete pSpace;
			return;
		}
		else
			vScriptClasses[pSpace->Name()] = pSpace;
	}
	else
	{
		if(pSpace->Type() == ScriptCodeParserData::ScriptNameSpace::eNameSpace || pSpace->Type() == ScriptCodeParserData::ScriptNameSpace::eFile)
		{
			ScriptCodeParserData::ScriptNameSpace** ppSpace = vScriptNameSpaces.Find(pSpace->Name());
			if(ppSpace)
			{
				(*ppSpace)->Merge(pSpace);
				delete pSpace;
				return;

			}
			else
				vScriptNameSpaces[pSpace->Name()] = pSpace;
		}
	}
	TScriptNameSpacesList* pList = vFileNamesData.Find(sFileName);
	if(!pList)
	{
		vFileNamesData.Insert(sFileName,TScriptNameSpacesList());
		pList = vFileNamesData.Find(sFileName);
	}
	if(pList)
		pList->push_back(pSpace);
}

void ScriptService::GetNameSpaces(std::vector<string>& sNames,const string& sFileName)
{
    TScriptNameSpacesList* pList = vFileNamesData.Find(sFileName);
    if(pList)
    {
        for(unsigned i = 0;i < pList->size();++i)
        {
            ScriptCodeParserData::ScriptNameSpace* pSpace = pList->at(i);
            if(pSpace && (pSpace->Type() == ScriptCodeParserData::ScriptNameSpace::eClass || pSpace->Type() == ScriptCodeParserData::ScriptNameSpace::eNameSpace))
                sNames.push_back(pSpace->Name());
        }
    }
}

void ScriptService::GetMembers(std::vector<string>& sNames,const string& sClass,bool bSearchInParents)
{
    ScriptCodeParserData::ScriptNameSpace** ppSpace = vScriptNameSpaces.Find(sClass);
    if(ppSpace)
        (*ppSpace)->GetMembers(sNames,true,bSearchInParents);
    ppSpace = vScriptClasses.Find(sClass);
    if(ppSpace)
        (*ppSpace)->GetMembers(sNames,true,bSearchInParents);
    
}

void ScriptService::GetFunctions(std::vector<string>& sNames,const string& sClass,bool bSearchInParents)
{
    ScriptCodeParserData::ScriptNameSpace** ppSpace = vScriptNameSpaces.Find(sClass);
    if(ppSpace)
        (*ppSpace)->GetSignatures(sNames,true,bSearchInParents);
    ppSpace = vScriptClasses.Find(sClass);
    if(ppSpace)
        (*ppSpace)->GetSignatures(sNames,true,bSearchInParents);

}

int ScriptService::GetMemberPos(const string& sClass,const string& sMember)
{
    ScriptCodeParserData::ScriptNameSpace** ppSpace = vScriptNameSpaces.Find(sClass);
    if(ppSpace)
        return (*ppSpace)->GetMemberPos(sMember);
    ppSpace = vScriptClasses.Find(sClass);
    if(ppSpace)
        (*ppSpace)->GetMemberPos(sMember);
    return -1;
}

int ScriptService::GetFunctionPos(const string& sClass,const string& sFunction)
{
    ScriptCodeParserData::ScriptNameSpace** ppSpace = vScriptNameSpaces.Find(sClass);
    if(ppSpace)
        return (*ppSpace)->GetFunctionPos(sFunction);
    ppSpace = vScriptClasses.Find(sClass);
    if(ppSpace)
        return (*ppSpace)->GetFunctionPos(sFunction);
    return -1;
}

void ScriptService::ReParseCode(const char* sFileName,const string& sText)
{
    TScriptNameSpacesList* pList = vFileNamesData.Find(sFileName);
    if(pList)
    {
        for(unsigned i = 0;i < pList->size();++i)
        {
            ScriptCodeParserData::ScriptNameSpace* pSpace = pList->at(i);
            if(pSpace)
            {
               ScriptCodeParserData::ScriptNameSpace** ppSpace =  vScriptNameSpaces.Find(pSpace->Name());
               if(ppSpace)
               {
                   (*ppSpace)->Cut(pSpace);
                   if((*ppSpace)->IsEmpty())
                       vScriptNameSpaces.Delete(pSpace->Name());
               }
               ppSpace =  vScriptClasses.Find(pSpace->Name());
               if(ppSpace && pSpace == *ppSpace)
                 vScriptClasses.Delete(pSpace->Name());
               delete pSpace;
            }
        }
        pList->clear();
        vFileNamesData.Delete(sFileName);
    }
    ScriptModule* module = new ScriptModule((ScriptMachine*)machine);
    module->ReParseCode(sFileName,sText);
    delete module;

}

#define SCRIPT_PARSER_CASH_VER  1
void ScriptService::SaveParseData(const char* name)
{
    if(strlen(name) == 0)return;
    IStreamSaver* pSaver = core->Files()->CreateStreamSaver(ScriptParseDataFileName(name), IStreamSaver::xml);
    if (!pSaver)
        return;
    pSaver->MarkBeginBlock("CodeData");	
    pSaver->Write("Version",SCRIPT_PARSER_CASH_VER);
    HashTable<string,TScriptNameSpacesList>::Iterator it = vFileNamesData.Begin();
    while(it != vFileNamesData.End())
    {
        TScriptNameSpacesList* pVector = (*it);
        pSaver->MarkBeginBlock("File");
        pSaver->Write("Name",it.GetKey().c_str());
        for(unsigned i = 0;i < pVector->size();++i)
        {
            pSaver->MarkBeginBlock("NameSpace");	
            pVector->at(i)->Save(pSaver);
            pSaver->MarkEndBlock("NameSpace");	
        }
        pSaver->MarkEndBlock("File");
        ++it;
    }
	pSaver->MarkEndBlock("CodeData");	
    pSaver->Release();
}

bool ScriptService::LoadParseData(const char* name)
{
    ClearScriptParseData();
    if(strlen(name) == 0)return false;
    IStreamLoader* pLoader = core->Files()->CreateStreamLoader(ScriptParseDataFileName(name),"CodeData",IStreamLoader::xml);
    if (!pLoader)
        return false;
    int nVersion = -1;
    pLoader->Read("Version", nVersion);
    if(nVersion != SCRIPT_PARSER_CASH_VER)
        return false;
    while (pLoader->EnterBlock("File"))
    {
        char sFileName[256];
        memset(sFileName,0,256);
        pLoader->Read("Name", sFileName, 256);
        while (pLoader->EnterBlock("NameSpace"))
        {
            ScriptCodeParserData::ScriptNameSpace* pSpace = new ScriptCodeParserData::ScriptNameSpace(sFileName);
            pSpace->Load(pLoader);
            AddNameSpace(pSpace,sFileName);
            pLoader->LeaveBlock();
        }
        pLoader->LeaveBlock();
    }
    pLoader->Release();
    return true;
}

char* ScriptService::ScriptParseDataFileName(const char* name)
{
    static char sBuffer[256];	
    memset(sBuffer,0,256);
    String::Copy(sBuffer, 256, name);
    int len = strlen(sBuffer);
    if(len > 4)
    {
        if (sBuffer[len - 4] != '.')
            String::Cat(sBuffer, 256,".sbd");
        else
        {
            sBuffer[len - 3] = 's';
            sBuffer[len - 2] = 'b';
            sBuffer[len - 1] = 'd';
        }
    }
    else
        String::Cat(sBuffer, 256,".sbd");
    return sBuffer;
}

void ScriptService::ClearScriptParseData()
{
    HashTable<string,TScriptNameSpacesList>::Iterator it = vFileNamesData.Begin();
    while(it != vFileNamesData.End())
    {
        TScriptNameSpacesList* pList = (*it);
        for(unsigned i = 0;i < pList->size();++i)
            delete pList->at(i);
        pList->clear();
        ++it;
    }
    vFileNamesData.Clear();
    vScriptNameSpaces.Clear();
    vScriptClasses.Clear();
}

void ScriptService::GetScriptFilesList(std::vector<string>& vList)
{
	vList.clear();
	HashTable<string,TScriptNameSpacesList>::Iterator it = vFileNamesData.Begin();
	while(it != vFileNamesData.End())
	{
		vList.push_back(it.GetKey());
		++it;
	}
}

const char* ScriptService::GetScriptFilePath(const char* sName)
{
	static char sBuffer[256];	
	memset(sBuffer,0,256);
	const char* sFilePath = en_core.EN_Scene()->GetSceneFileName(sName);
	string sPath = sName;
	if (!sFilePath)
	{
		if(String::GetAbsalutPath(sPath,core->GetProjectPath()))
			sFilePath = sPath.c_str();
		else
			sFilePath = NULL;
	}
	if (!sFilePath)
	{
		core->TraceTo("ScriptErr", "LoadScript: Wrong scene file name %s ",sName);
		return false;
	}
	String::Copy(sBuffer, 256, sFilePath);
	int len = strlen(sBuffer);
	if(len > 4)
	{
		if (sBuffer[len - 4] != '.')
			String::Cat(sBuffer, 256,".srt");
		else
		{
			sBuffer[len - 3] = 's';
			sBuffer[len - 2] = 'r';
			sBuffer[len - 1] = 't';
		}
	}
	else
		String::Cat(sBuffer, 256,".srt");
	return sBuffer;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ScriptCodeParserData::ScriptNameSpace::~ScriptNameSpace()
{
  Clear();
}

void ScriptCodeParserData::ScriptNameSpace::AddReference(const string& name,const std::vector<string>& vWordsStack,int nPos)
{
    if(vWordsStack.back() == "::")return;
    bool  bPrivate = false,bArray = false,bNamespace = false;int nNameSpaceIndex = -1;
    for(unsigned i = 0; i < vWordsStack.size();++i)
    {
        if(!bPrivate)
            if(vWordsStack[i] == "private")bPrivate = true;
        if(!bArray)
            if(vWordsStack[i] == "array")bArray = true;
        if(!bNamespace)
            if(vWordsStack[i] == "::")
            {
                bNamespace = true;
                nNameSpaceIndex = i - 1;
            }
    }
    string sClass  = vWordsStack.back();
    if(Type() == eNameSpace)
    {
        string sNewName = sName + "::" + sClass;
        for(unsigned i =0;i < vClasses.size();++i)
            if(vClasses[i]->Name() == sNewName)
            {
                sClass = sNewName;
                break;
            }
    }
    if(bNamespace)
         vReferences[name]  = new ScriptCodeParserData::Reference(sFileName,name,sClass,vWordsStack[nNameSpaceIndex],nPos,bArray,bPrivate);
    else
        vReferences[name]  = new ScriptCodeParserData::Reference(sFileName,name,sClass,nPos,bArray,bPrivate);
}

void ScriptCodeParserData::ScriptNameSpace::Merge(ScriptCodeParserData::ScriptNameSpace* pOther)
{
    if(!pOther->vParents.empty())
        vParents.insert(vParents.end(),pOther->vParents.begin(),pOther->vParents.end());

    TReferences::Iterator it = pOther->vReferences.Begin();
    while(it != pOther->vReferences.End())
    {
        Reference*  pRef = *(*it);
        Reference** ppRef = vReferences.Find(it.GetKey());
        if(!ppRef)
        {
            vReferences.Add(it.GetKey(),pRef);
            pRef->IncRef();
        }
        ++it;
    }
    {
        TFunctions::Iterator it = pOther->vFunctions.Begin();
        while(it != pOther->vFunctions.End())
        {
            Function*  pFunc = *(*it);
            Function** ppFunc = vFunctions.Find(it.GetKey()); 
            if(!ppFunc)
            {
                 vFunctions.Add(it.GetKey(),pFunc);
                 pFunc->IncRef();
            }
            else
            {
                if( pFunc->Signature() != (*ppFunc)->Signature() )
                {
                    vFunctions.Add(it.GetKey(),pFunc);
                    pFunc->IncRef();
                }
            }
            ++it;
        }
    }
}
void ScriptCodeParserData::ScriptNameSpace::Clear()
{
    TFunctions::Iterator it = vFunctions.Begin();
    while(it != vFunctions.End())
    {
        (*(*it))->Delete();
        ++it;
    }
    vFunctions.Clear();
    {
        TReferences::Iterator it = vReferences.Begin();
        while(it != vReferences.End())
        {
            (*(*it))->Delete();
            ++it;
        }
        vReferences.Clear();
    }
}
void ScriptCodeParserData::ScriptNameSpace::Cut(ScriptCodeParserData::ScriptNameSpace* pOther)
{

    if(pOther == this)
    {
        vParents.clear();
        Clear();
    }
    else
    {
        if(!pOther->vParents.empty())
        {
            for(unsigned i = 0;i < pOther->vParents.size();++i)
            {
                std::vector<string>::iterator it =  vParents.begin();
                for(;it != vParents.end();++it)
                    if(*it == pOther->vParents[i])
                    {
                        vParents.erase(it);
                        break;
                    }
            }
        }
        TReferences::Iterator it = pOther->vReferences.Begin();
        while(it != pOther->vReferences.End())
        {
            Reference*  pRef = *(*it);
            Reference** ppRef = vReferences.Find(it.GetKey());
            if(ppRef && pRef == *ppRef)
            {
                pRef->Delete();
                vReferences.Delete(it.GetKey());
            }
            ++it;
        }
        {
            TFunctions::Iterator it = pOther->vFunctions.Begin();
            while(it != pOther->vFunctions.End())
            {
                Function*  pFunc = *(*it);
                Function** ppFunc = vFunctions.Find(it.GetKey()); 
                if(ppFunc && pFunc == *ppFunc)
                {
                    pFunc->Delete();
                    vFunctions.Delete(it.GetKey());
                }
				++it;
            }
        }
    }
}

void ScriptCodeParserData::ScriptNameSpace::GetMembers(std::vector<string>& vMembers,int nLine,bool bSearchInParents)
{
    bool bPrivate = nLine != -1 && nStartLine < nLine &&  nLine < nEndLine;
    GetMembers(vMembers,bPrivate,bSearchInParents);
    GetFunctionNames(vMembers,bPrivate,bSearchInParents);
}



bool ScriptCodeParserData::ScriptNameSpace::IsNamePresents(std::vector<string>& vNames,const string& sName)
{
    for(unsigned i = 0;i < vNames.size();++i)
        if(vNames[i] == sName)
            return true;
    return false;
}

void ScriptCodeParserData::ScriptNameSpace::GetFunctions(std::vector<ScriptCodeParserData::Function*>& vList,const string& sName,bool bPrivate,bool bSearchInParents)
{
    if(bSearchInParents)
    {
        for(unsigned i = 0; i < vParents.size();++i)
        {
            ScriptCodeParserData::ScriptNameSpace* pParent = core->Script()->GetNameSpace(vParents[i].c_str());
            if(pParent)
                pParent->GetFunctions(vList,sName,bPrivate,true);
        }
    }
    const  std::vector<ScriptCodeParserData::Function**>&  vBuff = vFunctions.GetList(sName);
    for(unsigned j =0 ;j < vBuff.size();++j)
    {
        unsigned i = 0 ;
        Function* pFunc = *vBuff[j];
        for(;i < vList.size();++i)
            if(vList[i]->Signature() == pFunc->Signature())break;
        if(i >= vList.size())
        {
            if(pFunc->IsPrivate())
            {
                if(bPrivate)
                     vList.push_back(pFunc);
            }
            else
             vList.push_back(pFunc);
        }
    }
}

void ScriptCodeParserData::ScriptNameSpace::GetFunctionNames(std::vector<string>& vMembers,bool bPrivate,bool bSearchInParents)
{
    if(bSearchInParents)
    {
        for(unsigned i = 0; i < vParents.size();++i)
        {
            ScriptCodeParserData::ScriptNameSpace* pParent = core->Script()->GetNameSpace(vParents[i].c_str());
            if(pParent)
                pParent->GetFunctionNames(vMembers,bPrivate,true);
        }
    }
    TFunctions::Iterator it = vFunctions.Begin();
    std::vector<string> vBuff;
    while(it != vFunctions.End())
    {
        Function* pFunc = (*(*it));
        const string& sName  = pFunc->Name();
        if(!IsNamePresents(vMembers,sName))
        {
            if(pFunc->IsPrivate())
            {
                if(bPrivate)
                    vBuff.push_back(sName);
            }
            else
                vBuff.push_back(sName);
        }
        ++it;
    }
    vMembers.insert(vMembers.end(),vBuff.begin(),vBuff.end());
}


void ScriptCodeParserData::ScriptNameSpace::GetMembers(std::vector<string>& vMembers,bool bPrivate,bool bSearchInParents)
{
    if(bSearchInParents)
    {
        for(unsigned i = 0; i < vParents.size();++i)
        {
            ScriptCodeParserData::ScriptNameSpace* pParent = core->Script()->GetNameSpace(vParents[i].c_str());
            if(pParent)
                pParent->GetMembers(vMembers,bPrivate,true);

        }
    }
    TReferences::Iterator it = vReferences.Begin();
    std::vector<string> vBuff;
    while(it != vReferences.End())
    {
        Reference* pRef = (*(*it));
        const string& sName  = pRef->Name();
        if(!IsNamePresents(vMembers,sName))
        {
            if(pRef->IsPrivate())
            {
                if(bPrivate)
                    vBuff.push_back(sName);
            }
            else
                vBuff.push_back(sName);
        }
        ++it;
    }
    if(Type() == eNameSpace)
    {
        for(unsigned i =0 ;i < vClasses.size();++i)
        {
            string sName = vClasses[i]->Name();
            sName.assign(&sName[sName.find("::")],string::npos);
            sName.erase(sName.begin(),sName.begin() + 2);
            vBuff.push_back(sName);
        }
    }
    vMembers.insert(vMembers.end(),vBuff.begin(),vBuff.end());
}


void ScriptCodeParserData::ScriptNameSpace::GetSignatures(std::vector<string>& vSignatures,int nLine,const string& sName,bool bSearchInParents)
{
    if(bSearchInParents)
    {
        for(unsigned i = 0; i < vParents.size();++i)
        {
            ScriptCodeParserData::ScriptNameSpace* pParent = core->Script()->GetNameSpace(vParents[i].c_str());
            if(pParent)
                pParent->GetSignatures(vSignatures,nLine,sName,true);
        }
    }
    bool bPrivate = nLine != -1 && nStartLine < nLine &&  nLine < nEndLine;
    const  std::vector<ScriptCodeParserData::Function**>&  vList = vFunctions.GetList(sName);
    for(unsigned j =0 ;j < vList.size();++j)
    {
        Function* pFunc = *vList[j];
        if(!IsNamePresents(vSignatures,pFunc->Signature()))
        {
            if(pFunc->IsPrivate())
            {
                if(bPrivate)
                    vSignatures.push_back(pFunc->Signature());
            }
            else
                vSignatures.push_back(pFunc->Signature());
        }
    }
}

void ScriptCodeParserData::ScriptNameSpace::GetSignatures(std::vector<string>& vSignatures,bool bPrivate,bool bSearchInParents)
{
    if(bSearchInParents)
    {
        for(unsigned i = 0; i < vParents.size();++i)
        {
            ScriptCodeParserData::ScriptNameSpace* pParent = core->Script()->GetNameSpace(vParents[i].c_str());
            if(pParent)
                pParent->GetSignatures(vSignatures,bPrivate,true);
        }
    }
    TFunctions::Iterator it = vFunctions.Begin();
    while(it != vFunctions.End())
    {
        Function* pFunc = (*(*it));
        if(!IsNamePresents(vSignatures,pFunc->Signature()))
        {
            if(pFunc->IsPrivate())
            {
                if(bPrivate)
                    vSignatures.push_back(pFunc->Signature());
            }
            else
                vSignatures.push_back(pFunc->Signature());
        }
        ++it;
    }
}



void ScriptCodeParserData::ScriptNameSpace::AddPublicFunction(const string& name,const string& signature,int nPos)
{
    vFunctions.Add(name,new ScriptCodeParserData::Function(sFileName,name,signature,nPos,false));
#ifdef LOG_SCRIPT_PARSER
   // core->TraceTo("Script","Add public function:  %s::%s ",this->sName.c_str(),signature.c_str());
#endif
}

void ScriptCodeParserData::ScriptNameSpace::AddPrivateFunction(const string& name,const string& signature,int nPos)
{
    vFunctions.Add(name,new ScriptCodeParserData::Function(sFileName,name,signature,nPos,true));
#ifdef LOG_SCRIPT_PARSER
//    core->TraceTo("Script","Add private function:  %s::%s ",this->sName.c_str(),signature.c_str());
#endif
}

void ScriptCodeParserData::ScriptNameSpace::AddParent(const string& name)
{
    vParents.push_back(name);
#ifdef LOG_SCRIPT_PARSER
    core->TraceTo("Script","Add parent :  %s ",name.c_str());
#endif
}

int ScriptCodeParserData::ScriptNameSpace::GetMemberPos(const string& sMember)
{
    int line = -1;
    if(GetMemberPos(sMember,line))
        return line;
    for(unsigned i = 0;i < vParents.size();++i)
    {
        ScriptCodeParserData::ScriptNameSpace* pSpace = core->Script()->GetNameSpace(vParents[i].c_str());
        if(pSpace && pSpace->GetMemberPos(sMember,line))return line;
    }
    return line;

}

bool ScriptCodeParserData::ScriptNameSpace::GetMemberPos(const string& sMember,int& nPos)
{
    if(vReferences.Size() > 0)
    {
        ScriptCodeParserData::Reference**  pRef = vReferences.Find(sMember);
        if(pRef)
        {
            nPos = (*pRef)->Pos();
            return true;
        }
    }
    if(Type() == ScriptCodeParserData::ScriptNameSpace::eNameSpace)
    {
        string sNewName = Name() + "::" + sMember;
        nPos =  GetChildNameSpacePos(sNewName);
        return nPos  != -1;
    }
    return false;
}

int ScriptCodeParserData::ScriptNameSpace::GetFunctionPos(const string& sFunction)
{
    int line = -1;
    if(GetFunctionPos(sFunction,line))
        return line;
    for(unsigned i = 0;i < vParents.size();++i)
    {
        ScriptCodeParserData::ScriptNameSpace* pSpace = core->Script()->GetNameSpace(vParents[i].c_str());
        if(pSpace && pSpace->GetFunctionPos(sFunction,line))return line;
    }
    return line;
}

bool ScriptCodeParserData::ScriptNameSpace::GetFunctionPos(const string& sFunction,int& nPos)
{
    if(vFunctions.Size() > 0)
    {
        ScriptCodeParserData::Function**  pFunc = vFunctions.Find(sFunction);
        if(pFunc)
        {
            nPos = (*pFunc)->Pos();
            return true;
        }
    }
    return false;
}

bool ScriptCodeParserData::ScriptNameSpace::IsPresents(const string& sName,ScriptNameSpace** pSpaceOut)
{
    *pSpaceOut = this;
    if(vReferences.Find(sName))return true;
    if(vFunctions.Find(sName))return true;

    for(unsigned i = 0;i < vParents.size();++i)
    {
        ScriptCodeParserData::ScriptNameSpace* pSpace = core->Script()->GetNameSpace(vParents[i].c_str());
        if(pSpace && pSpace->IsPresents(sName,pSpaceOut))
        {
            *pSpaceOut = pSpace;
            return true;
        }
    }
    return false;
}

ScriptCodeParserData::Reference* ScriptCodeParserData::ScriptNameSpace::GetReference(const string& sName,bool bPrivate)
{
    if(vReferences.Size() > 0)
    {
        ScriptCodeParserData::Reference**  pRef = vReferences.Find(sName);
        if(pRef)
        {
            if(!bPrivate && (*pRef)->IsPrivate())return NULL;
            return *pRef;
        }
    }
    for(unsigned i = 0;i < vParents.size();++i)
    {
        ScriptCodeParserData::ScriptNameSpace* pSpace = core->Script()->GetNameSpace(vParents[i].c_str());
        if(pSpace)
        {
            ScriptCodeParserData::Reference*  pRef = pSpace->GetReference(sName,bPrivate);
            if(pRef)
                return pRef;
        }
    }
    return NULL;
}


ScriptCodeParserData::Function* ScriptCodeParserData::ScriptNameSpace::GetFunction(const string& sName,bool bPrivate)
{
    if(vFunctions.Size() > 0)
    {
        ScriptCodeParserData::Function**  pRef = vFunctions.Find(sName);
        if(pRef)
        {
            if(!bPrivate && (*pRef)->IsPrivate())return NULL;
            return *pRef;
        }
    }
    for(unsigned i = 0;i < vParents.size();++i)
    {
        ScriptCodeParserData::ScriptNameSpace* pSpace = core->Script()->GetNameSpace(vParents[i].c_str());
        if(pSpace)
        {
            ScriptCodeParserData::Function*  pRef = pSpace->GetFunction(sName,bPrivate);
            if(pRef)
                return pRef;
        }
    }
    return NULL;
}

void ScriptCodeParserData::ScriptNameSpace::GetNameSpace(const string& sName,int nLine,const char* _sFileName,std::vector<string>& vNameSpaces,bool& bNameSpace)
{
    bool bPrivate = String::IsEqual(_sFileName,sFileName.c_str()) && nLine > nStartLine && nLine < nEndLine;
    if(sName.find(")") != string::npos)
    {
        string sMember,sSignature;std::vector<string> vSignatures;
        unsigned paramsCount = 0;
        int pos = sName.find("(",0);
        for(unsigned i = pos + 1;i < sName.size() - 1;++i)
        {
            if(paramsCount == 0 && (sName[i] != ' ' || sName[i] != '\t'))paramsCount = 1;
            else
                if(sName[i] == ',')++paramsCount;
        }
        sMember.assign(&sName[0],pos);
        std::vector<ScriptCodeParserData::Function*> vList;
        GetFunctions(vList,sMember,bPrivate,true);
        if(vList.size() > 0)
        {
            ScriptCodeParserData::Function* pFunc = NULL;
            for(unsigned i = 0; i < vList.size(); ++i)
            {
                if(vList[i]->ParamsCount() == paramsCount)
                {
                    pFunc = vList[i];
                    break;
                }
            }
            if(pFunc)
            {
                if(pFunc->IsArray() && sName[sName.size() - 1] != ']')
                    vNameSpaces.push_back("array");
                else
                {
                    if(!pFunc->NameSpace().empty())
                    {
                        vNameSpaces.push_back(pFunc->NameSpace());
                        bNameSpace = true;
                    }
                    vNameSpaces.push_back(pFunc->Class());
                }
            }
        }
    }
    else
    {
        string sMember = sName;
        if(sName[sName.size() - 1] == ']')
            sMember.assign(&sName[0],sName.find("["));
        ScriptCodeParserData::Reference* pRef = GetReference(sMember,bPrivate);
        if(pRef) 
        {
            if(pRef->IsArray() && sName[sName.size() - 1] != ']')
                vNameSpaces.push_back("array");
            else
            {
                if(!pRef->NameSpace().empty())
                {
                    vNameSpaces.push_back(pRef->NameSpace());
                    bNameSpace = true;
                }
                vNameSpaces.push_back(pRef->Class());

            }
        }
    }
}

bool ScriptCodeParserData::ScriptNameSpace::IsEmpty() const
{
    int nParentsEmpty = 0;
    for(unsigned i = 0;i < vParents.size();++i)
    {
        ScriptCodeParserData::ScriptNameSpace* pSpace = core->Script()->GetNameSpace(vParents[i].c_str());
        if(pSpace && pSpace->IsEmpty())
           ++nParentsEmpty;
    }
    return nParentsEmpty == vParents.size() && vReferences.IsEmpty() && vFunctions.IsEmpty();
}

void ScriptCodeParserData::ScriptNameSpace::OnTextMofified(int nPos,int nOffset)
{
    TFunctions::Iterator it = vFunctions.Begin();
    while(it != vFunctions.End())
    {
        ScriptCodeParserData::Function* pFunc = *(*it);
        if(nPos <= pFunc->Pos())
            *pFunc += nOffset;
        ++it;
    }
    {
        TReferences::Iterator it = vReferences.Begin();
        while(it != vReferences.End())
        {
            ScriptCodeParserData::Reference* pRef = *(*it);
            if(nPos <= pRef->Pos())
                *pRef += nOffset;
            ++it;

        }
    }

}

int ScriptCodeParserData::ScriptNameSpace::GetChildNameSpacePos(const string& sNameSpace)
{
    for(unsigned i= 0;i < vClasses.size();++i)
        if(vClasses[i]->Name() == sNameSpace)
            return vClasses[i]->StartPos();
    return -1;
}

void ScriptCodeParserData::ScriptNameSpace::Save(IStreamSaver* pSaver)
{
    pSaver->Write("Name",sName.c_str());
    pSaver->Write("StartLine",nStartLine);
    pSaver->Write("StartPos",nStartPos);
    pSaver->Write("EndLine",nEndLine);
    pSaver->Write("EndPos",nEndPos);
    pSaver->Write("Type",(int)eType);
    if(!vParents.empty())
    {
        pSaver->MarkBeginBlock("Parents");
        for(unsigned i = 0; i < vParents.size();++i)
        {
            pSaver->MarkBeginBlock("Parent");
            pSaver->Write("Name",vParents[i].c_str());
            pSaver->MarkEndBlock("Parent");
        }
        pSaver->MarkEndBlock("Parents");
    }
    if(!vFunctions.IsEmpty())
    {
        pSaver->MarkBeginBlock("Functions");
        TFunctions::Iterator it = vFunctions.Begin();
        while(it != vFunctions.End())
        {
            ScriptCodeParserData::Function* pFunc = *(*it);
            pSaver->MarkBeginBlock("Function");
            pFunc->Save(pSaver);
            pSaver->MarkEndBlock("Function");
            ++it;
        }
        pSaver->MarkEndBlock("Functions");
    }
    if(!vReferences.IsEmpty())
    {
        pSaver->MarkBeginBlock("References");
        TReferences::Iterator it = vReferences.Begin();
        while(it != vReferences.End())
        {
            ScriptCodeParserData::Reference* pRef = *(*it);
            pSaver->MarkBeginBlock("Reference");
            pRef->Save(pSaver);
            pSaver->MarkEndBlock("Reference");
            ++it;

        }
        pSaver->MarkEndBlock("References");
    }
    
}

void ScriptCodeParserData::ScriptNameSpace::Load(IStreamLoader* pLoader)
{
    static char sBuffer[512]; 
    memset(sBuffer,0,512);
    pLoader->Read("Name",sBuffer,512);
    sName = sBuffer;
    pLoader->Read("StartLine",nStartLine);
    pLoader->Read("StartPos",nStartPos);
    pLoader->Read("EndLine",nEndLine);
    pLoader->Read("EndPos",nEndPos);
    int nType = eNone;
    pLoader->Read("Type",nType);
    eType = static_cast<ETypes>(nType);
    if(pLoader->EnterBlock("Parents"))
    {
        while (pLoader->EnterBlock("Parent"))
        {
            memset(sBuffer,0,512);
            pLoader->Read("Name", sBuffer, 512);
            vParents.push_back(sBuffer);
            pLoader->LeaveBlock();
        }
        pLoader->LeaveBlock();
    }
    if(pLoader->EnterBlock("Functions"))
    {
        while (pLoader->EnterBlock("Function"))
        {
            ScriptCodeParserData::Function* pFunc = new ScriptCodeParserData::Function(sFileName);
            pFunc->Load(pLoader);
            vFunctions.Add(pFunc->Name(),pFunc);
            pLoader->LeaveBlock();
        }
        pLoader->LeaveBlock();
    }
    if(pLoader->EnterBlock("References"))
    {
        while (pLoader->EnterBlock("Reference"))
        {
            ScriptCodeParserData::Reference* pRef = new ScriptCodeParserData::Reference(sFileName);
            pRef->Load(pLoader);
            vReferences[pRef->Name()] = pRef;
            pLoader->LeaveBlock();
        }
        pLoader->LeaveBlock();
    }
}

ScriptCodeParserData::Function::Function(const string& file,const string& name,const string& signature,int pos,bool  _bPrivate):Object(file,name,pos,_bPrivate),nParamsCount(0),sSignature(signature)
{
    int nStartPos =  signature.find("array");
    bArray = nStartPos != string::npos;
    if(bArray)nStartPos = 6;
    else nStartPos = 0;
    string word;
    for(unsigned i = nStartPos;i < sSignature.size();++i)
    {
        char ch = sSignature[i];
        if(ch == ':')
        {
            sNameSpace = word;
            word.clear();
            ++i;
        }
        else
           if(isalpha(ch) || isdigit(ch) || ch == '_')word += ch;
           else
             if(ch == ' ' || ch == '@')
             {
                 sClass = word;
                 break;
             }
    }
    for(unsigned i = signature.find("(") + 1;i < sSignature.size();++i)
    {
        if(nParamsCount == 0 && (sSignature[i] != ' ' || sSignature[i] != '\t'))nParamsCount = 1;
        if(sSignature[i] == ',')++nParamsCount;
    }
}

ScriptCodeParserData::Function::Function(const ScriptCodeParserData::Function& other)
{
    sName.assign(other.sName);
    sSignature.assign(other.sSignature);
    sFileName.assign(other.sFileName);
    sClass.assign(other.sClass);
    sNameSpace.assign(other.sNameSpace);
    nPos  = other.nPos;
    nParamsCount = other.nParamsCount;
    bArray = other.bArray;
    bPrivate = other.bPrivate;
}

ScriptCodeParserData::Function& ScriptCodeParserData::Function::operator=(const ScriptCodeParserData::Function& other)
{
    sName.assign(other.sName);
    sSignature.assign(other.sSignature);
    sFileName.assign(other.sFileName);
    sClass.assign(other.sClass);
    sNameSpace.assign(other.sNameSpace);
    nPos  = other.nPos;
    nParamsCount = other.nParamsCount;
    bArray = other.bArray;
    bPrivate = other.bPrivate;
    return *this;
}

void ScriptCodeParserData::Function::Save(IStreamSaver* pSaver)
{
    ScriptCodeParserData::Object::Save(pSaver);
    pSaver->Write("Signature",sSignature.c_str());
    pSaver->Write("ParamsCount",nParamsCount);
}

void  ScriptCodeParserData::Function::Load(IStreamLoader* pLoader)
{
   ScriptCodeParserData::Object::Load(pLoader);
   LoadString(sSignature,"Signature",pLoader);
   pLoader->Read("ParamsCount",nParamsCount);
}

ScriptCodeParserData::Reference::Reference(const ScriptCodeParserData::Reference& other)
{
    sName.assign(other.sName);
    sClass.assign(other.sClass);
    sNameSpace.assign(other.sNameSpace);
    sFileName.assign(other.sFileName);
    nPos  = other.nPos;
    bArray = other.bArray;
    bPrivate = other.bPrivate;

}

ScriptCodeParserData::Reference::Reference(const string& file,const string& name,const string& _class,int pos,bool _bArray,bool _bPrivate):Object(file,name,_class,pos,_bArray,_bPrivate)
{
#ifdef LOG_SCRIPT_PARSER
    if(bPrivate)
    {
        if(bArray)
            core->TraceTo("Script","Add private array:  %s %s ",sClass.c_str(),sName.c_str());
        else 
            core->TraceTo("Script","Add private reference:  %s %s ",sClass.c_str(),sName.c_str());
    }
    else
    {
        if(bArray)
            core->TraceTo("Script","Add public array:  %s %s ",sClass.c_str(),sName.c_str());
        else 
            core->TraceTo("Script","Add public reference:  %s %s ",sClass.c_str(),sName.c_str());
    }
#endif
}

ScriptCodeParserData::Reference::Reference(const string& file,const string& name,const string& _class,const string& nameSpace,int pos,bool _bArray,bool _bPrivate):Object(file,name,_class,nameSpace,pos,_bArray,_bPrivate)
{
#ifdef LOG_SCRIPT_PARSER
        if(bPrivate)
        {
            if(bArray)
                core->TraceTo("Script","Add private array:  %s::%s %s ",nameSpace.c_str(),sClass.c_str(),sName.c_str());
            else 
                core->TraceTo("Script","Add private reference:  %s::%s %s ",nameSpace.c_str(),sClass.c_str(),sName.c_str());
        }
        else
        {
            if(bArray)
                core->TraceTo("Script","Add public array:  %s::%s %s ",nameSpace.c_str(),sClass.c_str(),sName.c_str());
            else 
                core->TraceTo("Script","Add public reference:  %s::%s %s ",nameSpace.c_str(),sClass.c_str(),sName.c_str());
        }
#endif
}

ScriptCodeParserData::Reference& ScriptCodeParserData::Reference::operator=(const ScriptCodeParserData::Reference& other)
{
    sName.assign(other.sName);
    sClass.assign(other.sClass);
    sNameSpace.assign(other.sNameSpace);
    sFileName.assign(other.sFileName);
    nPos  = other.nPos;
    bArray = other.bArray;
    bPrivate = other.bPrivate;
    return *this;
}


void ScriptCodeParserData::Object::Save(IStreamSaver* pSaver)
{
    pSaver->Write("Name",sName.c_str());
    pSaver->Write("Class",sClass.c_str());
    pSaver->Write("NameSpace",sNameSpace.c_str());
    pSaver->Write("Pos",nPos);
    pSaver->Write("Array",bArray);
    pSaver->Write("Private",bPrivate);
}

void ScriptCodeParserData::Object::Load(IStreamLoader* pLoader)
{
    LoadString(sName,"Name",pLoader);
    LoadString(sClass,"Class",pLoader);
    LoadString(sNameSpace,"NameSpace",pLoader);
    pLoader->Read("Pos",nPos);
    pLoader->Read("Array",bArray);
    pLoader->Read("Private",bPrivate);
}

void ScriptCodeParserData::Object::LoadString(string& sBuffer,const char* sName,IStreamLoader* pLoader)
{
    static char vBuffer[512]; 
    memset(vBuffer,0,512);
    sBuffer.clear();
    if(pLoader->Read(sName,vBuffer,512))
        sBuffer = vBuffer;
}

int ScriptCodeParserData::Object::g_nCreatedCount = 0;

#endif

