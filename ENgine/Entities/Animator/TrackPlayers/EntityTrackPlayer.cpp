
#include "EntityTrackPlayer.h"

TRACKPLAYER(EntityTrackPlayer, "SceneEntity")

EntityTrackPlayer::EntityTrackPlayer() : TrackPlayer(), proxies(_FL_)
{
	target = null;	

#ifdef EDITOR
	parent_wgt = null;
	timeline = null;
#endif

	entity_type[0] = 0;
	proxy = null;

	proxies.Add("Camera", new CameraProxy());
	proxies.Add("Model", new ModelProxy());
	proxies.Add("GUILabel", new GUILabelProxy());
}

EntityTrackPlayer::~EntityTrackPlayer()
{
	Clear();
}

void EntityTrackPlayer::Init()
{
	PROP_REG( doll_name )
}

void EntityTrackPlayer::ReInit()
{
	SceneEntity* prev_doll = target;
	target = owner->Scene()->Find(doll_name);

	if (target != prev_doll)
	{

#ifdef EDITOR
		int index = timeline->FindLineIndex(this);

		for (int i=0;i<tracks.size();i++)
		{
			timeline->DelLine(index + tracks.size() - i);
		}
#endif

		Clear();

		if (target)
		{
			proxy = proxies.Get(target->Desc()->GetName());

			if (proxy)
			{
				String::Copy(entity_type, 64, target->Desc()->GetName());

				proxy->CreateTracks(this);
				proxy->TieValues(this);

#ifdef EDITOR
				InitControls(parent_wgt);

				int index = timeline->FindLineIndex(this);
				PrepareControls(timeline, index+1);

				for (int i=0;i<tracks.size();i++)
				{
					timeline->InsertLine(index + i + 1, tracks[i]->GetName(),tracks[i]);
				}
#endif
			}
		}
	}
}

void EntityTrackPlayer::PostLoadInit()
{
	target = owner->Scene()->Find(doll_name);

	if (target)
	{
		proxy = proxies.Get(target->Desc()->GetName());

		if (proxy)
		{
			proxy->TieValues(this);
		}
	}
}

#ifdef EDITOR

void EntityTrackPlayer::InitControls(BaseWidget* parent)
{		
	parent_wgt = parent;
	TrackPlayer::InitControls(parent_wgt);
}

void EntityTrackPlayer::PrepareControls(TimeLineWidget* tl, int line)
{
	timeline = tl;
	TrackPlayer::PrepareControls(timeline, line);
}

#endif

void EntityTrackPlayer::SetTime(float dt)
{	
	TrackPlayer::SetTime(dt);

	if (proxy)
	{
		proxy->UpdateEntity();
	}
}

void EntityTrackPlayer::LoadProperties(IStreamLoader* stream)
{
	PropertyHolder::LoadProperties(stream);
	stream->Read("EnType", entity_type, 64);

	proxy = proxies.Get(entity_type);

	if (proxy)
	{
		proxy->CreateTracks(this);
	}
}

void EntityTrackPlayer::SaveProperties(IStreamSaver* stream)
{
	PropertyHolder::SaveProperties(stream);
	stream->Write("EnType", entity_type);
}