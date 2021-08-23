
#include "NGUILabel.h"
#include "Core/Network/NetworkService.h"

ENTITY(NGUILabel)
ENTITY_IS_2D_OBJECT
ENTITY_PARAMS(NGUILabel,"NGUILabel","NGUI", "Label")

#ifdef EDITOR

void StartEditLabelGUITexture(PropertiesOwner* owner)
{
	NGUILabel* label = (NGUILabel*)owner;
	TextureEntry::StartEditGUITexture(&label->slice);
}

#endif

#define MENU_WAIT_TIME 1.3f
#define MENU_SIZE 1

#define OLD_VERSION

void NGUILabel::RegisterProperties()
{
	NGUIWidget::RegisterProperties();	

	PROP_REG( filled )
	PROP_REG( filled_color )
	PROP_REG( filled_alpha )

	PROP_REG( num_visible_lines )

	PROP_REG(selection_enable)
	PROP_REG(text_cursor_width)
	PROP_REG(text_cursor_height)
	PROP_REG(cursor_Color)
	PROP_REG(selection_Color)

	PROP_REG( fontUseEntity )
	PROP_REG( fontEntityName )
	PROP_REG( fontDesc )				
	PROP_REG( fontScale )

	PROP_REG( textureUseEntity )
	PROP_REG( textureEntityName )

#ifdef EDITOR
	PROP_REG( texEdit )	
#endif

	PROP_REG( texMaskName )

	PROP_REG( url_fade_time )

	PROP_REG(url_load_result)				

#ifdef EDITOR
	PROP_REG(generateFont)			
#endif
}

void NGUILabel::Init()
{
	NGUIWidget::Init();	    

	need_release_font = true;	

	font_entity = null;	

	cam_texture = null;

	slices.push_back(TexSlice(&slice, &textureUseEntity,
#ifdef EDITOR
							  get_textureEntityNameprop_inst(), get_texEditprop_inst(),
#endif
							  textureEntityName.c_str()));
}
	
void NGUILabel::ReInit()
{
	NGUIWidget::ReInit();	

	TextureInstance* prev_texMask = texMask;
	texMask = en_core.Render()->LoadTexture(texMaskName.c_str(), false, _FL_);
	RELEASE(prev_texMask)

#ifdef EDITOR	
	if (need_release_font != !fontUseEntity)
	{
		if (ed_rollout)
		{
			SET_SKIP(fontEntityName, !fontUseEntity )
		
			SET_SKIP(fontDesc, fontUseEntity )			

			ed_rollout->CalcPos();
			ed_rollout->Redraw();
		}
	}	
#endif
	
	if (need_release_font)
	{
		RELEASE(font)
	}
#ifdef EDITOR
	else
	{
		if (font_entity)
		{
			font_entity->DelUseFont(this);
		}
	}
#endif

    need_release_font = !fontUseEntity;

	if (fontUseEntity)
	{
		NGUIFont* fnt = (NGUIFont*)Scene()->Find(fontEntityName.c_str(), "NGUIFont");
		
		if (fnt)
		{
#ifdef EDITOR
			font_entity = fnt;
			font_entity->AddUseFont(this);
#endif
			font = fnt->font.font;			
		}
		else
		{
			font = null;
		}
	}
	else
	{
		font = en_core.Font()->CreateFont(fontDesc.name, fontDesc.bold, fontDesc.italic, ConvertVert(fontDesc.height), fontDesc.gap, fontDesc.outline);		
		font_entity = null;
	}	

//#ifdef EDITOR
	CalcLinesData();
//#endif
    float cursorW = NGUIService::instance->h_mul_RefToScreen * text_cursor_width;
    float cursorH = NGUIService::instance->v_mul_RefToScreen * text_cursor_height;
#ifndef OLD_VERSION
    selection.Init(this,selection_Color,fontScale);
    selection.cursors[TextProcessing::Selection::eSelectionCursorStart].Init(cursor_Color,fontScale,TextProcessing::Cursor::eCursorTop,false,this,true);
    selection.cursors[TextProcessing::Selection::eSelectionCursorEnd].Init(cursor_Color,fontScale,TextProcessing::Cursor::eCursorBottom,false,this,true);
    pDraggedCursor = null;
#endif
    in_drag = false;
    nDraggDpos = 0;
    nTouchTimer = 0;
}

bool NGUILabel::Start()
{			
	if (url_load_result[0])
	{
		DEBUG_BREAK
	}

	ScriptFunction::Find2PI(&callback_tex_loaded, Scene()->ScriptModule(), url_load_result.c_str(), Desc()->GetName());

	return NGUIWidget::Start();
}

void NGUILabel::Finish()
{
	callback_tex_loaded.Reset();
	NGUIWidget::Finish();	
}

void NGUILabel::RenderWidget()
{
	float delta_time = core->GetDeltaTime();

    if(filled && NGUIService::instance->pFilledTexture)
    {
        Color buffColor = filled_color;
        buffColor.a = filled_alpha * clr.a;
        buffColor *= overColor;
        AddDrawCall(NGUIService::instance->pFilledTexture,0,0,scr_w,scr_h,buffColor,0,0,1,1);
    }

	
    if (slice.tex.texture)
    {
        if (slice.tex.texture->IsReady())
        {
			if (slice.tex.tex_loading == 1)
			{
				slice.tex.tex_loading = 0;

                if (url_load_result[0])
                {
					core->Script()->Machine()->AddPtrArg(this);
					core->Script()->Machine()->AddIntArg(0);
					core->Script()->Machine()->Call(&callback_tex_loaded);			                    
                }
			}			

            if (slice.tex.fade_in_time > 0.0f)
            {
                slice.tex.fade_in_time -= delta_time;							

                if (slice.tex.fade_in_time < 0.0f)
                {
                    slice.tex.fade_in_time = -1.0f;
                }
            }
        }
	}

	if (slice.tex.tex_loading == 2)
	{		
		core->Script()->Machine()->AddPtrArg(this);
		core->Script()->Machine()->AddIntArg(1);
		core->Script()->Machine()->Call(&callback_tex_loaded);		

		slice.tex.tex_loading = 0;
	}

	if (cam_texture)
	{
		AddDrawCall(cam_texture, 0, 0, scr_w, scr_h, clr, 0, 0, 1.0f, 1.0f);
	}
	else
	if (slice.tex.texture || slices[0].tex_entity)
	{
		Color tmp = clr;

		if (clr.a>0.01f)
		{									
			if (slice.tex.fade_in_time > 0.0f)
			{
				tmp.a *= (1.0f - slice.tex.fade_in_time / url_fade_time);
			}					

			if (slices[0].tex_entity)
			{
				TextureEntry::Draw(this, 0, 0, scr_w, scr_h, tmp, &slices[0].tex_entity->slice);
			}
			else
			{
				TextureEntry::Draw(this, 0, 0, scr_w, scr_h, tmp, &slice);
			}
		}	
	}
#ifdef EDITOR
	else
	if (!Scene()->IsRunning() && NGUIService::instance->DebugColor())
	{	
		Color color = COLOR_WHITE;
		color.a = 0.75f;

		AddDrawCall(empty_tex, 0, 0, scr_w, scr_h, color, 0, 0, 1.0f, 1.0f);				
	}
#endif
#ifndef OLD_VERSION
    NGUIService::instance->RemoveTouchRegion(this);
    if(selection.IsShow())
        selection.Render(0,0,fontScale);
#endif
	if (font)
	{		
		if (visual_text.size() > 0)
		{
			AddPrintCall(font, 0, 0, scr_w, scr_h, &line_indexes, fontScale, font_clr, visual_text.c_str());			
		}
	}
#ifndef OLD_VERSION
    if(selection_enable)
    {
        if (mscover_widget == this)
        {
            if (core->Controls()->CheckTouch(0, ControlsService::FastActivatedExclusive, this))
            {
                int menuID = -1;bool bSuccess = false;
                if(selection.IsOn())
                    menuID = selection.CheckMenuPress(ms_pos_x - scr_x,ms_pos_y - scr_y,bSuccess);
                if(bSuccess)
                {
                    if(menuID != -1)
                    {
                        if(menuID == eMenuCopy)
                        {
                            int start = selection.cursors[TextProcessing::Selection::eSelectionCursorStart].Index();
                            int count = selection.cursors[TextProcessing::Selection::eSelectionCursorEnd].Index() - selection.cursors[TextProcessing::Selection::eSelectionCursorStart].Index();
                            string txt = visual_text.substr(start,count);	
                            core->Mobile()->SetTextToClipboard(txt.c_str());
                            selection.Hide();
                        }
                    }
                }
                else
                    selection.Hide();
            }
 
            if (core->Controls()->CheckTouch(0, ControlsService::Active, this))
            {
                if(in_drag)
                {
                    bool bDragg  = false;
                    if(fabs(ms_pos_x - ms_prev_pos_x) > 0)
                    {
                        bDragg = true;
                        nDraggDpos = fabs(ms_pos_x - ms_prev_pos_x);
                    }
                    else
                        if(fabs(ms_pos_y - ms_prev_pos_y) > 0)
                        {
                            bDragg = true;
                            nDraggDpos = fabs(ms_pos_y - ms_prev_pos_y);
                        }


                        if (!pDraggedCursor)
                        {

                            if(!bDragg)
                            {
                                nTouchTimer += core->GetDeltaTime();
                                if(nTouchTimer > MENU_WAIT_TIME && nDraggDpos == 0)
                                {
                                    nTouchTimer = 0;
                                    nDraggDpos = 1;
                                    static TextProcessing::Cursor temp_curret;
                                    temp_curret.FindPosition(ms_pos_x - scr_x,ms_pos_y - scr_y,font,&line_indexes,visual_text,fontScale);
                                    int start = 0,end = 0;
                                    TextProcessing::TextUtils::GetWordBorders(font,&line_indexes,visual_text,temp_curret,start,end,0,scr_w,fontScale);
                                    if(selection.Select(font,&line_indexes,visual_text,temp_curret,start,end,fontScale))
                                    {
                                        TextProcessing::Menu::TMenuIds  ids;
                                        ids.push_back(eMenuCopy);
                                        selection.CreateMenu(ids,TextProcessing::Menu::vLabelMenuText);
                                        pDraggedCursor = null;
                                    }
                                } 

                            }
                        }
                        else
                        {
                            if(bDragg)
                                pDraggedCursor->FindPosition(ms_pos_x - scr_x + pDraggedCursor->GetDraggetOffsetX(),ms_pos_y - scr_y + pDraggedCursor->GetDraggetOffsetY(),font,&line_indexes,visual_text,fontScale);
                        }
                }
                else
                {
                    pDraggedCursor = null;
                    float local_ms_pos_x = ms_pos_x - scr_x;
                    float local_ms_pos_y = ms_pos_y - scr_y;
                    if(selection.cursors[TextProcessing::Selection::eSelectionCursorStart].IsIn(local_ms_pos_x,local_ms_pos_y,true))
                    {
                        selection.HideMenu();
                        pDraggedCursor = &selection.cursors[TextProcessing::Selection::eSelectionCursorStart];
                    }
                    else
                        if(selection.cursors[TextProcessing::Selection::eSelectionCursorEnd].IsIn(local_ms_pos_x,local_ms_pos_y,true))
                        {
                            selection.HideMenu();
                            pDraggedCursor = &selection.cursors[TextProcessing::Selection::eSelectionCursorEnd];
                        }

                        in_drag = true;
                        nDraggDpos = 0;
                        nTouchTimer = 0;
                }
            }
            else 
            {

                if(in_drag)
                {  
                    if(selection.cursors[TextProcessing::Selection::eSelectionCursorStart].IsOn())
                                selection.ShowMenu();
                }

                in_drag = false;
                pDraggedCursor = null;
                nDraggDpos = 0;
                nTouchTimer = 0;
            }
        }
        if(selection.IsShow())
            selection.RenderCursors(0,0);
    }
#endif
}

void NGUILabel::GetClipRect(float& clip_x, float& clip_y, float& clip_x2, float& clip_y2)
{
	if (clipChilds)
	{
		if (clip_x<scr_x) clip_x = scr_x;
		if (clip_x>scr_x + scr_w) clip_x = scr_x + scr_w;

		if (clip_x2<scr_x) clip_x2 = scr_x;
		if (clip_x2>scr_x + scr_w) clip_x2 = scr_x + scr_w;

		if (clip_y<scr_y) clip_y = scr_y;
		if (clip_y>scr_y + scr_h) clip_y = scr_y + scr_h;		

		if (clip_y2<scr_y) clip_y2 = scr_y;
		if (clip_y2>scr_y + scr_h) clip_y2 = scr_y + scr_h;	
	}

	if (parent)
	{
		parent->GetClipRect(clip_x, clip_y, clip_x2, clip_y2);
	}
}

#ifdef EDITOR
void NGUILabel::SetEditMode(bool ed)
{
	NGUIWidget::SetEditMode(ed);

	if (ed)	
	{
		if (ed_rollout)
		{
			SET_SKIP(fontEntityName, !fontUseEntity )
		
			SET_SKIP(fontDesc, fontUseEntity )			

			ed_rollout->CalcPos();
			ed_rollout->Redraw();
		}
	}
}
#endif

CALLWRAPPERARG1(NGUILabel, NLbSetTextureScript, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG1(NGUILabel, NLbSetTextureURLScript, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG1(NGUILabel, NLbGetTextureFileNameScript, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())	
CALLWRAPPER(NGUILabel, NLbIsTextureLoading, CALLWRAPPERRET(bool))
CALLWRAPPERARG3(NGUILabel, NLbSetFontColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())	
CALLWRAPPERARG3(NGUILabel, NLbGetFontColorScript, CALLWRAPPERGETADRESS(float,arg1), CALLWRAPPERGETADRESS(float,arg2), CALLWRAPPERGETADRESS(float,arg3), CALLWRAPPERNORET())	
CALLWRAPPERARG3(NGUILabel, NLbSetFontShadowColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())	

CALLWRAPPER(NGUILabel, NLbRestartTextureScript, CALLWRAPPERNORET())
CALLWRAPPERARG1(NGUILabel, NLbPauseTextureScript, CALLWRAPPERGETPARAM(bool,arg1), CALLWRAPPERNORET())	
CALLWRAPPER(NGUILabel, NLbGetTextureDurationScript, CALLWRAPPERRET(float))

CALLWRAPPER(NGUILabel, NLbGetTextureWidth, CALLWRAPPERRET(int))
CALLWRAPPER(NGUILabel, NLbGetTextureHeight, CALLWRAPPERRET(int))
	
CALLWRAPPERARG1(NGUILabel, NLbScriptSetFontScale, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERNORET())	
CALLWRAPPER(NGUILabel, NLbScriptGetFontScale, CALLWRAPPERRET(float))
CALLWRAPPERARG4(NGUILabel, NLbSetScriptTextureUV, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERGETPARAM(float,arg4), CALLWRAPPERNORET())
CALLWRAPPERARG4(NGUILabel, NLbSetScriptMaskUV, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERGETPARAM(float,arg4), CALLWRAPPERNORET())

CALLWRAPPERARG6(NGUILabel, NLbSetFontName, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETPARAM(bool,arg2), CALLWRAPPERGETPARAM(bool,arg3),
										   CALLWRAPPERGETPARAM(int,arg4), CALLWRAPPERGETPARAM(float,arg5), CALLWRAPPERGETPARAM(float,arg6), CALLWRAPPERNORET())	

CALLWRAPPERARG1(NGUILabel, NLbEnableTextureCameraFeed, CALLWRAPPERGETPARAM(bool,arg1), CALLWRAPPERNORET())											   

CALLWRAPPERARG1(NGUILabel, NLbSetSelectionEnable, CALLWRAPPERGETPARAM(bool,arg1), CALLWRAPPERNORET())	
CALLWRAPPERARG4(NGUILabel, NLbSetCursorsColor, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3),CALLWRAPPERGETPARAM(float,arg4), CALLWRAPPERNORET())	
CALLWRAPPERARG4(NGUILabel, NLbSetSelectionColor, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3),CALLWRAPPERGETPARAM(float,arg4), CALLWRAPPERNORET())	


CALLWRAPPERARG1(NGUILabel, NLbSetFilledScript, CALLWRAPPERGETPARAM(bool,arg1), CALLWRAPPERNORET())	
CALLWRAPPER(NGUILabel, NLbGetFilledScript, CALLWRAPPERRET(bool))
CALLWRAPPERARG1(NGUILabel, NLbSetFilledColorStringScript, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG4(NGUILabel, NLbSetFilledColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3),CALLWRAPPERGETPARAM(float,arg4), CALLWRAPPERNORET())
CALLWRAPPERARG4(NGUILabel, NLbGetFilledColorScript, CALLWRAPPERGETADRESS(float,arg1), CALLWRAPPERGETADRESS(float,arg2), CALLWRAPPERGETADRESS(float,arg3),CALLWRAPPERGETADRESS(float,arg4), CALLWRAPPERNORET())

void CALLWRAPPERNAME(NLbScriptSetDelegate)(asIScriptGeneric *gen)
{
	NGUILabel* obj = (NGUILabel*)gen->GetObject();
	obj->NLbScriptSetDelegate(gen);	
}

void NGUILabel::MethodsToScript(const char* script_class_name)
{
	NGUIWidget::MethodsToScript(script_class_name);

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetTexture(string&in texture)", CALLWRAPPERNAMEAS(NLbSetTextureScript));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetTextureURL(string&in url)", CALLWRAPPERNAMEAS(NLbSetTextureURLScript));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void GetTextureFileName(string& name)", CALLWRAPPERNAMEAS(NLbGetTextureFileNameScript));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "bool IsTextureLoading()", CALLWRAPPERNAMEAS(NLbIsTextureLoading));		
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetFontColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NLbSetFontColorScript));			
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void GetFontColor(float& r, float& g, float& b)", CALLWRAPPERNAMEAS(NLbGetFontColorScript));			
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetFontShadowColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NLbSetFontShadowColorScript));
	

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void RestartTexture()", CALLWRAPPERNAMEAS(NLbRestartTextureScript));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void PauseTexture(bool pause)", CALLWRAPPERNAMEAS(NLbPauseTextureScript));		
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "float GetTextureDuration()", CALLWRAPPERNAMEAS(NLbGetTextureDurationScript));	

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void EnableTextureCameraFeed(bool pause)", CALLWRAPPERNAMEAS(NLbEnableTextureCameraFeed));

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "int GetTextureWidth()", CALLWRAPPERNAMEAS(NLbGetTextureWidth));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "int GetTextureHeight()", CALLWRAPPERNAMEAS(NLbGetTextureHeight));	
	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void set_fontScale(float s)", CALLWRAPPERNAMEAS(NLbScriptSetFontScale));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "float get_fontScale()", CALLWRAPPERNAMEAS(NLbScriptGetFontScale));

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetTextureUV(float u, float v, float du, float dv)", CALLWRAPPERNAMEAS(NLbSetScriptTextureUV));	    

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetScriptMaskUV(float u, float v, float du, float dv)", CALLWRAPPERNAMEAS(NLbSetScriptMaskUV));	

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetFontName(string&in name, bool bold, bool italic, int size, float gap, float outline)", CALLWRAPPERNAMEAS(NLbSetFontName));		

    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetSelectionEnable(bool bSet)", CALLWRAPPERNAMEAS(NLbSetSelectionEnable));

    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetSelectionColor(float r, float g, float b,float a)", CALLWRAPPERNAMEAS(NLbSetSelectionEnable));
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetCursorsColor(float r, float g, float b,float a)", CALLWRAPPERNAMEAS(NLbSetCursorsColor));



    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetFilled(bool flag)", CALLWRAPPERNAMEAS(NLbSetFilledScript));		
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "bool IsFilled()", CALLWRAPPERNAMEAS(NLbGetFilledScript));		
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetFilledColor(float r, float g, float b,float a)", CALLWRAPPERNAMEAS(NLbSetFilledColorScript));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetFilledColor(string&in txt)", CALLWRAPPERNAMEAS(NLbSetFilledColorStringScript));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void GetFilledColor(float& r, float& g, float& b, float& a)", CALLWRAPPERNAMEAS(NLbGetFilledColorScript));	

	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterface("NGUILabelDelegate");
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUILabelDelegate", "void OnTextureURLFinished(NGUILabel@, int)");	

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetDelegate(NGUILabelDelegate@ delegate)", CALLWRAPPERNAMEAS(NLbScriptSetDelegate));
}

void NGUILabel::NLbScriptSetDelegate(asIScriptGeneric *gen)
{
	asIScriptObject* scr_object = (asIScriptObject*)gen->GetArgObject(0);	

	asITypeInfo* tp_info = scr_object->GetObjectType();	

	asIScriptFunction* scr_func;

	scr_func = tp_info->GetMethodByDecl("void OnTextureURLFinished(NGUILabel@, int)", false);
	callback_tex_loaded.Set(scr_func, scr_object);	
}

void NGUILabel::AddPrintCall(FontInstance* font, float scr_x, float scr_y, float scr_w, float scr_h, std::vector<FontRes::LineData>* line_indices, float font_scale, Color color, const char* txt_text)
{
	if (fontDesc.use_shadow)
	{
		NGUIWidget::AddPrintCall(font, scr_x, scr_y + 1, scr_w, scr_h, line_indices, font_scale, font_shd_clr, txt_text);
		NGUIWidget::AddPrintCall(font, scr_x, scr_y, scr_w, scr_h, line_indices, font_scale, color, txt_text);		
	}
	else
	{
		NGUIWidget::AddPrintCall(font, scr_x, scr_y, scr_w, scr_h, line_indices, font_scale, color, txt_text);
	}
}

float NGUILabel::NWgtGetCurTextHeight()
{	
	return GetTextHeight(visual_text);	
}

void NGUILabel::NLbSetTextureScript(std::string& name)
{
	SetTextureData(&slice.tex, name.c_str());
}

void NGUILabel::NLbSetTextureURLScript(std::string& url)
{
	SetTextureDataURL(&slice.tex, url.c_str(), url_fade_time);
}

void NGUILabel::NLbGetTextureFileNameScript(std::string& name)
{
    GetTextureFileName(&slice.tex, name);
}

void NGUILabel::NLbPauseTextureScript(bool pause)
{
	if (slice.tex.texture) slice.tex.texture->Pause(pause);
}

void NGUILabel::NLbRestartTextureScript()
{
	if (slice.tex.texture) slice.tex.texture->Restart();
}

float NGUILabel::NLbGetTextureDurationScript()
{
	if (slice.tex.texture)
	{
		return slice.tex.texture->GetDuration();
	}

	return -1.0f;
}

int NGUILabel::NLbGetTextureWidth()
{
	if (slice.tex.texture)
	{
		return slice.tex.texture->GetWidth();
	}

	return -1;
}

int NGUILabel::NLbGetTextureHeight()
{
	if (slice.tex.texture)
	{
		return slice.tex.texture->GetHeight();
	}

	return -1;
}

void NGUILabel::NLbSetScriptTextureUV(float u, float v, float du, float dv)
{
	if (slice.type == 0)
	{
		slice.rects[0].u = u;
		slice.rects[0].v = v;
		slice.rects[0].du = du;
		slice.rects[0].dv = dv;
	}
}

void NGUILabel::NLbSetScriptMaskUV(float u, float v, float du, float dv)
{
	slice.mask_rect.u = u;
	slice.mask_rect.v = v;
	slice.mask_rect.du = du;
	slice.mask_rect.dv = dv;
}

bool NGUILabel::NLbIsTextureLoading()
{
	if (slice.tex.texture)
	{
        return !slice.tex.texture->IsReady();
	}

	return (slice.tex.tex_loading == 1);
}

void NGUILabel::NLbSetFontColorScript(float r, float g, float b)
{
	fontDesc.color.r = r;
	fontDesc.color.g = g;
	fontDesc.color.b = b;

	CalcColors();
}

void NGUILabel::NLbGetFontColorScript(float& r, float& g, float& b)
{
	r = fontDesc.color.r;
	g = fontDesc.color.g;
	b = fontDesc.color.b;
}

void NGUILabel::NLbSetFontShadowColorScript(float r, float g, float b)
{	
	fontDesc.shd_color.r = r;
	fontDesc.shd_color.g = g;
	fontDesc.shd_color.b = b;
}

bool NGUILabel::AllowMouseCover()
{
#ifdef PC
	if (core->IsEditorMode()) return true;
#endif

	return NGUIWidget::AllowMouseCover();	
}

#ifdef EDITOR
Color NGUILabel::GetDebugColor()
{
	return COLOR_WHITE;
}
#endif

void NGUILabel::Release()
{	
	if (need_release_font)
	{
		RELEASE(font);
	}
#ifdef EDITOR
	else
	{
		if (font_entity)
		{
			font_entity->DelUseFont(this);
		}
	}
#endif		

	NGUIWidget::Release();
}

void NGUILabel::CalcColors()
{
	clr = overColor;
	clr.a = alpha;

	font_clr = fontDesc.color;	
	font_clr.a = alpha;
	
	font_shd_clr = fontDesc.shd_color;	
	font_shd_clr.a = alpha;

	if (parent)
	{	
		parent_alpha = parent->clr.a;

		clr *= parent->clr;				
		font_clr *= parent->clr;		
		font_shd_clr *= parent->clr;		
	}	
	else
	{
		parent_alpha = 1.0f;
	}
		
	for (int i=0;i<(int)childs.size();i++)
	{
		childs[i]->CalcColors();
	}
}

void NGUILabel::NLbSetFontName(string& name, bool bold, bool italic, int size, float gap, float outline)
{
	if (need_release_font)
	{
		RELEASE(font);
		font = en_core.Font()->CreateFont(name.c_str(), bold, italic, ConvertVert(size), gap, outline);		
		text_draws.clear();
	}
}

void NGUILabel::NLbEnableTextureCameraFeed(bool set_cam_feed)
{
	if (set_cam_feed)
	{
		cam_texture = en_core.Mobile()->GetCameraTexture();
	}
	else
	{
		cam_texture = null;
	}
}

void NGUILabel::CalcLinesData()
{
	if (font)
	{
		visual_text = text;

		float dots_width = font->GetWidth("...") * fontScale;

		font->CalcLinesData(text.c_str(), line_indexes, scr_w, fontScale, (TextAlign)text_align);

		if (text.size() < 3)
		{
			return;
		}

		if (num_visible_lines >= 0 && num_visible_lines != 0)
		{
			if (num_visible_lines == 0)
			{
				//auto_calc num_vis lines
			}		

			int index = 0;			

			int line_index = num_visible_lines - 1;
			
			if (line_index <= line_indexes.size() - 1)
			{				
				FontRes::LineData& line = line_indexes[line_index];

				if (line_index > 0)
				{
					index = line_indexes[line_index - 1].index;
				}

				bool need_correction = false;

				float txt_wgt = 0.0f;

				int char_ind = -1;

				if (line_index < line_indexes.size() - 1)
				{
					visual_text.erase(line_indexes[line_index].index, string::npos);
					char_ind = visual_text.size() - 1;
                    if(visual_text[char_ind] == '\n')
                        visual_text.erase(char_ind,1);

					visual_text.append("...");

					need_correction= true;
				}
				else
				{									
					txt_wgt = font->GetWidth(&visual_text.c_str()[index]) * fontScale;

					if (txt_wgt > scr_w)
					{
						char_ind = visual_text.size() - 1;
						visual_text.append("...");
						need_correction= true;
					}
				}

				if (need_correction && char_ind >= 0)
				{													
					for (int i = 0; i<5; i++)
					{
						txt_wgt = font->GetWidth(&visual_text.c_str()[index]) * fontScale;

						if (txt_wgt > scr_w )
						{							
							visual_text.erase(char_ind, 1);
							char_ind--;
						}
						else
						{
							break;
						}
					}

					line_indexes[line_index].index+= 20;
							
					if (text_align == 1)
					{
						line_indexes[line_index].offset = (scr_w - txt_wgt) * 0.5f;				
					}			
					else
					if (text_align == 2)
					{
						line_indexes[line_index].offset = scr_w - txt_wgt;
					}
				}
			}
		}		
	}	
}

void NGUILabel::NLbScriptSetFontScale(float s)
{
	fontScale = s;

	CalcLinesData();	
}

float NGUILabel::NLbScriptGetFontScale()
{
	return fontScale;
}


bool NGUILabel::IsHitted(float mx, float my)
{
    float local_ms_pos_x = mx - scr_x,local_ms_pos_y = my - scr_y;

    if(NGUIWidget::IsHitted(mx,my))
        return true;
#ifndef OLD_VERSION
    if(selection.IsHitted(local_ms_pos_x,local_ms_pos_y))
        return true;
#endif
    return false;
}

void NGUILabel::NLbSetSelectionEnable(bool bSet)
{
    selection_enable = bSet;
}

void  NGUILabel::NLbSetCursorsColor(float r,float g,float b,float a)
{
#ifndef  OLD_VERSION
    selection.SetCursorsColor(Color(r,g,b,a));
#endif
}
void  NGUILabel::NLbSetSelectionColor(float r,float g,float b,float a)
{
#ifndef  OLD_VERSION
    selection.SetColor(Color(r,g,b,a));
#endif
}

void NGUILabel::SetFocused(bool focus)
{
    NGUIWidget::SetFocused(focus);
#ifndef  OLD_VERSION
    if(!focus)
        selection.Hide();
#endif

}

void NGUILabel::NLbSetFilledScript(bool bSet)
{
    filled = bSet;
}

void NGUILabel::NLbSetFilledColorScript(float r, float g, float b,float a)
{
    filled_color.r = r;
    filled_color.g = g;
    filled_color.b = b;
    filled_alpha = a;
}

void NGUILabel::NLbSetFilledColorStringScript(std::string& txt)
{
    long red,green,blue;float alpha;
    if(ConvertHexColor(txt,red,green,blue,alpha))
    {
        filled_color.r = red;
        filled_color.g = green;
        filled_color.b = blue;
        filled_alpha = alpha;
    }
}

bool NGUILabel::NLbGetFilledScript()
{
    return filled;
}

void NGUILabel::NLbGetFilledColorScript(float& r, float& g, float& b,float& a)
{
    r = filled_color.r;
    g = filled_color.g;
    b = filled_color.b;	
    a = filled_alpha;
}
