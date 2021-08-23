#pragma once

#include "NGUIWidget.h"

#ifdef PC
#include "Platforms/PC/GUI/ComboBoxWidget.h"
extern void EditorFontEnumerator(ComboBoxWidget* widget);
#endif

class NGUICalendar : public NGUIWidget
{		
public:		
		
	struct Str64
	{
		char txt[64];
	};

	Str64 month_str[12];
	Str64 day_str[7];	

	PROP_START
	BOOL_PROP(NGUICalendar, allow_past, true, "Properties", "AllowPast")
	BOOL_PROP(NGUICalendar, auto_slect_cur_date, false, "Properties", "AutoSelectCurDate")
	BOOL_PROP(NGUICalendar, allow_scroll, true, "Properties", "allowScroll")

	ENUM_PROP(NGUICalendar, sel_theme, 0, "Visual", "Theme")		
		ENUM_ITEM("Theme1", 0)
		ENUM_ITEM("Theme2", 1)		
	ENUM_END(NGUICalendar, sel_theme)

	BOOL_PROP(NGUICalendar, show_week_days, true, "Visual", "ShowWeekDays")

	FONT_PROP( NGUICalendar, topFontDesc, "TopFont", "Visual" )
	FONT_PROP( NGUICalendar, captionFontDesc, "CaptionFont", "Visual" )
	FONT_PROP( NGUICalendar, cellFontDesc, "cellFont", "Visual" )
	
	COLOR_PROP(NGUICalendar, fontCellColor, 0xffffffff, "Visual", "CellFontColor")	
	COLOR_PROP(NGUICalendar, fontCellSelColor, 0xffffffff, "Visual", "CellFontSelColor")	
	COLOR_PROP(NGUICalendar, fontCellPrevColor, 0xaaaaaaaa, "Visual", "CellPrevFontColor")	

	FLOAT_PROP(NGUICalendar, topSize, 32.0f, "Visual", "topSize")
	FLOAT_PROP(NGUICalendar, captionSize, 32.0f, "Visual", "captionSize")
	FLOAT_PROP(NGUICalendar, cellSize, 60.0f, "Visual", "cellSize")
	FLOAT_PROP(NGUICalendar, monthGap, 60.0f, "Visual", "monthGap")
	FLOAT_PROP(NGUICalendar, nextBtnSize, 32.0f, "Visual", "nextBtnSize")	
	FLOAT_PROP(NGUICalendar, markerSize, 32.0f, "Visual", "markerSize")

	FILENAME_PROP(NGUICalendar, topTexName, "", "Visual", "topTexName", "*.*")	
	COLOR_PROP(NGUICalendar, topTexColor, 0xffffffff, "Visual", "topTexColor")

	FILENAME_PROP(NGUICalendar, lineTexName, "", "Visual", "lineTexName", "*.*")
	COLOR_PROP(NGUICalendar, lineTexColor, 0xffffffff, "Visual", "lineTexColor")	

	FILENAME_PROP(NGUICalendar, cellTexName, "", "Visual", "cellTexName", "*.*")
	COLOR_PROP(NGUICalendar, cellTexColor, 0xffffffff, "Visual", "cellTexColor")	

	FILENAME_PROP(NGUICalendar, cellSelTexName, "", "Visual", "cellSelTexName", "*.*")
	COLOR_PROP(NGUICalendar, cellSelTexColor, 0xffffffff, "Visual", "cellSelTexColor")

	FILENAME_PROP(NGUICalendar, cellMarkedTexName, "", "Visual", "cellMarkedTexName", "*.*")
	COLOR_PROP(NGUICalendar, cellMarkedTexColor, 0xffffffff, "Visual", "cellMarkedTexColor")

	FILENAME_PROP(NGUICalendar, cellMarkedPrevTexName, "", "Visual", "cellMarkedPrevTexName", "*.*")				
	COLOR_PROP(NGUICalendar, cellMarkedPrevTexColor, 0xffffffff, "Visual", "cellMarkedPrevTexColor")

	STRING_PROP(NGUICalendar, funcDaySelect, "", "Events", "funcDaySelect")	
		
	void NClScriptSetDelegate(asIScriptGeneric *gen);

	TextureInstance* topTex;	
	TextureInstance* lineTex;
	TextureInstance* cellTex;
	TextureInstance* cellSelTex;
	TextureInstance* cellMarkedTex;
	TextureInstance* cellMarkedPrevTex;
	
	ScriptFunction callback;	

	struct CacheDay
	{
		int  day;
		int  month;
		int  year;
		char txt[4];
		int  marked;
		int  forbided;
	};

	int cur_day;
	int cur_month;
	int cur_year;

	int sel_day;
	int sel_month;
	int sel_year;

	struct Date
	{
		int day;
		int month;
		int year;
	};

	bool  in_scroll;
	bool  is_draged;
	int   touchType;
	float childs_offset;
	int   cur_page;
	int   target_page;
	float ms_pos;
	int   day, month, year;	

	NGUIWidget::ScrollerData scroller;

	std::vector<Date> marked_dates;
	std::vector<Date> forbided_dates;

	int scroll_action;

	struct MonthChache
	{
		int num_lines;
		float offset;
		CacheDay days[6 * 7];
	};

	MonthChache months[12];

	NGUIWidget* sub_widget;

	ENTITY_CONSTRUCT(NGUICalendar,NGUIWidget)
	ENTITY_SCRIPTDEC

	virtual void RegisterProperties();
	virtual void Init();
	virtual void ReInit();
	virtual bool Start();	
	virtual void Finish();

	virtual void RenderWidget();
	void RenderMonth(float offset, MonthChache* cache);
	void RenderMonth2(MonthChache* cache);
	virtual void MethodsToScript(const char* script_class_name);
			
	void CheckPress(float dt, int level);
	void LoadTexture(TextureInstance* &tex, const char* name);

	int NClGetDaysInMonth(int year, int month);

	void NClGetSelectedDate(int& day, int& month, int& year);
	void NClSetSelectedDate(int day, int month, int year);
	void NClMarkDate(bool mark, int day, int month, int year);
	bool NClIsDayMarked(int day, int month, int year);
	void NClClearMarkedDays();
	void NClSetCurrentDate(int day, int month, int year);

	void NClAddForbidedDate(bool add, int day, int month, int year);
	bool NClIsDayForbided(int day, int month, int year);
	void NClClearForbidedDays();

	int  NClGetWeekOfDay(int day, int month, int year);
	void NClGetNextDayScript(int day, int month, int year, int& out_day, int& out_month, int& out_year);
	void NClGetPrevDayScript(int day, int month, int year, int& out_day, int& out_month, int& out_year);

	void NClSetMonthTextScript(int index, string& text);
	void NClSetDayTextScript(int index, string& text);

	void NClSetTopFontColorScript(float r, float g, float b);
	void NClSetCaptionFontColorScript(float r, float g, float b);
	void NClSetCellFontColorScript(float r, float g, float b);	
	void NClSetCellSelFontColorScript(float r, float g, float b);	
	void NClSetCellPrevFontColorScript(float r, float g, float b);	

	void NClSetTopTexColorScript(float r, float g, float b);
	void NClSetLineTexColorScript(float r, float g, float b);
	void NClSetCellTexColorScript(float r, float g, float b);
	void NClSetCellSelTexColorScript(float r, float g, float b);
	void NClSetMarkedTexColorScript(float r, float g, float b);
	void NClSetMarkedPrevTexColorScript(float r, float g, float b);
	
	void CacheDays();
	void CacheDays(int year, int month, MonthChache* cache);

	//virtual void ModifyTextClipRect(ClipRect& rect);

	virtual void Release();
};