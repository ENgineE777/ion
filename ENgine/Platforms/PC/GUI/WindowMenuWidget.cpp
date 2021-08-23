
#include "WindowMenuWidget.h"

WindowMenuWidget::WindowMenuWidget(BaseWidget* prnt) : BaseWidget(0, prnt, "WinMenu", BaseWidget::eAutoSizeNone)
{
	menu = CreateMenu();
	SetMenu( prnt->GetHandle(), menu );	

	cur_depth = 0;
	depth_menu[cur_depth] = menu;	
}

void WindowMenuWidget::ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_COMMAND)
        EventsWidget::SetEvent(LOWORD(wParam), EventsWidget::menuitem_click);
}

void WindowMenuWidget::Show(bool set)
{
	if (set)
	{
		SetMenu( parent->GetHandle(), menu );	
	}
	else
	{
		SetMenu( parent->GetHandle(), null );	
	}

	BaseWidget::Show(set);
}

void WindowMenuWidget::AddMenuItem(int id, const char* name,unsigned flags)
{	
    AppendMenu(depth_menu[cur_depth], MF_STRING | flags, id, name);
    vMenuItemParents.Insert(id,depth_menu[cur_depth]);
}

void WindowMenuWidget::AddMenuItem(HMENU hParent,int id, const char* name , unsigned flags /*= 0*/)
{
    AppendMenu(hParent, MF_STRING | flags, id, name);
    vMenuItemParents.Insert(id,hParent);
}

void WindowMenuWidget::AddSeparator()
{
	AppendMenu(depth_menu[cur_depth], MF_SEPARATOR, id, 0);   
}

void WindowMenuWidget::StartSubMenu(const char* name)
{
	cur_depth++;
	depth_menu[cur_depth] = CreatePopupMenu();	
    vSubMenus[name] = depth_menu[cur_depth];
}

void WindowMenuWidget::EndSubMenu(const char* name)
{
	if (cur_depth == 0) return;

	AppendMenu(depth_menu[cur_depth-1], MF_POPUP, (UINT)depth_menu[cur_depth], name);

	cur_depth--;
}

void WindowMenuWidget::ModifyMenuItem(int id, const char* name,unsigned flags)
{
    ModifyMenu(*vMenuItemParents.Find(id),id,(UINT)flags,id,name);
}

void WindowMenuWidget::ModifyMenuItem(int id,unsigned flags)
{
    char sName[128];
    HMENU* phMenu = vMenuItemParents.Find(id);
    GetMenuString(*phMenu,id,sName,128,MF_BYCOMMAND);
    ModifyMenu(*phMenu,id,(UINT)flags,id,sName);
}

bool WindowMenuWidget::IsChecked(int id)
{
    UINT state = GetMenuState(*vMenuItemParents.Find(id),id, MF_BYCOMMAND);
    return state & MF_CHECKED;
}

void WindowMenuWidget::Delete(unsigned id)
{
    HMENU* pHandle = vMenuItemParents.Find(id);
    if(pHandle)
     DeleteMenu(*pHandle,id,MF_BYCOMMAND);
}
HMENU* WindowMenuWidget::GetSubMemu(const string& sName) 
{
    return vSubMenus.Find(sName);
}

void WindowMenuWidget::EnableSubMenu(const string& sName,bool bflag)
{
    HMENU* pHandle =  vSubMenus.Find(sName);
    if(pHandle)
       EnableMenuItem(menu,(UINT_PTR)*pHandle,bflag? MF_ENABLED: MF_GRAYED);
}

