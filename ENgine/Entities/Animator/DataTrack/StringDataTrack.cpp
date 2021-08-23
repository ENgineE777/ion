
#include "StringDataTrack.h"
#include "StringDataTrackWidgets.h"

StringDataTrack::StringDataTrack(TrackPlayer* own, const char* nm,
							     KeyFunc key_Func,
							     KeyFunc changeKey_Func, bool _blend /*= true*/):TemplDataTrack<TrackStringPoint>( own, nm, key_Func, changeKey_Func)
{	
	m_needBlendInitValue = _blend;
}

StringDataTrack::~StringDataTrack()
{
	points.clear();
}

void StringDataTrack::Load(StreamLoader* stream, int num)
{
	points.resize(num);

	for (int i=0;i<num;i++)
	{		
		if (!stream->EnterBlock("StringData")) break;

		stream->ReadString("val",points[i].val);
		stream->ReadFloat("time",points[i].time);
		
		stream->LeaveBlock();
	}
}

void StringDataTrack::Save(StreamSaver* stream)
{	
	for (int i=0;i<points.size();i++)
	{		
		stream->MarkBeginBlock("StringData");

		stream->SaveString("val",points[i].val);
		stream->SaveFloat("time",points[i].time);
		
		stream->MarkEndBlock("StringData");
	}
}

void StringDataTrack::CalcValue(int index, float blend)
{
	strcpy(cur_val.val, points[index].val);			
}

#ifdef EDITOR

void StringDataTrack::SetDefaultKey(int index)
{
	points[index].val[0] = 0;
}

void StringDataTrack::InitControls(wxWindow* prop_dock)
{
	TemplDataTrack::InitControls(prop_dock);

	wigets = new StringDataTrackWidgets(prop_dock);
	wigets->InitControls();
	wigets->Hide();	
}

void StringDataTrack::StartEditKey(int key, Gizmo* gz)
{			
	TemplDataTrack::StartEditKey(key, gz);		

	if (edited_key!=-1)
	{
		wigets->StartEditKey(points[edited_key].val, name);		
	}
}

void StringDataTrack::StopEditKey()
{
	TemplDataTrack::StopEditKey();
	wigets->Hide();
}

void StringDataTrack::OnClose()
{
	__super::OnClose();
	delete wigets;
	wigets = 0;
}

#endif