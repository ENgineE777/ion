#if defined(PC) || defined(OSUNIX)
#include "SceneInfo.h"
#include "SymbolsTree.h"
#include "Core/Files/IStreamLoader.h"
#include "Entities/NGUI/NGUIService.h"
#include "Entities/NGUI/NGUIWidget.h"
#include <algorithm>

namespace ProjectInfo
{
    void SceneSymbolsTree::InitView()
    {
        Entity::TTypeList vTypes;
        vSymbols.clear();
        vTypes.push_back(Entity::eSymbol);
        pScene->GetTopList(vTypes,vSymbols);
        ISceneTree::InitView(true);
        pView->ForciblySortRoot();
    }

    bool SceneSymbolsTree::CanDelete(Entity* pEntity)
    {
        if(pEntity->IsIncluded())return false;
        if(!pEntity->Parent())
        {
            vSymbols.remove(pEntity);
            if (NGUIService::instance->symbol_widget == pEntity->GetEngineEntity())
                NGUIService::instance->symbol_widget = NULL;
        }
        return true;
    }

    bool SceneSymbolsTree::Rename(void* pHandle,const char* sNewName /*= NULL*/)
    {
        bool bRet =  ISceneTree::Rename(pHandle,sNewName);
        void* pParent = pView->GetParent(pHandle);
        if(pParent)
        {
            if(!pView->GetHandleData(pParent) && !pView->GetParent(pParent))
                pView->ForciblySort(pParent);
        }
        else
            pView->ForciblySortRoot();
        return bRet;
    }


    void* SceneSymbolsTree::InsertEntity(void* pParentHandle,Entity* pEntity,int weight)
    {
        if(!pParentHandle)
            vSymbols.push_back(pEntity);
        NGUIWidget* pWidget =  static_cast<NGUIWidget*>(pEntity->GetEngineEntity());
        pWidget->horzAlign = 1;
        pWidget->vertAlign = 1;
        pWidget->UpdateChildsPos();
        return ISceneTree::InsertEntity(pParentHandle,pEntity,weight,true);
    }

    void SceneSymbolsTree::Sort(void* pHandle)
    {
        if(!pHandle)
            pView->ForciblySortRoot();
        else
        {
            if(pView->IsFolder(pHandle))
            {
                void* pIter = pView->GetParent(pHandle);
                if(pIter)
                {
                    while(pView->IsFolder(pIter))
                        pIter = pView->GetParent(pIter);
                    if(!pIter)
                        pView->ForciblySort(pHandle);
                }
                else pView->ForciblySortRoot();
            }
        }
    }

    void  SceneSymbolsTree::UpdateEngineTopEntities()
    {
        for(TEntitiesList::iterator it = vSymbols.begin();it !=  vSymbols.end();++it)
        {
            Entity* pEntity = *it;
            if(pEntity->GetEngineEntity())
                NGUIService::instance->AddSymbolWidget(static_cast<NGUIWidget*>(pEntity->GetEngineEntity()));
        }

    }

    Entity* SceneSymbolsTree::ConvertToSymbol(void* pHandle,ISceneTree* pSymbolList)
    {
        Entity* pEntity = static_cast<Entity*>(pView->GetHandleData(pHandle));
        if(pEntity && pEntity->IsWidget())
        {
            return  ISceneTree::ConvertToSymbol(pEntity,pSymbolList);
        }
        return NULL;
    }

    Entity* SceneSymbolsTree::MoveItem(ISceneTree* pSourceList,void* pSource,void* pTarget)
    {
        if(pSource == pTarget || pSourceList->Type() == ISceneTree::eEntityList || String::IsEqual(pSourceList->View()->GetItemText(pSource),"childs"))return NULL;
        if(pSource && pSourceList)
        {
            if(pView->IsFolder(pTarget) )
            {
                if(pSourceList == this)
                {
                    Entity* pSourceEntity = pSourceList->GetEntity(pSource);
                    if(pSourceEntity && pSourceEntity->Parent())return NULL;
                    ITree::MoveTreeItem(pSourceList,pSource,pTarget);
                    pView->ForciblySort(pTarget);
                    return pSourceEntity;
                }
            }
            else
            {
                if(pSourceList->IsFolder(pSource))return NULL;
                Entity* pTargetEntity = GetEntity(pTarget);
                Entity* pSourceEntity = pSourceList->GetEntity(pSource);
                if(pSourceEntity->Type() == Entity::eSymbol)
                {
                    if(!pTargetEntity)return NULL; 
                    if(pTargetEntity->Type() == Entity::eSymbolInstance)
                    {
                        Entity* pSymbol = pTargetEntity->Descriptor();
                        if(pSymbol == pSourceEntity)return NULL;
                    }
                    if(String::IsEqual(pView->GetItemText(pTarget),"childs") || pTargetEntity->IsWidget())
                    {
                        void* pParentTargetHandle = pView->GetParent(pTarget);
                        Entity* pParentTargetEntity = GetEntity(pParentTargetHandle);
                        if(!pParentTargetEntity)return NULL;
                        Entity* pInstanse = pScene->CreateEntity("NGUISymbolInstance",Matrix(),pSourceEntity->GetEngineEntity());
                        if(pInstanse && pInstanse->GetEngineEntity())
                        {
                            pTargetEntity = GetEntity(pView->GetNextItem(pTarget));
                            if(pTargetEntity)
                            {
                                InsertEntity(pParentTargetHandle,pInstanse,pTargetEntity->Weight());
                            }
                            else
                                InsertEntity(pParentTargetHandle,pInstanse,pParentTargetEntity->ChildsCount());
                        }
                        return pInstanse;
                    }
                    else
                        if(pTargetEntity->Type() == Entity::eSymbol)
                        {
                            if(pTarget)
                            {
                                void* pTargetParentHandler =  pSourceList->View()->GetParent(pTarget);
                                if(pTargetParentHandler && pView->IsFolder(pTargetParentHandler))
                                {
                                    ITree::MoveTreeItem(pSourceList,pSource,pTargetParentHandler);
                                    pView->ForciblySort(pTargetParentHandler);
                                    return pSourceEntity;
                                }
                            }
                            else
                                if(pSourceList->View()->GetParent(pSource))
                                {
                                    ITree::MoveTreeItem(pSourceList,pSource,NULL);
                                    pView->ForciblySortRoot();
                                    return pSourceEntity;
                                }
                        }
                }
                else
                {
                    Entity* pParentTargetEntity = GetEntity(pView->GetParent(pTarget));
                    Entity* pParentSourceEntity = pSourceList->GetEntity(pSourceList->View()->GetParent(pSource));
                    if(pTargetEntity && pSourceEntity->Type() == Entity::eSymbolInstance)
                    {
                        Entity* pSymbol = pTargetEntity->GetSymbol();
                        if(pSymbol == pSourceEntity->Descriptor())return NULL;
                    }
                    if(!pParentTargetEntity)
                    {
                        if(!pSourceEntity->IsIncluded() && pSourceEntity->IsWidget())
                        {
                            if(pSourceList == this)return NULL;
                            else
                                if(pSourceList->Type() == ISceneTree::eGuiList)
                                {
                                    if(pTargetEntity)
                                    {
                                        NGUIService::instance->DelTopWidget(static_cast<NGUIWidget*>(pSourceEntity->GetEngineEntity()));
                                        pSourceEntity->ChangeParent(pTargetEntity,0);
                                        ITree::MoveTreeItem(pSourceList,pSource,pTarget,pView->GetFirstItem(pTarget));
                                        return pTargetEntity;
                                    }
                                    else
                                        return pSourceList->ConvertToSymbol(pSource,this);
                                }
                        }
                        return NULL;
                    }
                    MoveTreeItem(pSourceList,pSource,pTarget,pSourceEntity,pParentSourceEntity,pParentTargetEntity);
                    return pSourceEntity;
                }
            }

        }
        return NULL;
    }

    bool SceneSymbolsTree::IsValidNode(Entity* pEntity) const
    {
        if(!pEntity->Parent() && pEntity->Type() != Entity::eSymbol)
            return false;
        return true;
    }

    Entity* SceneSymbolsTree::CopyEntity(void* pHandle,void* pParentHandle /*= NULL*/)
    {
        Entity* pEntity = ISceneTree::CopyEntity(pHandle,pParentHandle);
        if(pEntity && pEntity->GetEngineEntity())
        {
            if(!pEntity->Parent())
                vSymbols.push_back(pEntity);
        }
        return pEntity;
    }
	
	class Iter
    {
		public:
            TreePopUpMenuItem* pItem;
            int                nCount; 
			Iter():pItem(NULL),nCount(0){}
            Iter(TreePopUpMenuItem* item):pItem(item),nCount(0){}
     };
	 
    void SceneSymbolsTree::ConvertSymbolsTreeInMenu(TreePopUpMenuItem& rItem,Entity* pSymbol)
    {
        GrabTree(vTree);
        std::vector<Iter>  vParents;
        vParents.push_back(Iter(&rItem));
        int  nId = ePopUpCreateInstanceId;
        for (size_t i=0;i < vTree.size();++i)
        {
            if (vTree[i].eType == ProjectInfo::Node::eEntity)
            {
                Entity* pEntity = pScene->Find(vTree[i].sName);
                if(pEntity && pEntity->Type() == Entity::eSymbol)
                {
                    if(pSymbol && pSymbol->Name() == vTree[i].sName)continue;
                    vParents.back().pItem->vSubMenu.push_back(TreePopUpMenuItem(nId,vTree[i].sName,true));
                    ++nId;
                }
            }
            else
            {
                if (vTree[i].eType == ProjectInfo::Node::eFolder)
                {
                    TreePopUpMenuItem* pItem =  vParents.back().pItem;
                    pItem->vSubMenu.push_back(TreePopUpMenuItem(ePopUpSubMenuId,vTree[i].sName,true));
                    vParents.push_back(Iter(&pItem->vSubMenu.back()));
                }
                else
                {
                    if (vTree[i].eType == ProjectInfo::Node::eStartBlock)++(vParents.back().nCount);
                    else
                    {
                        if (vTree[i].eType == ProjectInfo::Node::eEndBlock)
                        {
                            if(vParents.size() > 1)
                            {
                                --(vParents.back().nCount);
                                if(vParents.back().nCount <= 0)
                                {
                                    TreePopUpMenuItem* pItem =  vParents.back().pItem;
                                    pItem->ClearEmptySubMenus();
                                    std::sort(pItem->vSubMenu.begin(),pItem->vSubMenu.end(),TreePopUpMenuItem::Comparator());
                                    vParents.pop_back();
                                }
                            }
                        }
                    }
                }
            }
        }
        rItem.ClearEmptySubMenus();
        std::sort(rItem.vSubMenu.begin(),rItem.vSubMenu.end(),TreePopUpMenuItem::Comparator());
    }

    void SceneSymbolsTree::GetPopUpMenu(void* pHandle,TTreePopUpMenu& vMenu)
    {
        if(pView)
        {
            bool bEditBlock = true;bool bDelete = true;
            Entity* pEntity = GetEntity(pHandle);
            if(pEntity)
            {
                bool bHelper = String::IsEqual(pView->GetItemText(pHandle),"childs");
                if(bHelper  || pEntity->Parent())
                {
                    std::vector<string> vEntityClassesTypes;vEntityClassesTypes.push_back("NGUI");
                    AddFactoryPopUpMenu(vMenu,vEntityClassesTypes,pEntity->GetSymbol());
                }
                else
                {
                    vMenu.push_back(TreePopUpMenuItem(ePopUpSubMenuId,"Create",true));
                    vMenu.back().vSubMenu.push_back(TreePopUpMenuItem(ePopUpCreateId,"NGUISymbol",true));
                }
                vMenu.push_back(TreePopUpMenuItem(ePopUpSeparatorId));
                vMenu.push_back(TreePopUpMenuItem(ePopUpConvertToSymbolId,"Convert to Symbol",(pEntity->Type() == Entity::eWidget && !bHelper)));
                vMenu.push_back(TreePopUpMenuItem(ePopUpRollBackChanges,"Revert changes",pEntity->IsIncluded() && pEntity->IsHaveLocalChanges()));
                vMenu.push_back(TreePopUpMenuItem(ePopUpGetFullName,"Get full name",pEntity->GetEngineEntity() != NULL));
                vMenu.push_back(TreePopUpMenuItem(ePopUpSeparatorId));
                vMenu.push_back(TreePopUpMenuItem(ePopUpCreateFolderId,"Create folder",false));
                if(bHelper)
                    bDelete = bEditBlock = false;
                else
                    bDelete = !pEntity->IsIncluded();
            }
            else
            {
                if(!pHandle)
                    bDelete = bEditBlock = false;

                vMenu.push_back(TreePopUpMenuItem(ePopUpSubMenuId,"Create",true));
                vMenu.back().vSubMenu.push_back(TreePopUpMenuItem(ePopUpCreateId,"NGUISymbol",true));
                vMenu.push_back(TreePopUpMenuItem(ePopUpSeparatorId));
                vMenu.push_back(TreePopUpMenuItem(ePopUpConvertToSymbolId,"Convert to Symbol",false));
                vMenu.push_back(TreePopUpMenuItem(ePopUpRollBackChanges,"Revert changes",false));
                vMenu.push_back(TreePopUpMenuItem(ePopUpGetFullName,"Get full name",false));
                vMenu.push_back(TreePopUpMenuItem(ePopUpSeparatorId));
                vMenu.push_back(TreePopUpMenuItem(ePopUpCreateFolderId,"Create folder",true));
            }

            vMenu.push_back(TreePopUpMenuItem(ePopUpSeparatorId));
            vMenu.push_back(TreePopUpMenuItem(ePopUpCopyId,"Copy",bEditBlock));
            vMenu.push_back(TreePopUpMenuItem(ePopUpDeleteId,"Delete",bDelete));
            vMenu.push_back(TreePopUpMenuItem(ePopUpSeparatorId));
            vMenu.push_back(TreePopUpMenuItem(ePopUpCutId,"Cut",bEditBlock));
            vMenu.push_back(TreePopUpMenuItem(ePopUpPasteId,"Paste",(pView->GetPopUpCutItem() != NULL && pView->GetPopUpCutTreeWidget() != pScene->SceneTree()->View())));
        }
    }

}

#endif