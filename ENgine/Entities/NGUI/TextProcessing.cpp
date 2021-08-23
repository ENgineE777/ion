#include "TextProcessing.h"
#include "NGUIService.h"
#include "NGUIFont.h"
#include "NGUIWidget.h"
#include "Core/Font/FontRes.h"

#define MENU_ARROW_CELL_SCALE 1.5f

TOKEN_LIBRARY
	REGISTER_TOKEN("colortbl",TextProcessing::ColorTable)
	REGISTER_TOKEN("r",TextProcessing::RedColor)
	REGISTER_TOKEN("g",TextProcessing::GreenColor)
	REGISTER_TOKEN("b",TextProcessing::BlueColor)
	REGISTER_TOKEN("a",TextProcessing::AlphaColor)
	REGISTER_TOKEN("clr",TextProcessing::TableColor)
	REGISTER_TOKEN("tab",TextProcessing::Tab)
	REGISTER_TOKEN("sh",TextProcessing::Shadow)
	REGISTER_TOKEN("shr",TextProcessing::ShadowRedColor)
	REGISTER_TOKEN("shg",TextProcessing::ShadowGreenColor)
	REGISTER_TOKEN("shb",TextProcessing::ShadowBlueColor)
	REGISTER_TOKEN("sha",TextProcessing::ShadowAlphaColor)
	REGISTER_TOKEN("shclr",TextProcessing::ShadowTableColor)
	REGISTER_TOKEN("fi",TextProcessing::FontItalic)
	REGISTER_TOKEN("fb",TextProcessing::FontBold)
	REGISTER_TOKEN("fn",TextProcessing::FontName)
	REGISTER_TOKEN("fz",TextProcessing::FontSize)
	REGISTER_TOKEN("gap",TextProcessing::FontGap)
	REGISTER_TOKEN("scl",TextProcessing::Scale)
	REGISTER_TOKEN("fonttbl",TextProcessing::FontTable)
	REGISTER_TOKEN("f",TextProcessing::FontFromTable)
	REGISTER_TOKEN("algL",TextProcessing::AlignLeft)
	REGISTER_TOKEN("algR",TextProcessing::AlignRight)
	REGISTER_TOKEN("algC",TextProcessing::AlignCenter)
	REGISTER_TOKEN("lnk",TextProcessing::Link)
	REGISTER_TOKEN("ln",TextProcessing::NextLine)
	REGISTER_TOKEN("pushcolor",TextProcessing::PushColor)
	REGISTER_TOKEN("popcolor",TextProcessing::PopColor)
	TOKEN_LIBRARY_END

	namespace TextProcessing
{
	static std::vector<FontRes::LineData> line_indices;

	void Cursor::Render(FontInstance* pFont,float nScale,float offsetX,float offsetY,bool bClearTouchRegion)
	{
		float scale = en_core.Render()->Display()->GetScreenDPI() / 326.0f;

		FontRes::Glyph* glyph = pFont->GetCharGlyph('|');

		float curretOffsetX = 1.0f;
		float curretH = (float)pFont->GetCharHeight();

		float vertOffset = 3.0f * scale;

		if (glyph)
		{
			//curretOffsetX = (float)(glyph->x_offset + glyph->width * 0.5f);
		}

		float charHigth = (pFont->GetCharHeight() + pFont->GetRowGap());

		if (bAnimatedCurret)
		{
			Color buffColor = color;
			buffColor *= pParent->overColor;
			nCurretAlpha += core->GetDeltaTime();

			if (nCurretAlpha > 1.0f)
			{
				nCurretAlpha -= 1.0f;
			}

			if (nCurretAlpha > 0.5f)
			{
				buffColor.a = (1.0f - nCurretAlpha) * 2.0f;
			}
			else
			{
				buffColor.a = nCurretAlpha * 2.0f;
			}

			//pParent->AddPrintCall(pFont,nX + offsetX - curretOffsetX,nY + offsetY,curretOffsetX, curretH, &line_indices, nScale, buffColor, "|");

			NGUIService::instance->AddTopSpriteCall(Cursor::pCarretTexture, nX + offsetX - curretOffsetX, nY + offsetY - vertOffset, 3, charHigth + vertOffset * 2, buffColor, pParent->global_mat, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		}

		if (bShow)
		{
			float charHigth = (pFont->GetCharHeight() + pFont->GetRowGap());

			Color buffColor = color;
			buffColor *= pParent->overColor;

			if (nFadeTimer > 0)
			{
				buffColor.a =  1 - nFadeTimer / nFadeTime;
			}
			else
				if (nFadeTimer <= -nShowTime)
				{
					buffColor.a = (nFadeTimer + nShowTime + nFadeTime) / nFadeTime;
					if(buffColor.a <= 0.1)
					{
						bShow = false;
					}
				}

				nFadeTimer -= core->GetDeltaTime();
				nAlpha = buffColor.a;

				/*if (bRenderWithCurret)
				{
				pParent->AddPrintCall(pFont, nX + offsetX - curretOffsetX, nY + offsetY,curretOffsetX, (charHigth - nHeigth) * 0.5f,  &line_indices, nScale, Color(0,0,0,nAlpha), "|");
				}*/

				if (bRender)
				{                
					float y = offsetY + nY + (charHigth - nHeigth) * 0.5f;
					float x = offsetX + nX - curretOffsetX - nWidth * 0.5f * scale + 2;// - curretOffsetX;

					switch(eAlign)
					{
					case eCursorRight:x = offsetX + nX - nWidth - curretOffsetX;
						break;
					case eCursorLeft:x = offsetX + nX + curretOffsetX;
						break;
					case eCursorTop:y = offsetY + nY - nHeigth * scale + 1;
						break;
					case eCursorBottom:y = offsetY + nY + charHigth - 1;
						break;
					}

					if (bClearTouchRegion)
					{
						NGUIService::instance->RemoveTouchRegion(pParent);
					}

					NGUIService::instance->AddTouchRegion(x + pParent->global_mat.pos_x, y + pParent->global_mat.pos_y, nWidth, nHeigth, pParent);
					//        core->TraceTo("Debug","Dpi = %d scaleS = %f scale = %f w = %f  h = %f ",en_core.Render()->Display()->GetScreenDPI(),en_core.Render()->Display()->GetScreenDPI() / 25.4f,nScale,nWidth,nHeigth);

					NGUIService::instance->AddTopSpriteCall(Cursor::pTexture, x, y + vertOffset, nWidth * scale, nHeigth * scale, buffColor, pParent->global_mat, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

					if (bMenuShow)
						Menu::GetInstance()->Render(nAlpha,false,pParent);
				}
		}
	}

	void Cursor::Render(float offsetX,float offsetY,bool bClearTouchRegion)
	{
		Render(pParent->font, scale, offsetX, offsetY, bClearTouchRegion);
	}

	void Cursor::Show(bool bSet)
	{
		if(!bShow && bSet)
		{
			nFadeTimer = nFadeTime;
			bShow = true;
			bRender = true;
		}
		if(bShow && !bSet)nFadeTimer = -nShowTime;

	}

	bool Cursor::IsIn(float x,float y,bool bCalcDraggOffsets) 
	{
		if(!bShow)return false;
		ClipRect rect;
		float charHigth = (pParent->font->GetCharHeight() + pParent->font->GetRowGap()) * scale;
		FontRes::Glyph* glyph = pParent->font->GetCharGlyph('|');
		float curretW = 2;
		if (glyph)
			curretW = (float)glyph->width;
		rect.x1 = nX - nWidth;
		rect.y1 = nY + (charHigth - nHeigth) * 0.5f;
		switch(eAlign)
		{
		case eCursorRight:
			{
				rect.x1 = nX - 2 * nWidth - curretW;
				rect.y2 = rect.y1 + nHeigth;
				break;
			}
		case eCursorLeft:
			{
				rect.x1 = nX + curretW;
				rect.y2 = rect.y1 + nHeigth;
				break;
			}
		case eCursorBottom:
			{
				rect.y1 = nY;
				rect.y2 = rect.y1 + 2 * nHeigth + charHigth;
				break;
			}
		case eCursorTop:
			{
				rect.y1 = nY - 2 * nHeigth;
				rect.y2 = rect.y1 + nHeigth + charHigth;
				break;
			}
		}
		rect.x2 = rect.x1 +  2 * nWidth;
		if(bCalcDraggOffsets)
		{
			nDraggetOffsetY = (nY + charHigth * 0.5f) - y;
			nDraggetOffsetX = nX - x;
		}
		return (rect.x1 < x && x < rect.x2 && rect.y1 < y && y < rect.y2);
	}

	void Cursor::SetPosition(float x,float y,int index,int line,bool bCompareLineWithBorder /* =  false */)
	{
		if(nFadeTimer < 0)nFadeTimer = 0;
		if(pLeftBorder)
		{
			pLeftBorder->ResetFadeTimer();
			if(bCompareLineWithBorder)
			{
				if(line < pLeftBorder->Line())return;
				else
					if(line == pLeftBorder->Line() && index <= pLeftBorder->Index())return;
			}
			else
				if(index <= pLeftBorder->Index())return;
		}
		if(pRightBorder)
		{
			pRightBorder->ResetFadeTimer();
			if(bCompareLineWithBorder)
			{
				if(line > pRightBorder->Line())return;
				else
					if(line == pRightBorder->Line() && index >= pRightBorder->Index())return;
			}
			else
				if(index >= pRightBorder->Index())return;
		}
		nX = x;
		nY = y;
		nIndex = index;
		nLine = line;
	}

	void Cursor::Reset()
	{
		if(nIndex == -1)
		{
			nIndex = 0;
			nX = nY = 0;
		}
	}

	void Cursor::CalculatePosition(const string& text,const std::vector<FontRes::LineData>& lines,FontInstance* font)
	{
		if (nIndex == -1)return;
		if (lines.size() == 0 || (lines.size() > 0 && nIndex < lines[0].index))
		{
			string txt = text.substr(0, nIndex);		
			nX = (float)font->GetWidth(txt.c_str());
			nY = 0;
		}
		else
		{
			for (unsigned i = 0; i < lines.size(); ++i)
			{
				if (i < lines.size() - 1 && nIndex >= lines[i + 1].index) continue;
				if (nIndex >= lines[i].index)
				{			
					string txt;
					txt = text.substr(lines[i].index, nIndex - lines[i].index);
					nX = (float)font->GetWidth(txt.c_str());
					nY = (i + 1) * ((float)font->GetCharHeight() + font->GetRowGap());
					break;
				}
			}
		}
	}

	void Cursor::CalculatePosition(const string& text,FontInstance* font,float y)
	{
		if (nIndex == -1)return;
		string txt = text.substr(0, nIndex);		
		nX = (float)font->GetWidth(txt.c_str());
		nY = y;
	}

	void Cursor::Init(const Color& _color,float _scale,int align,bool animatedCurret,NGUIWidget* parent,bool renderWithCurret /*= false*/)
	{
		SetColor(_color);
		eAlign = align;
		bAnimatedCurret = animatedCurret;
		pParent = parent;
		bRenderWithCurret = renderWithCurret;
		scale = _scale;

	}

	bool Cursor::FindPosition(float x,float y,FontInstance* pFont,TLinesData* line_indexes,const string& text,float scale)
	{
		if (line_indexes)
		{
			float lineH = ((float)pFont->GetCharHeight() + pFont->GetRowGap()) * scale;
			int num_lines = (int)( y /  lineH);
			if (num_lines >= 0 && num_lines < (int)line_indexes->size())
			{			
				int cursor_pos = nIndex;
				float cursor_pos_x = 0;
				if (line_indexes->size() <= 1)
					cursor_pos = TextUtils::GetCharPositionInLine(pFont,text,cursor_pos_x, 0,text.size(), x,scale);
				else
				{
					if(num_lines  == 0)
						cursor_pos = TextUtils::GetCharPositionInLine(pFont,text,cursor_pos_x, 0,line_indexes->operator[](num_lines).index,x,scale);
					else 
						cursor_pos = TextUtils::GetCharPositionInLine(pFont,text,cursor_pos_x,line_indexes->operator[](num_lines - 1).index,line_indexes->operator[](num_lines).index,x,scale);
				}
				SetPosition(cursor_pos_x,num_lines * lineH,cursor_pos,num_lines);
				return true;
			}	
			return false;
		}
		else
		{
			int cursor_pos = nIndex;
			float cursor_pos_x = 0;
			cursor_pos = TextUtils::GetCharPositionInLine(pFont,text,cursor_pos_x, 0,text.size(), x,scale);
			SetPosition(cursor_pos_x,0,cursor_pos,0);
			return true;
		}
	}

	void Cursor::CreateMenu(const Menu::TMenuIds& ids,const Menu::TMenuNames& menuText,float offsetX /*= 0*/,float offsetY /*= 0*/)
	{
		Menu::GetInstance()->Create(ids,menuText,nX - offsetX,nY + offsetY,pParent);
		ResetFadeTimer();
		bMenuShow = true;
	}

	int Cursor::CheckMenuPress(float localX,float localY,bool& bSuccess)
	{
		if(bMenuShow)
		{
			int ret = Menu::GetInstance()->CheckPress(localX,localY,bSuccess);
			if(bSuccess)
				ResetFadeTimer();
			return ret;
		}
		bSuccess = false;
		return -1;
	}

	bool Cursor::IsOn() const
	{
		if(bShow)
		{
			return nFadeTimer > -nShowTime;
		}
		return false;
	}

	void Selection::Set(float startX,float endX,int start,int end,int line,float lineY)
	{
		cursors[eSelectionCursorStart].SetPosition(startX,lineY,start,line);
		cursors[eSelectionCursorStart].Show(true);
		cursors[eSelectionCursorEnd].SetPosition(endX,lineY,end,line);
		cursors[eSelectionCursorEnd].Show(true);

		cursors[eSelectionCursorEnd].SetLeftBorder(&cursors[eSelectionCursorStart]);
		cursors[eSelectionCursorStart].SetRightBorder(&cursors[eSelectionCursorEnd]);
		Menu::GetInstance()->Reset();
		bShowMenu = true;
	}

	void Selection::RenderCursors(float offsetX,float offsetY)
	{
		cursors[Selection::eSelectionCursorStart].Render(offsetX,offsetY,false);
		cursors[Selection::eSelectionCursorEnd].Render(offsetX,offsetY,false);
	}

	void Selection::Render(float offsetX,float offsetY,float scale)
	{
		if(cursors[eSelectionCursorStart].IsShow() && pTexture)
		{
			Color buffColor = color;
			buffColor.a = cursors[eSelectionCursorStart].GetAlpha();
			buffColor *= pParent->overColor;
			int lines = cursors[eSelectionCursorEnd].Line() - cursors[eSelectionCursorStart].Line();
			float nFirstLineTextWidth = cursors[eSelectionCursorEnd].GetX();
			float charHigth = (pParent->font->GetCharHeight() + pParent->font->GetRowGap()) * scale;
			if(lines  == 0)
			{
				float w  = cursors[eSelectionCursorEnd].GetX() - cursors[eSelectionCursorStart].GetX();
				pParent->AddDrawCall(pTexture,offsetX + cursors[eSelectionCursorStart].GetX(),offsetY + cursors[eSelectionCursorStart].GetY(),w,charHigth,buffColor,0,0,1,1);
			}
			else
			{
				int startIndex = 0;
				if(cursors[eSelectionCursorStart].Line() > 0)
					startIndex = pParent->line_indexes[cursors[eSelectionCursorStart].Line() - 1].index;
				nFirstLineTextWidth = TextUtils::GetTextWidthFrom(pParent->font,pParent->text,startIndex,pParent->line_indexes[cursors[eSelectionCursorStart].Line()].index,false,scale);
				float w  = nFirstLineTextWidth - offsetX - cursors[eSelectionCursorStart].GetX();
				pParent->AddDrawCall(pTexture,offsetX + cursors[eSelectionCursorStart].GetX(),offsetY + cursors[eSelectionCursorStart].GetY(),w,charHigth,buffColor,0,0,1,1);
				float y = cursors[eSelectionCursorStart].GetY() + charHigth;
				for(int i = 1; i < lines;++i)
				{
					int line = cursors[eSelectionCursorStart].Line() + i;
					startIndex = pParent->line_indexes[line - 1].index;
					float textWidth = TextUtils::GetTextWidthFrom(pParent->font,pParent->text,startIndex,pParent->line_indexes[line].index,false,scale);
					pParent->AddDrawCall(pTexture,offsetX,offsetY + y,textWidth - offsetX,charHigth,buffColor,0,0,1,1);
					y += charHigth;
				}
				pParent->AddDrawCall(pTexture,offsetX,offsetY + cursors[eSelectionCursorEnd].GetY(),cursors[eSelectionCursorEnd].GetX(),charHigth,buffColor,0,0,1,1);
			}
			if(bShowMenu && (cursors[eSelectionCursorStart].IsRender() || cursors[eSelectionCursorEnd].IsRender()))
				Menu::GetInstance()->Render(cursors[eSelectionCursorStart].GetAlpha(),false,pParent);
		}

	}

	void Selection::Render(const RichText* pText)
	{
		if(cursors[eSelectionCursorStart].IsShow() && pTexture)
		{
			Color buffColor = color;
			buffColor.a = cursors[eSelectionCursorStart].GetAlpha();
			buffColor *= pParent->overColor;
			const RichText::TText&  vText = pText->GetText();
			const RichText::TextLine& first_line = vText[cursors[eSelectionCursorStart].Line()];
			float startX = cursors[eSelectionCursorStart].GetX();
			float endX = cursors[eSelectionCursorEnd].GetX();
			float firstLineEndX = endX;
			if(cursors[eSelectionCursorEnd].Line() - cursors[eSelectionCursorStart].Line()  == 0)
				pParent->AddDrawCall(pTexture,cursors[eSelectionCursorStart].GetX(),first_line.nY,endX- startX,first_line.nHeigth,buffColor,0,0,1,1);
			else
			{
				firstLineEndX = first_line.vElements[0].x + first_line.nWidth;
				float w  = (first_line.vElements[0].x + first_line.nWidth) - startX;
				pParent->AddDrawCall(pTexture,startX,first_line.nY,w,first_line.nHeigth,buffColor,0,0,1,1);
				for(int i = cursors[eSelectionCursorStart].Line() + 1; i < cursors[eSelectionCursorEnd].Line();++i)
				{
					const RichText::TextLine& line = vText[i];
					pParent->AddDrawCall(pTexture,line.vElements[0].x,line.nY,line.nWidth,line.nHeigth,buffColor,0,0,1,1);
				}

				const RichText::TextLine& last_line = vText[cursors[eSelectionCursorEnd].Line()];
				pParent->AddDrawCall(pTexture,last_line.vElements[0].x,last_line.nY,endX - last_line.vElements[0].x,last_line.nHeigth,buffColor,0,0,1,1);
			}
			if(bShowMenu && (cursors[eSelectionCursorStart].IsRender() || cursors[eSelectionCursorEnd].IsRender()))
				Menu::GetInstance()->Render(cursors[eSelectionCursorStart].GetAlpha(),false,pParent);
		}

	}

	void Selection::Hide()
	{
		cursors[eSelectionCursorStart].Hide();
		cursors[eSelectionCursorEnd].Hide();
		cursors[eSelectionCursorStart].SetRightBorder(null);
		cursors[eSelectionCursorEnd].SetLeftBorder(null);
	}

	void Selection::Init(NGUIWidget* parent,const Color& _color,float _scale)
	{
		pParent = parent;
		color  = _color;
		scale = _scale;

	}

	void Selection::CreateMenu(const Menu::TMenuIds& ids,const Menu::TMenuNames& menuText,float offsetX /*= 0*/,float offsetY /*= 0*/)
	{
		Menu::GetInstance()->Create(ids,menuText,cursors[eSelectionCursorStart].GetX() - offsetX,cursors[eSelectionCursorEnd].GetX() - offsetX,cursors[TextProcessing::Selection::eSelectionCursorStart].GetY() + offsetY,pParent);
		bShowMenu = true;
	}

	void Selection::ShowMenu(float offsetX /*= 0*/,float offsetY /*= 0*/)
	{
		Menu::GetInstance()->CalculatePosition(cursors[eSelectionCursorStart].GetX() - offsetX,cursors[eSelectionCursorEnd].GetX() - offsetX,cursors[TextProcessing::Selection::eSelectionCursorStart].GetY() + offsetY,pParent);
		bShowMenu = true;
	}

	int Selection::CheckMenuPress(float localX,float localY,bool& bSuccess)
	{
		int ret =  Menu::GetInstance()->CheckPress(localX,localY,bSuccess);
		if(bSuccess)
		{
			cursors[Selection::eSelectionCursorStart].ResetFadeTimer();
			cursors[Selection::eSelectionCursorEnd].ResetFadeTimer();
		}
		return ret;
	}

	bool Selection::IsHitted(float local_ms_pos_x, float local_ms_pos_y,float offsetX /*= 0*/,float offsetY /*= 0*/)
	{
		if(IsOn())
		{
			if(cursors[Selection::eSelectionCursorStart].IsIn(local_ms_pos_x + offsetX,local_ms_pos_y + offsetY))
				return true;
			else
				if(cursors[Selection::eSelectionCursorEnd].IsIn(local_ms_pos_x + offsetX,local_ms_pos_y + offsetY))
					return true;
				else
				{  
					if( Menu::GetInstance()->IsHitted(local_ms_pos_x,local_ms_pos_y))
						return true;
				}
		}
		return false;
	}

	bool Selection::Select(FontInstance* pFont,TLinesData* line_indexes,const string& text,const Cursor& curret,int start, int end,float scale)
	{
		bool bCalculate = true;
		if(cursors[TextProcessing::Selection::eSelectionCursorStart].Index() == start || cursors[TextProcessing::Selection::eSelectionCursorEnd].Index() == end)
			bCalculate = !IsOn();
		if(bCalculate)
		{
			Hide();
			if(end > start)
			{
				int start_index = 0;
				if(line_indexes)
				{
					if(curret.Line()  > 0)
						start_index = line_indexes->operator[](curret.Line() - 1).index;
				}
				float startX = TextProcessing::TextUtils::GetTextWidthFrom(pFont,text,start_index,start,line_indexes != null,scale);
				float endX = TextProcessing::TextUtils::GetTextWidthFrom(pFont,text,start_index + (start - start_index),end,line_indexes != null,scale) + startX;
				Set(startX,endX,start,end,curret.Line(),curret.GetY());
				return true;
			}
		}
		return false;
	}


	void Selection::SetCursorsColor(const Color& _color)
	{
		cursors[TextProcessing::Selection::eSelectionCursorStart].SetColor(_color);
		cursors[TextProcessing::Selection::eSelectionCursorEnd].SetColor(_color);
	}

	void Menu::CalculateSize()
	{
		float nScale = en_core.Render()->Display()->GetScreenDPI() / 326.0f; 
		nHeigth =  nScale * 72.0f;
		nAnchorH = nScale * 20.0f;
		nAnchorW = nScale * 38.0f;
		nArrowW =  nScale * 14.0f;
		nArrowH =  nScale  * 14.0f;
		nLineWidth = 1.0f;
	}

	void Menu::Render(float alpha,bool bClearTouchRegion,NGUIWidget* pParent)
	{
		if(currentMenu.size() > 0)
		{
			float y =  -(nHeigth + nAnchorH - nY /* - 2 * nScale */ );

			if( y + nAnchorH < pParent->scr_h)
			{
				Color buffColor;
				buffColor = pParent->overColor;
				buffColor.a = alpha;

				if (bClearTouchRegion)
				{
					NGUIService::instance->RemoveTouchRegion(pParent);
				}

				NGUIService::instance->AddTouchRegion(nX + pParent->global_mat.pos_x, y + pParent->global_mat.pos_y, nWidth,nHeigth,pParent);
				TextureEntry::Draw(pParent, nX, y, nWidth, nHeigth, buffColor,&slice,false);

				NGUIService::instance->AddTopSpriteCall(pMenuAnchorTexture,nAnchorX, y + nHeigth - 3/*- 2 * nScale*/, nAnchorW,nAnchorH,buffColor,pParent->global_mat, -1.0f, 0.0f, 0.0f, 1.0f,1.0f);

				float init_x = menuTextTransform.pos_x;
				buffColor = Color(1,1,1,buffColor.a);
				ClipRect buffRect;

				for(unsigned i = 0; i < currentMenu.size();++i)
				{
					if (currentMenu[i].eSystemId == eMenuLeftArrow)
					{
						NGUIService::instance->AddTopSpriteCall(pMenuArrowLTexture,nX + currentMenu[i].rect.x1 + (currentMenu[i].nWidth - nArrowW) * 0.5f,y + (nHeigth - nArrowH) * 0.5f,nArrowW,nArrowH,buffColor, pParent->global_mat, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
					}
					else 
						if(currentMenu[i].eSystemId == eMenuRightArrow)
						{
							NGUIService::instance->AddTopSpriteCall(pMenuArrowRTexture,nX + currentMenu[i].rect.x1 + (currentMenu[i].nWidth - nArrowW) * 0.5f,y +(nHeigth - nArrowH) * 0.5f,nArrowW,nArrowH,buffColor, pParent->global_mat, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
						}
						else
						{
							menuTextTransform.pos_x = init_x + currentMenu[i].rect.x1;
							buffRect.x2 =  currentMenu[i].nWidth;
							NGUIService::instance->AddTopPrintCall(pMenuFont,menuTextTransform,buffRect,1,buffColor,text_center,core->Localize()->GetString(currentMenu[i].sName).c_str());
						}

						if (i + 1 < currentMenu.size())
						{
							Color cl(buffColor.r,buffColor.g,buffColor.b,buffColor.a * 0.7f);
							NGUIService::instance->AddTopSpriteCall(pLineTexture,nX + currentMenu[i].rect.x2,y,nLineWidth,nHeigth, cl, pParent->global_mat, -1.0f, 0.0f, 0.0f, 1.0f,1.0f);
						}
				}

				menuTextTransform.pos_x = init_x;
			}
		}
	}

	void Menu::ShowMenuPage(unsigned index)
	{
		if(index < menuPages.size())
		{
			nCurrentPage = index;
			currentMenu.clear();
			unsigned startIndex  = menuPages[index].nMin;
			float x = 0;
			if(index == 0)
			{
				currentMenu.push_back(SMenuEelement(menu[menuPages[index].nMin].eId,menu[menuPages[index].nMin].sName,x,menu[menuPages[index].nMin].nWidth + nArrowW,nHeigth));
				x += menu[menuPages[index].nMin].nWidth + nArrowW;
				for(unsigned i = menuPages[index].nMin + 1;i < menuPages[index].nMax;++i)
				{
					currentMenu.push_back(SMenuEelement(menu[i].eId,menu[i].sName,x,menu[i].nWidth,nHeigth));
					x += menu[i].nWidth;
				}
				currentMenu.push_back(SMenuEelement(x,nWidth - x,nHeigth,eMenuRightArrow));
			}
			else
				if(index == menuPages.size() - 1)
				{
					currentMenu.push_back(SMenuEelement(0,nArrowW * MENU_ARROW_CELL_SCALE,nHeigth,eMenuLeftArrow));
					x += nArrowW * MENU_ARROW_CELL_SCALE;
					for(unsigned i = menuPages[index].nMin;i < menuPages[index].nMax - 1;++i)
					{
						currentMenu.push_back(SMenuEelement(menu[i].eId,menu[i].sName,x,menu[i].nWidth,nHeigth));
						x += menu[i].nWidth;
					}
					currentMenu.push_back(SMenuEelement(menu[menuPages[index].nMax - 1].eId,menu[menuPages[index].nMax - 1].sName,x,nWidth - x,nHeigth));
				}
				else
				{
					currentMenu.push_back(SMenuEelement(0,nArrowW * MENU_ARROW_CELL_SCALE,nHeigth,eMenuLeftArrow));
					x += nArrowW * MENU_ARROW_CELL_SCALE;
					for(unsigned i = menuPages[index].nMin;i < menuPages[index].nMax;++i)
					{
						float w = menu[i].nWidth;
						if(i + 1 == menuPages[index].nMax)w = nWidth - x - nArrowW * MENU_ARROW_CELL_SCALE;
						currentMenu.push_back(SMenuEelement(menu[i].eId,menu[i].sName,x,w,nHeigth));
						x += w;
					}
					currentMenu.push_back(SMenuEelement(x,nArrowW * MENU_ARROW_CELL_SCALE,nHeigth,eMenuRightArrow));
				}
		}
	}

	bool Menu::IsHitted(float local_ms_pos_x, float local_ms_pos_y)
	{
		float y =  -(nHeigth + nAnchorH - nY);
		if( local_ms_pos_y > y && local_ms_pos_y < y + nHeigth &&  local_ms_pos_x > nX && local_ms_pos_x < nX + nWidth)
			return true;
		return false;
	}

	int Menu::CheckPress(float localX,float localY,bool& bSuccess)
	{
		SMenuEelement* pElement = null;
		float y =  -(nHeigth + nAnchorH - nY);
		for(unsigned i = 0; i < currentMenu.size();++i)
		{
			if(localY >= y && localY <= y  + 1.5f * nHeigth && localX >= nX + currentMenu[i].rect.x1   &&  localX <= nX + currentMenu[i].rect.x2 )
			{
				pElement = &currentMenu[i];
				break;
			}
		}
		if(pElement)
		{
			bSuccess = true;
			switch(pElement->eSystemId)
			{
			case eMenuRightArrow:
				{
					if(nCurrentPage + 1 < menuPages.size())
						ShowMenuPage(nCurrentPage + 1);
					return -1;
				}
			case eMenuLeftArrow:
				{
					if(nCurrentPage - 1 >= 0)
						ShowMenuPage(nCurrentPage - 1);
					return -1;
				}
			case eUser:
				{
					return pElement->eId;
				}

			}
		}
		bSuccess = false;
		return -1;
	}

	void Menu::CalculateX(float cursorStartX,float cursorEndX,NGUIWidget* pParent)
	{
		nX  = 0;
		if(nWidth <  pParent->scr_w)
		{
			float selW = cursorEndX - cursorStartX;
			nAnchorX = cursorStartX + selW * 0.5f - nAnchorW * 0.5f;
			if(nWidth < cursorStartX)
			{
				nX = cursorStartX + selW * 0.5f - nWidth * 0.5f;
				if(nX + nWidth > pParent->scr_w)
					nX = pParent->scr_w - nWidth;
				if(fabs((nAnchorX + nAnchorW) - (nX + nWidth)) < 5)nX += 5;
			}
			else
				if(nWidth > cursorStartX)
				{
					nX = nAnchorX - nWidth * 0.5f;
					if(nX < 0)nX = 0;
					if(nAnchorX  < nX + 5)nX = nAnchorX - 5;
				}
		}
		menuTextTransform.pos_x = pParent->global_mat.pos_x + nX;
	}

	bool Menu::BuildCurrentMenu(const TMenuIds& ids,const TMenuNames& menuText,float cursorStartX,float cursorEndX,float y)
	{
		if(pMenuFont)
		{
			CalculateSize();
			bShow = true;
			nWidth = 0;
			menu.clear();
			currentMenu.clear();
			float nMenuBorder = 110;

			if(ids.size() ==  1)
			{
				nMenuBorder = 40;
			}
			for(unsigned i = 0; i < ids.size();++i)
			{
				if(ids[i] < menuText.size())
				{
					float w = pMenuFont->GetWidth(menuText[ids[i]].c_str()) + nMenuBorder;
					menu.push_back(SMenuEelement(ids[i],menuText[ids[i]],nWidth,w,nHeigth));
					nWidth += w;
				}
			}
			if(menu.empty())return false;
			float nMaxMenuWidth = (float)en_core.Render()->Display()->GetWidth();
			if(nWidth > nMaxMenuWidth && menu.size() > 1)
			{
				float nMinWidth = menu[0].nWidth + nArrowW * MENU_ARROW_CELL_SCALE * 2;
				nWidth = nMaxMenuWidth;
				if(nWidth < nMinWidth)
					nWidth = nMinWidth;
				float nMaxWidth = nWidth - nArrowW * MENU_ARROW_CELL_SCALE * 2;
				unsigned i = 0;
				menuPages.clear();
				do
				{
					float w = 0;
					unsigned j = i;
					for(; j < menu.size();++j)
					{
						if(w  + menu[j].nWidth >  nMaxWidth)
						{
							if(w == 0)++j;
							break;
						}
						w += menu[j].nWidth;
					}
					if(menuPages.empty())
						menuPages.push_back(SMenuPage(i,j));
					else
					{
						if(menuPages.back().nMax != j)
							menuPages.push_back(SMenuPage(i,j));
					}
					i = j;
				}while(i <  menu.size());
				ShowMenuPage(0);
			}
			else
			{
				for(unsigned i = 0; i < menu.size();++i)
					currentMenu.push_back(menu[i]);
			}
			return true;
		}
		return false;
	}

	void Menu::CalculatePosition(float cursorX,float y,NGUIWidget* pParent)
	{
		CalculateSize();
		menuTextTransform = pParent->global_mat;
		CalculateX(cursorX,pParent);
		SetY(y,pParent->global_mat.pos_y);
		nAnchorX = cursorX - nAnchorW * 0.5f;
		if(nAnchorX  < nX + 5)nX = nAnchorX - 5;
		if(fabs((nAnchorX + nAnchorW) - (nX + nWidth))  < 5)nX += 5;
		for(unsigned i = 0; i < currentMenu.size() - 1;++i)
		{
			float realX = nX + currentMenu[i].rect.x2;
			if( realX > nAnchorX && realX < nAnchorX + nAnchorW)
			{
				nX += (nAnchorX + nAnchorW) - realX;
				break;
			}
		}
		menuTextTransform.pos_x =  pParent->global_mat.pos_x + nX;
	}

	void Menu::CalculatePosition(float cursorStartX,float cursorEndX,float y,NGUIWidget* pParent)
	{
		CalculateSize();
		menuTextTransform = pParent->global_mat;
		CalculateX(cursorStartX,cursorEndX,pParent);
		SetY(y,pParent->global_mat.pos_y);
		nAnchorX = cursorStartX + (cursorEndX - cursorStartX) * 0.5f - nAnchorW * 0.5f;
		if(nAnchorX  + nAnchorW >= pParent->scr_w)
			nAnchorX = pParent->scr_w  - nAnchorW - 5;
		if(nAnchorX  + nAnchorW < 0)
			nAnchorX = nAnchorW + 5;
		if(fabs((nAnchorX + nAnchorW) - (nX + nWidth))  < 5)nX += 5;
		if(nAnchorX  < nX + 5)nX = nAnchorX - 5;
		menuTextTransform.pos_x = pParent->global_mat.pos_x + nX;
		for(unsigned i = 0; i < currentMenu.size() - 1;++i)
		{
			if( nX + currentMenu[i].rect.x2 > nAnchorX &&  nX + currentMenu[i + 1].rect.x1 < nAnchorX + nAnchorW)
			{
				nAnchorX = nX + currentMenu[i + 1].rect.x1 + 2;
				break;
			}
		}
	}

	void Menu::Create(const TMenuIds& ids,const TMenuNames& menuText,float cursorStartX,float cursorEndX,float y,NGUIWidget* pParent)
	{
		if(BuildCurrentMenu(ids,menuText,cursorStartX,cursorEndX,y))
			CalculatePosition(cursorStartX,cursorEndX,y,pParent);
	}

	void Menu::Create(const TMenuIds& ids,const TMenuNames& menuText,float cursorX,float y,NGUIWidget* pParent)
	{
		if(BuildCurrentMenu(ids,menuText,cursorX,cursorX,y))
			CalculatePosition(cursorX,y,pParent);
	}

	void Menu::SetY(float y,float nParentY)
	{
		nY = y;
		menuTextTransform.pos_y = nParentY - (nHeigth + nAnchorH - nY) + (nHeigth  -  pMenuFont->GetHeight()) * 0.5f;
	}

	void Menu::CreateFont()
	{
		RELEASE(pMenuFont)   
			float scale = en_core.Render()->Display()->GetScreenDPI() / 326.0f;
		pMenuFont = en_core.Font()->CreateFont("helveticaneuecur",false, false, (int)(scale * 18), 0.5f, 0);
	}

	Menu* Menu::GetInstance()
	{
		if(!pInstance)pInstance = new Menu();
		return pInstance;
	}

	void Menu::Release()
	{
		if(pInstance) delete pInstance;
		pInstance = NULL;
	}

	Menu* Menu::pInstance = NULL;



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ParserState::GetColorFromTable(Color& color,unsigned index) const
	{
		if(index < vColors.size())
		{
			color.r = vColors[index].r;
			color.g = vColors[index].g;
			color.b = vColors[index].b;
			color.a *= vColors[index].a;
		}
	}

	void ParserState::Clear()
	{
		for(unsigned i = 0;i < vFonts.size();++i)
			for(unsigned j = 0;j < vFonts[i].size();++j)
				delete vFonts[i][j];
		vFonts.clear();
		for(unsigned i = 0;i < vTokens.size();++i)
			delete vTokens[i];
		vTokens.clear();
		vColors.clear();
	}

	void ParserState::GetColor(Color& color,unsigned& index,const TParserStack& tokens) const
	{
		if(index < tokens.size())
		{
			bool bInitColor = false;
			bool bInitAlpha = false;
			if(tokens[index]->Id() == IToken::eTextColor || tokens[index]->Id() == IToken::eTextShadowColor)
			{
				color.r = color.g = color.b = 0;
				bInitColor = true;
			}
			else
				if(tokens[index]->Id() == IToken::eTextAlpha || tokens[index]->Id() == IToken::eTextShadowAlpha)
				{
					color.a = 1;
					bInitAlpha = true;
				}
				tokens[index]->GetColor(color,this);
				IToken::ETokenID colorId,alphaId;
				if(tokens[index]->Id() == IToken::eTextColor || tokens[index]->Id() == IToken::eTextAlpha)
				{
					colorId =  IToken::eTextColor;
					alphaId =  IToken::eTextAlpha;

				}
				else
				{
					colorId =  IToken::eTextShadowColor;
					alphaId =  IToken::eTextShadowAlpha;
				}
				unsigned j = index + 1;
				for(;j < tokens.size();++j)
				{
					if(tokens[j]->Id() == colorId || tokens[j]->Id() == alphaId)
					{
						if(tokens[j]->Id() == colorId && !bInitColor)
						{
							color.r = color.g = color.b = 0;
							bInitColor = true;
						}
						if(tokens[j]->Id() == alphaId && !bInitAlpha)
						{
							color.a = 1;
							bInitAlpha = true;
						}
						tokens[j]->GetColor(color,this);
						++index;
					}
					else
						break;
				}
		}
	}

	void ParserState::GetFontParams(const IToken* pToken,FontParams& desc) const
	{
		switch(pToken->Id())
		{
		case IToken::eShadow:
			{
				desc.use_shadow = static_cast<const TextProcessing::Shadow*>(pToken)->IsOn();
				break;
			}
		case IToken::eFontName:
			{
				String::Copy(desc.name,128,static_cast<const TextProcessing::FontName*>(pToken)->GetData().c_str());
				break;
			}
		case IToken::eFontItalic:
			{
				desc.italic = static_cast<const TextProcessing::FontItalic*>(pToken)->IsOn();
				break;
			}
		case IToken::eFontBold:
			{
				desc.bold = static_cast<const TextProcessing::FontBold*>(pToken)->IsOn();
				break;
			}
		case IToken::eFontSize:
			{
				desc.height = static_cast<const TextProcessing::FontSize*>(pToken)->GetData();
				break;
			}
		case IToken::eFontGap:
			{
				desc.gap = static_cast<const TextProcessing::FontGap*>(pToken)->GetData();
				break;
			}
		}
	}

	void ParserState::GetFontParams(FontParams& desc,unsigned& index) const
	{
		if(index < vTokens.size())
		{
			GetFontParams(vTokens[index],desc);
			unsigned j = index + 1;
			for(;j < vTokens.size();++j)
			{
				switch(vTokens[j]->Id())
				{
				case IToken::eShadow:
				case IToken::eFontName:
				case IToken::eFontItalic:
				case IToken::eFontBold:
				case IToken::eFontSize:
				case IToken::eFontGap:
					{
						GetFontParams(vTokens[j],desc);
						++index;
						break;
					}
				default:return;
				}
			}
		}
	}

	HashMap<TextProcessing::RegistryItem> Parser::vTokensTable(_FL_);
	TextProcessing::ParserState Parser::result;

	const ParserState& Parser::Parse(const char* pText)
	{
		Clear();
		const char* pIter = pText;
		string  sBuffer;
		while(*pIter)
		{    
			int w = 0;
			int bytes = 0;	
			BuildUtf16fromUtf8(*pIter, bytes, w);
			int chr = w;		
			switch(chr)
			{
			case   13:++pIter;break;
			case   10:
				{
					++pIter;
					if(!sBuffer.empty())
					{
						result.vTokens.push_back(new  TextToken(sBuffer));
						sBuffer.clear();
					}
					result.vTokens.push_back(new  NextLine());
					break;
				}
			case '\\':
				{
					++pIter;
					if (*pIter)
					{			
						switch(*pIter)
						{
						case 'n':
							{
								++pIter; 
								if(!sBuffer.empty())
								{
									result.vTokens.push_back(new  TextToken(sBuffer));
									sBuffer.clear();
								}
								result.vTokens.push_back(new  NextLine());
								break;
							}
						case '}':
						case '{':
						case '#':
							{
								sBuffer += *pIter;
								++pIter;
								break;
							}
						}
					}
					break;
				}
			case '#':
				{
					string  sCmd;
					++pIter;
					const char* pOldIter =  pIter;
					if(!sBuffer.empty())
					{
						result.vTokens.push_back(new  TextToken(sBuffer));
						sBuffer.clear();
					}
					IToken* pToken = IToken::ParseToken(&pIter,&result);
					if(pToken)
						result.vTokens.push_back(pToken);
					else
					{
						pIter = pOldIter;
						sBuffer += '#';
					}
					break;
				}
			default:
				{
					sBuffer += *pIter;
					++pIter;
					break;
				}
			}

		}
		if(!sBuffer.empty())
			result.vTokens.push_back(new  TextToken(sBuffer));
		return result;
	}

	void Parser::Init()
	{
		if(vTokensTable.Size() == 0)
		{
			for (int i = 0; i < sizeof(vTokenRegistryTable) / sizeof(vTokenRegistryTable[0]); ++i)
				vTokensTable.Add(vTokenRegistryTable[i].sName.c_str(),&vTokenRegistryTable[i]);
		}
	}

	IToken* Parser::GetToken(const char* sName)
	{
		RegistryItem* pItem =  vTokensTable.Get(sName);
		if(pItem)
			return pItem->getInstanceFunction();
		return NULL;
	}

	void IToken::ParseTokenName(const char** pIter,string& sCmd)
	{
		sCmd.clear();
		while(**pIter)
		{
			switch (**pIter)
			{
			case ' ':
			case '{':
			case ';':
			case '#':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': return;
			default:
				{
					sCmd += **pIter;
					break;
				}
			}
			++(*pIter);
		}
	}

	bool IToken::ParseData(const char** pIter,float& data)
	{
		string sNumber;
		while(**pIter)
		{
			switch (**pIter)
			{
			case ' ':++(*pIter);
			case '}':
			case ';':
			case '#':
				{
					if(sNumber.empty())return false;
					data = static_cast<float>(atof(sNumber.c_str()));
					return true;
				}
			case ',':++(*pIter); sNumber += '.';break;
			case '.':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				{
					sNumber += **pIter;
					++(*pIter);
					break;
				}
			default: return false;
			}
		}
		return false;
	}

	bool IToken::ParseData(const char** pIter,int& data)
	{
		float buff;
		bool bRet = ParseData(pIter,buff);
		if(bRet)
			data = static_cast<int>(buff);
		return bRet;
	}

	bool IToken::ParseData(const char** pIter,string& data)
	{
		data.clear();
		if((**pIter) == '{')
		{
			++(*pIter);
			while(**pIter)
			{
				switch (**pIter)
				{
				case '#':return false;
				case '}':
					{
						++(*pIter);
						return !data.empty();
					}
				default:
					{
						data += **pIter;
						++(*pIter);
						break;
					}
				}
			}
		}
		return false;
	}

	bool IToken::ParseData(const char** pIter,ParserState* pState)
	{
		if(**pIter == ' ')
			++(*pIter);
		return true;
	}

	IToken* IToken::ParseToken(const char** pIter,ParserState* pState)
	{
		string  sCmd;IToken* pToken = NULL;
		ParseTokenName(pIter,sCmd);
		if(!sCmd.empty())
		{
			pToken = Parser::GetToken(sCmd.c_str());
			if(pToken && !pToken->ParseData(pIter,pState))
				return NULL;
		}
		return pToken;
	}

	bool ColorTable::ParseData(const char** pIter,ParserState* pState)
	{
		if((**pIter) == '{')
		{
			++(*pIter);
			TParserStack vTokens;
			while(**pIter)
			{
				switch (**pIter)
				{
				case '#':
					{
						++(*pIter);
						IToken* pToken = IToken::ParseToken(pIter,pState);
						if(pToken && (pToken->Id() == eTextColor || pToken->Id() == eTextAlpha)) 
							vTokens.push_back(pToken);
						break;
					}
				case ';':
					{
						Color color(0,0,0,1);
						for(unsigned i = 0;i < vTokens.size();++i)
						{
							vTokens[i]->GetColor(color);
							delete vTokens[i];
						}
						pState->AddColorToTable(color);
						vTokens.clear();
						++(*pIter);
						if((**pIter) == '}')return false;
						break;
					}
				case '}':
					{
						Color color(0,0,0,1);
						for(unsigned i = 0;i < vTokens.size();++i)
						{
							vTokens[i]->GetColor(color);
							delete vTokens[i];
						}
						pState->AddColorToTable(color);
						vTokens.clear();
						++(*pIter);
						return false;
					}
				default:++(*pIter);break;
				}

			}
			for(unsigned i = 0;i < vTokens.size();++i)
				delete vTokens[i];
			vTokens.clear();
		}
		return false;
	}

	bool FontTable::ParseData(const char** pIter,ParserState* pState)
	{
		if((**pIter) == '{')
		{
			++(*pIter);
			TParserStack vTokens;
			while(**pIter)
			{
				switch (**pIter)
				{
				case '#':
					{
						++(*pIter);
						IToken* pToken = IToken::ParseToken(pIter,pState);
						if(pToken)
						{
							switch(pToken->Id())
							{
							case IToken::eShadow:
							case IToken::eTextAlpha:
							case IToken::eTextColor:
							case IToken::eTextShadowAlpha:
							case IToken::eTextShadowColor:
							case IToken::eFontName:
							case IToken::eFontItalic:
							case IToken::eFontBold:
								{
									vTokens.push_back(pToken);
									break;
								}
							}
						}
						break;
					}
				case ';':
					{
						pState->AddFontToTable(vTokens);
						vTokens.clear();
						++(*pIter);
						if((**pIter) == '}')return false;
						break;
					}
				case '}':
					{

						pState->AddFontToTable(vTokens);
						vTokens.clear();
						++(*pIter);
						return false;
					}
				default:++(*pIter);break;
				}

			}
			vTokens.clear();
		}
		return false;
	}

	bool Tab::ParseData(const char** pIter,ParserState* pState)
	{
		data = 4;
		IToken::ParseData(pIter,data);
		return true;
	}

	bool Link::ParseData(const char** pIter,ParserState* pState)
	{
		if((**pIter) == '{')
		{
			++(*pIter);
			string sBuffer;
			char mode = 0;
			while(**pIter)
			{
				switch (**pIter)
				{
				case '#':return false;
				case ',':
					{
						++(*pIter);
						sCallBack = sBuffer;
						sBuffer.clear();
						mode = 1;
						break;
					}
				case '}':
					{
						++(*pIter);
						if(mode == 0)sCallBack = sBuffer;
						else
						{
							if(sBuffer.empty())return false;
							nIndex = atoi(sBuffer.c_str());
						}
						return !sCallBack.empty();
					}
				default:
					{
						sBuffer += **pIter;
						++(*pIter);
						break;
					}
				}
			}
		}
		else
			if((**pIter) == '0')
			{
				++(*pIter);
				if(**pIter == ' ')
					++(*pIter);
				return true;
			}
			return false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	FontInstance* RichText::GetFont(const FontParams& desc)
	{
		string sFontName;
		sFontName += desc.name;
		if(desc.bold)
			sFontName += "_b";
		if(desc.italic)
			sFontName += "_i";
		char buffer [10];
		String::Printf(buffer,9,"_%4.1f",desc.height);
		sFontName +=  buffer;

		FontInstance* font = vFonts.Get(sFontName.c_str());
		if(!font)
		{
			font = en_core.Font()->CreateFont(desc.name, desc.bold, desc.italic,(int)(NGUIService::instance->v_mul_RefToScreen * desc.height), desc.gap, desc.outline);	
			if(font)
				vFonts.Add(sFontName.c_str(),font);
		}
		return font;
	}

	void RichText::Set(const char* pText,const FontParams& initDesc,float scale,float width,const string& sLinksCallback,const Color& linksColor)
	{
		string sBuff = pText;
		for(unsigned i = 1;i < sBuff.size();++i)
		{
			char c = sBuff[i];
			if(sBuff[i] == '.')
			{
				string sDomain;
				unsigned nEnd = i + 1;
				while(sBuff[nEnd] != ' ' && sBuff[nEnd] != '\\' && sBuff[nEnd] != '/' && nEnd < sBuff.size())++nEnd;
				sDomain.assign(&sBuff[i + 1],nEnd - i - 1);
				while(sBuff[nEnd] != ' ' && nEnd < sBuff.size())++nEnd;
				if(!sDomain.empty())
				{
					bool bFind = false;
					if(sDomain.size() == 2)
					{
						for(unsigned j = 0;j < AlphaDomainsCount;++j)
						{
							if(vAlphaDomains[j].cAlpha == sDomain[0])
							{
								for(unsigned k = 0;k < AlphaDomainsNextLevelCount;++k)
								{
									if(vAlphaDomains[j].vNextAlpha[k] == 0)break;
									if(vAlphaDomains[j].vNextAlpha[k] == sDomain[1])
									{
										bFind = true;
										break;
									}
								}
								break;
							}
						}
					}
					else
						if(sDomain.size() < 7)
						{
							unsigned k = 0;
							for(; k < nDomainsCount;++k)
								if(sDomain == vDomains[k])break;
							bFind = k < nDomainsCount;
						}

						if(bFind)
						{
							int j = i - 1;
							for(;j > 0;--j)
								if(sBuff[j - 1] == ' ')break;

							char sColor[64];
							String::Printf(sColor,64,"#r%d#g%d#b%d#a%d",(int)(linksColor.r * 255),(int)(linksColor.g * 255),(int)(linksColor.b * 255),(int)(linksColor.a * 255));
							string sCommand = "#pushcolor" + string(sColor) + "#lnk{" + sLinksCallback +",-2} ";
							sBuff.insert(nEnd,"#lnk0#popcolor ");
							sBuff.insert(j,sCommand);
							i = nEnd + 14 + sCommand.length();
						}
				}
			}
		}
		Set(sBuff.c_str(),initDesc,scale,width);
	}

	void RichText::Set(const char* pText,const FontParams& initDesc,float scale,float width)
	{
		Parser::Init();
		vLinks.clear();
		vFontTable.clear();
		std::vector<Color> vColorStack;
		TextDescription desc(initDesc,scale,text_left);
		const TextProcessing::ParserState& state = TextProcessing::Parser::Parse(pText);
		for(unsigned i = 0;i < state.vFonts.size();++i)
		{
			FontParams buff(initDesc);
			for(unsigned j = 0;j < state.vFonts[i].size();++j)
			{
				switch(state.vFonts[i][j]->Id())
				{
				case IToken::eTextShadowAlpha:
				case IToken::eTextShadowColor:state.GetColor(buff.shd_color,j,state.vFonts[i]);break;
				case IToken::eTextAlpha:
				case IToken::eTextColor:state.GetColor(buff.color,j,state.vFonts[i]);break;
				default:state.GetFontParams(state.vFonts[i][j],buff);break;
				}
			}
			if(buff.font)
				vFontTable.push_back(buff);
		}
		vText.clear();
		float y = 0;
		nMaxWidth = width;
		for(unsigned i = 0;i < state.vTokens.size();++i)
		{
			switch(state.vTokens[i]->Id())
			{
			case IToken::eText:
				{
					AddText(y,desc,static_cast<TextToken*>(state.vTokens[i])->GetData());
					break;
				}
			case IToken::eFontFromTable:
				{
					int index = static_cast<FontFromTable*>(state.vTokens[i])->GetData();
					if(index >= 0 && index < (int)vFontTable.size())
						desc.font = vFontTable[index];
					break;
				}
			case IToken::eShadow:
				{
					desc.font.use_shadow = static_cast<Shadow*>(state.vTokens[i])->IsOn();
					break;
				}
			case IToken::eFontGap:
			case IToken::eFontSize:
			case IToken::eFontBold:
			case IToken::eFontItalic:
			case IToken::eFontName:
				{
					state.GetFontParams(desc.font,i);
					FontInstance* temp = GetFont(desc.font);
					if(temp)
						desc.font.font = temp->Clone();
					break;
				}

			case IToken::eTab:
				{
					int count = static_cast<Tab*>(state.vTokens[i])->GetData();
					string sTab;
					for(int i = 0 ;i < count;++i)
						sTab += ' ';
					AddText(y,desc,sTab);
					break;
				}
			case IToken::ePushColor:
				{
					vColorStack.push_back(desc.font.color);
					break;
				}
			case IToken::ePopColor:
				{
					desc.font.color = vColorStack[vColorStack.size() - 1];
					vColorStack.pop_back();
					break;
				}

			case IToken::eTextShadowAlpha:
			case IToken::eTextShadowColor:
				{
					state.GetColor(desc.font.shd_color,i);
					break;
				}
			case IToken::eTextAlpha:
			case IToken::eTextColor:
				{
					state.GetColor(desc.font.color,i);
					break;
				}
			case IToken::eScale:
				{
					desc.nScale = static_cast<Scale*>(state.vTokens[i])->GetData();
					break;
				}
			case IToken::eAlignLeft:
				{
					CalculateLinesY(y);
					vText.push_back(TextLine());
					desc.eAlign =  text_left;
					break;
				}
			case IToken::eAlignRight:
				{
					CalculateLinesY(y);
					vText.push_back(TextLine());
					desc.eAlign = text_right;
					break;
				}
			case IToken::eAlignCenter:
				{
					CalculateLinesY(y);
					vText.push_back(TextLine());
					desc.eAlign = text_center;
					break;
				}
			case IToken::eNextLine:
				{
					float nOldY =  y;
					CalculateLinesY(y);
					if(y == nOldY)
					{
						for(int i = vText.size() - 1;i >= 0;--i)
						{
							if(vText[i].nHeigth > 0)
							{
								y += vText[i].nHeigth;
								break;
							}
						}
						if(y == nOldY)
						{
							float heigth = desc.font.font->GetHeight() * desc.nScale;
							ROUND(heigth)
								y += heigth  + heigth * desc.font.gap * desc.nScale;
						}
					}
					vText.push_back(TextLine(y - nOldY));
					break;
				}
			case IToken::eLink:
				{
					static_cast<Link*>(state.vTokens[i])->GetLink(desc.sLink,desc.nLinkIndex);
					break;
				}

			}
		}
		CalculateLinesY(y);
		for(unsigned i = 0 ;i < vText.size();++i)
			for(unsigned j = 0;j < vText[i].vElements.size();++j)
			{
				TextElement* pElement =  &vText[i].vElements[j];
				if( !pElement->descriptor.sLink.empty())
					vLinks.push_back(pElement);
			}
			Parser::Clear();
	}

	void RichText::Clear()
	{
		vLinks.clear();
		vFontTable.clear();
		for(int i = 0; i < vFonts.Size();++i) 
		{
			FontInstance* pFont = vFonts.Get(i);
			if (pFont)
				pFont->Release();
		}

#ifndef OSUNIX
		vFonts.Clear(false);
#endif
	}

	void RichText::ChangeFont(const FontInstance* pOld,FontInstance* pNew)
	{
		for(unsigned i = 0;i < vText.size();++i)
			for(unsigned j = 0;j < vText[i].vElements.size();++j)
				if(vText[i].vElements[j].descriptor.font.font == pOld)
					vText[i].vElements[j].descriptor.font.font = pNew;
	}

	void RichText::Render(NGUIWidget* pOwner,int nMaxLines,float nAlpha,float nStartY)
	{
		unsigned count =  vText.size();
		bool bAddEndPoints = false;
		if(nMaxLines != -1 && nMaxLines < (int)count)
		{
			count = nMaxLines;
			bAddEndPoints = true;
		}
		for(unsigned i = 0;i < count;++i)
		{
			const TextLine& rLine = vText[i];
			int nElementsCount = rLine.vElements.size();
			if(bAddEndPoints && nElementsCount > 0)
			{
				for(unsigned j = nElementsCount - 1;j >= 0;--j)
				{
					if(rLine.vElements[j].sText.size() < 3)--nElementsCount;
					else break;
				}
			}
			for(unsigned j = 0;j < nElementsCount;++j)
				rLine.vElements[j].Render(pOwner,nAlpha,(bAddEndPoints && i + 1 == count &&  j + 1 == rLine.vElements.size()),nStartY);
		}
	}

	RichText::TextLine* RichText::NextLine(float& y)
	{
		CalculateLinesY(y);
		vText.push_back(TextLine());
		return &vText.back();
	}

	bool  RichText::IsSeparator(const char  symbol)
	{
		for(unsigned i = 0;i < FontRes::vTextSeparatorsSize;++i)
			if(symbol == FontRes::vTextSeparators[i])return true;
		return false;
	}

	void RichText::AddText(float& y,const TextDescription& descriptor,const string& text)
	{
		unsigned i = 0;
		string word,sLine;
		if(vText.empty())
			vText.push_back(TextLine());
		TextLine* pLine = &vText.back();
		bool bRealWord = false;float nLineWidth = 0;float nWordLength = 0;
		int w = 0;
		int bytes = 0;	
		for(;i < text.size();++i)
		{
			int char_w = 0;
			if (BuildUtf16fromUtf8(text[i], bytes, w))
			{
				int chr = w;		
				if (chr <= 65000)
					char_w = descriptor.font.font->GetCharGlyph(chr)->x_advance * descriptor.nScale;
			}
			if(IsSeparator(text[i]) && bRealWord)
			{
				float w = descriptor.font.font->GetWidth(word.c_str()) * descriptor.nScale;
				if(pLine->nWidth + nLineWidth + w > nMaxWidth)
				{
					if(!sLine.empty())
					{
						pLine->vElements.push_back(RichText::TextElement(descriptor,sLine));
						pLine->nWidth += pLine->vElements.back().width;
					}
					pLine = NextLine(y);
					nLineWidth =  0;
					sLine.clear();
					CutSpacesAtStart(word);
				}
				sLine += (word + text[i]);
				nLineWidth +=  w + descriptor.font.font->GetWidth(" ") * descriptor.nScale;
				word.clear();
				nWordLength = 0;
				bRealWord = false;
			}
			else
			{
				if(!IsSeparator(text[i]))bRealWord = true;
				if(nWordLength + char_w > nMaxWidth)
				{
					if(!sLine.empty())
					{
						pLine = AddLine(pLine,y,descriptor,sLine);
						sLine.clear();
					}
					pLine->vElements.push_back(RichText::TextElement(descriptor,word));
					pLine->nWidth += pLine->vElements.back().width;
					pLine = NextLine(y);
					word.clear();
					nWordLength = 0;
				}
				word += text[i];
				nWordLength += char_w;
			}

		}
		CutSpacesAtStart(word);
		if(!sLine.empty())
		{
			if(!word.empty())
			{
				float w = descriptor.font.font->GetWidth(sLine.c_str()) * descriptor.nScale;
				float w2 = descriptor.font.font->GetWidth(word.c_str()) * descriptor.nScale;
				if(pLine->nWidth + w  + w2 > nMaxWidth)
				{
					pLine = AddLine(pLine,y,descriptor,sLine);
					pLine = AddText(pLine,y,descriptor,word);
				}
				else 
				{
					sLine += word;
					pLine = AddText(pLine,y,descriptor,sLine);
				}

			}
			else pLine = AddText(pLine,y,descriptor,sLine);
		}
		else
			if(!word.empty())
				pLine = AddText(pLine,y,descriptor,word);
	}
	void    RichText::CutSpacesAtStart(string& sText)
	{
		unsigned count  = 0;
		while(sText[count] == ' ')++count;
		if(count > 0)
			sText.erase(0,count);
	}
	RichText::TextLine*     RichText::AddLine(TextLine* pLine,float& y,const TextDescription& descriptor,string& text)
	{
		TextLine* pOldLine = pLine;
		pLine = AddText(pLine,y,descriptor,text);
		if(pLine == pOldLine)
			pLine = NextLine(y);
		return pLine;
	}
	RichText::TextLine* RichText::AddText(TextLine* pLine,float& y,const TextDescription& descriptor,string& text)
	{
		float w = descriptor.font.font->GetWidth(text.c_str()) * descriptor.nScale;
		if(IsSeparator(text[text.size() - 1]))
			w -= descriptor.font.font->GetWidth(&text[text.size() - 1]) * descriptor.nScale;
		if(pLine->nWidth + w > nMaxWidth)
		{
			pLine = NextLine(y);
			CutSpacesAtStart(text);
		}
		pLine->vElements.push_back(RichText::TextElement(descriptor,text));
		pLine->nWidth += pLine->vElements.back().width;
		return  pLine;
	}

	void RichText::CalculateLinesY(float& y)
	{
		if(vText.empty())return;
		TextLine& line = vText.back();
		if(line.vElements.empty())return;
		float nMaxHeigth = 0;
		float nMaxGap = 0;
		line.nY = y;
		TextAlign    align = line.vElements[0].descriptor.eAlign;
		for(unsigned i = 0 ;i < line.vElements.size();++i)
		{
			const TextElement& element = line.vElements[i];
			float heigth = element.descriptor.font.font->GetHeight() * element.descriptor.nScale;
			ROUND(heigth)
				if(nMaxHeigth < heigth)
					nMaxHeigth = heigth;
			if(nMaxGap < heigth * element.descriptor.font.gap)
				nMaxGap = heigth * element.descriptor.font.gap;
		}
		switch(align)
		{
		case text_left:
			{
				float x = 0;
				for(unsigned i = 0 ;i < line.vElements.size();++i)
				{
					line.vElements[i].x = x;
					float  heigth = line.vElements[i].descriptor.font.font->GetHeight() * line.vElements[i].descriptor.nScale; 
					ROUND(heigth)
						if(heigth < nMaxHeigth)
							line.vElements[i].y = y + nMaxHeigth - heigth; 
						else
							line.vElements[i].y  = y;
					x += line.vElements[i].width;
				}
				break;
			}
		case text_right:
			{
				float x = nMaxWidth;
				for(int i = line.vElements.size() - 1;i >= 0;--i)
				{
					x -= line.vElements[i].width;
					line.vElements[i].x = x;
					float  heigth = line.vElements[i].descriptor.font.font->GetHeight() * line.vElements[i].descriptor.nScale; 
					ROUND(heigth)
						if(heigth < nMaxHeigth)
							line.vElements[i].y = y + nMaxHeigth - heigth; 
						else
							line.vElements[i].y  = y;
				}
				break;
			}
		case text_center:
			{
				float x = (nMaxWidth  - line.nWidth) * 0.5f;
				for(unsigned i = 0 ;i < line.vElements.size();++i)
				{
					line.vElements[i].x = x;
					float  heigth = line.vElements[i].descriptor.font.font->GetHeight() * line.vElements[i].descriptor.nScale; 
					ROUND(heigth)
						if(heigth < nMaxHeigth)
							line.vElements[i].y = y + nMaxHeigth - heigth; 
						else
							line.vElements[i].y  = y;
					x += line.vElements[i].width;
				}
				break;
			}
		}
		line.nHeigth = nMaxHeigth + nMaxGap;
		line.nGap = nMaxGap;
		ROUND(line.nGap)
			ROUND(line.nHeigth)
			y += nMaxHeigth + nMaxGap;
		ROUND(y)
	}

	void RichText::CheckPress(Vector point,string& sCallBack,int& index,string& sLink)
	{
		sCallBack.clear();
		for(unsigned i = 0; i < vLinks.size();++i)
		{
			float  heigth = vLinks[i]->descriptor.font.font->GetHeight() * vLinks[i]->descriptor.nScale; 
			ROUND(heigth)
				if(point.x >= vLinks[i]->x && point.x <= vLinks[i]->x + vLinks[i]->width && point.y >= vLinks[i]->y && point.y <= vLinks[i]->y + heigth)
				{
					sCallBack = vLinks[i]->descriptor.sLink;
					index = vLinks[i]->descriptor.nLinkIndex;
					sLink = vLinks[i]->sText;
				}
		}
	}

	float RichText::GetTextHeight(int nMaxLines)
	{
		if(!vText.empty())
		{
			int count = vText.size();
			if(nMaxLines != -1 && nMaxLines < count)count = nMaxLines;
			float h =  vText[0].nY;
			for(int i = 0; i < count;i++)
			{
				if(i + 1 == count && vText[i].vElements.empty())return h;
				h += vText[i].nHeigth;
			}
			return h - vText.back().nGap;
		}
		return 0;
	}

	float RichText::GetTextWidth()
	{
		if(!vText.empty())
		{
			float   nMaxWidth = 0;
			for(unsigned i = 0 ;i < vText.size();++i)
			{
				TextLine& line = vText[i];
				if(nMaxWidth <  line.nWidth )
					nMaxWidth = line.nWidth;
			}
			return nMaxWidth;
		}
		return 0;
	}

	void RichText::GetWordBorders(const Cursor& cursor,int& start,int& end)
	{
		start = end = 0;
		if(!vText.empty() && cursor.Line() < (int)vText.size())
		{
			TextLine& line = vText[cursor.Line()];
			std::string sBuffer;
			for(unsigned i = 0; i < line.vElements.size();++i)
				sBuffer += line.vElements[i].sText;
			TextUtils::GetWordBorders(sBuffer,cursor.Index(),start,end);
		}
	}

	bool RichText::Select(int nLine,int start, int end,Selection& selection)
	{
		bool bRet = false;
		if(!vText.empty() && nLine < (int)vText.size())
		{
			float startX = 0;
			float endX = 0;
			const RichText::TextLine& line = vText[nLine];
			int k = 0;
			int w = 0;
			int bytes = 0;	
			for(unsigned i = 0; i < line.vElements.size();++i)
			{
				const RichText::TextElement& element = line.vElements[i];
				float width = 0;
				for(unsigned j = 0;j < element.sText.size();++j,++k)
				{
					if (!BuildUtf16fromUtf8(element.sText[j], bytes, w))continue;
					int chr = w;		
					if (chr > 65000) continue;		
					FontRes::Glyph* set_glyph = element.descriptor.font.font->GetCharGlyph(chr);
					if(set_glyph)
					{
						float wdt = set_glyph->x_advance * element.descriptor.nScale;
						if(k <= start)startX = element.x + width;
						width += wdt;
						if(k < end)endX = element.x + width;
					}
				}
			}
			selection.Set(startX,endX,start,end,nLine,line.nY);
			bRet = true;
		}
		return bRet;
	}

	bool RichText::FindPosition(float x,float y,Cursor* pCursor)
	{
		bool bRet = false;
		for(unsigned i = 0; i < vText.size();++i)
		{
			if(y > vText[i].nY && y < vText[i].nY + vText[i].nHeigth)
			{
				float cursor_pos_x = 0;
				int cursor_pos = 0;
				for(unsigned j = 0; j < vText[i].vElements.size();++j)
				{
					const RichText::TextElement& element = vText[i].vElements[j];
					bool xInside =  false;
					if(j + 1 < vText[i].vElements.size())xInside = x > element.x && x <= element.x + element.width;
					else
						xInside = x > element.x;
					if(xInside)
					{
						cursor_pos_x = element.x;
						cursor_pos += TextUtils::GetCharPositionInLine(element.descriptor.font.font,element.sText,cursor_pos_x, 0,element.sText.size(),x,element.descriptor.nScale);
						pCursor->SetPosition(cursor_pos_x,vText[i].nY,cursor_pos,i,true);
						bRet = true;
					}
					else cursor_pos += element.sText.length();
				}
			}
		}
		return bRet;
	}

	void RichText::RenderCursor(Selection& selection)
	{
		RenderCursor(selection.cursors[Selection::eSelectionCursorStart]);
		RenderCursor(selection.cursors[Selection::eSelectionCursorEnd]);
	}

	void RichText::RenderCursor(Cursor& cursor)
	{
		if(!vText.empty() && cursor.Line() < (int)vText.size())
		{
			const RichText::TextLine& line = vText[cursor.Line()];
			int k = 0;
			for(unsigned j = 0; j < line.vElements.size();++j)
			{
				const RichText::TextElement& element = line.vElements[j];
				k += element.sText.size();
				if(cursor.Index() <= k )
				{
					cursor.SetY(element.y);
					cursor.Render(element.descriptor.font.font,element.descriptor.nScale,0,0,false);
					break;                
				}
			}
		}
	}

	void RichText::CopyText(Selection& selection)
	{
		int start = selection.cursors[TextProcessing::Selection::eSelectionCursorStart].Index();
		int end = selection.cursors[TextProcessing::Selection::eSelectionCursorEnd].Index();
		string txt;
		const RichText::TextLine& first_line = vText[selection.cursors[TextProcessing::Selection::eSelectionCursorStart].Line()];
		if(selection.cursors[TextProcessing::Selection::eSelectionCursorEnd].Line() - selection.cursors[TextProcessing::Selection::eSelectionCursorStart].Line()  == 0)
		{
			int k = 0;
			for(unsigned i = 0;i < first_line.vElements.size();++i)
			{
				for(unsigned j = 0;j < first_line.vElements[i].sText.size();++j,++k)
					if(k >= start && k < end)txt += first_line.vElements[i].sText[j];
			}
		}
		else
		{
			int k = 0;
			for(unsigned i = 0;i < first_line.vElements.size();++i)
			{
				for(unsigned j = 0;j < first_line.vElements[i].sText.size();++j,++k)
					if(k >= start)txt += first_line.vElements[i].sText[j];
			}
			for(int i = selection.cursors[TextProcessing::Selection::eSelectionCursorStart].Line() + 1; i < selection.cursors[TextProcessing::Selection::eSelectionCursorEnd].Line();++i)
			{
				const RichText::TextLine& line = vText[i];
				for(unsigned j = 0;j < line.vElements.size();++j)
					for(unsigned l = 0;l < line.vElements[j].sText.size();++l)
						txt += line.vElements[j].sText[l];

			}
			k = 0;
			const RichText::TextLine& last_line = vText[selection.cursors[TextProcessing::Selection::eSelectionCursorEnd].Line()];
			for(unsigned i = 0;i < last_line.vElements.size();++i)
			{
				for(unsigned j = 0;j < last_line.vElements[i].sText.size();++j,++k)
					if(k < end)txt += last_line.vElements[i].sText[j];
			}
		}
		core->Mobile()->SetTextToClipboard(txt.c_str());
	}
	void RichText::TextElement::Render(NGUIWidget* pOwner,const string& sText,float nAlpha,float nStartY) const
	{
		Color rTextColor = descriptor.font.color;
		rTextColor.a *= nAlpha;
		if (descriptor.font.use_shadow)
		{
			Color rShadowTextColor = descriptor.font.shd_color;
			rShadowTextColor.a *= nAlpha;
			pOwner->AddPrintCall(descriptor.font.font, x + 1,nStartY + y + 1, pOwner->scr_w, pOwner->scr_h, &line_indices, descriptor.nScale,rShadowTextColor, sText.c_str());
			pOwner->AddPrintCall(descriptor.font.font, x,nStartY + y, pOwner->scr_w, pOwner->scr_h, &line_indices, descriptor.nScale,rTextColor, sText.c_str());		
		}
		else
			pOwner->AddPrintCall(descriptor.font.font, x,nStartY + y, pOwner->scr_w, pOwner->scr_h, &line_indices, descriptor.nScale,rTextColor, sText.c_str());
	}
	void RichText::TextElement::Render(NGUIWidget* pOwner,float nAlpha,bool bAddEndPoints,float nStartY) const
	{
		if(sText.empty())return;
		if(bAddEndPoints)
		{
			string sBuffer = sText;
			int i = sText.size() - 1;
			while(RichText::IsSeparator(sText[i]))--i;
			for(;i >= 0;--i)
				if(RichText::IsSeparator(sText[i]))break;
			if(i < 0)
				i = sText.size() - 3;
			sBuffer.erase(i,sBuffer.size() - i);
			sBuffer += "...";
			Render(pOwner,sBuffer,nAlpha,nStartY);
		}
		else Render(pOwner,sText,nAlpha,nStartY);
	}

	RichText::TextElement::TextElement(const TextDescription& desc,const string& text) :descriptor(desc),x(0),y(0),sText(text)
	{
		width = descriptor.font.font->GetWidth(sText.c_str())  * descriptor.nScale;
	}

	int TextProcessing::TextUtils::GetCharPositionInLine(FontInstance* pFont,const string& text,float& pos_x,int start_pos,int end_pos,float cur_pos_x,float scale)
	{
		if (text.size() == 0) return start_pos;
		if(end_pos > (int)text.size())end_pos = text.size();
		float cur_x = pos_x;
		if (cur_pos_x < cur_x)
		{
			pos_x = cur_x;
			return start_pos;
		}	
		int w = 0;
		int bytes = 0;	
		for (int i = start_pos; i < end_pos;++i)
		{				
			if (!BuildUtf16fromUtf8(text[i], bytes, w))continue;
			int chr = w;		
			if (chr > 65000) continue;	
			if(chr == 10)continue;
			else
				if (chr == '\\' && i < end_pos - 1 && text[i + 1] == 'n')
				{
					i += 2;
					continue;
				}
				FontRes::Glyph* set_glyph = pFont->GetCharGlyph(chr);
				if(set_glyph)
				{
					float wdt = set_glyph->x_advance * scale;
					if (cur_x < cur_pos_x && cur_pos_x <= cur_x + wdt)
					{			
						pos_x = cur_x;
						return i;
					}
					cur_x += wdt;		
				}
		}	
		pos_x = cur_x;
		return end_pos;
	}

	float TextUtils::GetTextWidthFrom(FontInstance* pFont,const string& text,int start_pos,int index,bool bMultiLine,float scale)
	{
		int len = (int)text.size();
		if (len == 0) return 0;
		float cur_x = 0;
		float max_x = 0;
		int w = 0;
		int bytes = 0;	
		for (int i = start_pos; i < index; ++i)
		{				
			if (!BuildUtf16fromUtf8(text[i], bytes, w))continue;
			int chr = w;		
			if (chr > 65000) continue;		
			if (chr == '\\')
			{
				if (i < len-1)
				{				
					if (text[i+1] == 'n')
					{
						if(bMultiLine)
						{
							if(cur_x > max_x)max_x = cur_x;
							cur_x = 0;
						}
						i += 2;
						continue;
					}									
				}			
			}
			FontRes::Glyph* set_glyph = pFont->GetCharGlyph(chr);
			if(set_glyph)
				cur_x += set_glyph->x_advance * scale;	
		}	
		if(cur_x > max_x)max_x = cur_x;
		return max_x;
	}

	void TextUtils::GetWordBorders(FontInstance* pFont,TLinesData* line_indexes,const string& text,const Cursor& cursor,int& start,int& end,float minX,float maxX,float scale)
	{
		if (text.empty() || cursor.Index() > (int)text.size())
		{
			start = end = 0;
			return;
		}
		int w = 0;
		int bytes = 0;	
		float cur_x = cursor.GetX();
		for (unsigned i = cursor.Index(); i < text.size(); ++i)
		{
			if (!BuildUtf16fromUtf8(text[i], bytes, w))continue;
			int chr = w;		
			if (chr > 65000) continue;		
			if(chr == 10)continue;
			else
				if (chr == '\\' && i < text.size() - 1 && text[i + 1] == 'n')
				{
					i += 2;
					continue;
				}
				if(chr == ' ')
				{
					end = i;
					break;
				}
				else
				{
					FontRes::Glyph* set_glyph = pFont->GetCharGlyph(chr);
					if(set_glyph)
					{
						float wdt = set_glyph->x_advance * scale;
						if(cur_x + wdt > maxX)
						{
							end = i;
							break;
						}
						else
							if(i + 1 >= text.size())
								end = i + 1;
						cur_x += wdt;
					}
				}
		}
		cur_x = cursor.GetX();
		for (int i = cursor.Index(); i >= 0; --i)
		{
			if (!BuildUtf16fromUtf8(text[i], bytes, w))continue;
			int chr = w;		
			if (chr > 65000) continue;	
			if(chr == 10)continue;
			else
				if (chr == 'n' && i < (int)text.size() - 1 && text[i + 1] == '\\')
				{
					i += 2;
					continue;
				}
				if(chr == ' ')
				{
					start = i + 1;
					return;
				}
				else
				{
					FontRes::Glyph* set_glyph = pFont->GetCharGlyph(chr);
					if(set_glyph)
					{
						float wdt = set_glyph->x_advance * scale;
						if(cur_x - wdt  <  minX - set_glyph->x_advance * 0.5f)
						{
							if(line_indexes)
							{
								if(line_indexes->size() <= 1 || cursor.Line() == 0)start = 0;
								else start = line_indexes->operator[](cursor.Line() - 1).index;
							}
							else
								start = i + 1;
							return;
						}
						cur_x -= wdt;
					}
				}
		}
		if(line_indexes)
		{
			if(line_indexes->size() <= 1 || cursor.Line() == 0)start = 0;
			else start = line_indexes->operator[](cursor.Line() - 1).index;
		}
		else 
			start = 0;
	}

	void TextUtils::GetWordBorders(const string& text,int index,int& start,int& end)
	{
		if (text.empty() || index > (int)text.size())
		{
			start = end = 0;
			return;
		}
		int w = 0;
		int bytes = 0;	
		for (unsigned i = index; i < text.size(); ++i)
		{
			if (!BuildUtf16fromUtf8(text[i], bytes, w))continue;
			int chr = w;		
			if (chr > 65000) continue;		
			if(chr == 10)continue;
			else
				if (chr == '\\' && i < text.size() - 1 && text[i + 1] == 'n')
				{
					i += 2;
					continue;
				}
				if(chr == ' ')
				{
					end = i;
					break;
				}
				else
				{
					if(i + 1 >= text.size())
						end = i + 1;
				}
		}
		for (int i = index; i >= 0; --i)
		{
			if (!BuildUtf16fromUtf8(text[i], bytes, w))continue;
			int chr = w;		
			if (chr > 65000) continue;	
			if(chr == 10)continue;
			else
				if (chr == 'n' && i < (int)text.size() - 1 && text[i + 1] == '\\')
				{
					i += 2;
					continue;
				}
				if(chr == ' ')
				{
					start = i + 1;
					return;
				}
		}
		start = 0;
	}


	const unsigned RichText::nDomainsCount = 21;
	const string RichText::vDomains[] = 
	{
		"com","net","org","aero","asia","biz","cat","coop","edu","gov","info","int","jobs","mil","mobi","museum","name","pro","tel","travel","xxx"
	};

	const RichText::AlphaDomains RichText::vAlphaDomains[AlphaDomainsCount] = 
	{
		{'a',{'c','d','e','f','g','i','l','m','n','o','q','r','s','t','u','w','z',0,0,0,0,0}},
		{'b',{'a','b','d','e','f','g','h','i','j','m','n','o','r','s','t','v','z',0,0,0,0,0}},
		{'c',{'a','c','d','f','g','h','i','k','l','m','n','o','r','s','u','v','x','y','z',0,0,0}},
		{'d',{'d','e','j','k','m','o',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
		{'e',{'c','e','g','h','r','s','t','u',0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
		{'f',{'i','j','k','m','o','r',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
		{'g',{'a','b','d','e','f','g','h','l','m','n','p','q','r','s','t','u','y',0,0,0,0,0}},
		{'h',{'k','m','n','r','t','u',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
		{'i',{'d','e','l','m','n','o','q','r','s','t',0,0,0,0,0,0,0,0,0,0,0,0}},
		{'j',{'e','m','o','p',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
		{'k',{'e','g','h','i','m','n','p','r','y','z',0,0,0,0,0,0,0,0,0,0,0,0}},
		{'l',{'a','b','c','i','k','r','s','t','u','v','y',0,0,0,0,0,0,0,0,0,0,0}},
		{'m',{'a','c','d','e','g','h','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'}},
		{'n',{'a','c','e','f','g','i','l','o','p','r','u','z',0,0,0,0,0,0,0,0,0,0}},
		{'o',{'m',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
		{'p',{'a','e','f','g','h','k','l','m','n','r','s','y','w',0,0,0,0,0,0,0,0,0}},
		{'q',{'a',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
		{'r',{'u','e','o','s','w',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
		{'s',{'a','b','c','d','e','g','h','j','k','l','m','n','n','o','r','t','u','v','y','z',0,0}},
		{'t',{'c','d','f','g','h','j','k','l','m','n','o','p','r','t','v','w','z',0,0,0,0,0}},
		{'u',{'a','g','k','s','y','z',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
		{'v',{'a','c','e','g','i','n','u',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
		{'w',{'f','s',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
		{'y',{'e','t',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
		{'z',{'a','m','w',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
	};
	static bool bInited = false;
	float Cursor::nWidth = 22;
	float Cursor::nHeigth = 22;
	TextureInstance* Cursor::pTexture = NULL;
	TextureInstance* Cursor::pCarretTexture = NULL;
	TextureInstance* Selection::pTexture = NULL;
	TextureEntry::Slice Menu::slice;
	TextureInstance* Menu::pLineTexture = NULL;
	FontInstance*    Menu::pMenuFont = NULL;
	TextureInstance* Menu::pMenuAnchorTexture = NULL;
	TextureInstance* Menu::pMenuArrowRTexture = NULL;
	TextureInstance* Menu::pMenuArrowLTexture = NULL;
	TextProcessing::Menu::TMenuNames TextProcessing::Menu::vLabelMenuText;
	TextProcessing::Menu::TMenuNames TextProcessing::Menu::vEditMenuText;

#define MENU_SIZE 7
	static const char* menu_str_src [] = {"#Copy", "#Cut", "#Replace", "#Select", "#Paste", "", "", ""};

	void StaticData::Release()
	{
		RELEASE(Menu::slice.tex.texture)
			RELEASE(Menu::pMenuAnchorTexture)
			RELEASE(Menu::pMenuArrowRTexture)
			RELEASE(Menu::pMenuArrowLTexture)
			RELEASE(Menu::pLineTexture)
			RELEASE(Menu::pMenuFont)   

			RELEASE(Selection::pTexture)
			RELEASE(Cursor::pTexture)
			RELEASE(Cursor::pCarretTexture)

			Menu::Release();
	}

	void StaticData::Init()
	{
		if(!bInited)
		{
			bInited = true;

			Menu::vLabelMenuText.resize(1);
			Menu::vLabelMenuText[0] = "Copy";

			Menu::vEditMenuText.resize(MENU_SIZE);
			for (int i=0;i<MENU_SIZE;i++)
			{
				Menu::vEditMenuText[i].clear();
				if(strlen(menu_str_src[i]) > 0)
					Menu::vEditMenuText[i] = menu_str_src[i];
			}

			TextureEntry::Slice& slice = Menu::slice;

			slice.tex.texture = en_core.Render()->LoadTexture("Editor/EdiboxMenuBack.tga",false, _FL_);

			if (!slice.tex.texture)
			{
				slice.width  = 0;
				slice.height = 0;
			}
			else
			{
				slice.width  = slice.tex.texture->GetWidth();
				slice.height = slice.tex.texture->GetHeight();
			}

			slice.type = 3;

			slice.rects[0].x = 0;
			slice.rects[0].y = 50;
			slice.rects[0].width = 25;
			slice.rects[0].height = 23;
			slice.rects[0].u = 0.0f;
			slice.rects[0].v = 0.0f;
			slice.rects[0].du = 0.5f;
			slice.rects[0].dv = 0.46f;

			slice.rects[1].x = 25;
			slice.rects[1].y = 50;
			slice.rects[1].width = 3;
			slice.rects[1].height = 23;
			slice.rects[1].u = 0.5f;
			slice.rects[1].v = 0.0f;
			slice.rects[1].du = 0.06f;
			slice.rects[1].dv = 0.46f;

			slice.rects[2].x = 28;
			slice.rects[2].y = 50;
			slice.rects[2].width = 22;
			slice.rects[2].height = 23;
			slice.rects[2].u = 0.56f;
			slice.rects[2].v = 0.0f;
			slice.rects[2].du = 0.44f;
			slice.rects[2].dv = 0.46f;

			slice.rects[3].x = 0;
			slice.rects[3].y = 27;
			slice.rects[3].width = 25;
			slice.rects[3].height = 3;
			slice.rects[3].u = 0.0f;
			slice.rects[3].v = 0.46f;
			slice.rects[3].du = 0.5f;
			slice.rects[3].dv = 0.06f;


			slice.rects[4].x = 25;
			slice.rects[4].y = 27;
			slice.rects[4].width = 3;
			slice.rects[4].height = 3;
			slice.rects[4].u = 0.5f;
			slice.rects[4].v = 0.46f;
			slice.rects[4].du = 0.06f;
			slice.rects[4].dv = 0.06f;

			slice.rects[5].x = 28;
			slice.rects[5].y = 27;
			slice.rects[5].width = 22;
			slice.rects[5].height = 3;
			slice.rects[5].u = 0.56f;
			slice.rects[5].v = 0.46f;
			slice.rects[5].du = 0.44f;
			slice.rects[5].dv = 0.06f;

			slice.rects[6].x = 0;
			slice.rects[6].y = 24;
			slice.rects[6].width = 25;
			slice.rects[6].height = 24;
			slice.rects[6].u = 0.0f;
			slice.rects[6].v = 0.52f;
			slice.rects[6].du = 0.5f;
			slice.rects[6].dv = 0.48f;

			slice.rects[7].x = 25;
			slice.rects[7].y = 24;
			slice.rects[7].width = 3;
			slice.rects[7].height = 24;
			slice.rects[7].u = 0.5f;
			slice.rects[7].v = 0.52f;
			slice.rects[7].du = 0.06f;
			slice.rects[7].dv = 0.48f;

			slice.rects[8].x = 28;
			slice.rects[8].y = 24;
			slice.rects[8].width = 22;
			slice.rects[8].height = 24;
			slice.rects[8].u = 0.56f;
			slice.rects[8].v = 0.52f;
			slice.rects[8].du = 0.44f;
			slice.rects[8].dv = 0.48f;				

			slice.scale_type = 1;
			slice.hack_scale = 1;

			Cursor::pTexture = en_core.Render()->LoadTexture("Editor/curret.png",false, _FL_);
			Cursor::pCarretTexture = en_core.Render()->LoadTexture("Editor/white.png", false, _FL_);
			Selection::pTexture = en_core.Render()->LoadTexture("Editor/white.png", false, _FL_);


			Menu::pLineTexture = en_core.Render()->LoadTexture("Editor/white.png",false, _FL_);
			Menu::pMenuAnchorTexture = en_core.Render()->LoadTexture("Editor/editboxmenuanchor.tga", false, _FL_);
			Menu::pMenuArrowRTexture = en_core.Render()->LoadTexture("Editor/editboxmenuarrow.tga", false, _FL_);
			Menu::pMenuArrowLTexture = en_core.Render()->LoadTexture("Editor/editboxmenuarrowM.tga", false, _FL_);
			Menu::CreateFont();
		}
	}

}

