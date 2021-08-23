#pragma once

/*
#include "NGUIWidget.h"

class NGUITrackBar : public NGUIWidget
{		
public:		
	
	STRING_PROP(NGUITrackBar, skinName, "", "Visual", "skin_Name")	
	STRING_PROP(NGUITrackBar, skinElemName, "", "Visual", "skin_Elem_Name")
	STRING_PROP(NGUITrackBar, skinElemBarName, "", "Visual", "skin_ElemBar_Name")
	COLOR_PROP(NGUITrackBar, trackColor, 0xffffffff, "Visual", "trackColor")

	FILENAME_PROP(NGUITrackBar, thumb_tex_name, "", "Visual", "thumb_tex", "*.*")
	COLOR_PROP(NGUITrackBar, thumbColor, 0xffffffff, "Visual", "thumb_Color")
		
	ENUM_PROP(NGUITrackBar, trackType, 0, "Type")
		ENUM_ITEM("horizontal", 0)
		ENUM_ITEM("vertical", 1)		
	ENUM_END(NGUITrackBar, trackType, false, "Properties")

	FLOAT_PROP(NGUITrackBar, thumb_width, 32, "Visual", "thumb_width")
	FLOAT_PROP(NGUITrackBar, thumb_height, 32, "Visual", "thumb_height")
	FLOAT_PROP(NGUITrackBar, thumb_offset_left, 0, "Visual", "thumb_offset_left")
	FLOAT_PROP(NGUITrackBar, thumb_offset_right, 0, "Visual", "thumb_offset_right")

	INT_PROP(NGUITrackBar, minValue, 1, "Properties", "minValue")
	INT_PROP(NGUITrackBar, maxValue, 10, "Properties", "maxValue")
	INT_PROP(NGUITrackBar, stepValue, 1, "Properties", "stepValue")
	INT_PROP(NGUITrackBar, initValue, 1, "Properties", "initValue")
		
	NGUISkin* skin;	
	int skinElem;
	int skinElemBar;
	bool need_additional_clip;
	

	TextureInstance* thumb_tex;
	int val;	
	bool in_drag;
		
	Vector2 start_pos;
	Vector2 end_pos;
	Vector2 thum_pos;
	Vector2 limit;

	float add_clip_x;
	float add_clip_y;
	float add_clip_x2;
	float add_clip_y2;
	

	ENTITY_CONSTRUCT(NGUITrackBar,NGUIWidget)
	ENTITY_SCRIPTDEC

	virtual void Init();
	virtual void ReInit();
	virtual void RenderWidget();	
	virtual void MethodsToScript();
	
	void SetValue(int set_val);	
	int  GetValue();		

	void CheckPress(float dt, int level);
	void UpdateSkin(float dt, int level);

	virtual void AddDrawCall(TextureInstance* tex, float set_scr_x, float set_scr_y, float wdt, float hgt, Color color, float rotate, float u, float v, float du, float dv);

	virtual void Release();
};
*/