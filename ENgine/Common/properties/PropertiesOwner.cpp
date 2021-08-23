
#include "Core/Core.h"

PropertiesOwner::PropertiesOwner()
{
#ifdef EDITOR
	ed_rollout = null;
#endif
}

void PropertiesOwner::RegisterProperties()
{

}

#if defined(PC) || defined(OSUNIX)
void PropertiesOwner::SetDefault()
{
	for (int i=0;i<(int)Properties().size();i++)
	{
		
        Properties()[i]->SetDefault(this);
	}	
}
#endif

void PropertiesOwner::LoadProperties(IStreamLoader* loader)
{
	for (int i=0;i<(int)Properties().size();i++)
	{
		Properties()[i]->Load(loader, this);
	}		
}	

void PropertiesOwner::SaveProperties(IStreamSaver* saver)
{
	for (int i=0;i<(int)Properties().size();i++)
	{
		Properties()[i]->Save(saver, this);
	}
}	

void PropertiesOwner::Copy(PropertiesOwner* propHolder)
{
	for (int i=0;i<(int)Properties().size();i++)
	{
		Properties()[i]->Copy(this, propHolder);
	}
}	

bool PropertiesOwner::HaveDifferences(PropertiesOwner* holder)
{
	for (int j=0;j<(int)Properties().size();j++)
	{
		if (!Properties()[j]->IsEqual(this, holder))
		{
			return true;
		}
	}
	return false;
}

void PropertiesOwner::SaveDiffProperties(IStreamSaver* saver, PropertiesOwner* holder)
{
	for (int i=0;i<(int)Properties().size();i++)
	{
		if (!Properties()[i]->IsEqual(this, holder))
		{
			Properties()[i]->SaveDiff(saver, this,holder);
		}			
	}
}

#ifdef EDITOR

void PropertiesOwner::CreateEditorWidgets(BaseWidget* parent, const char* script_class_name)
{		
	//if (ed_rollout) return;

	ed_rollout = (RolloutWidget*)PropertyBaseWidgetsHolder::GetWidget(script_class_name, "");

	if (!ed_rollout)
	{
		ed_rollout = new RolloutWidget(111, parent, "", 0, 0, parent->GetWidth(), 1000, BaseWidget::eAutoSizeWidth);		
		PropertyBaseWidgetsHolder::AddWidget(ed_rollout, script_class_name, "");
    
	}

	for (int i=0;i<(int)Properties().size();i++)
	{	
		PropertyBase* prop = Properties()[i];

		prop->CreateWidget(ed_rollout, script_class_name);			
	}		
}	

void PropertiesOwner::ShowEditorWidgets(bool show)
{		
    if (!ed_rollout) return;

    ed_rollout->Show(show);

    if(show)
        for (int i=0;i<(int)Properties().size();i++)
        {
            Properties()[i]->PrepareWidgets(this);
        }
}	

bool PropertiesOwner::UpdateEditorWidgets()
{
	bool res = false;

	for (int i=0;i<(int)Properties().size();i++)
	{
		res |= Properties()[i]->UpdateWidget();
	}		

	return res;
}

void PropertiesOwner::OnPropertiesChanged()
{

}

#endif