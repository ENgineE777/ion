#pragma once

#include "GameBaseEntityPlayer.h"

class AnimModelPlayer : public GameBaseEntityPlayer
{		
public:

	AnimModelPlayer(TrackDescBase* Desc): GameBaseEntityPlayer(Desc)
	{
		m_Anim = NULL;
		m_AnimModel = NULL;
		m_cutSceneAnimEntity = NULL;
		GameBaseEntityPlayer::ms_AttachedToTypes.insert("AnimModel");
	};

	virtual void Init();
	virtual void SetTime(float dt);	

	virtual void StartPlay();
	virtual void EndPlay();

	virtual void Reset();

	void ApplyDefault();

	void ApplyTrans(TransformTrackPoint* tr);	

	void SetModelName(const char* _name);
	void SetAnimGrafName(const char* _name);
	
	virtual void LoadProperties(StreamLoader* stream);	
	virtual void SaveProperties(StreamSaver* stream);	

	virtual class CGameBaseEntity* GetEntity();

#ifdef EDITOR
	virtual void InitControls(wxWindow* prop_dock);		
	virtual void StartEditProp();	
	virtual void StopEditProp();
#endif

	virtual void Release();

	class IAnimationInstance* GetAnim(){ return m_Anim;	}

private:
#ifdef EDITOR
	class AnimModelPlayerWidgets* m_propPanel;
#endif
	AnimDataTrack* animTrack;
	class IAnimationInstance* m_Anim;	
	class CAnimModel* m_AnimModel;
	class CutSceneAnimEntity* m_cutSceneAnimEntity;
	string m_ModelName;
	string m_AnimGrafName;

	STRING_PROP(AnimModelPlayer, m_PropOnEndPLayPointName, "", "OnEndPLayPointName")
};