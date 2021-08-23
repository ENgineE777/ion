#pragma once
#if defined(PC) || defined(OSUNIX)
#include "Common/Utils.h"
#include "Common/Containers/HashMap.h"
#include "Core/Files/IStreamLoader.h"
#include <list>
#include "SymbolsTree.h"
#include "GuiTree.h"

class SceneInstance;
class IStreamSaver;
class IStreamLoader;
class TreeViewWidget;
class SceneEntity;
class NGUIWidget;
namespace ProjectInfo
{
    class Scene;
    class Entity
    {

    public:
        struct Comparator 
        {
            bool operator() (const Entity& i,const Entity& j)
            { 
                return (i.Weight() < j.Weight());
            }
            bool operator() (const Entity* i,const Entity* j)
            {
                return (i->Weight() < j->Weight());
            }
        };

        enum EType{eSymbol,eSymbolInstance,eWidget,eEntity};
        typedef std::vector<EType> TTypeList;
        typedef std::list<Entity*>::iterator Iterator; 
    private:
        typedef HashTable<string,Entity*> TEntities;
        typedef std::list<Entity*>        TChilds;

        bool                    bIncluded;
        string                  sName;
        string                  sClassName;
        string                  sSymbolName;
        string                  sSourceScenePath;
        string                  sParentName;
        int                     nWeight;
        EType                   eType;
        bool                    bUseInstance;
        Entity*                 pParent;
        Entity*                 pDescriptor;
        SceneEntity*            pEntity;
        Scene*                  pScene;  
        ISceneTree*             pTree;
        void*                   pHandle;                        
        std::vector<string>     vChildsNames;
        TChilds                 vChilds;
        TChilds                 vInstances;

        inline void LoadString(IStreamLoader* pLoader,const char* sName,string& sOut)
        {
            static char vBuffer[256];
            memset(vBuffer,0,256);
            pLoader->Read(sName,vBuffer,256);
            sOut = vBuffer;
        }

    public:
		Entity() :pParent(NULL), pDescriptor(NULL), pEntity(NULL), eType(eEntity), pScene(NULL), pTree(NULL), pHandle(NULL), bUseInstance(false), bIncluded(false), nWeight(0)
        {}
        Entity(Scene* _pScene,bool _bIncluded):pDescriptor(NULL),pParent(NULL),pEntity(NULL),pTree(NULL),pHandle(NULL),
			eType(eEntity), bUseInstance(false), pScene(_pScene), bIncluded(_bIncluded), nWeight(0)
        {}
        Entity(Scene* _pScene,SceneEntity* _pEntity);
        Entity(const Entity& copy){Copy(copy);}
        Entity& operator = (const Entity& copy)
        {
            Copy(copy); 
            return *this;
        }
    public:
        void                 Load(IStreamLoader* oader);
        Entity*              GetSymbol();
        inline bool          IsSymbol()
        {
            if(GetSymbol())
                return true;  
            return false;
        }
        inline const string& Name() const {return sName;}
        inline void          SetName(const string& _sName){sName = _sName;}
        inline const string& ParentName() const {return sParentName;}
        inline const string& ClassName() const {return sClassName;}
        inline const string& SymbolName() const {return sSymbolName;}
        inline int           Weight() const { return nWeight;}
        void                 SetWeight(int w);
        inline bool          IsUseInstance() const {return bUseInstance;}
        inline bool          IsIncluded() const {return bIncluded;}
        inline EType         Type() const {return eType;}
        inline Entity*       Parent() const {return pParent;}
        inline SceneEntity*  GetEngineEntity() const { return pEntity;}
        inline void          SetEngineEntity(SceneEntity* _pEntity){ pEntity = _pEntity;}
        inline void          SetParent(Entity*  _pParent){pParent = _pParent;}
        void                 SetDescriptorName(const string& sName );
        inline void          SetDescriptor(Entity*  _pSymbol)
        {
            pDescriptor = _pSymbol;
            SetDescriptorName(pDescriptor->Name());
        }
        inline Entity*       Descriptor() const{return pDescriptor;}
        inline void          SetHandle(void* _pHandle){pHandle = _pHandle;}
        inline void*         Handle() const {return pHandle;}
        inline void          SetTree(ISceneTree* _pTree){pTree = _pTree;}
        inline ISceneTree*   Tree() const { return pTree;}
        inline bool          IsWidget() const {return (eType == Entity::eWidget || eType == Entity::eSymbolInstance);} 


        bool                 IsHaveLocalChanges() const;
        bool                 IsContains(Entity* pEntity);
        void                 Select();                       
        Entity*              Copy() const;
        void                 GetMembers(std::vector<string>& vMembers);
        void                 ChangeParent(Entity*  _pParent,int weight);
        bool                 Rename(const char* sNewName); 
        void                 FindChilds(TEntities& vList);

        Entity*              GetChild(int weight);
        Entity*              GetChild(const string& sName);
        Entity*              GetChildBefore(const string& sName);
        void                 DeleteChild(Entity* pChild,bool bModifyEngineEntity = true);
        void                 OnChildRename(Entity* pChild);
        inline void          AddChild(const string& sName){vChildsNames.push_back(sName);}
        inline void          AddChild(const string& sName,int weight)
        {
            vChildsNames.insert(vChildsNames.begin() + weight,sName);
        }
        void                 AddChild(Entity* pChild,int weight);
        inline unsigned      ChildsCount() const {return vChilds.size();}
        inline void          AddChild(Entity* pChild)
        {
            pChild->SetWeight(vChilds.size());
            vChilds.push_back(pChild);
            AddChild(pChild->Name());
        }
        void                 MoveChild(Entity* pEntity,int nNewWeight);
        void                 DeleteInstances();               
        void                 ReInitInstances();   
        inline void          AddInstance(Entity*  pInstance){vInstances.push_back(pInstance);}
        inline void          RemoveInstance(Entity*  pInstance){vInstances.remove(pInstance);}
        void                 Delete(bool bModifyEngineEntity = true);   
        inline bool          HaveChilds() const {return !vChilds.empty();}

        void                 AddToTreeList(TTree& rTree);
        void*                AddToTreeView(ISceneTree* pTree,void* pParent = NULL,void* pLast = NULL,bool bAddChildsRoot = true);

        inline  Iterator    ChildsStart(){return vChilds.begin();}                        
        inline  Iterator    ChildsEnd(){return vChilds.end();}          

        inline  Iterator    InstancesStart(){return vInstances.begin();}                        
        inline  Iterator    InstancesEnd(){return vInstances.end();} 

		bool   RollBackLocalChanges();

        inline void Copy(Entity* pOther)
        {
            if(pOther)
            {
                bIncluded = pOther->bIncluded;
                nWeight = pOther->nWeight;
                pScene =  pOther->pScene;   
                vChildsNames.assign(pOther->vChildsNames.begin(),pOther->vChildsNames.end());
                vChilds.assign(pOther->vChilds.begin(),pOther->vChilds.end());
                vInstances.assign(pOther->vInstances.begin(),pOther->vInstances.end());
                for(TChilds::iterator it = vChilds.begin();it != vChilds.end();++it)
                {
                    (*it)->SetParent(this);
                }
                if(Type() == eSymbol)
                {
                    for(TChilds::iterator it = vInstances.begin();it != vInstances.end();++it)
                    {
                        (*it)->SetDescriptor(this);
                    }
                }
                pOther->vChilds.clear();
                pOther->vChildsNames.clear();
                pOther->vInstances.clear();
            }
        }

        inline void SetSourceScenePath(const char* sPath){sSourceScenePath = sPath;}
		const char* GetScenePath();


    private:
        inline void Copy(const Entity& copy)
        {
            bIncluded = copy.bIncluded;
            sName = copy.sName;
            sClassName = copy.sClassName;
            sSymbolName = copy.sSymbolName;
            nWeight = copy.nWeight;
            eType = copy.eType;
            bUseInstance = copy.bUseInstance;
            pParent = copy.pParent;
            pDescriptor = copy.pDescriptor;
            pEntity = copy.pEntity;
            pScene = copy.pScene;   
            pTree = copy.pTree;
            pHandle = copy.pHandle;
            sSourceScenePath = copy.sSourceScenePath;
            vChildsNames.assign(copy.vChildsNames.begin(),copy.vChildsNames.end());
            vChilds.assign(copy.vChilds.begin(),copy.vChilds.end());
            vInstances.assign(copy.vInstances.begin(),copy.vInstances.end());
        }
        void    AddToWidget(Entity* pParent,NGUIWidget* pChildWidget,int nWeigth);
    };

    typedef HashTable<string,Entity*> TEntities;
    typedef std::vector<Entity*>      TEntitiesVector;
    typedef std::list<Entity*>        TEntitiesList;
    
    class Scene
    {
    private:
        string                 sName; 
        string                 sScreenName; 
        string                 sFilePath; 
        TEntities              vEntities;
        SceneInstance*         pScene;
        SceneSymbolsTree       rSymbolsTree;
        SceneEntityTree        rSceneTree;
        SceneGuiTree           rGuiTree;

        SceneInstance*         pExportedScene;
        SceneEntity*           pSelected;
        void*                  pHandle;
        bool		           bScript;	
        bool                   bOpened;
        int                    nIndex;

    public:
        std::vector<string>    vIncludes;

        Scene():pScene(NULL),pSelected(NULL),pExportedScene(NULL),rSymbolsTree("SymbolNodes",this),rSceneTree("Nodes",this),rGuiTree("Gui",this),pHandle(NULL),bScript(false),bOpened(false),nIndex(-1){}

        Scene(const string& name):pScene(NULL),pSelected(NULL),pExportedScene(NULL),rSymbolsTree("SymbolNodes",this),rSceneTree("Nodes",this),rGuiTree("Gui",this),pHandle(NULL),bScript(false),bOpened(false),nIndex(-1)
        {
            SetName(name.c_str());
        }

        Scene(int index):pScene(NULL),pSelected(NULL),pExportedScene(NULL),rSymbolsTree("SymbolNodes",this),rSceneTree("Nodes",this),rGuiTree("Gui",this),pHandle(NULL),bScript(false),bOpened(false),nIndex(index)
        {}

        Scene(const string& name,int index,bool script):pScene(NULL),pSelected(NULL),pExportedScene(NULL),rSymbolsTree("SymbolNodes",this),rSceneTree("Nodes",this),rGuiTree("Gui",this),pHandle(NULL),bScript(script),bOpened(false),nIndex(index)
        {
            SetName(name.c_str());
        }

        Scene(const Scene& copy):rSymbolsTree("SymbolNodes",this),rSceneTree("Nodes",this),rGuiTree("Gui",this){Copy(copy);}
        Scene& operator = (const Scene& copy)
        {
            Copy(copy);
            return *this;
        }
    public:
        ~Scene();

        inline SceneInstance* ExportedScene() const {return pExportedScene;}
        inline SceneInstance* Instance() const {return pScene;}
        inline SceneEntity*   SelectedEntity() const {return pSelected;}
        inline void           SetSelectedEntity(SceneEntity* pEntity)  {pSelected = pEntity;}
        inline void SetExportedScene(SceneInstance* pScene) {pExportedScene = pScene;}
        inline int Index() const {return nIndex;}
        inline void SetIndex(int index){nIndex = index;}
        inline const char* Path() const {return sFilePath.c_str();}
        inline const char* Name() const {return sName.c_str();}
        inline const char* ScreenName() const {return sScreenName.c_str();}
        inline bool IsScript() const {return bScript;}
        inline bool IsOpen() const {return bOpened;}
        inline void Open() {bOpened = true;}
        inline void Close() {bOpened = false;}
        inline void* Handle() const {return pHandle;}
        inline void  SetHandle(void* handle)  {pHandle = handle;}
        inline void SetPath(const char* sPath){sFilePath = sPath;}
        inline void SetScreenName(const char* sName){sScreenName = sName;}
        inline void SetName(const char* _sName)
        {
            sName = _sName;
            sScreenName = sName;
            sScreenName.erase(sScreenName.size() - 3,3);
        }
        inline void LowerCasePath()
        {
            String::LowerCase(&sFilePath[0]);
        }
        inline void FindChilds(Entity* pEntity)
        {
            pEntity->FindChilds(vEntities);
        }
        void    Load(IStreamLoader* pLoader);
        void    Save(IStreamSaver* saver);
        void    ReLoad(bool bClear = false);
        void    ReLoadInstance(bool bClear = false);
        void    Load(const char* sPath,IStreamLoader::FileType data_type);
        void    GetMembersInScene(const std::vector<string>& vPath,std::vector<string>& vMembers);
        void    GetSignatureInScene(const std::vector<string>& vPath,std::vector<string>& vSignatures);
        void    LoadSceneTrees();
        void    Save();
        void    GetTopList(Entity::TTypeList& eTypes,TEntitiesList& vList);

        inline void     SetInstance( SceneInstance*   _pScene){ pScene = _pScene;} 
        inline SceneInstance* GetEngineScene(){return pScene;} 
        inline Entity*  Find(const string& sName)
        {
            Entity** ppEntity = vEntities.Find(sName);
            if(ppEntity)
                return *ppEntity;
            else
                return NULL;
        }
        inline void     SetSceneTreeView(ISceneTreeView* pView){rSceneTree.SetView(pView);}
        inline void     SetSymbolsTreeView(ISceneTreeView* pView){rSymbolsTree.SetView(pView);}
        inline void     SetGuiTreeView(ISceneTreeView* pView){rGuiTree.SetView(pView);}
        inline void     Unselect()
        {
            rSymbolsTree.SetView(NULL);
            rSceneTree.SetView(NULL);
            rGuiTree.SetView(NULL);
        }
        inline void     UpdateEngineTopEntities()
        {
            rGuiTree.UpdateEngineTopEntities();
            rSymbolsTree.UpdateEngineTopEntities();
        }

        inline ISceneTree* SymbolsTree() {return &rSymbolsTree;}
        inline ISceneTree* SceneTree() {return &rSceneTree;}
        inline ISceneTree* GuiTree() {return &rGuiTree;}
        ISceneTree*        GetEntityTree(SceneEntity* pEngineEntity);
        void               OnEntityVisibleChange(SceneEntity* pEngineEntity);
        void               OnEntityEctiveChange(SceneEntity* pEngineEntity);

        ISceneTree*        FindTree(void* pHandle);

        Entity*  CreateEntity(const char* sName,const Matrix& rTansform,SceneEntity* pDesc = NULL,const char* sEntityName = NULL);
        Entity*  CopyEntity(const Entity* pSource);
        Entity*  ConvertToSymbol(Entity* pDesc);
        Entity*  CreateEntity(SceneEntity* pEntity);

        void  Remove(Entity* pEntity);
        void  RenameEntity(const string& sOldName,const string& sNewName);

        void  OnPropertiesChanged(SceneEntity* pEngineEntity);

        const char* GenerateName(const char* sBase);

        void GetEntityClassesNames(vector<const char*>& vNames,const std::vector<string>& vEntityClassesTypes);

    private:
        Entity* GetLastEntity(Entity* pParent,const std::vector<string>& vPath,unsigned offset,unsigned endOffset = 0);
        void    BuildSceneTree();
        void    LoadEntities(const char* sPath,IStreamLoader::FileType data_type,bool bIncluded = false);
        bool    LoadSceneTreeCash();
		void    BuildTreeFromScene();

        inline void Copy(const Scene& copy)
        {
            sFilePath = copy.sFilePath;
            sName = copy.sName;
            vIncludes.clear();
            vIncludes.assign(copy.vIncludes.begin(),copy.vIncludes.end());
            vEntities = copy.vEntities;
            pScene = copy.pScene;
            bScript = copy.bScript;
            bOpened = copy.bOpened;
            pHandle = copy.pHandle;
            rSymbolsTree.Copy(copy.rSymbolsTree);
            rSceneTree.Copy(copy.rSceneTree);
            rGuiTree.Copy(copy.rGuiTree);
        }


    };

    typedef HashTable<string,Scene*>  TScenes;

}

#endif