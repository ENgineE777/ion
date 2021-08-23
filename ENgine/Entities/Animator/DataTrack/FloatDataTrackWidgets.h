#pragma once

#ifdef EDITOR

#include "Platforms/PC/GUI/Widgets.h"

class FloatDataTrackWidgets
{
	float offset_x;
	float offset_y;

public:

	FloatDataTrackWidgets(int x, int y);
	float* value;

	LabelWidget*  prop_label;
	TextBoxWidget* prop_ed;

	void InitControls(BaseWidget* parent);
	void StartEditKey(float* vl, const char* name);
	void UpdateWidgets();
	void StopEditKey();
};

#endif