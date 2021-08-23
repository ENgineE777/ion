
#include "TrackPlayer.h"

TrackPlayerDeclBase* TrackPlayerDeclBase::first = null;

TrackPlayer::TrackPlayer()
{
	active = true;
	owner = null;
	name[0] = 0;
}

TrackPlayer::~TrackPlayer()
{
	Clear();
}

void TrackPlayer::SetOwner(SceneEntity* own)
{
	owner = own;
}

void TrackPlayer::Clear()
{
	for (int i=0; i<tracks.size();i++)
	{
		delete tracks[i];
	}

	tracks.clear();
}

#ifdef EDITOR

void TrackPlayer::InitControls(BaseWidget* parent)
{		
	CreateWidgets(parent);

	for (int i=0; i<tracks.size();i++)
	{
		tracks[i]->InitControls(parent);
	}
}

void TrackPlayer::PrepareControls(TimeLineWidget* timeline, int line)
{
	for (int i=0; i<tracks.size();i++)
	{
		tracks[i]->PrepareControls(timeline, line + i);
	}
}

void TrackPlayer::StartEditProp()
{	
	ShowWidgets(true);
}

bool TrackPlayer::UpdateWidgets()
{
	PropertyHolder::UpdateWidgets();

	bool res = false;
	for (int i=0; i<tracks.size();i++)
	{
		res |= tracks[i]->UpdateWidgets();
	}

	return res;
}

void TrackPlayer::StopEditProp()
{	
	ShowWidgets(false);
}

void TrackPlayer::DataFromGizmo()
{
	for (int i=0; i<tracks.size();i++)
	{
		tracks[i]->DataFromGizmo();
	}
}

void TrackPlayer::DebugDraw()
{
	for (int i=0; i<tracks.size();i++)
	{
		tracks[i]->DebugDraw();
	}
}

void TrackPlayer::OnClose()
{
	for (int i=0; i<tracks.size();i++)
	{
		tracks[i]->OnClose();
	}
}

void TrackPlayer::OnStopEdit()
{
}

void TrackPlayer::CheckHotKeys()
{
	/*if (!MainFrame::tools)
		return;

	for (int i=0; i<tracks.size();i++)
	{
		if (MainFrame::tools->cutscene->timeLine->GetSelDataTrack() == tracks[i])
			tracks[i]->CheckHotKeys();
	}*/
}



#endif



void TrackPlayer::Load(IStreamLoader* stream)
{
	stream->Read("Name", name, 64);
	stream->Read("Active", active);
	
	LoadProperties(stream);

	int count = 0;
	stream->Read("Count", count);

	for (int i=0; i<count;i++)
	{
		if (!stream->EnterBlock("DataTrack")) break;

		int num_keys = 0;
		stream->Read("Count", num_keys);

		char trackName[64];
		stream->Read("Name", trackName, 64);

		tracks[i]->Load(stream, num_keys);		

		stream->LeaveBlock();
	}
}

void TrackPlayer::ReInit()
{
}

void TrackPlayer::OnPropertiesChanged()
{
	ReInit();
}

void TrackPlayer::LoadProperties(IStreamLoader* stream)
{
	PropertyHolder::LoadProperties(stream);
	ReInit();
}

void TrackPlayer::Save(IStreamSaver* stream)
{
	stream->Write("Type", type);
	stream->Write("Name", name);
	stream->Write("Active", active);

	SaveProperties(stream);

	int count = tracks.size();
	stream->Write("Count", count);

	for (int i=0; i<count;i++)
	{
		stream->MarkBeginBlock("DataTrack");

		int num_keys = tracks[i]->GetKeysCount();
		stream->Write("Count", num_keys);

		char trackName[64];
		stream->Write("Name", tracks[i]->GetName());

		tracks[i]->Save(stream);

		stream->MarkEndBlock("DataTrack");
	}
}

void TrackPlayer::SaveProperties(IStreamSaver* stream)
{
	PropertyHolder::SaveProperties(stream);
}

void TrackPlayer::Reset()
{	
	for (int i=0; i<tracks.size();i++)
	{
		tracks[i]->Reset();
	}
}

void TrackPlayer::SetTime(float time)
{	
	if (!owner->Scene()->IsRunning())
	{
		if (!IsActive()) return;
	}

	for (int i=0; i<tracks.size();i++)
	{
		tracks[i]->SetTime(time);

		/*
#ifdef EDITOR
		tracks[i]->SetEditorPlayBack(CWorld::instance()->getUpdateScene()->getCutSceneManager()->ed_playBack);
		tracks[i]->SetEditorPlay(CWorld::instance()->getUpdateScene()->getCutSceneManager()->ed_played);
#endif*/
		
	}
}

void TrackPlayer::PostLoadInit()
{

}

void TrackPlayer::Release()
{		
	Clear();

	delete this;
}

void TrackPlayer::SetActive(bool value)
{
	active = value;
}

DataTrack* TrackPlayer::GetDataTrack( const char* _name )
{
	dword size = tracks.size();
	for (dword i = 0; i < size; ++i)
	{
		if (!strcmp(tracks[i]->GetName(), _name))
			return tracks[i];
	}

	return NULL;
}