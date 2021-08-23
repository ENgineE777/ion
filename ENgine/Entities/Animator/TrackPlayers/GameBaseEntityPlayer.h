#pragma once

#include "TrackPlayer.h"

class GameBaseEntityPlayer : public TrackPlayer
{		
public:

	GameBaseEntityPlayer(TrackDescBase* Desc): TrackPlayer(Desc)
	{
		m_isEntityUsedAfterEndPlay = false;
	};

	virtual void Init();

	virtual void StartPlay();
	virtual void EndPlay();

	virtual void ReInit();
	virtual void PostLoadCutScene();
	virtual void Release();
	
	virtual class CGameBaseEntity* GetEntity() = 0;
	void SetEntityUsedAfterEndPlay( bool _value ) { m_isEntityUsedAfterEndPlay = _value;}
	//void SetChildEntitesUsedAfterEndPlay( bool _value );
	virtual bool UseChildsAfterEndPlay() { return false;}


	

	static std::set<std::string> ms_AttachedToTypes;

protected:
	bool IsEntityBelongsToPlayers(CGameBaseEntity* _entity);
	GameBaseEntityPlayer* GetEntityPlayer(CGameBaseEntity* _entity);
	void ApplyVisibility(const BoolDataTrackPoint& _point);
	
	bool m_isEntityUsedAfterEndPlay;

	STRING_PROP(GameBaseEntityPlayer, m_PropPlayerName, "", "TrackPlayer Name")
	STRING_PROP(GameBaseEntityPlayer, m_PropBoneName, "", "Bone Name")
};