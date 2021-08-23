
#include "NGUIService.h"
#include "NGUIWidget.h"

#ifdef EDITOR
#include "Common/Utils.h"
#endif
#include "TextProcessing.h"

NGUIService* NGUIService::instance = null;

#ifdef EDITOR
int NGUIService::display_width = 1024;
int NGUIService::display_height = 768;
#endif

SCENESERVICE(NGUIService, "NGUIService", 0)

void NGUIService::Init()
{	
	symbol_widget = null;

    pFilledTexture = null;
    pFilledTexture =  en_core.Render()->LoadTexture("Editor/editboxselection.png", false, _FL_);

#ifdef EDITOR
	checker_tex = en_core.Render()->LoadTexture("Editor/checker.png", false, _FL_);	
#endif

	instance = this;

	h_mul_RefToScreen = 1.0f;
	v_mul_RefToScreen = 1.0f;
	h_mul_ScreenToRef = 1.0f;
	v_mul_ScreenToRef = 1.0f;

	cur_rwidth = -1;
	cur_rheight = -1;
	
	render_changed = false;

	ref_width = -1;
	ref_height = -1;

	en_core.Render()->Tasks()->AddTask("gui", this, this, (TaskHolder::Task)&NGUIService::ExecuteCalls, 0);

	tech = en_core.Render()->CreateRenderTechnique("NGUI");
	tech_alpha = null;//en_core.Render()->CreateRenderTechnique("NGUIA");
	tech_video = null;// en_core.Render()->CreateRenderTechnique("GUI_Video");
	tech_mask = en_core.Render()->CreateRenderTechnique("NGUIMASK");

	vbuffer = en_core.Render()->CreateVertexBuffer(sizeof(SpriteVertex) * 4,sizeof(SpriteVertex), _FL_);

	vbuffer->StartDesc();
	vbuffer->AddDescElem(VertexBuffer::position,VertexBuffer::float3, "position");
	vbuffer->AddDescElem(VertexBuffer::texcoord,VertexBuffer::float2, "texCoord");			
	vbuffer->EndDesc();

	{
		SpriteVertex* Data = (SpriteVertex*)vbuffer->Lock();			

		Data[0].Position.x = 0.0f;
		Data[1].Position.x = 0.0f;
		Data[2].Position.x = 1.0f;
		Data[3].Position.x = 1.0f;

		Data[0].Position.y = 0.0f;
		Data[1].Position.y = 1.0f;
		Data[2].Position.y = 0.0f;
		Data[3].Position.y = 1.0f;

		Data[0].Position.z = 0.0f;
		Data[1].Position.z = 0.0f;
		Data[2].Position.z = 0.0f;
		Data[3].Position.z = 0.0f;

		Data[0].Texcoords.x = 0.0f;
		Data[1].Texcoords.x = 0.0f;
		Data[2].Texcoords.x = 1.0f;
		Data[3].Texcoords.x = 1.0f;

		Data[0].Texcoords.y = 0.0f;
		Data[1].Texcoords.y = 1.0f;
		Data[2].Texcoords.y = 0.0f;
		Data[3].Texcoords.y = 1.0f;

		vbuffer->Unlock();
	}

	vbuffer_mask = en_core.Render()->CreateVertexBuffer(sizeof(SpriteVertexMask) * 4,sizeof(SpriteVertexMask), _FL_);

	vbuffer_mask->StartDesc();
	vbuffer_mask->AddDescElem(VertexBuffer::position,VertexBuffer::float3, "position");
	vbuffer_mask->AddDescElem(VertexBuffer::texcoord,VertexBuffer::float2, "texCoord");			
	vbuffer_mask->AddDescElem(VertexBuffer::texcoord,VertexBuffer::float2, "texCoord2");			
	vbuffer_mask->EndDesc();

	{
		SpriteVertexMask* Data = (SpriteVertexMask*)vbuffer_mask->Lock();			

		Data[0].Position.x = 0.0f;
		Data[1].Position.x = 0.0f;
		Data[2].Position.x = 1.0f;
		Data[3].Position.x = 1.0f;

		Data[0].Position.y = 0.0f;
		Data[1].Position.y = 1.0f;
		Data[2].Position.y = 0.0f;
		Data[3].Position.y = 1.0f;

		Data[0].Position.z = 0.0f;
		Data[1].Position.z = 0.0f;
		Data[2].Position.z = 0.0f;
		Data[3].Position.z = 0.0f;

		Data[0].Texcoords.x = 0.0f;
		Data[1].Texcoords.x = 0.0f;
		Data[2].Texcoords.x = 1.0f;
		Data[3].Texcoords.x = 1.0f;

		Data[0].Texcoords.y = 0.0f;
		Data[1].Texcoords.y = 1.0f;
		Data[2].Texcoords.y = 0.0f;
		Data[3].Texcoords.y = 1.0f;

		Data[0].Texcoords2.x = 0.0f;
		Data[1].Texcoords2.x = 0.0f;
		Data[2].Texcoords2.x = 1.0f;
		Data[3].Texcoords2.x = 1.0f;

		Data[0].Texcoords2.y = 0.0f;
		Data[1].Texcoords2.y = 1.0f;
		Data[2].Texcoords2.y = 0.0f;
		Data[3].Texcoords2.y = 1.0f;

		vbuffer_mask->Unlock();
	}

	//shape_tech = en_core.Render()->CreateRenderTechnique("GUI_shape");
	//shape_tech_alpha = en_core.Render()->CreateRenderTechnique("GUI_A_shape");

	shape_vbuffer = en_core.Render()->CreateVertexBuffer(sizeof(SpriteVertex) * 4, sizeof(SpriteVertex), _FL_);

	shape_vbuffer->StartDesc();
	shape_vbuffer->AddDescElem(VertexBuffer::position,VertexBuffer::float3, "position");
	shape_vbuffer->AddDescElem(VertexBuffer::texcoord,VertexBuffer::float2, "texCoord");			
	shape_vbuffer->EndDesc();

	{
		SpriteVertex* Data = (SpriteVertex*)shape_vbuffer->Lock();			

		Data[0].Position.x = 1.0f;
		Data[0].Position.y = 0.0f;
		Data[0].Position.z = 0.0f;
		Data[0].Texcoords.x = 0.0f;
		Data[0].Texcoords.y = 0.0f;

		Data[1].Position.x = 0.0f;
		Data[1].Position.y = 1.0f;
		Data[1].Position.z = 0.0f;
		Data[1].Texcoords.x = 0.0f;
		Data[1].Texcoords.y = 0.0f;

		Data[2].Position.x = 0.0f;
		Data[2].Position.y = 0.0f;
		Data[2].Position.z = 0.0f;
		Data[2].Texcoords.x = 1.0f;
		Data[2].Texcoords.y = 0.0f;

		Data[3].Position.x = 0.0f;
		Data[3].Position.y = 0.0f;
		Data[3].Position.z = 0.0f;
		Data[3].Texcoords.x = 0.0f;
		Data[3].Texcoords.y = 1.0f;
	
		shape_vbuffer->Unlock();
	}
    

	core->AddTask(this,this, (TaskHolder::Task)&NGUIService::CalcRefMultiplaers, lvl_pre1, -1.0f);
	core->AddTask(this,this, (TaskHolder::Task)&NGUIService::UpdateChildsPos, lvl_camera1, -1.0f);

#ifdef EDITOR	
	debug_color = (ReadINI("settings.ini", 0, "GUI", "debug_colors") > 0);
	trace_hovered_widget = (ReadINI("settings.ini", 0, "GUI", "trace_hovered_widget") > 0);
#endif

	locker.Init();
    TextProcessing::StaticData::Init();

    CalcRefMultiplaers(0.0f, 0);

}

void NGUIService::AddSpriteCall(Texture* texture, TextureInstance* textureMask, float x, float y, float wdt, float hgt, Color& color, Matrix& transform, float depth,
							   float u, float v, float du, float dv)
{			
	if (!texture) return;

	if (draw_mode != 1)
	{
		if (textureMask)
		{		
			en_core.Render()->SetVertexBuffer( 0, vbuffer_mask );
		}
		else
		{
			en_core.Render()->SetVertexBuffer( 0, vbuffer );
		}

		last_tech = null;		

		draw_mode = 1;
		state = 0;
	}

	RenderTechnique* cur_tech = tech;

#ifdef ANDROID
	/*if (texture->HasSecData())
	{
		cur_tech = tech_alpha;
	}

	if ( texture->IsVideo() )
	{
		cur_tech = tech_video;
	}*/
#endif

	//DIRTY
	if (textureMask)
	{		
		en_core.Render()->SetVertexBuffer( 0, vbuffer_mask );
	}
	else
	{
		en_core.Render()->SetVertexBuffer( 0, vbuffer );
	}

	if (textureMask)
	{
		cur_tech = tech_mask;
	}

	if (cur_tech != last_tech)
	{
		en_core.Render()->SetRenderTechnique(cur_tech);
		last_tech = cur_tech;
	}
	
	cur_tech->PS_SetTexture("diffuseMap", texture);

	if (textureMask)
	{		
#ifdef PC
		cur_tech->PS_SetTexture("maskMap", textureMask);
#else
		cur_tech->PS_SetTexture("maskMap", 1, textureMask);	
#endif
	}

	params[0] = Vector4(u, v, du, dv);
	params[1] = Vector4(x, y, wdt, hgt);

	if (depth < -0.5f)
	{
		if (state != 0)
		{
			en_core.Render()->State()->SetDepthTest(0);
			en_core.Render()->State()->SetAlphaTest(0);
			state = 0;
		}

		params[2] = Vector4(en_core.Render()->Display()->GetWidth(),
							en_core.Render()->Display()->GetHeight(),
							0.5f,
							0.0f);
	}
	else
	{
		if (state != 1)
		{
			en_core.Render()->State()->SetDepthTest(1);
			en_core.Render()->State()->SetAlphaTest(1);		
			state = 1;
		}

		params[2] = Vector4(en_core.Render()->Display()->GetWidth(),
							en_core.Render()->Display()->GetHeight(),
							1.0f - depth,
							0.0f);
	}
	
	if (textureMask)
	{			
		params[3] = Vector4(0, 0, 1.0f, 1.0f);
		cur_tech->VS_SetVector4Array("desc", 4, &params[0].x);
	}
	else
	{
		cur_tech->VS_SetVector4Array("desc", 3, &params[0].x);
	}

	cur_tech->VS_SetMatrix("transform", transform);
	cur_tech->PS_SetVector4("color", (Vector4&)color);

	en_core.Render()->DrawPrimitive( RenderService::triangleStrip, 0, 2 );	
}

void NGUIService::AddTopSpriteCall(Texture* texture, float x, float y, float wdt, float hgt, Color& color, Matrix& transform, float depth, float u, float v, float du, float dv)
{
	draws.push_back(DrawCall());
	DrawCall* draw_call = &draws[draws.size()-1];
	
	draw_call->tex = texture;	
	draw_call->scr_x = x;
	draw_call->scr_y = y;
	draw_call->width = wdt;
	draw_call->height = hgt;	

	draw_call->color = color;	
	draw_call->transform = transform;

	draw_call->depth = -1.0f;	

	draw_call->u = u;	
	draw_call->du = du;	

	draw_call->v = v;
	draw_call->dv = dv;
}


void NGUIService::AddTopPrintCall(FontInstance* font, Matrix& transform,const ClipRect& rect, float font_scale, Color color, TextAlign align, const char* text)
{
    drawsText.push_back(DrawTextCall());
    DrawTextCall& draw_call = drawsText[drawsText.size()-1];
    draw_call.text = text;
    draw_call.align = align;
    draw_call.color = color;
    draw_call.font_scale = font_scale;
    draw_call.rect = rect;
    draw_call.transform = transform;
    draw_call.font = font;
    font->CalcLinesData(text, draw_call.line_indexes, rect.x2 - rect.x1, font_scale, align);

}

void NGUIService::AddSquareCall(TextureInstance* texture, float depth,
							   Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4,
							   Vector2 uv1, Vector2 uv2, Vector2 uv3, Vector2 uv4, Color color)
{			
	if (!texture) return;

	if (draw_mode != 2)
	{
		en_core.Render()->SetVertexBuffer( 0, shape_vbuffer );
		last_tech = null;		

		draw_mode = 2;
		state = 0;
	}

	RenderTechnique* cur_tech = shape_tech;

	if (cur_tech != last_tech)
	{
		en_core.Render()->SetRenderTechnique(cur_tech);
		last_tech = cur_tech;
	}
	
	cur_tech->PS_SetTexture("diffuseMap", texture);

	params[0] = Vector4(p1.x, p4.x, p2.x, p3.x);
	params[1] = Vector4(p1.y, p4.y, p2.y, p3.y);
	params[2] = Vector4(uv1.x, uv4.x, uv2.x, uv3.x);
	params[3] = Vector4(uv1.y, uv4.y, uv2.y, uv3.y);

	if (depth < -0.5f)
	{
		if (state != 0)
		{
			en_core.Render()->State()->SetDepthTest(0);
			en_core.Render()->State()->SetAlphaTest(0);
			state = 0;
		}

		params[4] = Vector4(en_core.Render()->Display()->GetWidth(),
							en_core.Render()->Display()->GetHeight(),
							0.5f,
							0.0f);
	}
	else
	{
		if (state != 1)
		{
			en_core.Render()->State()->SetDepthTest(1);
			en_core.Render()->State()->SetAlphaTest(1);		
			state = 1;
		}

		params[4] = Vector4(en_core.Render()->Display()->GetWidth(),
							en_core.Render()->Display()->GetHeight(),
							1.0f - depth,
							0.0f);
	}

	cur_tech->VS_SetVector4Array("desc", 5, &params[0].x);
	cur_tech->PS_SetVector4("color", (Vector4&)color);

	en_core.Render()->DrawPrimitive( RenderService::triangleStrip, 0, 2 );
}

void NGUIService::AddPrintCall(FontInstance* font, Matrix& transform, ClipRect* rect, std::vector<FontRes::LineData>* line_indices, float font_scale, Color color, TextAlign align,  const char* text)
{	
	draw_mode = 3;
	state = 0;
	
	if (line_indices)
	{
		font->Print(transform, rect, *line_indices, font_scale, color, align, text);	
	}
	else
	{
		font->CalcLinesData(text, line_indexes, rect->x2 - rect->x1, font_scale, align);
		font->Print(transform, rect, line_indexes, font_scale, color, align, text);	
	}
}

void NGUIService::ExecuteCalls(float dt, int level)
{
	draw_mode = 0;
	state = -1;

	last_tech = null;

	locker.Lock();	
	
	if (symbol_widget && core->IsEditorMode())
	{
		symbol_widget->Render(dt, level);
	}
	else
	{
#ifdef EDITOR
		float scr_w = display_width * h_mul_RefToScreen;
		float scr_h = display_height* v_mul_RefToScreen;

		float x = NGUIWidget::editor_offset_x + (core->Render()->Display()->GetWidth() - scr_w) * 0.5f;
		float y = NGUIWidget::editor_offset_y;				

		AddSpriteCall(checker_tex, null, x, y, scr_w, scr_h,
					  COLOR_WHITE, Matrix(), 0.5f, 0, 0, scr_w / 16.0f, scr_h / 16.0f);
		
#endif

		for (int i=0;i<(int)top_widgets.size();i++)
		{
			if (top_widgets[i]->Scene()->IsTaskHoldersOwnerActive()) 
                top_widgets[i]->Render(dt, level);
		}
	}

	if (draws.size() > 0)
	{
		for (int i=0;i<draws.size();i++)
		{
			DrawCall& darw_call = draws[i];

			AddSpriteCall(darw_call.tex, null, darw_call.scr_x, darw_call.scr_y, darw_call.width, darw_call.height, darw_call.color, darw_call.transform, darw_call.depth, darw_call.u, darw_call.v, darw_call.du, darw_call.dv);
		}

		draws.clear();
	}

    if (drawsText.size() > 0)
    {
        for (int i=0;i<drawsText.size();i++)
        {
            DrawTextCall& draw_call = drawsText[i];
            AddPrintCall(draw_call.font,draw_call.transform,&draw_call.rect,&draw_call.line_indexes,draw_call.font_scale,draw_call.color,draw_call.align,draw_call.text.c_str());
        }
        drawsText.clear();
    }

	en_core.Render()->State()->SetDepthTest(0);
	en_core.Render()->State()->SetAlphaTest(0);
	state = 0;	

	locker.UnLock();
}

void NGUIService::ClearWidgetsLists()
{
	locker.Lock();
    top_widgets.clear();
    symbol_widgets.clear();
    locker.UnLock();
}

void NGUIService::AddTopWidget(NGUIWidget* child,bool bSort)
{
	locker.Lock();

	for (int i=0;i<(int)top_widgets.size();i++)
	{
		if (top_widgets[i] == child)
		{	
			locker.UnLock();

			return;
		}
	}

	top_widgets.push_back(child);
    if(bSort)
    {
	    for (int i=top_widgets.size()-1;i>0;i--)
	    {
		    if (top_widgets[i-1]->weight >= top_widgets[i]->weight)
		    {
			    NGUIWidget* tmp = top_widgets[i-1];
			    top_widgets[i-1] = top_widgets[i];
			    top_widgets[i] = tmp;

			    continue;
		    }

		    break;
	    }
    }
    else
        child->weight = top_widgets.size() - 1;
	locker.UnLock();
}

void NGUIService::DelTopWidget(NGUIWidget* child)
{
	locker.Lock();

	for (int i=0;i<(int)top_widgets.size();i++)
	{
		if (top_widgets[i] == child)
		{			
			top_widgets.erase(top_widgets.begin() + i);
			break;
		}		
	}	

	locker.UnLock();
}

void NGUIService::AddSymbolWidget(NGUIWidget* symbol)
{
	locker.Lock();

	for (int i=0;i<(int)symbol_widgets.size();i++)
	{
		if (symbol_widgets[i] == symbol)
		{	
			locker.UnLock();

			return;
		}
	}

	symbol_widgets.push_back(symbol);

	locker.UnLock();
}

void NGUIService::DelSymbolWidget(NGUIWidget* symbol)
{
	locker.Lock();

	for (int i=0;i<(int)symbol_widgets.size();i++)
	{
		if (symbol_widgets[i] == symbol)
		{			
			symbol_widgets.erase(symbol_widgets.begin() + i);
			break;
		}		
	}	

	locker.UnLock();
}

NGUIWidget* NGUIService::FindSymbolWidget(ISceneInstance* scene, const char* name)
{
	for (int i=0;i<(int)symbol_widgets.size();i++)
	{
		if (symbol_widgets[i]->Scene() == scene && String::IsEqual(symbol_widgets[i]->GetName(), name))
		{	
			locker.UnLock();

			return symbol_widgets[i];
		}
	}

	return null;
}

void NGUIService::UpdateChildsPos(float dt, int level)
{
	NGUIWidget::ms_prev_pos_x = NGUIWidget::ms_pos_x;
	NGUIWidget::ms_prev_pos_y = NGUIWidget::ms_pos_y;

	NGUIWidget::ms_pos_x = core->Controls()->GetTouchX(0);
	NGUIWidget::ms_pos_y = core->Controls()->GetTouchY(0);	

    NGUIWidget* prev_wgt = NGUIWidget::mscover_widget;
	NGUIWidget::mscover_widget = null;
    
	locker.Lock();    

	if (symbol_widget && core->IsEditorMode())
	{
		symbol_widget->UpdateChildsPos();				
		symbol_widget->CalcColors();
		symbol_widget->CheckMouseCover();
	}
	else
	{
        if (render_changed)
        {
            for (int i=0;i<(int)top_widgets.size();i++)
            {
                if (top_widgets[i]->Scene()->IsTaskHoldersOwnerActive())
                {								
                    top_widgets[i]->UpdateChildsPos();					
                }
            }
            render_changed = false;
        }
        for(unsigned i = 0;i < touch_regions.size();++i)
        {
            const TouchRegion& region = touch_regions[i];
            if(region.IsIn(NGUIWidget::ms_pos_x,NGUIWidget::ms_pos_y))
            {
                NGUIWidget::mscover_widget = region.pWidget;
                break;
            }
        }

        if(NGUIWidget::mscover_widget == null)
        {
		    for (int i=0;i<(int)top_widgets.size();i++)
		    {
			    if (top_widgets[i]->Scene()->IsTaskHoldersOwnerActive())
			    {
				    top_widgets[i]->CheckMouseCover();
			    }
		    }
        }

		if (NGUIWidget::mscover_widget && !NGUIWidget::mscover_widget->IsFocused())
		{
			if (core->Controls()->CheckTouch(0, ControlsService::FastActivated, NGUIWidget::mscover_widget) )
			{
				NGUIWidget::mscover_widget->SetFocused(true);

			}
		}
		if (!NGUIWidget::mscover_widget && NGUIWidget::focusedWidget)
		{
			if (core->Controls()->CheckTouch(0, ControlsService::Activated, null))
			{
				NGUIWidget::focusedWidget->SetFocused(false);
			}
		}
	}
    
//#ifdef EDITOR
    trace_hovered_widget = true;
    
    if (NGUIWidget::mscover_widget && prev_wgt != NGUIWidget::mscover_widget)
	{
        if (trace_hovered_widget && !core->IsEditorMode())
        {
            core->TraceTo("Debug","wgt %s", NGUIWidget::mscover_widget->GetName());
        }
	}
	else
    if (NGUIWidget::mscover_widget_not_covered)
    {
        if (trace_hovered_widget && !core->IsEditorMode())
        {
			core->TraceTo("Debug","not covered wgt %s", NGUIWidget::mscover_widget_not_covered->GetName());
        }
    }
//#endif
    
	//en_core.Render()->Debug()->PrintText(10, 10, up_left, COLOR_GREEN, "Updates - %i", NGUIWidget::num_updates);
	NGUIWidget::num_updates = 0;
    
    locker.UnLock();
}

void NGUIService::CalcRefMultiplaers(float dt, int level)
{
    if (ref_height != en_core.Render()->Display()->GetReferenceHeight())
	{
		ref_height = en_core.Render()->Display()->GetReferenceHeight();
		cur_rwidth = -1;
	}
	
	if (cur_rwidth == -1 || cur_rwidth != en_core.Render()->Display()->GetWidth() || cur_rheight != en_core.Render()->Display()->GetHeight())
	{
		cur_rwidth = en_core.Render()->Display()->GetWidth();
		cur_rheight = en_core.Render()->Display()->GetHeight();
		
		float aspect = 1.0f / ((float)cur_rheight / (float)cur_rwidth);
        
		h_mul_RefToScreen = 1.0f /ref_height * (float)cur_rwidth / aspect;
		v_mul_RefToScreen = 1.0f / ref_height * (float)cur_rheight;
        
		h_mul_ScreenToRef = 1.0f / (float)cur_rwidth * ref_height * aspect;
		v_mul_ScreenToRef = 1.0f / (float)cur_rheight * ref_height;
		
		ref_width = cur_rwidth * h_mul_ScreenToRef;
        
		render_changed = true;
        TextProcessing::Menu::CreateFont();

	}
	else
	{
		render_changed = false;
	}
}

#ifdef EDITOR
bool NGUIService::DebugColor()
{
	return debug_color;
}
#endif

void NGUIService::Release()
{
	en_core.Render()->Tasks()->DelTask("gui", this);
    TextProcessing::StaticData::Release();
	if (vbuffer)
	{
		vbuffer->Release();
		vbuffer = null;
	}
    RELEASE(pFilledTexture)
}

void NGUIService::AddTouchRegion(float x,float y,float w,float h,NGUIWidget* widget)
{
    for(unsigned i = 0;i < touch_regions.size();++i)
    {
        if(widget = touch_regions[i].pWidget)
        {
            touch_regions[i].AddRect(x,y,w,h);
            return;
        }
    }
    touch_regions.push_back(TouchRegion(x,y,w,h,widget));
}

void NGUIService::RemoveTouchRegion(NGUIWidget* widget)
{
    for(TTouchRegions::iterator it = touch_regions.begin();it != touch_regions.end();++it)
        if(it->pWidget == widget)
        {
            touch_regions.erase(it);
            break;
        }
}