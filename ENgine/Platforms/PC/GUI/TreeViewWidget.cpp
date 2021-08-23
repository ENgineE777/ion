
#include "TreeViewWidget.h"
#include "CommCtrl.h"
#include "Core/ICore.h"
#include "Uxtheme.h"
#include "Entities/NGUI/NGUIWidget.h"

#include "Editor/Editor.h"

std::vector<TreeViewWidget*> TreeViewWidget::vTreeViewHolders;
HCURSOR TreeViewWidget::drag_cursor = null;
HCURSOR TreeViewWidget::normal_cursor = null;
bool TreeViewWidget::in_drag = false;
TreeViewWidget* TreeViewWidget::draged_tree_initial = null;
TreeViewWidget* TreeViewWidget::draged_tree_target = null;
TreeViewWidget* TreeViewWidget::popup_cut_tree_initial = null;
HTREEITEM TreeViewWidget::hPopUpCutMenuItem = null;

int CompareAlphapedOrder(const void* first,const void* second)
{
    if(String::CompareAlphapedOrder(*(const char**)first,*(const char**)second))return -1;
    else return 1;
}

LRESULT CALLBACK TreeViewWidget::TreeViewProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    TreeViewWidget* widget = (TreeViewWidget*)dwRefData;
    if (uMsg == WM_MOUSEMOVE)
    {
        widget->NotifyMouseOver(widget);
        for (size_t i=0;i<vTreeViewHolders.size();++i)
            if(vTreeViewHolders[i] != widget)
               vTreeViewHolders[i]->OnDrag();
    }
    widget->ProcessWidget( uMsg, wParam, lParam );
    return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);

}

TreeViewWidget::TreeViewWidget(int set_id, BaseWidget* prnt, float set_x, float set_y, float w, float h, int falgs,EAutoSizeType sizerType):BaseWidget(set_id, prnt, "", sizerType)
	,hPopUpMenuItem(null),hPopUpMenu(null),hImageList(NULL),pLogic(NULL),nFlags(falgs)
{
	x = set_x;
	y = set_y;
	width = w;
	height = h;		
	dword flag = 0;
	if (EFlags::eAllowRename & nFlags)
        flag = TVS_EDITLABELS;

	handle = CreateWindowEx(0, WC_TREEVIEW, "",
						    WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_SHOWSELALWAYS | WS_VISIBLE | TVS_EX_DOUBLEBUFFER | TVS_HASBUTTONS |  TVS_LINESATROOT | TVS_INFOTIP | flag, 
							(int)x,(int)y,(int)width,(int)height, parent->GetHandle(), NULL, NULL, NULL);	
			
	SetWindowLong(handle, GWL_ID, id);		

	sel_item = -1;

    vTreeViewHolders.push_back(this);
    SetWindowSubclass(handle, &TreeViewProc, 0, (DWORD_PTR)this);
	
	if (!drag_cursor)
	{		
		drag_cursor = LoadCursor(NULL, IDC_SIZEALL);
		normal_cursor = LoadCursor(NULL, IDC_ARROW);
	}

	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(handle, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));

	last_selection = 0;


	edited_text[0] = 0;

	num_items = 0;

    hImageList = ImageList_Create(21,21,ILC_COLOR24,0, 0);
    TreeView_SetImageList(handle, hImageList, TVSIL_NORMAL);
}

TreeViewWidget::~TreeViewWidget()
{
	for (size_t i=0;i<vTreeViewHolders.size();i++)
	{
		if (vTreeViewHolders[i] == this)
		{
			vTreeViewHolders.erase(vTreeViewHolders.begin() + i);
			break;
		}
	}
}

void TreeViewWidget::AddImage(const char* name)
{
    if (hImageList)
    {
        // Add the open file, closed file, and document bitmaps. 
        HBITMAP hbmp = (HBITMAP)LoadImage (0,name, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);	
        images.push_back(ImageList_Add(hImageList, hbmp, (HBITMAP)NULL)); 
        DeleteObject(hbmp); 
    }

}

void TreeViewWidget::OnStartDrag(LPNMTREEVIEW lpnmtv)
{ 	
	if (EFlags::eDraggEnable & nFlags)
    {
        draged_tree_initial = this;
        draged_tree_target = this;
        in_drag = true; 
        dragged_item = lpnmtv->itemNew.hItem;
        SetCursor(drag_cursor);
        SetCapture(handle); 
    }
} 

void TreeViewWidget::OnDrag()
{ 
    if (!IsHoveredByMouse()) return;
    HTREEITEM htiTarget;  // Handle to target item. 
    TVHITTESTINFO tvht;   // Hit test information. 
    if (in_drag) 
    { 
		if (draged_tree_target != this)
		{
			if (draged_tree_target)
				TreeView_SelectDropTarget(draged_tree_target->GetHandle(), null); 

			draged_tree_target = this;
			
			ReleaseCapture(); 
			SetCapture(handle);			
		}

		// Drag the item to the current position of the mouse pointer. 
        // First convert the dialog coordinates to control coordinates. 
        POINT point;
		if (GetCursorPos(&point))
			ScreenToClient(handle, &point);
        // Find out if the pointer is on the item. If it is, 
        // highlight the item as a drop target. 
        tvht.pt.x = point.x; 
        tvht.pt.y = point.y; 
        
		htiTarget = TreeView_HitTest(handle, &tvht);
        TreeView_SelectDropTarget(handle, htiTarget); 
    }     
}

void TreeViewWidget::OnEndDrag() 
{ 
    if(!in_drag)return;
    drag_target_item = TreeView_GetDropHilight(handle);
    TreeView_SelectDropTarget(handle, null); 
    SetCursor(normal_cursor);	
    ReleaseCapture(); 
    in_drag = false; 		
    EventsWidget::SetEvent(id, EventsWidget::treelist_drag_ended);

} 

void*  TreeViewWidget::AddItem(void* pParentHandle,ISceneTreeView::TreeViewItem* pTreeItem,void* pInsertAfter)
{
    ProxyPtr* pProxy = (ProxyPtr*)pTreeItem->pProxyObject;
    string sToolTip = pTreeItem->sName;
    bool bIncluded = false;
    TreeViewItem::EType eType = TreeViewItem::eWidget;
    if(pProxy)
    {
        eType = pProxy->eType;
        sToolTip = pProxy->toolTip;
        bIncluded = pProxy->bIncluded;
    }
    if(pTreeItem->eType == ISceneTreeView::TreeViewItem::eFolder)
        pParentHandle = CreateFolder(pTreeItem->sName.c_str(),pParentHandle,pInsertAfter,sToolTip.c_str());
    else
        pParentHandle = AddItemInner(pTreeItem->sName.c_str(), pTreeItem->nImageIndex, pTreeItem->pObject,pParentHandle,pInsertAfter,sToolTip.c_str(),eType,bIncluded);

    pTreeItem->pHandle = pParentHandle;

    for(unsigned i =0;i < pTreeItem->vChilds.size();++i)
        AddItem(pParentHandle,&pTreeItem->vChilds[i]);
    return  pParentHandle;
}

void TreeViewWidget::GetItemInfo(void* item,ISceneTreeView::TreeViewItem* pTreeItem) const
{
    if(!pTreeItem) return;

    // string storage
    char str[1024];

    // get TVITEM struct of item
    TVITEM tvItem;
    tvItem.hItem = (HTREEITEM)item;
    tvItem.mask = TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT | TVIF_PARAM; // all attributes
    tvItem.pszText = str;
    tvItem.cchTextMax = 1024 - 1;
    
	TreeView_GetItem(handle, &tvItem);

    // store info to TreeItem
    ProxyPtr* pProxy = reinterpret_cast<ProxyPtr*>(tvItem.lParam);
    pTreeItem->sName = str;
    pTreeItem->pHandle = item;
    pTreeItem->nImageIndex = tvItem.iImage;
	pTreeItem->pProxyObject = (void*)tvItem.lParam;
    pTreeItem->eType = ISceneTreeView::TreeViewItem::eWidget;
    if(pProxy)
    {
        pTreeItem->pObject = pProxy->ptr;
        pTreeItem->eType = pProxy->eType;
    }
}

void TreeViewWidget::CreatePopUpMenu(const ProjectInfo::TTreePopUpMenu& vMenu,HMENU hBase)
{
    if(vMenu.size() > 0)
    {
        for(unsigned i = 0;i < vMenu.size();++i)
        {
            const ProjectInfo::TreePopUpMenuItem& rItem = vMenu[i];
            switch(rItem.nId)
            {
            case ProjectInfo::ETreePopUPMenuId::ePopUpSeparatorId:
                {
                    AppendMenu(hBase, MFT_SEPARATOR, 1000, "");
                    break;
                }
            case ProjectInfo::ETreePopUPMenuId::ePopUpSubMenuId: 
                {
                    HMENU hSubMenu = CreatePopupMenu();
                    AppendMenu(hBase, MF_POPUP, (UINT_PTR)hSubMenu,rItem.sName.c_str());
                    if(rItem.bEnable)
                        CreatePopUpMenu(rItem.vSubMenu,hSubMenu);
                    else
                        EnableMenuItem(hBase,(UINT_PTR)hSubMenu,MF_GRAYED);
                    break;
                }
            default:
                {
                    AppendMenu(hBase, MFT_STRING,rItem.nId,rItem.sName.c_str());
                    if(!rItem.bEnable)
                        EnableMenuItem(hPopUpMenu,rItem.nId,MF_GRAYED);
                    break;
                }
            }
        }

    }
}

void TreeViewWidget::ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam)
{
	if (!IsVisible()) return;

    if (msg == WM_MOUSEMOVE)
	{		
		OnDrag();
		return;
	}

	if (msg == WM_LBUTTONUP)
	{
		OnEndDrag();
		return;
	}

	if (msg == WM_LBUTTONDBLCLK)
	{
		EventsWidget::SetEvent(id, EventsWidget::treelist_double_click);
		return;
	}		
	if(msg == WM_COMMAND && HIWORD(wParam) == 0)
	{
		switch(LOWORD(wParam))
		{
        case ProjectInfo::ETreePopUPMenuId::ePopUpIncludeScene:
            {
                if(hPopUpMenuItem)
                    EventsWidget::SetEvent(id, EventsWidget::popupmenu_include_scene);
                return;

            }
        case ProjectInfo::ETreePopUPMenuId::ePopUpMakeSeneAsStartId:
            {
                if(hPopUpMenuItem)
                    EventsWidget::SetEvent(id, EventsWidget::popupmenu_set_as_startup_scene);
                return;
            }
        case ProjectInfo::ETreePopUPMenuId::ePopUpAddNewSceneId:
            {
                EventsWidget::SetEvent(id, EventsWidget::popupmenu_add_new_scene);
                return;
            }
        case ProjectInfo::ETreePopUPMenuId::ePopUpAddExistingSceneId:
            {
                EventsWidget::SetEvent(id, EventsWidget::popupmenu_add_existing_scene);
                return;
            }
        case ProjectInfo::ETreePopUPMenuId::ePopUpAddNewScriptId:
            {
                EventsWidget::SetEvent(id, EventsWidget::popupmenu_add_new_script);
                return;
            }
        case ProjectInfo::ETreePopUPMenuId::ePopUpAddExistingSriptdId:
            {
                EventsWidget::SetEvent(id, EventsWidget::popupmenu_add_existing_script);
                return;
            }

        case ProjectInfo::ETreePopUPMenuId::ePopUpSaveSceneAsId:
            {
                if(hPopUpMenuItem)
                    EventsWidget::SetEvent(id, EventsWidget::popupmenu_save_scene_as);
                return;
            }
        case ProjectInfo::ETreePopUPMenuId::ePopUpSaveSceneAsAndDuplicateId:
            {
                if(hPopUpMenuItem)
                    EventsWidget::SetEvent(id, EventsWidget::popupmenu_save_scene_as_duplicate);
                return;
            }
        case  ProjectInfo::ETreePopUPMenuId::ePopUpSaveSceneId:
            {
                if(hPopUpMenuItem)
                    EventsWidget::SetEvent(id, EventsWidget::popupmenu_save_scene);
                return;
            }
        case  ProjectInfo::ETreePopUPMenuId::ePopUpCopyId:
			{
				if(hPopUpMenuItem)
					EventsWidget::SetEvent(id, EventsWidget::popupmenu_copy);
				return;
			}
        case ProjectInfo::ETreePopUPMenuId::ePopUpGetFullName:
            {
                if(hPopUpMenuItem)
                    EventsWidget::SetEvent(id, EventsWidget::popupmenu_get_full_whidget_name);
                return;
            }
        case ProjectInfo::ETreePopUPMenuId::ePopUpRollBackChanges:
            {
                if(hPopUpMenuItem)
                    EventsWidget::SetEvent(id, EventsWidget::popupmenu_rollback_changes);
                return;
            }
        case ProjectInfo::ETreePopUPMenuId::ePopUpCutId:
			{
				popup_cut_tree_initial = this;
				hPopUpCutMenuItem = hPopUpMenuItem;
				hPopUpMenuItem = null;
				return;
			}
        case  ProjectInfo::ETreePopUPMenuId::ePopUpPasteId:
			{
				if(popup_cut_tree_initial && hPopUpCutMenuItem)
				{
					if (popup_cut_tree_initial == this)
					{
                        last_selection = TreeView_GetSelection(handle);
					}
                    EventsWidget::SetEvent(id, EventsWidget::popupmenu_paste);
				}
				return;
			}
        case  ProjectInfo::ETreePopUPMenuId::ePopUpDeleteId:
			{
				if(hPopUpMenuItem)
					EventsWidget::SetEvent(id, EventsWidget::popupmenu_delete);
                if(hPopUpMenuItem == hPopUpCutMenuItem)
                    ClearCutData();
				return;
			}
        case  ProjectInfo::ETreePopUPMenuId::ePopUpConvertToSymbolId:
            {
                if(hPopUpMenuItem)
                    EventsWidget::SetEvent(id, EventsWidget::popupmenu_convert_to_symbol);
                return;
            }
        case  ProjectInfo::ETreePopUPMenuId::ePopUpCreateFolderId:
            {
                EventsWidget::SetEvent(id, EventsWidget::popupmenu_create_folder);
                return;
            }
		}
        int nPopUpCreateWidgetId  = LOWORD(wParam);
        for(unsigned i = 0;i < vMenu.size();++i)
        {
            const char* sName  = vMenu[i].Find(nPopUpCreateWidgetId);
            if(sName)
            {
                sPopUpCreateWidgetName = sName;
                break;
            }
        }
        if(nPopUpCreateWidgetId -  ProjectInfo::ETreePopUPMenuId::ePopUpCreateInstanceId < 0)
            EventsWidget::SetEvent(id, EventsWidget::popupmenu_create_widget);
        else
            EventsWidget::SetEvent(id, EventsWidget::popupmenu_create_instance);
	    return;
	}
	if (msg == WM_NOTIFY)
	{	
		if (((LPNMHDR) lParam)->code == TVN_GETINFOTIPA || ((LPNMHDR) lParam)->code == TVN_GETINFOTIPW)
		{
				LPNMTVGETINFOTIP pTip = (LPNMTVGETINFOTIP)lParam;
				ISceneTreeView::TreeViewItem treeItem;
				GetItemInfo(pTip->hItem, &treeItem);
				ProxyPtr* proxy = (ProxyPtr*)treeItem.pProxyObject;
				String::Copy(pTip->pszText, 512, proxy->toolTip);
                return;
			
		}
		if ( ((LPNMHDR)lParam)->code == NM_CLICK)
		{
			EventsWidget::SetEvent(id, EventsWidget::treelist_click);
			return;
		}		

		if(((LPNMHDR)lParam)->code == NM_RCLICK)
		{
			LPNMHDR lpnmh = (LPNMHDR) lParam;
			TVHITTESTINFO ht = {0};
			DWORD dwpos = GetMessagePos();
			ht.pt.x = GET_X_LPARAM(dwpos);
			ht.pt.y = GET_Y_LPARAM(dwpos);
			MapWindowPoints(HWND_DESKTOP, lpnmh->hwndFrom, &ht.pt, 1);
			TreeView_HitTest(lpnmh->hwndFrom, &ht);
            hPopUpMenuItem = NULL;
			if(TVHT_ONITEM & ht.flags)
			{
                ISceneTreeView::TreeViewItem  info;
                GetItemInfo(ht.hItem, &info);
                if(String::IsEqual(info.sName.c_str(),"root"))return;
				hPopUpMenuItem = ht.hItem;
            }
            vMenu.clear();
			if (pLogic)
				pLogic->GetPopUpMenu(hPopUpMenuItem,vMenu);
            if(vMenu.size() > 0)
            {
                if(hPopUpMenu)
                    DestroyMenu(hPopUpMenu);
                hPopUpMenu = CreatePopupMenu();
                CreatePopUpMenu(vMenu,hPopUpMenu);
            }
            POINT p = ht.pt;
            ClientToScreen(handle,&p);
            SetForegroundWindow(handle);
            TrackPopupMenuEx(hPopUpMenu,TPM_TOPALIGN | TPM_LEFTALIGN   , p.x, p.y,handle, 0);
			return;
		}
		if ( ((LPNMHDR)lParam)->code == TVN_BEGINDRAGW || ((LPNMHDR)lParam)->code == TVN_BEGINDRAGA)
		{
			OnStartDrag((LPNMTREEVIEW)lParam);
            return;
		}

		if ( ((LPNMHDR)lParam)->code == TVN_ENDLABELEDITW  || ((LPNMHDR)lParam)->code == TVN_ENDLABELEDITA)
		{
			LPNMTVDISPINFO ptvdi = (LPNMTVDISPINFO)lParam;
			if (ptvdi->item.pszText)
			{
				int index = 0;
                int step = 1;
                if( ((LPNMHDR)lParam)->code == TVN_ENDLABELEDITW)
                    step = 2;		
				while (ptvdi->item.pszText[index * step] != 0)
				{
					edited_text[index] = ptvdi->item.pszText[index * step];
					index++;
				}

				edited_text[index] = ptvdi->item.pszText[index * step];

				EventsWidget::SetEvent(id, EventsWidget::treelist_labeledited);				
			}
		}

		return;
	}
}

void* TreeViewWidget::AddSceneItem(const char* txt, int image, void* ptr, void* parent, void* insert_after, const char* toolTip /*= null*/)
{
	return AddItemInner(txt,image, ptr, parent, insert_after, toolTip,ISceneTreeView::TreeViewItem::eScene,false);
}

void* TreeViewWidget::AddHelperItem(const char* txt, int image, void* ptr, void* parent, void* insert_after, const char* toolTip /*= null*/)
{
    return AddItemInner(txt,image, ptr, parent, insert_after, toolTip,ISceneTreeView::TreeViewItem::eHelper,false);
}


void* TreeViewWidget::AddEntityItem(const char* txt, bool bIncluded, void* ptr, void* parent, int weigth,const char*  toolTip)
{
    int count = weigth;
    void *pIter = GetFirstItem(parent);
    while(pIter)
    {
        const char* sName = GetItemText(pIter);
        if(!String::IsEqual(sName,"childs") &&  !String::IsEqual(sName,"root"))
                count--;
        if(count <= 0)
            break;
        pIter = GetNextItem(pIter);
    }
    return AddItemInner(txt,bIncluded ? 2 : 1, ptr, parent, pIter,toolTip,ISceneTreeView::TreeViewItem::eWidget,bIncluded);
}

void* TreeViewWidget::AddEntityItem(const char* txt, bool bIncluded, void* ptr, void* parent, void* insert_after,const char*  toolTip)
{
	return AddItemInner(txt,bIncluded ? 2 : 1, ptr, parent, insert_after,toolTip,ISceneTreeView::TreeViewItem::eWidget,bIncluded);
}

void* TreeViewWidget::AddItem(const char* txt, int image_index, void* ptr, void* parent, void* insert_after,const char*  toolTip)
{
    return AddItemInner(txt, images[image_index], ptr, parent, insert_after,toolTip);
}

void* TreeViewWidget::AddItemInner(const char* txt, int image_index, void* ptr, void* parent, void* insert_after,const char*  toolTip,TreeViewItem::EType eType,bool bIncluded)
{
	num_items++;

	TVITEM tvi; 
	TVINSERTSTRUCT tvins;		
	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM; 
	tvi.pszText = (LPSTR)txt; 
	tvi.cchTextMax = 128; 
	ProxyPtr* proxy = new ProxyPtr();
	proxy->ptr = ptr;
    proxy->eType = eType;
    proxy->bIncluded = bIncluded; 

	String::Copy(proxy->text, 512, txt);
	if(toolTip)
		String::Copy(proxy->toolTip, 512, toolTip);
	else
		proxy->toolTip[0] = 0;

	proxies_ptr.push_back(proxy);

	tvi.lParam = (LPARAM)proxy;
	tvi.iImage = image_index; 
	tvi.iSelectedImage = image_index; 	
	tvi.state = TVIS_EXPANDED;

	tvins.item = tvi; 
	tvins.hInsertAfter = (HTREEITEM)insert_after;
	if (parent)
		tvins.hParent = (HTREEITEM)parent; 
	else
		tvins.hParent = TVI_ROOT; 
	HTREEITEM item = (HTREEITEM)TreeView_InsertItem(handle, &tvins);	
    InvalidateRect(handle,NULL,false);
	return item;	
}

void* TreeViewWidget::CreateFolder(const char* txt,void* parent, void* insert_after,const char* toolTip /*= null*/)
{
    TVITEM tvi; 
    TVINSERTSTRUCT tvins;		
    tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM; 
    
    ProxyPtr* proxy = new ProxyPtr();
    if(!txt)
    {
        char name[128];
        int number = 0;			
        bool satisfy = false;
        while (!satisfy)
        {		
            String::Printf(name, 256, "Folder%i", number);
            satisfy = CheckFolderName(parent, name);
            ++number;
        }
        tvi.pszText = (LPSTR)name; 
        String::Copy(proxy->text, 512, name);
    }
    else
    {
        tvi.pszText = (LPSTR)txt; 
        String::Copy(proxy->text, 512, txt);
    }
    proxy->ptr = NULL;
    proxy->eType = ISceneTreeView::TreeViewItem::eFolder;
    if(toolTip)
        String::Copy(proxy->toolTip, 512, toolTip);
    else
        proxy->toolTip[0] = 0;

    proxies_ptr.push_back(proxy);

    tvi.lParam = (LPARAM)proxy;
    tvi.iImage = 0; 
    tvi.iSelectedImage = 0; 	
    tvi.state = TVIS_EXPANDED;
    tvi.cchTextMax = 128; 

    tvins.item = tvi; 
    tvins.hInsertAfter = (HTREEITEM)insert_after;
    if (parent)
        tvins.hParent = (HTREEITEM)parent; 
    else
        tvins.hParent = TVI_ROOT; 
    HTREEITEM item = (HTREEITEM)TreeView_InsertItem(handle, &tvins);	
    ++num_items;
    SortChilds(parent,false);
    return item;

}

void TreeViewWidget::SetItemImage(void* ptr,int index)
{
    if(!ptr)return;
    TVITEM tvitem;

    tvitem.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tvitem.hItem = (HTREEITEM)ptr;
    tvitem.iImage = images[index];
    tvitem.iSelectedImage = images[index];
    TreeView_SetItem(handle, &tvitem);

}

void TreeViewWidget::SetItemText(void* ptr, const char* txt)
{
    if(!ptr)return;
	TVITEM tvitem;
					
	tvitem.mask = TVIF_TEXT;
	tvitem.hItem = (HTREEITEM)ptr;
					
	tvitem.pszText = (char*)txt;
	tvitem.cchTextMax = sizeof(txt) / sizeof(txt);


	TreeView_SetItem(handle, &tvitem);
	
	void* parent = GetParent(ptr);

	SortChilds(parent, false);	
}

const char*  TreeViewWidget::GetItemText(void* ptr) const
{
    static char sBuffer[256];
    ISceneTreeView::TreeViewItem treeItem;
    GetItemInfo((HTREEITEM)ptr, &treeItem);
    String::Copy(sBuffer, 256, treeItem.sName.c_str());	
    return sBuffer;
}

bool TreeViewWidget::IsFolder(void* ptr) const
{
    if(!ptr)return false;
    ISceneTreeView::TreeViewItem treeItem;
    GetItemInfo((HTREEITEM)ptr, &treeItem);
    return treeItem.eType == ISceneTreeView::TreeViewItem::eFolder;
}

void TreeViewWidget::SelectItem(void* ptr)
{
	TreeView_SelectItem(handle, ptr);	
	last_selection = TreeView_GetSelection(handle);
}

void TreeViewWidget::DeleteItem(void* pHandle)
{
    if (pHandle == 0) return;

    ISceneTreeView::TreeViewItem  rItem;
    GetItemInfo(pHandle,&rItem);
    DeleteProxy((ProxyPtr*)rItem.pProxyObject);

    void* pIter = GetFirstItem(pHandle);
    while(pIter)
    {
        DeleteItem(pIter);
        pIter = GetNextItem(pIter);
    }

    num_items--;
    TreeView_DeleteItem(handle, pHandle);	
    InvalidateRect(handle,NULL,false);
}

void* TreeViewWidget::GetRoot() const
{
	return TreeView_GetRoot(handle);
}

void* TreeViewWidget::GetParent(void* ptr) const
{
	return TreeView_GetParent(handle, ptr);
}

void* TreeViewWidget::GetSelectedItem() const
{
	return TreeView_GetSelection(handle);
}

void* TreeViewWidget::GetFirstItem(void* ptr) const
{
	return TreeView_GetChild(handle, ptr);
}

void* TreeViewWidget::GetNextItem(void* ptr) const
{
	return TreeView_GetNextSibling(handle, ptr);
}

void* TreeViewWidget::GetPrevItem(void* ptr) const 
{
	return TreeView_GetPrevSibling(handle, ptr);
}

void* TreeViewWidget::GetHandleData(void* ptr) const
{	
    if(!ptr)return NULL;
	ISceneTreeView::TreeViewItem treeItem;
	GetItemInfo((HTREEITEM)ptr, &treeItem);
	return ((ProxyPtr*)treeItem.pProxyObject)->ptr;
}

void TreeViewWidget::DeleteItems()
{
	num_items = 0;
	TreeView_DeleteAllItems( handle );
	ClearProxies();
}

bool TreeViewWidget::CheckFolderName(void* ptr, const char* name)
{
	HTREEITEM item = TreeView_GetChild(handle, ptr);
    while(item != 0)
    {
        ISceneTreeView::TreeViewItem  treeItem;
        GetItemInfo(item, &treeItem);
		if ( ((ProxyPtr*)treeItem.pProxyObject)->eType == ISceneTreeView::TreeViewItem::eFolder && String::IsEqual(treeItem.sName.c_str(), name))
				return false;
        item = TreeView_GetNextSibling(handle, item);
    }
	return true;
}

const char* TreeViewWidget::GetEditedText()
{
	return edited_text;
}

void TreeViewWidget::SortItems(bool recursive)
{
	SortChilds(TVI_ROOT, recursive);
}

int CALLBACK TreeViewWidget::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	ProxyPtr* pr1 = (ProxyPtr*)lParam1;
	ProxyPtr* pr2 = (ProxyPtr*)lParam2;
	if (pr1->eType == ISceneTreeView::TreeViewItem::eFolder && !pr2->eType == ISceneTreeView::TreeViewItem::eFolder)
		return false;
	if (!pr1->eType == ISceneTreeView::TreeViewItem::eFolder && pr2->eType == ISceneTreeView::TreeViewItem::eFolder)
		return true;
	return !String::CompareAlphapedOrder(pr1->text, pr2->text);	
}

void TreeViewWidget::ForciblySort(void* item)
{
    TVSORTCB cb;
    cb.hParent = (HTREEITEM)item;
    cb.lpfnCompare = CompareFunc;
    cb.lParam = (LPARAM)this;
    TreeView_SortChildrenCB(handle, &cb, 0);
}

void TreeViewWidget::SortChilds(void* item, bool recursive)
{	
	if (EFlags::eSorted & nFlags)
    {
        if(recursive)
        {
            void* pIter = GetFirstItem(item);
            while(pIter)
            {
                SortChilds(pIter, true);
                pIter = GetNextItem(pIter);
            }
        }
        ForciblySort(item);
    }
}

void TreeViewWidget::DeleteProxy(ProxyPtr* proxy)
{	
	for (size_t i=0; i<proxies_ptr.size();i++)
	{		
		if (proxies_ptr[i] == proxy)
		{			
			delete proxies_ptr[i];
			
			proxies_ptr[i] = proxies_ptr[proxies_ptr.size() - 1];
			proxies_ptr.pop_back();

			break;
		}
	}
}

void TreeViewWidget::ClearProxies()
{
	for (size_t i=0; i<proxies_ptr.size();i++)
		delete proxies_ptr[i];
	proxies_ptr.clear();
}

void  TreeViewWidget::GetItemInfoRecursive(void* pHandle,ISceneTreeView::TreeViewItem* pTreeItem) const
{
    if (pHandle == 0) return;

    GetItemInfo(pHandle,pTreeItem);
    pHandle = GetFirstItem(pHandle);

    while(pHandle)
    {
        pTreeItem->vChilds.push_back(ISceneTreeView::TreeViewItem());
        GetItemInfoRecursive(pHandle,&pTreeItem->vChilds.back());
        pHandle = GetNextItem(pHandle);
    }
}





