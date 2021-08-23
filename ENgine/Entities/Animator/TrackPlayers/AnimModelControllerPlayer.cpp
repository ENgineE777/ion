
#include "AnimModelControllerPlayer.h"
#include "AnimModelPlayer.h"
#include "DeepBlack/Cutscene/CutSceneManager.h"
#include "Entities/Render/AnimModel.h"
#include "DeepBlack/World.h"
#include "DeepBlack/WorldCamera.h"

#ifdef EDITOR
#include "Platforms\PC\Editor\EditorMain.h"
#include "Platforms\PC\Editor\EditorMainWnd.h"
#endif

TRACKDESC(AnimModelControllerPlayer, "AnimModelControllerPlayer", 0)

void AnimModelControllerPlayer::Init()
{	
#ifdef EDITOR
	m_propPanel = NULL;
#endif

	AddTransformDataTrack( Transform ,5, false,(TrackPlayer::KeyFunc)&AnimModelControllerPlayer::ApplyTrans, NULL)
	AddFloatDataTrack(Weight, NULL, (TrackPlayer::KeyFunc)&AnimModelControllerPlayer::ApplyWeight, 1.f )
	AddFloatDataTrack(WeightTime, NULL, NULL, 1.f )

#ifdef EDITOR
	GetDataTrack("Weight")->AddChild(GetDataTrack("WeightTime"));
	GetDataTrack("WeightTime")->SetParent(GetDataTrack("Weight"));
#endif

	PROP_REG(m_AnimModelPlayerName);
	PROP_REG(m_ControllerName);
	

//#ifdef EDITOR
//	((TransformDataTrack*)tracks[0])->invertSightInGizmo = true;
//#endif
}

void AnimModelControllerPlayer::ReInit()
{
	AnimModelPlayer* player = static_cast<AnimModelPlayer*>(owner->GetTrackPlayer(m_AnimModelPlayerName));
	if (!player || (player && strcmp(player->type, "AnimModel")))
	{
		#ifdef EDITOR
		if (g_GameProxy.EditorMode)
		{
			MessageBox((HWND)wxGetApp().frame->tools->GetHWND(), (LPCSTR)":)))))!", (LPCSTR)"Open your eyes loser!", 0);
		}
			
		#endif
		return;
	}

	m_AnimModelPlayer = player;
};


void AnimModelControllerPlayer::ApplyTrans(TransformTrackPoint* tr)
{
	if (m_AnimModelPlayer)
	{		
		m_AnimModelPlayer->GetAnim()->SetControllerGoal(m_ControllerName, 
			tr->pos);
	}
}

void AnimModelControllerPlayer::ApplyWeight(TrackFloatPoint* _value)
{
	if (m_AnimModelPlayer)
	{		

		float weight = static_cast<FloatDataTrack*>(GetDataTrack("WeightTime"))->GetCurValue().val;
		m_AnimModelPlayer->GetAnim()->SetControllerWeight(m_ControllerName, 
			_value->val, 
			weight);
	}
}

