
#include "Core/Core.h"
#include "Editor/Helpers/HistoryObject.h"

#ifdef EDITOR

int PropertyEnumWidget::used_id = 30000;

PropertyEnumWidget::PropertyEnumWidget(BaseWidget* parent) : PropertyBaseWidget(parent),PropertiesHistoryObject<int>(0)
{		
	value = null;
	prop_label = 0;
	prop_ed = 0;
	id = used_id;
	used_id++;	
}

void PropertyEnumWidget::InitControls()
{
    prop_label = new LabelWidget(1500,this, "prop", false,10, 2, PropertyBaseWidget::property_label_width - 10, 20);	
	prop_ed = new ComboBoxWidget(id, this, "",PropertyBaseWidget::property_label_width, 0, 105, 200, false,BaseWidget::eAutoSizeWidth);		
}

void PropertyEnumWidget::StartEdit(int* vl, const char* name)
{
	prop_label->SetText(name);

	int index_value = 0;

	if (vl)
	{
		value = vl;

		for (int i=0;i<table->size();i++)
		{
			if (table->at(i) == *value)
			{
				index_value = i;
				break;
			}
		}
	}

	prop_ed->SetCurString(index_value);
    SetCurrentHistoryData(index_value);
}

void PropertyEnumWidget::Show(bool show)
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

bool PropertyEnumWidget::CheckChange()
{
	if (EventsWidget::CheckEvent(id, EventsWidget::combobox_change))
	{
        AddToHistory(*value);
		*value = table->at(prop_ed->GetCurString());
		if (owner)
			owner->OnPropertiesChanged();
        SetCurrentHistoryData(*value);
		return true;
	}
	return false;
}
void PropertyEnumWidget::Apply(const int& data)
{
    *value = data;
    if (owner)
        owner->OnPropertiesChanged();
    prop_ed->SetCurString(*value);
}
#endif