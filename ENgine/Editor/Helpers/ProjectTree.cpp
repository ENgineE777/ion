#if defined(PC) || defined(OSUNIX)
#include "ProjectTree.h"
#include "Core/Files/IStreamLoader.h"
#include "EditorProject.h"

#define SCENE_IMAGE         1
#define START_SCENE_IMAGE   2
#define SCRIPT_IMAGE        3
#define INCLUDE_SCENE_IMAGE 4

namespace ProjectInfo
{
    void ProjectTree::Load(IStreamLoader* pLoader)
    {
        if(pLoader->EnterBlock("Nodes"))
        {
            while (pLoader->EnterBlock("Node"))
            {
                Node node(pLoader);
                vTree.push_back(node);
                pLoader->LeaveBlock();
            }
            pLoader->LeaveBlock();
        }
    }


    void ProjectTree::AddSceneToView(Scene* pScene,int nImageId,void* pParentHandle)
    {
         pScene->SetHandle(pView->AddSceneItem(pScene->ScreenName(),nImageId, pScene,pParentHandle, null, pScene->Path()));
         for(unsigned i = 0 ;i < pScene->vIncludes.size();++i)
         {
             pView->AddHelperItem(pScene->vIncludes[i].c_str(),INCLUDE_SCENE_IMAGE,&pScene->vIncludes[i][0],pScene->Handle(), null,NULL);
         }
    }

    void ProjectTree::InitView()
    {
        pView->DeleteItems();
        if(vTree.empty())
        {
            void * pSourceFolder = NULL;
            for (size_t i=0;i<pTarget->vScenes.size();i++)
                pSourceFolder = AddItem((i == pTarget->start_scene), pTarget->vScenes[i],pSourceFolder,true);
        }
        else
        {
            std::vector<void*>  vParents;
            void* last_item = 0;
            vParents.push_back(NULL);
            for (size_t i=0;i < vTree.size();++i)
            {
                if (vTree[i].eType == Node::eEntity)
                {
                    Scene* pScene = NULL;
                    for(unsigned j = 0;j < pTarget->vScenes.size();++j)
                    {
                        if(pTarget->vScenes[j]->ScreenName() == vTree[i].sName)
                        {
                            pScene = pTarget->vScenes[j];
                            break;
                        }
                    }
                    if(pScene)
                    {
                        if (pScene->Index() == pTarget->start_scene)
                        {
                            AddSceneToView(pScene,START_SCENE_IMAGE,vParents.back());
                        }
                        else
                            if (pScene->IsScript())
                            {
                                pScene->SetHandle(pView->AddSceneItem(pScene->ScreenName(),SCRIPT_IMAGE, pScene,vParents.back(), null, pScene->Path()));
                            }		
                            else
                            {
                                AddSceneToView(pScene,SCENE_IMAGE,vParents.back());
                            }
                    }
                }
                else
                    if (vTree[i].eType == Node::eFolder)
                        last_item = pView->CreateFolder(vTree[i].sName.c_str(),vParents.back(),null);
                    else
                        if (vTree[i].eType == Node::eStartBlock)vParents.push_back(last_item);
                        else
                            if (vTree[i].eType == Node::eEndBlock)vParents.pop_back();
            }	

        }
        pView->SortItems(true);
    }

    void* ProjectTree::AddItem(bool bStart,Scene* pScene,void * pSourceFolder,bool bCreateFolder /*= false*/)
    {
        if (bStart)
        {
            AddSceneToView(pScene,START_SCENE_IMAGE,null);
        }
        else
        {
            if (pScene->IsScript())
            {
                if(bCreateFolder)
                {
                    if(!pSourceFolder)
                        pSourceFolder = pView->CreateFolder("Scripts",NULL,NULL,"Source code");
                    pScene->SetHandle(pView->AddSceneItem( pScene->ScreenName(),SCRIPT_IMAGE,pScene, pSourceFolder, null, pScene->Path()));
                }
                else
                    pScene->SetHandle(pView->AddSceneItem( pScene->ScreenName(),SCRIPT_IMAGE,pScene, null, null, pScene->Path()));
            }		
            else
            {
                AddSceneToView(pScene,SCENE_IMAGE,null);
            }
        }
        return pSourceFolder;

    }

    void ProjectTree::AddItem(Scene* pScene,void* pParent,bool bStart)
    {
        if(pView)
        {
            if(!pView->IsFolder(pParent))
            {
                void* pGrandParent = pView->GetParent(pParent);
                if(pGrandParent)
                {
                    if(pView->IsFolder(pGrandParent))
                        pParent = pGrandParent;
                    else
                        pParent = NULL;
                }
                else pParent = NULL;
            }
            if (pScene->IsScript())
            {
                pScene->SetHandle(pView->AddSceneItem(pScene->ScreenName(),SCRIPT_IMAGE,pScene,pParent, null, pScene->Path()));
            }		
            else
            {
               AddSceneToView(pScene,bStart ? START_SCENE_IMAGE : SCENE_IMAGE,pParent);
            }
            pView->SortItems(true);
        }
    }

    bool ProjectTree::Rename(void* pHandle,const char* sNewName /*= NULL*/)
    {
        if(!sNewName)
            sNewName = pView->GetEditedText();
         Scene* pScene = static_cast<ProjectInfo::Scene*>(pView->GetHandleData(pHandle));		
         if (pScene)
         {
             pView->SetItemText(pHandle,sNewName);
             pView->SortItems(true);
         }
         else
             if(pView->CheckFolderName(pView->GetParent(pHandle),sNewName))
             {
                 pView->SetItemText(pHandle, sNewName);	
                 pView->SortItems(true);
             }

         return true;
    }

    void ProjectTree::SetStartImage(void* pHandle,bool bFlag)
    {
        if(pView)
           pView->SetItemImage(pHandle,bFlag ? START_SCENE_IMAGE : SCENE_IMAGE);
    }

    void ProjectTree::MoveItem(void* pSource,void* pTarget)
    {
        if(pSource && pView)
        {
            if(pTarget && !pView->IsFolder(pTarget))
            {
                void* pTargetParent = pView->GetParent(pTarget);
                if(pTargetParent)
                {
                    if(pView->IsFolder(pTargetParent))
                        pTarget = pTargetParent;
                    else 
                        return;
                }
                else 
                    return;
            }
            MoveTreeItem(this,pSource,pTarget);
            pView->SortItems(true);
        }
    }

    void ProjectTree::UpdateHandles(const ISceneTreeView::TreeViewItem& rItem)
    {
        Scene* pScene = pTarget->Find(rItem.sName.c_str());
        if(pScene)
            pScene->SetHandle(rItem.pHandle);
        for(unsigned i =0;i < rItem.vChilds.size();++i)
            UpdateHandles(rItem.vChilds[i]);
    }

    void ProjectTree::GetPopUpMenu(void* pHandle,TTreePopUpMenu& vMenu)
    {
        if(pView)
        {
            if(IsIncludedeSceneItem(pHandle))
            {
                 vMenu.push_back(TreePopUpMenuItem(ePopUpDeleteId,"Delete",true));
            }
            else
            {
                Scene* pScene = static_cast<ProjectInfo::Scene*>(pView->GetHandleData(pHandle));
                vMenu.push_back(TreePopUpMenuItem(ePopUpMakeSeneAsStartId,"Set as StartUp scene",(pScene && pTarget->start_scene != pScene->Index())));
                vMenu.push_back(TreePopUpMenuItem(ePopUpIncludeScene,"Include",pScene != NULL));
                vMenu.push_back(TreePopUpMenuItem(ePopUpSeparatorId));
                vMenu.push_back(TreePopUpMenuItem(ePopUpSaveSceneId,"Save scene",pScene != NULL));
                vMenu.push_back(TreePopUpMenuItem(ePopUpSaveSceneAsId,"Save scene as",pScene != NULL));
                vMenu.push_back(TreePopUpMenuItem(ePopUpSaveSceneAsAndDuplicateId,"Save scene as and duplicate",pScene != NULL));
                vMenu.push_back(TreePopUpMenuItem(ePopUpSeparatorId));
                vMenu.push_back(TreePopUpMenuItem(ePopUpCreateFolderId,"Create folder",true));
                vMenu.push_back(TreePopUpMenuItem(ePopUpDeleteId,"Delete",pScene != NULL));
                vMenu.push_back(TreePopUpMenuItem(ePopUpSeparatorId));
                vMenu.push_back(TreePopUpMenuItem(ePopUpAddNewSceneId,"Add new scene",true));
                vMenu.push_back(TreePopUpMenuItem(ePopUpAddExistingSceneId,"Add existing scene",true));
                vMenu.push_back(TreePopUpMenuItem(ePopUpSeparatorId));
                vMenu.push_back(TreePopUpMenuItem(ePopUpAddNewScriptId,"Add new external script",true));
                vMenu.push_back(TreePopUpMenuItem(ePopUpAddExistingSriptdId,"Add external existing script",true));
            }
        }

      }

    void ProjectTree::AddIncludeScene(Scene* pScene,const char* sName)
    {
        pScene->vIncludes.push_back(sName);
        if(pView)
            pView->AddSceneItem(sName,INCLUDE_SCENE_IMAGE,&(pScene->vIncludes.back()[0]),pScene->Handle(), null,NULL);
    }

    void ProjectTree::DeleteIncludeScene(Scene* pScene,void* pHandle,void* pNameStart)
    {
        if(pView)
            pView->DeleteItem(pHandle);

        for (size_t i=0; i<pScene->vIncludes.size(); i++)
        {
            if (&pScene->vIncludes[i][0] == pNameStart)
            {
                pScene->vIncludes.erase(pScene->vIncludes.begin() + i);
                break;
            }
        }
    }

    bool ProjectTree::IsIncludedeSceneItem(void* pHandle)
    {
        if(pView)
        {
            void* pParentHandle = pView->GetParent(pHandle);
            if(pParentHandle && !pView->IsFolder(pParentHandle))
                return true;
        }
        return false;
    }

}

#endif