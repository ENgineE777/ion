
#include "Vector4DataTrack.h"
#include "Vector4DataTrackWidgets.h"

Vector4DataTrack::Vector4DataTrack(TrackPlayer* own, const char* nm,
							   KeyFunc key_Func,
							   KeyFunc changeKey_Func):TemplDataTrack<Vector4DataTrackPoint>( own, nm, key_Func, changeKey_Func)
{
	def_val = 0.0f;
}

Vector4DataTrack::~Vector4DataTrack()
{
	points.clear();
}


void Vector4DataTrack::Load(StreamLoader* stream, int num)
{
	points.resize(num);

	for (int i=0;i<num;i++)
	{		
		if (!stream->EnterBlock("Vector4Data")) break;

		stream->ReadVector4("m_value",points[i].m_value);
	
		stream->ReadFloat("time",points[i].time);
		points[i].needBlend = true;
		stream->ReadBool("blend",points[i].needBlend);
		
		stream->LeaveBlock();
	}
}

void Vector4DataTrack::Save(StreamSaver* stream)
{	
	for (int i=0;i<points.size();i++)
	{		
		stream->MarkBeginBlock("Vector4Data");

		stream->SaveVector4("m_value",points[i].m_value);
		stream->SaveFloat("time",points[i].time);
		stream->SaveBool("blend",points[i].needBlend);
		
		stream->MarkEndBlock("Vector4Data");
	}
}

void Vector4DataTrack::CalcValue(int index, float blend)
{
	if (blend<-0.01f)
	{
		cur_val.m_value = points[index].m_value;
	}
	else
	{
		cur_val.m_value.Lerp(points[cur_index-1].m_value, points[cur_index].m_value, blend);
	}
}

#ifdef EDITOR

void Vector4DataTrack::SetDefaultKey(int index)
{
	points[index].m_value = def_val;
}

void Vector4DataTrack::InitControls(wxWindow* prop_dock)
{
	TemplDataTrack::InitControls(prop_dock);
	wigets = new Vector4DataTrackWidgets(prop_dock);
	wigets->InitControls();
	wigets->Hide();	
}

void Vector4DataTrack::StartEditKey(int key, Gizmo* gz)
{			
	TemplDataTrack::StartEditKey(key, gz);		
	
	if (edited_key!=-1)
	{
		wigets->StartEditKey(&points[edited_key].m_value, name);		
	}
}

void Vector4DataTrack::StopEditKey()
{
	TemplDataTrack::StopEditKey();
	wigets->Hide();
}
void Vector4DataTrack::OnClose()
{
	__super::OnClose();
	delete wigets;
	wigets = 0;
}	
#endif