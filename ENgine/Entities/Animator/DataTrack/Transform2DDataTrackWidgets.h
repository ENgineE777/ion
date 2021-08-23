#pragma once

#ifdef EDITOR

#include "Platforms/PC/GUI/Widgets.h"
#include "Common/Math.h"

class Transform2DDataTrack;

class Transform2DDataTrackWidgets
{
	float offset_x;
	float offset_y;

public:

	Transform2DDataTrackWidgets(Transform2DDataTrack* track, int x, int y);
	
	Transform2DDataTrack* track;

	LabelWidget*   prop_xlabel;
	TextBoxWidget* prop_xed;

	LabelWidget*   prop_ylabel;
	TextBoxWidget* prop_yed;

	LabelWidget*   prop_sz_xlabel;
	TextBoxWidget* prop_sz_xed;

	LabelWidget*   prop_sz_ylabel;
	TextBoxWidget* prop_sz_yed;

	void InitControls(BaseWidget* parent);
	void StartEditKey();	
	bool UpdateWidgets();
	void StopEditKey();

	void Show(bool show);
				
	void UpdateVal(int type, TextBoxWidget* box);	
};

#endif