#pragma once

#include "NGUIWidget.h"

class NGUIButton : public NGUIWidget
{
	struct Instance
	{		
		float x, y, width, height;
		float alpha;

		Instance()
		{
			x = y = width = height = 0;
			alpha = 0.0f;
		};
	};

	int sel_inst;
	vector<Instance> instances;
	
	bool in_drag;
	bool is_hightlited;
	float counter;
    bool  hack_skip_update;

public:									
	
	PROP_START	
	STRING_PROP(NGUIButton, funcName, "", "Events", "OnDown")
	STRING_PROP(NGUIButton, funcNameMove, "", "Events", "OnMove")
	STRING_PROP(NGUIButton, funcNameUp, "", "Events", "OnUp")
	STRING_PROP(NGUIButton, funcNameHighlightOn, "", "Events", "OnHighlightOn")
	STRING_PROP(NGUIButton, funcNameHighlightOff, "", "Events", "OnHighlightOff")	

	FLOAT_PROP(NGUIButton, border, 0.0f, "Properties", "Border")

	BOOL_PROP(NGUIButton, mouseup_always, false, "Properties", "TrigerAlwaysMUP")	

	ENUM_PROP(NGUIButton, eventType, 1, "Properties", "EventType")
		ENUM_ITEM("tap", 0)
		ENUM_ITEM("down", 1)		
	ENUM_END(NGUIButton, eventType)	
		

	ScriptFunction callback_ms_down;
	ScriptFunction callback_ms_move;
	ScriptFunction callback_ms_up;
	ScriptFunction callback_ms_hgl_on;
	ScriptFunction callback_ms_hgl_off;	


	ENTITY_CONSTRUCT(NGUIButton,NGUIWidget)
	ENTITY_SCRIPTDEC
	
	virtual void RegisterProperties();
	virtual void Init();	

	virtual bool Start();
	virtual void Finish();	

	virtual void MethodsToScript(const char* script_class_name);				

	void CheckPress(float dt, int level);

	virtual void CheckMouseCover();

	virtual float GetBorderVert();
	virtual float GetBorderHorz();	

	void NBtnScriptSetDelegate(asIScriptGeneric *gen);

	void CheckInstance();

#ifdef EDITOR
	virtual Color GetDebugColor();	
#endif	

	virtual void Release();
};
