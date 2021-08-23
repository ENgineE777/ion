
#include "NGUIPager.h"

ENTITY(NGUIPager)
ENTITY_IS_2D_OBJECT
ENTITY_PARAMS(NGUIPager,"NGUIPager","NGUI", "Pager")

void NGUIPager::RegisterProperties()
{
	NGUIWidget::RegisterProperties();	

	PROP_REG(pagerType)
	PROP_REG(allow_scroll)	
	PROP_REG(border)
	PROP_REG(num_pages)
	PROP_REG(texDotName)
	PROP_REG(texCurDotName)
	PROP_REG(dot_offset)
	PROP_REG(dot_size)
	PROP_REG(dot_distance)			
	PROP_REG(dot_color)
	PROP_REG(allowRubberBorder)	
	PROP_REG(exclusiveMouse)		
	PROP_REG(funcName)	
	PROP_REG(funcNameEndScroll)	
}

void NGUIPager::Init()
{			
	NGUIWidget::Init();						
	
	texDot = null;
	texCurDot = null;

	cur_page = 0;
	is_draged = 0;
	childs_offset = 0;
	in_scroll = false;
	target_page = 0;
	ms_pos = 0;

	border_v = 0;
	border_h = 0;

	allow_scroll = true;
	call_func_cur_page = 0;
	call_func_target_page = 0;
	call_func = false;
	call_funcEndScroll = false;

	touchType = -1;

	anim_time = -1.0f;

	SetUpdate((TaskHolder::Task)&NGUIPager::Work, lvl_pre5+3);
}
	
void NGUIPager::ReInit()
{
	RELEASE(texDot)
	texDot = en_core.Render()->LoadTexture(texDotName.c_str(), false, _FL_);		

	RELEASE(texCurDot)
	texCurDot = en_core.Render()->LoadTexture(texCurDotName.c_str(), false, _FL_);

	NGUIWidget::ReInit();	
}

bool NGUIPager::Start()
{
	cur_page = 0;
	cur_page_float = 0.0f;
	childs_offset = 0;
	is_draged = false;
	in_scroll = false;
	target_page = 0;
	ms_pos = 0;

	ScriptFunction::Find2PF(&callback_start, Scene()->ScriptModule(), funcName.c_str(), Desc()->GetName());
	ScriptFunction::Find2PF(&callback_end, Scene()->ScriptModule(), funcNameEndScroll.c_str(), Desc()->GetName());

	return NGUIWidget::Start();
}

void NGUIPager::Finish()
{
	callback_start.Reset();
	callback_end.Reset();

	cur_page = 0;	

	childs_offset = 0;
	is_draged = false;
	in_scroll = false;

	NGUIWidget::Finish();	
}

CALLWRAPPER(NGUIPager, NPgGetPositionScript, CALLWRAPPERRET(float))
CALLWRAPPERARG2(NGUIPager, NPgSetPositionScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(bool,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG1(NGUIPager, NPgScrollTo, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG1(NGUIPager, NPgSetCurPageIndex, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERNORET())
CALLWRAPPER(NGUIPager, NPgGetCurPage, CALLWRAPPERRET(int))
CALLWRAPPER(NGUIPager, NPgScriptGetAllowScroll, CALLWRAPPERRET(bool))
CALLWRAPPERARG1(NGUIPager, NPgScriptSetAllowScroll, CALLWRAPPERGETPARAM(bool,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG3(NGUIPager, NPgSetDotTexColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())	

void CALLWRAPPERNAME(NPgScriptSetDelegate)(asIScriptGeneric *gen)
{
	NGUIPager* obj = (NGUIPager*)gen->GetObject();
	obj->NPgScriptSetDelegate(gen);	
}

void NGUIPager::MethodsToScript(const char* script_class_name)
{
	NGUIWidget::MethodsToScript(script_class_name);

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "float GetPosition()", CALLWRAPPERNAMEAS(NPgGetPositionScript));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void  SetPosition(float set, bool instance)", CALLWRAPPERNAMEAS(NPgSetPositionScript));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void ScrollTo(int target_page)", CALLWRAPPERNAMEAS(NPgScrollTo));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetCurPageIndex(int index)", CALLWRAPPERNAMEAS(NPgSetCurPageIndex));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "int GetCurPage()", CALLWRAPPERNAMEAS(NPgGetCurPage));	

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetDotTexColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NPgSetDotTexColorScript));

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "bool get_Allow_Scroll()", CALLWRAPPERNAMEAS(NPgScriptGetAllowScroll));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void set_Allow_Scroll(bool set_Allow_Scroll)", CALLWRAPPERNAMEAS(NPgScriptSetAllowScroll));		

	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterface("NGUIPagerDelegate");
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUIPagerDelegate", "void OnStartScroll(NGUIPager@, int)");	
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUIPagerDelegate", "void OnEndScroll(NGUIPager@, int)");	

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetDelegate(NGUIPagerDelegate@ delegate)", CALLWRAPPERNAMEAS(NPgScriptSetDelegate));
}

void  NGUIPager::NPgScriptSetDelegate(asIScriptGeneric *gen)
{
	asIScriptObject* scr_object = (asIScriptObject*)gen->GetArgObject(0);	

	asITypeInfo* tp_info = scr_object->GetObjectType();	

	asIScriptFunction* scr_func;

	scr_func = tp_info->GetMethodByDecl("void OnStartScroll(NGUIPager@, float)", false);
	callback_start.Set(scr_func, scr_object);	

	scr_func = tp_info->GetMethodByDecl("void OnEndScroll(NGUIPager@, float)", false);
	callback_end.Set(scr_func, scr_object);	
}

void NGUIPager::CalcAnimTime()
{
	if (pagerType == 0)
	{
		if (ms_pos > width * 0.1f)
		{
			if (cur_page > 0)
			{
				target_page = cur_page - 1;
				call_func_cur_page = cur_page;
				call_func_target_page = target_page;
				call_func  = true;

				anim_time =  (-target_page) - (float)childs_offset / (float)width;

				anim_time = powf(anim_time, 1.0/ 3.0f);
			}
			else
			{							
				anim_time =  (float)childs_offset / (float)width - (-target_page);
				anim_time = powf(anim_time, 1.0/ 3.0f);
			}
		}
		else
		if (ms_pos < -width * 0.1f)
		{
			if (cur_page < num_pages - 1)
			{
				target_page = cur_page + 1;
				call_func_cur_page = cur_page;
				call_func_target_page = target_page;
				call_func  = true;

				anim_time =  (float)childs_offset / (float)width - (-target_page);

				anim_time = powf(anim_time, 1.0/ 3.0f);
			}
			else
			{
				anim_time =  (-target_page) - (float)childs_offset / (float)width;
				
				anim_time = powf(anim_time, 1.0/ 3.0f);
			}
		}
	}
	else
	{
		if (ms_pos > height * 0.2f)
		{
			if (cur_page > 0)
			{
				target_page = cur_page - 1;
				call_func_cur_page = cur_page;
				call_func_target_page = target_page;
				call_func  = true;
			}
		}
		else
		if (ms_pos < -height * 0.35f)
		{
			if (cur_page < num_pages - 1)
			{
				target_page = cur_page + 1;
				
				call_func_cur_page = cur_page;
				call_func_target_page = target_page;
				call_func  = true;
			}
		}
	}
	
	in_scroll = true;	
}

void NGUIPager::Work(float dt, int level)
{	
	if (pagerType == 0)
	{		
		border_h = ConvertHorz(border);
		border_v = 0;
	}
	else
	{
		border_h = 0;
		border_v = ConvertVert(border);		
	}

	float old_childs_offset = childs_offset;

	if (!Scene()->IsRunning()) return;		

	if (IsSymbol())// || String::IsEqual(name, "front_action_pager"))
    {
        Show(false);
        return;
    }

	//
	{		
		if ((!in_scroll && IsVisible() && IsActive()))
		{
			if (/*(mscover_widget == this || mscover_widget_not_covered == this) &&*/ !is_draged && allow_scroll)
			{	
				if (IsHitted(ms_pos_x, ms_pos_y, 0.0f, 0.0f, scr_w, scr_h))
				{		
					bool drag = core->Controls()->CheckTouch(0, ControlsService::Activated, this);
					
					if (drag)
					{
						ms_pos = 0;
						is_draged = true;
						touchType = -1;
					}
				}
			}
			else
			if (is_draged)
			{
				bool drag = core->Controls()->CheckTouchIgnoreOwner(0, ControlsService::Active);
							
				if ( !drag )
				{				
					is_draged = false;
				}
				else
				{
					/*if ((!scrollOnlyLocal) ||
						(scrollOnlyLocal && scr_x - border_h < ms_pos_x && ms_pos_x < scr_x + scr_w + border_h &&				
											scr_y - border_v < ms_pos_y && ms_pos_y < scr_y + scr_h + border_v))*/
					{						
						float delta = 0;
						float deltaX = ms_pos_x - ms_prev_pos_x;	
						float deltaY = ms_pos_y - ms_prev_pos_y;	

						if (touchType == -1)
						{
							if (fabs(deltaX) > fabs(deltaY))
							{
								touchType = 0;
							}
							else
							if (fabs(deltaX) < fabs(deltaY))
							{
								touchType = 1;
							}
						}						
						
						if (touchType == pagerType)
						{							
							if (pagerType == 0)
							{
								delta = Convert2AbsHorz(deltaX);
							}
							else
							{
								delta = Convert2AbsVert(deltaY);
							}

							childs_offset += delta;

							ms_pos += delta;

							float k_border = 1.0f;

							if (!allowRubberBorder)
							{
								k_border = 0.0f;
							}

							if (pagerType == 0)
							{
								if (cur_page == 0)
								{									
									if (childs_offset > width * 0.2f * k_border)
									{
										childs_offset = width * 0.2f * k_border;
									}						
								}
								
								if (cur_page == num_pages - 1)
								{
									if (childs_offset < -width * 0.2f * k_border - width * (num_pages - 1))
									{
										childs_offset = -width * 0.2f * k_border - width * (num_pages - 1);
									}						
								}								
							}
							else
							{
								if (cur_page == 0)
								{
									if (childs_offset > height * 0.2f * k_border)
									{
										childs_offset = height * 0.2f * k_border;
									}						
								}
								
								if (cur_page == num_pages - 1)
								{
									if (childs_offset < -height * 0.2f * k_border - height * (num_pages - 1))
									{
										childs_offset = -height * 0.2f * k_border - height * (num_pages - 1);
									}						
								}								
							}
						}
						else
						if (touchType != -1)
						{					
							is_draged = false;
						}
					}
					/*else
					{					
						is_draged = false;
					}*/
				}
				if (!is_draged)
				{
					CalcAnimTime();				
				}
			}
		}
		else
		{
			if (is_draged)
			{
				is_draged = false;
				CalcAnimTime();
			}

			if (pagerType == 0)
			{
				anim_time -= dt * 2.0f;

				if (anim_time < 0.0f)
				{
					anim_time = -1.0f;
				}

				if (childs_offset > -target_page * width)
				{										
					if (anim_time > 0.01f)
					{
						childs_offset = -target_page * width + EasyInOut::CubicEaseIn(anim_time) * width;						
					}
					else
					{
						childs_offset = -target_page * width;
						cur_page = target_page;
						cur_page_float = target_page;
						in_scroll = false;
						call_funcEndScroll = true;						
					}
				}
				else
				{
					if (anim_time > 0.01f)
					{
						childs_offset = -target_page * width - EasyInOut::CubicEaseIn(anim_time) * width;						
					}
					else
					{
						childs_offset = -target_page * width;
						cur_page = target_page;
						cur_page_float = target_page;
						in_scroll = false;
						call_funcEndScroll = true;
					}	
				}
			}
			else
			{
				/*if (childs_offset > -target_page * height)
				{
					childs_offset -= dt * scroll_speed;
					
					if (childs_offset < -target_page * height)
					{
						childs_offset = -target_page * height;
						cur_page = target_page;
						cur_page_float = target_page;
						in_scroll = false;
						call_funcEndScroll = true;
					}
				}
				else
				{
					childs_offset += dt * scroll_speed;						
	
					if (childs_offset > -target_page * height)
					{
						childs_offset = -target_page * height;
						cur_page = target_page;
						cur_page_float = target_page;
						in_scroll = false;
						call_funcEndScroll = true;
					}	
				}*/
			}
		}
	}

	if (pagerType == 0)
	{
		cur_page_float = -childs_offset / width;	
	}
	else
	{
		cur_page_float = -childs_offset / height;	
	}

	if (cur_page_float<0)
	{
		cur_page_float = 0;
	}	

	if (cur_page_float>num_pages - 1)
	{
		cur_page_float = num_pages - 1;
	}

	if (call_func && funcName[0])
	{
		call_func = false;		

		core->Script()->Machine()->AddPtrArg(this);
		core->Script()->Machine()->AddFloatArg(float(call_func_cur_page));
		core->Script()->Machine()->AddFloatArg(float(call_func_target_page));
		core->Script()->Machine()->Call(&callback_start);		

	}

	if (call_funcEndScroll && !funcNameEndScroll.empty())
	{
		call_funcEndScroll = false;
		
		core->Script()->Machine()->AddPtrArg(this);
		core->Script()->Machine()->AddFloatArg(float(cur_page));
		core->Script()->Machine()->Call(&callback_end);		
	}
	
	if (fabs(old_childs_offset - childs_offset) > 0.1f)
	{
		UpdateChildsPos();	
	}
}

void NGUIPager::NPgSetDotTexColorScript(float r, float g, float b)
{
	dot_color.r = r;
	dot_color.g = g;
	dot_color.b = b;
}

void NGUIPager::NPgScrollTo(int target_page)
{
	this->target_page = target_page;
	in_scroll = true;

	call_func_cur_page = cur_page;
	call_func_target_page = target_page;
	call_func  = true;
	
}

void NGUIPager::NPgSetCurPageIndex(int index)
{
	cur_page = index;
	childs_offset = -index * width;

	UpdateChildsPos();	
	
}

void NGUIPager::PostRenderWidget()
{    
	if (num_pages < 1) return;

	float sz_x = ConvertHorz(dot_size);
	float sz_y = ConvertVert(dot_size);

	float dx = ConvertHorz(dot_distance);	
	
	float offset_x = (scr_w - (sz_x * num_pages + dx * (num_pages - 1))) * 0.5f;
	float offset_y = scr_h - sz_y - ConvertVert(dot_offset);
	
	TextureInstance* tex = null;

	Color color = overColor;
	color *= dot_color;

	color.a = alpha;

	if (parent)
	{
		color *= parent->overColor;
		color.a = alpha * parent->alpha;
	}

	for (int i=0;i<num_pages;i++)
	{
		if (i == cur_page)
		{
			tex = texCurDot;
		}
		else
		{
			tex = texDot;
		}
		
		if (tex) AddDrawCall(tex, offset_x, offset_y, sz_x, sz_y, color, 0.0f, 0.0f, 1.0f, 1.0f);

		offset_x += sz_x + dx;
	}

#ifdef EDITOR	
	if (!Scene()->IsRunning() && NGUIService::instance->DebugColor())
	{
		Color color = COLOR_YELLOW;
		color.a = 0.75f;
		AddDrawCall(empty_tex, 0, 0, scr_w, scr_h, color, 0, 0, 1, 1);		
	}
#endif
}

void NGUIPager::ExecuteDrawCalls()
{
	NGUIWidget::ExecuteDrawCalls();

	PostRenderWidget();

	NGUIWidget::ExecuteDrawCalls();
}

float NGUIPager::NPgGetPositionScript()
{
	return cur_page_float;
}

void NGUIPager::NPgSetPositionScript(float set, bool instance)
{	
	cur_page_float = set;
	target_page = set;

	if (instance)
	{
		if (pagerType == 0)
		{
			childs_offset = -set * width;
		}
		else
		{
			childs_offset = -set * height;
		}

		cur_page = set;
	}
	else
	{
		in_scroll = true;
		target_page = set;		
	}

	UpdateChildsPos();
}

float NGUIPager::GetBorderHorz()
{
	return border_h;
}

float NGUIPager::GetBorderVert()
{
	return border_v;
}

void NGUIPager::NPgScriptSetAllowScroll(bool set_as)
{
	allow_scroll = set_as;

	if (!allow_scroll)
	{
		is_draged = false;
	}
}

bool NGUIPager::NPgScriptGetAllowScroll()
{
	return allow_scroll;
}

void NGUIPager::CalcChildsGlobalMat()
{
	Matrix trans;
	trans.Move(ConvertHorz(childs_offset), 0.0f, 0.0f);	

	childs_global_mat = trans * global_mat;
}

void NGUIPager::Release()
{
	DelUpdate((TaskHolder::Task)&NGUIPager::Work);

	NGUIWidget::Release();
}

#ifdef EDITOR
Color NGUIPager::GetDebugColor()
{
	return COLOR_YELLOW;
}
#endif