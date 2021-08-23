
#include "BoolDataTrack.h"
#include "BoolDataTrackWidgets.h"

BoolDataTrack::BoolDataTrack(TrackPlayer* own, const char* nm,
							   KeyFunc key_Func,
							   KeyFunc changeKey_Func,
								 bool _blend):TemplDataTrack<BoolDataTrackPoint>( own, nm, key_Func, changeKey_Func, _blend)
{
	def_val = 0.0f;
}

BoolDataTrack::~BoolDataTrack()
{
	points.clear();
}

void BoolDataTrack::Load(StreamLoader* stream, int num)
{
	points.resize(num);

	for (int i=0;i<num;i++)
	{		
		if (!stream->EnterBlock("BoolDataTrackData")) break;

		stream->ReadBool("m_value",points[i].m_value);
	
		stream->ReadFloat("time",points[i].time);
		points[i].needBlend = true;
		stream->ReadBool("blend",points[i].needBlend);
		
		stream->LeaveBlock();
	}
}

void BoolDataTrack::Save(StreamSaver* stream)
{	
	for (int i=0;i<points.size();i++)
	{		
		stream->MarkBeginBlock("BoolDataTrackData");

		stream->SaveBool("m_value",points[i].m_value);
		stream->SaveFloat("time",points[i].time);
		stream->SaveBool("blend",points[i].needBlend);
		
		stream->MarkEndBlock("BoolDataTrackData");
	}
}

void BoolDataTrack::CalcValue(int index, float blend)
{
	cur_val.m_value = points[index].m_value;
}

#ifdef EDITOR

void BoolDataTrack::SetDefaultKey(int index)
{
	points[index].m_value = def_val;
}

void BoolDataTrack::InitControls(wxWindow* prop_dock)
{
	TemplDataTrack::InitControls(prop_dock);
	wigets = new BoolDataTrackWidgets( this, prop_dock );
	wigets->InitControls();
	wigets->Hide();	
}

void BoolDataTrack::StartEditKey(int key, Gizmo* gz)
{			
	TemplDataTrack::StartEditKey(key, gz);		
	
	if (edited_key!=-1)
	{
		wigets->StartEditKey(name);		
	}
}

void BoolDataTrack::StopEditKey()
{
	TemplDataTrack::StopEditKey();
	wigets->Hide();
}
void BoolDataTrack::OnClose()
{
	__super::OnClose();
	delete wigets;
	wigets = 0;
}	
#endif