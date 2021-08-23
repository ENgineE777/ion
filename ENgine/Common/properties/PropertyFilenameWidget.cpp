
#include "Core/Core.h"
#ifdef EDITOR
#include "Platforms/PC/GUI/SizerWidget.h"

int PropertyFilenameWidget::used_id = 60000;

PropertyFilenameWidget::PropertyFilenameWidget(BaseWidget* parent) : PropertyBaseWidget(parent),PropertiesHistoryObject<string>("")
{		
	file_name = null;
	prop_label = 0;
	prop_ed = 0;
	del_ed = 0;
	id = used_id;
	res_type[0] = 0;

}

void PropertyFilenameWidget::InitControls(const char* tp)
{

    SizerWidget* pLeftSizer = new SizerWidget(9999, this, false,PropertyBaseWidget::property_label_width);	
    SizerWidget* pCenterSizer = new SizerWidget(9999, this, false, -1);
    SizerWidget* pRightSizer = new SizerWidget(9999, this, false, 20);

	prop_label = new LabelWidget(1500, pLeftSizer, "prop", false,10, 2,PropertyBaseWidget::property_label_width - 10, 20);	
	prop_ed = new ButtonWidget(id, pCenterSizer, false, "", 0, 0,20, 20,BaseWidget::eAutoSizeWidth);	
	del_ed = new ButtonWidget(id + 10000, pRightSizer, false, "X",0, 0, 20, 20);	
    del_ed->CreateTooltip("Clear");
	String::Copy(res_type, 16, tp);

	used_id++;
}
void PropertyFilenameWidget::UpdateFileName()
{
    int nPos = file_name->find_last_of("/");
    string text;
    if(nPos != -1)
    {
        ++nPos;
        text.assign(&file_name->operator[](nPos),file_name->size() - nPos);
    }
    prop_ed->SetText(text.c_str());
    prop_ed->CreateTooltip(file_name->c_str());
}
void PropertyFilenameWidget::StartEdit(string* fl_name, const char* name)
{
	prop_label->SetText(name);
	file_name = fl_name;	
	if (file_name)
	{	
        UpdateFileName();
        SetCurrentHistoryData(*file_name);
	}
}

void PropertyFilenameWidget::Show(bool show)
{
	PropertyBaseWidget::Show(show);
	//if(prop_label) prop_label->Show(show);
	//if(prop_ed)prop_ed->Show(show);
   // if(del_ed)del_ed->Show(show);
    if(show)
        Resize();
}

bool PropertyFilenameWidget::CheckChange()
{
	if (EventsWidget::CheckEvent(id, EventsWidget::button_click))
	{								
		char name[512];
		name[0] = 0;

		if (OpenDialog(name, true))
		{
            if(!String::IsEqual(name,file_name->c_str()))
            {
                AddToHistory(*file_name);
                file_name->assign(name);
                UpdateFileName();
                if (owner)
                    owner->OnPropertiesChanged();
                SetCurrentHistoryData(*file_name);
            }
			return true;
		}		
	}
	if (EventsWidget::CheckEvent(id + 10000, EventsWidget::button_click))
	{
        if(file_name->empty())
            return true;
        AddToHistory(*file_name);
		file_name->assign("");
		prop_ed->SetText("");
		if (owner)
			owner->OnPropertiesChanged();
        SetCurrentHistoryData(*file_name);
		return true;
	}

	return false;
}
void PropertyFilenameWidget::Apply(const string& data)
{
    *file_name = data;
    if (owner)
        owner->OnPropertiesChanged();
    prop_ed->SetText(file_name->c_str());
}

#endif