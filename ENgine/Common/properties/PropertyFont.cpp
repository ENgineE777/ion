
#include "Core/Core.h"

void PropertyFont::Load(IStreamLoader* loader, PropertiesOwner* object)
{
	FontParams* value = (FontParams*)GetPtr(object);

	if (loader->EnterBlock(GetName()))
	{
		loader->Read("name", value->name, 128);
		loader->Read("bold", value->bold);
		loader->Read("italic", value->italic);
		loader->Read("height", value->height);
		loader->Read("outline", value->outline);
		loader->Read("gap", value->gap);
		loader->Read("color_r", value->color.r);
		loader->Read("color_g", value->color.g);
		loader->Read("color_b", value->color.b);
		loader->Read("use_shadow", value->use_shadow);
		loader->Read("shd_color_r", value->shd_color.r);
		loader->Read("shd_color_g", value->shd_color.g);
		loader->Read("shd_color_b", value->shd_color.b);

		loader->LeaveBlock();
	}	
}

void PropertyFont::Save(IStreamSaver* saver, PropertiesOwner* object)
{
	FontParams* value = (FontParams*)GetPtr(object);
	
	saver->MarkBeginBlock(GetName());
	
	saver->Write("name", value->name);
	saver->Write("bold", value->bold);
	saver->Write("italic", value->italic);
	saver->Write("height", value->height);
	saver->Write("outline", value->outline);
	saver->Write("gap", value->gap);
	saver->Write("color_r", value->color.r);
	saver->Write("color_g", value->color.g);
	saver->Write("color_b", value->color.b);
	saver->Write("use_shadow", value->use_shadow);
	saver->Write("shd_color_r", value->shd_color.r);
	saver->Write("shd_color_g", value->shd_color.g);
	saver->Write("shd_color_b", value->shd_color.b);
	
	saver->MarkEndBlock(GetName());
}

void PropertyFont::SaveDiff(IStreamSaver* saver, PropertiesOwner* object, PropertiesOwner* origin)
{
   FontParams* pOriginParams = (FontParams*)GetPtr(origin);
   FontParams* pParams = (FontParams*)GetPtr(object);
   saver->MarkBeginBlock(GetName());
   if(!String::IsEqual(pOriginParams->name,pParams->name))saver->Write("name", pParams->name);
   if(pOriginParams->bold != pParams->bold)saver->Write("bold", pParams->bold);
   if(pOriginParams->italic != pParams->italic)saver->Write("italic", pParams->italic);
   if(pOriginParams->height != pParams->height)saver->Write("height", pParams->height);
   if(pOriginParams->outline != pParams->outline)saver->Write("outline", pParams->outline);
   if(pOriginParams->gap != pParams->gap)saver->Write("gap", pParams->gap);
   if(pOriginParams->use_shadow != pParams->use_shadow)saver->Write("use_shadow", pParams->use_shadow);
   if(pOriginParams->color.r != pParams->color.r)saver->Write("color_r", pParams->color.r);
   if(pOriginParams->color.g != pParams->color.g)saver->Write("color_g", pParams->color.g);
   if(pOriginParams->color.b != pParams->color.b)saver->Write("color_b", pParams->color.b);
   if(pOriginParams->shd_color.r != pParams->shd_color.r)saver->Write("shd_color_r", pParams->shd_color.r);
   if(pOriginParams->shd_color.g != pParams->shd_color.g)saver->Write("shd_color_g", pParams->shd_color.g);
   if(pOriginParams->shd_color.b != pParams->shd_color.b)saver->Write("shd_color_b", pParams->shd_color.b);
   saver->MarkEndBlock(GetName());
}



void PropertyFont::Copy(PropertiesOwner* object, PropertiesOwner* sec_object)
{
	((FontParams*)GetPtr(object))->Copy(*((FontParams*)GetPtr(sec_object)));
}

void PropertyFont::EmbedingScript(const char* script_class_name)
{	
}

bool PropertyFont::IsEqual(PropertiesOwner* object, PropertiesOwner* sec_object)
{
	return ((FontParams*)GetPtr(object))->IsEqual(*((FontParams*)GetPtr(sec_object)));
}

#ifdef EDITOR

extern void EditorFontEnumerator(ComboBoxWidget* widget);

void PropertyFont::CreateWidget(RolloutWidget* parent, const char* script_class_name)
{
	widgets = (PropertyFontWidget*)PropertyBaseWidgetsHolder::GetWidget(script_class_name, GetName());

	if (!widgets)
	{
		widgets = new PropertyFontWidget(parent);
		PropertyBaseWidgetsHolder::AddWidget(widgets, script_class_name, GetName());
		widgets->InitControls();
		widgets->SetSize(220, WidgetHeight());
		parent->AddRolloutChild(GetCategory(), widgets);

		EditorFontEnumerator(widgets->prop_cb_name);
	}
}

bool PropertyFont::UpdateWidget()
{
	if (widgets) widgets->CheckChange();
	return false;
};

void PropertyFont::SetSkipWidget(RolloutWidget* parent, bool set)
{
	parent->SetSkipChild(widgets, set);
}

void PropertyFont::PrepareWidgets(PropertiesOwner* object)
{				
	widgets->owner = object;
    widgets->applyTask.Init(this, widgets);
	widgets->StartEdit((FontParams*)GetPtr(object), GetScriptName());
}
#endif