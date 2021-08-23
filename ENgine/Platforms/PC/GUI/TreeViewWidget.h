
#pragma once

#include "BaseWidget.h"
#include "Editor/Helpers/View/ISceneTreeView.h"
#include "Editor/Helpers/SceneTree.h"

class NGUIWidget;

class DEF_EXPORT TreeViewWidget : public BaseWidget, public ISceneTreeView
{
    static HCURSOR drag_cursor;
    static HCURSOR normal_cursor;	
    static TreeViewWidget* popup_cut_tree_initial;
    static TreeViewWidget* draged_tree_initial;
    static TreeViewWidget* draged_tree_target;

    static std::vector<TreeViewWidget*> vTreeViewHolders;
	
    ProjectInfo::ITree* pLogic;

	vector<Str256> imgaes_path;
	vector<int> images;

	char edited_text[1024];
    int  sel_item;

	vector<ISceneTreeView::TreeViewItem>  items;
	int num_items;
	

    int  nFlags; 


    HTREEITEM last_selection;
	HTREEITEM dragged_item;	
	HTREEITEM drag_target_item;	

    HIMAGELIST hImageList;

	HMENU     hPopUpMenu; 
    ProjectInfo::TTreePopUpMenu vMenu;
    string    sPopUpCreateWidgetName;


	HTREEITEM           hPopUpMenuItem;
	static HTREEITEM    hPopUpCutMenuItem;

	struct ProxyPtr
	{
		void* ptr;
        TreeViewItem::EType eType;
        bool  bIncluded;
		bool  bStartScene;
		char  text[512];
		char  toolTip[512];
		ProxyPtr() :ptr(0),eType(TreeViewItem::eWidget),bStartScene(false),bIncluded(false)
        {
            text[0] = toolTip[0] = 0;
        }
	};
	std::vector<ProxyPtr*> proxies_ptr;
         

	void DeleteProxy(ProxyPtr* proxy);
	void ClearProxies();


	
public:

    enum EFlags 
    { 
        eSorted = 1, 
        eCreateElementsInTopFolders = 4, 
        eDraggEnable = 8,
        eAllowRename = 16,
        eCreateInstances = 32,
    };

	static bool in_drag;


	TreeViewWidget(int id, BaseWidget* parent, float x, float y, float w, float h, int  flags = 0,EAutoSizeType sizerType = EAutoSizeType::eAutoSizeNone);
	~TreeViewWidget();


	virtual void AddImage(const char* name);

    virtual const char* GetEditedText();
    virtual const char* GetItemText(void* ptr) const;
    virtual void* GetParent(void* ptr)const;
    virtual void* GetFirstItem(void* ptr) const;
    virtual void* GetNextItem(void* ptr) const;
    virtual void* GetPrevItem(void* ptr) const;
    virtual void  DeleteItems();	
    virtual void  DeleteItem(void* ptr);
    virtual void  SortItems(bool bRecursive);
    virtual void  ForciblySortRoot(){ForciblySort(TVI_ROOT);} 
    virtual void  ForciblySort(void* item);
    virtual void* CreateFolder(const char* txt,void* parent, void* insert_after,const char*  toolTip = null);
    virtual void* AddEntityItem(const char* txt, bool bIncluded, void* ptr, void* parent, int weigth,const char*  toolTip = null);
    virtual void* AddEntityItem(const char* txt, bool bIncluded, void* ptr, void* parent, void* insert_after,const char*  toolTip = null);
    virtual void* AddItem(const char* txt, int image_index, void* ptr, void* parent, void* insert_after,const char*  toolTip = null);
    virtual bool  CheckFolderName(void* ptr, const char* name);
    virtual void  SetItemImage(void* ptr,int index);
    virtual void  SetItemText(void* ptr, const char* txt);
    virtual void* GetSelectedItem() const;
    virtual void  SelectItem(void* ptr);	
    virtual bool  IsFolder(void* ptr) const;
    virtual void  SetTreeLogic(ProjectInfo::ITree* _pLogic){pLogic = _pLogic;}
    virtual ProjectInfo::ITree*  GetTreeLogic() const {return pLogic;}
    virtual void* GetRoot() const;
    virtual void  GetItemInfo(void* item,ISceneTreeView::TreeViewItem* pTreeItem) const;
    virtual void  GetItemInfoRecursive(void* pHandle,ISceneTreeView::TreeViewItem* pTreeItem) const;
    virtual void* AddItem(void* pParentHandle,ISceneTreeView::TreeViewItem* pTreeItem,void* pInsertAfter = NULL);
    virtual void* GetHandleData(void* ptr) const;

    virtual void* GetPopUpCutItem() const {return hPopUpCutMenuItem;}
    virtual ISceneTreeView* GetPopUpCutTreeWidget() const {return popup_cut_tree_initial;}


    virtual void* AddSceneItem(const char* txt, int image, void* ptr, void* parent, void* insert_after, const char*  toolTip = null);
    virtual void* AddHelperItem(const char* txt, int image, void* ptr, void* parent, void* insert_after, const char* toolTip = null);


	virtual void  ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam);

	
	inline virtual const string&  GetPopUpCreatingWidgetName(){return sPopUpCreateWidgetName;}

	inline  virtual void* GetPopUpItem() const {return hPopUpMenuItem;}
	inline  virtual void  ClearPopUpItem(){hPopUpMenuItem = null;}

    
    inline  virtual void  ClearCutData(){hPopUpCutMenuItem = null;popup_cut_tree_initial = null;}

	inline virtual void*           GetDragegItem(){return dragged_item;}
    inline virtual void*           GetDragTargetItem(){return drag_target_item;}
    inline virtual TreeViewWidget* GetDragSourceWidget(){return draged_tree_initial;}
        
	void OnStartDrag(LPNMTREEVIEW lpnmtv);	
	void OnDrag();
	void OnEndDrag();	

private:
    void* AddItemInner(const char* txt, int image_index, void* ptr, void* parent, void* insert_after,const char*  toolTip = null,TreeViewItem::EType eType = TreeViewItem::EType::eWidget,bool bIncluded = false);
    void  SortChilds(void* item, bool recursive);

   
	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
    static LRESULT CALLBACK TreeViewProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);


     void CreatePopUpMenu(const ProjectInfo::TTreePopUpMenu& vMenu,HMENU hBase);

};

