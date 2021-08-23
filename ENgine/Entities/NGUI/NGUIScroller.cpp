
#include "NGUIScroller.h"

ENTITY(NGUIScroller)
ENTITY_IS_2D_OBJECT
ENTITY_PARAMS(NGUIScroller,"NGUIScroller","NGUI", "Scroller")

void NGUIScroller::RegisterProperties()
{
	NGUIWidget::RegisterProperties();	

	PROP_REG(scrollerType)
	PROP_REG(whole_scroll_size)	

	PROP_REG(allow_scroll)	

	PROP_REG(allow_rubber)		

	PROP_REG(scroll_back_Name)
	PROP_REG(scroll_thumb_Name)	

	PROP_REG(scroll_thin)
	PROP_REG(thumb_size)	

	PROP_REG(refresh_threshold)
	PROP_REG(refresh_offset)

	PROP_REG(onStartRefreshFunc)
	PROP_REG(onRubberStopInRefreshModeFunc)
}

void NGUIScroller::Init()
{			
	NGUIWidget::Init();		
    
	texScrollBack = null;
	texScrollThumb = null;
    onRubberStopInRefreshModeFuncCall = onStartRefreshFuncCall = false;

	SetUpdate((TaskHolder::Task)&NGUIScroller::Work, lvl_camera5 + 3);
   
}
	
void NGUIScroller::ReInit()
{	
	RELEASE(texScrollBack)
	texScrollBack = en_core.Render()->LoadTexture(scroll_back_Name.c_str(), false, _FL_);		

	RELEASE(texScrollThumb)
	texScrollThumb = en_core.Render()->LoadTexture(scroll_thumb_Name.c_str(), false, _FL_);

	NGUIWidget::ReInit();
	
	NScSetScrollSize(whole_scroll_size);
}

bool NGUIScroller::Start()
{
	ScriptFunction::Find2PI(&callback_start_refresh, Scene()->ScriptModule(), onStartRefreshFunc.c_str(), Desc()->GetName());
	ScriptFunction::Find1P(&callback_stop_refresh, Scene()->ScriptModule(), onRubberStopInRefreshModeFunc.c_str(), Desc()->GetName());

	return NGUIWidget::Start();
}

void NGUIScroller::Finish()
{
	callback_start_refresh.Reset();
	callback_stop_refresh.Reset();
}

CALLWRAPPER(NGUIScroller, NScGetPositionScript, CALLWRAPPERRET(float))
CALLWRAPPERARG2(NGUIScroller, NScSetPositionScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(bool,arg2), CALLWRAPPERNORET())

CALLWRAPPER(NGUIScroller, NScIsInScrollingScript, CALLWRAPPERRET(bool))

CALLWRAPPER(NGUIScroller, NScGetScrollSize, CALLWRAPPERRET(float))
CALLWRAPPERARG1(NGUIScroller, NScSetScrollSize, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERNORET())

CALLWRAPPERARG1(NGUIScroller,   NScEnableRefreshOffset , CALLWRAPPERGETPARAM(bool,arg1), CALLWRAPPERNORET())

void CALLWRAPPERNAME(NScScriptSetDelegate)(asIScriptGeneric *gen)
{
	NGUIScroller* obj = (NGUIScroller*)gen->GetObject();
	obj->NScScriptSetDelegate(gen);	
}

void NGUIScroller::MethodsToScript(const char* script_class_name)
{
	NGUIWidget::MethodsToScript(script_class_name);

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "float GetPosition()", CALLWRAPPERNAMEAS(NScGetPositionScript));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void  SetPosition(float set, bool instance)", CALLWRAPPERNAMEAS(NScSetPositionScript));    
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "bool  IsInScrolling()", CALLWRAPPERNAMEAS(NScIsInScrollingScript));
	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "float get_scrollSize()", CALLWRAPPERNAMEAS(NScGetScrollSize));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void  set_scrollSize(float sz)", CALLWRAPPERNAMEAS(NScSetScrollSize));	

    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void EnableRefreshOffset(bool enable)", CALLWRAPPERNAMEAS(NScEnableRefreshOffset));	

	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterface("NGUIScrollerDelegate");
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUIScrollerDelegate", "void OnStartRefresh(NGUIScroller@, int)");	
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUIScrollerDelegate", "void OnRubberStopInRefreshMode(NGUIScroller@)");	

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetDelegate(NGUIScrollerDelegate@ delegate)", CALLWRAPPERNAMEAS(NScScriptSetDelegate));
}

void NGUIScroller::NScScriptSetDelegate(asIScriptGeneric *gen)
{
	asIScriptObject* scr_object = (asIScriptObject*)gen->GetArgObject(0);	

	asITypeInfo* tp_info = scr_object->GetObjectType();	

	asIScriptFunction* scr_func;

	scr_func = tp_info->GetMethodByDecl("void OnStartRefresh(NGUIScroller@, int)", false);
	callback_start_refresh.Set(scr_func, scr_object);	

	scr_func = tp_info->GetMethodByDecl("void OnRubberStopInRefreshMode(NGUIScroller@, float)", false);
	callback_stop_refresh.Set(scr_func, scr_object);
}

void NGUIScroller::Work(float dt, int level)
{	
	float old_offset = scroller.offset;

	if (!Scene()->IsRunning()) return;

    if (!IsVisible())
    {
        return;
    }
    
    if (IsSymbol())
    {
        Show(false);
		DelUpdate((TaskHolder::Task)&NGUIScroller::Work);
        return;
    }
    
	if (childs.size() == 0) return;

    if(scrollerMover.IsWorking())
    {
        scrollerMover.Update(dt,scroller);
        if(scrollerMover.IsAllowScroll())
        {
            float old_offset  = scroller.offset;
            ScrollerWork(this, scroller, dt, scrollerType, allow_scroll);
            if(old_offset != scroller.offset)
                scrollerMover.Stop();
        }
        UpdateChildsPos();
    }
    else
    {
		scroller.allow_rubber = allow_rubber;

		if (!scroller.allow_rubber)
		{
			scroller.auto_border = 0;
		}
		else
		{
			scroller.auto_border = 200;
		}

        ScrollerWork(this, scroller, dt, scrollerType, allow_scroll);

        if (refresh_threshold > refresh_offset)
        {
            if(scroller.offset > 0)
            {
                if(!onStartRefreshFuncCall)
                {
                    if(scroller.offset > refresh_threshold)
                    {
                        onStartRefreshFuncCall = true;

						core->Script()->Machine()->AddPtrArg(this);
						core->Script()->Machine()->AddIntArg(0);
						core->Script()->Machine()->Call(&callback_start_refresh);		
                        
                        onRubberStopInRefreshModeFuncCall = scroller.refresh_offset != 0;

                    }
                }
                else
                    if(scroller.offset <= refresh_offset)
                    {
                        onStartRefreshFuncCall = false;
                        if(onRubberStopInRefreshModeFuncCall)
                        {
							scroller.Reset();
							scroller.refresh_offset = refresh_offset;
							scroller.offset = refresh_offset;
							// UpdateChildsPos();

                            onRubberStopInRefreshModeFuncCall = false;

							core->Script()->Machine()->AddPtrArg(this);							
							core->Script()->Machine()->Call(&callback_stop_refresh);		
                        }
                    }
            }
            else
            {
                if(!onStartRefreshFuncCall)
                {
                    if(scroller.offset < -scroller.scroll_size - refresh_threshold)
                    {
                        onStartRefreshFuncCall = true;
                        
						core->Script()->Machine()->AddPtrArg(this);
						core->Script()->Machine()->AddIntArg(1);
						core->Script()->Machine()->Call(&callback_start_refresh);		

                        onRubberStopInRefreshModeFuncCall = scroller.refresh_offset != 0;

                    }
                }
                else
                    if(scroller.offset >= -scroller.scroll_size - refresh_offset)
                    {
                        onStartRefreshFuncCall = false;
                        if(onRubberStopInRefreshModeFuncCall)
                        {
							scroller.Reset();
							scroller.refresh_offset = refresh_offset;
							scroller.offset = -scroller.scroll_size - refresh_offset;
							// UpdateChildsPos();

                            onRubberStopInRefreshModeFuncCall = false;
                            
							core->Script()->Machine()->AddPtrArg(this);							
							core->Script()->Machine()->Call(&callback_stop_refresh);		
                        }
                    }
            }
        }
       	if (fabs(old_offset - scroller.offset) > 0.1f)
	    {
		    UpdateChildsPos();
            /*if (!IsVisible())
            {
                return;
            }
        
            float prev_scr_w = scr_w;
        
            if (scaleChilds)
            {
                if (prev_w > -99000.0f)
                {
                    if (fabsf(prev_w - width) > 1.0f || fabsf(prev_h - height) > 1.0f)
                    {
                        scale_w = width / original_w;
                        scale_h = height / original_h;
                    
                        float scale_x = width / prev_w;
                        float scale_y = height / prev_h;
                    
                        for (int i=0;i<childs.size();i++)
                        {
                            childs[i]->Scale(scale_x, scale_y);
                        }
                    
                        need_recalc_pos = true;
                    }
                }
            
                prev_w = width;
                prev_h = height;
            }
        
            need_recalc_pos = true;
        
            prev_x = x;
            prev_y = y;
        
            if (need_recalc_pos)
            {
                CalcHorzPos();
                CalcVertPos();
            
                Matrix mat_rt;
                //mat_rt.RotateZ(RADIAN * rot_x);
                //mat_rt.RotateX(RADIAN * rot_x);
                mat_rt.RotateZ(RADIAN * rot_z);
            
                Matrix mat_trans;
                mat_trans.Move(Vector(ConvertHorz(pre_anchor_x), ConvertVert(pre_anchor_y), 0.0f));
            
                Matrix mat_trans2;
                mat_trans2.Move(Vector(ConvertHorz(-cur_anchor_x), ConvertVert(-cur_anchor_y), 0.0f));
            
                local_mat = mat_trans2 * mat_rt * mat_trans;		
            
                if (parent)
                {
                    global_mat = local_mat * parent->childs_global_mat;
                }
                else
                {
                    global_mat = local_mat;
                }		
            
                Vector ps(0, 0, 0.0f);
            
                ps = ps * global_mat;
            
                scr_x = ps.x;
                scr_y = ps.y;
            
                CalcChildsGlobalMat();
            
                need_recalc_pos = false;
            }	
        
            core->TraceTo("app", "start %s", GetName());
        
            for (int i=0;i<(int)childs.size();i++)
            {
                core->TraceTo("app", childs[i]->GetName());
            
                //if (i < 3)
                {
                    childs[i]->UpdateChildsPos();
                }
            }
        
            core->TraceTo("app", "done %s", GetName());
    */
	    }	
    }

}

void NGUIScroller::Release()
{
	DelUpdate((TaskHolder::Task)&NGUIScroller::Work);

	NGUIWidget::Release();
}

void NGUIScroller::PostRenderWidget()
{    
	Color color = overColor;
	color.a = alpha;

	if (parent)
	{
		color *= parent->overColor;
		color.a = alpha * parent->alpha;
	}	

	if (scrollerType == 0)
	{
		float scr_scroll_thin = ConvertVert(scroll_thin);
		float scr_thumb_size = ConvertHorz(thumb_size);

		if (texScrollBack)
		{
			AddDrawCall(texScrollBack, 0, scr_h - scr_scroll_thin, scr_w, scr_scroll_thin, color, 0.0f, 0.0f, 1.0f, 1.0f);			
		}

		float scr_thumb_pos = -ConvertHorz((width - thumb_size) * scroller.offset / scroller.scroll_size);

		if (texScrollThumb)
		{
			AddDrawCall(texScrollThumb, scr_thumb_pos, scr_h - scr_scroll_thin, scr_thumb_size, scr_scroll_thin, color, 0.0f, 0.0f, 1.0f, 1.0f);			
		}
	}
	else
	{
		float scr_scroll_thin = ConvertHorz(scroll_thin);
		float scr_thumb_size = ConvertVert(thumb_size);

		if (texScrollBack)
		{
			AddDrawCall(texScrollBack, scr_w - scr_scroll_thin, 0, scr_scroll_thin, scr_h, color, 0.0f, 0.0f, 1.0f, 1.0f);			
		}

		float scr_thumb_pos = -ConvertVert((height - thumb_size) * scroller.offset / scroller.scroll_size);

		if (texScrollThumb)
		{
			AddDrawCall(texScrollThumb, scr_w - scr_scroll_thin, scr_thumb_pos, scr_scroll_thin, scr_thumb_size, color, 0.0f, 0.0f, 1.0f, 1.0f);			
		}
	}
}

void NGUIScroller::ExecuteDrawCalls()
{
#ifdef EDITOR	
	if (!Scene()->IsRunning() && NGUIService::instance->DebugColor())
	{
		Color color = COLOR_YELLOW;
		color.a = 0.75f;
		AddDrawCall(empty_tex, 0, 0, scr_w, scr_h, color, 0, 0, 1, 1);		
	}
#endif

	NGUIWidget::ExecuteDrawCalls();

	PostRenderWidget();

	NGUIWidget::ExecuteDrawCalls();
}

float NGUIScroller::NScGetPositionScript()
{
	return scroller.offset;
}

void NGUIScroller::CalcChildsGlobalMat()
{
	Matrix trans;

	if (scrollerType == 0)
	{
		trans.Move((int)(ConvertHorz(scroller.offset)), 0.0f, 0.0f);
	}
	else
	if (scrollerType == 1)
	{
		trans.Move(0.0f, (int)(ConvertVert(scroller.offset)), 0.0f);
	}

	childs_global_mat = trans * global_mat;
}

void NGUIScroller::NScSetPositionScript(float set, bool instance)
{
	if (instance)
	{
		if (set > 0)
		{
			set  = 0;
		}
		else
		{
			if(set < -scroller.scroll_size)
			{
				set = -scroller.scroll_size;
			}
		}

		scroller.Reset();
		scroller.offset = set;
		UpdateChildsPos();
	}
	else
	{
		if (set > 0)
		{
			set  = 0;
		}
		else
        if (set < -scroller.scroll_size)
		{
             set = -scroller.scroll_size;
		}

		scroller.Reset();
		scrollerMover.Init(set,scroller);
	}
}

bool NGUIScroller::NScIsInScrollingScript()
{
	return  (scroller.is_draged || scroller.need_auto);
}

void NGUIScroller::NScSetScrollSize(float sz)
{
	whole_scroll_size = sz;
	
	scroller.scroll_size = whole_scroll_size;
	
	if (scrollerType == 0)
	{
		scroller.scroll_size -= width;
	}
	else
	{
		scroller.scroll_size -= height;
	}	

	if (scroller.scroll_size < 0)
	{
		scroller.scroll_size = 0;
	}	
}

float NGUIScroller::NScGetScrollSize()
{
	return whole_scroll_size;
}

void NGUIScroller::NScEnableRefreshOffset(bool enable)
{
    if (enable)
    {
        scroller.refresh_offset = refresh_offset;
    }
    else
    {
        scroller.refresh_offset = 0;
    }
}

void NGUIScroller::ScrollerMover::Init(float _target,const NGUIWidget::ScrollerData& scroller)
{
    Reset();
    target = _target;
    velocity = (target - scroller.offset) * 0.8f;
}

void NGUIScroller::ScrollerMover::Update(float dt,NGUIWidget::ScrollerData& scroller)
{

    timer += dt;
    delta  = -velocity * exp(-static_cast<double>(timer) * 2.0);
    if (fabs(delta) > 0.2f)
    {
        scroller.offset = target + delta;
    }
    else
    {
        scroller.offset = target;
        Reset();
    }
}


#ifdef EDITOR
Color NGUIScroller::GetDebugColor()
{
	return COLOR_YELLOW;
}
#endif
 

