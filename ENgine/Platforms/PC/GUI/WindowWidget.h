
#pragma once

#include "BaseWidget.h"

class DEF_EXPORT WindowWidget : public BaseWidget
{
	static bool wndClassReg;	
	bool active;
	int flag;

	int hack_wnd_width;
	int hack_wnd_height;

	bool need_strict_size;
	bool is_minimazed;
    bool bSetSize;

public:
	WindowWidget(int id, const char* text, bool popup, bool adjust, float x, float y, float w, float h);
	virtual ~WindowWidget();
	virtual LRESULT Proc( HWND hwnd, dword msg, WPARAM wParam, LPARAM lParam );
	virtual void Resize();
    virtual void SetSize(float w, float h);

	virtual bool IsActive();

	virtual void SetText(const char* txt);

	virtual void Maximaze();
	virtual bool IsMinimazed();

	void SetNeedStrictSize(bool set_strict);
};
