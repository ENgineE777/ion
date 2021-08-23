
#pragma once

#include "BaseWidget.h"

class DEF_EXPORT ListBoxWidget : public BaseWidget
{
	bool is_multi_select;

	int  sel_count;
	int* sel_buffer;

public:    

	ListBoxWidget(int id, BaseWidget* parent, const char* name, float x, float y, float w, float h, bool alphabet_order, bool multi_select, EAutoSizeType sizerType = BaseWidget::eAutoSizeNone);
	virtual ~ListBoxWidget();
	virtual void ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam);
	virtual void ClearList();
	virtual int AddString(const char* str, int meta_data);	
    virtual bool CatchEraseBkgnd(){return false;}
    
    virtual void SelectText(const string& sText);	
	virtual int  GetSelectedItemsCount();	
	virtual int GetSelectedItem(int index, string& sText);	
	virtual int GetSelectedItem(int index, string& sText, int& meta_data);	

    virtual int GetSelectedIndex(int index = 0);	
};
