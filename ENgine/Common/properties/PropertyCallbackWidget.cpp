
#include "Core/Core.h"

#ifdef EDITOR

int PropertyCallbackWidget::used_id = 65000;

PropertyCallbackWidget::PropertyCallbackWidget(BaseWidget* parent) : PropertyBaseWidget(parent)
{				
	prop_ed = 0;	
	id = used_id;	
	owner = null;
}

void PropertyCallbackWidget::InitControls(CallbackProperty set_callback, const char* name)
{	
	callback = set_callback;

	prop_ed = new ButtonWidget(id, this, false, name, 5, 0, 100, 20,ButtonWidget::eAutoSizeWidth);		

	used_id++;
}

void PropertyCallbackWidget::Show(bool show)
{
	PropertyBaseWidget::Show(show);	
    if(prop_ed)
    {
        prop_ed->Show(show);
        if(show)
            prop_ed->Resize();
    }
}

void PropertyCallbackWidget::StartEdit(PropertiesOwner* prop_owner)
{
	owner = prop_owner;
}

void PropertyCallbackWidget::CheckClick()
{
	if (EventsWidget::CheckEvent(id, EventsWidget::button_click))
	{		
		if (callback)
		{
			callback(owner);
		}
	}
}

#endif