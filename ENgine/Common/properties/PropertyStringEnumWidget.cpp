
#include "Core/Core.h"

#ifdef EDITOR

int PropertyStringEnumWidget::used_id = 35000;

PropertyStringEnumWidget::PropertyStringEnumWidget(BaseWidget* parent, bool set_need_sort) : PropertyBaseWidget(parent),PropertiesHistoryObject<string>("")
{		
	value = null;
	prop_label = 0;
	prop_ed = 0;
	id = used_id;
	used_id++;
	need_sort = set_need_sort;
}

void PropertyStringEnumWidget::InitControls()
{
    prop_label = new LabelWidget(1500,this, "prop", false,10, 2, PropertyBaseWidget::property_label_width - 10, 20);	
	prop_ed = new ComboBoxWidget(id, this, "",PropertyBaseWidget::property_label_width, 0, 105, 200, need_sort,BaseWidget::eAutoSizeWidth);		
}

void PropertyStringEnumWidget::StartEdit(string* vl, const char* name)
{
	prop_label->SetText(name);
	value = vl;	
    if(value)
    {
	    prop_ed->SetCurString(value->c_str());
        SetCurrentHistoryData(*value);
    }
}

void PropertyStringEnumWidget::Show(bool show)
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

bool PropertyStringEnumWidget::CheckChange()
{
	if (EventsWidget::CheckEvent(id, EventsWidget::combobox_change))
	{
        prop_ed->GetTextFromWidget();
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
void PropertyStringEnumWidget::Apply(const string &data)
{
    *value = data;
    if (owner)
        owner->OnPropertiesChanged();
    prop_ed->SetCurString(value->c_str());
}
#endif