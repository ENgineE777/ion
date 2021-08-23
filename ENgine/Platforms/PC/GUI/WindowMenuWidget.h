
#pragma once

#include "BaseWidget.h"

class DEF_EXPORT WindowMenuWidget : public BaseWidget
{
	HMENU menu;
	
	int cur_depth;
	HMENU depth_menu[16];	

    typedef HashTable<int,HMENU>     TMenuItemParents;
    typedef HashTable<string,HMENU>  TSubMenus;
    TMenuItemParents    vMenuItemParents;
    TSubMenus           vSubMenus;
public:
	
	WindowMenuWidget(BaseWidget* parent);
	virtual void ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam);	
	virtual void Show(bool set);

	void AddMenuItem(int id, const char* name , unsigned flags = 0);
    void AddMenuItem(HMENU hParent,int id, const char* name , unsigned flags = 0);

    void ModifyMenuItem(int id, const char* name,unsigned flags);
    void ModifyMenuItem(int id,unsigned flags);
    bool IsChecked(int id);
	void AddSeparator();
	void StartSubMenu(const char* name);
	void EndSubMenu(const char* name);	
    void Delete(unsigned id);
    HMENU* GetSubMemu(const string& sName);
    void EnableSubMenu(const string& sName,bool bflag);
   
};
