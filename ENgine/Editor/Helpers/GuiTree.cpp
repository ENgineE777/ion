#if defined(PC) || defined(OSUNIX)
#include "SceneInfo.h"
#include "GuiTree.h"
#include "Core/Files/IStreamLoader.h"
#include "Entities/NGUI/NGUIService.h"
#include "Entities/NGUI/NGUIWidget.h"

namespace ProjectInfo
{
    void SceneGuiTree::InitView()
    {
        pView->DeleteItems();
        Entity::TTypeList vTypes;
        vTypes.push_back(Entity::eWidget);
        vTypes.push_back(Entity::eSymbolInstance);
        vTops.clear();
        pScene->GetTopList(vTypes,vTops);
        pView->AddItem("root",0, NULL, NULL, null);
        unsigned i = 0;
        for (TEntitiesList::iterator it = vTops.begin();it !=  vTops.end();++it,++i)
        {
            Entity* pEntity = (*it);
            pEntity->SetWeight(i);
            pEntity->AddToTreeView(this);
        }
    }

    bool SceneGuiTree::CanDelete(Entity* pEntity)
    {
        if(!pEntity->Parent())
        {
            vTops.remove(pEntity);
            unsigned i = 0;
            for (TEntitiesList::iterator it = vTops.begin();it !=  vTops.end();++it,++i)
                (*it)->SetWeight(i);
        }
        return true;
    }

    void* SceneGuiTree::InsertEntity(void* pParentHandle,Entity* pEntity,int weight)
    {

        if(!pParentHandle && pEntity->IsWidget() && pEntity->GetEngineEntity())
        {
            NGUIWidget* pWidget = reinterpret_cast<NGUIWidget*>(pEntity->GetEngineEntity());
            if(weight < 0)
                weight =  NGUIService::instance->TopWidgetsCount();
            pWidget->weight = weight;
            NGUIService::instance->AddTopWidget(pWidget);
            vTops.push_back(pEntity);
        }
        return ISceneTree::InsertEntity(pParentHandle,pEntity,weight,true);
    }

    Entity* SceneGuiTree::ConvertToSymbol(void* pHandle,ISceneTree* pSymbolList)
    {
        Entity* pEntity = static_cast<Entity*>(pView->GetHandleData(pHandle));
        if(pEntity)
            return  ISceneTree::ConvertToSymbol(pEntity,pSymbolList);
        return NULL;
    }

    void  SceneGuiTree::UpdateEngineTopEntities()
    {
        for(TEntitiesList::iterator it = vTops.begin();it !=  vTops.end();++it)
        {
            Entity* pEntity = *it;
            if(pEntity->GetEngineEntity())
                NGUIService::instance->AddTopWidget(static_cast<NGUIWidget*>(pEntity->GetEngineEntity()));
        }

    }

    Entity* SceneGuiTree::CopyEntity(void* pHandle,void* pParentHandle /*= NULL*/)
    {
        Entity* pEntity = ISceneTree::CopyEntity(pHandle,pParentHandle);
        if(pEntity && pEntity->GetEngineEntity() && !pEntity->Parent())
            NGUIService::instance->AddTopWidget(static_cast<NGUIWidget*>(pEntity->GetEngineEntity()));
        return pEntity;
    }


    Entity* SceneGuiTree::MoveItem(ISceneTree* pSourceList,void* pSource,void* pTarget)
    {
        if(pSource == pTarget || pSourceList->Type() == ISceneTree::eEntityList || String::IsEqual(pSourceList->View()->GetItemText(pSource),"childs"))return NULL;
        if(pSource && pSourceList)
        {
            Entity* pSourceEntity = pSourceList->GetEntity(pSource);
            void* pParentTargetHandle = pView->GetParent(pTarget);
            Entity* pParentTargetEntity = GetEntity(pParentTargetHandle);
            if(pSourceEntity->Type() == Entity::eSymbol)
            {
                Entity* pTargetEntity = GetEntity(pTarget);
                if(pTargetEntity && pTargetEntity->Type() == Entity::eSymbolInstance)
                {
                    Entity* pSymbol = pTargetEntity->Descriptor();
                    if(pSymbol == pSourceEntity)return NULL;
                }
                Entity* pInstanse = pScene->CreateEntity("NGUISymbolInstance",Matrix(),pSourceEntity->GetEngineEntity());
                if(pInstanse && pInstanse->GetEngineEntity())
                {
                    if(pParentTargetHandle)
                    {
                        Entity* pTargetEntity = GetEntity(pView->GetNextItem(pTarget));
                        if(pTargetEntity)
                            InsertEntity(pParentTargetHandle,pInstanse,pTargetEntity->Weight());
                        else
                            InsertEntity(pParentTargetHandle,pInstanse,pParentTargetEntity->ChildsCount());
                    }
                    else
                    {
                        if(pTarget)
                        {
                            Entity* pTargetEntity = GetEntity(pView->GetNextItem(pTarget));
                            InsertEntity(NULL,pInstanse,pTargetEntity->Weight());
                        }
                        else
                            InsertEntity(NULL,pInstanse, NGUIService::instance->TopWidgetsCount());
                        NGUIService::instance->AddTopWidget(static_cast<NGUIWidget*>(pInstanse->GetEngineEntity()));
                    }
                }
                return pInstanse;
            }
            else
            {
                Entity* pParentSourceEntity = pSourceList->GetEntity(pSourceList->View()->GetParent(pSource));
                if(!pParentTargetEntity)
                {
                    if(pParentSourceEntity)
                    {
                        if(pTarget)
                        {
                            Entity* pTargetEntity = GetEntity(pView->GetNextItem(pTarget));
                            pSourceEntity->ChangeParent(NULL,pTargetEntity->Weight());
                            ITree::MoveTreeItem(pSourceList,pSource,NULL,pTarget);
                        }
                        else
                        {
                            pSourceEntity->ChangeParent(NULL,NGUIService::instance->TopWidgetsCount());
                            ITree::MoveTreeItem(pSourceList,pSource,NULL,NULL);
                        }
                        NGUIService::instance->AddTopWidget(static_cast<NGUIWidget*>(pSourceEntity->GetEngineEntity()));
                    }
                    else
                    {
                        Entity* pTargetEntity = GetEntity(pTarget);
                        if (pTargetEntity && pTargetEntity->Weight() + 1 == pSourceEntity->Weight())return NULL;
                        NGUIService::instance->DelTopWidget(static_cast<NGUIWidget*>(pSourceEntity->GetEngineEntity()));
                        if (pTargetEntity)
                        {
                            NGUIService::instance->DelTopWidget(static_cast<NGUIWidget*>(pTargetEntity->GetEngineEntity()));
                            pSourceEntity->SetWeight(pTargetEntity->Weight() + 1);
                            ITree::MoveTreeItem(pSourceList,pSource,NULL,pTarget);
                            NGUIService::instance->AddTopWidget(static_cast<NGUIWidget*>(pTargetEntity->GetEngineEntity()));
                        }
                        else
                        {
                            if (pTarget)
                            {
                                pSourceEntity->SetWeight(-1);
                                ITree::MoveTreeItem(pSourceList,pSource,NULL,pTarget);
                            }
                            else
                            {
                                pSourceEntity->SetWeight(NGUIService::instance->TopWidgetsCount() + 1);
                                ITree::MoveTreeItem(pSourceList,pSource,NULL,NULL);
                            }
                        }
                        NGUIService::instance->AddTopWidget(static_cast<NGUIWidget*>(pSourceEntity->GetEngineEntity()));
                        vTops.sort(Entity::Comparator());
                        unsigned i = 0;
                        for (TEntitiesList::iterator it = vTops.begin();it !=  vTops.end();++it,++i)
                        {
                            Entity* pEntity = (*it);
                            pEntity->SetWeight(i);
                        }
                    }
                }
                else
                {
                    if(pSourceList == this && !pSourceEntity->Parent())
                        NGUIService::instance->DelTopWidget(static_cast<NGUIWidget*>(pSourceEntity->GetEngineEntity()));
                    MoveTreeItem(pSourceList,pSource,pTarget,pSourceEntity,pParentSourceEntity,pParentTargetEntity);
                    return pSourceEntity;
                }

            }
        }
        return NULL;
    }

    void SceneGuiTree::GetPopUpMenu(void* pHandle,TTreePopUpMenu& vMenu)
    {
        if(pView)
        {
            bool bEditBlock = true;bool bDelete = true;
            std::vector<string> vEntityClassesTypes;vEntityClassesTypes.push_back("NGUI");
            AddFactoryPopUpMenu(vMenu,vEntityClassesTypes);

            Entity* pEntity = GetEntity(pHandle);
            if(pEntity)
            {
                bool bHelper = String::IsEqual(pView->GetItemText(pHandle),"childs");
                vMenu.push_back(TreePopUpMenuItem(ePopUpSeparatorId));
                vMenu.push_back(TreePopUpMenuItem(ePopUpConvertToSymbolId,"Convert to Symbol",(pEntity->Type() == Entity::eWidget && !bHelper)));
                vMenu.push_back(TreePopUpMenuItem(ePopUpRollBackChanges,"Revert changes",pEntity->IsIncluded() && pEntity->IsHaveLocalChanges()));
                vMenu.push_back(TreePopUpMenuItem(ePopUpGetFullName,"Get full name",pEntity->GetEngineEntity() != NULL));
                if(bHelper)
                   bDelete = bEditBlock = false;
                else
                    bDelete = !pEntity->IsIncluded();
            }
            else
            {
                if(!pHandle)
                  bDelete = bEditBlock = false;

                vMenu.push_back(TreePopUpMenuItem(ePopUpSeparatorId));
                vMenu.push_back(TreePopUpMenuItem(ePopUpConvertToSymbolId,"Convert to Symbol",false));
                vMenu.push_back(TreePopUpMenuItem(ePopUpRollBackChanges,"Revert changes",false));
                vMenu.push_back(TreePopUpMenuItem(ePopUpGetFullName,"Get full name",false));

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