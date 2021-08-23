
#include "NGUIButtonInstances.h"

ENTITY(NGUIButtonInstances)
ENTITY_IS_2D_OBJECT
ENTITY_PARAMS(NGUIButtonInstances,"NGUIButtonInstances","NGUI", "ButtonInstanñes")

void NGUIButtonInstances::RegisterProperties()
{
	NGUIWidget::RegisterProperties();				

	PROP_REG( funcName )
	PROP_REG( funcNameMove )
	PROP_REG( funcNameUp )
	PROP_REG( funcNameHighlightOn )
	PROP_REG( funcNameHighlightOff )

	PROP_REG( border )
	PROP_REG( eventType )
}

void NGUIButtonInstances::Init()
{	
	NGUIWidget::Init();
	
	sel_inst = -1;
	
	SetUpdate((TaskHolder::Task)&NGUIButtonInstances::CheckPress, lvl_pre5 + 3);	

	in_drag = false;
	is_hightlited= false;

	counter = -1.0f;
    
    hack_skip_update = true;
}
	
bool NGUIButtonInstances::Start()
{
	ScriptFunction::Find2PI(&callback_ms_down, Scene()->ScriptModule(), funcName.c_str(), Desc()->GetName());
	ScriptFunction::Find4PIFF(&callback_ms_move, Scene()->ScriptModule(), funcNameMove.c_str(), Desc()->GetName());
	ScriptFunction::Find2PI(&callback_ms_up, Scene()->ScriptModule(), funcNameUp.c_str(), Desc()->GetName());
	ScriptFunction::Find2PI(&callback_ms_hgl_on, Scene()->ScriptModule(), funcNameHighlightOn.c_str(), Desc()->GetName());
	ScriptFunction::Find2PI(&callback_ms_hgl_off, Scene()->ScriptModule(), funcNameHighlightOff.c_str(), Desc()->GetName());

	return NGUIWidget::Start();
}

void NGUIButtonInstances::Finish()
{
	callback_ms_down.Reset();
	callback_ms_move.Reset();
	callback_ms_up.Reset();
	callback_ms_hgl_on.Reset();
	callback_ms_hgl_off.Reset();
}


void NGUIButtonInstances::CheckPress(float dt, int level)
{
	if (Scene()->IsRunning())
	{		
		if (((mscover_widget == this || mscover_widget_not_covered == this) && !in_drag))
		{
			if (IsActive() && IsVisible())
			{
				bool res = false;
				
				if (eventType == 0)
				{
					res = core->Controls()->CheckTouch(0, ControlsService::FastActivatedExclusive, this);
				}
				else
				if (eventType == 1)
				{
					res = core->Controls()->CheckTouch(0, ControlsService::ActivatedExclusive, this);
				}

				if (res)
				{
					in_drag = true;

					core->Script()->Machine()->AddPtrArg(this);
					core->Script()->Machine()->AddIntArg(sel_inst);
					core->Script()->Machine()->Call(&callback_ms_down);					

					if (eventType == 0)
					{
						core->Script()->Machine()->AddPtrArg(this);
						core->Script()->Machine()->AddIntArg(sel_inst);
						core->Script()->Machine()->Call(&callback_ms_hgl_on);

						is_hightlited = true;

						counter = 0.15f;
					}
                    
                    hack_skip_update = true;
				}

				//core->TraceTo("debug", "in check 1 %s", GetName());
			}
		}
		
		if (counter > 0.0f)
		{
			counter -= dt;

			if (counter < 0.0f)
			{
				counter = -1.0f;
			}
		}
		else
		{
			if (in_drag)
			{
				if (!is_hightlited)
				{
					if (CheckSelInstance())
					{
						core->Script()->Machine()->AddPtrArg(this);
						core->Script()->Machine()->AddIntArg(sel_inst);
						core->Script()->Machine()->Call(&callback_ms_hgl_on);

						is_hightlited = true;
					}				
				}
				else
				{
					if (!CheckSelInstance())
					{
						core->Script()->Machine()->AddPtrArg(this);
						core->Script()->Machine()->AddIntArg(sel_inst);
						core->Script()->Machine()->Call(&callback_ms_hgl_off);

						is_hightlited = false;
					}				
				}

				if (core->Controls()->CheckTouch(0, ControlsService::ActiveExclusive, this))
				{
                    if (!hack_skip_update)
                    {
						core->Script()->Machine()->AddPtrArg(this);
						core->Script()->Machine()->AddIntArg(sel_inst);
						core->Script()->Machine()->AddFloatArg(Convert2AbsHorz(ms_pos_x - ms_prev_pos_x));
						core->Script()->Machine()->AddFloatArg(Convert2AbsVert(ms_pos_y - ms_prev_pos_y));
						core->Script()->Machine()->Call(&callback_ms_move);
                    }
                    else
                    {
                        hack_skip_update = false;
                    }

					//core->TraceTo("debug", "in check 2 %s", GetName());
				}
				else
				{
					in_drag = false;
					
					core->Script()->Machine()->AddPtrArg(this);
					core->Script()->Machine()->AddIntArg(sel_inst);
					core->Script()->Machine()->Call(&callback_ms_up);

					if (is_hightlited)
					{
						core->Script()->Machine()->AddPtrArg(this);
						core->Script()->Machine()->AddIntArg(sel_inst);
						core->Script()->Machine()->Call(&callback_ms_hgl_off);

						is_hightlited = false;
					}

					//core->TraceTo("debug", "in check 3 %s", GetName());
				}
			}
		}
	}
}

void NGUIButtonInstances::CheckMouseCover()
{
	if (!IsVisible()) return;
		
	if (AllowMouseCover())
	{
		CheckInstance();

		if (sel_inst != -1)
		{
			mscover_widget = this;
			mscover_widget_not_covered = null;
		}		
	}
	else
	if (!mscover_widget)
	{
		CheckInstance();

		if (sel_inst != -1)
		{				
			mscover_widget_not_covered = this;
		}		
	}			

	for (int i=0;i<(int)childs.size();i++)
	{
		childs[i]->CheckMouseCover();
	}
}

void NGUIButtonInstances::CheckInstance()
{
	sel_inst = -1;

	float clip_x =-10000000.0f;
	float clip_y =-10000000.0f;
	float clip_x2 = 10000000.0f;
	float clip_y2 = 10000000.0f;

	GetClipRect(clip_x, clip_y, clip_x2, clip_y2);

	bool need_clip = false;
	
	if (clip_x>-900000.0f)
	{				
		need_clip = true;
	}
    
	for (int i=instances.size()-1;i>=0;i--)
	{
		Instance& inst = instances[i];

		if (parent_alpha * inst.alpha < 0.995f) continue;

		float brd_x = scr_x + inst.x - border;
		float brd_y = scr_y + inst.y - border;
		float brd_x2 = scr_x + inst.x + inst.width + border;
		float brd_y2 = scr_y + inst.y + inst.height + border;

		if (need_clip)
		{
			if (brd_x<clip_x) brd_x = clip_x;
			if (brd_y<clip_y) brd_y = clip_y;
			if (brd_x2>clip_x2) brd_x2 = clip_x2;
			if (brd_y2>clip_y2) brd_y2 = clip_y2;
		}
		
		if (brd_x < ms_pos_x && ms_pos_x < brd_x2 &&
			brd_y < ms_pos_y && ms_pos_y < brd_y2)
		{					
			sel_inst = i;			
			break;
		}
	}
}

bool NGUIButtonInstances::CheckSelInstance()
{
	if (sel_inst == -1)
	{
		 return false;
	}

	float clip_x =-10000000.0f;
	float clip_y =-10000000.0f;
	float clip_x2 = 10000000.0f;
	float clip_y2 = 10000000.0f;

	GetClipRect(clip_x, clip_y, clip_x2, clip_y2);

	bool need_clip = false;
	
	if (clip_x>-900000.0f)
	{				
		need_clip = true;
	}
    
	Instance& inst = instances[sel_inst];
	
	if (inst.alpha < 0.05f)
	{
		return false;
	}

	float brd_x = scr_x + inst.x - border;
	float brd_y = scr_y + inst.y - border;
	float brd_x2 = scr_x + inst.x + inst.width + border;
	float brd_y2 = scr_y + inst.y + inst.height + border;

	if (need_clip)
	{
		if (brd_x<clip_x) brd_x = clip_x;
		if (brd_y<clip_y) brd_y = clip_y;
		if (brd_x2>clip_x2) brd_x2 = clip_x2;
		if (brd_y2>clip_y2) brd_y2 = clip_y2;
	}
	
	if (brd_x < ms_pos_x && ms_pos_x < brd_x2 &&
			brd_y < ms_pos_y && ms_pos_y < brd_y2)
	{					
		return true;
	}	

	return false;
}

float NGUIButtonInstances::GetBorderHorz()
{
	return border;
}

float NGUIButtonInstances::GetBorderVert()
{
	return border;
}

CALLWRAPPER(NGUIButtonInstances, NBtnInstClearInstancesScript, CALLWRAPPERNORET())
CALLWRAPPERARG1(NGUIButtonInstances, NBtnInstResizeInstancesScript, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERNORET())
CALLWRAPPER(NGUIButtonInstances, NBtnInstGetInstancesCountScript, CALLWRAPPERRET(int))
CALLWRAPPERARG4(NGUIButtonInstances, NBtnInstAddInstanceScript, CALLWRAPPERGETPARAM(float,arg1),CALLWRAPPERGETPARAM(float,arg2),CALLWRAPPERGETPARAM(float,arg3),CALLWRAPPERGETPARAM(float,arg4), CALLWRAPPERNORET())
CALLWRAPPERARG5(NGUIButtonInstances, NBtnInstSetInstanceScript, CALLWRAPPERGETPARAM(int,arg1),CALLWRAPPERGETPARAM(float,arg2),CALLWRAPPERGETPARAM(float,arg3),CALLWRAPPERGETPARAM(float,arg4),CALLWRAPPERGETPARAM(float,arg5), CALLWRAPPERNORET())
CALLWRAPPERARG1(NGUIButtonInstances, NBtnInstDelInstanceScript, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERNORET())

void CALLWRAPPERNAME(NBtnInstScriptSetDelegate)(asIScriptGeneric *gen)
{
	NGUIButtonInstances* obj = (NGUIButtonInstances*)gen->GetObject();
	obj->NBtnInstScriptSetDelegate(gen);	
}

void NGUIButtonInstances::MethodsToScript(const char* script_class_name)
{
	NGUIWidget::MethodsToScript(script_class_name);			

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void ClearInstances()", CALLWRAPPERNAMEAS(NBtnInstClearInstancesScript));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void ResizeInstances(int size)", CALLWRAPPERNAMEAS(NBtnInstResizeInstancesScript));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "int  GetInstancesCount()", CALLWRAPPERNAMEAS(NBtnInstGetInstancesCountScript));

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void AddInstance(float x, float y, float width, float height)", CALLWRAPPERNAMEAS(NBtnInstAddInstanceScript));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetInstance(int index, float x, float y, float width, float height)", CALLWRAPPERNAMEAS(NBtnInstSetInstanceScript));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void DelInstance(int index)", CALLWRAPPERNAMEAS(NBtnInstDelInstanceScript));	

	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterface("NGUIButtonInstancesDelegate");
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUIButtonInstancesDelegate", "void OnDown(NGUIButtonInstances@, int)");
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUIButtonInstancesDelegate", "void OnMove(NGUIButtonInstances@, int, float, float)");
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUIButtonInstancesDelegate", "void OnUp(NGUIButtonInstances@, int)");
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUIButtonInstancesDelegate", "void OnHighlightOn(NGUIButtonInstances@, int)");
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUIButtonInstancesDelegate", "void OnHighlightOff(NGUIButtonInstances@, int)");	

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetDelegate(NGUIButtonInstancesDelegate@ delegate)", CALLWRAPPERNAMEAS(NBtnInstScriptSetDelegate));
}

void NGUIButtonInstances::NBtnInstClearInstancesScript()
{
	instances.clear();
}

void NGUIButtonInstances::NBtnInstAddInstanceScript(float x, float y, float width, float height)
{
	instances.push_back(Instance());
	
	NBtnInstSetInstanceScript(instances.size() - 1, x, y, width, height);
}

void NGUIButtonInstances::NBtnInstSetInstanceScript(int index, float x, float y, float width, float height)
{
	if (index<0 || index>instances.size()-1) return;

	Instance* inst = &instances[index];

	inst->alpha = 1.0f;
	inst->x = ConvertHorz(x);
	inst->y = ConvertVert(y);
	inst->width = ConvertHorz(width);
	inst->height = ConvertVert(height);
}

void NGUIButtonInstances::NBtnInstResizeInstancesScript(int sz)
{
	instances.resize(sz);
}

int  NGUIButtonInstances::NBtnInstGetInstancesCountScript()
{
	return instances.size();
}

void NGUIButtonInstances::NBtnInstDelInstanceScript(int index)
{
	if (index<0 || index>instances.size()-1) return;
	instances.erase(instances.begin() + index);
}

void NGUIButtonInstances::NBtnInstScriptSetDelegate(asIScriptGeneric *gen)
{	
	asIScriptObject* scr_object = (asIScriptObject*)gen->GetArgObject(0);	

	asITypeInfo* tp_info = scr_object->GetObjectType();	

	asIScriptFunction* scr_func;

	scr_func = tp_info->GetMethodByDecl("void OnDown(NGUIButtonInstances@, int)", false);

	if (scr_func)
	{		
		callback_ms_down.Set(scr_func, scr_object);
	}

	scr_func = tp_info->GetMethodByDecl("void OnMove(NGUIButtonInstances@, int, float, float)", false);

	if (scr_func)
	{		
		callback_ms_move.Set(scr_func, scr_object);
	}

	scr_func = tp_info->GetMethodByDecl("void OnUp(NGUIButtonInstances@, int)", false);

	if (scr_func)
	{		
		callback_ms_up.Set(scr_func, scr_object);
	}

	scr_func = tp_info->GetMethodByDecl("void OnHighlightOn(NGUIButtonInstances@, int)", false);

	if (scr_func)
	{		
		callback_ms_hgl_on.Set(scr_func, scr_object);
	}

	scr_func = tp_info->GetMethodByDecl("void OnHighlightOff(NGUIButtonInstances@, int)", false);

	if (scr_func)
	{		
		callback_ms_hgl_off.Set(scr_func, scr_object);
	}
}

#ifdef EDITOR
Color NGUIButtonInstances::GetDebugColor()
{
	return COLOR_CYAN;
}
#endif

void NGUIButtonInstances::Release()
{
	DelUpdate((TaskHolder::Task)&NGUIButtonInstances::CheckPress);	
	
	NGUIWidget::Release();
}