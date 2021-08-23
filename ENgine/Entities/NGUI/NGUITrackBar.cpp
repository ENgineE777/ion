
#include "NGUITrackBar.h"
#include "Common/Utils.h"

/*
ENTITY(NGUITrackBar)
ENTITY_IS_2D_OBJECT
ENTITY_PARAMS(NGUITrackBar,"NGUITrackBar","NGUI", "TrackBar")

void NGUITrackBar::Init()
{	
	NGUIWidget::Init();
								
	PROP_REG( skinName )
	PROP_REG( skinElemName )		
	PROP_REG( skinElemBarName )			
	PROP_REG( trackColor )	
	
	PROP_REG( thumb_tex_name )
	PROP_REG( thumbColor )		
	
	PROP_REG( trackType )	
	
	PROP_REG( thumb_width )
	PROP_REG( thumb_height )		
	PROP_REG( thumb_offset_left )	
	PROP_REG( thumb_offset_right )
	PROP_REG( minValue )		
	PROP_REG( maxValue )	
	PROP_REG( stepValue )
	PROP_REG( initValue )		

	val = 0;	
	in_drag = false;	

	thumb_tex = null;

	need_additional_clip = false;

	skin = null;
	skinElem = -1;
	skinElemBar = -1;

	start_pos = 0.0f;
	end_pos = 0.0f;
	thum_pos = 0.0f;

	SetUpdate((TaskHolder::Task)&NGUITrackBar::CheckPress, lvl_pre5 + 3);	
	SetUpdate((TaskHolder::Task)&NGUITrackBar::UpdateSkin, lvl_pre5+1);

	add_clip_x =-10000000.0f;
	add_clip_y =-10000000.0f;
	add_clip_x2 = 10000000.0f;
	add_clip_y2 = 10000000.0f;
}
	
void NGUITrackBar::ReInit()
{
	NGUIWidget::ReInit();			
	
	RELEASE(thumb_tex)
	thumb_tex = en_core.Render()->LoadTexture(thumb_tex_name, false, _FL_);

	skin = (NGUISkin*)Scene()->Find(skinName, "NGUISkin");	

	if (skin)
	{
		if (skinElemName[0]) skinElem = skin->GetSkinElemIndex(skinElemName);		
		if (skinElemBarName[0]) skinElemBar = skin->GetSkinElemIndex(skinElemBarName);		
	}
	else
	{
		skinElem = -1;
		skinElemBar = -1;
	}

	val = initValue;
}

void NGUITrackBar::UpdateSkin(float dt, int level)
{	
	if (skin)
	{
		if (skinElemName[0]) skinElem = skin->GetSkinElemIndex(skinElemName);		
		if (skinElemBarName[0]) skinElemBar = skin->GetSkinElemIndex(skinElemBarName);		
	}
	else
	{
		skinElem = -1;
		skinElemBar = -1;
	}
	
#ifndef EDITOR
	DelUpdate((TaskHolder::Task)&NGUITrackBar::UpdateSkin);
#endif
}

void NGUITrackBar::RenderWidget()
{		   
	Color tr_color = trackColor;
	tr_color.a = alpha;

	Color tmb_color = thumbColor;
	tmb_color.a = alpha;

	if (parent)
	{
		tr_color *= parent->overColor;
		tr_color.a = alpha * parent->alpha;
	
		tmb_color *= parent->overColor;
		tmb_color.a = alpha * parent->alpha;
	}

	if (skinElem != -1)
	{
		skin->Draw(this, skinElem, Vector2(0, 0), Vector2(scr_w, scr_h), tr_color, 0.0f);
	}
		
#ifdef EDITOR
	float k = 1.0f;

	if (core->IsEditorMode())
	{
		k = (initValue - minValue)/(float)(maxValue - minValue);
	}
	else
	{
		k = (val - minValue)/(float)(maxValue - minValue);
	}
#else
	float k = (val - minValue)/(float)(maxValue - minValue);
#endif

	if (skinElemBar != -1)
	{
		need_additional_clip = true;

		add_clip_x = 0;
		add_clip_y = 0;

		if (trackType == 0)
		{
			add_clip_x2 = scr_w * k;
			add_clip_y2 = scr_h;
		}
		else
		{
			add_clip_x2 = scr_w;
			add_clip_y2 = scr_h * k;
		}

		skin->Draw(this, skinElemBar, Vector2(0, 0), Vector2(scr_w, scr_h), tr_color, 0.0f);

		need_additional_clip = false;
	}
	

	if (thumb_tex)
	{
		float thumb_w = ConvertHorz(thumb_width);
		float thumb_h = ConvertVert(thumb_height);

		float pos_x = 0.0f;
		float pos_y = 0.0f;		

		if (trackType == 0)
		{
			start_pos.x = ConvertHorz(thumb_offset_left) + thumb_w * 0.5f;
			start_pos.y = scr_h * 0.5f;	

			end_pos.x = scr_w - thumb_w * 0.5f - ConvertHorz(thumb_offset_right);
			end_pos.y = scr_h * 0.5f;										

			limit.x = scr_x + start_pos.x;
			limit.y = scr_x + end_pos.x;
		}
		else
		{
			start_pos.x = scr_w * 0.5f;
			start_pos.y = ConvertVert(thumb_offset_left) + thumb_h * 0.5f;	

			end_pos.x = scr_w * 0.5f;
			end_pos.y = scr_h - thumb_h * 0.5f - ConvertVert(thumb_offset_right);							

			limit.x = scr_y + start_pos.y;
			limit.y = scr_y + end_pos.y;
		}

		thum_pos.Lerp(start_pos, end_pos, k);		

		thum_pos.x -= thumb_w * 0.5f;
		thum_pos.y -= thumb_h * 0.5f;		

		AddDrawCall(thumb_tex, thum_pos.x, thum_pos.y, thumb_w, thumb_h, tmb_color, 0.0f, 0.0f,0.0f,1.0f,1.0f);		
	}	
}

void NGUITrackBar::CheckPress(float dt, int level)
{
	if (Scene()->IsRunning())
	{
		if (mscover_widget == this)
		{
			if (IsActive() && IsVisible())
			{
				if (core->Controls()->CheckTouch(0, ControlsService::ActivatedExclusive, this))
				{
					in_drag = true;
				}				

				if (in_drag)
				{
					if (!core->Controls()->CheckTouch(0, ControlsService::ActiveExclusive, this))
					{
						in_drag = false;
					}
				}

				if (in_drag)
				{
					float ms = 0.0f;

					if (trackType == 0)
					{
						ms = core->Controls()->GetTouchX(0);
					}
					else
					{
						ms = core->Controls()->GetTouchY(0);
					}

					if (ms < limit.x) ms = limit.x;
					if (ms > limit.y) ms = limit.y;

					float k = (ms - limit.x) / (limit.y - limit.x);

					k = minValue + (maxValue + 1 - minValue) * k;
					val = (int)(k);												

					if (val < minValue)  val = minValue;
					if (val > maxValue)  val = maxValue;

					if (val != minValue && val != maxValue)
					{
						int delta = val - minValue;
						val = (int)((float)delta / (float)stepValue) * stepValue + minValue;
					}											
				}
			}
		}
	}
}

void NGUITrackBar::MethodsToScript()
{
	NGUIWidget::MethodsToScript();
	
	core->Script()->Machine()->RegisterObjectMethod(Desc()->GetName(), "void SetValue(int set_value)", asMETHOD(NGUITrackBar,SetValue), asCALL_THISCALL);	
	core->Script()->Machine()->RegisterObjectMethod(Desc()->GetName(), "int GetValue()", asMETHOD(NGUITrackBar,GetValue), asCALL_THISCALL);					
}

void NGUITrackBar::SetValue(int set_val)
{
	val = set_val;
}

int NGUITrackBar::GetValue()
{
	return val;
}

void NGUITrackBar::AddDrawCall(TextureInstance* tex, float set_scr_x, float set_scr_y, float wdt, float hgt, Color color, float rotate, float u, float v, float du, float dv)
{
	if (!IsVisible()) return;

	if (need_additional_clip)
	{
		if (!ClipSprite(set_scr_x, set_scr_y, wdt, hgt,
						u, v, du, dv,
						add_clip_x, add_clip_x2, add_clip_y, add_clip_y2))
		{					
				return;
		}			
	}

	draws.push_back(DrawCall());
	DrawCall* draw_call = &draws[draws.size()-1];
	
	draw_call->tex = tex;	
	draw_call->scr_x = set_scr_x;
	draw_call->scr_y = set_scr_y;
	draw_call->width = wdt;
	draw_call->height = hgt;	

	draw_call->color = color;	
	draw_call->transform = global_mat;

	draw_call->depth = -1.0f;	

	draw_call->u = u;	
	draw_call->du = du;	

	draw_call->v = v;
	draw_call->dv = dv;
}

void NGUITrackBar::Release()
{
	RELEASE(thumb_tex)

	NGUIWidget::Release();
}*/