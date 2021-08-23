
#include "Core/Core.h"

#ifdef EDITOR
#include "Platforms/PC/GUI/SizerWidget.h"

int PropertyFloatWidget::used_id = 40000;

PropertyFloatWidget::PropertyFloatWidget(BaseWidget* parent) : PropertyBaseWidget(parent),PropertiesHistoryObject(0)
{		
	value = null;
	prop_label = 0;
	prop_ed = 0;
	id = used_id;
}

void PropertyFloatWidget::InitControls()
{
	prop_label = new LabelWidget(1500,this, "prop", false,10, 2, PropertyBaseWidget::property_label_width - 10, 20);	
	prop_ed = new TextBoxWidget(id, this, "",PropertyBaseWidget::property_label_width, 0, 100, 20,BaseWidget::eAutoSizeWidth);	
	used_id++;
}

void PropertyFloatWidget::StartEdit(float* vl, const char* name)
{
	if (!prop_label) return;

	prop_label->SetText(name);

	value = vl;	

	if (value)
	{
		char str[32];
		String::Printf(str,32, "%4.3f", *value);
		prop_ed->SetText(str);
        SetCurrentHistoryData(*value);
	}
}

void PropertyFloatWidget::Show(bool show)
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

bool PropertyFloatWidget::CheckChange()
{
	if (EventsWidget::CheckEvent(id, EventsWidget::textbox_change))
	{
		newValue = atof(prop_ed->GetText());
		applyTask.Start(PropertyBaseWidget::apply_property_delay);
		return true;
	}
	return false;
}


void PropertyFloatWidget::Apply()
{
	if (fabs(newValue - *value)>0.001f)
	{
        AddToHistory(*value);
		*value = newValue;
		if (owner)
			owner->OnPropertiesChanged();
        SetCurrentHistoryData(*value);
	}
    char str[32];
    String::Printf(str,32,"%4.3f",*value);
    prop_ed->SetText(str);
}

void PropertyFloatWidget::Apply(const float& data)
{
    *value = data;
    if (owner)
        owner->OnPropertiesChanged();
    char str[32];
    String::Printf(str,32,"%4.3f",*value);
    prop_ed->SetText(str);
}

#endif