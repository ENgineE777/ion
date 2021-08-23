#pragma once

#include "NGUIWidget.h"
#include "NGUIFont.h"
#include "NGUITexture.h"

#ifdef EDITOR

#include "Platforms/PC/GUI/ComboBoxWidget.h"
extern void EditorFontEnumerator(ComboBoxWidget* widget);
extern void EditorFontEntityEnumerator(ComboBoxWidget* widget);
extern void EditorGUITexrtureEnumerator(ComboBoxWidget* widget);
extern void StartEditEditBoxGUITexture(PropertiesOwner* owner);
extern void StartEditEditBoxMenuGUITexture(PropertiesOwner* owner);

#endif

#include "TextProcessing.h"
#include "Core/Mobile/MobileService.h"

class NGUIEditBox : public NGUIWidget , public  MobileService::ClipboardListener
{		
    enum ETextEditStyle{eMultiLine,eOneLine};
    enum EMenuId{eMenuCopy,eMenuCut,eMenuReplace,eMenuSelect,eMenuPaste};

    void  FindCursorPosition(float x,float y,TextProcessing::Cursor* pCursor);
    void  CalculateLocalMousePos(float& local_ms_pos_x,float& local_ms_pos_y);
    void  CalculateLinesAndOffsetY();
    void  CalcTextOffsetY(TextProcessing::Cursor* pCursor);
    void  ReCalculateTextOffsets();
    bool  ChracterIsAllowed(wchar_t chr);
    void  MoveCurretRight(int count);
    void  MoveCurretLeft(int count);
    void  SelectWord();
    void  CheckCurretsVisibility();
    void  CalculateTextClipX(float& min,float& max);

    ClipRect                        border;

    TextProcessing::Selection       selection;
    TextProcessing::Cursor          curret;
    TextProcessing::Cursor*         pDraggedCursor;

   
    float           nDraggDpos;
    float           nTouchTimer;
    float           init_Y;
    float           init_height;
    float           text_max_offset;
    float           text_cur_offsetY;
    float           text_cur_offsetX;
    float           expanded_height;
    int             need_call_callback;
    bool            was_focued;
    bool            in_drag;

    int             menuID;

public:	
	
 
	PROP_START
	STRING_PROP(NGUIEditBox, funcOnFocusGain, "", "Events", "funcOnFocusGain")		
	STRING_PROP(NGUIEditBox, funcOnChange, "", "Events", "funcOnChange")
	STRING_PROP(NGUIEditBox, funcOnReturnKey, "", "Events", "funcOnReturnKey")
	STRING_PROP(NGUIEditBox, funcOnFocusLost, "", "Events", "funcOnFocusLost")				
	
	INT_PROP(NGUIEditBox, max_text, 100, "Text", "max_text")

	STRING_PROP(NGUIEditBox, placeholder_text, "", "Text", "Placeholder")	
	FLOAT_PROP(NGUIEditBox,  placeholder_alpha, 1.0f, "Text", "PlaceholderAlpha")

	INT_PROP(NGUIEditBox, expand_lines, 0, "Text", "expand_lines")				

	ENUM_PROP(NGUIEditBox, keyboardType, 0, "Text", "keyboardType")
		ENUM_ITEM("Text", 0)
		ENUM_ITEM("URL", 1)
		ENUM_ITEM("Number", 2)
		ENUM_ITEM("Phone", 3)
		ENUM_ITEM("Email", 4)
	ENUM_END(NGUIEditBox, keyboardType)

    ENUM_PROP(NGUIEditBox, text_edit_style, eMultiLine, "Text", "text_edit_style")
		ENUM_ITEM("Multiline", eMultiLine)
		ENUM_ITEM("One Line", eOneLine)
	ENUM_END(NGUIEditBox, text_edit_style)

	ENUM_PROP(NGUIEditBox, returnKey, 0, "Text", "returnKey")
		ENUM_ITEM("Default", 0)
		ENUM_ITEM("Next", 1)
		ENUM_ITEM("Done", 2)
		ENUM_ITEM("Go", 3)
		ENUM_ITEM("Search", 4)
	ENUM_END(NGUIEditBox, returnKey)		

	FONT_PROP( NGUIEditBox, fontDesc, "Font", "Font" )

	FLOAT_PROP(NGUIEditBox, fontScale, 1.0f, "Font", "fontScale")

	BOOL_PROP(NGUIEditBox, textureUseEntity, false, "Texture", "UseEntity2")

	STRING_ENUM_PROP(NGUIEditBox, textureEntityName, "", "Texture", "Texture", true, EditorGUITexrtureEnumerator )

#ifdef EDITOR
	CALLBACK_PROP(NGUIEditBox, texEdit, StartEditEditBoxGUITexture, "Texture", "Edit")
#endif



	INT_PROP(NGUIEditBox, text_horz_border, 3, "Visual", "text_horz_border")
	INT_PROP(NGUIEditBox, text_vert_border, 5, "Visual", "text_vert_border")	
	COLOR_PROP(NGUIEditBox, cursor_Color, 0xffe16f14, "Visual", "cursorColor")
    COLOR_PROP(NGUIEditBox, selection_Color, 0xffe16f14, "Visual", "selectionColor")

	COLOR_PROP(NGUIEditBox, overColor, 0xffffffff, "Visual", "overColor")	
		
  	TextureEntry::Slice slice;	

	ScriptFunction callback_focus_gain;
	ScriptFunction callback_focus_lost;
	ScriptFunction callback_focus_retrn_key;
	ScriptFunction callback_change_text;
	
	ENTITY_CONSTRUCT(NGUIEditBox,NGUIWidget)
	ENTITY_SCRIPTDEC

	virtual void    RegisterProperties();
	virtual void    Init();
	virtual void    ReInit();
	virtual void    RenderWidget();	
	virtual void    MethodsToScript(const char* script_class_name);	

	virtual bool    Start();
	virtual void    Finish();	
    virtual void    Release();

	virtual void    SetFocused(bool focus);
    virtual bool    IsHitted(float mx, float my);
    virtual void    UpdateChildsPos();
    virtual float   GetTextHeight(std::string& txt);	
    virtual void    SetText(const char* txt);
    virtual void    ModifyTextClipRect(ClipRect& rect);

    // Script 	
	virtual void    NWgtSetTextScript(const string& txt);	
	virtual void    NWgtGetTextScript(std::string& val);
    virtual void    NWgtScriptSetHeight(float w);
    virtual void    NWgtScriptSetY(float set_y);

	virtual void    NEbSetPlaceholderScript(const string& txt);	
    float           NEbGetExtendedHeightScript();
    void            NEbSetFontColorScript(float r, float g, float b);
    void            NEbSetCursorColorScript(float r, float g, float b);
    void            NEbSetSelectionColor(float r,float g,float b,float a);

	void NEbScriptSetDelegate(asIScriptGeneric *gen);

    virtual void PasteFromClipboard(const char* sBuffer);
    virtual void OnRequestIsClipboardEmpty(int bClipboardEmpty);

#ifdef EDITOR		
	virtual Color   GetDebugColor();	
#endif	

};
