
#include "CharacterPlayer.h"
#include "CharacterTrackWidgets.h"
#include "DeepBlack/World.h"
#include "DeepBlack/Game.h"
#include "DeepBlack/Entities/Character.h"
#include "DeepBlack/WorldCamera.h"
#include "DeepBlack/AiEssence.h"
#include "Core/Physics/Physics.h"


TRACKDESC(CharacterPlayer, "CharacterPlayer", 0)
//-------------------------------------------------------------------------------------------------------------------------
void CharacterPlayer::Init()
{		
	GameBaseEntityPlayer::Init();

	AddTransformDataTrack( Transform , 5, true, (TrackPlayer::ChangeKeyFunc)&CharacterPlayer::ApplyTrans, NULL)	
	AddAICommandTrack( Commands , NULL,(TrackPlayer::ChangeKeyFunc)&CharacterPlayer::ApplyCommand)	
	#ifdef EDITOR
		((TransformDataTrack*)GetDataTrack("Transform"))->invertSightInGizmo = true;
	#endif

}

void CharacterPlayer::ReInit()
{
	GameBaseEntityPlayer::ReInit();


}

CGameBaseEntity* CharacterPlayer::GetEntity()
{
	return m_Character;
}

//void CharacterPlayer::PostLoadCutScene() 
//{
//	GameBaseEntityPlayer::PostLoadCutScene();
//
//	SetChildEntitesUsedAfterEndPlay(true);
//
//}

//-------------------------------------------------------------------------------------------------------------------------
void CharacterPlayer::SetTime(float dt)
{	
	TrackPlayer::SetTime(dt);
}
void CharacterPlayer::Reset()
{	
	TrackPlayer::Reset();
	if(m_Character)
	{
		if (m_Character->GetAnimation())
			m_Character->GetAnimation()->Restart();
		Turrets_t& turrets = m_Character->getTurrets();
		for (Turrets_t::iterator It = turrets.begin(); It != turrets.end(); It++)
		{
			CTurret& turret = It->second;
			turret.setMode(eScanForTarget);
		}

		CGameEntityMirror * mirror = CWorld::instance()->getUpdateScene()->getNetEntityMirror(m_Character->getId());
		m_Character->setFlag(false,CBaseEntity::eReadyToShoot);
		m_Character->setFlag(false,CBaseEntity::eHide);
		m_Character->setFlag(true,CBaseEntity::eSimulate);
		m_Character->setFlag(true,CBaseEntity::eCollide);
	}
}


void CharacterPlayer::StartPlay()
{	
	GameBaseEntityPlayer::StartPlay();

	if(m_Character)
	{
		m_Character->GetAnimation()->Restart();
		Turrets_t& turrets = m_Character->getTurrets();
		for (Turrets_t::iterator It = turrets.begin(); It != turrets.end(); It++)
		{
			CTurret& turret = It->second;
			turret.setMode(eScanForTarget);
		}

		m_Character->setFlag(false,CBaseEntity::eReadyToShoot);
		m_Character->setFlag(false,CBaseEntity::eHide);
		m_Character->setFlag(true,CBaseEntity::eSimulate);
		m_Character->setFlag(true,CBaseEntity::eCollide);
	}
}
//-------------------------------------------------------------------------------------------------------------------------
void CharacterPlayer::ApplyTrans(TransformTrackPoint* tr)
{
	
	if(m_Character)
	{
		if (m_Character->isDead())
		{
			m_Character = NULL;
			return;
		}

		//if(m_Character->isHide())
		//{
		//	m_Character->setFlag(false,CBaseEntity::eHide);
		//	m_Character->setFlag(true,CBaseEntity::eSimulate);
		//	m_Character->setFlag(true,CBaseEntity::eCollide);
		//}
		if(m_Movement2D)
		{
			NxVec3 pos;NxQuat q;
			pos.x = tr->pos.x;
			pos.y = tr->pos.y;
			pos.z = tr->pos.z;
			q.x = tr->rot.x;
			q.y = tr->rot.y;
			q.z = tr->rot.z;
			q.w = tr->rot.w;
			NxReal halfHeight = m_Character->getDesc().m_Size.y * 0.5f;
			NxVec3 end(pos + g_down *  100);
			m_Character->setFlag(false,CBaseEntity::eCollide);
			const sPhysicsContact& contact  = m_Character->getScene()->getPhysicScene()->rayTrace(pos,end,CPhysicScene::sc_RayTraceCollisionGroups);
			m_Character->setFlag(true,CBaseEntity::eCollide);
			if(contact.pObj)
			{
				pos.y = contact.point.y;
				pos.y += halfHeight + 0.02f;
			}
			m_Character->setPosition(pos);
			m_Character->setRotation(q);
		}
		else
		{
			NxVec3 pos;NxQuat q;
			pos.x = tr->pos.x;
			pos.y = tr->pos.y;
			pos.z = tr->pos.z;
			q.x = tr->rot.x;
			q.y = tr->rot.y;
			q.z = tr->rot.z;
			q.w = tr->rot.w;
			m_Character->setPosition(pos);
			m_Character->setRotation(q);
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------
void CharacterPlayer::ApplyCommand(AICommandPoint* tr)
{ 		
	if(m_Character)
	{
		if (m_Character->isDead())
		{
			m_Character = NULL;
			return;
		}

		for(NxU32 i = 0;i < tr->list.size(); ++i)
		{
			((CAiCommand*)tr->list[i])->start(m_Character);
			((CAiCommand*)tr->list[i])->doAction(0,m_Character);
		}
	}
	
}
//-------------------------------------------------------------------------------------------------------------------------
void CharacterPlayer::EndPlay()
{
	GameBaseEntityPlayer::EndPlay();

	if (m_Character)
		m_Character->setFlag(false, CBaseEntity::eForceMoving);

	TransformDataTrack* transform_track =  ((TransformDataTrack*)GetDataTrack("Transform"));
	if(transform_track->GetCurIndex() < transform_track->GetPoints().size())
	{
		ApplyTrans((TransformTrackPoint*)&transform_track->GetPoints()[transform_track->GetPoints().size() - 1]);
	}
	AICommandTrack*  command_track = ((AICommandTrack*)GetDataTrack("Commands"));
	for(int i = command_track->GetCurIndex();i < command_track->GetPoints().size();i++)
	{
		ApplyCommand((AICommandPoint*)&command_track->GetPoints()[i]);
	}
}
//-------------------------------------------------------------------------------------------------------------------------
void CharacterPlayer::LoadProperties(StreamLoader* stream)
{
	GameBaseEntityPlayer::LoadProperties(stream);

	char name[128];
	name[0] = 0;

	stream->ReadString("Respawn", name);
	if(strlen(name) != 0)
	{
		SetRespawn(name);
		stream->ReadString("Character", name);
	}
	else
	{
		stream->ReadString("Character", name);
		SetCharacter(name);
	}
	m_Movement2D = false;
	stream->ReadBool("Movement2D", m_Movement2D);
	m_OrientBySpline = true;
	stream->ReadBool("OrientBySpline", m_OrientBySpline);
	((TransformDataTrack*)GetDataTrack("Transform"))->orient_by_spline = m_OrientBySpline;

		//SetChildEntitesUsedAfterEndPlay(true);
}
//-------------------------------------------------------------------------------------------------------------------------
void CharacterPlayer::SaveProperties(StreamSaver* stream)
{
	GameBaseEntityPlayer::SaveProperties(stream);

	stream->SaveString("Respawn", m_RespawnName);
	stream->SaveString("Character", m_CharacterName);
	stream->SaveBool("Movement2D", m_Movement2D);
	stream->SaveBool("OrientBySpline", m_OrientBySpline);
}
//-------------------------------------------------------------------------------------------------------------------------
void CharacterPlayer::Release()
{
	//if(m_Character)
	//	 delete m_Character;
	m_Character = 0;
	GameBaseEntityPlayer::Release();
}
//-------------------------------------------------------------------------------------------------------------------------
void CharacterPlayer::setMovementFlag(bool bSet)
{
	m_Movement2D = bSet;
}
//-------------------------------------------------------------------------------------------------------------------------
void CharacterPlayer::setOrientBySplineFlag(bool bSet)
{
	m_OrientBySpline = bSet;
	((TransformDataTrack*)GetDataTrack("Transform"))->orient_by_spline = m_OrientBySpline;
}
//-------------------------------------------------------------------------------------------------------------------------
void CharacterPlayer::SetRespawn(const char* name,bool bload)
{
	strcpy(m_RespawnName,name);
	CScene*     scene = 0;
	if(bload)
		scene =	CWorld::instance()->getLoadScene();
	else
		scene =	CWorld::instance()->getUpdateScene();

	if ( !scene )
		return;

	CRespawn*	resp = scene->getGameManager()->findRespawn(name);
	if(resp && resp->getType() == CRespawn::eAi)
	{
		if(strlen(m_RespawnName) > 0)
		{
			CRespawn* respawn = scene->getGameManager()->findRespawn(m_RespawnName);
			if(respawn)
				respawn->onChildDelete(m_Character); 
		}
		if(m_Character)
			m_Character->setFlag(true, CBaseEntity::eDeleted);
		m_Character = 0;

		strcpy(m_CharacterName,resp->getEntityDesc());
		m_Character = static_cast<CCharacter*>(resp->createEntity(scene));
		if(m_Character)
		{
			m_Character->setBornState("Script");
			m_Character->setFlag(false,CBaseEntity::eUnderGravity);
			m_Character->setFlag(true,CBaseEntity::eForceMoving);
			//m_Character->setFlag(false,CBaseEntity::eAutomateDelete);
			
			if(bload)
			{
				m_Character->setFlag(true,CBaseEntity::eHide);
				m_Character->setFlag(false,CBaseEntity::eCollide);
				m_Character->setFlag(false,CBaseEntity::eSimulate);
			}
			else
			{
				m_Character->setFlag(false,CBaseEntity::eHide);
				m_Character->setFlag(true,CBaseEntity::eCollide);
				m_Character->setFlag(true,CBaseEntity::eSimulate);
				
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------
void CharacterPlayer::SetCharacter(const char* name,bool bload )
{
	m_RespawnName[0] = 0;
	strcpy(m_CharacterName,name);
	const sEntityDesc& desc = CGame::getCharDesc(m_CharacterName);
	eGameEntityClass entityClass = (eGameEntityClass)desc.m_CharType;

	NxMat34 transform;transform.id();

	CScene* scene = 0;
	if(bload)
	{
		scene = CWorld::instance()->getLoadScene();
	}
	else
	{
		scene = CWorld::instance()->getUpdateScene();
		const CWorldCamera* camera = CWorld::instance()->getWorldCamera();
		if ( camera )
		{
			transform.t = camera->getPos();
			transform.M.fromQuat(camera->getRot());
			transform.t += transform.M.getColumn(2) * 5;
		}
	}
	if(strlen(m_RespawnName) > 0)
	{
		if ( scene )
		{
			CRespawn* respawn = scene->getGameManager()->findRespawn(m_RespawnName);
			if(respawn)
				respawn->onChildDelete(m_Character);
		}
	}
	if(m_Character)
		m_Character->setFlag(true, CBaseEntity::eDeleted);
	m_Character = 0;

	if ( scene )
	{
		m_Character = (CCharacter*)scene->createNetEntity(entityClass,transform,desc.m_Name.c_str(),true);
	}
	else
	{
		if (g_GameProxy.PackMode)
		{
			if (core.Files()->GetFilePath(name, "ai"))
			{
				core.Files()->RegisterFile(name, "ai");
			}
		}
	}

	if(m_Character)
	{
		m_Character->setBornState("Script");
		
		m_Character->setFlag(false,CBaseEntity::eUnderGravity);
		m_Character->setFlag(true,CBaseEntity::eForceMoving);
		//m_Character->setFlag(false,CBaseEntity::eAutomateDelete);
		if(bload)
		{
			m_Character->setFlag(true,CBaseEntity::eHide);
			m_Character->setFlag(false,CBaseEntity::eCollide);
			m_Character->setFlag(false,CBaseEntity::eSimulate);
		}
		else
		{
			m_Character->setFlag(false,CBaseEntity::eHide);
			m_Character->setFlag(true,CBaseEntity::eCollide);
			m_Character->setFlag(true,CBaseEntity::eSimulate);
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------
#ifdef EDITOR
void CharacterPlayer::InitControls(wxWindow* prop_dock)
{
	TrackPlayer::InitControls(prop_dock);
	m_Wigets = new CharacterTrackWidgets(prop_dock);
	m_Wigets->InitControls();
	m_Wigets->Hide();
	if(!m_OrientBySpline)
	{
		m_Wigets->prop_OrientByspline->SetValue(false);
	}
	if(strlen(m_RespawnName) > 0)
	{
		m_Wigets->prop_LoadStyle->SetValue(false);
		m_Wigets->m_LoadFromFile = false;
		int index = m_Wigets->respawnsList->FindString(wxString(m_RespawnName));
		m_Wigets->respawnsList->SetSelection(index);
	}
	else
	{
		m_Wigets->prop_LoadStyle->SetValue(true);
		m_Wigets->m_LoadFromFile = true;
	}
}
//-------------------------------------------------------------------------------------------------------------------------
void CharacterPlayer::OnStopEdit()
{
  if(m_Character)
  {
	  m_Character->setFlag(true,CBaseEntity::eHide);
	  m_Character->setFlag(false,CBaseEntity::eSimulate);
  }
}
//-------------------------------------------------------------------------------------------------------------------------
void CharacterPlayer::StartEditProp()
{
	CharacterTrackWidgets::g_CurPlayer = this;	
	m_Wigets->Show();
}
//-------------------------------------------------------------------------------------------------------------------------
void CharacterPlayer::StopEditProp()
{
  CharacterTrackWidgets::g_CurPlayer = 0;
  m_Wigets->Hide();
}
//-------------------------------------------------------------------------------------------------------------------------
void CharacterPlayer::OnClose()
{
	if(m_Character)
	{
		m_Character->setFlag(true,CBaseEntity::eHide);
		m_Character->setFlag(false,CBaseEntity::eSimulate);
	}
	if(m_Wigets)
		delete m_Wigets;
	m_Wigets = 0;
	__super::OnClose();
}
#endif