
#include "Core/Core.h"

void PropertyFloat::Load(IStreamLoader* loader, PropertiesOwner* object)
{
	float value;
	if (loader->Read(GetName(), value))
	{
		*((float*)GetPtr(object)) = value;
	}
}

void PropertyFloat::Save(IStreamSaver* saver, PropertiesOwner* object)
{
	float value = *((float*)GetPtr(object));
	saver->Write(GetName(), value);
}

void PropertyFloat::Copy(PropertiesOwner* object, PropertiesOwner* sec_object)
{
	*((float*)GetPtr(sec_object)) = *((float*)GetPtr(object));
}

void PropertyFloat::EmbedingScript(const char* script_class_name)
{		
	static char prop_name[128];
	sprintf(prop_name, "float %s", GetScriptName());
	core->Script()->Machine()->RegisterObjectProperty(script_class_name, prop_name, GetOffset());
}

bool PropertyFloat::IsEqual(PropertiesOwner* object, PropertiesOwner* sec_object)
{
	return *((float*)GetPtr(object)) == *((float*)GetPtr(sec_object));
}

#ifdef EDITOR

void PropertyFloat::CreateWidget(RolloutWidget* parent, const char* script_class_name)
{
	widgets = (PropertyFloatWidget*)PropertyBaseWidgetsHolder::GetWidget(script_class_name, GetName());

	if (!widgets)
	{
		widgets = new PropertyFloatWidget(parent);
		PropertyBaseWidgetsHolder::AddWidget(widgets, script_class_name, GetName());
		widgets->InitControls();
		widgets->SetSize(parent->GetWidth(), WidgetHeight());
		parent->AddRolloutChild(GetCategory(), widgets);
	}
}

bool PropertyFloat::UpdateWidget()
{
	if (widgets) widgets->CheckChange();
	return false;
};

void PropertyFloat::SetSkipWidget(RolloutWidget* parent, bool set)
{
	parent->SetSkipChild(widgets, set);
}

void PropertyFloat::PrepareWidgets(PropertiesOwner* object)
{			
	widgets->owner = object;
	widgets->applyTask.Init(this, widgets);
	widgets->StartEdit((float*)GetPtr(object), GetScriptName());
}
#endif