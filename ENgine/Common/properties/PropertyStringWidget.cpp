
#include "Core/Core.h"

#ifdef EDITOR

int PropertyStringWidget::used_id = 50000;

PropertyStringWidget::PropertyStringWidget(BaseWidget* parent) : PropertyBaseWidget(parent),PropertiesHistoryObject<string>("",2.0)
{		
	value = null;
	prop_label = 0;
	prop_ed = 0;
	id = used_id;
}

void PropertyStringWidget::InitControls()
{
    prop_label = new LabelWidget(1500,this, "prop", false,10, 2, PropertyBaseWidget::property_label_width - 10, 20);	
    prop_ed = new TextBoxWidget(id, this, "", PropertyBaseWidget::property_label_width, 0, 100, 20,BaseWidget::eAutoSizeWidth);	
	used_id++;
}

void PropertyStringWidget::StartEdit(string* vl, const char* name)
{
	prop_label->SetText(name);

	value = vl;	

	if (value)
	{		
		prop_ed->SetText(value->c_str());
        SetCurrentHistoryData(*value);
	}
}

void PropertyStringWidget::Show(bool show)
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

bool PropertyStringWidget::CheckChange()
{
	if (EventsWidget::CheckEvent(id, EventsWidget::textbox_change))
	{
		char str[1024];
		String::Copy(str, 512, prop_ed->GetText());
        if(!String::IsEqual(str,value->c_str()))
        {
            AddToHistory(*value);
            value->assign(str);
            if (owner)
                owner->OnPropertiesChanged();
            SetCurrentHistoryData(*value);
        }
		return true;
	}

	return false;
}

void PropertyStringWidget::Apply(const string& data)
{
    *value = data;
    if (owner)
        owner->OnPropertiesChanged();
    prop_ed->SetText(value->c_str());
}

#endif