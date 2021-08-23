#pragma once

#include "NGUIWidget.h"
#include "NGUIFont.h"

#ifdef EDITOR

#include "Platforms/PC/GUI/ComboBoxWidget.h"
extern void EditorFontEnumerator(ComboBoxWidget* widget);
extern void EditorFontEntityEnumerator(ComboBoxWidget* widget);
extern void EditorGUITexrtureEnumerator(ComboBoxWidget* widget);
extern void StartEditLabelExtGUITexture(PropertiesOwner* owner);

#endif
#include "TextProcessing.h"

class NGUILabelRich : public NGUIWidget 
{	
    enum EMenuId{eMenuCopy};

    bool  need_release_font;
    Color font_clr, font_shd_clr;
    NGUIFont* font_entity;
	string text;
    TextProcessing::RichText  label;
    static TextProcessing::RichText  gBuff;

    TextProcessing::Selection        selection;
    TextProcessing::Cursor*          pDraggedCursor;

    bool            in_drag;
    float           nDraggDpos;
    float           nTouchTimer;
	bool            need_set_label;

    void SetToBuff(const string& txt);


    void CheckPress(float dt, int level);
    void ParseText(); 

public:			

	PROP_START
    BOOL_PROP(NGUILabelRich, filled, false, "Visual", "filled")
    COLOR_PROP(NGUILabelRich, filled_color, 0xffffffff, "Visual", "filled_color")
    FLOAT_PROP(NGUILabelRich, filled_alpha, 1.0f, "Visual", "filled_alpha")

    BOOL_PROP(NGUILabelRich, find_links_enable,false,"Text", "find_links_enable" )
    COLOR_PROP(NGUILabelRich, links_color,0xffff0000, "Text", "links_color")
    STRING_PROP(NGUILabelRich, links_callback, "", "Text", "links_callback")
	INT_PROP(NGUILabelRich, num_visible_lines, -1, "Text", "num_visible_lines")
    BOOL_PROP(NGUILabelRich,selection_enable,true,"Text", "selection_enable" )
    INT_PROP(NGUILabelRich, text_cursor_width, 10, "Text", "text_cursor_width")
    INT_PROP(NGUILabelRich, text_cursor_height, 10, "Text", "text_cursor_height")
    COLOR_PROP(NGUILabelRich, cursorColor, 0xffff0000, "Text", "cursorColor")
    COLOR_PROP(NGUILabelRich, selection_color, 0xffff0000, "Text", "selection_color")

	BOOL_PROP(NGUILabelRich, fontUseEntity, false, "Font", "UseEntity")

	STRING_ENUM_PROP(NGUILabelRich, fontEntityName, "", "Font", "Font", true, EditorFontEntityEnumerator)

	FONT_PROP( NGUILabelRich, fontDesc, "Font", "Font" )

	FLOAT_PROP(NGUILabelRich, fontScale, 1.0f, "Font", "fontScale")			

#ifdef EDITOR
	CALLBACK_PROP( NGUILabelRich, generateFont, null, "Tools", "GenerateFont")
#endif

    ENTITY_CONSTRUCT(NGUILabelRich,NGUIWidget)
	ENTITY_SCRIPTDEC

	virtual void  RegisterProperties();
	virtual void  Init();
	virtual void  ReInit();
	virtual void  RenderWidget();	
	virtual void  MethodsToScript(const char* script_class_name);		

	virtual bool  Start();
    virtual void  SetFocused(bool focus);

    virtual bool  IsHitted(float mx, float my);
    virtual bool  AllowMouseCover();
    virtual void  GetClipRect(float& clip_x, float& clip_y, float& clip_x2, float& clip_y2);				
#ifdef EDITOR
    virtual void  SetEditMode(bool ed);
    virtual Color GetDebugColor();	
#endif	
    virtual void  Release();
    inline  void  CalcColors();	  
    virtual void  SetText(const char* txt);

    virtual void CalcLinesData();    

    float    NWgtGetCurTextWidth();
    float    NWgtGetCurTextHeight();
    int      NWgtGetCurTextLinesCount();	

    float    GetTextWidth(std::string& txt);	
    float    GetTextHeight(std::string& txt);	
    int      GetTextLinesCount(std::string& txt);	

    void     NLBESetSelectionEnable(bool bSet);
    void     NLBESetFindLinksEnable(bool bSet);
    bool     NLBEGetFindLinksEnable(){ return find_links_enable;}

    void     NLBESetCursorsColor(float r,float g,float b,float a);
    void     NLBESetSelectionColor(float r,float g,float b,float a);

    void     NLBEScriptSetFontScale(float s);
    float    NLBEScriptGetFontScale();
    void     NLBESetFontColorScript(float r, float g, float b);
    void     NLBEGetFontColorScript(float& r, float& g, float& b);
    void     NLBESetFontShadowColorScript(float r, float g, float b);
    void     NLBESetLinksColorScript(float r, float g, float b);
    void     NLBEGetLinksColorScript(float& r, float& g, float& b);
	
	void     NLBESetFontName(string& name, bool bold, bool italic, int size, float gap, float outline);
	void     NLBESetFontSize(int size);
    
    int      NLBECheckLinks(string& text);
    int      NLBECheckLinksId();

    void    NLBESetFilledScript(bool bSet);
    bool    NLBEGetFilledScript();
    void    NLBESetFilledColorScript(float r, float g, float b,float a);
    void    NLBESetFilledColorStringScript(std::string& txt);
    void    NLBEGetFilledColorScript(float& r, float& g, float& b,float& a);

};

