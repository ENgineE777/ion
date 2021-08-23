
#ifdef EDITOR

#include "EditorWindow.h"
#include "Core/Core.h"
#include "Editor/Editor.h"

EditorWindow::EditorWindow(int id, const char* txt, bool set_adjust, float set_x, float set_y, float w, float h) : WindowWidget(id, txt, false, set_adjust, set_x, set_y, w, h)
{

}

LRESULT EditorWindow::Proc( HWND hwnd, dword msg, WPARAM wParam, LPARAM lParam )
{
	switch (msg)
	{			
        case WM_CLOSE:core_ed.SaveDockedWindowsLocation();break;
		case WM_DESTROY:
			en_core.SetUseFakeExit(false);
			core->Exit();		
		return 0;
	}	

	return WindowWidget::Proc(hwnd, msg, wParam, lParam);
}

#endif