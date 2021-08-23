
#include "Core/Core.h"

void PropertyColor::Load(IStreamLoader* loader, PropertiesOwner* object)
{
	Color* value = (Color*)GetPtr(object);

	if (loader->EnterBlock(GetName()))
	{
		loader->Read("r", value->r);
		loader->Read("g", value->g);
		loader->Read("b", value->b);
		loader->LeaveBlock();
	}	
}

void PropertyColor::Save(IStreamSaver* saver, PropertiesOwner* object)
{
	Color* value = (Color*)GetPtr(object);
	
	saver->MarkBeginBlock(GetName());
	saver->Write("r", value->r);
	saver->Write("g", value->g);
	saver->Write("b", value->b);
	saver->MarkEndBlock(GetName());
}

void PropertyColor::Copy(PropertiesOwner* object, PropertiesOwner* sec_object)
{
	((Color*)GetPtr(sec_object))->r = ((Color*)GetPtr(object))->r;
	((Color*)GetPtr(sec_object))->g = ((Color*)GetPtr(object))->g;
	((Color*)GetPtr(sec_object))->b = ((Color*)GetPtr(object))->b;
}

void PropertyColor::EmbedingScript(const char* script_class_name)
{	
}

bool PropertyColor::IsEqual(PropertiesOwner* object, PropertiesOwner* sec_object)
{
	return (((Color*)GetPtr(object))->r == ((Color*)GetPtr(sec_object))->r &&
			((Color*)GetPtr(object))->g == ((Color*)GetPtr(sec_object))->g &&
			((Color*)GetPtr(object))->b == ((Color*)GetPtr(sec_object))->b);	
}

#ifdef EDITOR

void PropertyColor::CreateWidget(RolloutWidget* parent, const char* script_class_name)
{
	widgets = (PropertyColorWidget*)PropertyBaseWidgetsHolder::GetWidget(script_class_name, GetName());

	if (!widgets)
	{
		widgets = new PropertyColorWidget(parent);
		PropertyBaseWidgetsHolder::AddWidget(widgets, script_class_name, GetName());
		widgets->InitControls();
		widgets->SetSize(220, WidgetHeight());
		parent->AddRolloutChild(GetCategory(), widgets);
	}
}

bool PropertyColor::UpdateWidget()
{
	if (widgets) widgets->CheckChange();
	return false;
};

void PropertyColor::SetSkipWidget(RolloutWidget* parent, bool set)
{
	parent->SetSkipChild(widgets, set);
}

void PropertyColor::PrepareWidgets(PropertiesOwner* object)
{				
	widgets->owner = object;
	widgets->StartEdit((Color*)GetPtr(object), GetScriptName());
}
#endif