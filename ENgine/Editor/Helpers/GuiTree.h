#pragma once
#if defined(PC) || defined(OSUNIX)
#include "SceneTree.h"

namespace ProjectInfo
{
    class Entity;

    class SceneGuiTree: public ISceneTree
    {
        TEntitiesList vTops;
    public:
        SceneGuiTree(const string& _sName,Scene* _pScene):ISceneTree(_sName,_pScene)
        {};
        virtual EType    Type() const {return ISceneTree::eGuiList;}
        virtual void*    InsertEntity(void* pParentHandle,Entity* pEntity,int weight);
        virtual Entity*  CopyEntity(void* pHandle,void* pParentHandle = NULL);
        virtual Entity*  ConvertToSymbol(void* pHandle,ISceneTree* pSymbolList);
        virtual void     UpdateEngineTopEntities();
        virtual Entity*  MoveItem(ISceneTree* pSourceList,void* pSource,void* pTarget);
        virtual bool     IsValidNode(Entity* pEntity) const {return true;}
        virtual void     InitView();
        virtual void     GetPopUpMenu(void* pHandle,TTreePopUpMenu& vMenu);
    protected:
        virtual bool  CanDelete(Entity* pEntity);
    };

}

#endif