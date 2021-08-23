
#ifdef EDITOR

#include "AnimatorWindow.h"
#include "Animator.h"

#define ANW_WINDOW    1000
#define ANW_ADDCSCB   1001
#define ANW_DELCSBTN  1002
#define ANW_ADDKEYBTN 1003
#define ANW_DELKEYBTN 1004
#define ANW_BLENDBTN  1005
#define ANW_PLAYBTN   1006
#define ANW_CURTLB    1007
#define ANW_CURTTB    1008
#define ANW_TLLB      1009
#define ANW_TLTB      1010
#define ANW_FTLB      1011
#define ANW_FTTB      1012
#define ANW_TTLB      1013
#define ANW_TTTB      1014
#define ANW_TIMELINE  1015
#define ANW_HZSCROLL  1016
#define ANW_VRSCROLL  1017
#define ANW_PROPDOCK  1018

AnimatorWindow* AnimatorWindow::instance = null;

void AnimatorWindow::Init()
{	
	cur_time = 0.0f;
	time_offset = 0.0f;

	owner = null;

	num_buttons = 0;
	wnd = new WindowWidget(ANW_WINDOW,"Animator", true, false, 10,35,940,640);
	
	cb_addObject = new ComboBoxWidget(ANW_ADDCSCB,wnd, "", 10,10,110,95, false, false);
	cb_addObject->Show(true);

	for (TrackPlayerDeclBase* decl = TrackPlayerDeclBase::GetFirst(); decl != NULL; decl = decl->GetNext())
	{
		cb_addObject->AddString(decl->Name());
	}

	btn_delObject = new ButtonWidget(ANW_DELCSBTN,wnd, false, "Del", 125,10,35,22, false);
	btn_delObject->Show(true);

	btn_addKey = new ButtonWidget(ANW_ADDKEYBTN,wnd, false, "Add Key", 165,10,60,22, false);
	btn_addKey->Show(true);

	btn_delKey = new ButtonWidget(ANW_DELKEYBTN,wnd, false, "Del Key", 230,10,60,22, false);
	btn_delKey->Show(true);

	btn_blendKey = new ButtonWidget(ANW_BLENDBTN,wnd, false, "Blend", 295,10,60,22, false);
	btn_blendKey->Show(true);

	btn_playKey = new ButtonWidget(ANW_PLAYBTN,wnd, false, "Play", 360,10,65,22, false);
	btn_playKey->Show(true);

	lb_curTime = new LabelWidget(ANW_CURTLB,wnd, "Cur Time", false, 445,10,60,22, false);
	lb_curTime->Show(true);	

	tb_curTime = new TextBoxWidget(ANW_CURTTB,wnd, "0.0", 505,10,60,22, false);
	tb_curTime->Show(true);

	lb_timeLenght = new LabelWidget(ANW_TLLB,wnd, "Time Length", false, 570,10,80,22,false);
	lb_timeLenght->Show(true);

	tb_timeLenght = new TextBoxWidget(ANW_TLTB,wnd, "0.0", 650,10,60,22, false);
	tb_timeLenght->Show(true);

	lb_fromTime = new LabelWidget(ANW_TTLB,wnd, "From", false, 730,10,50,22, false);
	lb_fromTime->Show(true);
	
	tb_fromTime = new TextBoxWidget(ANW_TTTB,wnd, "0.0", 780,10,60,22, false);
	tb_fromTime->Show(true);

	lb_toTime = new LabelWidget(ANW_FTLB,wnd, "To", false, 840,10,30,22, false);
	lb_toTime->Show(true);

	tb_toTime = new TextBoxWidget(ANW_FTTB,wnd, "0.0", 870,10,60,22, false);
	tb_toTime->Show(true);

	timeline =  new TimeLineWidget(ANW_TIMELINE, wnd, 10,35,700,270);
	timeline->Show(true);

	horz_scroll = new ScrollBarWidget(ANW_HZSCROLL, wnd, true, 160,305,550,15, false);
	horz_scroll->Show(true);

	vert_scroll = new ScrollBarWidget(ANW_VRSCROLL, wnd, false, 710,65,15,240, false);
	vert_scroll->Show(true);

	prop_dock = new LabelWidget(ANW_PROPDOCK, wnd, "", false, 730,35,200,500, false);
	prop_dock->Show(true);
}

void AnimatorWindow::Show(bool sh)
{
	wnd->Show(sh);
}

void AnimatorWindow::CreateAnimatorWidgets(Animator* anim)
{
	Clear();

	owner = anim;

	timeline->SetWidgets(&owner->ed_cur_time, tb_curTime,
						 &owner->from_time, tb_fromTime,
						 &owner->to_time, tb_toTime,
						 horz_scroll, vert_scroll);
	


	if (!owner) return;

	char txt[128];
	String::Printf(txt, 128, "%4.2f", owner->timeLenght);

	tb_timeLenght->SetText(txt);

	timeline->SetTimeLenght(owner->timeLenght);
	timeline->SetFromTime(owner->from_time);
	timeline->SetToTime(owner->to_time);

	int index = 0;

	for (int i=0;i<owner->players.size();i++)
	{
		owner->players[i]->InitControls(prop_dock);

		timeline->AddLine(owner->players[i]->GetName(), owner->players[i], true);
		index++;

		for (int j=0;j<owner->players[i]->tracks.size();j++)
		{
			timeline->AddLine(owner->players[i]->tracks[j]->GetName(), owner->players[i]->tracks[j]);
		}

		owner->players[i]->PrepareControls(timeline, index);

		index += owner->players[i]->tracks.size();
	}

	UpdateWidgets();
}

void AnimatorWindow::Update()
{
	timeline->Update();

	if (EventsWidget::CheckEvent(ANW_ADDCSCB, EventsWidget::combobox_change))
	{
		TrackPlayer* player = TrackPlayerDeclBase::Create(cb_addObject->GetText());

		if (player)
		{
			player->SetOwner(owner);
			player->SetName(cb_addObject->GetText());
			player->SetType(cb_addObject->GetText());

			player->Init();
			player->InitControls(prop_dock);
			
			timeline->AddLine(player->GetName(), player, true);

			for (int j=0;j<player->tracks.size();j++)
			{
				timeline->AddLine(player->tracks[j]->GetName(), player->tracks[j]);
			}

			player->PrepareControls(timeline, timeline->FindLineIndex(player) + 1);

			owner->players.push_back(player);
		}

		cb_addObject->SetCurString(-1);
	}

	if (EventsWidget::CheckEvent(ANW_DELCSBTN, EventsWidget::button_click))
	{
		void* ptr = timeline->GetSelLineOwner();

		if (ptr)
		{
			timeline->SetSelection(-1,-1);

			TrackPlayer* player = (TrackPlayer*)ptr;

			int index = timeline->FindLineIndex(player);

			for (int i=0;i<player->tracks.size();i++)
			{
				timeline->DelLine(index + player->tracks.size() - i);
			}

			timeline->DelLine(index);

			for (int i=0;i<owner->players.size();i++)
			{
				if (owner->players[i] == player)
				{
					owner->players.erase(owner->players.begin() + i);
					break;
				}
			}
		}
	}

	if (EventsWidget::CheckEvent(ANW_ADDKEYBTN, EventsWidget::button_click))
	{
		timeline->AddKey();
	}

	if (EventsWidget::CheckEvent(ANW_DELKEYBTN, EventsWidget::button_click))
	{
		timeline->DelKey();
	}

	if (EventsWidget::CheckEvent(ANW_BLENDBTN, EventsWidget::button_click))
	{
		timeline->SwitchKeyCut();
	}

	if (EventsWidget::CheckEvent(ANW_PLAYBTN, EventsWidget::button_click))
	{
		timeline->Play();
	}

	if (EventsWidget::CheckEvent(tb_timeLenght->GetID(), EventsWidget::textbox_change))
	{
		float val = atof(tb_timeLenght->GetText());

		if (fabs(val - owner->timeLenght)>0.001f)
		{
			owner->timeLenght = val;
			owner->ReverseTieValues();

			timeline->SetTimeLenght(owner->timeLenght);
			timeline->SetToTime(owner->timeLenght);
		}
		else
		{
			char str[32];
			sprintf(str, "%4.2f", owner->timeLenght);
			tb_timeLenght->SetText(str);
		}
	}

	if (owner)
	{
		void* ptr = timeline->GetSelLineOwner();

		if (ptr)
		{			
			TrackPlayer* player = (TrackPlayer*)ptr;			
			player->UpdateWidgets();
		}

		for (int i=0;i<owner->players.size();i++)
		{
			owner->players[i]->UpdateWidgets();
			owner->players[i]->Reset();
			owner->players[i]->SetTime(owner->ed_cur_time);

			owner->players[i]->DataFromGizmo();
		}
	}
}

void AnimatorWindow::UpdateWidgets()
{
	char txt[128];
	String::Printf(txt, 128, "%4.2f", cur_time);

	tb_curTime->SetText(txt);
}

void AnimatorWindow::Clear()
{
	timeline->SetSelection(-1,-1);
	timeline->Clear();
	cur_time = 0.0f;
	time_offset = 0.0f;
}

void AnimatorWindow::SetGizmo(Gizmo* gizmo)
{
	timeline->gizmo = gizmo;
}

#endif