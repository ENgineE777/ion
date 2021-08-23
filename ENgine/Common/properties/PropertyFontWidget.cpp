
#include "Core/Core.h"

#ifdef EDITOR

int PropertyFontWidget::used_id = 37500;

extern bool IsFontStyleSupported(const char* name, bool bold, bool italic);

PropertyFontWidget::PropertyFontWidget(BaseWidget* parent) : PropertyBaseWidget(parent),PropertiesHistoryObject()
    ,value(NULL),id(used_id),pEditBox(NULL),pEditValue(NULL),nNewValue(0)
    ,prop_label_name(NULL),prop_cb_name(NULL),prop_label_style(NULL),prop_cb_style(NULL),prop_label_height(NULL),prop_ed_height(NULL),prop_label_outline(NULL),prop_ed_outline(NULL)
    ,prop_label_gap(NULL),prop_ed_gap(NULL),prop_label_color(NULL),prop_ed_color(NULL),prop_label_use_shd(NULL),prop_cb_use_shd(NULL),prop_label_shd_color(NULL),prop_ed_shd_color(NULL)
{		
	used_id += 8;	
}

void PropertyFontWidget::InitControls()
{
	prop_label_name = new LabelWidget(1500, this, "name", false, 10, 2,PropertyBaseWidget::property_label_width - 10, 20);	
	prop_cb_name = new ComboBoxWidget(id, this, "",PropertyBaseWidget::property_label_width, 0, 105, 200, true,BaseWidget::eAutoSizeWidth);

	prop_label_style = new LabelWidget(1500, this, "style", false, 10, 25 + 2, PropertyBaseWidget::property_label_width - 10, 20);	
	prop_cb_style = new ComboBoxWidget(id + 1, this, "",PropertyBaseWidget::property_label_width, 25, 105, 200,  false,BaseWidget::eAutoSizeWidth);	

	prop_label_height = new LabelWidget(1500, this, "height", false, 10, 50 + 2, PropertyBaseWidget::property_label_width - 10, 20);	
	prop_ed_height = new TextBoxWidget(id + 2, this, "",PropertyBaseWidget::property_label_width, 50, 105, 20,BaseWidget::eAutoSizeWidth);

	prop_label_outline = new LabelWidget(1500, this, "outline", false, 10, 75 + 2, PropertyBaseWidget::property_label_width - 10, 20);	
	prop_ed_outline = new TextBoxWidget(id + 3, this, "",PropertyBaseWidget::property_label_width, 75, 105, 20,BaseWidget::eAutoSizeWidth);

	prop_label_gap = new LabelWidget(1500, this, "gap", false, 10, 100 + 2, PropertyBaseWidget::property_label_width - 10, 20);	
	prop_ed_gap = new TextBoxWidget(id + 4, this, "",PropertyBaseWidget::property_label_width, 100, 105, 20,BaseWidget::eAutoSizeWidth);

	prop_label_color = new LabelWidget(1500, this, "color", false, 10, 125 + 2, PropertyBaseWidget::property_label_width - 10, 20);	
	prop_ed_color = new LabelWidget(id + 5, this, "", true,PropertyBaseWidget::property_label_width, 125, 105, 20,BaseWidget::eAutoSizeWidth);	

	prop_label_use_shd = new LabelWidget(1500, this, "use shadow", false, 10, 150 + 2, PropertyBaseWidget::property_label_width - 10, 20);	
	prop_cb_use_shd = new CheckBoxWidget(id + 6, this, "",PropertyBaseWidget::property_label_width, 150, 105, 20,BaseWidget::eAutoSizeWidth);

	prop_label_shd_color = new LabelWidget(1500, this, "shadow color", false, 10, 175 + 2, PropertyBaseWidget::property_label_width - 10, 20);	
	prop_ed_shd_color = new LabelWidget(id + 7, this, "", true,PropertyBaseWidget::property_label_width, 175, 105, 20,BaseWidget::eAutoSizeWidth);
}

void PropertyFontWidget::StartEdit(FontParams* vl, const char* name)
{
	value = vl;
    UpdateWidgets();
    SetCurrentHistoryData(*value); 
}

void PropertyFontWidget::Show(bool show)
{
    PropertyBaseWidget::Show(show);
    if(show)
    {
       prop_cb_name->Resize();
       prop_cb_style->Resize();
       prop_ed_color->Resize();
       prop_ed_height->Resize();
       prop_ed_outline->Resize();
       prop_ed_gap->Resize();
       prop_cb_use_shd->Resize();
       prop_ed_shd_color->Resize();
    }
}

void PropertyFontWidget::UpdateStyleWidget()
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

bool PropertyFontWidget::CheckChange()
{
	if (EventsWidget::CheckEvent(prop_cb_name->GetID(), EventsWidget::combobox_change))
	{	
		prop_cb_name->GetTextFromWidget();
		String::Copy(value->name, 512, prop_cb_name->GetText());			
        
		AddToHistory(*value);

		if (!IsFontStyleSupported(value->name, value->bold, value->italic))
		{
            if (IsFontStyleSupported(value->name, false, false))
            {
                value->bold = 0;
                value->italic = 0;
            }
            else
            if (IsFontStyleSupported(value->name, true, false))
            {
				value->bold = true;
                value->italic = 0;
            }
            else			
            if (IsFontStyleSupported(value->name, false, true))
            {
				value->bold = false;
                value->italic = true;
            }
            else			
            if (IsFontStyleSupported(value->name, true, true))
            {
				value->bold = true;
                value->italic = true;
            }
		}

        UpdateStyleWidget();		
		if (owner)
			owner->OnPropertiesChanged();
        
		SetCurrentHistoryData(*value);
		
		return true;
	}

	if (EventsWidget::CheckEvent(prop_cb_style->GetID(), EventsWidget::combobox_change))
	{	        
		AddToHistory(*value);

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
		
		if (owner)
			owner->OnPropertiesChanged();
        
		SetCurrentHistoryData(*value);
		
		return true;
	}	

	if (EventsWidget::CheckEvent(prop_ed_height->GetID(), EventsWidget::textbox_change))
	{		
        pEditValue = &value->height;
        pEditBox = prop_ed_height;
        nNewValue = atof(prop_ed_height->GetText());
        applyTask.Start(PropertyBaseWidget::apply_property_delay);

		return true;
	}

	if (EventsWidget::CheckEvent(prop_ed_outline->GetID(), EventsWidget::textbox_change))
	{	
        pEditValue = &value->outline;
        pEditBox = prop_ed_outline;
        nNewValue = atof(prop_ed_outline->GetText());
        applyTask.Start(PropertyBaseWidget::apply_property_delay);

		return true;
	}

	if (EventsWidget::CheckEvent(prop_ed_gap->GetID(), EventsWidget::textbox_change))
	{	
        pEditValue = &value->gap;
        pEditBox = prop_ed_gap;
        nNewValue = atof(prop_ed_gap->GetText());
        applyTask.Start(PropertyBaseWidget::apply_property_delay);

		return true;
	}

	if (EventsWidget::CheckEvent(prop_ed_color->GetID(), EventsWidget::label_click))
	{	
        AddToHistory(*value);
		ColorDialog(value->color);		
		if (owner)
			owner->OnPropertiesChanged();
		prop_ed_color->color = value->color;
		prop_ed_color->Redraw();
        
		SetCurrentHistoryData(*value);

		return true;
	}

	if (EventsWidget::CheckEvent(prop_cb_use_shd->GetID(), EventsWidget::checkbox_click))
	{	
        AddToHistory(*value);
		value->use_shadow = prop_cb_use_shd->IsChecked();		
		if (owner)
			owner->OnPropertiesChanged();
        
		SetCurrentHistoryData(*value);

		return true;
	}

	if (EventsWidget::CheckEvent(prop_ed_shd_color->GetID(), EventsWidget::label_click))
	{
        AddToHistory(*value);
		ColorDialog(value->shd_color);		
		
		if (owner)
			owner->OnPropertiesChanged();
		
		prop_ed_shd_color->color = value->color;
		prop_ed_shd_color->Redraw();
        
		SetCurrentHistoryData(*value);
		
		return true;
	}
	return false;
}

void PropertyFontWidget::Apply(const FontParams& data)
{
    *value = data;
    if (owner)
        owner->OnPropertiesChanged();
    UpdateWidgets();
}

void PropertyFontWidget::Apply()
{
    if(pEditBox && pEditValue)
    {
        if (fabs(nNewValue - *pEditValue)>0.001f)
        {
            AddToHistory(*value);
            *pEditValue = nNewValue;
            if (owner)
                owner->OnPropertiesChanged();
            //SetCurrentHistoryData(*value);
        }
        else SetTextBoxValue(pEditBox,*pEditValue);
        pEditBox = NULL;
        pEditValue = NULL;
    }
}

void PropertyFontWidget::UpdateWidgets()
{
    prop_cb_name->SetCurString(value->name);

    UpdateStyleWidget();	

    SetTextBoxValue(prop_ed_height, value->height);
    SetTextBoxValue(prop_ed_outline, value->outline);
    SetTextBoxValue(prop_ed_gap,value->gap);

    prop_ed_color->color = value->color;
    prop_ed_color->Redraw();

    prop_cb_use_shd->SetChecked(value->use_shadow);	

    prop_ed_shd_color->color = value->shd_color;
    prop_ed_shd_color->Redraw();	
}



#endif