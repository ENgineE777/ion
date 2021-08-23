#pragma once

#include "NGUIWidget.h"

class NGUIButtonInstances : public NGUIWidget
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
	
	STRING_PROP(NGUIButtonInstances, funcName, "", "Events", "OnMouseDown")
	STRING_PROP(NGUIButtonInstances, funcNameMove, "", "Events", "OnMouseMove")
	STRING_PROP(NGUIButtonInstances, funcNameUp, "", "Events", "OnMouseUp")
	STRING_PROP(NGUIButtonInstances, funcNameHighlightOn, "", "Events", "OnHighlightOn")
	STRING_PROP(NGUIButtonInstances, funcNameHighlightOff, "", "Events", "OnHighlightOff")

	FLOAT_PROP(NGUIButtonInstances, border, 0.0f, "Properties", "Border")

	ENUM_PROP(NGUIButtonInstances, eventType, 1, "Properties", "EventType")
		ENUM_ITEM("tap", 0)
		ENUM_ITEM("down", 1)		
	ENUM_END(NGUIButtonInstances, eventType)		
	
	ENTITY_CONSTRUCT(NGUIButtonInstances,NGUIWidget)
	ENTITY_SCRIPTDEC

	ScriptFunction callback_ms_down;
	ScriptFunction callback_ms_move;
	ScriptFunction callback_ms_up;
	ScriptFunction callback_ms_hgl_on;
	ScriptFunction callback_ms_hgl_off;	

	virtual void RegisterProperties();
	virtual void Init();	

	virtual bool Start();
	virtual void Finish();	

	virtual void MethodsToScript(const char* script_class_name);	
		
	void NBtnInstClearInstancesScript();
	void NBtnInstResizeInstancesScript(int size);
	int  NBtnInstGetInstancesCountScript();
	void NBtnInstAddInstanceScript(float x, float y, float width, float height);
	void NBtnInstSetInstanceScript(int index, float x, float y, float width, float height);
	void NBtnInstDelInstanceScript(int index);

	void NBtnInstScriptSetDelegate(asIScriptGeneric *gen);

	void CheckPress(float dt, int level);

	virtual void CheckMouseCover();

	virtual float GetBorderVert();
	virtual float GetBorderHorz();

	void CheckInstance();
	bool CheckSelInstance();

#ifdef EDITOR
	virtual Color GetDebugColor();	
#endif	

	virtual void Release();
};
