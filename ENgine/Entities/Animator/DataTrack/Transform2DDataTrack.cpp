
#include "Transform2DDataTrack.h"
#include "Transform2DDataTrackWidgets.h"
#include "Core/ICore.h"

Transform2DDataTrack::Transform2DDataTrack(TrackPlayer* own, const char* nm,
									   KeyFunc key_Func, ChangeKeyFunc changeKey_Func,  Vector _defaultPos)
										 : TemplDataTrack<Transform2DTrackPoint>(own, nm, key_Func, changeKey_Func)
{			
#ifdef EDITOR
	wigets = NULL;	
#endif	
}

Transform2DDataTrack::~Transform2DDataTrack()
{
	points.clear();	
}


void Transform2DDataTrack::Load(IStreamLoader* stream, int num)
{
	points.resize(num);

	for (int i=0;i<num;i++)
	{		
		if (!stream->EnterBlock("TransformData")) break;

		stream->Read("x",points[i].pos.x);
		stream->Read("y",points[i].pos.y);	

		stream->Read("sz_x",points[i].size.x);
		stream->Read("sz_y",points[i].size.y);		

		stream->Read("time",points[i].time);

		points[i].needBlend = true;
		stream->Read("blend",points[i].needBlend);
		
		stream->LeaveBlock();
	}	
}

void Transform2DDataTrack::Save(IStreamSaver* stream)
{	
	for (int i=0;i<points.size();i++)
	{		
		stream->MarkBeginBlock("TransformData");

		stream->Write("x",points[i].pos.x);
		stream->Write("y",points[i].pos.y);	

		stream->Write("sz_x",points[i].size.x);
		stream->Write("sz_y",points[i].size.y);		

		stream->Write("time",points[i].time);
		stream->Write("blend",points[i].needBlend);
		
		stream->MarkEndBlock("TransformData");
	}
}

void Transform2DDataTrack::CalcValue(int index, float blend)
{
	if (blend<-0.01f)
	{
		cur_val.pos = points[index].pos;
		cur_val.size = points[index].size;
	}
	else
	{		
		cur_val.pos = points[cur_index-1].pos + (points[cur_index].pos - points[cur_index-1].pos) * blend;
		cur_val.size = points[cur_index-1].size + (points[cur_index].size - points[cur_index-1].size) * blend;		  		
	}	

	/*if (doll)
	{
		doll->x = cur_val.pos.x;
		doll->y = cur_val.pos.y;

		doll->width = cur_val.size.x;
		doll->height = cur_val.size.y;		

		doll->UpdateChildsPos();
	}*/
}

void Transform2DDataTrack::SetControlledValue(void* value)
{
	//doll = (GUIWidget*)value;
}

#ifdef EDITOR

void Transform2DDataTrack::SetDefaultKey(int index)
{
	points[index].pos = 0.0f;
	points[index].size = 100.0f;	
}

void Transform2DDataTrack::InitControls(BaseWidget* parent)
{
	TemplDataTrack::InitControls(parent);
	wigets = new Transform2DDataTrackWidgets(this, 0, 35); 
	wigets->InitControls(parent);
}

void Transform2DDataTrack::StartEditKey(int key, Gizmo* gz)
{
	TemplDataTrack::StartEditKey(key, gz);
	
	if (edited_key!=-1)
	{
		wigets->StartEditKey();		
	}

	gz->type = 1;
	DataToGizmo();
}

bool Transform2DDataTrack::UpdateWidgets()
{
	if (edited_key!=-1)
	{
		wigets->UpdateWidgets();
	}

	return false;
}

void Transform2DDataTrack::StopEditMultiKeys()
{
	TemplDataTrack::StopEditMultiKeys();
	
	wigets->StopEditKey();

	if (gizmo) gizmo->enabled = false;
}

void Transform2DDataTrack::DataToGizmo()
{
	if (edited_key != -1 && gizmo)
	{		
		gizmo->transform._11 = points[edited_key].size.x;
		gizmo->transform._22 = points[edited_key].size.y;	

		gizmo->transform._41 = points[edited_key].pos.x;
		gizmo->transform._42 = points[edited_key].pos.y;
			
		gizmo->type = 1;
		gizmo->enabled = true;				
	}
}


void Transform2DDataTrack::DataToGizmoMultiSelect()
{
	if (gizmo)
	{
	}
}
void Transform2DDataTrack::DataFromGizmo()
{
	if (edited_key != -1 && gizmo)
	{		
		if (edited_key!=-1)
		{
			if (fabs(points[edited_key].size.x - gizmo->transform._11)>0.0001f ||
				fabs(points[edited_key].size.y - gizmo->transform._22)>0.0001f ||
				fabs(points[edited_key].pos.x - gizmo->transform._41)>0.0001f ||
				fabs(points[edited_key].pos.y - gizmo->transform._42)>0.0001f)
			{
				wigets->StartEditKey();
			}
		}

		points[edited_key].pos.x = gizmo->transform._41;
		points[edited_key].pos.y = gizmo->transform._42;	

		points[edited_key].size.x = gizmo->transform._11;
		points[edited_key].size.y = gizmo->transform._22;		
	}
	else if (!m_EditedKeys.empty())
	{		
	}
}

void Transform2DDataTrack::StopEditKey()
{
	if (gizmo)
	{
		gizmo->type = 0;
		gizmo->enabled = false;
	}

	TemplDataTrack::StopEditKey();

	if (wigets)	wigets->StopEditKey();
}

void Transform2DDataTrack::OnClose()
{
	TemplDataTrack::OnClose();
	delete wigets;
	wigets = 0;
}

Transform2DTrackPoint* Transform2DDataTrack::GetPoint()
{
	if (edited_key != -1)
	{
		return &points[edited_key];
	}

	return NULL;
}

void Transform2DDataTrack::CheckHotKeys()
{
	if (GetKeyState('G') < 0)
	{
		//wigets->GrabKey();
	}
	else
	if (GetKeyState('H') < 0)
	{
		/*if (edited_key != -1)
		{
			points[edited_key].rot = Quaternion(0, 0, 0, 1);
			points[edited_key].pos = Vector(0, 0, 0);
			Prepare();
			DataToGizmo();
			wigets->StartEditKey();
		}*/
	}
}

void Transform2DDataTrack::UpdateValue(int type, float value)
{
	if (edited_key == -1)
	{
		dword size = m_EditedKeys.size();
		for (dword i = 0; i < size; ++i)
		{
			if (type == 0)
			{			
				points[m_EditedKeys[i]].pos.x += value;
			}
			else
			if (type == 1)
			{
				points[m_EditedKeys[i]].pos.y += value;
			}
			else
			if (type == 2)
			{
				points[m_EditedKeys[i]].size.x += value;
			}
			else
			if (type == 3)
			{
				points[m_EditedKeys[i]].size.y += value;
			}
		}
	}
	else
	{
		if (type == 0)
		{			
			points[edited_key].pos.x = value;
		}
		else
		if (type == 1)
		{
			points[edited_key].pos.y = value;
		}
		else
		if (type == 2)
		{			
			points[edited_key].size.x = value;
		}
		else
		if (type == 3)
		{
			points[edited_key].size.y = value;
		}

		DataToGizmo();
	}
}

#endif

