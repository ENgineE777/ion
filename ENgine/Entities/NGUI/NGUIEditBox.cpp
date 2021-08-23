
#include "NGUIEditBox.h"


ENTITY(NGUIEditBox)
ENTITY_IS_2D_OBJECT
ENTITY_PARAMS(NGUIEditBox,"NGUIEditBox","NGUI", "EditBox")

#ifdef EDITOR
void StartEditEditBoxGUITexture(PropertiesOwner* owner)
{
	NGUIEditBox* label = (NGUIEditBox*)owner;
	TextureEntry::StartEditGUITexture(&label->slice);
}
#endif


#define MENU_WAIT_TIME 1.3f
 
#define  OLD_VERSION

void NGUIEditBox::RegisterProperties()
{
	NGUIWidget::RegisterProperties();				

	PROP_REG( funcOnFocusGain )	
	PROP_REG( funcOnChange )			
	PROP_REG( funcOnReturnKey )			
	PROP_REG( funcOnFocusLost )	

	PROP_REG( max_text )	

	PROP_REG( placeholder_text )		
	PROP_REG( placeholder_alpha )		

	PROP_REG( text_edit_style )
	PROP_REG( expand_lines )	

	PROP_REG( keyboardType )	
	PROP_REG( returnKey )		

	PROP_REG( fontDesc )				
	PROP_REG( fontScale )	

	PROP_REG( textureUseEntity )
	PROP_REG( textureEntityName )
#ifdef EDITOR
	PROP_REG( texEdit )	
#endif

	PROP_REG( text_horz_border )	
	PROP_REG( text_vert_border )		

	PROP_REG( cursor_Color )	
	PROP_REG( selection_Color );

	PROP_REG( overColor )
}

void NGUIEditBox::Init()
{	
	NGUIWidget::Init();		
		
	was_focued = false;
	
	text_max_offset = 0;
	text_cur_offsetX = text_cur_offsetY = 0;

	init_height = 0;
	expanded_height = 0;
    init_Y = 0;

	in_drag = false;

    border.x1 = border.x2 = 0;
    border.y1 = border.y1 = 0;
    pDraggedCursor = null;


	need_call_callback = 0;

    nDraggDpos = 0;
    nTouchTimer = 0;

	slices.push_back(TexSlice(&slice, &textureUseEntity,
#ifdef EDITOR
							  get_textureEntityNameprop_inst(), get_texEditprop_inst(),
#endif
							  textureEntityName.c_str()));
}

void NGUIEditBox::ReInit()
{
	init_height = height;
    init_Y = y;
	
	clipChilds = true;	

	NGUIWidget::ReInit();			

	text_align = 0;
	vtext_align = 0;
	
	RELEASE(font)
	font = en_core.Font()->CreateFont(fontDesc.name, fontDesc.bold, fontDesc.italic, (int)ConvertVert(fontDesc.height), fontDesc.gap, fontDesc.outline);

    selection.Init(this,selection_Color,fontScale);
    curret.Init(cursor_Color,fontScale,TextProcessing::Cursor::eCursorBottom,true,this);
    selection.cursors[TextProcessing::Selection::eSelectionCursorStart].Init(cursor_Color,fontScale,TextProcessing::Cursor::eCursorTop,false,this,true);
    selection.cursors[TextProcessing::Selection::eSelectionCursorEnd].Init(cursor_Color,fontScale,TextProcessing::Cursor::eCursorBottom,false,this,true);
    float min,max;
    CalculateTextClipX(min,max);
}
void  NGUIEditBox::CalculateTextClipX(float& min,float& max)
{
    FontRes::Glyph* glyph = font->GetCharGlyph('|');
    float curretW = 2;
    if (glyph)
        curretW = (float)glyph->width;
    border.x1 = NGUIService::instance->h_mul_RefToScreen * text_horz_border;
    border.x2 = scr_w -  border.x1;
    border.y1 =  NGUIService::instance->v_mul_RefToScreen * text_vert_border;
    border.y2 = scr_h - border.y1;
    max = border.x2 + curretW;
    if(border.x1 > curretW)
        min = border.x1 - curretW;
    else
        min = 0;
}

bool NGUIEditBox::Start()
{				
	ScriptFunction::Find1P(&callback_focus_gain, Scene()->ScriptModule(), funcOnFocusGain.c_str(), Desc()->GetName());
	ScriptFunction::Find1P(&callback_focus_lost, Scene()->ScriptModule(), funcOnFocusLost.c_str(), Desc()->GetName());
	ScriptFunction::Find1P(&callback_focus_retrn_key, Scene()->ScriptModule(), funcOnReturnKey.c_str(), Desc()->GetName());	
	ScriptFunction::Find1P(&callback_change_text, Scene()->ScriptModule(), funcOnChange.c_str(), Desc()->GetName());		

	return NGUIWidget::Start();
}

void NGUIEditBox::Finish()
{
	callback_focus_gain.Reset();
	callback_focus_lost.Reset();
	callback_focus_retrn_key.Reset();	
	callback_change_text.Reset();	

	NGUIWidget::Finish();	
	in_drag = false;
		
}
void NGUIEditBox::MoveCurretRight(int count)
{
    selection.Hide();
    if(text_edit_style == eMultiLine)
    {
        float prev_text_offset = text_cur_offsetY;
        float prev_max_offset = text_max_offset;
        CalculateLinesAndOffsetY();					

        curret += count;					
        curret.CalculatePosition(text,line_indexes,font);

        if (text_cur_offsetY > prev_text_offset)
            text_cur_offsetY = prev_text_offset;

        if (text_max_offset > prev_max_offset)
        {
            text_cur_offsetY -= font->GetCharHeight() + font->GetRowGap();
            if (text_cur_offsetY < 0) text_cur_offsetY = 0;
        }
        CalcTextOffsetY(&curret);					
    }
    else
    {
        curret += count;					
        curret.CalculatePosition(text,line_indexes,font);
        if(curret.GetX() - text_cur_offsetX + border.x1 >= border.x2)
            text_cur_offsetX =  curret.GetX() - border.x2 + border.x1;
    }
}

void NGUIEditBox::MoveCurretLeft(int count)
{
    selection.Hide();
    if(text_edit_style == eMultiLine)
    {
        CalculateLinesAndOffsetY();
        curret -= count;
        curret.CalculatePosition(text,line_indexes,font);
        CalcTextOffsetY(&curret);
    }
    else
    {
        float old_curret_x = curret.GetX();
        curret -= count;
        curret.CalculatePosition(text,line_indexes,font);
        text_cur_offsetX -= (old_curret_x - curret.GetX());
        if(text_cur_offsetX < 0)
            text_cur_offsetX = 0;
    }
}

void NGUIEditBox::CheckCurretsVisibility()
{
#ifndef OLD_VERSION
    if(text_edit_style == eOneLine)
    {

        TextProcessing::Cursor* pCursor = &selection.cursors[TextProcessing::Selection::eSelectionCursorEnd];
        if(pCursor->GetX() -  text_cur_offsetX  +  border.x1 >  border.x2 || pCursor->GetX() - text_cur_offsetX < 0)
            pCursor->StopRender();
        else
            pCursor->StartRender();

        pCursor = &selection.cursors[TextProcessing::Selection::eSelectionCursorStart];
        if(pCursor->GetX() -  text_cur_offsetX  +  border.x1 >  border.x2 || pCursor->GetX() - text_cur_offsetX < 0)
            pCursor->StopRender();
        else
            pCursor->StartRender();

        if(curret.GetX() - text_cur_offsetX + border.x1 > border.x2  || curret.GetX() - text_cur_offsetX < 0 )
            curret.StopRender();
        else
            curret.StartRender();

    }
    else
    {
        float offsetY = -text_max_offset + text_cur_offsetY + border.y1;
        float charH = font->GetCharHeight();
        float H = charH;
        if(curret.GetAlign() == TextProcessing::Cursor::eCursorTop)H = 0;
        if(curret.GetY() +  offsetY + H >  border.y2 || curret.GetY() + offsetY < 0)
            curret.StopRender();
        else
            curret.StartRender();

        TextProcessing::Cursor* pCursor = &selection.cursors[TextProcessing::Selection::eSelectionCursorEnd];
        H = charH;
        if(pCursor->GetAlign() == TextProcessing::Cursor::eCursorTop)H = 0;
        if(pCursor->GetY() + offsetY + H  > border.y2 || pCursor->GetY() + offsetY < 0)
            pCursor->StopRender();
        else
            pCursor->StartRender();

        pCursor = &selection.cursors[TextProcessing::Selection::eSelectionCursorStart];
        H = charH;
        if(pCursor->GetAlign() == TextProcessing::Cursor::eCursorTop)H = 0;
        if(pCursor->GetY() + offsetY + H  > border.y2 || pCursor->GetY() + offsetY < 0)
            pCursor->StopRender();
        else
            pCursor->StartRender();
    }
#endif

}
void NGUIEditBox::RenderWidget()
{	
	if (need_call_callback > 0)
	{
		if (funcOnFocusLost[0] && need_call_callback == 2)
		{
			core->Script()->Machine()->AddPtrArg(this);
			core->Script()->Machine()->Call(&callback_focus_lost);			
		}

		if (funcOnFocusGain[0] && need_call_callback == 1)
		{
			core->Script()->Machine()->AddPtrArg(this);
			core->Script()->Machine()->Call(&callback_focus_gain);
		}

		need_call_callback = 0;
	}

	if (!core->IsEditorMode())
	{
        float local_ms_pos_x = 0 , local_ms_pos_y = 0;
        CalculateLocalMousePos(local_ms_pos_x,local_ms_pos_y);
		if (mscover_widget == this)
		{
			if (core->Controls()->CheckTouch(0, ControlsService::FastActivatedExclusive, this))
			{
#ifndef OLD_VERSION
                menuID = -1;bool bSuccess = false;
                if(selection.IsOn())
                    menuID = selection.CheckMenuPress(ms_pos_x - scr_x,ms_pos_y - scr_y,bSuccess);
                else
                    if(curret.IsOn())
                        menuID = curret.CheckMenuPress(ms_pos_x - scr_x,ms_pos_y - scr_y,bSuccess);
                if(bSuccess)
                {
                    if(menuID != -1)
                    {
                        int start = selection.cursors[TextProcessing::Selection::eSelectionCursorStart].Index();
                        int count = selection.cursors[TextProcessing::Selection::eSelectionCursorEnd].Index() - selection.cursors[TextProcessing::Selection::eSelectionCursorStart].Index();
                        switch(menuID)
                        {
                        case eMenuSelect:
                            {
                                int start = 0,end = 0;
                                TextProcessing::TLinesData* lines = null; 
                                if(text_edit_style == eMultiLine)lines = &line_indexes;
                                TextProcessing::TextUtils::GetWordBorders(font,lines,text,curret,start,end,-border.x1 + text_cur_offsetX,border.x2 - 2 * border.x1 + text_cur_offsetX,fontScale);
                                if(selection.Select(font,lines,text,curret,start,end,fontScale))
                                {
                                    curret.Hide();
                                    menuID = eMenuReplace;
                                    core->Mobile()->IsClipboardEmpty(this);// will create menu in callback
                                }
                                break;
                            }
                        case eMenuCopy:
                            {
                                string txt = text.substr(start,count);	
                                core->Mobile()->SetTextToClipboard(txt.c_str());
                                selection.Hide();
                                curret.Hide();
                                break;
                            }
                        case eMenuCut:
                            {
                                text.erase(start,count);

								core->Script()->Machine()->AddPtrArg(this);
								core->Script()->Machine()->Call(&callback_change_text);

                                if(curret.Index() >  start)MoveCurretLeft(count);
                                else ReCalculateTextOffsets();
                                selection.Hide();
                                curret.Hide();
                                break;
                            }
                        case eMenuPaste:
                        case eMenuReplace:
                            {
                                core->Mobile()->GetTextFromClipboard(this);
                                selection.Hide();
                                curret.Hide();
                                break;
                            }
                        }
                    }
                }
                else
#endif
				if (text.size() > 0 )
                {
					FindCursorPosition(local_ms_pos_x,local_ms_pos_y,&curret);
#ifndef OLD_VERSION
                    curret.Show(true);
                    selection.Hide();
#endif
                    nDraggDpos = 0;
                    nTouchTimer = 0;
                    curret.HideMenu();
                }
				
				if (!IsFocused())
				{
					SetFocused(true);
				}
			}

            if (core->Controls()->CheckTouch(0, ControlsService::Active, this))
            {
                if(in_drag)
                {
                    bool bDragg  = false;

                    if (text_edit_style == eOneLine)
					{
                        nDraggDpos = fabs(ms_pos_x - ms_prev_pos_x);
					}
                    else
					{
                        nDraggDpos = fabs(ms_pos_y - ms_prev_pos_y);
					}

                    if (fabs(ms_pos_x - ms_prev_pos_x) > 0)
					{
						bDragg = true;
					}
                    else
					{
                        if (fabs(ms_pos_y - ms_prev_pos_y) > 0)
						{
							bDragg = true;
						}
					}


                    if (!pDraggedCursor)
                    {
                        if (bDragg)
                        {
                            if (text_edit_style == eOneLine)
                            {
                                float dx = ms_pos_x - ms_prev_pos_x;
                                
								if (dx > 0)
                                {
                                    text_cur_offsetX -= dx;
                                    
									if (text_cur_offsetX < 0)
									{
										text_cur_offsetX = 0;
									}
                                }
                                else
								if(dx < 0)
                                {
									float textW = (float)font->GetWidth(text.c_str()); // TODO: calculate  width when typing!!!! 
                                        
									if (textW - text_cur_offsetX + dx > border.x2)
                                    {
										text_cur_offsetX -= dx;
                                        
										if (text_cur_offsetX < 0)
										{
											text_cur_offsetX = 0;
										}
									}                                    
                                    else
									{
										if (textW > border.x2)
										{
											text_cur_offsetX = textW - border.x2 + border.x1;
										}
                                    }
								}
                            }    
                            else
                            {
                                text_cur_offsetY += (ms_pos_y - ms_prev_pos_y);
                                MathClip(text_cur_offsetY,text_max_offset,0)
                            }

                            CheckCurretsVisibility();
                        }
                        else
                        {
#ifndef OLD_VERSION
                            nTouchTimer += core->GetDeltaTime();
                            if(nTouchTimer > MENU_WAIT_TIME && nDraggDpos == 0)
                            {
                                float offsetY = -text_max_offset + text_cur_offsetY + border.y1;
                                nTouchTimer = 0;
                                nDraggDpos = 1;
                                static TextProcessing::Cursor temp_curret;
                                FindCursorPosition(local_ms_pos_x,local_ms_pos_y,&temp_curret);
                                
								if(temp_curret.GetY() + offsetY > 0)
                                {
                                    int start = 0,end = 0;
                                    TextProcessing::TLinesData* lines = null; 
                                    if(text_edit_style  == eMultiLine)lines = &line_indexes;
                                    TextProcessing::TextUtils::GetWordBorders(font,lines,text,temp_curret,start,end,-border.x1 + text_cur_offsetX,border.x2 - 2 * border.x1 + text_cur_offsetX,fontScale);
                                    if(selection.Select(font,lines,text,temp_curret,start,end,fontScale))
                                    {
                                        curret.Hide();
                                        menuID = eMenuReplace;
                                        core->Mobile()->IsClipboardEmpty(this);// will create menu in callback
                                        pDraggedCursor = null;
                                    }
                                }
								else
								{
									core->TraceTo("jhfjgjh", "temp_curret.GetY() + offsetY failed");
								}
                            } 
#endif
                        }
                    }
                    else
                    {
#ifndef OLD_VERSION
                        if(bDragg)
                        {
                            FindCursorPosition(local_ms_pos_x + pDraggedCursor->GetDraggetOffsetX(),local_ms_pos_y + pDraggedCursor->GetDraggetOffsetY(),pDraggedCursor);
                            if(pDraggedCursor !=  &curret)
                                CheckCurretsVisibility();
                        }
                        else
                        {
                            nTouchTimer += core->GetDeltaTime();
                            if(nTouchTimer >MENU_WAIT_TIME && nDraggDpos == 0 && pDraggedCursor == &curret)
                            {
                                nTouchTimer = 0;
                                nDraggDpos = 1;
                                menuID = eMenuPaste;
                                core->Mobile()->IsClipboardEmpty(this);// will create menu in callback
                                pDraggedCursor = null;
                            }
                        }
#endif
                    }
                }
                else
                {
                    pDraggedCursor = null;
#ifndef OLD_VERSION
                    if(curret.IsIn(local_ms_pos_x,local_ms_pos_y,true))
                    {
                        curret.HideMenu();
                        pDraggedCursor = &curret;
                    }
                    else
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
#endif
                            in_drag = true;
                            nDraggDpos = 0;
                            nTouchTimer = 0;
                }
            }
            else 
            {
#ifndef OLD_VERSION
                if(in_drag)
                {  
                    if(selection.cursors[TextProcessing::Selection::eSelectionCursorStart].IsOn())
                    {
                        float offsetY = -text_max_offset + text_cur_offsetY + border.y1;
                        float offsetX = -border.x1 + text_cur_offsetX;
                        selection.ShowMenu(offsetX,offsetY);
                    }
                }
#endif
                in_drag = false;
                pDraggedCursor = null;
                nDraggDpos = 0;
                nTouchTimer = 0;
            }

		}
        else
        {
            if(pDraggedCursor)
            {
                FindCursorPosition(local_ms_pos_x + pDraggedCursor->GetDraggetOffsetX(),local_ms_pos_y + pDraggedCursor->GetDraggetOffsetY(),pDraggedCursor);
                if(pDraggedCursor !=  &curret)
                    CheckCurretsVisibility();
            }
        }
		if (focusedWidget == this && !selection.IsShow())
		{			
			wchar_t chr;

			if (core->Controls()->GetLastChar(chr))
			{

				if ((((chr == 9 || chr == 13) && returnKey == 0) || chr > 31) && CalcUtf8Len(text.c_str()) < max_text)
				{
					if (ChracterIsAllowed(chr))
					{
						string tmp;					

						if (chr == 9 || chr == 13)
						{
							if (text_edit_style != eOneLine)
							{
								char str_tmp[2];
								str_tmp[0] = 10;
								str_tmp[1] = 0;
								tmp = str_tmp;
							}
						}
						else
						{
							BuildUtf8fromUtf16(chr, tmp);
						}

						text.insert(curret.Index(),  tmp);

						core->Script()->Machine()->AddPtrArg(this);
						core->Script()->Machine()->Call(&callback_change_text);

                        MoveCurretRight(tmp.size());
					}
				}
				else
				{
					if (chr == 8 && text.length() > 0)
					{						
						int last_index = 0;
						int next_index = -1;

						int w = 0;
						int bytes = 0;

                        int cursor_pos = curret.Index();

	    				for (int i=0;i<cursor_pos;++i)
						{											
							if (!BuildUtf16fromUtf8(text[i], bytes, w))continue;
							if (next_index == -1)next_index = i+1;
							else
							{
								last_index = next_index;
								next_index = i+1;
							}													
						}					

						text.erase(last_index, cursor_pos - last_index);

						core->Script()->Machine()->AddPtrArg(this);
						core->Script()->Machine()->Call(&callback_change_text);

                        MoveCurretLeft(cursor_pos - last_index);

					}
					if ((chr == 9 || chr == 13) && returnKey != 0)
					{										
						if (funcOnReturnKey[0])
						{
							core->Script()->Machine()->AddPtrArg(this);
							core->Script()->Machine()->Call(&callback_focus_retrn_key);			
						}						
					}
				}
			}
		}				
	}	

	float alp = alpha;
	
	if (parent)
	{
		alp *= parent->alpha;
	}

	overColor.a = alp;

	
	if (slice.tex.texture || slices[0].tex_entity)
	{
		if (overColor.a > 0.01f)
		{
			if (slices[0].tex_entity)
			{
				TextureEntry::Draw(this, 0, 0, scr_w, scr_h, overColor, &slices[0].tex_entity->slice);
			}
			else
			{
				TextureEntry::Draw(this, 0, 0, scr_w, scr_h, overColor, &slice);
			}
		}	
	}

    float offsetY = -text_max_offset + text_cur_offsetY + border.y1;
    float offsetX = border.x1 - text_cur_offsetX;
#ifndef OLD_VERSION
    NGUIService::instance->RemoveTouchRegion(this);
    if (!core->IsEditorMode() && focused)
        selection.Render(offsetX,offsetY,fontScale);
#endif

	if (font)
	{
		if (text.c_str()[0] || IsFocused())
		{
			fontDesc.color.a = alp;
			AddPrintCall(font,offsetX,offsetY, scr_w - border.x1 * 2, scr_h - border.y1 * 2 + text_max_offset, &line_indexes, fontScale, fontDesc.color, text.c_str());		
		}
		else
		{
			float real_alpha = alpha * placeholder_alpha;
			if (parent)
				real_alpha *= parent->alpha;
			fontDesc.color.a = real_alpha;
			AddPrintCall(font,offsetX,offsetY, scr_w - border.x1 * 2, scr_h - border.y1 * 2 + text_max_offset, &line_indexes, fontScale, fontDesc.color, placeholder_text.c_str());		
		}
	}	

	if (!core->IsEditorMode() && focused)
    {
        if(!selection.IsShow())
		{
			curret.Render(offsetX,offsetY,false);
		}
        else
		{
			selection.RenderCursors(offsetX,offsetY);
		}
    }
}

CALLWRAPPER(NGUIEditBox, NEbGetExtendedHeightScript, CALLWRAPPERRET(float))
CALLWRAPPERARG1(NGUIEditBox, NEbSetPlaceholderScript, CALLWRAPPERGETADRESS(string,arg1),CALLWRAPPERNORET())
CALLWRAPPERARG3(NGUIEditBox, NEbSetFontColorScript, CALLWRAPPERGETPARAM(float,arg1),CALLWRAPPERGETPARAM(float,arg2),CALLWRAPPERGETPARAM(float,arg3),CALLWRAPPERNORET())
CALLWRAPPERARG3(NGUIEditBox, NEbSetCursorColorScript, CALLWRAPPERGETPARAM(float,arg1),CALLWRAPPERGETPARAM(float,arg2),CALLWRAPPERGETPARAM(float,arg3),CALLWRAPPERNORET())
CALLWRAPPERARG4(NGUIEditBox, NEbSetSelectionColor, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3),CALLWRAPPERGETPARAM(float,arg4), CALLWRAPPERNORET())	

void CALLWRAPPERNAME(NEbScriptSetDelegate)(asIScriptGeneric *gen)
{
	NGUIEditBox* obj = (NGUIEditBox*)gen->GetObject();
	obj->NEbScriptSetDelegate(gen);	
}

void NGUIEditBox::MethodsToScript(const char* script_class_name)
{
	NGUIWidget::MethodsToScript(script_class_name);
	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetPlaceholder(string& text)", CALLWRAPPERNAMEAS(NEbSetPlaceholderScript));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "float GetExtendedHeight()", CALLWRAPPERNAMEAS(NEbGetExtendedHeightScript));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetFontColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NEbSetFontColorScript));			
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetCursoColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NEbSetCursorColorScript));
    core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetSelectionColor(float r, float g, float b,float a)", CALLWRAPPERNAMEAS(NEbSetSelectionColor));

	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterface("NGUIEditBoxDelegate");
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUIEditBoxDelegate", "void OnFocusGain(NGUIEditBox@)");
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUIEditBoxDelegate", "void OnFocusLost(NGUIEditBox@)");
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUIEditBoxDelegate", "void OnReturnKey(NGUIEditBox@)");
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUIEditBoxDelegate", "void OnChange(NGUIEditBox@)");

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetDelegate(NGUIEditBoxDelegate@ delegate)", CALLWRAPPERNAMEAS(NEbScriptSetDelegate));
}

void NGUIEditBox::SetFocused(bool focus)
{
	NGUIWidget::SetFocused(focus);

	if (focus && IsVisible())
	{
		if (!was_focued)
		{
			CalculateLinesAndOffsetY();
            curret.Reset();
            curret.CalculatePosition(text,line_indexes,font);
			need_call_callback = 1;			
			was_focued = true;	
		}
		core->Controls()->ShowSoftKeyboard((IControlsService::KeyboardType)keyboardType, (IControlsService::KeyboardReturnKey)returnKey);
	}
	else
	{
		if (was_focued)
		{			
			need_call_callback = 2;			
			was_focued = false;
			core->Controls()->HideSoftKeyboard();
            curret.Hide();
            selection.Hide();
		}
	}
}

#ifdef EDITOR
Color NGUIEditBox::GetDebugColor()
{
	return COLOR_WHITE;
}
#endif

void NGUIEditBox::NEbSetPlaceholderScript(const string& txt)
{
	placeholder_text = txt;
}

void NGUIEditBox::NWgtSetTextScript(const string& txt)
{	
	text = txt;	
	CalculateLinesAndOffsetY();
	if (font)
	{						
		float cursor_pos_x = 0;										
        float cursor_pos_y = 0;
        int cursor_pos = 0;
		if (line_indexes.size() <= 1)
        {
			cursor_pos = font->GetCharPosition(text.c_str(), cursor_pos_x, (int)(border.x2 * 2), 0, 100000, (TextAlign)text_align);
            curret.SetPosition(cursor_pos_x,cursor_pos_y,cursor_pos,0);
        }
		else
		{
			cursor_pos = font->GetCharPosition(text.c_str(), cursor_pos_x, (int)(border.x2 * 2), line_indexes[line_indexes.size() - 2].index + 1, 100000, (TextAlign)text_align);
			cursor_pos_y = (line_indexes.size() - 1) * ((float)font->GetCharHeight() + font->GetRowGap());
            curret.SetPosition(cursor_pos_x,cursor_pos_y,cursor_pos,line_indexes.size() - 1);
		}
	}
}

void NGUIEditBox::NWgtGetTextScript(std::string& val)
{	
	val = text;	
}

float NGUIEditBox::GetTextHeight(std::string& txt)
{
	if (!font) return -1.0f;

	ClipRect rect;

	rect.x1 = scr_x + border.x1;
	rect.y1 = scr_y + border.y1;		

	rect.x2 = scr_x + border.x2;
	rect.y2 = scr_y + border.y2;

	return  Convert2AbsVert((float)font->GetHeight(txt.c_str(), &rect, 1.0f, (TextAlign)text_align));
}

void NGUIEditBox::CalculateLinesAndOffsetY()
{
    if(text_edit_style  == eMultiLine)
    {
        if (font)
        {
            font->CalcLinesData(text.c_str(), line_indexes, border.x2 - border.x1, 1.0f, (TextAlign)text_align);

            float cur_height = GetTextHeight(text);

            int num_lines = (int)((ConvertVert(init_height) - border.y1 * 2 + font->GetRowGap() + 1)/ ((float)font->GetCharHeight() + font->GetRowGap()));		

            int num_lines2 = (int)((ConvertVert(cur_height) + font->GetRowGap() + 1) / ((float)font->GetCharHeight() + font->GetRowGap()));		

            if (num_lines2 > num_lines)
            {
                int delta = num_lines2 - num_lines;
                int delta2 = 0;

                if (delta > expand_lines)
                {
                    delta2 = delta - expand_lines;
                    delta = expand_lines;				
                }			

                expanded_height = Convert2AbsVert((delta) * ((float)font->GetCharHeight() + font->GetRowGap()));			
                text_max_offset = (delta2) * ((float)font->GetCharHeight() + font->GetRowGap());					

                if (text_cur_offsetY > text_max_offset)			
                    text_cur_offsetY = text_max_offset;
            }
            else
            {
                text_max_offset = 0;
                text_cur_offsetY = 0;
                expanded_height = 0;			
            }

            if(height  != init_height + expanded_height && !core->IsEditorMode())
            {
                height = init_height + expanded_height;
                CalcVertPos();
                border.y2 = scr_h - border.y1;
            }
        }
    }
    else
    {
        text_max_offset = 0;
        expanded_height = 0;	
        text_cur_offsetY = 0;
        line_indexes.clear();
    }
}

 void NGUIEditBox::NWgtScriptSetY(float set_y)
 {
     NGUIWidget::NWgtScriptSetY(set_y);
     init_Y = set_y;
     CalculateLinesAndOffsetY();
 }
void NGUIEditBox::NWgtScriptSetHeight(float h)
{
	NGUIWidget::NWgtScriptSetHeight(h);

	init_height = h;

	CalculateLinesAndOffsetY();
}


float NGUIEditBox::NEbGetExtendedHeightScript()
{
	return expanded_height;
}

void NGUIEditBox::NEbSetFontColorScript(float r, float g, float b)
{
	fontDesc.color.r = r;
	fontDesc.color.g = g;
	fontDesc.color.b = b;

	CalcColors();
}

void NGUIEditBox::NEbSetCursorColorScript(float r, float g, float b)
{
	cursor_Color.r = r;
	cursor_Color.g = g;
	cursor_Color.b = b;
    selection.SetCursorsColor(cursor_Color);
    curret.SetColor(cursor_Color);
}

void NGUIEditBox::NEbSetSelectionColor(float r,float g,float b,float a)
{
    selection.SetColor(Color(r,g,b,a));
}

void NGUIEditBox::NEbScriptSetDelegate(asIScriptGeneric *gen)
{
	asIScriptObject* scr_object = (asIScriptObject*)gen->GetArgObject(0);	

	asITypeInfo* tp_info = scr_object->GetObjectType();	

	asIScriptFunction* scr_func;

	scr_func = tp_info->GetMethodByDecl("void OnFocusGain(NGUIEditBox@)", false);
	callback_focus_gain.Set(scr_func, scr_object);	

	scr_func = tp_info->GetMethodByDecl("void OnFocusLost(NGUIEditBox@)", false);
	callback_focus_lost.Set(scr_func, scr_object);	

	scr_func = tp_info->GetMethodByDecl("void OnReturnKey(NGUIEditBox@)", false);
	callback_focus_retrn_key.Set(scr_func, scr_object);	

	scr_func = tp_info->GetMethodByDecl("void OnChange(NGUIEditBox@)", false);
	callback_change_text.Set(scr_func, scr_object);	
}

void NGUIEditBox::UpdateChildsPos()
{
	NGUIWidget::UpdateChildsPos();
	CalculateLinesAndOffsetY();
}

void NGUIEditBox::CalcTextOffsetY(TextProcessing::Cursor* pCursor)
{

    if (text_max_offset < 0.1f)text_cur_offsetY = 0.0f;
    else
    {
        float cursor_pos_y = pCursor->GetY();
        float ps_y = -text_max_offset + text_cur_offsetY + border.y1 + cursor_pos_y;
        if (ps_y < border.y1)
            text_cur_offsetY = text_max_offset - cursor_pos_y;
        else
            if (ps_y > scr_h - font->GetCharHeight() - border.y1 * 2.0f)
            {
                text_cur_offsetY = text_max_offset - border.y1 * 3.0f - cursor_pos_y + scr_h - font->GetCharHeight();
                if (text_cur_offsetY < 0) text_cur_offsetY = 0;
            }	
    }

}

void NGUIEditBox::ModifyTextClipRect(ClipRect& rect)
{	
	float _border = 0, nMinClipX = 0,nMaxClipX = 0;
    CalculateTextClipX(nMinClipX,nMaxClipX);
	if (slice.tex.texture && slice.type == 3)
	{
		_border = (float)slice.rects[0].width;
		if (slice.scale_type)
		{
			_border *= NGUIService::instance->h_mul_RefToScreen;
		}
	}

	if (rect.x1<scr_x + _border + nMinClipX) rect.x1 = scr_x + _border + nMinClipX;

	if (slice.tex.texture && slice.type == 3)
	{
		_border = (float)slice.rects[2].width;
		if (slice.scale_type)
		{
			_border *= NGUIService::instance->h_mul_RefToScreen;
		}
	}

	if (rect.x2>scr_x + nMaxClipX - _border) rect.x2 = scr_x +nMaxClipX - _border;

	if (slice.tex.texture && slice.type == 3)
	{
		_border = (float)slice.rects[0].height;
		if (slice.scale_type)
		{
			_border *= NGUIService::instance->v_mul_RefToScreen;
		}
	}

	if (rect.y1<scr_y + _border) rect.y1 = scr_y + _border;

	if (slice.tex.texture && slice.type == 3)
	{
		_border = (float)slice.rects[6].height;
		if (slice.scale_type)
		{
			_border *= NGUIService::instance->v_mul_RefToScreen;
		}
	}

	if (rect.y2>scr_y + scr_h - _border) rect.y2 = scr_y + scr_h - _border;	
}

bool NGUIEditBox::ChracterIsAllowed(wchar_t chr)
{	
	if (keyboardType == 2)
	{
		if (chr < '0' || chr > '9')
		{
			return false;
		}
	}
	else
	if (keyboardType == 3)
	{	
		if (text.size() == 0 && chr == '+')
		{
			return true;
		}

		if (chr < '0' || chr > '9')
		{
			return false;
		}
	}
	else
	if (keyboardType == 4)
	{
		if (chr > 128)
		{
			return false;
		}
	}	

	return true;
}

void NGUIEditBox::Release()
{
	RELEASE(font)
	NGUIWidget::Release();	
}

void NGUIEditBox::SetText(const char* txt)
{
	 string tmp = txt;
	 ClampUtf8(tmp,max_text);
	 NGUIWidget::SetText(tmp.c_str()); 
}


void NGUIEditBox::CalculateLocalMousePos(float& local_ms_pos_x,float& local_ms_pos_y)
{
    local_ms_pos_x = 0;
    if(text_edit_style == eMultiLine)
        local_ms_pos_x = ms_pos_x - scr_x - border.x1;
    else
        local_ms_pos_x = ms_pos_x - scr_x + text_cur_offsetX - border.x1;
    local_ms_pos_y = ms_pos_y - scr_y + text_max_offset - text_cur_offsetY - border.y1;
}

bool NGUIEditBox::IsHitted(float mx, float my)
{
    if(NGUIWidget::IsHitted(mx,my))
        return true;

    float local_ms_pos_x = mx - scr_x,local_ms_pos_y = my - scr_y;

    float offsetY =  text_max_offset - text_cur_offsetY - border.y1;
    float offsetX =  text_cur_offsetX - border.x1;

    if(curret.IsIn(local_ms_pos_x + offsetX,local_ms_pos_y  + offsetY))
        return true;


    if(selection.IsHitted(local_ms_pos_x,local_ms_pos_y,offsetX,offsetY))
        return true;
           
    return false;
}

void NGUIEditBox::FindCursorPosition(float x,float y,TextProcessing::Cursor* pCursor)
{
    if (text_edit_style == eMultiLine)
    {
        text_cur_offsetX = 0;
        if(pCursor->FindPosition(x,y,font,&line_indexes,text,fontScale))
            CalcTextOffsetY(pCursor);
    }
    else
    {
        pCursor->FindPosition(x,y,font,NULL,text,fontScale);
        if(pCursor->GetX() - text_cur_offsetX + border.x1 >= border.x2)
            text_cur_offsetX =  pCursor->GetX() - border.x2 + border.x1;
        else
            if(pCursor->GetX() - text_cur_offsetX + border.x1 < border.x1)
                text_cur_offsetX -= fabs(border.x1 - (pCursor->GetX()- text_cur_offsetX + border.x1));
    }
}


void NGUIEditBox::PasteFromClipboard(const char* sBuffer)
{
    int start = selection.cursors[TextProcessing::Selection::eSelectionCursorStart].Index();
    int count = selection.cursors[TextProcessing::Selection::eSelectionCursorEnd].Index() - selection.cursors[TextProcessing::Selection::eSelectionCursorStart].Index();
    switch(menuID)
    {
    case eMenuReplace:
        {
            if(sBuffer[0] != 0)
            {
                int len = strlen(sBuffer);
                text.erase(start,count);
                text.insert(start,sBuffer);
                if(curret.Index() >  start)
                {
                    if(len < count)
                        MoveCurretLeft(count - len);
                    else
                        if(len > count)
                            MoveCurretRight(len - count);
                }
                else ReCalculateTextOffsets();
            }
            break;
        }
    case eMenuPaste:
        {
            if(sBuffer[0] != 0)
            {
                text.insert(curret.Index(),sBuffer);
                if(curret.Index() >  start)MoveCurretRight(strlen(sBuffer));
                else ReCalculateTextOffsets();
            }
            break;
        }
    }

}

void NGUIEditBox::ReCalculateTextOffsets()
{
    if(text_edit_style == eMultiLine)CalculateLinesAndOffsetY();
    else
    {
        float  textW = (float)font->GetWidth(text.c_str()); // TODO: calculate  width when typing!!!! 
        if(textW - text_cur_offsetX < border.x2)  
            text_cur_offsetX = textW - border.x2 + border.x1;
    }
}

void NGUIEditBox::OnRequestIsClipboardEmpty(int bClipboardEmpty)
{
    switch(menuID)
    {
    case eMenuReplace:
        {
            TextProcessing::Menu::TMenuIds  ids;
			ids.push_back(eMenuCut);
            ids.push_back(eMenuCopy);
            
            if(!bClipboardEmpty)
                ids.push_back(eMenuReplace);

            float offsetY = -text_max_offset + text_cur_offsetY + border.y1;
            float offsetX = -border.x1 + text_cur_offsetX;
            selection.CreateMenu(ids,TextProcessing::Menu::vEditMenuText,offsetX,offsetY);
            break;
        }
    case eMenuPaste:
        {
            TextProcessing::Menu::TMenuIds  ids;
            ids.push_back(eMenuSelect);
            if(!bClipboardEmpty)
                ids.push_back(eMenuPaste);

            float offsetY = -text_max_offset + text_cur_offsetY + border.y1;
            float offsetX = -border.x1 + text_cur_offsetX;
            curret.CreateMenu(ids,TextProcessing::Menu::vEditMenuText,offsetX,offsetY);
            break;
        }
    }
}







