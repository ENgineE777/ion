#if defined(PC) || defined(OSUNIX)
#include "Core/ICore.h"
#include "Entities/NGUI/NGUISymbol.h"
#include "Core/Scene/SceneInstance.h"
#include "Entities/NGUI/NGUISymbolInstance.h"
#include "SceneInfo.h"
#include <algorithm>

namespace ProjectInfo
{
    void Entity::GetMembers(std::vector<string>& vMembers)
    {
        switch(eType)
        {
            case Entity::eSymbol:
            {
                for(unsigned i = 0;i < vChildsNames.size();++i)
                    vMembers.push_back(vChildsNames[i]);
                NGUISymbol::GetMembersForAutoComplete(true,vMembers);
                break;
            }
            case Entity::eSymbolInstance:
            {
                if(pDescriptor)
                {
                    for(unsigned i = 0;i < pDescriptor->vChildsNames.size();++i)
                        vMembers.push_back(pDescriptor->vChildsNames[i]);
                }
                for(unsigned i = 0;i < vChildsNames.size();++i)
                    vMembers.push_back(vChildsNames[i]);
                NGUISymbol::GetMembersForAutoComplete(bUseInstance,vMembers);
                break;
            }
            default:core->Script()->Machine()->GetMembers(sClassName,vMembers);
        }
    }

    void Entity::FindChilds(TEntities& vList)
    {
        for(unsigned i = 0; i < vChildsNames.size();++i)
        {
            Entity** ppChild = vList.Find(vChildsNames[i]);
            if(ppChild)
            {
                Entity* pChild = *ppChild;
                pChild->SetParent(this);
                pChild->SetWeight(i);
                vChilds.push_back(pChild);
            }
        }
    }

    Entity* Entity::GetChild(const string& sName)
    {
        for(TChilds::iterator it = vChilds.begin();it != vChilds.end();++it)
        {
            if((*it)->Name() == sName)
                return *it;
        }
        return NULL;
    }

    Entity* Entity::GetChild(int weight)
    {
        for(TChilds::iterator it = vChilds.begin();it != vChilds.end();++it)
        {
            if((*it)->Weight() == weight)
                return *it;
        }
        return NULL;
    }

    Entity* Entity::GetChildBefore(const string& sName)
    {
        for(TChilds::iterator it = vChilds.begin();it != vChilds.end();++it)
        {
            if((*it)->Name() == sName)
            {
                if(it != vChilds.begin())
                {
                    --it;
                    return *it;
                }
                return NULL;
            }
        }
        return NULL;
    }

    void Entity::AddToTreeList(TTree& rTree)
    {
        rTree.push_back(Node(this));
        if (HaveChilds())
        {
            rTree.push_back(Node(Node::eStartBlock));

            for(TChilds::iterator it = vChilds.begin();it != vChilds.end();++it)
                (*it)->AddToTreeList(rTree);

            rTree.push_back(Node(Node::eEndBlock));
        }
    }

    void* Entity::AddToTreeView(ISceneTree* pTree,void* pParent,void* pLast,bool bAddChildsRoot)
    {
        this->pTree = pTree;
        if(pLast)
            pParent = pTree->View()->AddEntityItem(sName.c_str(),bIncluded,this,pParent,pLast);
        else
            pParent = pTree->View()->AddEntityItem(sName.c_str(),bIncluded,this,pParent,nWeight);
        SetHandle(pParent);
        if(bAddChildsRoot)
            pLast = pTree->View()->AddItem("childs",0,NULL, pParent, null);
        for (TChilds::iterator it = vChilds.begin();it  != vChilds.end();++it)
        {
            Entity* pChild = (*it);
            pLast = pChild->AddToTreeView(pTree,pParent,pLast);
        }
        return pParent;
    }
    
    Entity* Entity::GetSymbol() 
    {
        if(pParent)
        {
            Entity* pIter = pParent;
            while(pIter)
            {
                if(pIter->eType == eSymbol)return pIter;
                pIter = pIter->pParent;
            }
        }
        else
            if(eType == eSymbol)
               return this;
        return NULL;
    }

    void Entity::OnChildRename(Entity* pChild)
    {
        if(pEntity)
        {
            if(Type() != Entity::eEntity)
            {
                NGUIWidget* pWidget =  static_cast<NGUIWidget*>(pEntity);
                String::Copy(pWidget->childs_names[pChild->Weight()].data, 128, pChild->Name().c_str());
            }
        }
        vChildsNames.erase(vChildsNames.begin() + pChild->Weight());
        vChildsNames.insert(vChildsNames.begin() + pChild->Weight(),pChild->Name());
    }

    void Entity::DeleteChild(Entity* pChild,bool bModifyEngineEntity/* = true*/)
    {
        if(pEntity)
        {
            if(Type() != Entity::eEntity)
            {
                NGUIWidget* pWidget =  static_cast<NGUIWidget*>(pEntity);
                if(pChild->IsWidget() && pChild->GetEngineEntity() && bModifyEngineEntity)
                {
                    pWidget->childs_names.erase(pWidget->childs_names.begin() + pChild->Weight());
                    pWidget->DelChild(static_cast<NGUIWidget*>(pChild->GetEngineEntity()));
                    pWidget->UpdateChildsPos();
                    Entity* pSymbol = GetSymbol();
                    if(pSymbol)
                        pSymbol->ReInitInstances();
                }
            }
        }
        vChildsNames.erase(vChildsNames.begin() + pChild->Weight());
        vChilds.remove(pChild);
        unsigned i = 0;
        for (TChilds::iterator  it = vChilds.begin();it != vChilds.end();++it,++i)
            (*it)->SetWeight(i);
    }
    void Entity::Delete(bool bModifyEngineEntity/* = true*/)
    {
        TChilds vBuffer = vChilds;
        for(TChilds::iterator it = vBuffer.begin();it != vBuffer.end();++it)
             pScene->Remove(*it);

        vChilds.clear();
        vChildsNames.clear();
        if(eType == Entity::eSymbol)
        {
            DeleteInstances();
        }
        else
        {
            if(eType == Entity::eSymbolInstance && pDescriptor)
                pDescriptor->RemoveInstance(this);

            if(pParent)
                pParent->DeleteChild(this,bModifyEngineEntity);
            else
                if(IsWidget())
                    NGUIService::instance->DelTopWidget(reinterpret_cast<NGUIWidget*>(pEntity));
        }

        if(pTree && pTree->View() && pHandle)
            pTree->View()->DeleteItem(pHandle);

        if(pEntity)
            pEntity->Scene()->RemoveEntity(pEntity);

    }

    void Entity::DeleteInstances()
    {
        TChilds vBuffer = vInstances;
        for(TChilds::iterator it = vBuffer.begin();it != vBuffer.end();++it)
        {
            pScene->Remove(*it);
        }
    }


    Entity::Entity(ProjectInfo::Scene* _pScene,SceneEntity* _pEntity):pDescriptor(NULL),pParent(NULL),pEntity(_pEntity),pTree(NULL),pHandle(NULL),
		eType(eEntity), bUseInstance(false), pScene(_pScene), bIncluded(false), nWeight(0)
    {
        sName = pEntity->name;
        sClassName = pEntity->className;
        if(String::IsEqual(sClassName.c_str(),"NGUISymbol"))eType = Entity::eSymbol;
        else
           if(String::IsEqual(sClassName.c_str(),"NGUISymbolInstance"))
           {
               eType = Entity::eSymbolInstance;
               NGUISymbolInstance* pInstance = static_cast<NGUISymbolInstance*>(pEntity);
               bUseInstance = pInstance->useInstance;
               sSymbolName = pInstance->symbol_name;
           }
           else
               if(String::IsSubstr(sClassName.c_str(),"NGUI") && !String::IsEqual(sClassName.c_str(),"NGUIFont") && !String::IsEqual(sClassName.c_str(),"NGUITexture"))
                   eType = Entity::eWidget;
        if(pScene && pScene->Instance())
        {
            bIncluded = pScene->Instance()->IsIncludedEntity(pEntity);
        }
        if(eType != eEntity)
        {
            NGUIWidget* pWidget = static_cast<NGUIWidget*>(pEntity);
            nWeight = pWidget->weight;
            sParentName = pWidget->sParentName;
            if(eType == Entity::eSymbolInstance)
            {
                pDescriptor = pScene->Find(sSymbolName);
                if(pDescriptor)
                    pDescriptor->AddInstance(this);
            }
        }
        
    }

    void Entity::Load(IStreamLoader* pLoader)
    {
        LoadString(pLoader,"className",sClassName);
        LoadString(pLoader,"entityName",sName);
        LoadString(pLoader,"parentName",sParentName);
        pLoader->Read("weight",nWeight);
        if(String::IsEqual(sClassName.c_str(),"NGUISymbol"))eType = Entity::eSymbol;
        else
        {
            if(String::IsEqual(sClassName.c_str(),"NGUISymbolInstance"))
            {
                eType = Entity::eSymbolInstance;
                pLoader->Read("useInstance",bUseInstance);
                LoadString(pLoader,"symbol_name",sSymbolName);
            }
            else
                if(String::IsSubstr(sClassName.c_str(),"NGUI") && !String::IsEqual(sClassName.c_str(),"NGUIFont") && !String::IsEqual(sClassName.c_str(),"NGUITexture"))
                    eType = Entity::eWidget;
        }
        int count = 0;
        pLoader->Read("childs_count",count);
        if(count > 0)
            vChildsNames.clear();
        for (int i=0; i < count; ++i)
        {
            if (pLoader->EnterBlock("childs"))
            {
                char name[128];
                pLoader->Read("name",name, 128);	
                AddChild(name);
                pLoader->LeaveBlock();
            }
        }

    }

    void Entity::ReInitInstances()
    {
        for(TChilds::iterator it  = vInstances.begin();it != vInstances.end();++it)
        {
            Entity* pEntity = *it; 
            if(pEntity->GetEngineEntity())
                pEntity->GetEngineEntity()->ReInit();
            Entity* pSymbol = pEntity->GetSymbol();
            if(pSymbol)
                pSymbol->ReInitInstances();
        }
    }

    bool Entity::Rename(const char* sNewName)
    {
        bool bGenerate = false;
        if (pScene->Find(sNewName)) 
        {
            sName = pScene->GenerateName(sNewName);
            bGenerate = true;
        }
        else sName = sNewName;
        if(pEntity)
            pEntity->SetName(sName.c_str());
        if(pParent)
            pParent->OnChildRename(this);
        if(Type() == Entity::eSymbol)
            for(TChilds::iterator it = vInstances.begin();it != vInstances.end();++it)
                (*it)->SetDescriptorName(sName);
        return bGenerate;
    }

    void Entity::AddChild(Entity* pChild,int weight)
    {
        for (TChilds::iterator  it = vChilds.begin();it != vChilds.end();++it)
        {
            int nWeigth = (*it)->Weight();
            if(nWeigth >= pChild->Weight())
                (*it)->SetWeight(nWeigth + 1);
        }
        vChilds.push_back(pChild);
        vChilds.sort(Entity::Comparator());
        unsigned i = 0;
        for (TChilds::iterator  it = vChilds.begin();it != vChilds.end();++it,++i)
            (*it)->SetWeight(i);
        AddChild(pChild->Name(),pChild->Weight());

    }

    void Entity::ChangeParent(Entity* _pParent,int weight)
    {
        if(pParent)
            pParent->DeleteChild(this);
        SetWeight(weight);
        pParent = _pParent;
        if(pParent)
        {
            pParent->AddChild(this,nWeight);
            if(IsWidget() && GetEngineEntity() && (pParent->IsWidget() || pParent->Type() == Entity::eSymbol) && pParent->GetEngineEntity())
            {
               NGUIWidget* pParentWidget = static_cast<NGUIWidget*>(pParent->GetEngineEntity());
               NGUIWidget* pWidget = static_cast<NGUIWidget*>(GetEngineEntity());
               AddToWidget(pParent,pWidget,nWeight);
               pParentWidget->UpdateChildsPos();
               Entity* pSymbol = pParent->GetSymbol();
			   
#ifdef PC
               if(pSymbol)
                   pSymbol->ReInitInstances();
               pWidget->DataToGizmo();
#endif
            }
        }
    }

    void Entity::SetWeight(int w)
    {
        nWeight = w;
        if(pEntity && IsWidget())
        {
            NGUIWidget* pWidget = static_cast<NGUIWidget*>(pEntity);
            pWidget->weight = w;
        }
    }

    Entity* Entity::Copy() const
    {
        return pScene->CopyEntity(this);
    }

    void Entity::Select()
    {
        if(pTree && pHandle)pTree->SelectItem(pHandle);
        Entity* pSymbol = GetSymbol();
        if(pSymbol)
            NGUIService::instance->symbol_widget = static_cast<NGUIWidget*>(pSymbol->GetEngineEntity());
        else
            NGUIService::instance->symbol_widget = NULL;
    }
    void  Entity::AddToWidget(Entity* pParent,NGUIWidget* pChildWidget,int nWeigth)
    {
        NGUIWidget* pWidget = static_cast<NGUIWidget*>(pParent->GetEngineEntity());
        pChildWidget->parent = pWidget;
        if(nWeigth < pWidget->childs_names.size())
        {
            if(pParent->Type() == Entity::eSymbolInstance)
               pWidget->childs.insert(pWidget->childs.begin() + nWeigth + pParent->Descriptor()->ChildsCount(),pChildWidget);
            else
                pWidget->childs.insert(pWidget->childs.begin() + nWeigth,pChildWidget);
            pWidget->childs_names.insert(pWidget->childs_names.begin() + nWeigth,Str128());
        }
        else
        {
            nWeigth =  pWidget->childs_names.size();
            pWidget->childs.push_back(pChildWidget);
            pWidget->childs_names.push_back(Str128());
        }
        String::Copy(pWidget->childs_names[nWeigth].data,128,pChildWidget->GetName());
        pWidget->UpdateChildsPos();
    }

    void Entity::MoveChild(Entity* pEntity,int nNewWeight)
    {
        int nOldWeigth = pEntity->Weight();
        vChilds.remove(pEntity);
        vChildsNames.erase(vChildsNames.begin() + pEntity->Weight());
        --nNewWeight;
        if(nNewWeight < 0)nNewWeight = 0;
        if (nNewWeight < vChilds.size())
        {
            unsigned i = 0;
            for(TChilds::iterator it = vChilds.begin();it != vChilds.end();++it,++i)
            {
                if(i == nNewWeight)
                {
                    vChilds.insert(it,pEntity);
                    break;
                }
            }
            vChildsNames.insert(vChildsNames.begin() + nNewWeight,pEntity->Name());
        }
        else
        {
            vChildsNames.push_back(pEntity->Name());
            vChilds.push_back(pEntity);
        }
        unsigned i = 0;
        for(TChilds::iterator it = vChilds.begin();it != vChilds.end();++it,++i)
        {
            (*it)->SetWeight(i);
        }

        if (GetEngineEntity() && (IsWidget() || eType == Entity::eSymbol) && pEntity->GetEngineEntity() && pEntity->IsWidget())
        {
            NGUIWidget* pWidget = static_cast<NGUIWidget*>(GetEngineEntity());
            NGUIWidget* pChildWidget = static_cast<NGUIWidget*>(pEntity->GetEngineEntity());
            pWidget->childs_names.erase(pWidget->childs_names.begin() + nOldWeigth);

            if(eType == Entity::eSymbolInstance)
                pWidget->childs.erase(pWidget->childs.begin() + nOldWeigth + pDescriptor->ChildsCount());
            else 
                pWidget->childs.erase(pWidget->childs.begin() + nOldWeigth);

            AddToWidget(this,pChildWidget,nNewWeight);
            Entity* pSymbol = GetSymbol();
            if(pSymbol)
                pSymbol->ReInitInstances();
        }

    }

    bool Entity::IsContains(Entity* pEntity)
    {
        for(TChilds::iterator it = vChilds.begin();it != vChilds.end();++it)
        {
            if (*it == pEntity)return true;
            else
            {
               if ((*it)->IsContains(pEntity))
               {
                   return true;
               }
            }
        }
        return false;
    }

    void Entity::SetDescriptorName(const string& sName)
    {
        sSymbolName = sName;
        if (eType == Entity::eSymbolInstance && pEntity)
        {
            NGUISymbolInstance* pWidget = static_cast<NGUISymbolInstance*>(pEntity);
            pWidget->symbol_name = sName;
        }
    }

	bool Entity::RollBackLocalChanges()
	{
		if (bIncluded && pEntity && pScene && pScene->Instance())
		{
            bool bRet = false;
    	    if (pScene->Instance()->ReloadIncludedEntity(pEntity, GetScenePath()))
		    {
                bRet = true;
                if (GetEngineEntity() && (IsWidget() || eType == Entity::eSymbol))
                {
                    NGUIWidget* pWidget = static_cast<NGUIWidget*>(GetEngineEntity());
                    std::vector<Entity*> vToDelete;
                    for(TChilds::iterator it = vChilds.begin();it != vChilds.end();++it)
                    {
                        unsigned i = 0;
                        for(;i < pWidget->childs_names.size();++i)
                        {
                            if(String::IsEqual((*it)->Name().c_str(),pWidget->childs_names[i].data))
                                break;
                        }
                        if(i >= pWidget->childs_names.size())
                            vToDelete.push_back((*it));
                    }
                    if(vToDelete.size() > 0)
                    {
                        for(unsigned i = 0;i < vToDelete.size();++i)
                            vToDelete[i]->Delete(false);
                        vChildsNames.clear();
                        vChilds.clear();
                        for(unsigned i = 0;i < pWidget->childs_names.size();++i)
                            vChildsNames.push_back(pWidget->childs_names[i].data);

                        pScene->FindChilds(this);
                        unsigned i = 0;
                        for(TChilds::iterator it = vChilds.begin();it != vChilds.end();++it,++i)
                            (*it)->SetWeight(i);

                        Entity* pSymbol = GetSymbol();
                        if(pSymbol)
                            pSymbol->ReInitInstances();
                    }
                }
		    }

            for(TChilds::iterator it = vChilds.begin();it != vChilds.end();++it)
                bRet |= (*it)->RollBackLocalChanges();

            return bRet;
		}
		return false;
	}
	const char* Entity::GetScenePath()
	{
        if(IsIncluded())
        {
            if(sSourceScenePath.empty())return NULL;
            else return sSourceScenePath.c_str();
        }
        else
        {
		    if (pScene && pScene->Instance())
		    {
			    return pScene->Instance()->sc_filename;
		    }
        }
	   return NULL;
	}

    bool Entity::IsHaveLocalChanges() const
    {
        for(TChilds::const_iterator it = vChilds.begin();it != vChilds.end();++it)
        {
            if((*it)->IsHaveLocalChanges())
                return true;
        }
        if(pScene->Instance())
            return pScene->Instance()->HaveDifferences(pEntity);
        return false;
    }

    int CompareAlphapedOrder(const void* first,const void* second)
    {
        if(String::CompareAlphapedOrder(*(const char**)first,*(const char**)second))return -1;
        else return 1;
    }

    void Scene::GetEntityClassesNames(vector<const char*>& vNames,const std::vector<string>& vEntityClassesTypes)
    {
        for (EntityDedeclarator* decl = EntityDedeclarator::GetFirst(); decl != NULL; decl = decl->GetNext())
        {
            if (String::IsEqual(decl->GetName(), "NGUISymbolInstance") || String::IsEqual(decl->GetName(), "NGUIWidget") || String::IsEqual(decl->GetName(), "NGUISymbol"))continue;
            unsigned i = 0;
            for(;i < vEntityClassesTypes.size();++i)
                if(String::IsEqual(decl->GetNodePanelSheetName(),vEntityClassesTypes[i].c_str()))break;
            if(i < vEntityClassesTypes.size())
                vNames.push_back(decl->GetName());
        }
        qsort(&vNames[0],vNames.size(),sizeof(const char*),CompareAlphapedOrder);
    }


    void   Scene::ReLoadInstance(bool bClear/* = false*/)
    {
        if(pScene)
        {
            pScene->Save(Path(), IStreamSaver::xml);	

            pScene->Clear();
            pScene->ResetIncludedScene();				

            for (size_t i=0;i<vIncludes.size();i++)
                pScene->IncludeScene(vIncludes[i].c_str());

            pScene->LoadImpl();
            ReLoad(bClear);
        }
    }

    void Scene::ReLoad(bool bClear /* = false*/)
    {
        if(pScene)
        {
			std::vector<Entity*> vAdded;
            std::vector<Entity*> vCorrupted;
            if(bClear)
            {
                vEntities.Clear();
                rSymbolsTree.Clear();
                rSceneTree.Clear();
                rGuiTree.Clear();
            }
            for (unsigned i = 0; i < pScene->entities.size(); ++i)
            {
                SceneEntity* pEngineEntity =  pScene->entities[i];
                Entity* pEntity = Find(pEngineEntity->name);
				if(pEntity)
                {
                    if((pEntity->Type() == Entity::eSymbolInstance && pScene->Find(pEntity->SymbolName().c_str()) == NULL) || 
                        (!pEntity->ParentName().empty() && pScene->Find(pEntity->ParentName().c_str()) == NULL))
                    {
                        vCorrupted.push_back(pEntity);
                    }
                }
                else
                {
                    pEntity = CreateEntity(pEngineEntity);
                    if((pEntity->Type() == Entity::eSymbolInstance && pScene->Find(pEntity->SymbolName().c_str()) == NULL) ||
                        (!pEntity->ParentName().empty() && pScene->Find(pEntity->ParentName().c_str()) == NULL))
                    {
                        vCorrupted.push_back(pEntity);
                    }
                    else
                    {
                        bool bNoParent = pEntity->ParentName().empty();
                        if(pEntity->Type()  ==  Entity::eSymbol || pEntity->Type() == Entity::eSymbolInstance || pEntity->Type() == Entity::eWidget)
                        {
                            NGUIWidget* pWidget = static_cast<NGUIWidget*>(pEngineEntity);
                            bNoParent = String::IsEmpty(pWidget->parent_path);
                            for (unsigned i= 0; i < pWidget->childs_names.size(); ++i)
                                pEntity->AddChild(pWidget->childs_names[i].data);
                        }
                        if(bNoParent)
                            vAdded.push_back(pEntity);
                    }
                }
            }
            for(unsigned i= 0;i < vCorrupted.size();++i)
            {
                Entity* pEntity = vCorrupted[i];
                vEntities.Delete(pEntity->Name());
                pEntity->Delete();
            }
			if(vAdded.size() > 0)
			{
                for(unsigned i= 0;i < vAdded.size();++i)
                    vAdded[i]->FindChilds(vEntities);

				for(unsigned i= 0;i < vAdded.size();++i)
				{
                    Entity* pEntity = vAdded[i];
                    if(pEntity->Type() == Entity::eSymbolInstance)
                    {
                        Entity*  pSymbol = Find(pEntity->SymbolName());
                        if(pSymbol)
                        {
                            pEntity->SetDescriptor(pSymbol);
                            pSymbol->AddInstance(pEntity);
                        }
                    }
                    if(pEntity->Type() == ProjectInfo::Entity::eEntity)
                        rSceneTree.Add(pEntity);
                    else
                        if(pEntity->Type() == ProjectInfo::Entity::eSymbol)
                            rSymbolsTree.Add(pEntity);
                        else
                            rGuiTree.Add(pEntity);


				}
				rSymbolsTree.InitView();
				rSceneTree.InitView();
                rGuiTree.InitView();
			}
        }
    }


    void Scene::Load(const char* sPath,IStreamLoader::FileType data_type)
    {
        sFilePath = sPath;
        for (unsigned i=0;i < vIncludes.size();i++)
            LoadEntities(vIncludes[i].c_str(),data_type,true);
        LoadEntities(sPath,data_type);
        BuildSceneTree();
    }

    void Scene::Load(IStreamLoader* pLoader)
    {
        char buff[256];
        pLoader->Read("Name", buff, 128);
        sName = buff;
        memset(buff,0,256);
        pLoader->Read("ScreenName", buff, 128);
        sScreenName = buff;
        if(sScreenName[0] == 0)
        {
            sScreenName = sName;
            sScreenName.erase(sScreenName.size() - 3,3);
        }
        pLoader->Read("Path", buff, 512);
        sFilePath = buff;
        pLoader->Read("OnlySrt", bScript);
        bOpened = false;
        pLoader->Read("Opened",bOpened);
        if (!bScript)
        {
            vIncludes.clear();
            while (pLoader->EnterBlock("IncludedScene"))
            {	
                pLoader->Read("Path",buff, 256);			
                vIncludes.push_back(buff);
                pLoader->LeaveBlock();
            }				
        }
    }

    void Scene::LoadEntities(const char* sPath,IStreamLoader::FileType data_type,bool bIncluded)
    {
        IStreamLoader* pLoader = core->Files()->CreateStreamLoader(sPath, "scene", data_type);
        if(pLoader)
        {
            while (pLoader->EnterBlock("Entity"))
            {			
                Entity* pEntity = new Entity(this,bIncluded);
                pEntity->Load(pLoader);
                if(bIncluded)
                    pEntity->SetSourceScenePath(sPath);
                vEntities[pEntity->Name()] = pEntity;
                pLoader->LeaveBlock();
            }
            pLoader->Release();
        }
        char sPath_isi[256];
        String::Copy(sPath_isi, 256, sPath);
        String::RemoveExctention(sPath_isi);
        String::Cat(sPath_isi,256,".isi");
        pLoader = core->Files()->CreateStreamLoader(sPath_isi, "include", data_type);				
        if (pLoader)
        {
            pLoader->SetNeedLogWarnings(false);
            while (pLoader->EnterBlock("Entity"))
            {
                Str128 sName;
                pLoader->Read("entityName", sName.data, 128);
                Entity* pEntity = Find(sName.data);
                if (pEntity)
                {
                    pEntity->Load(pLoader);
                }
                pLoader->LeaveBlock();
            }				

            pLoader->Release();
        }

    }

    void Scene::BuildSceneTree()
    {
        TEntities::Iterator it = vEntities.Begin();
        while(it != vEntities.End())
        {
            Entity**  ppEntity = *it;
            if(ppEntity)
            {
               Entity*  pEntity = *ppEntity;
               pEntity->FindChilds(vEntities);
               if(pEntity->Type() == Entity::eSymbolInstance)
               {
                   Entity*  pSymbol = Find(pEntity->SymbolName());
                   if(pSymbol)
                   {
                       pEntity->SetDescriptor(pSymbol);
                       pSymbol->AddInstance(pEntity);
                   }
               }
            }
            ++it;
        }
    }

    void Scene::GetTopList(Entity::TTypeList& eTypes,TEntitiesList& vList)
    {
        TEntities::Iterator it = vEntities.Begin();
        while(it != vEntities.End())
        {
            Entity**  ppEntity = *it;
            if(ppEntity)
            {
                Entity* pEntity = *ppEntity;
                if(!pEntity->Parent())
                {
                    for(unsigned i = 0;i < eTypes.size();++i)
                    {
                        if(pEntity->Type() == eTypes[i])
                        {
                            vList.push_back(pEntity);
                            break;
                        }
                    }
                }
            }
            ++it;
        }
        vList.sort(Entity::Comparator());
    }
    
    Entity* Scene::GetLastEntity(Entity* pParent,const std::vector<string>& vPath,unsigned offset,unsigned endOffset)
    {
        for(unsigned k = offset; k < vPath.size() - endOffset;++k)
        {
            Entity* pChild = pParent->GetChild(vPath[k]);
            if(pChild)
            {
                if(k + 1 >= vPath.size())
                    return pChild;
                else
                {
                    pParent = pChild;
                    continue;
                }
            }
            if(pParent->Type() == Entity::eSymbolInstance)
            {
                pParent = Find(pParent->SymbolName());
                if(pParent)
                {
                    Entity* pChild = pParent->GetChild(vPath[k]);
                    if(pChild)
                    {
                        if(k + 1 >= vPath.size())
                            return pChild;
                        else
                        {
                            pParent = pChild;
                            continue;
                        }
                    }
                }
            }
        }
        return NULL;
    }

    void Scene::GetMembersInScene(const std::vector<string>& vPath,std::vector<string>& vMembers)
    {
        if(vPath.size() == 1)
        {
            TEntities::Iterator it = vEntities.Begin();
            while(it != vEntities.End())
            {
                Entity**  ppEntity = *it;
                if(ppEntity)
                {
                    Entity*  pEntity = *ppEntity;
                    if(pEntity->Type() == Entity::eSymbol)
                    {
                        vMembers.push_back("Symbol" + pEntity->Name());
                        vMembers.push_back("SymbolInst" + pEntity->Name());
                    }
                    else
                       vMembers.push_back(pEntity->Name());
                }
                ++it;
            }
        }
        else
        {
            const string& sLast = vPath.back();
            if(sLast[sLast.size() - 1] == ')')
            {
                std::vector<string> vNewPath,vSignatures;string sSignature;
                vNewPath.assign(vPath.begin(),vPath.end());
                string& sLast = vNewPath.back();
                unsigned paramsCount = 0;
                int pos = sLast.find("(",0);
                for(unsigned i = pos;i < sLast.size() - 1;++i)
                    if(sLast[i] == ',')++paramsCount;
                sLast.erase(pos,string::npos);
                GetSignatureInScene(vNewPath,vSignatures);
                if(vSignatures.size() > 0)
                    for(unsigned i = 0; i < vSignatures.size();++i)
                    {
                        unsigned params = 0;
                        int pos = vSignatures[i].find("(",0);
                        for(unsigned j = pos;j < vSignatures[i].size() - 1;++j)
                            if(vSignatures[i][j] == ',')++params;
                        if(params > 0)++params;
                        if(params == paramsCount)
                        {
                            sSignature = vSignatures[i];
                            break;
                        }
                    }
                    pos = sSignature.find("@",0);
                    if(pos != string::npos)
                    {
                        sSignature.erase(pos,string::npos);
                        if(sSignature.find("SymbolInst") == 0)
                        {
                            Entity*  pEntity = Find(sSignature);
                            if(pEntity)
                                NGUISymbol::GetMembersForAutoComplete(pEntity->IsUseInstance(),vMembers);
                            else
                                NGUISymbol::GetMembersForAutoComplete(false,vMembers);
                        }
                        else
                            if(sSignature.find("Symbol") == 0)
                                NGUISymbol::GetMembersForAutoComplete(true,vMembers);
                            else
                                core->Script()->Machine()->GetMembers(sSignature,vMembers);
                    }

            }
            if(vPath.size() > 1)
            {
                string sName = vPath[1];
                Entity* pEntity = Find(sName);
                if(pEntity)
                {
                    if(vPath.size() > 2)
                    {
                        if(pEntity->Type() == Entity::eWidget || pEntity->Type() == Entity::eSymbol || pEntity->Type() == Entity::eSymbolInstance)
                        {
                            pEntity  =  GetLastEntity(pEntity,vPath,2);
                            if(pEntity)
                                pEntity->GetMembers(vMembers);
                        }
                    }
                    else
                        pEntity->GetMembers(vMembers);
                }
            }
        }
    }

    void Scene::GetSignatureInScene(const std::vector<string>& vPath,std::vector<string>& vSignatures)
    {
        if(vPath.size() > 2)
        {
            string sName = vPath[1];
            Entity* pEntity = Find(sName);
            if(pEntity)
            {
                Entity* pLast = NULL;
                if(pEntity->Type() == Entity::eWidget || pEntity->Type() == Entity::eSymbol || pEntity->Type() == Entity::eSymbolInstance)
                {
                    pLast =  GetLastEntity(pEntity,vPath,2,1);
                    if(pLast)
                        pEntity = pLast;
                }
                if(!pLast &&  pEntity->Type() == Entity::eSymbol || pEntity->Type() == Entity::eSymbolInstance)
                    NGUISymbol::GetSignatureForAutoComplete(pEntity->Type() == Entity::eSymbol,sName,vPath[vPath.size() - 1],vSignatures);
                else
                {
                    if(pEntity->Type() == Entity::eWidget || pEntity->Type() == Entity::eEntity)
                        core->Script()->Machine()->GetSignature(pEntity->ClassName(),vPath[vPath.size() - 1].c_str(),vSignatures);
                    else
                        if(pEntity->Type() == Entity::eSymbolInstance)
                            NGUISymbol::GetSignatureForAutoComplete(pEntity->IsUseInstance(),pEntity->SymbolName(),vPath[vPath.size() - 1],vSignatures);
                }

            }
        }

    }

    void Scene::Save()
    {
        char path[512];
        String::Printf(path, 512, "%s", sFilePath.c_str());
        path[strlen(path) - 1] = 't';
        IStreamSaver* pSaver = core->Files()->CreateStreamSaver(path, IStreamSaver::xml);	
        pSaver->MarkBeginBlock("NodesList");
        pSaver->Write("Version",IST_FILE_VERSION);
        rSceneTree.Save(pSaver);
        rSymbolsTree.Save(pSaver);
        pSaver->MarkEndBlock("NodesList");
        pSaver->Release();
    }

    void Scene::Save(IStreamSaver* saver)
    {
        saver->Write("Name", Name());
        saver->Write("ScreenName",ScreenName());
        saver->Write("Path", Path());
        saver->Write("OnlySrt", bScript);
        saver->Write("Opened",bOpened);
        if (!bScript)
        {
            for (int j=0; j<vIncludes.size(); j++)
            {
                saver->MarkBeginBlock("IncludedScene");
                saver->Write("Path",vIncludes[j].c_str());
                saver->MarkEndBlock("IncludedScene");
            }				
        }
    }

    bool Scene::LoadSceneTreeCash()
    {
        if(sFilePath.empty())return false;
        char sPath[512];
        String::Printf(sPath, 512, "%s",sFilePath.c_str());
        sPath[strlen(sPath) - 1] = 't';
        IStreamLoader* pLoader = core->Files()->CreateStreamLoader(sPath, "NodesList", IStreamLoader::xml);	
        if (pLoader)
        {
            int nVersion = -1;
            pLoader->Read("Version",nVersion);
            if(nVersion != IST_FILE_VERSION)return false;
            rSceneTree.Load(pLoader,this);
            rSymbolsTree.Load(pLoader,this);
            TEntities::Iterator it = vEntities.Begin();
            while(it != vEntities.End())
            {
                Entity**  ppEntity = *it;
                if(ppEntity)
                {
                    Entity*  pEntity = *ppEntity;
                    if(pEntity->IsSymbol())
                    {
                        if(rSymbolsTree.IsMiss(Node::eEntity,pEntity->Name()))
                            rSymbolsTree.Merge(pEntity);
                    }
                    else
                        if(pEntity->Type() == Entity::eEntity)
                        {
                            if(rSceneTree.IsMiss(Node::eEntity,pEntity->Name()))
                                rSceneTree.Merge(pEntity);
                        }
                    if(pScene)
                       pEntity->SetEngineEntity(pScene->Find(pEntity->Name().c_str()));
                }
                ++it;
            }

            return true;
        }
        return false;
    }
    void Scene::LoadSceneTrees()
    {	
        rSceneTree.ClearNodes();
        rSymbolsTree.ClearNodes();
        if (!LoadSceneTreeCash())
        {
                TEntities::Iterator it = vEntities.Begin();
                while(it != vEntities.End())
                {
                    Entity**  ppEntity = *it;
                    if(ppEntity)
                    {
                        Entity*  pEntity = *ppEntity;

                        if(pScene)
                            pEntity->SetEngineEntity(pScene->Find(pEntity->Name().c_str()));

                        if(pEntity->Type() == ProjectInfo::Entity::eEntity)
                            rSceneTree.Add(pEntity);
                        else
                            if(pEntity->Type() == ProjectInfo::Entity::eSymbol)
                                rSymbolsTree.Add(pEntity);
                    }
                    ++it;
                }
        }
    }

    void Scene::RenameEntity(const string& sOldName,const string& sNewName)
    {
        Entity* pEntity =  Find(sOldName);
        if(pEntity)
        {
            vEntities[sNewName] = pEntity;
            vEntities.Delete(sOldName);
        }
    }

    void Scene::Remove(Entity* pEntity)
    {
        pEntity->Delete();
        vEntities.Delete(pEntity->Name());
        delete pEntity;
    }

    const char* Scene::GenerateName(const char* sBase)
    {
        static char sResult[256];
        static char sBuffer[256];
        int number = 0;
        String::ExtractNameNumber(sBase, sBuffer, 64, number);
        bool satisfy = false;
        while (!satisfy)
        {		
            String::Printf(sResult, 256, "%s%i", sBuffer, number);
            satisfy = !(Find(sResult));
            number++;
        }
        return sResult;
    }

    ISceneTree* Scene::GetEntityTree(SceneEntity* pEngineEntity)
    {
        Entity* pEntity = Find(pEngineEntity->GetName());
        if(pEntity)
            return pEntity->Tree();
        return NULL;
    }

    Entity* Scene::CreateEntity(const char* sName,const Matrix& rTansform,SceneEntity* pDesc, /*= NULL*/const char* sEntityName/*= NULL*/)
    {
        if(pScene)
        {
            SceneEntity* pEntity = pScene->AddSceneEntity(sName);
            if(pEntity)
            {
                pEntity->SetDefault();
                if (pDesc)
                {
                    string  sNewName = string(pDesc->GetName()) +"Instance";
                    if(sEntityName)
                       pEntity->SetName(sEntityName); 
                    else
                       pEntity->SetName(GenerateName(sNewName.c_str()));
                }
                else
                {
                    if(sEntityName)
                        pEntity->SetName(sEntityName); 
                    else
                        pEntity->SetName(GenerateName(sName));
                }
                if(pDesc && String::IsEqual(pEntity->className, "NGUISymbolInstance") && String::IsEqual(pDesc->className, "NGUISymbol"))
                {
                    NGUISymbolInstance* pCreated = static_cast<NGUISymbolInstance*>(pEntity);
                    pCreated->CopyProperties(static_cast<NGUIWidget*>(pDesc));
                    pCreated->symbol_name = pDesc->GetName();
                }
                pEntity->SetTransform(rTansform);
                pEntity->ReInit();
                return CreateEntity(pEntity);
            }
        }
        return NULL;
    }


    Entity* Scene::CopyEntity(const Entity* pSource)
    {
        if(pSource)
        {
            SceneEntity* pEngineSource = pSource->GetEngineEntity();
            if(pEngineSource)
            {
                Matrix transform;Vector scale;
                pEngineSource->GetTransform(transform);
                pEngineSource->GetScale(scale);

                if(pSource->Type() == Entity::eSymbolInstance)
                {
                   Entity* pSymbol = pSource->Descriptor(); 
                   if(pSymbol)
                   {
                       Entity* pEntity = CreateEntity("NGUISymbolInstance",transform,pSymbol->GetEngineEntity(), GenerateName(pEngineSource->GetName()));
                       SceneEntity* pEngineEntity = pEntity->GetEngineEntity();
                       if(pEngineEntity)
                       {
                           pEngineSource->Copy(pEngineEntity);
                           pEngineEntity->ReInit();
                       }
                       return pEntity;
                   }
                   return NULL;
                }
                else
                {
                    SceneEntity* pEngineEntity = pScene->AddSceneEntity(pEngineSource->Desc()->GetName());
                    pEngineEntity->SetDefault();
                    pEngineSource->Copy(pEngineEntity);

                    pEngineEntity->SetName(GenerateName(pEngineSource->GetName()));
                    String::Copy(pEngineEntity->assetName, 64, pEngineSource->assetName);

                    Entity*  pEntity = CreateEntity(pEngineEntity);

					if(pSource->IsWidget())
					{
						NGUIWidget* pWidgetSource = static_cast<NGUIWidget*>(pEngineSource);
						for(size_t i = 0;i < pWidgetSource->childs.size();++i)
						{
							Entity* pChild = CopyEntity(Find(pWidgetSource->childs[i]->name));
							if(pChild)
								pChild->ChangeParent(pEntity,i);
						}
					}

                    pEngineEntity->SetTransform(transform);
                    pEngineEntity->SetScale(scale);
                    pEngineEntity->ReInit();					
                    return pEntity;
                }
            }
        }
        return NULL;
    }

    ISceneTree* Scene::FindTree(void* pHandle)
    {
        if(rSymbolsTree.IsPresent(pHandle))
            return &rSymbolsTree;
        if(rSceneTree.IsPresent(pHandle))
            return &rSceneTree;
        if(rGuiTree.IsPresent(pHandle))
            return &rGuiTree;
        return NULL;
    }

    Entity* Scene::ConvertToSymbol(Entity* pDesc)
    {
        if (pScene && pDesc && pDesc->GetEngineEntity() && pDesc->IsWidget())
        {
            NGUISymbol*  pSymbol = static_cast<NGUISymbol*>(pScene->AddSceneEntity("NGUISymbol"));
            pSymbol->Init();
            pSymbol->SetDefault();
            pSymbol->CopyProperties(static_cast<NGUIWidget*>(pDesc->GetEngineEntity()));
            pSymbol->SetName(GenerateName("NGUISymbol"));
            Entity *pEtity = CreateEntity(pSymbol);
            pEtity->Copy(pDesc);
            vEntities[pSymbol->GetName()] = pEtity;
            return pEtity;
        }
        return NULL;
    }

    Entity* Scene::CreateEntity(SceneEntity* pEntity)
    {
        Entity* pWrapper =  new Entity(this,pEntity);
        vEntities[pWrapper->Name()] = pWrapper;
        return pWrapper;
    }

    void Scene::OnPropertiesChanged(SceneEntity* pEngineEntity)
    {
        if(pEngineEntity)
        {
            Entity* pEntity = Find(pEngineEntity->GetName());
            if(pEntity)
            {
                Entity* pSymbol = pEntity->GetSymbol();
                if(pSymbol)
                    pSymbol->ReInitInstances();
            }
        }
    }

    Scene::~Scene()
    {
        TEntities::Iterator it = vEntities.Begin();
        while(it != vEntities.End())
        {
            Entity**  ppEntity = *it;
            if(ppEntity)
                delete (*ppEntity);
            ++it;
        }
        vEntities.Clear();
    }

    void Scene::OnEntityVisibleChange(SceneEntity* pEngineEntity)
    {
        Entity* pEntity = Find(pEngineEntity->GetName());
        if(pEntity)
        {
          Entity* pSymbol = pEntity->GetSymbol();
          if(pSymbol)
              pSymbol->ReInitInstances();
        }
    }

}


#endif