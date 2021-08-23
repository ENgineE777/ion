
#pragma once

#ifdef EDITOR

#include "Platforms/pc/gui/Widgets.h"
#include "Editor\Helpers\Gizmo.h"
#include "Editor/Editor.h"

class DEF_EXPORT EditorViewPort : public BaseWidget
{
	bool move_gizmo;
	int  last_mx, last_my;
	bool rb_pressed;
	bool md_pressed;
	Gizmo* gizmo;
	Matrix* view;
	bool need_reset_render;
	bool needShowActiveText;
    bool bDockResize;

public:	

	//Hack
	Editor* core_ed; 

	EditorViewPort(int id, BaseWidget* parent, const char* txt, float x, float y, float w, float h);
	virtual ~EditorViewPort();
	
	virtual void Reset();

	virtual void OnLeftMouseDown(int mx, int my);	
	virtual void OnLeftMouseUp(int mx, int my);

	virtual void OnMouseMove(int mx, int my);

	virtual void OnRightMouseDown(int mx, int my);	
	virtual void OnRightMouseUp(int mx, int my);

	virtual void OnMiddleMouseDown(int mx, int my);	
	virtual void OnMiddleMouseUp(int mx, int my);

    virtual void ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam);

	void SetGizmo(Gizmo* gizmo);
	void SetView(Matrix* v);

	void SetNeedShowActiveText(bool set);	

	virtual void Resize();
	virtual void Update();
};

#endif