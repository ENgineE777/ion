
#if defined(PC) || defined(OSUNIX)
#include "SceneTree.h"
#include "SceneInfo.h"
#include "Core/Files/IStreamSaver.h"
#include "Core/Files/IStreamLoader.h"
#include "Entities/NGUI/NGUIService.h"
#include "Entities/NGUI/NGUIWidget.h"
#include <algorithm>
#include "EditorProject.h"

namespace ProjectInfo
{
    Node::Node(ProjectInfo::Entity* _pEntity):eType(eEntity),pEntity(_pEntity),sName(_pEntity->Name())
    {

    }

    Node::Node(IStreamLoader* pLoader)
    {
        char sBuff[128];
        pLoader->Read("Name",sBuff,128);
        sName = sBuff;
        pLoader->Read("Type",eType);
    }

    void Node::Save(IStreamSaver* pSaver) const
    {
        pSaver->MarkBeginBlock("Node");
        pSaver->Write("Name", sName.c_str());
        pSaver->Write("Type",eType);
        pSaver->MarkEndBlock("Node");
    }

    void ISceneTree::Load(IStreamLoader* pLoader,Scene* pScene)
    {
        if(pLoader->EnterBlock(sName.c_str()))
        {
            while (pLoader->EnterBlock("Node"))
            {
                Node node(pLoader);
                if (node.eType == Node::eEntity)
                {
                    Entity* pEntity = pScene->Find(node.sName);
                    if(pEntity)
                        vTree.push_back(node);
                }
                else
                    vTree.push_back(node);
                pLoader->LeaveBlock();
            }
            pLoader->LeaveBlock();
        }
    }

    void ITree::Save(IStreamSaver* pSaver)
    {
        if(pView)
            GrabTree(vTree);
        pSaver->MarkBeginBlock(sName.c_str());
        for (size_t j = 0; j < vTree.size();++j)
            vTree[j].Save(pSaver);
        pSaver->MarkEndBlock(sName.c_str());
    }

    void ISceneTree::Merge(Entity* pEntity)
    {
        int nAfterIndex = -1;
        Entity* pParent = pEntity->Parent();
        if(pParent)
        {
            string sAfterName;
            Entity* pChild = pParent->GetChildBefore(pEntity->Name());
            if(pChild)
                sAfterName = pChild->Name();
            if(!sAfterName.empty())
            {
                nAfterIndex =  Find(Node::eEntity,sAfterName);
                if(nAfterIndex + 1 < (int)vTree.size() && vTree[nAfterIndex + 1].eType == Node::eStartBlock)
                {
                    int nCount = 0;
                    for(unsigned i = nAfterIndex + 1;i < vTree.size();++i)
                    {
                        if(vTree[i].eType == Node::eStartBlock)++nCount;
                        if(vTree[i].eType == Node::eEndBlock)
                        {
                            --nCount;
                            if(nCount == 0)
                            {
                                nAfterIndex = i;
                                break;
                            }
                        }
                    }
                }

            }
            else
            {
                nAfterIndex =  Find(Node::eEntity,pParent->Name());
                if(nAfterIndex == -1)
                    pParent->AddToTreeList(vTree);
                else
                    ++nAfterIndex;
            }
            if(nAfterIndex != -1)
            {
                vector<Node> vBuff;
                pEntity->AddToTreeList(vBuff);
                vTree.insert(vTree.begin() + (nAfterIndex + 1),vBuff.begin(),vBuff.end());
            }
        }
        else
            pEntity->AddToTreeList(vTree);

    }

    int ISceneTree::Find(Node::ENodeType eType,const string& sName)
    {
        for(unsigned i = 0;i < vTree.size();++i)
        {
            if(vTree[i].eType == eType && sName  == vTree[i].sName)
                return  i;
        }
        return -1;
    }

    void ISceneTree::InitView(bool bAddChilds)
    {
        std::vector<void*>  vParents;
        void* last_item = 0;
        vParents.push_back(NULL);
        pView->DeleteItems();
        for (size_t i=0;i < vTree.size();++i)
        {
            if (vTree[i].eType == ProjectInfo::Node::eEntity)
            {
                Entity* pEntity = pScene->Find(vTree[i].sName);
                if (pEntity && IsValidNode(pEntity))
                {
                    last_item = pView->AddEntityItem(vTree[i].sName.c_str(), pEntity->IsIncluded(), pEntity,vParents.back(), pEntity->Weight());
                    pEntity->SetTree(this);
                    pEntity->SetHandle(last_item);
                    if(bAddChilds)
                         pView->AddItem("childs",0, NULL, last_item, null);
                }
            }
            else
                if (vTree[i].eType == ProjectInfo::Node::eFolder)
                    last_item = pView->CreateFolder(vTree[i].sName.c_str(),vParents.back(),null);
                else
                    if (vTree[i].eType == ProjectInfo::Node::eStartBlock)vParents.push_back(last_item);
                    else
                        if (vTree[i].eType == ProjectInfo::Node::eEndBlock)vParents.pop_back();
        }

    }

    void ISceneTree::Add(Entity* pEntity)
    {
        pEntity->AddToTreeList(vTree);
    }

    void ISceneTree::Copy(const ISceneTree& other)
    {
        sName = other.sName;
        pView = other.pView;
        vTree.assign(other.vTree.begin(),other.vTree.end());
    }
    
    void ISceneTree::RecursiveCopy(void* pHandle,void* pParentHandle)
    {
        void* pIter = pView->GetFirstItem(pHandle);
        void* pLast = NULL;
        while(pIter)
        {
            void* pChild = pIter;
            pIter = pView->GetNextItem(pIter);
            CopyEntity(pChild,pParentHandle);
        }
    }

    bool ISceneTree::Rename(void* pHandle,const char* sNewName /*= NULL*/)
    {
        bool bGenerate = false;
        if(!sNewName)
            sNewName = pView->GetEditedText();
        if(sNewName)
        {
            Entity* pEntity = static_cast<Entity*>(pView->GetHandleData(pHandle));
            if(pEntity)
            {
                if(pEntity->IsIncluded())
                {
                    pView->SetItemText(pHandle, pEntity->Name().c_str());	
                    return false;
                }
                if(sNewName)
                {
                    string sOldName = pEntity->Name();
                    bGenerate = pEntity->Rename(sNewName);
                    pView->SetItemText(pHandle, pEntity->Name().c_str());
                    pScene->RenameEntity(sOldName,pEntity->Name());
                }
            }
            else
            {
                if(pView->CheckFolderName(pView->GetParent(pHandle),sNewName))
                    pView->SetItemText(pHandle, sNewName);	
            }

        }
        return bGenerate;
    }

    bool ISceneTree::Rename(const char* sOldName,const char* sNewName /*= NULL*/)
    {
        Entity*  pEntity = pScene->Find(sOldName);
        if(pEntity)
        {
            return Rename(pEntity->Handle(),sNewName);
        }
        return false;
    }

    void* ISceneTree::InsertEntity(void* pParentHandle,Entity* pEntity,int weight,bool bAddChildsRoot)
    {
        if(pParentHandle)
        {
            const char* sName = pView->GetItemText(pParentHandle);
            if(String::IsEqual(sName,"childs"))
                pParentHandle  = pView->GetParent(pParentHandle);
            else
                if(String::IsEqual(sName,"root"))
                    pParentHandle = NULL;
        }
        pEntity->SetWeight(weight);
        void* pHandle =  pEntity->AddToTreeView(this,pParentHandle,NULL,bAddChildsRoot);
        if(pParentHandle)
        {
            Entity* pParent = static_cast<Entity*>(pView->GetHandleData(pParentHandle));
            if(pParent)
                pEntity->ChangeParent(pParent,weight);

        }
        return pHandle;
    }

    Entity* ISceneTree::GetEntity(void* pHandle)
    {
        Entity* pEntity = static_cast<Entity*>(pView->GetHandleData(pHandle));
        if(pEntity)
            return pEntity;
        else
            if(pHandle)
            {
                if(String::IsEqual(pView->GetItemText(pHandle),"childs"))
                    return GetEntity(pView->GetParent(pHandle));
            }
        return NULL;
    }

    bool ISceneTree::IsDeletedEntitySelected(Entity* pEntity,void* pSelected)
    {
        if(pEntity->Handle() == pSelected)return true;
        else
        {
            if(IsDeletedSelected(pEntity->Handle(),pSelected))
                return true;
        }
        return false;
    }

    bool ISceneTree::IsDeletedSelected(void* pDeleted,void* pSelected)
    {
        if(pDeleted)
        {
            Entity* pEntity = static_cast<Entity*>(pView->GetHandleData(pDeleted));
            if(pEntity)
            {
                for(Entity::Iterator it = pEntity->ChildsStart();it != pEntity->ChildsEnd();++it)
                {
                    Entity*  pChild =  *it;
                    if(IsDeletedEntitySelected(pChild,pSelected))
                        return true;
                }
                if(pEntity->Type() == Entity::eSymbol)
                {
                    for(Entity::Iterator it = pEntity->InstancesStart();it != pEntity->InstancesEnd();++it)
                    {
                        Entity*  pChild =  *it;
                        if(IsDeletedEntitySelected(pChild,pSelected))
                            return true;
                    }
                }
                return false;
            }
            else
            {
                void* pIter = pView->GetFirstItem(pDeleted);
                while(pIter)
                {
                    if(IsDeletedSelected(pIter,pSelected))
                        return true;
                    pIter = pView->GetNextItem(pIter);
                }
            }
        }
        return false;
    }
    void ISceneTree::Delete(void* pHandle)
    {
        if(pHandle)
        {
            Entity* pEntity = static_cast<Entity*>(pView->GetHandleData(pHandle));
            if(pEntity)
            {
                if(!CanDelete(pEntity))
                    return;
                pScene->Remove(pEntity);
            }
            else
            {
                void* pIter = pView->GetFirstItem(pHandle);
                while(pIter)
                {
                    void* pChild = pIter;
                    pIter = pView->GetNextItem(pIter);
                    Delete(pChild);
                }
            }
        }
    }

    Entity* ISceneTree::CopyEntity(void* pHandle,void* pParentHandle)
    {
        if(pHandle)
        {
            Entity* pEntity = static_cast<Entity*>(pView->GetHandleData(pHandle));
            if(pEntity)
            {
                if(!pParentHandle)
                    pParentHandle = pView->GetParent(pHandle);
                Entity* pNewEntity = pEntity->Copy();
                pNewEntity->AddToTreeView(this,pParentHandle,pEntity->Handle(),pNewEntity->IsWidget() || (pNewEntity->Type() == Entity::eSymbol));
                if(pParentHandle)
                {
                    Entity* pParentEntity = static_cast<Entity*>(pView->GetHandleData(pParentHandle));
                    if(pParentEntity)
                        pNewEntity->ChangeParent(pParentEntity,pEntity->Weight() + 1);
                }
                else
                    pNewEntity->SetWeight(pEntity->Weight() + 1);
                Sort(pHandle);
                return pNewEntity;
            }
            else
                RecursiveCopy(pHandle,pView->CreateFolder(NULL,pView->GetParent(pHandle),&pHandle));
            Sort(pHandle);
        }
        return NULL;
    }

    Entity* ISceneTree::ConvertToSymbol(Entity* pEntity,ISceneTree* pSymbolList)
    {
        if(pEntity->GetEngineEntity() && pEntity->Type() == Entity::eWidget)
        {
            Entity* pSymbol = pScene->ConvertToSymbol(pEntity);
            if(pSymbol->GetEngineEntity())
            {
                NGUIWidget* pWidget = static_cast<NGUIWidget*>(pEntity->GetEngineEntity());
                void* pParentHandel = NULL;
                if(pEntity->Parent())
                    pParentHandel = pEntity->Parent()->Handle();
                int weight = pEntity->Weight();
                Matrix transform = pWidget->global_mat;
                int x = pWidget->x;
                int y = pWidget->y;
                pScene->Remove(pEntity);
                pEntity = NULL;
                pSymbolList->InsertEntity(NULL,pSymbol,0);
                pSymbolList->Sort();
                Entity* pInstanse = pScene->CreateEntity("NGUISymbolInstance",pWidget->transform,pSymbol->GetEngineEntity());
                if(pInstanse && pInstanse->GetEngineEntity())
                {
                    NGUIWidget* pWidgetInstance = static_cast<NGUIWidget*>(pInstanse->GetEngineEntity());
                    pWidgetInstance->global_mat = transform;
                    pWidgetInstance->x = x;
                    pWidgetInstance->y = y;
                    InsertEntity(pParentHandel,pInstanse,weight);
                }
                return pInstanse;
            }
        }
        return NULL;
    }

    void ITree::GrabTree(vector<ProjectInfo::Node>& vTree)
    {
        vTree.clear();
        if(pView)
            GrabTree(pView->GetRoot(),vTree);
    }

    void ITree::GrabTree(void* pHandle,vector<ProjectInfo::Node>& vTree)
    {
        if (pHandle == 0) return;
        do
        {  
            ISceneTreeView::TreeViewItem rTreeItem;
            pView->GetItemInfo(pHandle,&rTreeItem);
            ProjectInfo::Node node;
            if(rTreeItem.eType != ISceneTreeView::TreeViewItem::eHelper)
            {
                if(rTreeItem.eType == ISceneTreeView::TreeViewItem::eScene)
                {
                    Scene* pScene = static_cast<Scene*>(rTreeItem.pObject);
                    if(pScene)
                        node.sName = pScene->ScreenName();
                }
                else
                    node.sName = pView->GetItemText(pHandle);

                if (pView->IsFolder(pHandle))
                    node.eType = ProjectInfo::Node::eFolder;
                else
                    node.eType = ProjectInfo::Node::eEntity;

                if(!String::IsEqual(node.sName.c_str(), "childs"))
                {
                    if (node.eType == ProjectInfo::Node::eFolder)
                    {
                        vTree.push_back(node);
                        ProjectInfo::Node start;
                        start.eType = ProjectInfo::Node::eStartBlock;
                        vTree.push_back(start);
                        if(pView->GetFirstItem(pHandle))
                            GrabTree(pView->GetFirstItem(pHandle),vTree);
                        ProjectInfo::Node end;
                        end.eType = ProjectInfo::Node::eEndBlock;
                        vTree.push_back(end);
                    }
                    else
                    {
                        vTree.push_back(node);
                        void* pChild = pView->GetFirstItem(pHandle);
                        unsigned nChildsCount = 0;
                        if (pChild)
                        {
                            do
                            {   
                                ++nChildsCount;
                                pChild = pView->GetNextItem(pChild);
                            }
                            while(pChild != 0);
                        }
                        if(nChildsCount > 1)
                        {
                            ProjectInfo::Node start;
                            start.eType = ProjectInfo::Node::eStartBlock;
                            vTree.push_back(start);
                            GrabTree(pView->GetFirstItem(pHandle),vTree);
                            ProjectInfo::Node end;
                            end.eType = ProjectInfo::Node::eEndBlock;
                            vTree.push_back(end);
                        }
                    }
                }
            }
            pHandle = pView->GetNextItem(pHandle);
        }
        while(pHandle != 0);
    }

    void ITree::SetView(ISceneTreeView* _pView)
    {
        if(pView != _pView)
        {
            if(pView)
                GrabTree(vTree);
            pView = _pView;
            if(pView)
            {
                InitView();
                pView->SetTreeLogic(this);
            }
        }
    }

     void ISceneTree::UpdateHandles(const ISceneTreeView::TreeViewItem& rItem)
     {
         Entity* pEntity = pScene->Find(rItem.sName);
         if(pEntity)
             pEntity->SetHandle(rItem.pHandle);
         for(unsigned i =0;i < rItem.vChilds.size();++i)
             UpdateHandles(rItem.vChilds[i]);
     }

    void* ITree::MoveTreeItem(ITree* pSourceList,void* pSource,void* pTarget,void* pInsertAfter)
    {
        ISceneTreeView::TreeViewItem rItem;
        pSourceList->View()->GetItemInfoRecursive(pSource,&rItem);
        void* pHandle = pView->AddItem(pTarget,&rItem,pInsertAfter);
        pSourceList->View()->DeleteItem(pSource);
        UpdateHandles(rItem);
        return pHandle;

    }

    void ISceneTree::MoveTreeItem(ISceneTree* pSourceList,void* pSource,void* pTarget,Entity* pSourceEntity,Entity* pParentSourceEntity,Entity* pParentTargetEntity)
    {
        if(pParentTargetEntity == pParentSourceEntity)
        {
            if(String::IsEqual(pView->GetItemText(pTarget),"childs"))
            {
                pParentTargetEntity->MoveChild(pSourceEntity,0);
                ITree::MoveTreeItem(pSourceList,pSource,pParentTargetEntity->Handle(),pTarget);
            }
            else
            {
                Entity* pTargetEntity = GetEntity(pView->GetNextItem(pTarget));
                if(pTargetEntity == pSourceEntity)return;
                if(pTargetEntity)
                    pParentTargetEntity->MoveChild(pSourceEntity,pTargetEntity->Weight());
                else
                    pParentTargetEntity->MoveChild(pSourceEntity,pParentTargetEntity->ChildsCount());
               ITree::MoveTreeItem(pSourceList,pSource,pParentTargetEntity->Handle(),pTarget);
            }

        }
        else
            if(pSourceEntity != pParentTargetEntity && !pSourceEntity->IsIncluded() && pSourceEntity->IsWidget() && !pSourceEntity->IsContains(pParentTargetEntity))
            {
                Entity* pTargetEntity = GetEntity(pView->GetNextItem(pTarget));
                if(pTargetEntity)
                    pSourceEntity->ChangeParent(pParentTargetEntity,pTargetEntity->Weight());
                else
                    pSourceEntity->ChangeParent(pParentTargetEntity,pParentTargetEntity->ChildsCount());
                ITree::MoveTreeItem(pSourceList,pSource,pParentTargetEntity->Handle(),pTarget);
            }
    }

    void ISceneTree::AddFactoryPopUpMenu(TTreePopUpMenu& vMenu,std::vector<string>& vEntityClassesTypes,Entity* pEntity)
    {
        vector<const char*> vNames;
        pScene->GetEntityClassesNames(vNames,vEntityClassesTypes);
        TreePopUpMenuItem factory = TreePopUpMenuItem(ePopUpSubMenuId,"Create",true);
        factory.vSubMenu.push_back(TreePopUpMenuItem(ePopUpSubMenuId,"Instances",true));
        static_cast<SceneSymbolsTree*>(pScene->SymbolsTree())->ConvertSymbolsTreeInMenu(factory.vSubMenu[0],pEntity);

        for(unsigned i =0;i < vNames.size();++i)
            factory.vSubMenu.push_back(TreePopUpMenuItem(ePopUpCreateId + i,vNames[i],true));

        if(factory.vSubMenu[0].vSubMenu.size() > 0 || factory.vSubMenu.size() > 1)
        {
            if(factory.vSubMenu[0].vSubMenu.empty())
                factory.vSubMenu.erase(factory.vSubMenu.begin());
            vMenu.push_back(factory);
        }
        else
            vMenu.push_back(TreePopUpMenuItem(ePopUpSubMenuId,"Create",false));
    }


    void SceneEntityTree::InitView()
    {
        ISceneTree::InitView(false);
        pView->SortItems(true);
    }

    bool SceneEntityTree::CanDelete(Entity* pEntity)
    {
        if(pEntity->IsIncluded())return false;
        return true;
    }

    bool SceneEntityTree::Rename(void* pHandle,const char* sNewName /*= NULL*/)
    {
        bool bRet =  ISceneTree::Rename(pHandle,sNewName);
        pView->SortItems(true);
        return bRet;
    }

    void* SceneEntityTree::InsertEntity(void* pParentHandle,Entity* pEntity,int weight)
    {
        void* pHandle = ISceneTree::InsertEntity(pParentHandle,pEntity,weight,false);
        pView->SortItems(true);
        return pHandle;
    }

    Entity* SceneEntityTree::MoveItem(ISceneTree* pSourceList,void* pSource,void* pTarget)
    {
        if(pSourceList == this)
        {
            if(pTarget)
            {
                 if(pView->IsFolder(pTarget) )
                 {
                    ITree::MoveTreeItem(pSourceList,pSource,pTarget);
                    pView->SortItems(false);
                 }
            }
            else
            {
                if(pView->IsFolder(pSource) || pView->IsFolder(pView->GetParent(pSource)))
                {
                  ITree::MoveTreeItem(pSourceList,pSource,NULL);
                  pView->ForciblySort(pTarget);
                }
            }
        }
        return NULL;
    }

    void SceneEntityTree::GetPopUpMenu(void* pHandle,TTreePopUpMenu& vMenu)
    {
        if(pView)
        {
            bool bEditBlock = true;bool bDelete = true;
            std::vector<string> vEntityClassesTypes;
            vEntityClassesTypes.push_back("Scene");
            vEntityClassesTypes.push_back("AppNow");
            vEntityClassesTypes.push_back("Camera");
            vEntityClassesTypes.push_back("Environment");
            vEntityClassesTypes.push_back("Geometry");
            vEntityClassesTypes.push_back("Lighting");
            vEntityClassesTypes.push_back("Logic");
            vEntityClassesTypes.push_back("Online");
            vEntityClassesTypes.push_back("Other");
            vEntityClassesTypes.push_back("Sound");            
            AddFactoryPopUpMenu(vMenu,vEntityClassesTypes);

            Entity* pEntity = GetEntity(pHandle);
            if(pEntity)
            {
                vMenu.push_back(TreePopUpMenuItem(ePopUpSeparatorId));
                vMenu.push_back(TreePopUpMenuItem(ePopUpRollBackChanges,"Revert changes",pEntity->IsIncluded() && pEntity->IsHaveLocalChanges()));
                vMenu.push_back(TreePopUpMenuItem(ePopUpCreateFolderId,"Create folder",false));
                bDelete = !pEntity->IsIncluded();
            }
            else
            {
                if(!pHandle)
                    bDelete =bEditBlock = false;

                vMenu.push_back(TreePopUpMenuItem(ePopUpSeparatorId));
                vMenu.push_back(TreePopUpMenuItem(ePopUpRollBackChanges,"Revert changes",false));
                vMenu.push_back(TreePopUpMenuItem(ePopUpCreateFolderId,"Create folder",true));
            }

            vMenu.push_back(TreePopUpMenuItem(ePopUpSeparatorId));
            vMenu.push_back(TreePopUpMenuItem(ePopUpCopyId,"Copy",bEditBlock));
            vMenu.push_back(TreePopUpMenuItem(ePopUpDeleteId,"Delete",bDelete));
            vMenu.push_back(TreePopUpMenuItem(ePopUpSeparatorId));
            vMenu.push_back(TreePopUpMenuItem(ePopUpCutId,"Cut",bEditBlock));
            vMenu.push_back(TreePopUpMenuItem(ePopUpPasteId,"Paste",(pView->GetPopUpCutItem() != NULL && pView->GetPopUpCutTreeWidget() == pView)));
        }
    }

    void TreePopUpMenuItem::ClearEmptySubMenus()
    {
        for(TTreePopUpMenu::iterator it = vSubMenu.begin();it !=  vSubMenu.end();)
        {
            if((*it).nId ==  ePopUpSubMenuId && (*it).vSubMenu.size() == 0)
            {
                it = vSubMenu.erase(it);
            }
            else
                ++it;
        }
    }

}

#endif