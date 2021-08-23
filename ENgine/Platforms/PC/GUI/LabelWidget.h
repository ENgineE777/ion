
#pragma once

#include "BaseWidget.h"

class DEF_EXPORT LabelWidget : public BaseWidget
{	
	bool color_box;

public:

	Color color;

	LabelWidget(int id, BaseWidget* parent, const char* txt, bool color_box, float x, float y, float w, float h,EAutoSizeType sizerType = BaseWidget::eAutoSizeNone,int align = SS_LEFTNOWORDWRAP);
	virtual ~LabelWidget();

	virtual void ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam);
	virtual void SetText(const char* txt);

	virtual void OnLeftMouseUp(int mx, int my);

	virtual void Draw();

};
