
#include "NGUICalendar.h"
#include "Common/Utils.h"

ENTITY(NGUICalendar)
ENTITY_IS_2D_OBJECT
ENTITY_PARAMS(NGUICalendar,"NGUICalendar","NGUI", "Calendar")

void NGUICalendar::RegisterProperties()
{
	NGUIWidget::RegisterProperties();				

	PROP_REG( allow_past )		
	PROP_REG( auto_slect_cur_date )
	PROP_REG( allow_scroll )

	PROP_REG( sel_theme )	

	PROP_REG( show_week_days )		

	PROP_REG( topFontDesc )	
	PROP_REG( captionFontDesc )			
	PROP_REG( cellFontDesc )	

	PROP_REG( fontCellColor )		
	PROP_REG( fontCellSelColor )			
	PROP_REG( fontCellPrevColor )				

	PROP_REG( topSize )	
	PROP_REG( captionSize )		
	PROP_REG( cellSize )	
	PROP_REG( monthGap )	
	PROP_REG( nextBtnSize )		
	PROP_REG( markerSize )		

	PROP_REG( topTexName )	
	PROP_REG( topTexColor )	
	PROP_REG( lineTexName )
	PROP_REG( lineTexColor )
	PROP_REG( cellTexName )
	PROP_REG( cellTexColor )
	PROP_REG( cellSelTexName )		
	PROP_REG( cellSelTexColor )
	PROP_REG( cellMarkedTexName )	
	PROP_REG( cellMarkedTexColor )
	PROP_REG( cellMarkedPrevTexName )			
	PROP_REG( cellMarkedPrevTexColor )	

	PROP_REG( funcDaySelect )
}

void NGUICalendar::Init()
{	
	NGUIWidget::Init();		

	SetUpdate((TaskHolder::Task)&NGUICalendar::CheckPress, lvl_pre5 + 3);

	topTex = null;		
	lineTex = null;
	cellTex = null;
	cellSelTex = null;
	cellMarkedTex = null;
	cellMarkedPrevTex = null;

	sel_day = -1;
	sel_month = -1;
	sel_year = -1;

	is_draged = false;
	touchType = -1;
	in_scroll = false;
	cur_page = 0;
	target_page = 0;
	ms_pos = 0;

	childs_offset = 0.0f;	

	const char* month_str_src [] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

	for (int i=0;i<12;i++)
	{
		String::Copy(month_str[i].txt, 64, month_str_src[i]);
	}
	
	const char* day_str_src [] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };				

	for (int i=0;i<7;i++)
	{
		String::Copy(day_str[i].txt, 64, day_str_src[i]);
	}

	day = GetTime(3);
	month = GetTime(4);
	year = GetTime(5);	

	sub_widget = new NGUIWidget(Desc(), Scene());
	
	sub_widget->Init();
	sub_widget->clipChilds = true;
	sub_widget->parent = this;
	sub_widget->anchor_x = 0.0f;
	sub_widget->anchor_y = 0.0f;
	sub_widget->horzAlign = 0;
	sub_widget->vertAlign = 3;
	sub_widget->text_align = 1;
	sub_widget->vtext_align = 1;
	sub_drawers.push_back(sub_widget);

	scroll_action = 0;
}
	
void NGUICalendar::LoadTexture(TextureInstance* &tex, const char* name)
{
	RELEASE(tex)
	tex = en_core.Render()->LoadTexture(name, false, _FL_);

	if (tex) tex->SetAdress(Texture::addressClamp);
}

void NGUICalendar::ReInit()
{
	NGUIWidget::ReInit();			
	
	LoadTexture(topTex, topTexName.c_str());	
	LoadTexture(lineTex, lineTexName.c_str());
	LoadTexture(cellTex, cellTexName.c_str());
	LoadTexture(cellSelTex, cellSelTexName.c_str());
	LoadTexture(cellMarkedTex, cellMarkedTexName.c_str());
	LoadTexture(cellMarkedPrevTex, cellMarkedPrevTexName.c_str());
		
	text_align = 1;
	vtext_align = 1;

	CacheDays();	

	clipChilds = true;	

	RELEASE(topFontDesc.font)
	topFontDesc.font = en_core.Font()->CreateFont(topFontDesc.name, topFontDesc.bold, topFontDesc.italic, ConvertVert(topFontDesc.height), topFontDesc.gap, topFontDesc.outline);			

	RELEASE(captionFontDesc.font)
	captionFontDesc.font = en_core.Font()->CreateFont(captionFontDesc.name, captionFontDesc.bold, captionFontDesc.italic, ConvertVert(captionFontDesc.height), captionFontDesc.gap, captionFontDesc.outline);			

	RELEASE(cellFontDesc.font)
	cellFontDesc.font = en_core.Font()->CreateFont(cellFontDesc.name, cellFontDesc.bold, cellFontDesc.italic, ConvertVert(cellFontDesc.height), cellFontDesc.gap, cellFontDesc.outline);			

#ifdef EDITOR
	
	//SET_SKIP(fontEntityName, !fontUseEntity )	
	//SET_SKIP(fontDesc, fontUseEntity )			
	
#endif	

	cur_day = day;
	cur_month = month;
	cur_year = year;

	if (auto_slect_cur_date)
	{
		sel_day = day;
		sel_month = month;
		sel_year = year;
	}
	else
	{
		sel_day = -1;
		sel_month = -1;
		sel_year = -1;
	}

}

bool NGUICalendar::Start()
{
	cur_day = day;
	cur_month = month;
	cur_year = year;

	if (auto_slect_cur_date)
	{
		sel_day = day;
		sel_month = month;
		sel_year = year;
	}
	else
	{
		sel_day = -1;
		sel_month = -1;
		sel_year = -1;
	}

	ScriptFunction::Find1P(&callback, Scene()->ScriptModule(), funcDaySelect.c_str(), Desc()->GetName());

	return NGUIWidget::Start();
}

void NGUICalendar::Finish()
{
	callback.Reset();
}

void NGUICalendar::RenderWidget()
{
	if (sel_theme == 0)
	{
		RenderMonth(-scr_w, &months[0]);
		RenderMonth(0, &months[1]);
		RenderMonth(scr_w, &months[2]);
	}
	else
	{
		sub_widget->x = 0;
		sub_widget->y = topSize;
		sub_widget->width = width;
		sub_widget->height = height - topSize;

		sub_widget->UpdateChildsPos();

		float sz = ConvertVert(topSize);
		float sz2 = ConvertVert(captionSize);
		float cell_sx = scr_w * 0.1428f;
					
		if (topTex)
		{
			Color c = clr;
			c *= topTexColor;
			AddDrawCall(topTex, 0, 0, scr_w, sz, c, 0.0f, 0.0f, 1.0f, 1.0f);
		}

		if (topFontDesc.font)
		{				
			float cur_x = childs_offset;

			for (int j=0;j<7;j++)
			{
				string txt = day_str[j].txt;				

				ClampUtf8(txt, 1);

				AddPrintCall(topFontDesc.font, cur_x, 0, cell_sx, sz, null, 1.0, topFontDesc.color, txt.c_str());
				cur_x += cell_sx;
			}
		}
		
		for (int i=0; i<12; i++)
		{
			if ( sz < ConvertVert(months[i].offset + months[i].num_lines * cellSize + scroller.offset) + sz + sz2 &&
				 ConvertVert(months[i].offset + scroller.offset) + sz < scr_h)
			{
				RenderMonth2(&months[i]);
			}
		}		
	}
}

void NGUICalendar::RenderMonth(float offset, MonthChache* cache)
{
	float sz = ConvertVert(topSize);
	float sz2 = ConvertVert(captionSize);

	if (!show_week_days)
	{
		sz2 = 0;
	}

	float sz_btn = ConvertHorz(nextBtnSize);
	float sz_marker = ConvertHorz(markerSize);	
	
	if (topTex)
	{
		Color c = clr;
		c *= topTexColor;
		AddDrawCall(topTex, 0, 0, scr_w, sz, c, 0.0f, 0.0f, 1.0f, 1.0f);
	}

	float cell_sx = scr_w * 0.1428f;	
	float cell_sy = (scr_h - sz - sz2) * 0.16666f;
	
	float cur_x = 0;
	float cur_y = sz + sz2;
	
	Color cell_clr = cellTexColor;
	cell_clr *= clr;
	cell_clr.a = clr.a;

	Color cell_sel_clr = cellSelTexColor;
	cell_sel_clr *= clr;
	cell_sel_clr.a = clr.a;

	Color cell_marked_clr = cellMarkedTexColor;
	cell_marked_clr *= clr;
	cell_marked_clr.a = clr.a;

	Color cell_marked_prev_clr = cellMarkedPrevTexColor;
	cell_marked_prev_clr *= clr;
	cell_marked_prev_clr.a = clr.a;	

	for (int i=0;i<6;i++)
	{
		cur_x = 0 + childs_offset + offset;		

		for (int j=0;j<7;j++)
		{
			CacheDay& cday = cache->days[i * 7 + j];

			if (cday.day == sel_day && 
				cday.month == sel_month && 
				cday.year == sel_year)
			{				
				if (cellSelTex)
				{				
					float sz = min(cell_sx,cell_sy);
					sub_widget->AddDrawCall(cellSelTex, cur_x + (cell_sx - sz) * 0.5f, cur_y + (cell_sy - sz) * 0.5f, sz, sz, cell_sel_clr, 0.0f, 0.0f, 1.0f, 1.0f);
				}
			}			
			else
			{
				if (cellTex) AddDrawCall(cellTex, cur_x, cur_y, cell_sx, cell_sy, cell_clr, 0.0f, 0.0f, 1.0f, 1.0f);				
			}

			if (cday.marked)
			{
				if (cday.month == month)
				{
					if (cellMarkedTex) AddDrawCall(cellMarkedTex, cur_x + (cell_sx - sz_marker) * 0.5f, cur_y, sz_marker, cell_sy, cell_marked_clr, 0.0f, 0.0f, 1.0f, 1.0f);
				}
				else
				{
					if (cellMarkedPrevTex) AddDrawCall(cellMarkedPrevTex, cur_x + (cell_sx - sz_marker) * 0.5f, cur_y, sz_marker, cell_sy, cell_marked_prev_clr, 0.0f, 0.0f, 1.0f, 1.0f);
				}

			}			

			cur_x += cell_sx;
		}

		cur_y += cell_sy;
	}

	if (topFontDesc.font)
	{				
		char str[128];
		String::Printf(str, 128, "%s %i", month_str[cache->days[3 * 7 + 0].month - 1].txt, year);		
		
		AddPrintCall(topFontDesc.font, childs_offset + offset, 0, scr_w, sz, null, 1.0, topFontDesc.color, str);
	}

	if (show_week_days && captionFontDesc.font)
	{				
		cur_x = childs_offset + offset;

		for (int j=0;j<7;j++)
		{
			AddPrintCall(captionFontDesc.font, cur_x, sz, cell_sx, sz2, null, 1.0, captionFontDesc.color, day_str[j].txt);
			cur_x += cell_sx;
		}
	}

	if (cellFontDesc.font)
	{		
		cur_y = sz + sz2;

		for (int i=0;i<6;i++)
		{
			cur_x = childs_offset + offset;

			for (int j=0;j<7;j++)
			{
				CacheDay& cday = cache->days[i * 7 + j];

				if (cday.month == cache->days[3 * 7 + 0].month)
				{
					if (cday.day == sel_day && 
						cday.month == sel_month &&
						cday.year == sel_year)
					{
						AddPrintCall(cellFontDesc.font, cur_x, cur_y, cell_sx, cell_sy, null, 1.0, fontCellSelColor, cday.txt);
					}
					else
					{
						if (((cday.month == cur_month && cday.year == cur_year && cday.day < day) || 
							 (cday.month < cur_month && cday.year == cur_year) ||
							 (cday.year < cur_year)) && !allow_past || cday.forbided)
						{
							AddPrintCall(cellFontDesc.font, cur_x, cur_y, cell_sx, cell_sy, null, 1.0, fontCellPrevColor, cday.txt);
						}
						else
						{
							AddPrintCall(cellFontDesc.font, cur_x, cur_y, cell_sx, cell_sy, null, 1.0, fontCellColor, cday.txt);
						}
					}
				}
				else
				{
					//AddPrintCall(cellFontDesc.font, cur_x, cur_y, cell_sx, cell_sy, null, 1.0, fontCellPrevColor, cday.txt);
				}								

				cur_x += cell_sx;
			}

			cur_y += cell_sy;
		}
	}
}

void NGUICalendar::RenderMonth2(MonthChache* cache)
{
	float sz = ConvertVert(topSize) * 0;	
	float sz2 = ConvertVert(captionSize);
	float sz_btn = ConvertHorz(nextBtnSize);
	float sz_marker = ConvertHorz(markerSize);		

	float cell_sx = scr_w * 0.1428f;
	float cell_sy = ConvertVert(cellSize);	
	
	float cur_x = 0;
	float cur_y = sz + sz2 + ConvertVert(scroller.offset + cache->offset);

	if (captionFontDesc.font)
	{				
		char str[128];
		String::Printf(str, 128, "%s %i", month_str[cache->days[3 * 7 + 0].month - 1].txt, cache->days[3 * 7 + 0].year);		
		
		sub_widget->AddPrintCall(captionFontDesc.font, childs_offset, sz + ConvertVert(scroller.offset + cache->offset), scr_w, sz2, null, 1.0, captionFontDesc.color, str);
	}

	Color line_clr = lineTexColor;
	line_clr *= clr;
	line_clr.a = clr.a;

	Color cell_clr = cellTexColor;
	cell_clr *= clr;
	cell_clr.a = clr.a;

	Color cell_sel_clr = cellSelTexColor;
	cell_sel_clr *= clr;
	cell_sel_clr.a = clr.a;

	Color cell_marked_clr = cellMarkedTexColor;
	cell_marked_clr *= clr;
	cell_marked_clr.a = clr.a;

	Color cell_marked_prev_clr = cellMarkedPrevTexColor;
	cell_marked_prev_clr *= clr;
	cell_marked_prev_clr.a = clr.a;		

	for (int i=0;i<6;i++)
	{
		cur_x = 0;

		if (lineTex)
		{
			if (i==0 || cache->days[i * 7].month == cache->days[3 * 7 + 0].month)
			{
				sub_widget->AddDrawCall(lineTex, cur_x, cur_y, scr_w, Convert2AbsVert(lineTex->GetHeight()), line_clr, 0.0f, 0.0f, 1.0f, 1.0f);
			}
		}

		for (int j=0;j<7;j++)
		{
			CacheDay& cday = cache->days[i * 7 + j];

			if (cday.month != cache->days[3 * 7 + 0].month)
			{
				cur_x += cell_sx;
				continue;
			}

			if (cday.day == sel_day && 
				cday.month == sel_month && 
				cday.year == sel_year)
			{
				if (cellSelTex)
				{
					float sz = min(cell_sx,cell_sy);
					sub_widget->AddDrawCall(cellSelTex, cur_x + (cell_sx - sz) * 0.5f, cur_y + (cell_sy - sz) * 0.5f, sz, sz, cell_sel_clr, 0.0f, 0.0f, 1.0f, 1.0f);
				}
			}			
			else
			{
				if (cellTex) sub_widget->AddDrawCall(cellTex, cur_x, cur_y, cell_sx, cell_sy, cell_clr, 0.0f, 0.0f, 1.0f, 1.0f);				
			}

			if (cday.marked)
			{
				if (cday.month == month)
				{
					if (cellMarkedTex) sub_widget->AddDrawCall(cellMarkedTex, cur_x + (cell_sx - sz_marker) * 0.5f, cur_y, sz_marker, cell_sy, cell_marked_clr, 0.0f, 0.0f, 1.0f, 1.0f);
				}
				else
				{
					if (cellMarkedPrevTex) sub_widget->AddDrawCall(cellMarkedPrevTex, cur_x + (cell_sx - sz_marker) * 0.5f, cur_y, sz_marker, cell_sy, cell_marked_prev_clr, 0.0f, 0.0f, 1.0f, 1.0f);
				}

			}			

			cur_x += cell_sx;
		}

		cur_y += cell_sy;
	}	

	if (cellFontDesc.font)
	{		
		cur_y = sz + sz2 + ConvertVert(scroller.offset + cache->offset);

		for (int i=0;i<6;i++)
		{
			cur_x = 0;

			for (int j=0;j<7;j++)
			{
				CacheDay& cday = cache->days[i * 7 + j];

				if (cday.month == cache->days[3 * 7 + 0].month)
				{
					if (cday.month == month && cday.year == year)
					{
						if (cday.day < day)
						{
							sub_widget->AddPrintCall(cellFontDesc.font, cur_x, cur_y, cell_sx, cell_sy, null, 1.0, fontCellPrevColor, cday.txt);
						}
						else
						{
							if (cday.day == sel_day && 
								cday.month == sel_month &&
								cday.year == sel_year)
							{
								sub_widget->AddPrintCall(cellFontDesc.font, cur_x, cur_y, cell_sx, cell_sy, null, 1.0, fontCellSelColor, cday.txt);
							}
							else
							{	
								if (cday.forbided)
								{
									sub_widget->AddPrintCall(cellFontDesc.font, cur_x, cur_y, cell_sx, cell_sy, null, 1.0, fontCellPrevColor, cday.txt);
								}
								else
								{
									sub_widget->AddPrintCall(cellFontDesc.font, cur_x, cur_y, cell_sx, cell_sy, null, 1.0, fontCellColor, cday.txt);
								}
							}
						}
					}
					else
					{
						if (cday.forbided)
						{
							sub_widget->AddPrintCall(cellFontDesc.font, cur_x, cur_y, cell_sx, cell_sy, null, 1.0, fontCellPrevColor, cday.txt);
						}
						else
						{
							sub_widget->AddPrintCall(cellFontDesc.font, cur_x, cur_y, cell_sx, cell_sy, null, 1.0, fontCellColor, cday.txt);
						}
					}
				}				

                cur_x += cell_sx;
			}

			cur_y += cell_sy;
		}
	}
}

void NGUICalendar::CheckPress(float dt, int level)
{
	if (Scene()->IsRunning() && IsVisible() && IsActive())
	{		
		if (sel_theme == 0)
		{			
			if (!in_scroll)
			{
				if (!is_draged && allow_scroll)
				{	
					if (IsHitted(ms_pos_x, ms_pos_y, 0, 0, scr_w, scr_h))					
					{		
						bool drag = core->Controls()->CheckTouch(0, ControlsService::Activated, this);
					
						if (drag)
						{						
							is_draged = true;	
							touchType = -1;
							ms_pos = 0;
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
						{						
							float delta = ms_pos_x - ms_prev_pos_x;						

							if (touchType == -1)
							{
								if (fabs(ms_pos_x - ms_prev_pos_x) > fabs(ms_pos_y - ms_prev_pos_y))
								{
									touchType = 0;
								}
								else							
								if (fabs(ms_pos_x - ms_prev_pos_x) < fabs(ms_pos_y - ms_prev_pos_y))
								{
									touchType = 1;
								}
							}						
						
							if (touchType != -1)
							{
								if (touchType == 0)
								{							
									delta = Convert2AbsHorz(delta);			
	
									childs_offset += delta;			
									ms_pos += delta;

									float k_border = 1.0f;														
								}
								else
								{					
									is_draged = false;
								}
							}
						}						
					}

					if (!is_draged)
					{
						if (ms_pos > scr_w * 0.1f)
						{							
							if (cur_month == month &&
								cur_year == year)
							{
								target_page = cur_page;	
								scroll_action = 0;
							}
							else
							{
								target_page = cur_page - 1;
								scroll_action = 1;
							}
						}
						else
						if (ms_pos < -scr_w * 0.1f)
						{
							target_page = cur_page + 1;									
							scroll_action = 2;							
						}

						in_scroll = true;
					}											
				}
			}
			else
			{
				if (childs_offset > -target_page * scr_w)
				{
					childs_offset -= dt * ConvertHorz(800);
					
					if (childs_offset < -target_page * scr_w)
					{
						target_page = 0;

						childs_offset = -target_page * scr_w;
						cur_page = target_page;	

						if (fabs(ms_pos) > scr_w * 0.1f)
						{
							if (scroll_action == 2)							
							{
								month++;

								if (month > 12)
								{
									year++;
									month = 1;
								}

								CacheDays();	

								scroll_action = 0;
							}
							else
							if (scroll_action == 1)							
							{
								month--;

								if (month <= 0)
								{
									year--;
									month = 12;
								}	
	
								CacheDays();	

								scroll_action = 0;
							}
						}

						in_scroll = false;						
					}
				}
				else
				{
					childs_offset += dt * ConvertHorz(800);					

					if (childs_offset > -target_page * scr_w)
					{
						target_page = 0;

						childs_offset = -target_page * scr_w;
						cur_page = target_page;						

						if (fabs(ms_pos) > scr_w * 0.1f)
						{
							if (scroll_action == 2)							
							{
								month++;

								if (month > 12)
								{
									year++;
									month = 1;
								}

								CacheDays();	

								scroll_action = 0;
							}
							else
							if (scroll_action == 1)							
							{
								month--;

								if (month <= 0)
								{
									year--;
									month = 12;
								}	
	
								CacheDays();	

								scroll_action = 0;
							}							
						}

						in_scroll = false;						
					}	
				}
			}						
		}
		else
		{
			ScrollerWork(this, scroller, dt, 1, allow_scroll);
		}

		if (mscover_widget == this && (!scroller.is_draged && (scroller.time_to_block_childs < 0.0001f)) && IsHitted(ms_pos_x, ms_pos_y, 0, 0, scr_w, scr_h))
		{
			if (core->Controls()->CheckTouch(0, ControlsService::FastActivatedExclusive, this))
			{
				float sz = ConvertVert(topSize);
				float sz2 = ConvertVert(captionSize);

				if (!show_week_days)
				{
					sz2 = 0;
				}

				float sz_btn = ConvertHorz(nextBtnSize);				

				if (IsHitted(ms_pos_x, ms_pos_y))
				{
					float cell_sx = scr_w * 0.1428f;
					float cell_sy = 0;
					
					if (sel_theme == 0)	
					{
						cell_sy = (scr_h - sz) * 0.16666f;
					}
					else
					{
						cell_sy = ConvertVert(cellSize);
					}

					if (sel_theme == 0)	
					{
						float cell_x = (ms_pos_x - scr_x) / cell_sx;
						float cell_y = (ms_pos_y - scr_y - sz - sz2) / cell_sy;

						CacheDay& cday = months[1].days[(int)cell_y * 7 + (int)cell_x];

						bool allow = true;
						
						if (!allow_past && cur_day > cday.day &&
							cur_month == cday.month && cur_year == cday.year)
						{
							allow = false;
						}

						if (cday.forbided)
						{
							allow = false;
						}

						if (cday.month != months[1].days[3 * 7 + 0].month)
						{							
							allow = false;
						}


						if ((sel_day != cday.day ||
							sel_month != cday.month ||
							sel_year != cday.year) && allow)
						{
							sel_day = cday.day;
							sel_month = cday.month;
							sel_year = cday.year;

							if (sel_month != month || sel_year != year)
							{
								month = sel_month;
								year = sel_year;

								CacheDays();
							}
		
							core->Script()->Machine()->AddPtrArg(this);
							core->Script()->Machine()->Call(&callback);
						}
					}
					else
					{
						for (int i=0; i<12; i++)
						{
							if ( sz < ConvertVert(months[i].offset + months[i].num_lines * cellSize + scroller.offset) + sz2 &&
								ConvertVert(months[i].offset + scroller.offset) < scr_h)
							{
								float y1 = scr_y + sz + ConvertVert(months[i].offset + scroller.offset) + sz2;
								float y2 = y1 + ConvertVert(months[i].num_lines * cellSize);

								if (y1 < ms_pos_y && ms_pos_y < y2)
								{
									float cell_x = (ms_pos_x - scr_x) / cell_sx;
									float cell_y = (ms_pos_y - scr_y - ConvertVert(months[i].offset + scroller.offset) - sz2 - sz) / cell_sy;

									CacheDay& cday = months[i].days[(int)cell_y * 7 + (int)cell_x];
									
									if (!cday.forbided && cday.month == months[i].days[3 * 7 + 0].month)
									{
										if ((month == cday.month && year == cday.year && cday.day >= day) || (cday.year == year && cday.month > month) || cday.year > year)
										{
											if (sel_day != cday.day ||
												sel_month != cday.month ||
												sel_year != cday.year)
											{
												sel_day = cday.day;
												sel_month = cday.month;
												sel_year = cday.year;
									
												core->Script()->Machine()->AddPtrArg(this);
												core->Script()->Machine()->Call(&callback);
											}
										}
									}

									break;
								}
							}
						}	
					}
				}
			}
		}
	}
}

CALLWRAPPERARG3(NGUICalendar, NClGetSelectedDate, CALLWRAPPERGETADRESS(int,arg1), CALLWRAPPERGETADRESS(int,arg2), CALLWRAPPERGETADRESS(int,arg3), CALLWRAPPERNORET())
CALLWRAPPERARG4(NGUICalendar, NClMarkDate, CALLWRAPPERGETPARAM(bool,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETPARAM(int,arg3), CALLWRAPPERGETPARAM(int,arg4), CALLWRAPPERNORET())
CALLWRAPPERARG3(NGUICalendar, NClIsDayMarked, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETPARAM(int,arg3), CALLWRAPPERNORET())
CALLWRAPPER(NGUICalendar, NClClearMarkedDays, CALLWRAPPERNORET())
CALLWRAPPERARG3(NGUICalendar, NClSetCurrentDate, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETPARAM(int,arg3), CALLWRAPPERNORET())
CALLWRAPPERARG3(NGUICalendar, NClSetSelectedDate, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETPARAM(int,arg3), CALLWRAPPERNORET())

CALLWRAPPERARG4(NGUICalendar, NClAddForbidedDate, CALLWRAPPERGETPARAM(bool,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETPARAM(int,arg3), CALLWRAPPERGETPARAM(int,arg4), CALLWRAPPERNORET())
CALLWRAPPERARG3(NGUICalendar, NClIsDayForbided, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETPARAM(int,arg3), CALLWRAPPERNORET())
CALLWRAPPER(NGUICalendar, NClClearForbidedDays, CALLWRAPPERNORET())

CALLWRAPPERARG6(NGUICalendar, NClGetNextDayScript, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETPARAM(int,arg3),CALLWRAPPERGETADRESS(int,arg4), CALLWRAPPERGETADRESS(int,arg5), CALLWRAPPERGETADRESS(int,arg6), CALLWRAPPERNORET())
CALLWRAPPERARG6(NGUICalendar, NClGetPrevDayScript, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETPARAM(int,arg3),CALLWRAPPERGETADRESS(int,arg4), CALLWRAPPERGETADRESS(int,arg5), CALLWRAPPERGETADRESS(int,arg6), CALLWRAPPERNORET())
CALLWRAPPERARG2(NGUICalendar, NClSetMonthTextScript, CALLWRAPPERGETPARAM(int,arg1),CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG2(NGUICalendar, NClSetDayTextScript, CALLWRAPPERGETPARAM(int,arg1),CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())

CALLWRAPPERARG3(NGUICalendar, NClSetTopFontColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())	
CALLWRAPPERARG3(NGUICalendar, NClSetCaptionFontColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())	
CALLWRAPPERARG3(NGUICalendar, NClSetCellFontColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())	
CALLWRAPPERARG3(NGUICalendar, NClSetCellSelFontColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())	
CALLWRAPPERARG3(NGUICalendar, NClSetCellPrevFontColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())	

CALLWRAPPERARG3(NGUICalendar, NClSetTopTexColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())	
CALLWRAPPERARG3(NGUICalendar, NClSetLineTexColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())	
CALLWRAPPERARG3(NGUICalendar, NClSetCellTexColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())	
CALLWRAPPERARG3(NGUICalendar, NClSetCellSelTexColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())	
CALLWRAPPERARG3(NGUICalendar, NClSetMarkedTexColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())	
CALLWRAPPERARG3(NGUICalendar, NClSetMarkedPrevTexColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())	

void CALLWRAPPERNAME(NClScriptSetDelegate)(asIScriptGeneric *gen)
{
	NGUICalendar* obj = (NGUICalendar*)gen->GetObject();
	obj->NClScriptSetDelegate(gen);	
}

void NGUICalendar::MethodsToScript(const char* script_class_name)
{
	NGUIWidget::MethodsToScript(script_class_name);

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void GetSelectedDate(int&out day, int&out month, int&out year)", CALLWRAPPERNAMEAS(NClGetSelectedDate));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void MarkDate(bool mark, int day, int month, int year)", CALLWRAPPERNAMEAS(NClMarkDate));					
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "bool IsDayMarked(int day, int month, int year)", CALLWRAPPERNAMEAS(NClIsDayMarked));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void ClearMarkedDays()", CALLWRAPPERNAMEAS(NClClearMarkedDays));					

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetCurrentDate(int day, int month, int year)", CALLWRAPPERNAMEAS(NClSetCurrentDate));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetSelectedDate(int day, int month, int year)", CALLWRAPPERNAMEAS(NClSetSelectedDate));	

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void AddForbidedDate(bool mark, int day, int month, int year)", CALLWRAPPERNAMEAS(NClAddForbidedDate));					
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "bool IsDayForbided(int day, int month, int year)", CALLWRAPPERNAMEAS(NClIsDayForbided));					
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void ClearForbidedDays()", CALLWRAPPERNAMEAS(NClClearForbidedDays));					

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void GetNextDay(int day, int month, int year, int&out out_day, int&out out_month, int&out out_year)", CALLWRAPPERNAMEAS(NClGetNextDayScript));		
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void GetPrevDay(int day, int month, int year, int&out out_day, int&out out_month, int&out out_year)", CALLWRAPPERNAMEAS(NClGetPrevDayScript));
	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetMonthText(int index, string&in text)", CALLWRAPPERNAMEAS(NClSetMonthTextScript));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetDayText(int index, string&in text)", CALLWRAPPERNAMEAS(NClSetDayTextScript));	

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetTopFontColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NClSetTopFontColorScript));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetCaptionFontColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NClSetCaptionFontColorScript));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetCellFontColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NClSetCellFontColorScript));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetCellSelFontColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NClSetCellSelFontColorScript));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetCellPrevFontColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NClSetCellPrevFontColorScript));
	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetTopTexColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NClSetTopTexColorScript));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetLineTexColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NClSetLineTexColorScript));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetCellTexColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NClSetCellTexColorScript));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetCellSelTexColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NClSetCellSelTexColorScript));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetMarkedTexColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NClSetMarkedTexColorScript));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetMarkedPrevTexColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NClSetMarkedPrevTexColorScript));

	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterface("NGUICalendarDelegate");
	((ScriptMachine*)core->Script()->Machine())->Machine()->RegisterInterfaceMethod("NGUICalendarDelegate", "void OnDaySelect(NGUICalendar@)");

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetDelegate(NGUICalendarDelegate@ delegate)", CALLWRAPPERNAMEAS(NClScriptSetDelegate));
}

void NGUICalendar::NClScriptSetDelegate(asIScriptGeneric *gen)
{
	asIScriptObject* scr_object = (asIScriptObject*)gen->GetArgObject(0);	

	asITypeInfo* tp_info = scr_object->GetObjectType();	

	asIScriptFunction* scr_func;

	scr_func = tp_info->GetMethodByDecl("void OnDaySelect(NGUICalendar@)", false);
	callback.Set(scr_func, scr_object);	
}

int NGUICalendar::NClGetDaysInMonth(int month, int year)
{
	if (month < 1)
	{
		month = 12;
		year--;
	}

	int days = 31;

	if (month >= 8)
	{
		days = 30;
	}	

	if (month % 2 == 0)
	{
		if (month >= 8)
		{
			days = 31;
		}
		else
		{
			days = 30;
		}

		if (month == 2)
		{
			if (year % 4 == 0)
			{
				days = 29;
			}
			else
			{
				days = 28;
			}
		}
	}

	return days;
}

void NGUICalendar::NClGetSelectedDate(int& day, int& month, int& year)
{
	year = sel_year;
	month = sel_month;
	day = sel_day;
}

void NGUICalendar::NClSetSelectedDate(int day, int month, int year)
{
	sel_year = year;
	sel_month = month;
	sel_day = day;
}

void NGUICalendar::NClMarkDate(bool mark, int day, int month, int year)
{
	for (int i=0;i<marked_dates.size();i++)
	{
		if (marked_dates[i].day == day &&
			marked_dates[i].month == month &&
			marked_dates[i].year == year)
		{
			if (!mark)
			{
				marked_dates.erase(marked_dates.begin() + i);
				CacheDays();
			}						

			return;
		}
	}

	if (!mark)
	{
		return;
	}

	Date date;
	date.day = day;
	date.month = month;
	date.year = year;

	marked_dates.push_back(date);

	CacheDays();
}

bool NGUICalendar::NClIsDayMarked(int day, int month, int year)
{
	for (int i=0;i<marked_dates.size();i++)
	{
		if (marked_dates[i].day == day &&
			marked_dates[i].month == month &&
			marked_dates[i].year == year)
		{
			return true;
		}
	}

	return false;
}

void NGUICalendar::NClClearMarkedDays()
{
	marked_dates.clear();
}

void NGUICalendar::NClSetCurrentDate(int set_day, int set_month, int set_year)
{
	cur_day = set_day;
	cur_month = set_month;
	cur_year = set_year;

	day = set_day;
	month = set_month;
	year = set_year;

	CacheDays();
}

void NGUICalendar::CacheDays()
{	
	if (sel_theme == 0)
	{
		if (month - 1 == 0)
		{
			CacheDays(year - 1, 12, &months[0]);
		}
		else
		{
			CacheDays(year, month - 1, &months[0]);
		}

		CacheDays(year, month, &months[1]);
	
		if (month + 1 == 13)
		{
			CacheDays(year + 1, 1, &months[2]);
		}
		else
		{
			CacheDays(year, month + 1, &months[2]);
		}
	}
	else
	{		
		int system_month = GetTime(4);
		int system_year = GetTime(5);	

		scroller.scroll_size = 0;

		for (int i=0; i<12; i++)
		{
			CacheDays(system_year, system_month, &months[i]);

			months[i].offset = scroller.scroll_size;
			scroller.scroll_size += months[i].num_lines * cellSize + captionSize;

			if (i<11)
			{
				scroller.scroll_size += monthGap;
			}

			system_month++;

			if (system_month > 12)
			{
				system_month = 1;
				system_year++;
			}
		}

		scroller.scroll_size -= (height - topSize);

		if (scroller.scroll_size < 0)
		{
			scroller.scroll_size = 0;
		}
	}
}

void NGUICalendar::NClSetTopFontColorScript(float r, float g, float b)
{
	topFontDesc.color.r = r;
	topFontDesc.color.g = g;
	topFontDesc.color.b = b;
}

void NGUICalendar::NClSetCaptionFontColorScript(float r, float g, float b)
{
	captionFontDesc.color.r = r;
	captionFontDesc.color.g = g;
	captionFontDesc.color.b = b;
}

void NGUICalendar::NClSetCellFontColorScript(float r, float g, float b)
{
	fontCellColor.r = r;
	fontCellColor.g = g;
	fontCellColor.b = b;
}

void NGUICalendar::NClSetCellSelFontColorScript(float r, float g, float b)
{
	fontCellSelColor.r = r;
	fontCellSelColor.g = g;
	fontCellSelColor.b = b;
}

void NGUICalendar::NClSetCellPrevFontColorScript(float r, float g, float b)
{
	fontCellPrevColor.r = r;
	fontCellPrevColor.g = g;
	fontCellPrevColor.b = b;
}

void NGUICalendar::NClSetTopTexColorScript(float r, float g, float b)
{
	topTexColor.r = r;
	topTexColor.g = g;
	topTexColor.b = b;
}

void NGUICalendar::NClSetLineTexColorScript(float r, float g, float b)
{
	lineTexColor.r = r;
	lineTexColor.g = g;
	lineTexColor.b = b;
}

void NGUICalendar::NClSetCellTexColorScript(float r, float g, float b)
{
	cellTexColor.r = r;
	cellTexColor.g = g;
	cellTexColor.b = b;
}

void NGUICalendar::NClSetCellSelTexColorScript(float r, float g, float b)
{
	cellSelTexColor.r = r;
	cellSelTexColor.g = g;
	cellSelTexColor.b = b;
}

void NGUICalendar::NClSetMarkedTexColorScript(float r, float g, float b)
{
	cellMarkedTexColor.r = r;
	cellMarkedTexColor.g = g;
	cellMarkedTexColor.b = b;
}

void NGUICalendar::NClSetMarkedPrevTexColorScript(float r, float g, float b)
{
	cellMarkedPrevTexColor.r = r;
	cellMarkedPrevTexColor.g = g;
	cellMarkedPrevTexColor.b = b;
}

void NGUICalendar::CacheDays(int year, int month, MonthChache* cache)
{	
	int a = (14 - month) / 12;
	int y = year - a;
	int m = month + 12 * a - 2;

	int day = (7000 + (1 + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12)) % 7 - 1;
	if (day < 0) day = 6;
	
	int cur_day = 1;
	int cur_month = month;
	int cur_year = year;

	int days_in_month = NClGetDaysInMonth(month, year);

	for (int i=0;i<6;i++)
	{
		if (day != 0 && i == 0)
		{				
			cur_month--;

			if (cur_month < 1)
			{
				cur_month = 12;
				cur_year--;
			}

			int pre_mn_day = NClGetDaysInMonth(month - 1, year);

			cur_day = pre_mn_day - day + 1;

			for (int j=0;j<7;j++)
			{
				CacheDay& cday = cache->days[i * 7 + j];
				
				String::Printf(cday.txt, 4, "%i", cur_day);			
				
				cday.day = cur_day;
				cday.month = cur_month;
				cday.year = cur_year;				

				cday.marked = NClIsDayMarked(cur_day, cur_month, cur_year);

				cday.forbided = NClIsDayForbided(cur_day, cur_month, cur_year);

				cur_day++;

				if (cur_day > pre_mn_day)
				{
					cur_day = 1;	
					cur_month = month;
					cur_year = year;
				}
			}				

			int days_in_month = NClGetDaysInMonth(month, year);
		}
		else
		{
			for (int j=0;j<7;j++)
			{
				CacheDay& cday = cache->days[i * 7 + j];

				String::Printf(cday.txt, 4, "%i", cur_day);			

				cday.day = cur_day;
				cday.month = cur_month;
				cday.year = cur_year;											
				
				cday.marked = NClIsDayMarked(cur_day, cur_month, cur_year);

				cday.forbided = NClIsDayForbided(cur_day, cur_month, cur_year);

				cur_day++;

				if (cur_day > days_in_month)
				{
					cur_day = 1;	
					cur_month++;

					if (cur_month > 12)
					{
						cur_month = 1;
						cur_year++;
					}
				}			
			}
		}		
	}

	if (cache->days[5 * 7].month == cache->days[3 * 7 + 0].month)
	{
		cache->num_lines = 6;
	}
	else
	{
		cache->num_lines = 5;
	}
}

void NGUICalendar::NClAddForbidedDate(bool add, int day, int month, int year)
{
	for (int i=0;i<forbided_dates.size();i++)
	{
		if (forbided_dates[i].day == day &&
			forbided_dates[i].month == month &&
			forbided_dates[i].year == year)
		{
			if (!add)
			{
				forbided_dates.erase(forbided_dates.begin() + i);
				CacheDays();
			}						

			return;
		}
	}

	if (!add)
	{
		return;
	}

	Date date;
	date.day = day;
	date.month = month;
	date.year = year;

	forbided_dates.push_back(date);

	CacheDays();
}

bool NGUICalendar::NClIsDayForbided(int day, int month, int year)
{
	for (int i=0;i<forbided_dates.size();i++)
	{
		if (forbided_dates[i].day == day &&
			forbided_dates[i].month == month &&
			forbided_dates[i].year == year)
		{
			return true;
		}
	}

	return false;
}

void NGUICalendar::NClClearForbidedDays()
{
	forbided_dates.clear();

	CacheDays();
}

void NGUICalendar::NClGetPrevDayScript(int day, int month, int year, int& out_day, int& out_month, int& out_year)
{
	day--;
	
	if (day == 0)
	{
		month--;

		if (month == 0)
		{
			month = 12;
			year--;
		}

		day = NClGetDaysInMonth(month, year);
	}

	out_day = day;
	out_month = month;
	out_year = year;
}

void NGUICalendar::NClGetNextDayScript(int day, int month, int year, int& out_day, int& out_month, int& out_year)
{	
	day++;
	
	if (day > NClGetDaysInMonth(month, year))
	{
		day = 1;
		month++;

		if (month > 12)
		{
			month = 1;
			year++;
		}
	}	

	out_day = day;
	out_month = month;
	out_year = year;
}

void NGUICalendar::NClSetMonthTextScript(int index, string& text)
{
	if (index<0 || index>11) return;

	String::Copy(month_str[index].txt, 64, text.c_str());
}

void NGUICalendar::NClSetDayTextScript(int index, string& text)
{
	if (index<0 || index>6) return;

	String::Copy(day_str[index].txt, 64, text.c_str());	
}

void NGUICalendar::Release()
{
	DelUpdate((TaskHolder::Task)&NGUICalendar::CheckPress);

	delete sub_widget;

	RELEASE(topTex)		
    RELEASE(lineTex)
	RELEASE(cellTex)
	RELEASE(cellSelTex)
	RELEASE(cellMarkedTex)
	RELEASE(cellMarkedPrevTex)		

	RELEASE(topFontDesc.font)	
	RELEASE(captionFontDesc.font)	
	RELEASE(cellFontDesc.font)

	NGUIWidget::Release();
}