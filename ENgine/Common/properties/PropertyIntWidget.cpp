
#include "Core/Core.h"

#ifdef EDITOR

int PropertyIntWidget::used_id = 20000;

PropertyIntWidget::PropertyIntWidget(BaseWidget* parent) : PropertyBaseWidget(parent),PropertiesHistoryObject<int>(0)
{		
	value = null;
	prop_label = 0;
	prop_ed = 0;
	id = used_id;
}

void PropertyIntWidget::InitControls()
{
    prop_label = new LabelWidget(1500,this, "prop", false,10, 2, PropertyBaseWidget::property_label_width - 10, 20);	
    prop_ed = new TextBoxWidget(id, this, "", PropertyBaseWidget::property_label_width, 0, 100, 20,BaseWidget::eAutoSizeWidth);	

	used_id++;
}

void PropertyIntWidget::StartEdit(int* vl, const char* name)
{
	prop_label->SetText(name);

	value = vl;	

	if (value)
	{
		char str[32];
		String::Printf(str,32,"%i", *value);
		prop_ed->SetText(str);
        SetCurrentHistoryData(*value);
	}
}

void PropertyIntWidget::Show(bool show)
{
	PropertyBaseWidget::Show(show);
	if (prop_label) prop_label->Show(show);
	if (prop_ed)
    {
        prop_ed->Show(show);
        if(show)
            prop_ed->Resize();
    }
}

bool PropertyIntWidget::CheckChange()
{
	if (EventsWidget::CheckEvent(id, EventsWidget::textbox_change))
	{
		//core_ed.AddHistory(true);
		newValue = atoi(prop_ed->GetText());
		applyTask.Start(PropertyBaseWidget::apply_property_delay);
		return true;
	}

	return false;
}
void PropertyIntWidget::Apply()
{

	if (abs(newValue - *value)> 0)
	{
        AddToHistory(*value);
		*value = newValue;
		if (owner)
			owner->OnPropertiesChanged();
        SetCurrentHistoryData(*value);
	}
	char str[32];
	String::Printf(str,32, "%i", *value);
	prop_ed->SetText(str);
}

void PropertyIntWidget::Apply(const int& data)
{
    *value = data;
    if (owner)
        owner->OnPropertiesChanged();
    char str[32];
    String::Printf(str,32, "%i", *value);
    prop_ed->SetText(str);
}

#endif