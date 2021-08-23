#pragma once
#if defined(PC) || defined(OSUNIX)
#include "Common/Utils.h"
#include <list>
#include "Common/Containers/HashMap.h"
#include "View/ISceneTreeView.h"

class SceneInstance;
class IStreamSaver;
class IStreamLoader;
class TreeViewWidget;
class SceneEntity;
#define IST_FILE_VERSION 1

namespace ProjectInfo
{
    class Entity;
    class Scene;
    typedef std::list<Entity*>   TEntitiesList;

    struct Node
    {
        enum ENodeType
        {
            eEntity = 0,
            eFolder = 1,
            eStartBlock = 2,
            eEndBlock = 3,
        };

        int         eType;
        string      sName;	
        Entity*     pEntity;

        Node():eType(eEntity),pEntity(NULL)
        {};
        Node(ENodeType _eType):eType(_eType),pEntity(NULL)
        {};
        Node(const string& _sName,ENodeType _eType):eType(_eType),pEntity(NULL),sName(_sName)
        {};
        Node(ProjectInfo::Entity*  _pEntity);
        Node(IStreamLoader* pLoader);

        void Save(IStreamSaver* pSaver) const;

    };	

    typedef vector<Node> TTree;

    enum ETreePopUPMenuId
    {
        ePopUpCopyId = 0,
        ePopUpCutId  = 1,
        ePopUpPasteId = 2,
        ePopUpDeleteId = 3,
        ePopUpCreateFolderId = 4,
        ePopUpConvertToSymbolId = 5,
        ePopUpMakeSeneAsStartId  = 6,
        ePopUpSaveSceneId         = 7,
        ePopUpSaveSceneAsId		= 8,
        ePopUpSaveSceneAsAndDuplicateId = 9,
        ePopUpAddNewSceneId			= 10,
        ePopUpAddExistingSceneId		= 11,
        ePopUpAddNewScriptId			= 12,
        ePopUpAddExistingSriptdId		= 13,
        ePopUpSeparatorId               = 14,
        ePopUpSubMenuId                 = 15,
        ePopUpRollBackChanges           = 16,    
        ePopUpGetFullName               = 17, 
        ePopUpIncludeScene              = 18, 

        ePopUpCreateId  = 24,
        ePopUpCreateInstanceId  = 100,
    };


    class TreePopUpMenuItem
    {
    public:
        int    nId;
        string sName;
        bool   bEnable;

        struct Comparator 
        {
            bool operator() (const TreePopUpMenuItem& i,const TreePopUpMenuItem& j)
            { 
                return operator() (&i,&j);
            }
            bool operator() (const TreePopUpMenuItem* i,const TreePopUpMenuItem* j)
            {
                if(i->nId == ePopUpSubMenuId && j->nId == ePopUpSubMenuId)
                    return String::CompareAlphapedOrder(i->sName.c_str(),j->sName.c_str());
                else
                {
                    if(i->nId == ePopUpSubMenuId && j->nId != ePopUpSubMenuId)return false;
                    if(i->nId != ePopUpSubMenuId && j->nId == ePopUpSubMenuId)return true;
                    return String::CompareAlphapedOrder(i->sName.c_str(),j->sName.c_str());
                }
            }
        };

        std::vector<TreePopUpMenuItem> vSubMenu;

        TreePopUpMenuItem():nId(-1),bEnable(false){}
        TreePopUpMenuItem(int id,const string& name,bool enable):nId(id),bEnable(enable),sName(name){}
        TreePopUpMenuItem(int id):nId(id),bEnable(true){}
        TreePopUpMenuItem(const TreePopUpMenuItem& copy){Copy(copy);}
        TreePopUpMenuItem& operator = (const TreePopUpMenuItem& copy)
        {
            Copy(copy);
            return *this;
        }
        const char* Find(int id)
        {
            if(id == nId)return sName.c_str();
            for(unsigned i = 0;i < vSubMenu.size();++i)
            {
                 const char* pRet = vSubMenu[i].Find(id);
                 if(pRet)
                     return pRet;
            }
            return NULL;
        }
        void ClearEmptySubMenus();

    private:
        inline void Copy(const TreePopUpMenuItem& copy)
        {
            nId = copy.nId;
            sName = copy.sName;
            bEnable = copy.bEnable;
            vSubMenu.assign(copy.vSubMenu.begin(),copy.vSubMenu.end());
        }
    };

    typedef std::vector<TreePopUpMenuItem> TTreePopUpMenu;


    class ITree
    {
    public:
        ITree(const string& _sName):pView(NULL),sName(_sName)
        {}

        void Save(IStreamSaver*  pSaver);


        inline void             ClearNodes(){vTree.clear();}
        inline void             Clear()
        {
            vTree.clear();
            if(pView)
                pView->DeleteItems();
        }
        inline bool             IsFolder(void* pHandle) const {return pView->IsFolder(pHandle);}
        inline ISceneTreeView*  View() const {return pView;}
        void                    SetView(ISceneTreeView* _pView);
        inline void             GrabTree(){GrabTree(vTree);}

        virtual void     Sort(void* pHandle = NULL){pView->SortItems(true);}
        virtual void     SelectItem(void* pHandle){pView->SelectItem(pHandle);}

        virtual void     GetPopUpMenu(void* pHandle,TTreePopUpMenu& vMenu) = 0;


    protected:
        TTree           vTree;
        ISceneTreeView* pView; 
        string          sName;

        void            GrabTree(vector<ProjectInfo::Node>& vTree);
        void            GrabTree(void* pHandle,vector<ProjectInfo::Node>& vTree);
        void*           MoveTreeItem(ITree* pSourceList,void* pSource,void* pTarget,void* pInsertAfter = NULL);
        virtual void    UpdateHandles(const ISceneTreeView::TreeViewItem& rItem) = 0;
        virtual void    InitView() = 0;   

    };


    class ISceneTree: public ITree
    {
    protected:
        Scene*          pScene; 
    public:
        enum EType{eEntityList,eGuiList,eSymbolsList};
        ISceneTree(const string& sName,Scene* _pScene):ITree(sName),pScene(_pScene)
        {};
        void Load(IStreamLoader* pLoader,Scene* pScene);
        void Merge(Entity* pEntity);
        
        inline bool             IsPresent(void* pHandle){return pView->GetHandleData(pHandle) != NULL;}
        inline bool             IsPresent(Node::ENodeType eType,const string& sName){return Find(eType,sName) != -1;}
        inline bool             IsMiss(Node::ENodeType eType,const string& sName){return Find(eType,sName) == -1;}
         


        inline Entity* GetSelectedEntity() const {return static_cast<Entity*>(pView->GetHandleData(pView->GetSelectedItem()));}
        Entity* GetEntity(void* pHandle);
        void    Add(Entity* pEntity);
        void    Copy(const ISceneTree& other);

        virtual EType    Type() const = 0;
        virtual Entity*  ConvertToSymbol(void* pHandle,ISceneTree* pSymbolList) = 0;
        virtual Entity*  MoveItem(ISceneTree* pSourceList,void* pSource,void* pTarget) = 0;
        virtual void*    InsertEntity(void* pParentHandle,Entity* pEntity,int weight) = 0;
        virtual bool     IsValidNode(Entity* pEntity) const = 0;

       
        virtual bool     IsDeletedSelected(void* pDeleted,void* pSelected);
        virtual void     Delete(void* pHandle);
        virtual Entity*  CopyEntity(void* pHandle,void* pParentHandle = NULL);
        virtual bool     Rename(const char* sOldName,const char* sNewName = NULL); 
        virtual bool     Rename(void* pHandle,const char* sNewName = NULL);   

    protected:
        virtual bool    CanDelete(Entity* pEntity) = 0;   
        virtual Entity* ConvertToSymbol(Entity* pEntity,ISceneTree* pSymbolList);   

        int             Find(Node::ENodeType eType,const string& sName);
        void*           InsertEntity(void* pParentHandle,Entity* pEntity,int weight,bool bAddChildsRoot);
        void            RecursiveCopy(void* pHandle,void* pParentHandle);
        void            InitView(bool bAddChilds);
        void            MoveTreeItem(ISceneTree* pSourceList,void* pSource,void* pTarget,Entity* pSourceEntity,Entity* pParentSourceEntity,Entity* pParentTargetEntity);
        bool            IsDeletedEntitySelected(Entity* pEntity,void* pSelected);
        virtual void    UpdateHandles(const ISceneTreeView::TreeViewItem& rItem);

        virtual void     AddFactoryPopUpMenu(TTreePopUpMenu& vMenu,std::vector<string>& vEntityClassesTypes,Entity* pEntity = NULL);

    };

    class SceneEntityTree: public ISceneTree
    {
    public:
        SceneEntityTree(const string& _sName,Scene* _pScene):ISceneTree(_sName,_pScene)
        {};
        virtual EType   Type() const {return ISceneTree::eEntityList;}
        virtual Entity* ConvertToSymbol(void* pHandle,ISceneTree* pSymbolList){return NULL;}
        virtual bool    Rename(void* pHandle,const char* sNewName = NULL);
        virtual void*   InsertEntity(void* pParentHandle,Entity* pEntity,int weight);
        virtual Entity* MoveItem(ISceneTree* pSourceList,void* pSource,void* pTarget);
        virtual bool    IsValidNode(Entity* pEntity) const {return true;}

        virtual void     GetPopUpMenu(void* pHandle,TTreePopUpMenu& vMenu);
        virtual void  InitView();
    protected:
        virtual bool  CanDelete(Entity* pEntity);   
    };
}

#endif