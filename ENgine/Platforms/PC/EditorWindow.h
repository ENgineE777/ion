
#pragma once

#ifdef EDITOR

#include "Platforms/pc/gui/Widgets.h"

class DEF_EXPORT EditorWindow : public WindowWidget
{
public:	
	EditorWindow(int id, const char* text, bool adjust, float x, float y, float w, float h);
	virtual LRESULT Proc( HWND hwnd, dword msg, WPARAM wParam, LPARAM lParam );
};

#endif