
#include "WindowWidget.h"
#include "CommCtrl.h"
#include "Core/ICore.h"

bool WindowWidget::wndClassReg = false;

std::vector<WindowWidget*> wnds;

#include "Uxtheme.h"

LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	for (int i=0;i<wnds.size();i++)
	{
		if (wnds[i]->GetHandle() == hwnd)
		{
			return wnds[i]->Proc( hwnd, msg, wParam, lParam );
		}
	}

	return DefWindowProc( hwnd, msg, wParam, lParam );
}

WindowWidget::WindowWidget(int set_id, const char* txt, bool popup, bool adjust, float set_x, float set_y, float w, float h) : BaseWidget(set_id, null, txt, BaseWidget::eAutoSizeBoth),bSetSize(false)
{
	wnds.push_back(this);

	need_strict_size = false;

	active = false;

	x = set_x;
	y = set_y;
	width = w;
	height = h;

	visible = false;

#ifdef WINAPI_UTF16
	const wchar_t* class_name = L"ENgineWindow";
#else
	const char* class_name = "ENgineWindow";
#endif

	if (!wndClassReg)
	{
		WNDCLASS	winClass;
	
		winClass.cbClsExtra = 0;
		winClass.cbWndExtra = 0;
		winClass.hbrBackground = ( HBRUSH )GetStockObject( GRAY_BRUSH );
		winClass.hCursor = LoadCursor( NULL, IDC_ARROW );
		winClass.hIcon = 0;
		winClass.hInstance = NULL;	
		winClass.lpfnWndProc = WndProc;

		winClass.lpszClassName = "ENgineWindow";
		winClass.lpszMenuName = NULL;
		winClass.style = CS_CLASSDC | CS_DBLCLKS;

		if ( !RegisterClass( &winClass ) )
		{
			//core->Trace( "\"ENgineWindow\" RegisterClass() failed");
		}
		
		wndClassReg = true;
	}
	
	flag = 0;

	if (!popup)
	{
		flag = WS_OVERLAPPEDWINDOW | WS_MAXIMIZE;
	}
	else
	{
		flag = WS_POPUP | WS_BORDER | WS_CAPTION;
	}

	RECT rect;

	rect.left = x;
	rect.top = y;

	rect.right = x + width;
	rect.bottom = y + height;

	if (adjust)
	{		
		AdjustWindowRectEx(&rect, flag, false, null);
	}

	hack_wnd_width = rect.right - rect.left;
	hack_wnd_height = rect.bottom - rect.top;	

	handle = CreateWindow( "ENgineWindow", text.c_str(), flag,// | WS_VSCROLL,
						   rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, NULL, NULL );
	SetWindowLong(handle, GWL_ID, id);

	if (!adjust)
	{
		RECT rect;
		
		GetClientRect(handle, &rect);
		width = rect.right;
		height = rect.bottom;
	}

	is_minimazed = false;
}

WindowWidget::~WindowWidget()
{
	for (int i=0;i<wnds.size();i++)
	{
		if (wnds[i] == this)
		{
			wnds.erase(wnds.begin() + i);
			break;
		}
	}
}

void WindowWidget::SetText(const char* txt)
{
	SetWindowText(handle, txt);
	BaseWidget::SetText(txt);
}

LRESULT WindowWidget::Proc( HWND hwnd, dword msg, WPARAM wParam, LPARAM lParam )
{	
	switch( msg )
	{	
		case WM_SYSCOMMAND:
			{
				if ((wParam & 0xFFF0) == SC_MINIMIZE)
				{					
					is_minimazed = true;
				}

				if ((wParam & 0xFFF0) == SC_RESTORE)
				{					
					is_minimazed = false;
				}				
			}
			break;

		case WM_MOUSEMOVE:
			{
				NotifyMouseOver(this);			

				int xPos = GET_X_LPARAM(lParam); 
				int yPos = GET_Y_LPARAM(lParam); 

				for (int i=childs.size()-1;i>=0;i--)
				{
					childs[i]->OnMouseMove(xPos - childs[i]->GetX(),
										   yPos - childs[i]->GetY());
				}
			};
			break;

		case WM_LBUTTONDOWN:
			{
				int xPos = GET_X_LPARAM(lParam); 
				int yPos = GET_Y_LPARAM(lParam); 
	
				OnLeftMouseDown(xPos, yPos);
			};
			break;		

		case WM_LBUTTONUP:
			{
				int xPos = GET_X_LPARAM(lParam); 
				int yPos = GET_Y_LPARAM(lParam); 

				OnLeftMouseUp(xPos, yPos);				
			};
			break;
		
		case WM_LBUTTONDBLCLK:
			{
				int xPos = GET_X_LPARAM(lParam); 
				int yPos = GET_Y_LPARAM(lParam); 

				for (int i=childs.size()-1;i>=0;i--)
				{
					childs[i]->OnLeftDoubliClick(xPos - childs[i]->GetX(),
												 yPos - childs[i]->GetY());
				}
			};
			break;			

		case WM_RBUTTONDOWN:
			{
				int xPos = GET_X_LPARAM(lParam); 
				int yPos = GET_Y_LPARAM(lParam); 

				for (int i=childs.size()-1;i>=0;i--)
				{
					childs[i]->OnRightMouseDown(xPos - childs[i]->GetX(),
												yPos - childs[i]->GetY());
				}
			};
			break;

		case WM_MBUTTONDOWN:
			{
				int xPos = GET_X_LPARAM(lParam); 
				int yPos = GET_Y_LPARAM(lParam); 

				for (int i=childs.size()-1;i>=0;i--)
				{
					childs[i]->OnMiddleMouseDown(xPos - childs[i]->GetX(),
												 yPos - childs[i]->GetY());
				}
			};
			break;

		case WM_RBUTTONUP:
			{
				int xPos = GET_X_LPARAM(lParam); 
				int yPos = GET_Y_LPARAM(lParam); 

				for (int i=childs.size()-1;i>=0;i--)
				{
					childs[i]->OnRightMouseUp(xPos - childs[i]->GetX(),
											  yPos - childs[i]->GetY());
				}
			};
			break;

		case WM_MBUTTONUP:
			{
				int xPos = GET_X_LPARAM(lParam); 
				int yPos = GET_Y_LPARAM(lParam); 

				for (int i=childs.size()-1;i>=0;i--)
				{
					childs[i]->OnMiddleMouseUp(xPos - childs[i]->GetX(),
											   yPos - childs[i]->GetY());
				}
			};
			break;		
		case WM_SIZE:
			{					
                if(wParam != 1)
				    Resize();
			};
			break;

		case WM_HSCROLL:
		case WM_VSCROLL:
		case WM_COMMAND:
		case WM_NOTIFY:
		{
			if (id == LOWORD(wParam))
			{
			}
			else
			{
				for (int i=0;i<childs.size();i++)
				{
					childs[i]->ProcessWidget(msg, wParam, lParam);
				}
			}
			//if ( !lParam ) OnMenuItem( wParam );
		}
		break;

		case WM_DRAWITEM:
		case WM_PAINT:
		{
			for (int i=0;i<childs.size();i++)
			{
				childs[i]->Draw();
			}
		}
		break;
		case WM_CLOSE:
			EventsWidget::SetEvent(id, EventsWidget::window_close);
		break;		
        case WM_SHOWWINDOW:
            EventsWidget::SetEvent(id, EventsWidget::window_show);
        break;
		case WM_ACTIVATE:			
			active = (LOWORD(wParam) > 0);			
		break;
		case WM_GETMINMAXINFO:
		{			
			if (need_strict_size)
			{
				MINMAXINFO* pmmi = (MINMAXINFO*)lParam;
			
				pmmi->ptMaxSize.x = hack_wnd_width;
				pmmi->ptMaxSize.y = hack_wnd_height;

				pmmi->ptMinTrackSize.x = hack_wnd_width;
				pmmi->ptMinTrackSize.y = hack_wnd_height;

				pmmi->ptMaxTrackSize.x = hack_wnd_width;
				pmmi->ptMaxTrackSize.y = hack_wnd_height;
			}

			return DefWindowProc(hwnd, msg, wParam, lParam);

			return 0;
		}
	}
	return DefWindowProc( hwnd, msg, wParam, lParam );
}

void WindowWidget::Resize()
{
    if(!bSetSize)
    {
	    RECT rect;
	    GetClientRect(handle, &rect);
	    width = rect.right;
	    height = rect.bottom;
    }
    bSetSize = false;
    for (int i=0;i<childs.size();i++)
	{
		childs[i]->Resize();			
	}	
}

void WindowWidget::SetSize(float w, float h)
{
    bSetSize = true;
    BaseWidget::SetSize(w,h);
}

bool WindowWidget::IsActive()
{
	return active;
}

void WindowWidget::Maximaze()
{	
	if (IsVisible())
	{						
		ShowWindow(handle, SW_MAXIMIZE);
	}
}

bool WindowWidget::IsMinimazed()
{
	return is_minimazed;
}

void WindowWidget::SetNeedStrictSize(bool set_strict)
{
	need_strict_size = set_strict;
}