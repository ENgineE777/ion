
#include "Core/Core.h"

void PropertyInt::Load(IStreamLoader* loader, PropertiesOwner* object)
{
	int value;
	if (loader->Read(GetName(), value))
	{
		*((int*)GetPtr(object)) = value;
	}
}

void PropertyInt::Save(IStreamSaver* saver, PropertiesOwner* object)
{
	int value = *((int*)GetPtr(object));
	saver->Write(GetName(), value);
}

void PropertyInt::Copy(PropertiesOwner* object, PropertiesOwner* sec_object)
{
	*((int*)GetPtr(sec_object)) = *((int*)GetPtr(object));
}

void PropertyInt::EmbedingScript(const char* script_class_name)
{		
	static char prop_name[128];
	sprintf(prop_name, "int %s", GetScriptName());
	core->Script()->Machine()->RegisterObjectProperty(script_class_name, prop_name, GetOffset());
}

bool PropertyInt::IsEqual(PropertiesOwner* object, PropertiesOwner* sec_object)
{
	return *((int*)GetPtr(object)) == *((int*)GetPtr(sec_object));
}

#ifdef EDITOR

void PropertyInt::CreateWidget(RolloutWidget* parent, const char* script_class_name)
{
	widgets = (PropertyIntWidget*)PropertyBaseWidgetsHolder::GetWidget(script_class_name, GetName());

	if (!widgets)
	{
		widgets = new PropertyIntWidget(parent);
		PropertyBaseWidgetsHolder::AddWidget(widgets, script_class_name, GetName());
		widgets->InitControls();
		widgets->SetSize(220, WidgetHeight());
		parent->AddRolloutChild(GetCategory(), widgets);
	}
}

bool PropertyInt::UpdateWidget()
{
	if (widgets) widgets->CheckChange();
	return false;
};

void PropertyInt::SetSkipWidget(RolloutWidget* parent, bool set)
{
	parent->SetSkipChild(widgets, set);
}

void PropertyInt::PrepareWidgets(PropertiesOwner* object)
{			
	widgets->owner = object;
	widgets->applyTask.Init(this, widgets);
	widgets->StartEdit((int*)GetPtr(object), GetScriptName());
}
#endif