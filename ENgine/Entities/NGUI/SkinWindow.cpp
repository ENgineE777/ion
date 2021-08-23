
#ifdef EDITOR

#include "SkinWindow.h"

SkinWindow* SkinWindow::instance = null;

void SkinWindow::PrepareForUse()
{
	if (!instance)
	{
		instance = new SkinWindow();
		instance->Init();
	}
}

void SkinWindow::Init()
{	
	wnd = new WindowWidget(7000,"Texture Editor", false, false, 10,35,940,640);
	
	ButtonWidget* btn_move = new ButtonWidget(7008, wnd, true, "zoom in", 5, 5, 20, 20);
	btn_move->SetPushable(false);
	btn_move->SetImage("Editor/tex_editor_zin.bmp");	

	ButtonWidget* btn_move2 = new ButtonWidget(7009, wnd, true, "zoom out", 30, 5, 20, 20);	
	btn_move2->SetImage("Editor/tex_editor_zout.bmp");	

	ButtonWidget* del_ed = new ButtonWidget(7010, wnd, false, "Actual Pixels", 60, 5, 70, 20);
	del_ed = new ButtonWidget(7011, wnd, false, "Fit", 135, 5, 60, 20);
	
	LabelWidget* prop_label = null;
	TextBoxWidget* prop_ed = null;	

	//SizerWidget* top_sizer = new SizerWidget(1000, wnd, true, 50);
	//SizerWidget* bottom_sizer = new SizerWidget(1000, wnd, true, -1);

	prop_label = new LabelWidget(1500, wnd, "Source", false, 5, 40, 90, 20);

	load_image = new ButtonWidget(7006, wnd, false, "", 95, 40, 80, 20);	
	
	del_ed = new ButtonWidget(7007, wnd, false, "X", 175, 40, 20, 20);

	prop_label = new LabelWidget(1500, wnd, "Tile Mode", false, 5, 70, 90, 20);	
	
	texture_mode = new ComboBoxWidget(7018, wnd, "Tile Mode", 95, 70, 100, 250, false);
	texture_mode->AddString("clamp");	
	texture_mode->AddString("wrap");
	texture_mode->AddString("mirror");	
	texture_mode->SetCurString(0);		
	
	prop_label = new LabelWidget(1500, wnd, "Filter", false, 5, 100, 90, 20);	

	texture_filter = new ComboBoxWidget(7019, wnd, "Filter", 95, 100, 100, 250, false);
	texture_filter->AddString("point");	
	texture_filter->AddString("linear");	
	texture_filter->SetCurString(0);				

	prop_label = new LabelWidget(1500, wnd, "Type", false, 5, 130, 90, 20);	

	cb_type = new ComboBoxWidget(7001, wnd, "Type", 95, 130, 100, 250, false);
	cb_type->AddString("Image");	
	cb_type->AddString("3 Horz slice");
	cb_type->AddString("3 Vert slice");
	cb_type->AddString("9 slice");
	cb_type->SetCurString(0);	
	

	prop_label = new LabelWidget(1500, wnd, "Scale", false, 5, 160, 90, 20);	
	
	image_scale_type = new ComboBoxWidget(7002, wnd, "type", 95, 160, 100, 250, false);	
	image_scale_type->AddString("Fit");	
	image_scale_type->AddString("Scale Horz");
	image_scale_type->AddString("Scale Vert");	
	image_scale_type->SetCurString(0);	

	slice_scale_type = new CheckBoxWidget(7005, wnd, "Pixel to pixel", 95, 190, 100, 20);		

	align_label = new LabelWidget(1500, wnd, "Align", false, 5, 210, 90, 20);	

	horz_align_type = new ComboBoxWidget(7003, wnd, "type", 95, 210, 100, 250, false);
	horz_align_type->AddString("Left");
	horz_align_type->AddString("Center");
	horz_align_type->AddString("Right");
	horz_align_type->SetCurString(0);

	vert_align_type = new ComboBoxWidget(7004, wnd, "type", 95, 210, 100, 250, false);
	vert_align_type->AddString("Top");
	vert_align_type->AddString("Center");
	vert_align_type->AddString("Bottom");
	vert_align_type->SetCurString(0);

	image_size_label = new LabelWidget(1500, wnd, "Image Size: 0 x 0", false, 5, 270, 195, 20);	

	/*prop_label = new LabelWidget(1500, wnd, "x", false, 5, 270, 90, 20, false);	
	prop_ed_x = new TextBoxWidget(1501, wnd, "", 95, 270, 105, 20, false);	

	prop_label = new LabelWidget(1500, wnd, "y", false, 5, 300, 90, 20, false);	
	prop_ed_y = new TextBoxWidget(1502, wnd, "", 95, 300, 105, 20, false);	

	prop_label = new LabelWidget(1500, wnd, "width", false, 5, 330, 90, 20, false);	
	prop_ed_w = new TextBoxWidget(1503, wnd, "", 95, 330, 105, 20, false);	

	prop_label = new LabelWidget(1500, wnd, "height", false, 5, 360, 90, 20, false);	
	prop_ed_h = new TextBoxWidget(1504, wnd, "", 95, 360, 105, 20, false);*/

	img_wgt = new ImageWidget(111, wnd, "move", 270, 5, 700, 550);

	scale_type = 0;
	align_type = 0;	

	ShowSubWidgets();
	
}

void SkinWindow::Show(bool sh)
{
	wnd->Show(sh);
}

void SkinWindow::SetImage(const char* img)
{
	load_image->SetText(img);
	img_wgt->SetImage(img);

	char str[128];
	String::Printf(str, 128, "Image size: %i x %i", img_wgt->img_width, img_wgt->img_height);
	image_size_label->SetText(str);
}

void SkinWindow::ShowSubWidgets()
{	
	align_label->Show(img_wgt->cur_type == 0 && img_wgt->scale_type != 0);
	vert_align_type->Show(img_wgt->cur_type == 0 && img_wgt->scale_type == 1);	
	horz_align_type->Show(img_wgt->cur_type == 0 && img_wgt->scale_type == 2);	

	image_scale_type->Show(img_wgt->cur_type != 3);
	slice_scale_type->Show(img_wgt->cur_type == 3);
}

void SkinWindow::SetupWidgets()
{
	ShowSubWidgets();

	cb_type->SetCurString(img_wgt->cur_type);
			
	image_scale_type->SetCurString(img_wgt->scale_type);
	
	texture_mode->SetCurString(img_wgt->texMode);	

	texture_filter->SetCurString(img_wgt->texFilter);	

	horz_align_type->SetCurString(img_wgt->horz_align);		
		
	vert_align_type->SetCurString(img_wgt->vert_align);	
	
	slice_scale_type->SetChecked(!img_wgt->slice_in_ref);			
}

void SkinWindow::Update()
{
	img_wgt->Update();

	if (EventsWidget::CheckEvent(7000, EventsWidget::window_close))
	{
		delete this;
		instance = null;
		
		return;
	}

	if (EventsWidget::CheckEvent(7001, EventsWidget::combobox_change))
	{
		img_wgt->cur_type = cb_type->GetCurString();
		img_wgt->Redraw();
		ShowSubWidgets();
	}

	if (EventsWidget::CheckEvent(7002, EventsWidget::combobox_change))
	{
		img_wgt->scale_type = image_scale_type->GetCurString();
		ShowSubWidgets();
	}
	
	if (EventsWidget::CheckEvent(7003, EventsWidget::combobox_change))
	{
		img_wgt->horz_align = horz_align_type->GetCurString();		
		ShowSubWidgets();
	}

	if (EventsWidget::CheckEvent(7004, EventsWidget::combobox_change))
	{
		img_wgt->vert_align = vert_align_type->GetCurString();	
		ShowSubWidgets();
	}

	if (EventsWidget::CheckEvent(7005, EventsWidget::checkbox_click))
	{
		img_wgt->slice_in_ref = !img_wgt->slice_in_ref;	
		ShowSubWidgets();
	}	

	if (EventsWidget::CheckEvent(7006, EventsWidget::button_click))
	{
		char name[512];
		name[0] = 0;

		if (OpenDialog(name, true))
		{			
			SetImage(name);
			img_wgt->Redraw();
		}
	}

	if (EventsWidget::CheckEvent(7007, EventsWidget::button_click))
	{		
		SetImage("");
		img_wgt->Redraw();
	}

	if (EventsWidget::CheckEvent(7008, EventsWidget::button_click))
	{
		img_wgt->pixel_density += 0.5f;
		img_wgt->Redraw();
	}

	if (EventsWidget::CheckEvent(7009, EventsWidget::button_click))
	{
		if (img_wgt->pixel_density > 1.0f)
		{
			img_wgt->pixel_density -= 0.5f;
		}
		
		img_wgt->Redraw();
	}

	if (EventsWidget::CheckEvent(7011, EventsWidget::button_click))
	{				
		img_wgt->FitImage();		
	}

	if (EventsWidget::CheckEvent(7018, EventsWidget::combobox_change))
	{		
		img_wgt->texMode = texture_mode->GetCurString();		
	}

	if (EventsWidget::CheckEvent(7019, EventsWidget::combobox_change))
	{		
		img_wgt->texFilter = texture_filter->GetCurString();		
	}
}

#endif