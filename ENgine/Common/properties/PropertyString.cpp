
#include "Core/Core.h"

void PropertyString::Load(IStreamLoader* loader, PropertiesOwner* object)
{
	char value[1024];
	if (loader->Read(GetName(), value, 1024))
	{
		((string*)GetPtr(object))->assign(value);
	}
}

void PropertyString::Save(IStreamSaver* saver, PropertiesOwner* object)
{	
	saver->Write(GetName(), ((string*)GetPtr(object))->c_str());
}

void PropertyString::Copy(PropertiesOwner* object, PropertiesOwner* sec_object)
{
	((string*)GetPtr(sec_object))->assign(((string*)GetPtr(object))->c_str());
}

void PropertyString::EmbedingScript(const char* script_class_name)
{			
}

bool PropertyString::IsEqual(PropertiesOwner* object, PropertiesOwner* sec_object)
{
	return String::IsEqual(((string*)GetPtr(object))->c_str(), ((string*)GetPtr(sec_object))->c_str());
}

#ifdef EDITOR

void PropertyString::CreateWidget(RolloutWidget* parent, const char* script_class_name)
{
	widgets = (PropertyStringWidget*)PropertyBaseWidgetsHolder::GetWidget(script_class_name, GetName());

	if (!widgets)
	{
		widgets = new PropertyStringWidget(parent);
		PropertyBaseWidgetsHolder::AddWidget(widgets, script_class_name, GetName());
		widgets->InitControls();
		widgets->SetSize(220, WidgetHeight());
		parent->AddRolloutChild(GetCategory(), widgets);
	}
}

bool PropertyString::UpdateWidget()
{
	if (widgets) widgets->CheckChange();
	return false;
};

void PropertyString::SetSkipWidget(RolloutWidget* parent, bool set)
{
	parent->SetSkipChild(widgets, set);
}

void PropertyString::PrepareWidgets(PropertiesOwner* object)
{			
	widgets->owner = object;
	widgets->StartEdit((string*)GetPtr(object), GetScriptName());
}
#endif