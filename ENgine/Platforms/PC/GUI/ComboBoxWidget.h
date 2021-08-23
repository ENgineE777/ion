
#pragma once

#include "BaseWidget.h"

class DEF_EXPORT ComboBoxWidget : public BaseWidget
{
public:
	ComboBoxWidget(int id, BaseWidget* parent, const char* name, float x, float y, float w, float h, bool sort,EAutoSizeType sizerType = BaseWidget::eAutoSizeNone, bool bEdit = false);
	virtual ~ComboBoxWidget();
	virtual void ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam);
    virtual const char* GetText();
	void ClearList();
	void AddString(const char* str);
	void SetCurString(int index);
	void SetCurString(const char* str);
	int  GetCurString();
	void SetText(const char* txt);	
    int  GetIndex(const char* txt);
    void ClearText();
	void GetTextFromWidget();
};
