
#include "TabSheetWidget.h"
#include "CommCtrl.h"
#include "Core/ICore.h"


TabSheetWidget::TabSheetWidget(int set_id, BaseWidget* prnt, const char* txt, float set_x, float set_y, float w, float h,EAutoSizeType sizerType) : BaseWidget(set_id, prnt, txt, sizerType)
{
	x = set_x;
	y = set_y;
	width = w;
	height = h;		

	handle = CreateWindow(WC_TABCONTROL, "", WS_CHILD | WS_VISIBLE, x, y, width, height,parent->GetHandle(), NULL, NULL, NULL); 	
			
	SetWindowLong(handle, GWL_ID, id);	

	cur_tab = -1;

    MakeSubClassing();

	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(handle, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
}

TabSheetWidget::~TabSheetWidget()
{
}

float TabSheetWidget::GetWidth()
{	
	return width - 8;	
}

float TabSheetWidget::GetHeight()
{	
	return height - gY_Offset - 5;
}

float TabSheetWidget::GetXOffset()
{
	return 5.0f;
}

float TabSheetWidget::GetYOffset()
{
	return gY_Offset;
}

void TabSheetWidget::ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam)
{	
	if (msg == 15)
    {
        int index  = TabCtrl_GetCurSel(handle);
        if (cur_tab == index) return;
        ShowTab(cur_tab, false);
        cur_tab = index;
        ShowTab(cur_tab, true);
        for(unsigned i = 0;i < tabs.size();++i)
            for(unsigned j = 0; j < tabs[i]->childs.size();++j)
                tabs[i]->childs[j]->ProcessWidget(msg,wParam,lParam);
    }
}

unsigned TabSheetWidget::AddTab(const char* txt)
{
	tabs.push_back(new Tab(txt));

	if (tabs.size() == 1)cur_tab = 0;

	TCITEM tie; 
	tie.mask = TCIF_TEXT | TCIF_IMAGE; 
	tie.iImage = -1; 
	tie.pszText = tabs.back()->name; 
	TabCtrl_InsertItem(handle, tabs.size()-1, &tie);
    return tabs.size() - 1;
}

void TabSheetWidget::SetTabName(int index,const char* sName)
{
    if(index < tabs.size())
    {
        String::Copy(tabs[index]->name, 64, sName);
        TCITEM tie; 
        tie.mask = TCIF_TEXT | TCIF_IMAGE; 
        tie.iImage = -1; 
        tie.pszText = tabs[index]->name; 
        TabCtrl_SetItem(handle, index, &tie);
    }
}

void TabSheetWidget::AddTabWidget(int index, BaseWidget* widget)
{
	if (index<0) return;

    if(index >= tabs.size())
        AddTab("");

	tabs[index]->childs.push_back(widget);

	widget->Show((cur_tab == index));	
}

void TabSheetWidget::SetCurrentTab(int index)
{
    if(index >= 0)
    {
        if (cur_tab == index || index >= tabs.size()) return;
        TabCtrl_SetCurSel(handle,index);
        ShowTab(cur_tab, false);
        cur_tab = index;
        ShowTab(cur_tab, true);
    }
}

int TabSheetWidget::GetCurrentTab()
{
	return cur_tab;
}

void TabSheetWidget::ShowTab(int index, bool show)
{
	if (index<0 || index>=tabs.size()) return;

	for (int i=0;i<tabs[index]->childs.size();i++)
		tabs[index]->childs[i]->Show(show);
}

void TabSheetWidget::ClearTabs()
{
    ShowTab(cur_tab,false);
    for(unsigned i = 0;i < tabs.size();++i)delete tabs[i];
    tabs.clear();
    cur_tab = -1;
    childs.clear();
    TabCtrl_DeleteAllItems(handle);
}

void TabSheetWidget::CloseTab(int index)
{
    if (cur_tab == index || index >= tabs.size()) return;
    TabCtrl_DeleteItem(handle,index);
    for(unsigned i = index;i < tabs.size() - 1;++i)tabs[i] = tabs[i + 1];
    tabs.pop_back();
}

float TabSheetWidget::gY_Offset = 30.0f;

TabSheetWidget::Tab::Tab(const char* txt)
{
   String::Copy(name, 64, txt);
}
