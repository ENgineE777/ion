
#include "Core/Core.h"

#ifdef EDITOR

int PropertyBoolWidget::used_id = 10000;

PropertyBoolWidget::PropertyBoolWidget(BaseWidget* parent) : PropertyBaseWidget(parent),PropertiesHistoryObject(false)
{		
	value = null;
	prop_label = 0;
	prop_ed = 0;
	id = used_id;
}

void PropertyBoolWidget::InitControls()
{
	prop_label = new LabelWidget(1500,this, "prop", false,10, 2, PropertyBaseWidget::property_label_width - 10, 20);	
	prop_ed = new CheckBoxWidget(id, this, "",PropertyBaseWidget::property_label_width, 0, 32, 20);	
	used_id++;
}

void PropertyBoolWidget::StartEdit(bool* vl, const char* name)
{
	prop_label->SetText(name);
	value = vl;	
	if (value)
    {
		prop_ed->SetChecked( *value );
        SetCurrentHistoryData(*value);
    }
}

void PropertyBoolWidget::Show(bool show)
{
	PropertyBaseWidget::Show(show);
	if (prop_label) prop_label->Show(show);
	if (prop_ed) prop_ed->Show(show);
}

bool PropertyBoolWidget::CheckChange()
{
	if (EventsWidget::CheckEvent(id, EventsWidget::checkbox_click))
	{
        AddToHistory(*value);
		*value = prop_ed->IsChecked();
		if (owner)
			owner->OnPropertiesChanged();
        SetCurrentHistoryData(*value);
		return true;
	}
	return false;
}

void PropertyBoolWidget::Apply(const bool& data)
{
    *value = data;
    if (owner)
        owner->OnPropertiesChanged();
    prop_ed->SetChecked( *value );
}

#endif