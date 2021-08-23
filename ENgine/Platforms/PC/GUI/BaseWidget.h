
#pragma once

#define NOMINMAX
#include "windows.h"
#include "windowsx.h"
#include "Common/Common.h"
#include "Common/Color.h"
#include "EventsWidget.h"
#include "CommCtrl.h"

class DEF_EXPORT BaseWidget
{
public:
    enum EAutoSizeType{eAutoSizeNone,eAutoSizeBoth,eAutoSizeWidth,eAutoSizeHeigth};
protected:
	HWND  handle;
	HWND  tooltip;
	TOOLINFO ti;

	float x,y;
	float width,height;
	bool  visible;
	bool  enabled;

	int id;
	string text;
	
	BaseWidget* parent;
	std::vector<BaseWidget*> childs;

	BaseWidget* focused;
	BaseWidget* mouse_over;

	bool tooltip_inited;

    EAutoSizeType eSizerType;
  


public:
	BaseWidget(int id, BaseWidget* prnt, const char* txt,EAutoSizeType sizerType = eAutoSizeNone);
	virtual ~BaseWidget();

	virtual HWND GetHandle();
	virtual int GetID();

	virtual void Show(bool set);
	virtual bool IsVisible();
    inline  bool GetVisibleState(){return visible;}

	virtual void Enable(bool set);
	virtual bool IsEnabled();

	virtual void SetPos(float x, float y);
	virtual float GetX();
	virtual float GetY();

	virtual float GetXOffset();
	virtual float GetYOffset();

	virtual void SetSize(float w, float h);
	virtual float GetWidth();
	virtual float GetHeight();

	virtual void SetText(const char* txt);
	virtual const char* GetText();

	void AddChild(BaseWidget* child);
	void DelChild(BaseWidget* child);

	BaseWidget* GetParent();
	int		    GetChildsCount();
	BaseWidget* GetChild(int index);

	virtual void Release();

	virtual void NotifyMouseOver(BaseWidget* widget);

	virtual void OnMouseMove(int mx, int my);

	virtual void OnLeftMouseDown(int mx, int my);
	virtual void OnLeftMouseUp(int mx, int my);
	virtual void OnLeftDoubliClick(int mx, int my);	

	virtual void OnRightMouseDown(int mx, int my);	
	virtual void OnRightMouseUp(int mx, int my);

	virtual void OnMiddleMouseDown(int mx, int my);	
	virtual void OnMiddleMouseUp(int mx, int my);

	virtual void OnMouseLeave();

	virtual void ProcessChildWidget(dword msg, WPARAM wParam, LPARAM lParam);
	virtual void ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam);
	virtual void Draw();
	virtual void Redraw();

	virtual void Update();

	virtual void Resize();
	
	virtual void SetFocused();
	virtual bool IsFocused();

	void CreateTooltip(const string& sText);	

	bool IsHoveredByMouse();

	void MakeSubClassing();
	virtual bool IsEraseBkgnd();
    virtual bool CatchEraseBkgnd();

	static LRESULT CALLBACK WidgetProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
    static void WidgetBaseProc(UINT uMsg, WPARAM wParam, LPARAM lParam,BaseWidget* widget);
};
