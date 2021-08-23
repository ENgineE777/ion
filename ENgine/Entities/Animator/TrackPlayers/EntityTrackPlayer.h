#pragma once

#include "Core/Scene/SceneEntity.h"
#include "TrackPlayer.h"
#include "Common/Containers/HashMap.h"

class EntityTrackPlayer : public TrackPlayer
{
	SceneEntity* target;
	
#ifdef EDITOR
	BaseWidget* parent_wgt;
	TimeLineWidget* timeline;
#endif

	class EntityProxy
	{		
	public:

		virtual void CreateTracks(EntityTrackPlayer* player) = 0;
		virtual void UpdateEntity() {};
		virtual void TieValues(EntityTrackPlayer* player) = 0;
	};

	class CameraProxy : public EntityProxy
	{
		TransformDataTrack* tr_track;
		FloatDataTrack* fl_track;

		virtual void CreateTracks(EntityTrackPlayer* player)
		{
			tr_track = new TransformDataTrack(player,"Transform",2.5f,false, null, null);
			player->tracks.push_back(tr_track);

			fl_track = new FloatDataTrack(player,"Fov",null,null);
			player->tracks.push_back(fl_track);
		};

		virtual void TieValues(EntityTrackPlayer* player)
		{
			tr_track->SetControlledValue(&player->target->transform);
			fl_track->def_val = player->target->prop[0]->GetAsFloat();
			fl_track->SetControlledValue(player->target->prop[0]->GetValue());
		}
	};

	class ModelProxy : public EntityProxy
	{
		TransformDataTrack* tr_track;

		virtual void CreateTracks(EntityTrackPlayer* player)
		{
			tr_track = new TransformDataTrack(player,"Transform",2.5f,false, null, null);
			player->tracks.push_back(tr_track);
		};

		virtual void TieValues(EntityTrackPlayer* player)
		{
			tr_track->SetControlledValue(&player->target->transform);
		}
	};

	class GUILabelProxy : public EntityProxy
	{
		EntityTrackPlayer* plr;
		Transform2DDataTrack* geom_track;		
		FloatDataTrack* alpha_track;
		FloatDataTrack* rotate_track;
		FloatDataTrack* font_sz_track;

		virtual void CreateTracks(EntityTrackPlayer* player)
		{
			plr = player;			

			geom_track = new Transform2DDataTrack(player,"Transform", null, null);
			player->tracks.push_back(geom_track);			

			alpha_track = new FloatDataTrack(player,"alpha",null,null);
			player->tracks.push_back(alpha_track);

			rotate_track = new FloatDataTrack(player,"rotation",null,null);
			player->tracks.push_back(rotate_track);

			font_sz_track = new FloatDataTrack(player,"font_scale",null,null);
			player->tracks.push_back(font_sz_track);
		};

		virtual void UpdateEntity()
		{
			/*GUIWidget* widget = (GUIWidget*)(plr->target);			

			if (widget)
			{
				widget->CalcColors();
			}*/
		};

		virtual void TieValues(EntityTrackPlayer* player)
		{
			geom_track->SetControlledValue(player->target);
			
			for (int i=0;i<player->target->prop.size();i++)
			{
				if (String::IsEqual(player->target->prop[i]->GetName(), "alpha"))
				{
					alpha_track->def_val = player->target->prop[i]->GetAsFloat();
					alpha_track->SetControlledValue(player->target->prop[i]->GetValue());
					break;
				}
			}

			
			for (int i=0;i<player->target->prop.size();i++)
			{
				if (String::IsEqual(player->target->prop[i]->GetName(), "rotation"))
				{
					rotate_track->def_val = player->target->prop[i]->GetAsFloat();
					rotate_track->SetControlledValue(player->target->prop[i]->GetValue());
					break;
				}
			}

			for (int i=0;i<player->target->prop.size();i++)
			{
				if (String::IsEqual(player->target->prop[i]->GetName(), "fontScale"))
				{
					font_sz_track->def_val = player->target->prop[i]->GetAsFloat();
					font_sz_track->SetControlledValue(player->target->prop[i]->GetValue());					
					break;
				}
			}						
		}
	};

	HashMap<EntityProxy> proxies;
	char entity_type[64];
	EntityProxy* proxy;

public:	

	STRING_PROP(EntityTrackPlayer, doll_name, "", "Properties", "Target")

	EntityTrackPlayer();

	virtual ~EntityTrackPlayer();
	virtual void Init();
	virtual void ReInit();
	virtual void PostLoadInit();

#ifdef EDITOR
	virtual void InitControls(BaseWidget* parent);
	virtual void PrepareControls(TimeLineWidget* timeline, int line);	
#endif

	virtual void SetTime(float dt);	

	virtual void LoadProperties(IStreamLoader* stream);
	virtual void SaveProperties(IStreamSaver* stream);
};