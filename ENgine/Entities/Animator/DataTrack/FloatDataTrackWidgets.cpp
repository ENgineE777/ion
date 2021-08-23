
#ifdef EDITOR

#include "FloatDataTrackWidgets.h"

#define ANW_FLTWGTLB  2000
#define ANW_FLTWGTTB  2001

FloatDataTrackWidgets::FloatDataTrackWidgets(int x, int y)
{
	offset_x = x;
	offset_y = y;
	value = NULL;
}

void FloatDataTrackWidgets::InitControls(BaseWidget* parent)
{
	prop_label = new LabelWidget(ANW_FLTWGTLB,parent, "Value", false, offset_x + 10,offset_y,90,22, false);
	prop_label->Show(false);

	prop_ed = new TextBoxWidget(ANW_FLTWGTTB,parent, "0.0", offset_x + 105,offset_y,90,22, false);
	prop_ed->Show(false);
}

void FloatDataTrackWidgets::StartEditKey(float* vl, const char* name)
{
	value = vl;
	prop_label->SetText(name);

	char str[32];
	sprintf(str, "%4.1f", *value);
	prop_ed->SetText(str);

	prop_ed->Show(true);
	prop_label->Show(true);
}

void FloatDataTrackWidgets::StopEditKey()
{
	prop_ed->Show(false);
	prop_label->Show(false);
}

void FloatDataTrackWidgets::UpdateWidgets()
{			
	if (EventsWidget::CheckEvent(ANW_FLTWGTTB, EventsWidget::textbox_change))
	{
		float val = *value;
		val = atof(prop_ed->GetText());

		if (fabs(val - *value)>0.001f)
		{
			*value = val;
		}
		else
		{
			char str[32];
			sprintf(str, "%4.3f", *value);
			prop_ed->SetText(str);
		}
	}
}

#endif