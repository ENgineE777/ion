#pragma once

#include "TrackPlayer.h"
#include "GameBaseEntityPlayer.h"

class BaseModelPlayer : public GameBaseEntityPlayer
{		
public:

	BaseModelPlayer(TrackDescBase* Desc): GameBaseEntityPlayer(Desc)
	{
		m_Model = NULL;
		m_cutSceneBaseEntity = NULL;
		GameBaseEntityPlayer::ms_AttachedToTypes.insert("BaseModelPlayer");
	};

	virtual void Init();
	virtual void Release();

	virtual void Reset();
	virtual void StartPlay();
	virtual void EndPlay();

	virtual void ReInit();

	void ApplyTrans(TransformTrackPoint* tr);	
	void SetModelName(const char* _name);

	virtual class CGameBaseEntity* GetEntity();

private:
	class CBaseModel* m_Model;
	class CutSceneBaseEntity* m_cutSceneBaseEntity;
	FILENAME_PROP(BaseModelPlayer, m_FileNameProp, "", "*.mesh file")
};