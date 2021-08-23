#pragma once

#include "TrackPlayer.h"
#include "GameBaseEntityPlayer.h"

class CharacterTrackWidgets;
class CCharacter;
	
class CharacterPlayer : public GameBaseEntityPlayer
{		

#ifdef EDITOR
public:
	CharacterTrackWidgets*	m_Wigets;
#endif

public:
    CharacterPlayer(TrackDescBase* Desc):GameBaseEntityPlayer(Desc),m_Movement2D(false),m_Character(0),m_OrientBySpline(true)
#ifdef EDITOR
		,m_Wigets(0)
#endif
	{
		m_CharacterName[0] = 0;
		m_RespawnName[0] = 0;
		owner = NULL;

		GameBaseEntityPlayer::ms_AttachedToTypes.insert("CharacterPlayer");
	};

	virtual void Init();
	virtual void ReInit();
	virtual void SetTime(float dt);	
	virtual void Reset();
	virtual void StartPlay();
	virtual void EndPlay();

	virtual void LoadProperties(StreamLoader* stream);
	virtual void SaveProperties(StreamSaver* stream);
	virtual void Release();
	//virtual void PostLoadCutScene();

	

	virtual CGameBaseEntity* GetEntity();
	virtual bool UseChildsAfterEndPlay() { return true;}

	void ApplyTrans(TransformTrackPoint* tr);
	void ApplyCommand(AICommandPoint* tr);	
	
	void		SetCharacter(const char* name,bool bload = true);
	void		SetRespawn(const char* name,bool bload = true);
	const char* GetRespawnName(){return m_RespawnName;}
	const char* GetCharacterName(){return m_CharacterName;}
	bool		isMovement2D(){return m_Movement2D;}
	void        setMovementFlag(bool bSet);
	void        setOrientBySplineFlag(bool bSet);
  
#ifdef EDITOR
	virtual void InitControls(wxWindow* prop_dock);
	virtual void StartEditProp();	
	virtual void StopEditProp();
	virtual void OnClose();
	virtual void OnStopEdit();
#endif
private:
	 char					m_CharacterName[128];
	 char					m_RespawnName[128];
	 bool					m_Movement2D;
	 bool					m_OrientBySpline;
	 CCharacter*			m_Character;

};