#pragma once

#include "NGUIWidget.h"
#include "NGUIFont.h"

#ifdef EDITOR

#include "Platforms/PC/GUI/ComboBoxWidget.h"
extern void EditorFontEnumerator(ComboBoxWidget* widget);
extern void EditorFontEntityEnumerator(ComboBoxWidget* widget);
extern void EditorGUITexrtureEnumerator(ComboBoxWidget* widget);
extern void StartEditLabelGUITexture(PropertiesOwner* owner);

#endif
#include "TextProcessing.h"

class NGUILabel : public NGUIWidget
{		
    enum EMenuId{eMenuCopy};
   
   TextProcessing::Selection selection;
   TextProcessing::Cursor*   pDraggedCursor;

    bool     in_drag;
    float    nDraggDpos;
    float    nTouchTimer;

	Texture* cam_texture;

public:	
	
	TextureEntry::Slice slice;

	PROP_START
    BOOL_PROP(NGUILabel, filled, false, "Visual", "filled")
    COLOR_PROP(NGUILabel, filled_color, 0xffffffff, "Visual", "filled_color")
    FLOAT_PROP(NGUILabel, filled_alpha, 1.0f, "Visual", "filled_alpha")
	
	INT_PROP(NGUILabel, num_visible_lines, -1, "Text", "num_visible_lines")
    BOOL_PROP(NGUILabel,selection_enable,true,"Text", "selection_enable" )
    INT_PROP(NGUILabel, text_cursor_width, 10, "Text", "text_cursor_width")
    INT_PROP(NGUILabel, text_cursor_height, 10, "Text", "text_cursor_height")
    COLOR_PROP(NGUILabel, cursor_Color, 0xffe16f14, "Text", "cursorColor")
    COLOR_PROP(NGUILabel, selection_Color, 0xffe16f14, "Text", "selection_color")

	BOOL_PROP(NGUILabel, fontUseEntity, false, "Font", "UseEntity")

	STRING_ENUM_PROP(NGUILabel, fontEntityName, "", "Font", "Font", true, EditorFontEntityEnumerator)
	
	FONT_PROP( NGUILabel, fontDesc, "Font", "Font" )

	FLOAT_PROP(NGUILabel, fontScale, 1.0f, "Font", "fontScale")

	BOOL_PROP(NGUILabel, textureUseEntity, false, "Texture", "UseEntity2")

	STRING_ENUM_PROP(NGUILabel, textureEntityName, "", "Texture", "Texture", true, EditorGUITexrtureEnumerator)

#ifdef EDITOR
	CALLBACK_PROP(NGUILabel, texEdit, StartEditLabelGUITexture, "Texture", "Edit")
#endif

	FILENAME_PROP(NGUILabel, texMaskName, "", "Texture", "mask", "*.*")

	FLOAT_PROP(NGUILabel, url_fade_time, 0.5f, "Texture", "URL_fade_in")				
	
	STRING_PROP(NGUILabel, url_load_result, "", "Events", "OnTexFromURLResult")

#ifdef EDITOR
	CALLBACK_PROP( NGUILabel, generateFont, null, "Tools", "GenerateFont")
#endif
	
	ScriptFunction callback_tex_loaded;

	Color font_clr, font_shd_clr;
	bool  need_release_font;
	
	NGUIFont* font_entity;	

	string visual_text;		

	ENTITY_CONSTRUCT(NGUILabel,NGUIWidget)
	ENTITY_SCRIPTDEC

	virtual void RegisterProperties();
	virtual void Init();
	virtual void ReInit();
	virtual void RenderWidget();	
	virtual void MethodsToScript(const char* script_class_name);		

	virtual bool Start();
	virtual void Finish();	
    virtual void SetFocused(bool focus);
    virtual bool IsHitted(float mx, float my);
	
	virtual void AddPrintCall(FontInstance* font, float set_scr_x, float set_scr_y, float set_scr_w, float set_scr_h, std::vector<FontRes::LineData>* line_indices, float font_scale, Color color, const char* txt_text);	

	virtual float NWgtGetCurTextHeight();	

	void NLbSetTextureScript(std::string& name);
	void NLbSetTextureURLScript(std::string& url);	
	void NLbGetTextureFileNameScript(std::string& name);
	
	bool NLbIsTextureLoading();	

    void  NLbSetSelectionEnable(bool bSet);
    void  NLbSetCursorsColor(float r,float g,float b,float a);
    void  NLbSetSelectionColor(float r,float g,float b,float a);

    void  NLbSetFilledScript(bool bSet);
    bool  NLbGetFilledScript();
    void  NLbSetFilledColorScript(float r, float g, float b,float a);
    void  NLbSetFilledColorStringScript(std::string& txt);
    void  NLbGetFilledColorScript(float& r, float& g, float& b,float& a);
	
	void  NLbSetFontColorScript(float r, float g, float b);
	void  NLbGetFontColorScript(float& r, float& g, float& b);
	void  NLbSetFontShadowColorScript(float r, float g, float b);
	void  NLbPauseTextureScript(bool pause);
	void  NLbRestartTextureScript();	
	float NLbGetTextureDurationScript();
	int   NLbGetTextureWidth();
	int   NLbGetTextureHeight();	
	void  NLbSetFontName(string& name, bool bold, bool italic, int size, float gap, float outline);
	void  NLbSetFontSize(int size);

	void  NLbEnableTextureCameraFeed(bool set_cam_feed);

	void  NLbScriptSetDelegate(asIScriptGeneric *gen);

	virtual void CalcLinesData();

	void  NLbScriptSetFontScale(float s);
	float NLbScriptGetFontScale();
    
	virtual bool AllowMouseCover();
	virtual void GetClipRect(float& clip_x, float& clip_y, float& clip_x2, float& clip_y2);				
	
#ifdef EDITOR
	virtual void SetEditMode(bool ed);
	virtual Color GetDebugColor();	
#endif	



	virtual void Release();

	inline void CalcColors();	    

	void NLbSetScriptTextureUV(float u, float v, float du, float dv);
	void NLbSetScriptMaskUV(float u, float v, float du, float dv);
};

