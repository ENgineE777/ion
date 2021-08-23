
#include "FloatDataTrack.h"
#include "FloatDataTrackWidgets.h"
#include "../AnimatorWindow.h"

FloatDataTrack::FloatDataTrack(TrackPlayer* own, const char* nm,
							   KeyFunc key_Func,
							   KeyFunc changeKey_Func):TemplDataTrack<TrackFloatPoint>( own, nm, key_Func, changeKey_Func)
{
	def_val = 0.0f;
	target = null;
}

FloatDataTrack::~FloatDataTrack()
{
	points.clear();
}

void FloatDataTrack::Load(IStreamLoader* stream, int num)
{
	points.resize(num);

	for (int i=0;i<num;i++)
	{		
		if (!stream->EnterBlock("FloatData")) break;

		stream->Read("val",points[i].val);
		stream->Read("time",points[i].time);

		points[i].needBlend = true;
		stream->Read("blend",points[i].needBlend);
		
		stream->LeaveBlock();
	}
}

void FloatDataTrack::Save(IStreamSaver* stream)
{	
	for (int i=0;i<points.size();i++)
	{		
		stream->MarkBeginBlock("FloatData");

		stream->Write("val",points[i].val);
		stream->Write("time",points[i].time);
		stream->Write("blend",points[i].needBlend);
		
		stream->MarkEndBlock("FloatData");
	}
}

void FloatDataTrack::CalcValue(int index, float blend)
{
	if (blend<-0.01f)
	{
		cur_val.val = points[index].val;
	}
	else
	{
		cur_val.val = points[cur_index-1].val + (points[cur_index].val - points[cur_index-1].val) * blend;
	}

	if (target)
	{
		*target = cur_val.val;
	}
}

void FloatDataTrack::SetControlledValue(void* value)
{
	target = (float*)value;
}

#ifdef EDITOR

void FloatDataTrack::SetDefaultKey(int index)
{
	points[index].val = def_val;
}

void FloatDataTrack::InitControls(BaseWidget* parent)
{
	TemplDataTrack::InitControls(parent);
	wigets = new FloatDataTrackWidgets(0, 35);
	wigets->InitControls(parent);
}

void FloatDataTrack::StartEditKey(int key, Gizmo* gz)
{			
	TemplDataTrack::StartEditKey(key, gz);		
	
	if (edited_key!=-1)
	{
		wigets->StartEditKey(&points[edited_key].val, name);		
	}
}

bool FloatDataTrack::UpdateWidgets()
{
	if (edited_key!=-1)
	{
		wigets->UpdateWidgets();
	}

	return false;
}

void FloatDataTrack::StopEditKey()
{
	TemplDataTrack::StopEditKey();
	wigets->StopEditKey();
}
void FloatDataTrack::OnClose()
{
	TemplDataTrack::OnClose();
	delete wigets;
	wigets = 0;
}	
#endif