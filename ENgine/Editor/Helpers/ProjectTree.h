#pragma once
#if defined(PC) || defined(OSUNIX)
#include "SceneTree.h"

namespace ProjectInfo
{
    struct Target;


    class ProjectTree:public ITree
    {
        Target*  pTarget;

    public:
        ProjectTree(Target* _pTarget):ITree("Nodes"),pTarget(_pTarget)
        {};

        virtual void Load(IStreamLoader* pLoader);
        virtual bool Rename(void* pHandle,const char* sNewName = NULL);

        void AddItem(Scene* pScene,void* pParent,bool bStart = false);
        void SetStartImage(void* pHandle,bool bFlag);
        void MoveItem(void* pSource,void* pTarget);

        virtual void    InitView();   
        virtual void    GetPopUpMenu(void* pHandle,TTreePopUpMenu& vMenu);

        virtual void    AddIncludeScene(Scene* pScene,const char* sName);
        virtual void    DeleteIncludeScene(Scene* pScene,void* pHandle,void* pNameStart);
        virtual bool    IsIncludedeSceneItem(void* pHandle);

    protected:
        virtual void    UpdateHandles(const ISceneTreeView::TreeViewItem& rItem);
        

        void* AddItem(bool bStart,Scene* pScene,void * pSourceFolder,bool bCreateFolder = false);
    private:
        void  AddSceneToView(Scene* pScene,int nImageId,void* pParentHandle);

    };
}

#endif