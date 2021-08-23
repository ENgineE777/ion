
#include "NGUILabelRich.h"
#include "Core/Network/NetworkService.h"

ENTITY(NGUILabelRich)
ENTITY_IS_2D_OBJECT
ENTITY_PARAMS(NGUILabelRich,"NGUILabelRich","NGUI", "LabelRich")

TextProcessing::RichText NGUILabelRich::gBuff;
#define MENU_WAIT_TIME 1.3f
#define MENU_SIZE 1

#define OLD_VERSION

void NGUILabelRich::SetText(const char* txt)
{
    if (!*txt)
		text = "";
    else
		text = txt;

	need_set_label = true;
}

void NGUILabelRich::RegisterProperties()
{
	NGUIWidget::RegisterProperties();	

	PROP_REG( filled )
	PROP_REG( filled_color )
	PROP_REG( filled_alpha )

	PROP_REG( num_visible_lines )
	PROP_REG(selection_enable)
	PROP_REG(text_cursor_width)
	PROP_REG(text_cursor_height)
	PROP_REG(cursorColor)
	PROP_REG(selection_color)
    PROP_REG(find_links_enable)
    PROP_REG(links_color)
    PROP_REG(links_callback)

	PROP_REG( fontUseEntity )
	PROP_REG( fontEntityName )
	PROP_REG( fontDesc )				
	PROP_REG( fontScale )

#ifdef EDITOR
	PROP_REG(generateFont)			
#endif
}

void NGUILabelRich::Init()
{	
	NGUIWidget::Init();		   

	need_release_font = true;	

	font_entity = null;		
  
	SetUpdate((TaskHolder::Task)&NGUILabelRich::CheckPress, lvl_pre5 + 3);
	need_set_label = false;

	bUseVertTextAlignInDrawCall =  false;
}
	
void NGUILabelRich::ReInit()
{			
    NGUIWidget::ReInit();	
	text_align = 0;
	bUseVertTextAlignInDrawCall =  false;

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

		font = en_core.Font()->CreateFont(fontDesc.name, fontDesc.bold, fontDesc.italic,(int)ConvertVert(fontDesc.height), fontDesc.gap, fontDesc.outline);		
		font_entity = null;
	}	

    in_drag = false;
    nDraggDpos = 0;
    nTouchTimer = 0;

#ifndef  OLD_VERSION
    selection.Init(this,selection_color,fontScale);
    selection.cursors[TextProcessing::Selection::eSelectionCursorStart].Init(cursorColor,fontScale,TextProcessing::Cursor::eCursorTop,false,this,true);
    selection.cursors[TextProcessing::Selection::eSelectionCursorEnd].Init(cursorColor,fontScale,TextProcessing::Cursor::eCursorBottom,false,this,true);
    pDraggedCursor = null;
#endif // ! OLD_VERSION

}

bool NGUILabelRich::Start()
{		

	return NGUIWidget::Start();
}

void NGUILabelRich::RenderWidget()
{
	float delta_time = core->GetDeltaTime();

    if(filled && NGUIService::instance->pFilledTexture)
    {
        Color buffColor = filled_color;
        buffColor.a = filled_alpha * clr.a;
        buffColor *= overColor;
        AddDrawCall(NGUIService::instance->pFilledTexture,0,0,scr_w,scr_h,buffColor,0,0,1,1);
    }	
    
    if (need_set_label)ParseText();
#ifndef  OLD_VERSION
    NGUIService::instance->RemoveTouchRegion(this);
    selection.Render(&label);
#endif
	float nStartY = 0;
	if (vtext_align == 1 || vtext_align == 2)
	{
		float nTextHeight = NWgtGetCurTextHeight();
		if (vtext_align == 1)
		{
			nStartY = (scr_h - nTextHeight) * 0.5f;					
		} 
		else
		{
			if (vtext_align == 2)
				nStartY = scr_h - nTextHeight;
		}
	}
    label.Render(this,num_visible_lines,font_clr.a,nStartY);
#ifndef  OLD_VERSION
    if(selection.IsShow())
        label.RenderCursor(selection);
#endif

}

void NGUILabelRich::ParseText()
{
    if(font)
    {
        FontParams buff(fontDesc);
        buff.color = font_clr;
        buff.shd_color = font_shd_clr;
        buff.font = font->Clone();
        buff.shd_color.a = buff.color.a = 1;
        if(find_links_enable)
            label.Set(text.c_str(),buff,fontScale,scr_w,links_callback,links_color);
        else
            label.Set(text.c_str(),buff,fontScale,scr_w);
    }
    need_set_label = false;
    text_draws.clear();
}

void NGUILabelRich::GetClipRect(float& clip_x, float& clip_y, float& clip_x2, float& clip_y2)
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
void NGUILabelRich::SetEditMode(bool ed)
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

CALLWRAPPERARG3(NGUILabelRich, NLBESetFontColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())	
CALLWRAPPERARG3(NGUILabelRich, NLBEGetFontColorScript, CALLWRAPPERGETADRESS(float,arg1), CALLWRAPPERGETADRESS(float,arg2), CALLWRAPPERGETADRESS(float,arg3), CALLWRAPPERNORET())	
CALLWRAPPERARG3(NGUILabelRich, NLBESetFontShadowColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())	
	
CALLWRAPPERARG1(NGUILabelRich, NLBEScriptSetFontScale, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERNORET())	
CALLWRAPPER(NGUILabelRich, NLBEScriptGetFontScale, CALLWRAPPERRET(float))


CALLWRAPPERARG6(NGUILabelRich, NLBESetFontName, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETPARAM(bool,arg2), CALLWRAPPERGETPARAM(bool,arg3),
										   CALLWRAPPERGETPARAM(int,arg4), CALLWRAPPERGETPARAM(float,arg5), CALLWRAPPERGETPARAM(float,arg6), CALLWRAPPERNORET())	

CALLWRAPPERARG1(NGUILabelRich, NLBESetSelectionEnable, CALLWRAPPERGETPARAM(bool,arg1), CALLWRAPPERNORET())	
CALLWRAPPERARG4(NGUILabelRich, NLBESetCursorsColor, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3),CALLWRAPPERGETPARAM(float,arg4), CALLWRAPPERNORET())	
CALLWRAPPERARG4(NGUILabelRich, NLBESetSelectionColor, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3),CALLWRAPPERGETPARAM(float,arg4), CALLWRAPPERNORET())	


CALLWRAPPERARG1(NGUILabelRich, NLBECheckLinks,CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERRET(int))
CALLWRAPPER(NGUILabelRich,     NLBECheckLinksId, CALLWRAPPERRET(int))

CALLWRAPPERARG1(NGUILabelRich, NLBESetFilledScript, CALLWRAPPERGETPARAM(bool,arg1), CALLWRAPPERNORET())	
CALLWRAPPER(NGUILabelRich, NLBEGetFilledScript, CALLWRAPPERRET(bool))
CALLWRAPPERARG1(NGUILabelRich, NLBESetFilledColorStringScript, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG4(NGUILabelRich, NLBESetFilledColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3),CALLWRAPPERGETPARAM(float,arg4), CALLWRAPPERNORET())
CALLWRAPPERARG4(NGUILabelRich, NLBEGetFilledColorScript, CALLWRAPPERGETADRESS(float,arg1), CALLWRAPPERGETADRESS(float,arg2), CALLWRAPPERGETADRESS(float,arg3),CALLWRAPPERGETADRESS(float,arg4), CALLWRAPPERNORET())


CALLWRAPPERARG3(NGUILabelRich, NLBESetLinksColorScript, CALLWRAPPERGETADRESS(float,arg1), CALLWRAPPERGETADRESS(float,arg2), CALLWRAPPERGETADRESS(float,arg3), CALLWRAPPERNORET())	
CALLWRAPPERARG3(NGUILabelRich, NLBEGetLinksColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())	
CALLWRAPPERARG1(NGUILabelRich, NLBESetFindLinksEnable, CALLWRAPPERGETPARAM(bool,arg1), CALLWRAPPERNORET())	
CALLWRAPPER(NGUILabelRich, NLBEGetFindLinksEnable, CALLWRAPPERRET(bool))

void NGUILabelRich::MethodsToScript(const char* script_class_name)
{
	NGUIWidget::MethodsToScript(script_class_name);

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetFontColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NLBESetFontColorScript));			
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void GetFontColor(float&out r, float&out g, float&out b)", CALLWRAPPERNAMEAS(NLBEGetFontColorScript));			
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetFontShadowColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NLBESetFontShadowColorScript));

	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void set_fontScale(float s)", CALLWRAPPERNAMEAS(NLBEScriptSetFontScale));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "float get_fontScale()", CALLWRAPPERNAMEAS(NLBEScriptGetFontScale));

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetFontName(string&in name, bool bold, bool italic, int size, float gap, float outline)", CALLWRAPPERNAMEAS(NLBESetFontName));		

    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetSelectionEnable(bool bSet)", CALLWRAPPERNAMEAS(NLBESetSelectionEnable));
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetSelectionColor(float r, float g, float b,float a)", CALLWRAPPERNAMEAS(NLBESetSelectionEnable));
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetCursorsColor(float r, float g, float b,float a)", CALLWRAPPERNAMEAS(NLBESetCursorsColor));
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "int  CheckLinks(string&out text)", CALLWRAPPERNAMEAS(NLBECheckLinks));
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "int  CheckLinksId()", CALLWRAPPERNAMEAS(NLBECheckLinksId));


    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetFilled(bool flag)", CALLWRAPPERNAMEAS(NLBESetFilledScript));		
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "bool IsFilled()", CALLWRAPPERNAMEAS(NLBEGetFilledScript));		
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetFilledColor(float r, float g, float b,float a)", CALLWRAPPERNAMEAS(NLBESetFilledColorScript));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetFilledColor(string&in txt)", CALLWRAPPERNAMEAS(NLBESetFilledColorStringScript));	
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void GetFilledColor(float&out r, float&out g, float&out b, float&out a)", CALLWRAPPERNAMEAS(NLBEGetFilledColorScript));	

    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetLinksColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NLBESetLinksColorScript));			
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void GetLinksColor(float&out r, float&out g, float&out b)", CALLWRAPPERNAMEAS(NLBEGetLinksColorScript));			
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetFindLinks(bool flag)", CALLWRAPPERNAMEAS(NLBESetFindLinksEnable));		
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "bool IsFindLinksEnable()", CALLWRAPPERNAMEAS(NLBEGetFindLinksEnable));		

    
}

void NGUILabelRich::NLBESetFontColorScript(float r, float g, float b)
{
	fontDesc.color.r = r;
	fontDesc.color.g = g;
	fontDesc.color.b = b;

	CalcColors();
    ParseText();
}

void NGUILabelRich::NLBEGetFontColorScript(float& r, float& g, float& b)
{
	r = fontDesc.color.r;
	g = fontDesc.color.g;
	b = fontDesc.color.b;
}

void NGUILabelRich::NLBESetFontShadowColorScript(float r, float g, float b)
{	
	fontDesc.shd_color.r = r;
	fontDesc.shd_color.g = g;
	fontDesc.shd_color.b = b;
    ParseText();
}

void NGUILabelRich::NLBESetFindLinksEnable(bool bSet)
{
    find_links_enable = bSet;
    if(find_links_enable)
        ParseText();
}

void NGUILabelRich::NLBESetLinksColorScript(float r, float g, float b)
{
    links_color.r = r;
    links_color.g = g;
    links_color.b = b;
    if(find_links_enable)
        ParseText();
}

 void NGUILabelRich::NLBEGetLinksColorScript(float& r, float& g, float& b)
 {
     r = links_color.r;
     g = links_color.g;
     b = links_color.b;
 }

bool NGUILabelRich::AllowMouseCover()
{
#ifdef PC
	if (core->IsEditorMode()) return true;
#endif

	return NGUIWidget::AllowMouseCover();	
}

#ifdef EDITOR
Color NGUILabelRich::GetDebugColor()
{
	return COLOR_WHITE;
}
#endif

void NGUILabelRich::Release()
{	
    DelUpdate((TaskHolder::Task)&NGUILabelRich::CheckPress);
    label.Clear();
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

void NGUILabelRich::CalcColors()
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

void NGUILabelRich::NLBESetFontName(string& name, bool bold, bool italic, int size, float gap, float outline)
{
	if (need_release_font)
	{
        FontInstance* oldFont = font;
		font = en_core.Font()->CreateFont(name.c_str(), bold, italic,(int)ConvertVert(size), gap, outline);
        if (need_set_label)ParseText();
        label.ChangeFont(oldFont,font);
        RELEASE(oldFont);
	}
}

void NGUILabelRich::NLBEScriptSetFontScale(float s)
{
	fontScale = s;
    SetText(text.c_str());
}

float NGUILabelRich::NLBEScriptGetFontScale()
{
	return fontScale;
}

void NGUILabelRich::CheckPress(float dt, int level)
{
    if (!IsSymbol() && Scene()->IsRunning() && IsVisible() && IsActive())
    {

        if (mscover_widget == this)
        {
            if (core->Controls()->CheckTouch(0, ControlsService::FastActivatedExclusive, this))
            {
                int menuID = -1;bool bSuccess = false;
#ifndef  OLD_VERSION
                if(selection.IsOn() && selection_enable)
                    menuID = selection.CheckMenuPress(ms_pos_x - scr_x,ms_pos_y - scr_y,bSuccess);
#endif
                if(bSuccess)
                {
#ifndef  OLD_VERSION
                    if(menuID != -1)
                    {
                        if(menuID == eMenuCopy)
                        {
                            label.CopyText(selection);
                            selection.Hide();
                        }
                    }
#endif
                }
                else
                {
#ifndef  OLD_VERSION
                    selection.Hide();
#endif
                    Vector point(ms_pos_x - scr_x,ms_pos_y - scr_y);
                    string func,sLink;int index = -1;
                    label.CheckPress(point,func,index,sLink);
                    if(!func.empty() )
                    {
						ScriptFunction sfunc;																		

                        if(index == -1)
						{
							ScriptFunction::Find1P(&sfunc, Scene()->ScriptModule(), func.c_str(), Desc()->GetName());

							core->Script()->Machine()->AddPtrArg(this);
							core->Script()->Machine()->Call(&sfunc);							                         
						}
                        else
						{
                            if(index == -2)
							{
								ScriptFunction::Find2PS(&sfunc, Scene()->ScriptModule(), func.c_str(), Desc()->GetName());

								core->Script()->Machine()->AddPtrArg(this);
								core->Script()->Machine()->AddStrArg(sLink.c_str());
								core->Script()->Machine()->Call(&sfunc);                                
							}
                            else
							{
								ScriptFunction::Find2PI(&sfunc, Scene()->ScriptModule(), func.c_str(), Desc()->GetName());

								core->Script()->Machine()->AddPtrArg(this);
								core->Script()->Machine()->AddIntArg(index);
								core->Script()->Machine()->Call(&sfunc);
							}
						}

						sfunc.Reset(); 
                    }
                }
            }
        }
#ifndef  OLD_VERSION
        if(core->Controls()->CheckTouch(0, ControlsService::Active, this))
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
                        nTouchTimer += dt;
                        if(nTouchTimer > MENU_WAIT_TIME && nDraggDpos == 0)
                        {
                            nTouchTimer = 0;
                            nDraggDpos = 1;
                            static TextProcessing::Cursor temp_curret;
                            if(label.FindPosition(ms_pos_x - scr_x,ms_pos_y - scr_y,&temp_curret))
                            {
                                int start = 0,end = 0;
                                label.GetWordBorders(temp_curret,start,end);
                                if(label.Select(temp_curret.Line(),start,end,selection))
                                {
                                    TextProcessing::Menu::TMenuIds  ids;
                                    ids.push_back(eMenuCopy);
                                    selection.CreateMenu(ids,TextProcessing::Menu::vLabelMenuText);
                                    pDraggedCursor = null;
                                }
                            }
                        } 
                    }
                }
                else
                {
                   if(bDragg)
                       label.FindPosition(ms_pos_x - scr_x+ pDraggedCursor->GetDraggetOffsetX(),ms_pos_y - scr_y + pDraggedCursor->GetDraggetOffsetY(),pDraggedCursor);
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
#endif
     }
}

void NGUILabelRich::SetToBuff(const string& txt)
{
    FontParams buff(fontDesc);
    buff.color = font_clr;
    buff.shd_color = font_shd_clr;
    buff.font = font->Clone();
    gBuff.Set(txt.c_str(),buff,fontScale,scr_w);
}

float NGUILabelRich::GetTextWidth(std::string& txt)
{
    if(txt.empty())return 0;
    SetToBuff(txt);
    return NGUIService::instance->h_mul_ScreenToRef * gBuff.GetTextWidth();
}

float NGUILabelRich::GetTextHeight(std::string& txt)
{
    if(txt.empty())return 0;
    SetToBuff(txt);
    return  NGUIService::instance->v_mul_ScreenToRef * gBuff.GetTextHeight(num_visible_lines);
}

int NGUILabelRich::GetTextLinesCount(std::string& txt)
{
    if(txt.empty())return 0;
    SetToBuff(txt);
    return gBuff.GetTextLinesCount();
}

float NGUILabelRich::NWgtGetCurTextWidth()
{
    if (need_set_label)ParseText();
    return NGUIService::instance->h_mul_ScreenToRef * label.GetTextWidth();
}

float NGUILabelRich::NWgtGetCurTextHeight()
{
    if (need_set_label)ParseText();
    return NGUIService::instance->v_mul_ScreenToRef * label.GetTextHeight(num_visible_lines);
}

int NGUILabelRich::NWgtGetCurTextLinesCount()
{
    if (need_set_label)ParseText();
    return label.GetTextLinesCount();
}

void NGUILabelRich::NLBESetSelectionEnable(bool bSet)
{
    selection_enable = bSet;
}

void NGUILabelRich::NLBESetCursorsColor(float r,float g,float b,float a)
{
#ifndef OLD_VERSION
     selection.SetCursorsColor(Color(r,g,b,a));
#endif
}

void NGUILabelRich::NLBESetSelectionColor(float r,float g,float b,float a)
{
#ifndef OLD_VERSION
   selection.SetColor(Color(r,g,b,a));
#endif
}


bool NGUILabelRich::IsHitted(float mx, float my)
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

void NGUILabelRich::SetFocused(bool focus)
{
    NGUIWidget::SetFocused(focus);
#ifndef OLD_VERSION
    if(!focus)
        selection.Hide();
#endif
}

int NGUILabelRich::NLBECheckLinks(string& text)
{
    Vector point(ms_pos_x - scr_x,ms_pos_y - scr_y);
    string func,sLink;int index = -1;
    label.CheckPress(point,func,index,sLink);
    if( !func.empty() )
    {
        text = func;
        if(index == -2)return 0;
        return index;
    }
    return -1;
}
int NGUILabelRich::NLBECheckLinksId()
{
    Vector point(ms_pos_x - scr_x,ms_pos_y - scr_y);
    string func,sLink;int index = -1;
    label.CheckPress(point,func,index,sLink);
    if( !func.empty() )
    {
        if(index == -2)return 0;
        return index;
    }
    return -1;
}
void NGUILabelRich::NLBESetFilledScript(bool bSet)
{
    filled = bSet;
}

void NGUILabelRich::NLBESetFilledColorScript(float r, float g, float b,float a)
{
    filled_color.r = r;
    filled_color.g = g;
    filled_color.b = b;
    filled_alpha = a;
}

void NGUILabelRich::NLBESetFilledColorStringScript(std::string& txt)
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

bool NGUILabelRich::NLBEGetFilledScript()
{
    return filled;
}

void NGUILabelRich::NLBEGetFilledColorScript(float& r, float& g, float& b,float& a)
{
    r = filled_color.r;
    g = filled_color.g;
    b = filled_color.b;	
    a = filled_alpha;
}

void NGUILabelRich::CalcLinesData()
{
    need_set_label = true;
}





