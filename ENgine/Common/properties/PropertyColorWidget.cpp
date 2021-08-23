
#include "Core/Core.h"

#ifdef EDITOR

int PropertyColorWidget::used_id = 70000;

PropertyColorWidget::PropertyColorWidget(BaseWidget* parent) : PropertyBaseWidget(parent),PropertiesHistoryObject<Color>(COLOR_BLACK)
{
	value = null;
	prop_label = 0;
	prop_ed = 0;
	id = used_id;
}

void PropertyColorWidget::InitControls()
{
    prop_label = new LabelWidget(1500,this, "prop", false,10, 2, PropertyBaseWidget::property_label_width - 10, 20);	
    prop_ed = new LabelWidget(id, this, "",true,PropertyBaseWidget::property_label_width, 0, 100, 20,BaseWidget::eAutoSizeWidth);	
	used_id++;
}

void PropertyColorWidget::StartEdit(Color* vl, const char* name)
{
	if (vl)
	{
		prop_ed->color = *vl;
		prop_ed->Redraw();
	}

	prop_label->SetText(name);

	value = vl;	
    SetCurrentHistoryData(*value);

}


void PropertyColorWidget::Show(bool show)
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

bool PropertyColorWidget::CheckChange()
{
	if (EventsWidget::CheckEvent(id, EventsWidget::label_click))
	{
        AddToHistory(*value);
		ColorDialog(*value);		
		prop_ed->color = *value;
		prop_ed->Redraw();
		if (owner)
			owner->OnPropertiesChanged();
        SetCurrentHistoryData(*value);
        EventsWidget::Clear();
		return true;
	}

	return false;
}

void PropertyColorWidget::Apply(const Color& data)
{
    *value = data;
    prop_ed->color = *value;
    prop_ed->Redraw();
    if (owner)
        owner->OnPropertiesChanged();
}

#endif