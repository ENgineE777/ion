
#include "Core/Core.h"

void PropertyBool::Load(IStreamLoader* loader, PropertiesOwner* object)
{
	bool value;
	if (loader->Read(GetName(), value))
	{
		*((bool*)GetPtr(object)) = value;
	}
}

void PropertyBool::Save(IStreamSaver* saver, PropertiesOwner* object)
{
	bool value = *((bool*)GetPtr(object));
	saver->Write(GetName(), value);
}

void PropertyBool::Copy(PropertiesOwner* object, PropertiesOwner* sec_object)
{
	*((bool*)GetPtr(sec_object)) = *((bool*)GetPtr(object));
}

void PropertyBool::EmbedingScript(const char* script_class_name)
{		
	static char prop_name[128];
	sprintf(prop_name, "bool %s", GetScriptName());
	core->Script()->Machine()->RegisterObjectProperty(script_class_name, prop_name, GetOffset());
}

bool PropertyBool::IsEqual(PropertiesOwner* object, PropertiesOwner* sec_object)
{
	return *((bool*)GetPtr(object)) == *((bool*)GetPtr(sec_object));
}

#ifdef EDITOR

void PropertyBool::CreateWidget(RolloutWidget* parent, const char* script_class_name)
{
	widgets = (PropertyBoolWidget*)PropertyBaseWidgetsHolder::GetWidget(script_class_name, GetName());

	if (!widgets)
	{
		widgets = new PropertyBoolWidget(parent);
		PropertyBaseWidgetsHolder::AddWidget(widgets, script_class_name, GetName());		
		widgets->InitControls();
		widgets->SetSize(220, WidgetHeight());
		parent->AddRolloutChild(GetCategory(), widgets);
	}
}

bool PropertyBool::UpdateWidget()
{
	if (widgets) widgets->CheckChange();
	return false;
};

void PropertyBool::SetSkipWidget(RolloutWidget* parent, bool set)
{
	parent->SetSkipChild(widgets, set);
}

void PropertyBool::PrepareWidgets(PropertiesOwner* object)
{			
	widgets->owner = object;
	widgets->StartEdit((bool*)GetPtr(object), GetScriptName());
}
#endif