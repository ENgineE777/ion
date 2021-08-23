#pragma once

#include "NGUIWidget.h"

class NGUIScroller : public NGUIWidget
{				
	NGUIWidget::ScrollerData scroller;	
    bool                     onStartRefreshFuncCall;
    bool                     onRubberStopInRefreshModeFuncCall;
    struct ScrollerMover
    {
    private:
        float                    velocity;
        float                    target;
        float                    timer;  
        float                    delta;
        inline void    Reset(){velocity = target = timer = delta = 0;}

    public:
        ScrollerMover():velocity(0),target(0),timer(0),delta(0){}
        inline bool IsWorking(){return velocity != 0;} 
        inline bool IsAllowScroll(){return delta <= 30.f;} 
        inline void Stop(){Reset();} 
        void Init(float _target,const NGUIWidget::ScrollerData& scroller);
        void Update(float dt,NGUIWidget::ScrollerData& scroller);
    };
    ScrollerMover scrollerMover;

public:	

	PROP_START

	ENUM_PROP(NGUIScroller, scrollerType, 0, "Properties", "Type")
		ENUM_ITEM("horizontal", 0)
		ENUM_ITEM("vertical", 1)		
	ENUM_END(NGUIScroller, scrollerType)
	
	BOOL_PROP(NGUIScroller, allow_scroll, true, "Properties", "Allow_Scroll")

	BOOL_PROP(NGUIScroller, allow_rubber, true, "Properties", "Allow_Rubber")

	INT_PROP(NGUIScroller, whole_scroll_size, 100, "Properties", "scrollSize")

	FILENAME_PROP(NGUIScroller, scroll_back_Name, "", "Visual", "scrollBack", "*.*")
	FILENAME_PROP(NGUIScroller, scroll_thumb_Name, "", "Visual", "scrollThumb", "*.*")

	FLOAT_PROP(NGUIScroller, scroll_thin, 16.0f, "Visual", "scrollThin")
	FLOAT_PROP(NGUIScroller, thumb_size, 50.0f, "Visual", "thumbSize")	


    FLOAT_PROP(NGUIScroller,refresh_threshold,40.0f,"Properties","RefreshThreshold")
    FLOAT_PROP(NGUIScroller,refresh_offset,20.0f,"Properties","RefreshOffset")

    STRING_PROP(NGUIScroller, onStartRefreshFunc, "", "Events", "OnStartRefresh")
    STRING_PROP(NGUIScroller, onRubberStopInRefreshModeFunc, "", "Events", "OnRubberStop")

	ENTITY_CONSTRUCT(NGUIScroller,NGUIWidget)
	ENTITY_SCRIPTDEC


	ScriptFunction callback_start_refresh;
	ScriptFunction callback_stop_refresh;	

	TextureInstance* texScrollBack;
	TextureInstance* texScrollThumb;

	virtual void RegisterProperties();
	virtual void Init();
	virtual void ReInit();	
		
	virtual bool Start();
	virtual void Finish();		

	virtual void MethodsToScript(const char* script_class_name);

	void Work(float dt, int level);

	virtual void PostRenderWidget();
	virtual void ExecuteDrawCalls();

	virtual void CalcChildsGlobalMat();

	void  NScScriptSetDelegate(asIScriptGeneric *gen);

	float NScGetPositionScript();
	void  NScSetPositionScript(float set, bool instance);
	bool  NScIsInScrollingScript();	

	void NScSetScrollSize(float sz);
	float NScGetScrollSize();	

    void NScEnableRefreshOffset(bool enable);
	virtual void Release();

#ifdef EDITOR
	virtual Color GetDebugColor();
#endif
};
