
#include "windows.h"
#include "TimeLineWidget.h"
//#include "Entities/Animator/DataTrack/DataTrack.h"
//#include "Entities/Animator/TrackPlayers/TrackPlayer.h"
#include "Core/Core.h"

TimeLineWidget::TimeLineWidget(int set_id, BaseWidget* prnt, float set_x, float set_y, float w, float h) : BaseWidget(set_id, prnt, "TimeLine")
{
	x = set_x;
	y = set_y;
	width = w;
	height = h;

	handle = CreateWindow("STATIC","",SS_LEFT|WS_CHILD,
						   set_x,set_y,w,h,(HWND)parent->GetHandle(),
						   NULL, en_core.GetInstance(), NULL);

	SetWindowLong((HWND)handle, GWL_ID, id);


	timeLenght = 0.0f;
	scale = 100.0f;
	selLine = -1;
	selKey = -1;
	drag_mode = 0;

	play = false;

	curTime = null;
	fromTime = null;
	toTime = null;

	edCurTime = null;	
	edFromTime = null;
	edToTime = null;

	gizmo = null;
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(handle, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
}

void TimeLineWidget::ProcessWidget(WPARAM wParam, LPARAM lParam)
{
	if (selLine != -1)
	{
		Line* ln = &lines[selLine];

		if (ln->isSeparator)
		{
			//fix me!!!!
			//((TrackPlayer*)ln->ptr)->ProcessWidgets(wParam, lParam);
		}
	}

	/*if (id != LOWORD(wParam)) return;

	if (HIWORD(wParam)==BN_CLICKED)
	{
		EventsWidget::SetEvent(id, EventsWidget::btn_click);
	}*/
}

void TimeLineWidget::Draw()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint((HWND)handle, &ps);

	Rectangle(hdc, 0, 0, 150, 30);
	Rectangle(hdc, 150, 0, 700, 30);

	float offset = horz_scroll->GetPosition() * 0.1f;
	float tm = offset;

	while (tm<timeLenght + 0.001f && tm < offset + 5.0f)
	{
		float pos = (int)((tm - offset)* scale) + 154;
		float y = 24;
		
		if (fabs(tm -(int)(tm + 0.25f)) < 0.01f)
		{
			char str[16];
			sprintf(str, "%i", (int)(tm + 0.25f));

			TextOut(hdc,pos - 2, 3, str,strlen(str));
			y = 20;
		}
		
		MoveToEx(hdc, pos, y, 0);
		LineTo(hdc, pos, 30);

		tm += 0.1f;
	}	

	SelectObject(hdc, GetStockObject(DC_BRUSH));

	int last_y = 30;

	int active = 1;

	for (int i=0; i<(int)lines.size(); i++)
	{
		if (i<vert_scroll->GetPosition()) continue;
		if (i>vert_scroll->GetPosition()+11) continue;

		int index = i - vert_scroll->GetPosition();

		last_y += 20;

		if (lines[i].isSeparator)
		{
			//active = (((TrackPlayer*)lines[i].ptr)->IsActive());

			if (selLine == i)
			{
				SetDCBrushColor(hdc, RGB(150 + 50 * active,205 + 50 * active,205 + 50 * active));
				SetBkColor(hdc, RGB(150 + 50 * active,205 + 50 * active,205 + 50 * active));
			}
			else
			{
				SetDCBrushColor(hdc, RGB(100 + 50 * active,185 + 50 * active,185 + 50 * active));
				SetBkColor(hdc, RGB(100 + 50 * active,185 + 50 * active,185 + 50 * active));
			}
		}
		else
		{
			if (selLine == i)
			{
				SetDCBrushColor(hdc, RGB(205 + 50 * active,205 + 50 * active,205 + 50 * active));
				SetBkColor(hdc, RGB(205 + 50 * active,205 + 50 * active,205 + 50 * active));
			}
			else
			{
				SetDCBrushColor(hdc, RGB(160 + 50 * active,160 + 50 * active,160 + 50 * active));
				SetBkColor(hdc, RGB(160 + 50 * active,160 + 50 * active,160 + 50 * active));
			}
		}

		Rectangle(hdc, 0, 20 * (index) + 30, 150, 20 * (index + 1) + 30);
		Rectangle(hdc, 150, 20 * (index) + 30, 700, 20 * (index + 1) + 30);

		TextOut(hdc,10, 20 * index + 31, lines[i].name,strlen(lines[i].name));

		if (lines[i].isSeparator)
		{
			if (active)
			{
				TextOut(hdc,95, 20 * index + 31, "(Active)", strlen("(Active)"));
			}
			else
			{
				TextOut(hdc,90, 20 * index + 31, "(Inactive)", strlen("(Inactive)"));
			}
		}
	}	

	if ((*curTime - offset>=0.0f) && (*curTime - offset<=5.0f))
	{
		MoveToEx(hdc, CalcKeyPos(*curTime - offset) + 2, 12, 0);
		LineTo(hdc, CalcKeyPos(*curTime - offset) + 2, 30 + 12 * 20);
	}

	if ((*fromTime - offset>=0.0f) && (*fromTime - offset<=5.0f))
	{
		MoveToEx(hdc, CalcKeyPos(*fromTime - offset) + 2, 12, 0);
		LineTo(hdc,  CalcKeyPos(*fromTime - offset) + 2, 30 + 12 * 20);
	}

	if ((*toTime - offset>=0.0f) && (*toTime - offset<=5.0f))
	{
		MoveToEx(hdc, CalcKeyPos(*toTime - offset) + 2, 12, 0);
		LineTo(hdc, CalcKeyPos(*toTime - offset) + 2, 30 + 12 * 20);
	}

	SetDCBrushColor(hdc, RGB(210,210,210));

	Rectangle(hdc, 0, last_y, width, height);


	if ((*fromTime - offset>=0.0f) && (*fromTime - offset<=5.0f))
	{
		Rectangle(hdc, CalcKeyPos(*fromTime - offset), 2, CalcKeyPos(*fromTime - offset) + 5, 16);
	}

	if ((*toTime - offset>=0.0f) && (*toTime - offset<=5.0f))
	{
		Rectangle(hdc, CalcKeyPos(*toTime - offset), 2, CalcKeyPos(*toTime - offset) + 5, 16);
	}

	SetDCBrushColor(hdc, RGB(255,255,255));

	if ((*curTime - offset>=0.0f) && (*curTime - offset<=5.0f))
	{
		Rectangle(hdc, CalcKeyPos(*curTime - offset), 2, CalcKeyPos(*curTime - offset) + 5, 16);
	}

	for (int i=0; i<(int)lines.size(); i++)
	{
		if (i<vert_scroll->GetPosition()) continue;
		if (i>vert_scroll->GetPosition()+11) continue;

		Line* line = &lines[i];

		if (!line->isSeparator)
		{	
			for (int j=0; j<(int)line->keys.size()-1; j++)
			{
				if (!line->keys[j].needBlend) continue;

				if (line->keys[j+1].time > timeLenght) break;

				if (line->keys[j+1].time - offset<0.0f) continue;
				if (line->keys[j].time - offset>5.0f) continue;

				float tm1 = line->keys[j].time - offset;
				if (tm1<0.0f) tm1 = 0.0f;

				float tm2 = line->keys[j+1].time - offset;
				if (tm2>5.0f) tm2 = 5.0f;

				MoveToEx(hdc, CalcKeyPos(tm1), CalcLinePos(i-vert_scroll->GetPosition()) + 9, 0);
				LineTo(hdc, CalcKeyPos(tm2), CalcLinePos(i-vert_scroll->GetPosition()) + 9);		
			}

			for (int j=0; j<(int)line->keys.size(); j++)
			{		
				if (timeLenght<line->keys[j].time) break;

				SetDCBrushColor(hdc, RGB(255,255,255));

				if (selLine == i && selKey == j)
				{
					SetDCBrushColor(hdc, RGB(120, 255, 120));
				}

				/*else if (selLine == i)
				{
//				std::vector<int>::iterator it = std::find(m_SelLines.begin(), m_SelLines.end(), i);
//				if (it != m_SelLines.end())
					{
						std::vector<int>::iterator itkey = std::find(m_SelKeys.begin(), m_SelKeys.end(), j);
						if (itkey != m_SelKeys.end())
						{
							dc.SetPen( *wxBLUE_PEN );
							dc.SetBrush( *wxGREEN_BRUSH );
						}

					}
				}*/
				
				if (line->keys[j].time - offset<0.0f) continue;
				if (line->keys[j].time - offset>5.0f) continue;

				Rectangle(hdc, CalcKeyPos(line->keys[j].time-offset), CalcLinePos(i-vert_scroll->GetPosition()) + 2,
							   CalcKeyPos(line->keys[j].time-offset) + 5, CalcLinePos(i-vert_scroll->GetPosition()) + 16);
			}
		}
	}

	EndPaint((HWND)handle, &ps);

/*
	int dx, dy;
	GetViewStart(&dx, &dy);

	captions->Scroll(0, dy);

	wxPaintDC pdc(this);
	wxDC &dc = pdc ;
	PrepareDC(dc);		

	dc.Clear();

	int x = 100,
		y = 100,
		r = 20;

	dc.SetPen( *wxGREY_PEN );
	dc.SetBrush( *wxWHITE_BRUSH );
	
	float total_size = timeLenght * scale + 9;

	dc.DrawRectangle(0,0, total_size,30);
	dc.SetPen( *wxBLACK_PEN );

	float cur_time = 0.0f;	

	while (cur_time<timeLenght + 0.001f)
	{
		float pos = (int)(cur_time * scale) + 4;
		float y = 24;
		
		if (fabs(cur_time -(int)(cur_time + 0.25f)) < 0.01f)
		{
			char str[16];
			sprintf(str, "%i", (int)(cur_time + 0.25f));

			dc.DrawText(str, pos - 2, 3);
			y = 20;
		}
		
		dc.DrawLine(pos, y, pos, 30);

		cur_time += 0.1f;
	}	




  bool separatorActive = true;
	for (int i=0; i<(int)lines.size(); i++)
	{
		Line* line = &lines[i];

		if (line->isSeparator)
		{
			separatorActive = ((TrackPlayer*)line->ptr)->GetActive();
		}

		if (line->isSeparator)
		{
			dc.SetPen( *wxGREY_PEN );
			dc.SetBrush( *wxGREY_BRUSH );
		}
		else
		{
			if (separatorActive)
			{
				dc.SetPen( *wxGREY_PEN );
				if (! ((DataTrack*)line->ptr)->GetParent())
					dc.SetBrush( *wxWHITE_BRUSH );
				else
					dc.SetBrush( *wxLIGHT_GREY_BRUSH );
			}
			else
			{
				dc.SetPen( *wxGREY_PEN );
				dc.SetBrush( *wxBLACK_BRUSH);
			}

		}
	

		if (selLine == i)
		{
			if (separatorActive || line->isSeparator)
			{
				dc.SetPen( *wxGREY_PEN );
				dc.SetBrush( *wxCYAN_BRUSH );
			}
			else
			{
				dc.SetPen( *wxGREY_PEN );
				dc.SetBrush( *wxBLACK_BRUSH);
			}

		}

		dc.DrawRectangle(0,CalcLinePos(i), total_size, 20);
	}	

	
	dc.SetPen( *wxBLACK_PEN );	
	dc.DrawLine(CalcKeyPos(fromTime) + 2, 12, CalcKeyPos(fromTime) + 2, 30 + (int)lines.size() * 20);
	dc.DrawLine(CalcKeyPos(toTime) + 2, 12, CalcKeyPos(toTime) + 2, 30 + (int)lines.size() * 20);

	dc.SetPen( *wxRED_PEN );
	dc.DrawLine(CalcKeyPos(curTime) + 2, 12, CalcKeyPos(curTime) + 2, 30 + (int)lines.size() * 20);
	
	dc.SetPen( *wxGREEN_PEN );
	dc.SetBrush( *wxGREEN_BRUSH );

	dc.DrawRectangle(CalcKeyPos(fromTime), 2, 5, 16);
	dc.DrawRectangle(CalcKeyPos(toTime), 2, 5, 16);
	
	dc.SetPen( *wxBLUE_PEN );
	dc.SetBrush( *wxBLUE_BRUSH );

	dc.DrawRectangle(CalcKeyPos(curTime), 2, 5, 16);	
	
	dc.SetPen( *wxBLUE_PEN );
	dc.SetBrush( *wxBLUE_BRUSH );

	dc.DrawRectangle(CalcKeyPos(curTime), 2, 5, 16);	
		
	*/
}

float TimeLineWidget::CalcKeyPos(float ps_x)
{
	return (int)(ps_x * scale) - 2 + 154;
}

float TimeLineWidget::CalcLinePos(int index)
{
	return index*20 + 30;
}

float TimeLineWidget::PixelToTime(float px)
{
	return ((int)(px * 0.1f) * 10) / scale;
}

/*
void WidgetTimeLine::OnKeyDown(wxKeyEvent& event)
{    
	if (event.ControlDown())
	{
		if (selLine == -1 )
			return;

		Line* line = &lines[selLine];

		if (line->isSeparator)
			return;

		if	(event.GetKeyCode() == 'C')
		{
			((DataTrack*)line->ptr)->CopyToBuffer();		
		}
		else 	if	(event.GetKeyCode() == 'V')
		{
			if (selKey == -1)
			{
				MainFrame::tools->cutscene->OnKeyAdd(wxCommandEvent());
				if (selKey == -1)
					return;

				((DataTrack*)line->ptr)->PasteFromBuffer();	
				((DataTrack*)line->ptr)->StartEditKey(selKey, manager->gizmo);		
			}
			else
			{
				((DataTrack*)line->ptr)->PasteFromBuffer();	
				((DataTrack*)line->ptr)->StartEditKey(selKey, manager->gizmo);		
			}

		}

	}
}
*/

void TimeLineWidget::OnLeftMouseDown(int mx, int my)
{
	bool need_redraw = false;
		
	int new_selLine = -1;
	int new_selKey = -1;

	if (my>30)
	{
		new_selLine = (int)(((float)my - 30.0f)/20.0f) + vert_scroll->GetPosition();

		if (new_selLine>(int)lines.size()-1)
		{			
			return;
		}

		Line* line = &lines[new_selLine];
	
		if (!line->isSeparator)
		{						
			for (int i=0;i<(int)line->keys.size(); i++)
			{
				int pos = CalcKeyPos(line->keys[i].time - horz_scroll->GetPosition() * 0.1f);								

				if ( pos<mx && mx<pos + 5 &&
					 CalcLinePos(new_selLine) + 2 - vert_scroll->GetPosition() * 20.0f < my && my<CalcLinePos(new_selLine) + 18 - vert_scroll->GetPosition() * 20.0f)
				{
					new_selKey = i;
					if (m_SelKeys.empty())
						drag_mode = 2;
					//move multi keys
					else
						drag_mode = 6;
					break;
				}
			}
		}

		if (!m_SelKeys.empty() && GetKeyState(VK_CONTROL) >= 0 && GetKeyState(VK_SHIFT) >= 0)
		{
			m_SelLines.clear();
			m_SelKeys.clear();

			Line* ln = &lines[selLine];
			//((DataTrack*)ln->ptr)->StopEditMultiKeys();	
		}
		

		if (selLine != new_selLine || selKey != new_selKey)
		{
			if (GetKeyState(VK_SHIFT) < 0 )
			{
				int selKeyCopy = selKey ;
				if (selKeyCopy != -1 && new_selKey != -1)
				{
					if (selKeyCopy < new_selKey)
					{
						for (dword i = selKeyCopy+1; i <= new_selKey; ++i)
						{
							AddToMultiSelection(new_selLine, i);		
						}
					}
					else
					{
						for (dword i = new_selKey; i <= selKeyCopy-1; ++i)
						{
							AddToMultiSelection(new_selLine, i);		
						}
					}
					
				}
			}
			else
			{
				if (GetKeyState(VK_CONTROL) < 0 )
				{
					if (m_SelKeys.empty() && selKey == -1)
						SetSelection(new_selLine, new_selKey);			
					else
						AddToMultiSelection(new_selLine, new_selKey);	
				}
				else if (GetKeyState(VK_SHIFT) >= 0 )
					SetSelection(new_selLine, new_selKey);	
			}

			need_redraw = true;
		}		
	}
	
	if (mx>=150)
	{
		float pos = mx - 152.0f;
		pos = PixelToTime(pos);
		if (pos<0) pos = 0.0f;
		pos += horz_scroll->GetPosition() * 0.1f;

		if (fabs(pos-*fromTime)<0.01f && my<30)
		{
			drag_mode = 4;
		}
		else
		if (fabs(pos-*toTime)<0.01f && my<30)
		{
			drag_mode = 5;
		}
		else
		{
			if (selKey != -1 && my>30)
			{
				drag_mode = 3;
			}
			else
			{
				if (m_SelKeys.empty())
					drag_mode = 1;
				//move multi keys
				else
					drag_mode = 6;
			}	
			
			SetCurTime(pos);

			need_redraw = true;
		}
	}
	else
	if (mx>=0)
	{
		if (new_selLine != -1)
		{
			if (lines[new_selLine].isSeparator)
			{
				//((TrackPlayer*)lines[new_selLine].ptr)->SetActive(!((TrackPlayer*)lines[new_selLine].ptr)->IsActive());
				need_redraw = true;
			}
		}
	}

	if (need_redraw) Redraw();
}

void TimeLineWidget::OnMouseMove(int mx, int my)
{
	/*if ( GetKeyState(VK_LBUTTON) < 0 ) Refresh();

	int dx, dy;
	GetViewStart(&dx, &dy);

	

	int mx = event.GetX() + dx * 10;
	int my = event.GetY() + dy * 10;*/

	static int mxPrev = mx;
	static int myPrev = my;

	if (drag_mode>0)
	{		
		float pos = mx - 152.0f;
		pos = PixelToTime(pos);
		if (pos<0) pos = 0.0f;
		pos += horz_scroll->GetPosition() * 0.1f;
		
		if (drag_mode == 2 || drag_mode == 3)
		{
			bool allow = true;

			if (pos>timeLenght+0.01f)
			{
				allow = false;
			}

			if (selKey>0)
			{
				if (lines[selLine].keys[selKey-1].time+0.01f>pos)
					allow = false;
			}

			if (selKey<(int)lines[selLine].keys.size()-1)
			{
				if (lines[selLine].keys[selKey+1].time-0.01f<pos) 
					allow = false;
			}

			if (allow)
			{
				lines[selLine].keys[selKey].time = pos;
				//((DataTrack*)lines[selLine].ptr)->SetEditedKeyTime(pos);

				dword size = 0;//((DataTrack*)lines[selLine].ptr)->GetChilds().size();
				for (dword i = 1; i <= size; ++i)
				{
					lines[selLine + i].keys[selKey].time = pos;
				}
			}
		}		
		else
		if(drag_mode == 6)
		{
			float pos = mx - 2.0f;
			pos = PixelToTime(pos);
			float posPrev = mxPrev - 2.0f;
			posPrev = PixelToTime(posPrev);

			float difTime = pos - posPrev;

			bool allow = true;


			if (lines[selLine].keys[m_SelKeys.front()].time+ difTime < 0.f)
			{
				allow = false;
			}

			if (lines[selLine].keys[m_SelKeys.back()].time + difTime> timeLenght)
			{
				allow = false;
			}

			if (m_SelKeys.front() > 0)
			{
				if (lines[selLine].keys[m_SelKeys.front()].time + difTime < lines[selLine].keys[m_SelKeys.front()-1].time + 0.01f )
					allow = false;
			}

			if (m_SelKeys.back() < (int)lines[selLine].keys.size()-1)
			{
				if (lines[selLine].keys[m_SelKeys.back()].time + difTime > lines[selLine].keys[m_SelKeys.back()+1].time - 0.01f ) 
					allow = false;
			}

			/*if (difTime < 0)
			{
				dword size = m_SelKeys.size();
				for (dword i = 0; i < size; ++i)
				{
					if (m_SelKeys[i] == 0)
						continue;
					if (lines[selLine].keys[m_SelKeys[i]].time + difTime < lines[selLine].keys[m_SelKeys[i]-1].time + 0.01f 
						&& std::find(m_SelKeys.begin(), m_SelKeys.end(), m_SelKeys[i]-1) == m_SelKeys.end())
						allow = false;
				}
			}

			if (difTime > 0)
			{
				dword size = m_SelKeys.size();
				for (dword i = 0; i < size; ++i)
				{
					if (m_SelKeys[i] == (int)lines[selLine].keys.size()-1)
						continue;
					if (lines[selLine].keys[m_SelKeys[i]].time + difTime > lines[selLine].keys[m_SelKeys[i]+1].time - 0.01f
						&& std::find(m_SelKeys.begin(), m_SelKeys.end(), m_SelKeys[i]+1) == m_SelKeys.end()) 
						allow = false;
				}
			}*/

			if (allow)
			{
				dword size = m_SelKeys.size();
				for (dword i = 0; i < size; ++i)
				{
					lines[selLine].keys[m_SelKeys[i]].time += difTime;
					//((DataTrack*)lines[selLine].ptr)->SetKeyTime(m_SelKeys[i], lines[selLine].keys[m_SelKeys[i]].time);

					dword sizechilds  = 0;//((DataTrack*)lines[selLine].ptr)->GetChilds().size();
						
					for (dword j = 1 ; j <= sizechilds; ++j)
					{
						lines[selLine+j].keys[m_SelKeys[i]].time += difTime;
					}
				}

			}
		}

		if (drag_mode == 1 || drag_mode == 3)
		{
			SetCurTime(pos);
		}

		if (drag_mode == 4)
		{
			SetFromTime(pos);
		}

		if (drag_mode == 5)
		{
			SetToTime(pos);
		}

		Redraw();
	}	

	mxPrev = mx;
	myPrev = my;
}

void TimeLineWidget::OnLeftMouseUp(int mx, int my)
{
	drag_mode = 0;
}

void TimeLineWidget::Clear()
{
	//if (gizmo) gizmo->enabled = false;

	drag_mode = 0;
	timeLenght = 0.0f;
	selLine = -1;
	selKey = -1;
	lines.clear();
}

bool TimeLineWidget::IsTimeVisible(float time)
{
	/*int dx, dy;
	GetViewStart(&dx, &dy);

	float left_border = PixelToTime( dx * 10.0f);
	float right_border = PixelToTime( dx * 10.0f + GetSize().x);
	
	if (left_border<time && time<right_border)
	{
		return true;
	}*/

	return false;
}

void TimeLineWidget::SetTimeVisible(float time)
{
	/*if (!IsTimeVisible(time)) return;

	float pos = CalcKeyPos(time) + 2;
	
	pos -= (float)GetSize().x * 0.5f;

	if (pos<0.0f) pos = 0.0f;	

	pos *= 0.1f;

	Scroll(pos, 0);
	SetCurTime(time);*/
}

void TimeLineWidget::SetTime(float time)
{
	float pos = CalcKeyPos(time) + 2;

	pos -= (float)GetX() * 0.5f;

	if (pos<0.0f) pos = 0.0f;	

	pos *= 0.1f;

	//Scroll(pos, 0);
	//SetCurTime(time);
}

void TimeLineWidget::SetTimeLenght(float time)
{
	if (fabs(timeLenght - *toTime)<0.01f || *toTime>time)
	{
		SetToTime(time);
	}
	
	if (*fromTime > timeLenght)
	{
		SetFromTime(0.0f);
	}
	
	timeLenght = time;
	
	if (*curTime>timeLenght)
	{
		SetCurTime(timeLenght);	
	}

	UpdateHorizBar();
}

void TimeLineWidget::SetCurTime(float time)
{
	float ct = ((int)(time * 10.0f) * 0.1f);
	
	if (fabs(*curTime - ct) < 0.0001f) return;

	*curTime = ct;

	if (*curTime<*fromTime)
	{
		*curTime=*fromTime;
	}

	if (*curTime>*toTime)
	{
		*curTime = *toTime;
	}

	char str[32];
	sprintf(str, "%4.2f", *curTime);
	edCurTime->SetText(str);

	Redraw();	
}

void TimeLineWidget::SetFromTime(float time)
{
	*fromTime = ((int)(time * 10.0f) * 0.1f);

	if (*fromTime<0.0f)
	{
		*fromTime=0.0f;
	}

	if (*fromTime>*curTime)
	{
		*fromTime=*curTime;
	}

	char str[32];
	sprintf(str, "%4.2f", *fromTime);
	edFromTime->SetText(str);

	Redraw();
}

void TimeLineWidget::SetToTime(float time)
{
	*toTime = ((int)(time * 10.0f) * 0.1f);

	if (*toTime<*curTime)
	{
		*toTime=*curTime;
	}

	if (*toTime>timeLenght)
	{
		*toTime=timeLenght;
	}

	char str[32];
	sprintf(str, "%4.2f", *toTime);
	edToTime->SetText(str);

	Redraw();
}

void TimeLineWidget::AddLine(const char* name, void* ptr, bool separator)
{
	lines.push_back(Line());
	lines[(int)lines.size()-1].isSeparator = separator;
	String::Copy(lines[(int)lines.size()-1].name, 64, name);
	lines[(int)lines.size()-1].ptr = ptr;

	UpdateVertBar();
	Redraw();
}

void TimeLineWidget::InsertLine(int index, const char* name, void* ptr, bool separator)
{
	lines.insert(lines.begin() + index, Line());

	lines[index].isSeparator = separator;
	String::Copy(lines[index].name, 64, name);
	lines[index].ptr = ptr;

	UpdateVertBar();
	Redraw();
}

void TimeLineWidget::DelLine(int index)
{
	lines.erase(lines.begin() + index);

	UpdateVertBar();
	Redraw();
}

void* TimeLineWidget::GetSelLineOwner()
{
	if (selLine != -1)
	{
		int index = selLine;

		while(!lines[index].isSeparator)
		{
			index--;
		}

		return lines[index].ptr;
	}

	return null; 
}

void TimeLineWidget::AddKey()
{
	AddKey(selLine, *curTime, true);

	if (selLine!=-1)
	{
		Line* line = &lines[selLine];

		if (!line->isSeparator && line->ptr)
		{
			//int key = ((DataTrack*)line->ptr)->AddKey(*curTime);
			//SetSelection(selLine, key);
		}
	}
}

void TimeLineWidget::AddKey(int line_index, float time, bool needBlend)
{
	if ((int)lines.size() == 0) return;
	
	int index = line_index;
	if (index == -1 || index == 0) return;
	
	Line* line = &lines[index];
	
	if (line->isSeparator) return;

	for (int i=0;i<(int)line->keys.size();i++)
	{
		if (fabs(line->keys[i].time - time)<0.01f)
		{
			return;				
		}
	}

	Key key;
	key.time = time;
	key.needBlend = needBlend;

	line->keys.push_back(key);

	for (int i=(int)line->keys.size()-1; i>0; i--)
	{
		if (line->keys[i].time<line->keys[i-1].time)
		{
			Key tmp = line->keys[i];
			line->keys[i] = line->keys[i-1];
			line->keys[i-1] = tmp;

			continue;
		}
		
		break;		
	}	

	Redraw();
}

void TimeLineWidget::DelKey()
{
	if (selLine == -1 || selKey == -1) return;

	Line* line = &lines[selLine];

	if (!line->keys[selKey].needBlend)
	{
		if (selKey>0)
		{
			line->keys[selKey-1].needBlend = false;
		}
	}

	for (int j=selKey;j<(int)line->keys.size()-1;j++)
	{
		line->keys[j] = line->keys[j+1];
	}

	selKey = -1;
	line->keys.pop_back();
	
	Redraw();
}

void TimeLineWidget::DelKey(int _line, int _key)
{
	if (_line == -1 || _key == -1) return;

	Line* line = &lines[_line];

	if (!line->keys[_key].needBlend)
	{
		if (_key>0)
		{
			line->keys[_key-1].needBlend = false;
		}
	}

	for (int j=_key;j<(int)line->keys.size()-1;j++)
	{
		line->keys[j] = line->keys[j+1];
	}

	_key = -1;
	line->keys.pop_back();
	
	Redraw();
}

void TimeLineWidget::SwitchKeyCut()
{
	if (selLine == -1 || selKey == -1) return;

	Line* line = &lines[selLine];
	
	line->keys[selKey].needBlend = !line->keys[selKey].needBlend;
	//((DataTrack*)line->ptr)->SetKeyBlend(line->keys[selKey].needBlend);
	
	Redraw();
}

int TimeLineWidget::FindLineIndex(void* ptr)
{
	for (int i=0;i<lines.size();i++)
	{
		if (lines[i].ptr == ptr) return i;
	}

	return -1;
}

void TimeLineWidget::Play()
{
	play = !play;
}

void TimeLineWidget::Update()
{
	if (play)
	{
		*curTime += core->GetDeltaTime();

		if (*curTime>*toTime - 0.001f)
		{
			*curTime = *fromTime - 0.001f;
		}

		char str[32];
		sprintf(str, "%4.2f", *curTime);
		edCurTime->SetText(str);

		Redraw();	
	}

	if (edCurTime)
	{
		if (EventsWidget::CheckEvent(edCurTime->GetID(), EventsWidget::textbox_change))
		{
			float val = atof(edCurTime->GetText());

			if (fabs(val - *curTime)>0.001f && !play)
			{
				SetCurTime(val);
			}
			else
			{
				char str[32];
				sprintf(str, "%4.2f", *curTime);
				edCurTime->SetText(str);
			}
		}
	}

	if (edFromTime)
	{
		if (EventsWidget::CheckEvent(edFromTime->GetID(), EventsWidget::textbox_change))
		{
			float val = atof(edFromTime->GetText());

			if (fabs(val - *fromTime)>0.001f && !play)
			{
				SetFromTime(val);
			}
			else
			{
				char str[32];
				sprintf(str, "%4.2f", *fromTime);
				edFromTime->SetText(str);
			}
		}
	}

	if (edToTime)
	{
		if (EventsWidget::CheckEvent(edToTime->GetID(), EventsWidget::textbox_change))
		{
			float val = atof(edToTime->GetText());

			if (fabs(val - *toTime)>0.001f && !play)
			{
				SetToTime(val);
			}
			else
			{
				char str[32];
				sprintf(str, "%4.2f", *toTime);
				edToTime->SetText(str);
			}
		}
	}

	if (horz_scroll)
	{
		if (EventsWidget::CheckEvent(horz_scroll->GetID(), EventsWidget::scrollbar_change))
		{
			Redraw();
		}
	}

	if (vert_scroll)
	{
		if (EventsWidget::CheckEvent(vert_scroll->GetID(), EventsWidget::scrollbar_change))
		{
			Redraw();
		}
	}
}

void TimeLineWidget::SetWidgets(float* cr_time, TextBoxWidget* tbCurTime,
								float* from_time, TextBoxWidget* tbFromTime,
								float* to_time, TextBoxWidget* tbToTime,
								ScrollBarWidget* h_scroll,ScrollBarWidget* v_scroll)
{
	curTime = cr_time;
	edCurTime = tbCurTime;	

	fromTime = from_time;
	edFromTime = tbFromTime;

	toTime = to_time;
	edToTime = tbToTime;

	horz_scroll = h_scroll;
	vert_scroll = v_scroll;
}

/*
TrackPlayer* WidgetTimeLine::GetSelTrackPlayer()
{
	if (selLine == -1) return NULL;
	
	int index = selLine;
		
	while (!lines[index].isSeparator)
	{
		index--;
	}
	
	return (TrackPlayer*)lines[index].ptr;
}

DataTrack* WidgetTimeLine::GetSelDataTrack()
{
	if (selLine == -1) return NULL;

	int index = selLine;

	while (lines[index].isSeparator)
	{
		return NULL;
	}

	return (DataTrack*)lines[index].ptr;
}*/

void TimeLineWidget::SetSelection(int new_line, int new_key)
{
	if (selLine != -1)
	{
		Line* ln = &lines[selLine];

		if (!ln->isSeparator)
		{
				
			//((DataTrack*)ln->ptr)->StopEditKey();					

		}
		else
		{					
			//((TrackPlayer*)ln->ptr)->StopEditProp();
		}
	}

	selLine = new_line;
	selKey = new_key;

	if (selLine == -1)
	{
		selKey = -1;
		return;
	}

	Line* line = &lines[selLine];

	if (!line->isSeparator)
	{
		if (selKey != -1)
		{
			//((DataTrack*)line->ptr)->StartEditKey(selKey, gizmo);			
		}
	}
	else
	{
		//((TrackPlayer*)line->ptr)->StartEditProp();
	}

	Redraw();

	//MainFrame::tools->cutscene->SetSelection(selLine);
	//captions->selLine = new_line;
	//captions->Refresh();
}

void TimeLineWidget::AddToMultiSelection(int new_line, int new_key)
{
	/*if (new_key == -1)
		return;

	if (new_line != selLine)
	{
		m_SelKeys.clear();
		selLine = new_line;
	}

	std::vector<int>::iterator itkey =  std::find(m_SelKeys.begin(), m_SelKeys.end(), new_key);

	if (itkey != m_SelKeys.end())
	{
		m_SelKeys.erase(itkey);
		if (m_SelKeys.size() == 1)
		{
			SetSelection(new_line, m_SelKeys[0]);
			m_SelKeys.clear();
		}

	}
	else 
	{
		if (selKey != -1)
			m_SelKeys.push_back(selKey);

		SetSelection(new_line, -1);
		m_SelKeys.push_back(new_key);
		captions->selLine = new_line;
	}*/


	//std::sort(m_SelKeys.begin(), m_SelKeys.end());

	//((DataTrack*)lines[selLine].ptr)->StartEditMultiKeys(m_SelKeys, manager->gizmo);
	//captions->Refresh();
}

void TimeLineWidget::ChangeAcitivity()
{
	//if (!GetSelTrackPlayer())
	//	return;

	//GetSelTrackPlayer()->SetActive( !GetSelTrackPlayer()->GetActive()); 
	//Refresh();
}

void TimeLineWidget::CheckHotKeys()
{
	/*if (GetKeyState(VK_CONTROL) < 0 )
	{
		if (selLine == -1 )
			return;

		Line* line = &lines[selLine];

		if (line->isSeparator)
			return;

		if	(GetKeyState('C') < 0 )
		{
			((DataTrack*)line->ptr)->CopyToBuffer();		
		}
		else 	if	(GetKeyState('V') < 0 )
		{
			if (selKey == -1)
			{
				MainFrame::tools->cutscene->OnKeyAdd(wxCommandEvent());
				if (selKey == -1)
					return;

				((DataTrack*)line->ptr)->PasteFromBuffer();	
				((DataTrack*)line->ptr)->StartEditKey(selKey, manager->gizmo);		
			}
			else
			{
				((DataTrack*)line->ptr)->PasteFromBuffer();	
				((DataTrack*)line->ptr)->StartEditKey(selKey, manager->gizmo);		
			}

		}
	}*/
}

void TimeLineWidget::UpdateHorizBar()
{
	int mx = (timeLenght * scale - 490) * 0.1f;
	if (mx<0) mx = 0;
	horz_scroll->SetLimit(1, mx);
}

void TimeLineWidget::UpdateVertBar()
{
	int mx = lines.size() - 12;
	if (mx<0) mx = 0;
	vert_scroll->SetLimit(1, mx);
}