
#include "TextBoxWidget.h"
#include "Core/ICore.h"

TextBoxWidget::TextBoxWidget(int set_id, BaseWidget* prnt, const char* txt, float set_x, float set_y, float w, float h,EAutoSizeType sizerType,EType type):BaseWidget(set_id, prnt, txt,sizerType)
{
	x = set_x;
	y = set_y;
	width = w;
	height = h;

	std::wstring wtext;
	String::Utf8toUtf16(wtext, txt);
    eType = type;

	handle = CreateWindowW( L"EDIT", wtext.c_str(), ES_AUTOHSCROLL|WS_CHILD|WS_BORDER | WS_VISIBLE | WS_TABSTOP ,x, y, width, height, parent->GetHandle(), NULL, NULL, NULL );
	SetWindowLong(handle, GWL_ID, id);


   SetWindowSubclass(handle, &TextBoxProc, 0, (DWORD_PTR)this);

	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(handle, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
}

TextBoxWidget::~TextBoxWidget()
{
}

void TextBoxWidget::ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam)
{	
    int code = LOWORD(wParam);
    if (msg == WM_KEYDOWN  )
    {
        if(code == 13)
            EventsWidget::SetEvent(id, EventsWidget::textbox_enter_press);		
        else
            if(code<37 || code>40)
            {
                EventsWidget::SetEvent(id, EventsWidget::textbox_change);		
            }
    }

}

void TextBoxWidget::SetText(const char* txt)
{
    int pos =  LOWORD(Edit_GetSel(handle));
    bool bSetToEnd = pos == text.length();
	BaseWidget::SetText(txt);
	std::wstring wtext;
	String::Utf8toUtf16(wtext, txt);
	SetWindowTextW(handle, wtext.c_str());
    if(bSetToEnd)
        pos = text.length();
    Edit_SetSel(handle,pos,pos);
}

const char* TextBoxWidget::GetText()
{
	wchar_t wtmp[4096];
	GetWindowTextW(handle, wtmp, 4096);
	
	String::Utf16toUtf8(text, wtmp);

	return BaseWidget::GetText();
}

void TextBoxWidget::SelectText()
{
    Edit_SetSel(handle,0,Edit_GetTextLength(handle));
}

LRESULT CALLBACK TextBoxWidget::TextBoxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    TextBoxWidget* widget = (TextBoxWidget*)dwRefData;

    if (uMsg == WM_ERASEBKGND && widget->CatchEraseBkgnd())return widget->IsEraseBkgnd();
        
    WidgetBaseProc(uMsg,wParam,lParam,widget);

    if(uMsg == WM_CHAR)
    {
        if(wParam == VK_RETURN)
            EventsWidget::SetEvent(widget->GetID(), EventsWidget::textbox_enter_press);
        if(wParam != VK_RETURN && wParam != VK_DELETE && wParam != VK_BACK)
        {
            switch(widget->Type())
            {
            case eUFloat:
                {
                    if((wParam < '0' || wParam > '9') && wParam != '.' )return 0;
                    break;
                }
            case eFloat:
                {
                    if((wParam < '0' || wParam > '9') && wParam != '-' && wParam != '.')return 0;
                    break;
                }
            case eInt:
                {
                    if((wParam < '0' || wParam > '9') && wParam != '-') return 0;
                    break;
                }
            case eUnsigned:
                {
                    if(wParam < '0' || wParam > '9') return 0;
                    break;
                }
            }
        }
        EventsWidget::SetEvent(widget->GetID(),EventsWidget::textbox_change);	
    }
    return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
