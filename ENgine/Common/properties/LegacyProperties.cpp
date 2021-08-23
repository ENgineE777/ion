
#include "Properties.h"
#include "Core/Core.h"
#include "Common/Utils.h"

#ifdef EDITOR

#define APPLY_PROPERTY_DELAY 0.8f

extern bool IsFontStyleSupported(const char* name, bool bold, bool italic);

extern bool DialogSelectFile(bool open, char* file_name, bool full_path, const char* filter);

#include "Editor/Editor.h"

//extern EditorCore core_ed;

HashMap<BaseWidget> PropertyWidgetsHolder::widgets(_FL_);

BaseWidget* PropertyWidgetsHolder::GetWidget(const char* owner_name, const char* prop_name)
{
	char name[512];
	String::Printf(name, 512, "%s_%s", owner_name, prop_name);

	return widgets.Get(name);
}

void PropertyWidgetsHolder::AddWidget(BaseWidget* widget, const char* owner_name, const char* prop_name)
{
	char name[512];
	String::Printf(name, 512, "%s_%s", owner_name, prop_name);
	widget->Show(false);

	widgets.Add(name, widget);
}


int BoolPropWidget::used_id = 10000;

BoolPropWidget::BoolPropWidget(BaseWidget* parent) : PropertyWidget(parent)
{		
	value = null;
	prop_label = 0;
	prop_ed = 0;
	id = used_id;
}

void BoolPropWidget::InitControls()
{
	prop_label = new LabelWidget(1500, this, "prop", false, 5, 2, 90, 20, false);	
	prop_ed = new CheckBoxWidget(id, this, "", 95, 0, 105, 20, false);	

	used_id++;
}

void BoolPropWidget::StartEdit(bool* vl, const char* name)
{
	prop_label->SetText(name);
	value = vl;	
	if (value)
		prop_ed->SetChecked( *value );
}

void BoolPropWidget::Show(bool show)
{
	PropertyWidget::Show(show);
	if (prop_label) prop_label->Show(show);
	if (prop_ed) prop_ed->Show(show);
}

bool BoolPropWidget::CheckChange()
{
	if (EventsWidget::CheckEvent(id, EventsWidget::checkbox_click))
	{
		//core_ed.AddHistory(true);

		*value = prop_ed->IsChecked();		

		propertyOwner->TieValues();
		if (propertyOwner->owner->allow_reinit) propertyOwner->owner->OnPropertiesChanged();

		return true;
	}

	return false;
}

int IntPropWidget::used_id = 20000;

IntPropWidget::IntPropWidget(BaseWidget* parent) : PropertyWidget(parent)
{		
	value = null;
	prop_label = 0;
	prop_ed = 0;
	id = used_id;
}

void IntPropWidget::InitControls()
{
	prop_label = new LabelWidget(1500, this, "prop", false, 5, 2, 90, 20, false);	
	prop_ed = new TextBoxWidget(id, this, "", 95, 0, 105, 20, false);	

	used_id++;
}

void IntPropWidget::StartEdit(int* vl, const char* name)
{
	prop_label->SetText(name);
	
	value = vl;	

	if (value)
	{
		char str[32];
		String::Printf(str,32,"%i", *value);
		prop_ed->SetText(str);
	}
}

void IntPropWidget::Show(bool show)
{
	PropertyWidget::Show(show);
	if (prop_label) prop_label->Show(show);
	if (prop_ed) prop_ed->Show(show);
}

bool IntPropWidget::CheckChange()
{
	if (EventsWidget::CheckEvent(id, EventsWidget::textbox_change))
	{
		//core_ed.AddHistory(true);
		newValue = atoi(prop_ed->GetText());
        applyTask.Start(APPLY_PROPERTY_DELAY);
		return true;
	}

	return false;
}
bool IntPropWidget::CanApply(unsigned taskId)
{
    bool bRet = false;
    if (abs(newValue - *value)> 0)
    {
        *value = newValue;
        bRet = true;
    }
    char str[32];
    String::Printf(str,32, "%i", *value);
    prop_ed->SetText(str);
    return bRet;
}
int EnumPropWidget::used_id = 30000;

EnumPropWidget::EnumPropWidget(BaseWidget* parent, bool set_need_sort) : PropertyWidget(parent)
{		
	value = null;
	prop_label = 0;
	prop_ed = 0;
	id = used_id;
	used_id++;
	need_sort = set_need_sort;
}

void EnumPropWidget::InitControls()
{
	prop_label = new LabelWidget(1500, this, "prop", false, 5, 2, 90, 20, false);	
	prop_ed = new ComboBoxWidget(id, this, "", 95, 0, 105, 200, need_sort, false);		
}

void EnumPropWidget::StartEdit(int* vl, const char* name)
{
	prop_label->SetText(name);
	
	if (vl) value = vl;	
	prop_ed->SetCurString(*value);
}

void EnumPropWidget::Show(bool show)
{
	PropertyWidget::Show(show);
	if (prop_label) prop_label->Show(show);
	if (prop_ed) prop_ed->Show(show);
}

bool EnumPropWidget::CheckChange()
{
	if (EventsWidget::CheckEvent(id, EventsWidget::combobox_change))
	{
		//core_ed.AddHistory(true);
		*value = prop_ed->GetCurString();		

		propertyOwner->TieValues();
		if (propertyOwner->owner->allow_reinit) propertyOwner->owner->OnPropertiesChanged();				

		return true;
	}

	return false;
}


int StringEnumPropWidget::used_id = 35000;

StringEnumPropWidget::StringEnumPropWidget(BaseWidget* parent, bool set_need_sort) : PropertyWidget(parent)
{		
	value = null;
	prop_label = 0;
	prop_ed = 0;
	id = used_id;
	used_id++;
	need_sort = set_need_sort;
}

void StringEnumPropWidget::InitControls()
{
	prop_label = new LabelWidget(1500, this, "prop", false, 5, 2, 90, 20, false);	
	prop_ed = new ComboBoxWidget(id, this, "", 95, 0, 105, 200, need_sort, false);		
}

void StringEnumPropWidget::StartEdit(char* vl, const char* name)
{
	prop_label->SetText(name);
	
	if (vl) value = vl;	
	prop_ed->SetCurString(value);
}

void StringEnumPropWidget::Show(bool show)
{
	PropertyWidget::Show(show);
	if (prop_label) prop_label->Show(show);
	if (prop_ed) prop_ed->Show(show);
}

bool StringEnumPropWidget::CheckChange()
{
	if (EventsWidget::CheckEvent(id, EventsWidget::combobox_change))
	{
		//core_ed.AddHistory(true);

		prop_ed->GetTextFromWidget();
		String::Copy(value, 512, prop_ed->GetText());			

		propertyOwner->TieValues();
		if (propertyOwner->owner->allow_reinit) propertyOwner->owner->OnPropertiesChanged();				

		return true;
	}

	return false;
}


int FontPropWidget::used_id = 37500;

FontPropWidget::FontPropWidget(BaseWidget* parent) : PropertyWidget(parent)
{		
	value = null;
	
	prop_label_name = null;	
	prop_cb_name = null;

	prop_label_style = null;	
	prop_cb_style = null;	

	prop_label_height = null;	
	prop_ed_height = null;

	prop_label_outline = null;	
	prop_ed_outline = null;

	prop_label_gap = null;	
	prop_ed_gap = null;

	prop_label_color = null;	
	prop_ed_color = null;

	prop_label_use_shd = null;	
	prop_cb_use_shd = null;

	prop_label_shd_color = null;	
	prop_ed_shd_color = null;
	
	id = used_id;
	used_id += 8;	
}

void FontPropWidget::InitControls()
{
	prop_label_name = new LabelWidget(1500, this, "name", false, 5, 2, 90, 20, false);	
	prop_cb_name = new ComboBoxWidget(id, this, "", 95, 0, 105, 200, true, false);

	prop_label_style = new LabelWidget(1500, this, "style", false, 5, 25 + 2, 90, 20, false);	
	prop_cb_style = new ComboBoxWidget(id + 1, this, "", 95, 25, 105, 200,  false, false);	

	prop_label_height = new LabelWidget(1500, this, "height", false, 5, 50 + 2, 90, 20, false);	
	prop_ed_height = new TextBoxWidget(id + 2, this, "", 95, 50, 105, 20, false);

	prop_label_outline = new LabelWidget(1500, this, "outline", false, 5, 75 + 2, 90, 20, false);	
	prop_ed_outline = new TextBoxWidget(id + 3, this, "", 95, 75, 105, 20, false);

	prop_label_gap = new LabelWidget(1500, this, "gap", false, 5, 100 + 2, 90, 20, false);	
	prop_ed_gap = new TextBoxWidget(id + 4, this, "", 95, 100, 105, 20, false);

	prop_label_color = new LabelWidget(1500, this, "color", false, 5, 125 + 2, 90, 20, false);	
	prop_ed_color = new LabelWidget(id + 5, this, "", true, 95, 125, 105, 20, false);	

	prop_label_use_shd = new LabelWidget(1500, this, "use shadow", false, 5, 150 + 2, 90, 20, false);	
	prop_cb_use_shd = new CheckBoxWidget(id + 6, this, "", 95, 150, 105, 20, false);

	prop_label_shd_color = new LabelWidget(1500, this, "shadow color", false, 5, 175 + 2, 90, 20, false);	
	prop_ed_shd_color = new LabelWidget(id + 7, this, "", true, 95, 175, 105, 20, false);
}

void FontPropWidget::StartEdit(FontParams* vl, const char* name)
{
	value = vl;
	//prop_label->SetText(name);

	prop_cb_name->SetCurString(value->name);

	UpdateStyleWidget();	

	{
		char str[32];
		String::Printf(str,32,"%4.3f", value->height);
		prop_ed_height->SetText(str);
	}

	{
		char str[32];
		String::Printf(str,32, "%4.3f", value->outline);
		prop_ed_outline->SetText(str);
	}

	{
		char str[32];
		String::Printf(str,32, "%4.3f", value->gap);
		prop_ed_gap->SetText(str);
	}

	prop_ed_color->color = value->color;
	prop_ed_color->Redraw();
	
	prop_cb_use_shd->SetChecked(value->use_shadow);	

	prop_ed_shd_color->color = value->shd_color;
	prop_ed_shd_color->Redraw();	
}

void FontPropWidget::UpdateStyleWidget()
{
	prop_cb_style->ClearList();

	int index = -1;
	int cur_index = -1;		
	
	if (IsFontStyleSupported(value->name, false, false))
	{
		index++;
		prop_cb_style->AddString("Regular");

		if (!value->bold && !value->italic)
		{
			cur_index = index;
		}
	}

	if (IsFontStyleSupported(value->name, true, false))
	{
		index++;
		prop_cb_style->AddString("Bold");

		if (value->bold && !value->italic)
		{
			cur_index = index;
		}
	}
	
	if (IsFontStyleSupported(value->name, false, true))
	{
		index++;
		prop_cb_style->AddString("Italic");

		if (!value->bold && value->italic)
		{
			cur_index = index;
		}
	}
	
	if (IsFontStyleSupported(value->name, true, true))
	{
		index++;
		prop_cb_style->AddString("BoldItalic");

		if (value->bold && value->italic)
		{
			cur_index = index;
		}
	}	

	prop_cb_style->SetCurString(cur_index);
}

bool FontPropWidget::CheckChange()
{
	if (EventsWidget::CheckEvent(prop_cb_name->GetID(), EventsWidget::combobox_change))
	{	
		prop_cb_name->GetTextFromWidget();
		String::Copy(value->name, 512, prop_cb_name->GetText());			

		if (!IsFontStyleSupported(value->name, value->bold, value->italic))
		{
			if (IsFontStyleSupported(value->name, !value->bold, !value->italic))
			{
				value->bold = 0;
				value->italic = 0;
			}
			else
			if (IsFontStyleSupported(value->name, value->bold, !value->italic))
			{
				value->bold = true;
				value->italic = 0;
			}
			else			
			if (IsFontStyleSupported(value->name, !value->bold, value->italic))
			{
				value->bold = true;
				value->italic = 0;
			}
			else			
			if (IsFontStyleSupported(value->name, value->bold, value->italic))
			{
				value->bold = true;
				value->italic = true;
			}
		}

		UpdateStyleWidget();		

		propertyOwner->TieValues();
		if (propertyOwner->owner->allow_reinit) propertyOwner->owner->OnPropertiesChanged();				

		return true;
	}

	if (EventsWidget::CheckEvent(prop_cb_style->GetID(), EventsWidget::combobox_change))
	{	
		if (String::IsEqual(prop_cb_style->GetText(), "Regular"))
		{
			value->bold = 0;
			value->italic = 0;
		}
		else
		if (String::IsEqual(prop_cb_style->GetText(), "Bold"))
		{
			value->bold = true;
			value->italic = 0;
		}
		else
		if (String::IsEqual(prop_cb_style->GetText(), "Italic"))
		{
			value->bold = 0;
			value->italic = true;
		}
		else
		if (String::IsEqual(prop_cb_style->GetText(), "BoldItalic"))
		{
			value->bold = true;
			value->italic = true;
		}		

		propertyOwner->TieValues();
		if (propertyOwner->owner->allow_reinit) propertyOwner->owner->OnPropertiesChanged();

		return true;
	}	

	if (EventsWidget::CheckEvent(prop_ed_height->GetID(), EventsWidget::textbox_change))
	{		
		float val = value->height;
		val = atof(prop_ed_height->GetText());

		if (fabs(val - value->height)>0.001f)
		{
			value->height = val;
		}
		else
		{
			char str[32];
			String::Printf(str,32, "%4.3f", val);
			core->TraceTo("app", str);
			prop_ed_height->SetText(str);
		}	

        propertyOwner->TieValues();
        if (propertyOwner->owner->allow_reinit) propertyOwner->owner->OnPropertiesChanged();	

		return true;
	}

	if (EventsWidget::CheckEvent(prop_ed_outline->GetID(), EventsWidget::textbox_change))
	{		
		float val = value->outline;
		val = atof(prop_ed_outline->GetText());

		if (fabs(val - value->outline)>0.001f)
		{
			value->outline = val;
		}
		else
		{
			char str[32];
			String::Printf(str,32, "%4.3f", val);
			core->TraceTo("app", str);
			prop_ed_outline->SetText(str);
		}	

        propertyOwner->TieValues();
        if (propertyOwner->owner->allow_reinit) propertyOwner->owner->OnPropertiesChanged();	

		return true;
	}

	if (EventsWidget::CheckEvent(prop_ed_gap->GetID(), EventsWidget::textbox_change))
	{		
		float val = value->gap;
		val = atof(prop_ed_gap->GetText());

		if (fabs(val - value->gap)>0.001f)
		{
			value->gap = val;
		}
		else
		{
			char str[32];
			String::Printf(str,32, "%4.3f", val);
			core->TraceTo("app", str);
			prop_ed_gap->SetText(str);
		}	

        propertyOwner->TieValues();
        if (propertyOwner->owner->allow_reinit) propertyOwner->owner->OnPropertiesChanged();					

		return true;
	}

	if (EventsWidget::CheckEvent(prop_ed_color->GetID(), EventsWidget::label_click))
	{		
		ColorDialog(value->color);		

		propertyOwner->TieValues();
		if (propertyOwner->owner->allow_reinit) propertyOwner->owner->OnPropertiesChanged();		

		prop_ed_color->color = value->color;
		prop_ed_color->Redraw();

		return true;
	}

	if (EventsWidget::CheckEvent(prop_cb_use_shd->GetID(), EventsWidget::checkbox_click))
	{		
		value->use_shadow = prop_cb_use_shd->IsChecked();		

		propertyOwner->TieValues();
		if (propertyOwner->owner->allow_reinit) propertyOwner->owner->OnPropertiesChanged();

		return true;
	}

	if (EventsWidget::CheckEvent(prop_ed_shd_color->GetID(), EventsWidget::label_click))
	{
		ColorDialog(value->shd_color);		

		propertyOwner->TieValues();
		if (propertyOwner->owner->allow_reinit) propertyOwner->owner->OnPropertiesChanged();		

		prop_ed_shd_color->color = value->color;
		prop_ed_shd_color->Redraw();

		return true;
	}

	return false;
}


int FloatPropWidget::used_id = 40000;

FloatPropWidget::FloatPropWidget(BaseWidget* parent) : PropertyWidget(parent)
{		
	value = null;
	prop_label = 0;
	prop_ed = 0;
	id = used_id;
}

void FloatPropWidget::InitControls()
{
	prop_label = new LabelWidget(1500, this, "prop", false, 5, 2, 90, 20, false);	
	prop_ed = new TextBoxWidget(id, this, "", 95, 0, 105, 20, false);	

	used_id++;
}

void FloatPropWidget::StartEdit(float* vl, const char* name)
{
	if (!prop_label) return;

	prop_label->SetText(name);
	
	value = vl;	

	if (value)
	{
		char str[32];
		String::Printf(str,32, "%4.3f", *value);
		prop_ed->SetText(str);
	}
}

void FloatPropWidget::Show(bool show)
{
	PropertyWidget::Show(show);
	if (prop_label) prop_label->Show(show);
	if (prop_ed) prop_ed->Show(show);
}

bool FloatPropWidget::CheckChange()
{
	if (EventsWidget::CheckEvent(id, EventsWidget::textbox_change))
	{
		//core_ed.AddHistory(true);
		newValue = atof(prop_ed->GetText());
        applyTask.Start(APPLY_PROPERTY_DELAY);
		return true;
	}
	return false;
}

bool FloatPropWidget::CanApply(unsigned taskId)
{
    bool bRet = false;
    if (fabs(newValue - *value)>0.001f)
    {
        *value = newValue;
        bRet = true;
    }
    char str[32];
    String::Printf(str,32,"%4.3f",*value);
    prop_ed->SetText(str);
    return bRet;
}

int StringPropWidget::used_id = 50000;

StringPropWidget::StringPropWidget(BaseWidget* parent) : PropertyWidget(parent)
{		
	value = null;
	prop_label = 0;
	prop_ed = 0;
	id = used_id;
}

void StringPropWidget::InitControls()
{
	prop_label = new LabelWidget(1500, this, "prop", false, 5, 2, 90, 20, false);	
	prop_ed = new TextBoxWidget(id, this, "", 95, 0, 105, 20, false);	

	used_id++;
}

void StringPropWidget::StartEdit(char* vl, const char* name)
{
	prop_label->SetText(name);
	
	value = vl;	

	if (value)
	{		
		prop_ed->SetText(value);
	}
}

void StringPropWidget::Show(bool show)
{
	PropertyWidget::Show(show);
	if (prop_label) prop_label->Show(show);
	if (prop_ed) prop_ed->Show(show);
}

bool StringPropWidget::CheckChange()
{
	if (EventsWidget::CheckEvent(id, EventsWidget::textbox_change))
	{
		//core_ed.AddHistory(true);

		String::Copy(value, 512, prop_ed->GetText());

		propertyOwner->TieValues();
		if (propertyOwner->owner->allow_reinit)	propertyOwner->owner->OnPropertiesChanged();				

		return true;
	}

	return false;
}

int FileNameProp::used_id = 60000;

FileNameProp::FileNameProp(BaseWidget* parent) : PropertyWidget(parent)
{		
	file_name = null;
	prop_label = 0;
	prop_ed = 0;
	del_ed = 0;
	id = used_id;
	res_type[0] = 0;
	res_view_started = false;
}

void FileNameProp::InitControls(const char* tp)
{
	prop_label = new LabelWidget(1500, this, "prop", false, 5, 2, 90, 20, false);	
	prop_ed = new ButtonWidget(id, this, false, "", 95, 0, 85, 20, false);	
	del_ed = new ButtonWidget(id + 10000, this, false, "X", 180, 0, 20, 20, false);	

	String::Copy(res_type, 16, tp);

	used_id++;
}

void FileNameProp::StartEdit(char* fl_name, const char* name)
{
	prop_label->SetText(name);
	
	file_name = fl_name;	

	if (file_name)
	{		
		prop_ed->SetText(file_name);
	}
}

void FileNameProp::Show(bool show)
{
	PropertyWidget::Show(show);
	if (prop_label) prop_label->Show(show);
	if (prop_ed) prop_ed->Show(show);
}

bool FileNameProp::CheckChange()
{
	if (EventsWidget::CheckEvent(id, EventsWidget::button_click))
	{		
		//EventsWidget::OpenResourceBrowser(res_type);
		
		//res_view_started = true;
		
		char name[512];
		name[0] = 0;

		if (OpenDialog(name, true))
		{
			//core_ed.AddHistory(true);

			String::Copy(file_name, 512, name);

			prop_ed->SetText(file_name);
			propertyOwner->TieValues();
			if (propertyOwner->owner->allow_reinit) propertyOwner->owner->OnPropertiesChanged();
			return true;
		}

		/*{
			char name[512];
			name[0] = 0;
			
			char filter[128];
			String::Printf(filter, 128, "resource file(*.%s)\0*.%s\0", res_type, res_type);

			if (DialogSelectFile(true, name, false, ""))
			{
				if (!String::IsEqual(name, file_name))
				{
					core_ed.AddHistory(true);

					String::Copy(file_name, 512, name);

					prop_ed->SetText(file_name);
	
					propertyOwner->TieValues();
					if (propertyOwner->owner->allow_reinit) propertyOwner->owner->ReInit();
				}
			}
		}*/
	}

	if (res_view_started)
	{
		bool succeced = false;

		if (EventsWidget::IsResourceBrowserFinished(succeced))
		{			
			if (succeced && !String::IsEqual(EventsWidget::GetEventData(), file_name))
			{
				//core_ed.AddHistory(true);

				String::Copy(file_name, 512, EventsWidget::GetEventData());

				prop_ed->SetText(file_name);

				propertyOwner->TieValues();
				if (propertyOwner->owner->allow_reinit) propertyOwner->owner->OnPropertiesChanged();
			}

			res_view_started = false;

			return true;
		}
	}

	if (EventsWidget::CheckEvent(id + 10000, EventsWidget::button_click))
	{
		String::Copy(file_name, 512, "");

		prop_ed->SetText(file_name);
		propertyOwner->TieValues();
		if (propertyOwner->owner->allow_reinit) propertyOwner->owner->OnPropertiesChanged();
		return true;
	}

	return false;
}

int ColorPropWidget::used_id = 70000;

ColorPropWidget::ColorPropWidget(BaseWidget* parent) : PropertyWidget(parent)
{
	value = null;
	prop_label = 0;
	prop_ed = 0;
	id = used_id;
}

void ColorPropWidget::InitControls()
{
	prop_label = new LabelWidget(1500, this, "prop", false, 5, 2, 90, 20, false);	
	prop_ed = new LabelWidget(id, this, "", true, 95, 0, 105, 20, false);	

	used_id++;
}

void ColorPropWidget::StartEdit(Color* vl, const char* name)
{
	if (vl)
	{
		prop_ed->color = *vl;
		prop_ed->Redraw();
	}

	prop_label->SetText(name);

	value = vl;	

	/*if (value)
	{
		char str[32];
		sprintf(str, "%4.3f", *value);
		prop_ed->SetText(str);
	}*/
}

/*
void ColorDialog(Color& color)
{	
	CHOOSECOLOR cs_color;

	static dword colors[16];

	cs_color.lStructSize = sizeof(cs_color);
	cs_color.hwndOwner = core->GetHWND();
	cs_color.hInstance = NULL;
	cs_color.Flags = CC_FULLOPEN | CC_ANYCOLOR | CC_RGBINIT;
		
	Color tmp = Color(color.r, color.g, color.b);	

	cs_color.rgbResult = tmp.Get();
	cs_color.lpCustColors = (COLORREF*)&colors;
	cs_color.lCustData = 0;
	cs_color.lpfnHook = 0;
	cs_color.lpTemplateName = 0;	

	if (ChooseColor(&cs_color))
	{			
		tmp = Color(cs_color.rgbResult);			
		color = Color(tmp.r, tmp.g, tmp.b);
	}	
}*/

void ColorPropWidget::Show(bool show)
{
	PropertyWidget::Show(show);
	if (prop_label) prop_label->Show(show);
	if (prop_ed) prop_ed->Show(show);
}

bool ColorPropWidget::CheckChange()
{
	if (EventsWidget::CheckEvent(id, EventsWidget::label_click))
	{
		//core_ed.AddHistory(true);

		ColorDialog(*value);		

		propertyOwner->TieValues();
		if (propertyOwner->owner->allow_reinit) propertyOwner->owner->OnPropertiesChanged();		

		prop_ed->color = *value;
		prop_ed->Redraw();

		return true;
	}

	return false;
}

int CallbackPropWgt::used_id = 65000;

CallbackPropWgt::CallbackPropWgt(BaseWidget* parent) : PropertyWidget(parent)
{				
	prop_ed = 0;	
	id = used_id;	
}

void CallbackPropWgt::InitControls(const char* name)
{	
	prop_ed = new ButtonWidget(id, this, false, name, 5, 0, 195, 20, false);		

	used_id++;
}

void CallbackPropWgt::Show(bool show)
{
	PropertyWidget::Show(show);	
}

void CallbackPropWgt::CheckClick()
{
	if (EventsWidget::CheckEvent(id, EventsWidget::button_click))
	{		
		if (callback)
		{
			callback(propertyOwner->owner);
		}		
	}
}

void PropertyWidget::Task::ApplyChanges(float dt, int level)
{
    if(propertyOwner)
    {
        if(pOwner && pOwner->CanApply(id))
        {
            propertyOwner->TieValues();
            if (propertyOwner->owner->allow_reinit) propertyOwner->owner->OnPropertiesChanged();	
        }
        core->DelTask(this,(TaskHolder::Task)&PropertyWidget::Task::ApplyChanges);
        active = false;
    }
}

void PropertyWidget::Task::Start(float delay)
{
    if(propertyOwner)
    {
      if(!active)
      {
          core->AddTask(this,this, (TaskHolder::Task)&PropertyWidget::Task::ApplyChanges, 0, delay);
          active = true;
      }
      else
      {
           core->DelTask(this,(TaskHolder::Task)&PropertyWidget::Task::ApplyChanges);
           core->AddTask(this,this, (TaskHolder::Task)&PropertyWidget::Task::ApplyChanges, 0, delay);
      }
    }
}

#endif


