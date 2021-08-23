#pragma once

#include "NGUIWidget.h"

class NGUIPager : public NGUIWidget
{		
	int   cur_page;
	bool  is_draged;
	
	int   touchType;
		
	bool  in_scroll;
	int   target_page;
	float ms_pos;
	float cur_page_float;

	struct ChildPos
	{
		float child_pos;
		NGUIWidget* child;
	};

	ChildPos childs_pos[64];

	int border_v;
	int border_h;

	int call_func_cur_page;
	int call_func_target_page;
	bool call_func;
	bool call_funcEndScroll;

public:

	float childs_offset;

	PROP_START

	ENUM_PROP(NGUIPager, pagerType, 0, "Properties", "Type")
		ENUM_ITEM("horizontal", 0)
		ENUM_ITEM("vertical", 1)		
	ENUM_END(NGUIPager, pagerType)
	
	BOOL_PROP(NGUIPager, allow_scroll, true, "Properties", "allowScroll")

	INT_PROP(NGUIPager, border, 0, "Properties", "Border")

	INT_PROP(NGUIPager, num_pages, 2, "Properties", "numPages")

	FILENAME_PROP(NGUIPager, texDotName, "", "Visual", "dotTexture", "*.*")
	FILENAME_PROP(NGUIPager, texCurDotName, "", "Visual", "curDotTexture", "*.*")
	COLOR_PROP(NGUIPager, dot_color, 0xffffffff, "Visual", "dotColor")

	FLOAT_PROP(NGUIPager, dot_offset, 0, "Visual", "dotOffset")	
	FLOAT_PROP(NGUIPager, dot_size, 16, "Visual", "dotSize")	
	FLOAT_PROP(NGUIPager, dot_distance , 32, "Visual", "dotDistance")		
	
	BOOL_PROP(NGUIPager, exclusiveMouse, true, "Properties", "exclusiveMouse")	
	BOOL_PROP(NGUIPager, allowRubberBorder, true, "Properties", "allowRubberBorder")
	STRING_PROP(NGUIPager, funcName, "", "Events", "FuncNameOnStartScroll")
	STRING_PROP(NGUIPager, funcNameEndScroll, "", "Events", "FuncOnEndScroll")		

	float anim_time;

	TextureInstance* texDot;
	TextureInstance* texCurDot;

	ScriptFunction callback_start;
	ScriptFunction callback_end;

	ENTITY_CONSTRUCT(NGUIPager,NGUIWidget)
	ENTITY_SCRIPTDEC

	virtual void RegisterProperties();
	virtual void Init();
	virtual void ReInit();

	virtual bool Start();
	virtual void Finish();		

	virtual void MethodsToScript(const char* script_class_name);

	void Work(float dt, int level);

	virtual void PostRenderWidget();
	virtual void ExecuteDrawCalls();

	float NPgGetPositionScript();
	void  NPgSetPositionScript(float set, bool instance);		

	virtual float GetBorderVert();
	virtual float GetBorderHorz();

	void NPgScriptSetAllowScroll(bool set_as);
	bool NPgScriptGetAllowScroll();

	void  NPgScriptSetDelegate(asIScriptGeneric *gen);

	virtual void CalcChildsGlobalMat();

#ifdef EDITOR
	virtual Color GetDebugColor();
#endif
	
	void NPgSetDotTexColorScript(float r, float g, float b);
	void NPgScrollTo(int target_page);
	void NPgSetCurPageIndex(int index);

	int NPgGetCurPage()
	{
		return cur_page;
	}

	void CalcAnimTime();

	virtual void Release();
};
