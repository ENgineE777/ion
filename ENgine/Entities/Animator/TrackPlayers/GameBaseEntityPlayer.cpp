
#include "GameBaseEntityPlayer.h"

#include "Entities/Render/BaseModel.h"
#include "../Entities/CutSceneBaseEntity.h"
#include "../CutScene.h"
#include "DeepBlack/World.h"

#ifdef EDITOR
#include "Platforms\PC\Editor\EditorMain.h"
#include "Platforms\PC\Editor\EditorMainWnd.h"
#endif
//TRACKDESC(GameBaseEntityPlayer, "GameBaseEntityPlayer", 0)

std::set<std::string> GameBaseEntityPlayer::ms_AttachedToTypes;

void GameBaseEntityPlayer::Init()
{	
	AddBoolDataTrack( Visibility , NULL, (TrackPlayer::ChangeKeyFunc)&GameBaseEntityPlayer::ApplyVisibility, true, true)

	PROP_REG(m_PropPlayerName);
	PROP_REG(m_PropBoneName);
}

void GameBaseEntityPlayer::ReInit()
{
	if (!GetEntity())
		return;

	if (GetEntity()->getParent())
		GetEntity()->getParent()->removeChild(GetEntity());

	if (!strlen(m_PropPlayerName))
		return;


	TrackPlayer* player = owner->GetTrackPlayer(m_PropPlayerName);
	if (!player || (player && ms_AttachedToTypes.find(std::string(player->type)) == ms_AttachedToTypes.end() ))
	{
#ifdef EDITOR
		if (g_GameProxy.EditorMode)
		{
			//do not show message on loading;
			if(CWorld::instance()->getLoadScene() && !CWorld::instance()->getLoadScene()->getSceneSource().empty())
				return;

			if (wxGetApp().frame->tools)
				MessageBox((HWND)wxGetApp().frame->tools->GetHWND(), (LPCSTR)":)))))!", (LPCSTR)"Unvalid object", 0);
		}

#endif
		return;
	}

	GameBaseEntityPlayer* parent = static_cast<GameBaseEntityPlayer*>(player);
	m_isEntityUsedAfterEndPlay = parent->UseChildsAfterEndPlay();

	

	if ( !strlen(m_PropBoneName))
	{
		sHook hook(sHook::g_IdentityHook);
		parent->GetEntity()->addChild(GetEntity(), hook);
	}
	else
	{
		parent->GetEntity()->addChild(GetEntity(), sHook( std::string(m_PropBoneName), NxMat34()) );
	}

}

void GameBaseEntityPlayer::PostLoadCutScene() 
{
	// call ReInit() because "owner->GetTrackPlayer(m_PropPlayerName);" may return NULL while trackplayers are loading
	if ( GetEntity() && strlen(m_PropPlayerName) && !GetEntity()->getParent())
		GameBaseEntityPlayer::ReInit();
	
}

void GameBaseEntityPlayer::StartPlay()
{
	TrackPlayer::StartPlay();
}

void GameBaseEntityPlayer::EndPlay()
{
	TrackPlayer::EndPlay();
}


void GameBaseEntityPlayer::Release()
{
	CGameBaseEntity* ent = GetEntity();

	if ( ent )
	{
		for (uint i = 0; i < ent->getNumChilds(); ++i)
		{
			if ( IsEntityBelongsToPlayers( static_cast<CGameBaseEntity*>(ent->getChild(i)))	)
			{
				ent->removeChild(ent->getChild(i));
				--i;
			}
		}
	}

	TrackPlayer::Release();
}


bool GameBaseEntityPlayer::IsEntityBelongsToPlayers(CGameBaseEntity* _entity)
{
	uint size = owner->players.size();
	for (uint i = 0; i < size; ++i)
	{
		TrackPlayer* player = owner->players[i];
		if (!player)
			continue;
		
		if (ms_AttachedToTypes.find(player->type) != ms_AttachedToTypes.end() )
		{
			GameBaseEntityPlayer* playerEntity = static_cast<GameBaseEntityPlayer*>(player);
			if (playerEntity->GetEntity() == _entity)
				return true;
		}
	}

	return false;
}

GameBaseEntityPlayer* GameBaseEntityPlayer::GetEntityPlayer(CGameBaseEntity* _entity)
{
	uint size = owner->players.size();
	for (uint i = 0; i < size; ++i)
	{
		TrackPlayer* player = owner->players[i];
		if (!player)
			continue;

		if (ms_AttachedToTypes.find(player->type) != ms_AttachedToTypes.end() )
		{
			GameBaseEntityPlayer* playerEntity = static_cast<GameBaseEntityPlayer*>(player);
			if (playerEntity->GetEntity() == _entity)
				return playerEntity;
		}
	}

	return NULL;
}


//void GameBaseEntityPlayer::SetChildEntitesUsedAfterEndPlay( bool _value )
//{
//	CGameBaseEntity* ent = GetEntity();
//
//	if ( ent )
//	{
//		for (uint i = 0; i < ent->getNumChilds(); ++i)
//		{
//			GameBaseEntityPlayer* player = GetEntityPlayer( static_cast<CGameBaseEntity*>(ent->getChild(i)));
//			if ( player)
//			{
//				player->SetEntityUsedAfterEndPlay(_value);
//			}
//		}
//	}
//
//}

void GameBaseEntityPlayer::ApplyVisibility(const BoolDataTrackPoint& _point)
{
	if (GetEntity())
	{
		GetEntity()->setFlag(!_point.m_value, CBaseEntity::eHide);
		if (!_point.m_value)
			GetEntity()->hide();
		if (_point.m_value)
			GetEntity()->unhide();
	}
}



