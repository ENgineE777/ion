
#include "BaseWidget.h"
#include "Core\ICore.h"
#include "DockHost\DockHost.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
						name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
						processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

BaseWidget::BaseWidget(int set_id, BaseWidget* prnt, const char* txt,EAutoSizeType sizerType):x(0),y(0),width(1),height(1),handle(0),visible(true),id(set_id)
    ,parent(prnt),text(txt),focused(NULL),enabled(true),mouse_over(NULL),tooltip_inited(false),eSizerType(sizerType)
{
    if(parent)
        parent->AddChild(this);
}

BaseWidget::~BaseWidget()
{
    DestroyWindow(handle);

    if (parent)
        parent->DelChild(this);

    for (int i = 0; i < childs.size(); i++)
    {
        childs[i]->Release();
        i--;
    }
}

HWND BaseWidget::GetHandle()
{
	return handle;
}

int BaseWidget::GetID()
{
	return id;
}

void BaseWidget::Show(bool set)
{
	visible = set;
	ShowWindow(handle, set);
}

bool BaseWidget::IsVisible()
{
	if (!visible) return false;

	if (parent)
	{
		if (!parent->IsVisible()) return false;

		return parent->IsVisible();
	}

	return visible;
}

void BaseWidget::Enable(bool set)
{
	enabled = set;

	EnableWindow(handle, enabled);
}

bool BaseWidget::IsEnabled()
{
	return enabled;
}

void BaseWidget::SetPos(float set_x, float set_y)
{
	x = set_x;
	y = set_y;

	SetWindowPos(handle, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

float BaseWidget::GetX()
{
	return x;
}

float BaseWidget::GetY()
{
	return y;
}

float BaseWidget::GetXOffset()
{
	return 0.0f;
}

float BaseWidget::GetYOffset()
{
	return 0.0f;
}

void BaseWidget::SetSize(float w, float h)
{
	width = w;
	height = h;
	if (eSizerType != eAutoSizeNone && parent)
	{
        switch(eSizerType)
        {
            case eAutoSizeBoth:
            {
                width = parent->GetWidth() - x + GetXOffset() ;
                height = parent->GetHeight() - y + GetYOffset();
                break;
            }
            case eAutoSizeWidth:width = parent->GetWidth() - x + GetXOffset();break;
            case eAutoSizeHeigth:height = parent->GetHeight() - y + GetYOffset();break;
        }
	}
	SetWindowPos(handle, 0, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
}

float BaseWidget::GetWidth()
{
	return width;
}

float BaseWidget::GetHeight()
{
	return height;
}

void BaseWidget::SetText(const char* txt)
{
	text = txt;
	CreateTooltip(text);
}

const char* BaseWidget::GetText()
{
	return text.c_str();
}

void BaseWidget::AddChild(BaseWidget* child)
{
	childs.push_back(child);
}

void BaseWidget::DelChild(BaseWidget* child)
{
	for (unsigned i = 0; i < childs.size(); i++)
	{
		if (childs[i] == child)
		{
            childs.erase(childs.begin() + i);
			break;
		}
	}
}

BaseWidget* BaseWidget::GetParent()
{
	return parent;
}

int	BaseWidget::GetChildsCount()
{
	return childs.size();
}

BaseWidget* BaseWidget::GetChild(int index)
{
	return childs[index];
}

void BaseWidget::Release()
{
	DestroyWindow(handle);

	if (parent)
	{
		parent->DelChild(this);
	}

	for (int i = 0; i < childs.size(); i++)
	{
		childs[i]->Release();
		i--;
	}
	
	delete this;
}

void BaseWidget::NotifyMouseOver(BaseWidget* widget)
{
	if (parent)
	{
		parent->NotifyMouseOver(widget);
	}

	if (mouse_over != widget)
	{
		if (mouse_over) mouse_over->OnMouseLeave();
		mouse_over = widget;
	}
}

void BaseWidget::OnMouseMove(int mx, int my)
{
}

void BaseWidget::OnLeftMouseDown(int mx, int my)
{
	for (int i = childs.size() - 1; i >= 0; i--)
	{
		int new_mx = mx - childs[i]->GetX();
		int new_my = my - childs[i]->GetY();

		if (new_mx>0 && new_mx<childs[i]->GetWidth() &&
			new_my>0 && new_my<childs[i]->GetHeight())
		{
			childs[i]->OnLeftMouseDown(new_mx, new_my);
			return;
		}
	}	
}

void BaseWidget::OnLeftMouseUp(int mx, int my)
{
	for (int i = childs.size() - 1; i >= 0; i--)
	{
		int new_mx = mx - childs[i]->GetX();
		int new_my = my - childs[i]->GetY();

		if (new_mx>0 && new_mx<childs[i]->GetWidth() &&
			new_my>0 && new_my<childs[i]->GetHeight())
		{
			childs[i]->OnLeftMouseUp(new_mx, new_my);

			return;
		}
	}	
}

void BaseWidget::OnLeftDoubliClick(int mx, int my)
{

}


void BaseWidget::OnRightMouseDown(int mx, int my)
{

}

void BaseWidget::OnRightMouseUp(int mx, int my)
{

}

void BaseWidget::OnMiddleMouseDown(int mx, int my)
{

}

void BaseWidget::OnMiddleMouseUp(int mx, int my)
{

}

void BaseWidget::OnMouseLeave()
{
}

void BaseWidget::ProcessChildWidget(dword msg, WPARAM wParam, LPARAM lParam)
{
	ProcessWidget(msg, wParam, lParam);
	for (int i = 0; i < childs.size(); i++)
		childs[i]->ProcessChildWidget(msg, wParam, lParam);
}

void BaseWidget::ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam)
{
}

void BaseWidget::Draw()
{
}

void BaseWidget::Redraw()
{
	HWND h = handle;

	if (parent)
	{
		h = parent->handle;
	}

	RECT rc;
	rc.left = x;
	rc.top = y;

	rc.right = x+width;
	rc.bottom = y+height;

	InvalidateRect(h, &rc, false );
}

void BaseWidget::Update()
{

}

void BaseWidget::Resize()
{
	SetSize(width, height);
	for (int i = 0; i < childs.size(); i++)
		childs[i]->Resize();			
}

void BaseWidget::SetFocused()
{
	SetFocus(handle);
}

bool BaseWidget::IsFocused()
{
	return (GetFocus() == handle);
}

void BaseWidget::CreateTooltip(const string& sText)
{
	if (tooltip_inited)
	{
		ti.lpszText = (char*)sText.c_str();    
		SendMessage(tooltip, TTM_SETTOOLINFO , 0, (LPARAM) (LPTOOLINFO) &ti);		
		return;
	}

	RECT rect;
    

    tooltip = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,        
							 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,  handle,
							 NULL, GetModuleHandle(NULL), NULL);
 
    SetWindowPos(tooltip, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
     
    GetClientRect (handle, &rect);
     
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = handle;
    ti.hinst = GetModuleHandle(NULL);
    ti.uId = 0;
    ti.lpszText = (char*)sText.c_str();    
    ti.rect.left = rect.left;    
    ti.rect.top = rect.top;
    ti.rect.right = rect.right;
    ti.rect.bottom = rect.bottom;
     
	SendMessage(tooltip, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
	tooltip_inited = true;
}

bool BaseWidget::IsHoveredByMouse()
{
	if (!IsVisible()) return false;

	POINT point;
		
	if (GetCursorPos(&point))
	{
		RECT windowRect;
     
		GetWindowRect( handle, &windowRect );
      				
		return PtInRect( &windowRect, point );
	}

	return false;
}

void BaseWidget::MakeSubClassing()
{
	SetWindowSubclass(handle, &WidgetProc, 0, (DWORD_PTR)this);
}

bool BaseWidget::IsEraseBkgnd()
{
	return false;
}


bool BaseWidget::CatchEraseBkgnd()
{
    return true;
}


LRESULT CALLBACK BaseWidget::WidgetProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	BaseWidget* widget = (BaseWidget*)dwRefData;
	if (uMsg == WM_ERASEBKGND && widget->CatchEraseBkgnd())
	{			
		return widget->IsEraseBkgnd();
	}			
    WidgetBaseProc(uMsg,wParam,lParam,widget);

	widget->ProcessWidget( uMsg, wParam, lParam );

    return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
	
}

void BaseWidget::WidgetBaseProc(UINT uMsg, WPARAM wParam, LPARAM lParam,BaseWidget* widget)
{
    if (uMsg == WM_MOUSEMOVE)
        widget->NotifyMouseOver(widget);

    if (uMsg == WM_PAINT || uMsg == WM_DRAWITEM)
        widget->Draw();					

    if (uMsg == WM_LBUTTONUP)
        widget->OnLeftMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));


    if (uMsg == WM_VSCROLL || uMsg == WM_HSCROLL || uMsg == WM_NOTIFY)
    {
        for (int j = 0; j < widget->GetChildsCount(); j++)
            widget->GetChild(j)->ProcessWidget(uMsg, wParam, lParam);
    }
    if(uMsg == WM_COMMAND)
    {
        for (int j = 0; j < widget->GetChildsCount(); j++)
        {
            BaseWidget* pChild = widget->GetChild(j);
            if(pChild->IsVisible() && (HWND)lParam == pChild->GetHandle())
                pChild->ProcessWidget(uMsg, wParam, lParam);
        }
    }
}
