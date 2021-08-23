#pragma once

#ifdef EDITOR

#include "Platforms/PC/GUI/Widgets.h"
#include "ImageWidget.h"

class SkinWindow
{			
	int width, height;	
	
	ComboBoxWidget* cb_type;
	ComboBoxWidget* texture_mode;
	ComboBoxWidget* texture_filter;
	
	ButtonWidget*   load_image;	
	LabelWidget*    image_size_label;
	CheckBoxWidget* slice_scale_type;
	ComboBoxWidget* image_scale_type;
	LabelWidget*    align_label;
	ComboBoxWidget* horz_align_type;	
	ComboBoxWidget* vert_align_type;	

	TextBoxWidget*  prop_ed_x;
	TextBoxWidget*  prop_ed_y;
	TextBoxWidget*  prop_ed_w;
	TextBoxWidget*  prop_ed_h;

public:

	WindowWidget* wnd;
	ImageWidget* img_wgt;
	int          scale_type;
	int			 align_type;

	static SkinWindow* instance;
	
	static void PrepareForUse();

	void Init();
	void Show(bool sh);
	void SetImage(const char* img);
	void ShowSubWidgets();
	void SetupWidgets();

	void Update();	
};

#endif