
#include "NGUIButton.h"

ENTITY(NGUIButton)
ENTITY_IS_2D_OBJECT
ENTITY_PARAMS(NGUIButton,"NGUIButton","NGUI", "Button")

void NGUIButton::RegisterProperties()
{
	NGUIWidget::RegisterProperties();

	PROP_REG( funcName )
	PROP_REG( funcNameMove )
	PROP_REG( funcNameUp )	
	PROP_REG( funcNameHighlightOn )
	PROP_REG( funcNameHighlightOff )

	PROP_REG( mouseup_always )

	PROP_REG( border )
	PROP_REG( eventType )
}

void NGUIButton::Init()
{	
	NGUIWidget::Init();						
	
	sel_inst = -1;
	
    SetUpdate((TaskHolder::Task)&NGUIButton::CheckPress, lvl_pre5 + 3);

	in_drag = false;
	is_hightlited = false;

	counter = -1.0f;
    
    hack_skip_update = true;
}
	
void NGUIButton::CheckPress(float dt, int level)
{
	if (Scene()->IsRunning())
	{		
		if (((mscover_widget == this || mscover_widget_not_covered == this) && !in_drag))
		{
			if (IsActive() && IsVisible() && alpha > 0.995f)
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

                    SetFocused(true);
                    					
					core->Script()->Machine()->AddPtrArg(this);
					core->Script()->Machine()->Call(&callback_ms_down);

					if (eventType == 0)
					{
						core->Script()->Machine()->AddPtrArg(this);
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
					if (IsHitted(ms_pos_x, ms_pos_y))
					{
						core->Script()->Machine()->AddPtrArg(this);
						core->Script()->Machine()->Call(&callback_ms_hgl_on);

						is_hightlited = true;
					}				
				}
				else
				{
					if (!IsHitted(ms_pos_x, ms_pos_y))
					{
						core->Script()->Machine()->AddPtrArg(this);
						core->Script()->Machine()->Call(&callback_ms_hgl_off);
						
						is_hightlited = false;
					}				
				}

				if (core->Controls()->CheckTouch(0, ControlsService::ActiveExclusive, this))
				{
                    if (!hack_skip_update)
                    {												
						core->Script()->Machine()->AddPtrArg(this);
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

					if (IsHitted(ms_pos_x, ms_pos_y) || mouseup_always)
					{						
						core->Script()->Machine()->AddPtrArg(this);
						core->Script()->Machine()->Call(&callback_ms_up);

						if (is_hightlited)
						{							
							core->Script()->Machine()->AddPtrArg(this);
							core->Script()->Machine()->Call(&callback_ms_hgl_off);

							is_hightlited = false;
						}
					}

					//core->TraceTo("debug", "in check 3 %s", GetName());
				}
			}
		}
	}
}

void NGUIButton::CheckMouseCover()
{
	if (!IsVisible()) return;
		
	if (clr.a < 0.95f)
	{
		return;
	}

	if (AllowMouseCover())
	{
		if (alpha > 0.05f)
		{
			if (IsHitted(ms_pos_x, ms_pos_y, 0, 0, scr_w, scr_h))
			{
				mscover_widget = this;
				mscover_widget_not_covered = null;
			}	
		}		
	}
	else
	if (!mscover_widget)
	{
		if (alpha > 0.05f)
		{
			if (IsHitted(ms_pos_x, ms_pos_y, 0, 0, scr_w, scr_h))
			{
				mscover_widget_not_covered = this;
			}	
		}		
	}			

	for (int i=0;i<(int)childs.size();i++)
	{
        childs[i]->CheckMouseCover();
	}
}

void NGUIButton::CheckInstance()
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

		if (parent_alpha * inst.alpha < 0.05f) continue;

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

float NGUIButton::GetBorderHorz()
{
	return border;
}

float NGUIButton::GetBorderVert()
{
	return border;
}

bool NGUIButton::Start()
{
	ScriptFunction::Find1P(&callback_ms_down, Scene()->ScriptModule(), funcName.c_str(), Desc()->GetName());
	ScriptFunction::Find3PFF(&callback_ms_move, Scene()->ScriptModule(), funcNameMove.c_str(), Desc()->GetName());
	ScriptFunction::Find1P(&callback_ms_up, Scene()->ScriptModule(), funcNameUp.c_str(), Desc()->GetName());
	ScriptFunction::Find1P(&callback_ms_hgl_on, Scene()->ScriptModule(), funcNameHighlightOn.c_str(), Desc()->GetName());
	ScriptFunction::Find1P(&callback_ms_hgl_off, Scene()->ScriptModule(), funcNameHighlightOff.c_str(), Desc()->GetName());
	
	return NGUIWidget::Start();
}

void NGUIButton::Finish()
{
	callback_ms_down.Reset();
	callback_ms_move.Reset();
	callback_ms_up.Reset();
	callback_ms_hgl_on.Reset();
	callback_ms_hgl_off.Reset();
}

void CALLWRAPPERNAME(NBtnScriptSetDelegate)(asIScriptGeneric *gen)
{
	NGUIButton* obj = (NGUIButton*)gen->GetObject();
	obj->NBtnScriptSetDelegate(gen);	
}

void NGUIButton::MethodsToScript(const char* script_class_name)
{
	NGUIWidget::MethodsToScript(script_class_name);		

	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterface("NGUIButtonDelegate");
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUIButtonDelegate", "void OnDown(NGUIButton@)");
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUIButtonDelegate", "void OnMove(NGUIButton@, float, float)");
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUIButtonDelegate", "void OnUp(NGUIButton@)");
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUIButtonDelegate", "void OnHighlightOn(NGUIButton@)");
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUIButtonDelegate", "void OnHighlightOff(NGUIButton@)");	

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetDelegate(NGUIButtonDelegate@ delegate)", CALLWRAPPERNAMEAS(NBtnScriptSetDelegate));
}

void NGUIButton::NBtnScriptSetDelegate(asIScriptGeneric *gen)
{		
	asIScriptObject* scr_object = (asIScriptObject*)gen->GetArgObject(0);	

	asITypeInfo* tp_info = scr_object->GetObjectType();	

	asIScriptFunction* scr_func;
	
	scr_func = tp_info->GetMethodByDecl("void OnDown(NGUIButton@)", false);

	if (scr_func)
	{		
		callback_ms_down.Set(scr_func, scr_object);
	}

	scr_func = tp_info->GetMethodByDecl("void OnMove(NGUIButton@, float, float)", false);

	if (scr_func)
	{		
		callback_ms_move.Set(scr_func, scr_object);
	}

	scr_func = tp_info->GetMethodByDecl("void OnUp(NGUIButton@)", false);

	if (scr_func)
	{		
		callback_ms_up.Set(scr_func, scr_object);
	}

	scr_func = tp_info->GetMethodByDecl("void OnHighlightOn(NGUIButton@)", false);

	if (scr_func)
	{		
		callback_ms_hgl_on.Set(scr_func, scr_object);
	}

	scr_func = tp_info->GetMethodByDecl("void OnHighlightOff(NGUIButton@)", false);

	if (scr_func)
	{		
		callback_ms_hgl_off.Set(scr_func, scr_object);
	}
}

#ifdef EDITOR
Color NGUIButton::GetDebugColor()
{
	return COLOR_CYAN;
}
#endif

void NGUIButton::Release()
{
	DelUpdate((TaskHolder::Task)&NGUIButton::CheckPress);	
	
	NGUIWidget::Release();
}