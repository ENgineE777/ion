
#include "Core/Core.h"

void PropertyStringEnum::Load(IStreamLoader* loader, PropertiesOwner* object)
{
	char value[1024];
	if (loader->Read(GetName(), value, 1024))
	{
		((string*)GetPtr(object))->assign(value);
	}
}

void PropertyStringEnum::Save(IStreamSaver* saver, PropertiesOwner* object)
{	
	saver->Write(GetName(), ((string*)GetPtr(object))->c_str());
}

void PropertyStringEnum::Copy(PropertiesOwner* object, PropertiesOwner* sec_object)
{
	((string*)GetPtr(sec_object))->assign(((string*)GetPtr(object))->c_str());
}

void PropertyStringEnum::EmbedingScript(const char* script_class_name)
{			
}

bool PropertyStringEnum::IsEqual(PropertiesOwner* object, PropertiesOwner* sec_object)
{
	return String::IsEqual(((string*)GetPtr(object))->c_str(), ((string*)GetPtr(sec_object))->c_str());
}

#ifdef EDITOR

void PropertyStringEnum::CreateWidget(RolloutWidget* parent, const char* script_class_name)
{
	widgets = (PropertyStringEnumWidget*)PropertyBaseWidgetsHolder::GetWidget(script_class_name, GetName());

	if (!widgets)
	{
		widgets = new PropertyStringEnumWidget(parent, NeedSort());
		PropertyBaseWidgetsHolder::AddWidget(widgets, script_class_name, GetName());
		widgets->InitControls();
		widgets->SetSize(220, WidgetHeight());
		parent->AddRolloutChild(GetCategory(), widgets);
		if (Enumerator())
		{
			StringEnumPropEnumerator enumerator = Enumerator();
			enumerator(widgets->prop_ed);
		}
	}
}

bool PropertyStringEnum::UpdateWidget()
{
	if (widgets) widgets->CheckChange();
	return false;
};

void PropertyStringEnum::SetSkipWidget(RolloutWidget* parent, bool set)
{
	parent->SetSkipChild(widgets, set);
}

void PropertyStringEnum::PrepareWidgets(PropertiesOwner* object)
{			
	widgets->owner = object;
	widgets->StartEdit((string*)GetPtr(object), GetScriptName());
}
#endif