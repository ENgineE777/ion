
#include "Core/Core.h"

#ifdef EDITOR

void PropertyCallback::Load(IStreamLoader* loader, PropertiesOwner* object)
{	
}

void PropertyCallback::Save(IStreamSaver* saver, PropertiesOwner* object)
{	
}

void PropertyCallback::Copy(PropertiesOwner* object, PropertiesOwner* sec_object)
{
}

void PropertyCallback::EmbedingScript(const char* script_class_name)
{	
}

bool PropertyCallback::IsEqual(PropertiesOwner* object, PropertiesOwner* sec_object)
{
	return true;	
}

void PropertyCallback::CreateWidget(RolloutWidget* parent, const char* script_class_name)
{
	widgets = (PropertyCallbackWidget*)PropertyBaseWidgetsHolder::GetWidget(script_class_name, GetName());

	if (!widgets)
	{
		widgets = new PropertyCallbackWidget(parent);
		PropertyBaseWidgetsHolder::AddWidget(widgets, script_class_name, GetName());
		widgets->InitControls(Callback(), GetName());		
		widgets->SetSize(220, WidgetHeight());
		parent->AddRolloutChild(GetCategory(), widgets);
	}
}

bool PropertyCallback::UpdateWidget()
{
	if (widgets) widgets->CheckClick();
	return false;
}

void PropertyCallback::SetSkipWidget(RolloutWidget* parent, bool set)
{
	parent->SetSkipChild(widgets, set);
}

void PropertyCallback::PrepareWidgets(PropertiesOwner* object)
{
	widgets->StartEdit(object);
}

#endif