#pragma once
#if defined(PC) || defined(OSUNIX)
#include "Common/Utils.h"
namespace ProjectInfo
{
    class ITree;
}
class ISceneTreeView
{
public:


    enum EPopUPMenu 
    { 
        ePopUpCopy = 1, 
        ePopUpDelete = 2, 
        ePopUpCutPaste = 4, 
        ePopUpCreateFolder = 8, 
        ePopUpCreateWidget = 16, 
        ePopUpConvertToSymbol = 32, 
        ePopUpSaveScene = 64, 
        ePopUpAddScene = 128,
        ePopUpAddScript = 256,
        ePopUpMakeSceneAsStart = 512,
        ePopUpCreateInstances = 1024
    };

    struct TreeViewItem
    {
    public:
        typedef std::vector<TreeViewItem>  TItemsVector;

        enum EType {eScene,eFolder,eWidget,eHelper,eNone};

        string              sName;
        EType               eType;
        int                 nImageIndex;
        void*               pHandle; 
        void*               pObject;
        void*               pProxyObject;
        TItemsVector        vChilds;
        TreeViewItem():eType(eNone),pHandle(NULL),pObject(NULL),pProxyObject(NULL),nImageIndex(0){}
        TreeViewItem(const TreeViewItem& rOther){Copy(rOther);}
        TreeViewItem& operator = (const TreeViewItem& rOther)
        {
            Copy(rOther);
            return *this;
        }
    private:
        void Copy(const TreeViewItem& rOther)
        {
            eType = rOther.eType;
            sName = rOther.sName;
            pHandle = rOther.pHandle;
            pObject = rOther.pObject;
            pProxyObject = rOther.pProxyObject;
            nImageIndex = rOther.nImageIndex;
            vChilds.assign(rOther.vChilds.begin(),rOther.vChilds.end());
        }
    };


    virtual const char* GetEditedText() = 0;
    virtual const char* GetItemText(void* pHandle) const = 0;

    virtual void  SetTreeLogic(ProjectInfo::ITree* pLogic) = 0;
    virtual ProjectInfo::ITree*  GetTreeLogic() const  = 0;

    virtual void* GetParent(void* pHandle)const = 0;
    virtual void* GetFirstItem(void* pHandle) const = 0;
    virtual void* GetNextItem(void* pHandle) const = 0;
    virtual void* GetPrevItem(void* pHandle) const = 0;
    virtual void  DeleteItems() = 0;	
    virtual void  DeleteItem(void* pHandle) = 0;
    virtual void  SortItems(bool bRecursive) = 0;
    virtual void  ForciblySortRoot() = 0;
    virtual void  ForciblySort(void* item) = 0;
    virtual void* CreateFolder(const char* txt,void* parent, void* insert_after,const char*  toolTip = null) = 0;
    virtual void* AddSceneItem(const char* txt, int image, void* ptr, void* parent, void* insert_after, const char* toolTip = null) = 0;
    virtual void* AddHelperItem(const char* txt, int image, void* ptr, void* parent, void* insert_after, const char* toolTip = null) = 0;
    virtual void* AddEntityItem(const char* txt, bool bIncluded, void* pHandle, void* parent, void* insert_after,const char*  toolTip = null) = 0;
    virtual void* AddEntityItem(const char* txt, bool bIncluded, void* pHandle, void* parent, int weigth,const char*  toolTip = null) = 0;
    virtual void* AddItem(const char* txt, int image_index, void* pHandle, void* parent, void* insert_after,const char*  toolTip = null) = 0;
    virtual bool  CheckFolderName(void* pHandle, const char* name) = 0;
    virtual void  SetItemText(void* pHandle, const char* txt) = 0;
    virtual void  SetItemImage(void* ptr,int index) = 0;
    virtual void* GetSelectedItem() const = 0;
    virtual void  SelectItem(void* pHandle) = 0;	
    virtual bool  IsFolder(void* pHandle) const = 0;
    virtual void* GetRoot() const = 0;
    virtual void  GetItemInfo(void* pHandle,ISceneTreeView::TreeViewItem* pTreeItem) const = 0;
    virtual void  GetItemInfoRecursive(void* pHandle,ISceneTreeView::TreeViewItem* pTreeItem) const = 0;
    virtual void* AddItem(void* pParentHandle,ISceneTreeView::TreeViewItem* pTreeItem,void* pInsertAfter = NULL) = 0;
    virtual void* GetHandleData(void* ptr) const = 0;
    virtual void* GetPopUpCutItem()  const  = 0;
    virtual ISceneTreeView* GetPopUpCutTreeWidget() const = 0;
};
#endif