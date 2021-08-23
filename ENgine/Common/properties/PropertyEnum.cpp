
#include "Core/Core.h"

void PropertyEnum::Load(IStreamLoader* loader, PropertiesOwner* object)
{
	int value_index;
	if (loader->Read(GetName(), value_index))
	{
#ifdef EDITOR
		if (value_index > Table()->size() - 1)
		{
			value_index = Table()->size() - 1;
		}
#endif
		*((int*)GetPtr(object)) = Table()->at(value_index);
	}
}

void PropertyEnum::Save(IStreamSaver* saver, PropertiesOwner* object)
{
	int value_index = 0;

	for (int i=0;i<Table()->size();i++)
	{
		if (Table()->at(i) == *((int*)GetPtr(object)))
		{
			value_index = i;
			break;
		}
	}

	saver->Write(GetName(), value_index);
}

void PropertyEnum::Copy(PropertiesOwner* object, PropertiesOwner* sec_object)
{
	*((int*)GetPtr(sec_object)) = *((int*)GetPtr(object));
}

void PropertyEnum::EmbedingScript(const char* script_class_name)
{		
	static char prop_name[128];
	sprintf(prop_name, "int %s", GetScriptName());
	core->Script()->Machine()->RegisterObjectProperty(script_class_name, prop_name, GetOffset());
}

bool PropertyEnum::IsEqual(PropertiesOwner* object, PropertiesOwner* sec_object)
{
	return *((int*)GetPtr(object)) == *((int*)GetPtr(sec_object));
}

#ifdef EDITOR

void PropertyEnum::CreateWidget(RolloutWidget* parent, const char* script_class_name)
{
	widgets = (PropertyEnumWidget*)PropertyBaseWidgetsHolder::GetWidget(script_class_name, GetName());

	if (!widgets)
	{
		widgets = new PropertyEnumWidget(parent);
		PropertyBaseWidgetsHolder::AddWidget(widgets, script_class_name, GetName());
		widgets->InitControls();
		widgets->SetSize(220, WidgetHeight());
		parent->AddRolloutChild(GetCategory(), widgets);		

		widgets->table = Table();
		for (int i=0;i<tableNames.size();i++) widgets->prop_ed->AddString(tableNames[i]);
	}
}

bool PropertyEnum::UpdateWidget()
{
	if (widgets) widgets->CheckChange();
	return false;
};

void PropertyEnum::SetSkipWidget(RolloutWidget* parent, bool set)
{
	parent->SetSkipChild(widgets, set);
}

void PropertyEnum::PrepareWidgets(PropertiesOwner* object)
{			
	widgets->owner = object;
	widgets->applyTask.Init(this, widgets);
	widgets->StartEdit((int*)GetPtr(object), GetScriptName());
}
#endif