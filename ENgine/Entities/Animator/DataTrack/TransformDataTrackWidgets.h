#pragma once

#ifdef EDITOR

#include "Platforms/PC/GUI/Widgets.h"
#include "Common/Math.h"

class TransformDataTrack;

class TransformDataTrackWidgets
{
	float offset_x;
	float offset_y;

public:

	TransformDataTrackWidgets(TransformDataTrack* track, int x, int y);
	
	TransformDataTrack* track;

	LabelWidget*   prop_xlabel;
	TextBoxWidget* prop_xed;

	LabelWidget*   prop_ylabel;
	TextBoxWidget* prop_yed;

	LabelWidget*   prop_zlabel;
	TextBoxWidget* prop_zed;

	ButtonWidget*  btn_grab;

	void InitControls(BaseWidget* parent);
	void StartEditKey();
	void StartEditMultiKeys();
	bool UpdateWidgets();
	void StopEditKey();

	void Show(bool show);
				
	void UpdateVal(int type, TextBoxWidget* box);
	void GrabKey();
};

#endif