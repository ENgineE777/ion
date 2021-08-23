
#pragma once

#include "BaseWidget.h"

class DEF_EXPORT ScrollBarWidget : public BaseWidget
{
	int cur_pos;
	int max_pos;
	int page_size;
	bool horiz;

public:
	ScrollBarWidget(int id, BaseWidget* parent, bool horiz, float x, float y, float w, float h,EAutoSizeType sizerType = BaseWidget::eAutoSizeNone);
	virtual ~ScrollBarWidget();
	virtual void ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam);

	void SetPosition(int pos);
	int  GetPosition();
	void SetLimit(int page_sz, int max_ps);
};
