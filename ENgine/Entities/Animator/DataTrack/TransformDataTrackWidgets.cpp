

#ifdef EDITOR

#include "TransformDataTrackWidgets.h"
#include "TransformDataTrack.h"

#define ANW_XWGTLB  2000
#define ANW_XWGTTB  2001
#define ANW_YWGTLB  2002
#define ANW_YWGTTB  2003
#define ANW_ZWGTLB  2004
#define ANW_ZWGTTB  2005
#define ANW_GRABBTN 2006

TransformDataTrackWidgets::TransformDataTrackWidgets(TransformDataTrack* trck, int x, int y)
{	
	offset_x = x;
	offset_y = y;

	track = trck;	
}

void TransformDataTrackWidgets::InitControls(BaseWidget* parent)
{
	prop_xlabel = new LabelWidget(ANW_XWGTLB,parent, false, "x", offset_x + 10,offset_y,90,22, false);
	prop_xed = new TextBoxWidget(ANW_XWGTTB,parent, "0.0", offset_x + 105,offset_y,90,22, false);

	prop_ylabel = new LabelWidget(ANW_YWGTLB,parent, false, "y", offset_x + 10,offset_y + 30,90,22, false);
	prop_yed = new TextBoxWidget(ANW_YWGTTB,parent, "0.0", offset_x + 105,offset_y + 30,90,22, false);

	prop_zlabel = new LabelWidget(ANW_ZWGTLB,parent, false, "z", offset_x + 10,offset_y + 60,90,22, false);
	prop_zed = new TextBoxWidget(ANW_ZWGTTB,parent, "0.0", offset_x + 105,offset_y + 60,90,22, false);

	btn_grab = new ButtonWidget(ANW_GRABBTN,parent, "Grab", false, offset_x + 105,offset_y + 90,90,22, false);
}

void TransformDataTrackWidgets::StartEditKey()
{
	TransformTrackPoint* point = track->GetPoint();

	if (point)
	{
		char str[32];
		
		sprintf(str, "%4.3f", point->pos.x);
		prop_xed->SetText(str);

		sprintf(str, "%4.3f", point->pos.y);
		prop_yed->SetText(str);

		sprintf(str, "%4.3f", point->pos.z);
		prop_zed->SetText(str);		
	}

	Show(true);
}

void TransformDataTrackWidgets::StartEditMultiKeys()
{
	char str[32];

	sprintf(str, "%4.3f", 0.f);
	prop_xed->SetText(str);

	sprintf(str, "%4.3f", 0.f);
	prop_yed->SetText(str);

	sprintf(str, "%4.3f", 0.f);
	prop_zed->SetText(str);		

	Show(true);
}

bool TransformDataTrackWidgets::UpdateWidgets()
{
	UpdateVal(0, prop_xed);
	UpdateVal(1, prop_yed);
	UpdateVal(2, prop_zed);

	if (EventsWidget::CheckEvent(ANW_GRABBTN, EventsWidget::button_click))
	{
		GrabKey();
	}

	return false;
}

void TransformDataTrackWidgets::StopEditKey()
{
	Show(false);
}

void TransformDataTrackWidgets::UpdateVal(int type, TextBoxWidget* box)
{
	if (EventsWidget::CheckEvent(ANW_XWGTTB + type * 2, EventsWidget::textbox_change))
	{
		TransformTrackPoint* point = (TransformTrackPoint*)track->GetEditedPoint();

		if (point)
		{
			float value = 0;		

			if (type == 0)
			{
				value = point->pos.x;
			}
			else
			if (type == 1)
			{
				value = point->pos.y;
			}
			else
			if (type == 2)
			{
				value = point->pos.z;
			}

			float val = atof(box->GetText());

			if (fabs(val - value)>0.001f)
			{
				value = val;
			}
			else
			{
				char str[32];
				sprintf(str, "%4.3f", val);
				box->SetText(str);
			}

			track->UpdateValue(type, value);
		}
	}
}

void TransformDataTrackWidgets::GrabKey()
{
	if (track)
	{		
		if (track->edited_key != -1)
		{
			track->SetDefaultKey(track->edited_key);
			track->DataToGizmo();
			track->Prepare();
		}
	}
}

void TransformDataTrackWidgets::Show(bool show)
{
	prop_xlabel->Show(show);
	prop_xed->Show(show);

	prop_ylabel->Show(show);
	prop_yed->Show(show);

	prop_zlabel->Show(show);
	prop_zed->Show(show);

	btn_grab->Show(show);
}

#endif