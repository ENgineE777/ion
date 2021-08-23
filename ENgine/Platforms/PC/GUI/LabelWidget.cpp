
#include "LabelWidget.h"

LabelWidget::LabelWidget(int set_id, BaseWidget* prnt, const char* txt, bool box, float set_x, float set_y, float w, float h,EAutoSizeType sizerType,int align) : BaseWidget(set_id, prnt, txt, sizerType)
{
	color_box = box;

	x = set_x;
	y = set_y;
	width = w;
	height = h;

    dword flag = 0;
    if(color_box)
        flag = SS_OWNERDRAW;
	handle = CreateWindow("STATIC","Static",SS_LEFT|WS_CHILD | WS_VISIBLE | flag | align |SS_NOTIFY,x,y,width,height,parent->GetHandle(),NULL, NULL, NULL);
			
	SetWindowLong(handle, GWL_ID, id);

	Static_SetText(handle, text.c_str());

	color = COLOR_WHITE;

	MakeSubClassing();

	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(handle, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
}

LabelWidget::~LabelWidget()
{
}

void LabelWidget::ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam)
{

}

void LabelWidget::SetText(const char* txt)
{
	BaseWidget::SetText(txt);
	Static_SetText(handle, text.c_str());
}

void LabelWidget::Draw()
{
	if (color_box)
    {
        HDC hdc = GetDC(handle);
        HBRUSH hbrBkgnd = CreateSolidBrush(RGB(color.r * 255,color.g * 255,color.b * 255));
        SelectObject(hdc, hbrBkgnd);
        Rectangle ( hdc, 0, 0, width, 18 );
        TextOut(hdc, 1, 1, text.c_str(), text.size());
        DeleteObject(hbrBkgnd);
        ReleaseDC(handle, hdc);
    }
}

void LabelWidget::OnLeftMouseUp(int mx, int my)
{
	EventsWidget::SetEvent(GetID(), EventsWidget::label_click);
}
		