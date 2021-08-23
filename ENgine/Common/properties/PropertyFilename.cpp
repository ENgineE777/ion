
#include "Core/Core.h"

void PropertyFilename::Load(IStreamLoader* loader, PropertiesOwner* object)
{
	char value[1024];
	if (loader->Read(GetName(), value, 1024))
	{
		((string*)GetPtr(object))->assign(value);
	}
}

void PropertyFilename::Save(IStreamSaver* saver, PropertiesOwner* object)
{	
	saver->Write(GetName(), ((string*)GetPtr(object))->c_str());
}

void PropertyFilename::Copy(PropertiesOwner* object, PropertiesOwner* sec_object)
{
	((string*)GetPtr(sec_object))->assign(((string*)GetPtr(object))->c_str());
}

void PropertyFilename::EmbedingScript(const char* script_class_name)
{			
}

bool PropertyFilename::IsEqual(PropertiesOwner* object, PropertiesOwner* sec_object)
{
	return String::IsEqual(((string*)GetPtr(object))->c_str(), ((string*)GetPtr(sec_object))->c_str());
}

#ifdef EDITOR

void PropertyFilename::CreateWidget(RolloutWidget* parent, const char* script_class_name)
{
	widgets = (PropertyFilenameWidget*)PropertyBaseWidgetsHolder::GetWidget(script_class_name, GetName());

	if (!widgets)
	{
		widgets = new PropertyFilenameWidget(parent);
		PropertyBaseWidgetsHolder::AddWidget(widgets, script_class_name, GetName());
		widgets->InitControls(GetFIletype());
		widgets->SetSize(220, WidgetHeight());
		parent->AddRolloutChild(GetCategory(), widgets);
	}
}

bool PropertyFilename::UpdateWidget()
{
	if (widgets) widgets->CheckChange();
	return false;
};

void PropertyFilename::SetSkipWidget(RolloutWidget* parent, bool set)
{
	parent->SetSkipChild(widgets, set);
}

void PropertyFilename::PrepareWidgets(PropertiesOwner* object)
{			
	widgets->owner = object;
	widgets->StartEdit((string*)GetPtr(object), GetScriptName());
}
#endif