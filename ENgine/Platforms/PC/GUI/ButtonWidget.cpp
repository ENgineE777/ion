

#include "ButtonWidget.h"
#include "Core/Core.h"

#include "Platforms/pc/gui/UIBlue.h"

#include <commctrl.h>

extern CPaintManagerUI paintManagerUI;

int index = 0;

ButtonWidget::ButtonWidget(int set_id, BaseWidget* prnt, bool set_use_image, const char* txt, float set_x, float set_y, float w, float h,EAutoSizeType sizerType) : BaseWidget(set_id, prnt, txt,sizerType)
    ,nFlag(0),bEnable(true),use_image(set_use_image),is_pushed(false),is_pushable(false),is_howered(false),image(0),push_image(0),howered_image(0),howered_push_image(0)
{
	x = set_x;
	y = set_y;
	width = w;
	height = h;
	
	handle = CreateWindow("STATIC", "Static", SS_LEFT | WS_CHILD | WS_VISIBLE | SS_OWNERDRAW | SS_NOTIFY, x, y, width, height, parent->GetHandle(), NULL, NULL, NULL);		

	SetWindowLong(handle, GWL_ID, id);

	MakeSubClassing();

	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(handle, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));		

	CreateTooltip(txt);

}

ButtonWidget::~ButtonWidget()
{
}

void ButtonWidget::ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam)
{	
	if (use_image)
	{
		if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK)
		{
			EventsWidget::SetEvent(id, EventsWidget::button_click);

			if (is_pushable)
			{
				SetPushed(!IsPushed());
			}
		}	
		return;
	}

    if (WM_COMMAND == msg)
    {
        if (id == LOWORD(wParam) && HIWORD(wParam)==BN_CLICKED)
        {
            EventsWidget::SetEvent(id, EventsWidget::button_click);
        }
    }
    else
        if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK)
            EventsWidget::SetEvent(id, EventsWidget::button_click);

}

void ButtonWidget::SetText(const char* txt)
{
	BaseWidget::SetText(txt);
	Button_SetText(handle, txt);
    InvalidateRect(handle, null, false);
}

void ButtonWidget::SetImage(const char* image_name)
{
	if (image) DeleteObject(image);

	image = (HBITMAP)LoadImage (0, image_name, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);	

	InvalidateRect(handle, null, false);
}

void ButtonWidget::SetPushedImage(const char* image_name)
{
	if (push_image) DeleteObject(push_image);

	push_image = (HBITMAP)LoadImage (0, image_name, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);	
}

void ButtonWidget::SetHoweredImage(const char* image_name)
{
	if (howered_image) DeleteObject(howered_image);

	howered_image = (HBITMAP)LoadImage (0, image_name, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);	
}

void ButtonWidget::SetHoweredPushedImage(const char* image_name)
{
	if (howered_push_image) DeleteObject(howered_push_image);

	howered_push_image = (HBITMAP)LoadImage (0, image_name, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);	
}

void ButtonWidget::Draw()
{
	if (!use_image)
	{
		// Draw button
		UINT uState = 0;
		if( IsFocused() ) uState |= UISTATE_FOCUSED;
		if( !IsEnabled() ) uState |= UISTATE_DISABLED;
		RECT rcPadding = { 0, 0, 0, 0 };
		RECT m_rcItem = { 0, 0, width, height };
	
		if (is_howered)
		{
			uState |= UISTATE_HOT;
		}		

		CBlueRenderEngineUI::DoPaintButton(GetDC(handle), &paintManagerUI, m_rcItem, CStdString(text.c_str()), rcPadding, uState, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		return;
	}	

	HDC hDC = GetDC(handle);
							
	HDC MemDCExercising = CreateCompatibleDC(hDC);            			
	
	if (is_howered && bEnable)
	{
		if (is_pushed)
		{
			SelectObject(MemDCExercising, howered_push_image);
		}
		else
		{
			SelectObject(MemDCExercising, howered_image);
		}		
	}
	else
	{
		if (is_pushed)
		{
			SelectObject(MemDCExercising, push_image);
		}
		else
		{
			SelectObject(MemDCExercising, image);
		}
	}
			
	BitBlt(hDC, 0, 0, width, height, MemDCExercising, 0, 0, SRCCOPY);
			
	DeleteDC(MemDCExercising);
}

void ButtonWidget::NotifyMouseOver(BaseWidget* widget)
{
	if (widget == this)
	{
		if (!is_howered)
		{			
			is_howered = true;
			InvalidateRect(handle, null, false);
		}
	}

	BaseWidget::NotifyMouseOver(widget);
}

void ButtonWidget::OnMouseLeave()
{
	if (is_howered)
	{
		is_howered = false;
		InvalidateRect(handle, null, false);
	}
}

bool ButtonWidget::IsPushable()
{
	return is_pushable;
}

void ButtonWidget::SetPushable(bool set)
{
	is_pushable = set;
	
	if (!is_pushable) is_pushed = false;

	InvalidateRect(handle, null, false);
}

bool ButtonWidget::IsPushed()
{
	return is_pushed;
}

void ButtonWidget::SetPushed(bool set)
{
	if (!is_pushable) return;

	is_pushed = set;

	InvalidateRect(handle, null, false);
}

void ButtonWidget::SetEnable(bool bSet)
{
    if(bEnable == bSet)
        return;
    bEnable = bSet;
    InvalidateRect(handle, null, false);
}

