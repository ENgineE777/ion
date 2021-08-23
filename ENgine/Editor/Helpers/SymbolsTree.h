#pragma once
#if defined(PC) || defined(OSUNIX)
#include "SceneTree.h"

namespace ProjectInfo
{
    class Entity;

    class SceneSymbolsTree: public ISceneTree
    {
        TEntitiesList vSymbols;
    public:
        SceneSymbolsTree(const string& _sName,Scene* _pScene):ISceneTree(_sName,_pScene)
        {};
        virtual EType   Type() const {return ISceneTree::eSymbolsList;}
        virtual Entity* ConvertToSymbol(void* pHandle,ISceneTree* pSymbolList);
        virtual void    Sort(void* pHandle = NULL);
        virtual bool    Rename(void* pHandle,const char* sNewName = NULL);
        virtual void*   InsertEntity(void* pParentHandle,Entity* pEntity,int weight);
        virtual void    UpdateEngineTopEntities();
        virtual Entity* MoveItem(ISceneTree* pSourceList,void* pSource,void* pTarget);
        virtual bool    IsValidNode(Entity* pEntity) const;
        Entity*         CopyEntity(void* pHandle,void* pParentHandle /*= NULL*/);

        virtual void     GetPopUpMenu(void* pHandle,TTreePopUpMenu& vMenu);

        virtual void  InitView();

        void ConvertSymbolsTreeInMenu(TreePopUpMenuItem& rItem,Entity* pSymbol);
    protected:
        virtual bool  CanDelete(Entity* pEntity);

    };

}

#endif