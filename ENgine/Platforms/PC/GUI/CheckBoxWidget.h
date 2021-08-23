
#pragma once

#include "BaseWidget.h"

class DEF_EXPORT CheckBoxWidget : public BaseWidget
{		
	bool checked;

public:
	
	CheckBoxWidget(int id, BaseWidget* parent, const char* txt, float x, float y, float w, float h,EAutoSizeType sizerType = eAutoSizeNone);
	virtual ~CheckBoxWidget();

	virtual void ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam);
	virtual void SetText(const char* txt);	
	void SetChecked(bool set);
	bool IsChecked();
};
