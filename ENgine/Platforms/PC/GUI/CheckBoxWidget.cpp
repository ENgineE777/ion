
#include "CheckBoxWidget.h"

CheckBoxWidget::CheckBoxWidget(int set_id, BaseWidget* prnt, const char* txt, float set_x, float set_y, float w, float h,EAutoSizeType sizerType) : BaseWidget(set_id, prnt, txt, sizerType)
{	
	x = set_x;
	y = set_y;
	width = w;
	height = h;

	dword flag = 0;		

	handle = CreateWindow("Button", txt, BS_CHECKBOX | WS_CHILD | WS_VISIBLE, x, y, width, height, parent->GetHandle(), NULL, NULL, NULL);
			
	SetWindowLong(handle, GWL_ID, id);

	Static_SetText(handle, text.c_str());

	MakeSubClassing();

	checked = false;

	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(handle, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
}

CheckBoxWidget::~CheckBoxWidget()
{
}

void CheckBoxWidget::ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam)
{
	if (WM_COMMAND == msg)
	{
		if (id == LOWORD(wParam) && HIWORD(wParam)==BN_CLICKED)
		{
            checked = !checked;
            Button_SetCheck(handle, checked);
            EventsWidget::SetEvent(id, EventsWidget::checkbox_click);
		}
	}
}

void CheckBoxWidget::SetText(const char* txt)
{
	BaseWidget::SetText(txt);
	Button_SetText(handle, text.c_str());
}

void CheckBoxWidget::SetChecked(bool set)
{
	checked = set;
	Button_SetCheck(handle, checked);
}

bool CheckBoxWidget::IsChecked()
{
	return checked;
}