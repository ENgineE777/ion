

#ifdef EDITOR

#include "Transform2DDataTrackWidgets.h"
#include "Transform2DDataTrack.h"

#define T2D_X_LB  2010
#define T2D_X_ED  2011
#define T2D_Y_LB  2012
#define T2D_Y_ED  2013
#define T2D_SZX_LB  2014
#define T2D_SZX_ED  2015
#define T2D_SZY_LB  2016
#define T2D_SZY_ED  2017

Transform2DDataTrackWidgets::Transform2DDataTrackWidgets(Transform2DDataTrack* trck, int x, int y)
{	
	offset_x = x;
	offset_y = y;

	track = trck;	
}

void Transform2DDataTrackWidgets::InitControls(BaseWidget* parent)
{
	prop_xlabel = new LabelWidget(T2D_X_LB,parent, "x", false, offset_x + 10,offset_y,90,22, false);
	prop_xlabel->Show(false);
	prop_xed = new TextBoxWidget(T2D_X_ED,parent, "0.0", offset_x + 105,offset_y,90,22, false);
	prop_xed->Show(false);

	prop_ylabel = new LabelWidget(T2D_Y_LB,parent, "y",false,  offset_x + 10,offset_y + 30,90,22, false);
	prop_ylabel->Show(false);
	prop_yed = new TextBoxWidget(T2D_Y_ED,parent, "0.0", offset_x + 105,offset_y + 30,90,22, false);
	prop_yed->Show(false);

	prop_sz_xlabel = new LabelWidget(T2D_SZX_LB,parent, "sz_x", false, offset_x + 10,offset_y + 60,90,22, false);
	prop_sz_xlabel->Show(false);
	prop_sz_xed = new TextBoxWidget(T2D_SZX_ED,parent, "0.0", offset_x + 105,offset_y + 60,90,22, false);
	prop_sz_xed->Show(false);

	prop_sz_ylabel = new LabelWidget(T2D_SZX_LB,parent, "sz_y", false, offset_x + 10,offset_y + 90,90,22, false);
	prop_sz_ylabel->Show(false);
	prop_sz_yed = new TextBoxWidget(T2D_SZY_ED,parent, "0.0", offset_x + 105,offset_y + 90,90,22, false);
	prop_sz_yed->Show(false);
}

void Transform2DDataTrackWidgets::StartEditKey()
{
	Transform2DTrackPoint* point = track->GetPoint();

	if (point)
	{
		char str[32];
		
		sprintf(str, "%4.3f", point->pos.x);
		prop_xed->SetText(str);

		sprintf(str, "%4.3f", point->pos.y);
		prop_yed->SetText(str);	

		sprintf(str, "%4.3f", point->size.x);
		prop_sz_xed->SetText(str);	

		sprintf(str, "%4.3f", point->size.y);
		prop_sz_yed->SetText(str);	
	}

	Show(true);
}

bool Transform2DDataTrackWidgets::UpdateWidgets()
{
	UpdateVal(0, prop_xed);
	UpdateVal(1, prop_yed);
	
	UpdateVal(2, prop_sz_xed);
	UpdateVal(3, prop_sz_yed);

	return false;
}

void Transform2DDataTrackWidgets::StopEditKey()
{
	Show(false);
}

void Transform2DDataTrackWidgets::UpdateVal(int type, TextBoxWidget* box)
{
	if (EventsWidget::CheckEvent(T2D_X_ED + type * 2, EventsWidget::textbox_change))
	{
		Transform2DTrackPoint* point = (Transform2DTrackPoint*)track->GetEditedPoint();

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
				value = point->size.x;
			}
			else
			if (type == 3)
			{
				value = point->size.y;
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

void Transform2DDataTrackWidgets::Show(bool show)
{
	prop_xlabel->Show(show);
	prop_xed->Show(show);

	prop_ylabel->Show(show);
	prop_yed->Show(show);

	prop_sz_xlabel->Show(show);
	prop_sz_xed->Show(show);

	prop_sz_ylabel->Show(show);
	prop_sz_yed->Show(show);
}

#endif