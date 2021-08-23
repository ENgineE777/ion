#pragma once

#include "NGUIWidget.h"
#include "NGUIFont.h"

#ifdef EDITOR
#include "Platforms/PC/GUI/ComboBoxWidget.h"
extern void EditorFontEnumerator(ComboBoxWidget* widget);
extern void EditorFontEntityEnumerator(ComboBoxWidget* widget);
#endif

class NGUITextDrawer : public NGUIWidget
{		
public:		
					
	bool  need_release_font;
	NGUIFont* font_entity;		

	PROP_START

	BOOL_PROP(NGUITextDrawer, fontUseEntity, false, "Font", "UseEntity")

	STRING_ENUM_PROP(NGUITextDrawer, fontEntityName, "", "Font", "Font", true, EditorFontEntityEnumerator)
	
	FONT_PROP( NGUITextDrawer, fontDesc, "Font", "Font" )

	FLOAT_PROP(NGUITextDrawer, fontScale, 1.0f, "Font", "fontScale")


	ENTITY_CONSTRUCT(NGUITextDrawer,NGUIWidget)
	ENTITY_SCRIPTDEC

	virtual void RegisterProperties();
	virtual void Init();
	virtual void ReInit();	
	virtual void MethodsToScript(const char* script_class_name);	

	void NTDScriptAddPrintCall(float s_x, float s_y, std::string& txt_text, float alpha);		
	void NTDScriptAddPrintCall2(float s_x, float s_y, float s_w, float s_h, std::string& txt_text, float alpha);		
	void NTDScriptAddPrintCall3(float s_x, float s_y, float s_w, float s_h, float font_scale, std::string& txt_text, float alpha);	

	virtual void AddPrintCall(FontInstance* font, float set_scr_x, float set_scr_y, float set_scr_w, float set_scr_h, std::vector<int>* line_indices, float font_scale, Color color, const char* txt_text);	
		
	void NTDSetFontColorScript(float r, float g, float b);
	void NTDSetFontShadowColorScript(float r, float g, float b);	
	void NTDSetFontSize(int size);

	virtual bool AllowMouseCover();	
		
#ifdef EDITOR
	virtual Color GetDebugColor();	
#endif	
	
	virtual void Release();	
};

