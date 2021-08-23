#pragma once

#ifdef EDITOR

#include "Platforms/PC/GUI/Widgets.h"

class Animator;

class AnimatorWindow
{		
	WindowWidget* wnd;
	int			  num_buttons;
	TimeLineWidget* timeline;

	ComboBoxWidget* cb_addObject;
	ButtonWidget* btn_delObject;

	ButtonWidget* btn_addKey;
	ButtonWidget* btn_delKey;
	ButtonWidget* btn_blendKey;
	ButtonWidget* btn_playKey;

	LabelWidget*   lb_curTime;
	TextBoxWidget* tb_curTime;

	LabelWidget*   lb_timeLenght;
	TextBoxWidget* tb_timeLenght;

	LabelWidget*   lb_toTime;
	TextBoxWidget* tb_toTime;

	LabelWidget*   lb_fromTime;
	TextBoxWidget* tb_fromTime;

	ScrollBarWidget* horz_scroll;
	ScrollBarWidget* vert_scroll;

	LabelWidget*   prop_dock;

	float cur_time;
	float time_offset;

	Animator* owner;

public:

	static AnimatorWindow* instance;
	
	void Init();
	void Show(bool sh);

	void CreateAnimatorWidgets(Animator* anim);
	void Clear();

	void Update();
	void UpdateWidgets();

	void SetGizmo(Gizmo* gizmo);
};

#endif