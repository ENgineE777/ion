
#include "BaseModelPlayer.h"
#include "DeepBlack/World.h"
#include "DeepBlack/WorldCamera.h"

#include "AnimModelPlayerWidgets.h"
#include "Entities/Render/BaseModel.h"

#include "../Entities/CutSceneBaseEntity.h"
#include "../CutScene.h"

#include "Core/Core.h"

#include <algorithm>

TRACKDESC(BaseModelPlayer, "BaseModelPlayer", 0)

void BaseModelPlayer::Init()
{	
	GameBaseEntityPlayer::Init();
	AddTransformDataTrack( Transform ,5, false,(TrackPlayer::KeyFunc)&BaseModelPlayer::ApplyTrans, NULL)

#ifdef EDITOR
	((TransformDataTrack*)GetDataTrack("Transform"))->invertSightInGizmo = true;
#endif

	PROP_REG(m_FileNameProp);

	CScene* scene = CWorld::instance()->getUpdateScene();

	if(CWorld::instance()->getLoadScene() && !CWorld::instance()->getLoadScene()->getSceneSource().empty())
		scene = CWorld::instance()->getLoadScene();


	if (!scene)
		return;


	m_cutSceneBaseEntity = (CutSceneBaseEntity*)scene->createLocalEntity(eCUTSCENEBASEENTITY, NxMat34(), "");

}


CGameBaseEntity* BaseModelPlayer::GetEntity()
{
	return m_cutSceneBaseEntity;
}

void BaseModelPlayer::ReInit()
{
	GameBaseEntityPlayer::ReInit();
	SetModelName(m_FileNameProp);
}

void BaseModelPlayer::ApplyTrans(TransformTrackPoint* tr)
{
	if (GetEntity() && GetEntity()->getParent())
		return;

	IMatrix mat;
	tr->rot.GetMatrix(mat);
	mat.Pos() = tr->pos;
	
	NxMat34 nxmat;
	mat.Get(nxmat);

	m_cutSceneBaseEntity->setTransform(nxmat);
	m_cutSceneBaseEntity->update(0.f);

}


void BaseModelPlayer::Reset()
{
	GameBaseEntityPlayer::Reset();
	if (m_cutSceneBaseEntity->m_BaseModel) m_cutSceneBaseEntity->m_BaseModel->onSetEnable(true);

}

void BaseModelPlayer::StartPlay()
{
	if (m_cutSceneBaseEntity->m_BaseModel) m_cutSceneBaseEntity->m_BaseModel->onSetEnable(true);
}

void BaseModelPlayer::EndPlay()
{
	if (m_cutSceneBaseEntity->m_BaseModel && !m_isEntityUsedAfterEndPlay)
		m_cutSceneBaseEntity->m_BaseModel->onSetEnable(false);
}

void BaseModelPlayer::Release()
{
	if (m_cutSceneBaseEntity->m_BaseModel)
	{
		delete m_cutSceneBaseEntity->m_BaseModel;
		m_cutSceneBaseEntity->m_BaseModel = NULL;
	}

	m_cutSceneBaseEntity->setFlag(true, CBaseEntity::eDeleted);

	GameBaseEntityPlayer::Release();
}

void BaseModelPlayer::SetModelName(const char* _name)
{
	if (m_cutSceneBaseEntity->m_BaseModel)
	{
		delete m_cutSceneBaseEntity->m_BaseModel;
		m_cutSceneBaseEntity->m_BaseModel = NULL;
	}

	strcpy( m_FileNameProp, _name);

	m_cutSceneBaseEntity->m_BaseModel  =  new CBaseModel();
	m_cutSceneBaseEntity->m_BaseModel->onSetModel(_name);
	if (g_GameProxy.EditorMode)
	{
		CScene* scene = CWorld::instance()->getUpdateScene();

		//is loading
		if(CWorld::instance()->getLoadScene() && !CWorld::instance()->getLoadScene()->getSceneSource().empty())
			m_cutSceneBaseEntity->m_BaseModel->onSetEnable(false);
		else
			m_cutSceneBaseEntity->m_BaseModel->onSetEnable(true);
	}
	else
		m_cutSceneBaseEntity->m_BaseModel->onSetEnable(false);

	m_cutSceneBaseEntity->m_BaseModel->onForceEMotion(true);	
	//m_cutSceneBaseEntity->m_BaseModel->onSetUpdateBB(false);

	IMatrix mat;
	mat.Identity();

	m_cutSceneBaseEntity->m_BaseModel->onSetPlace(&mat);
	m_cutSceneBaseEntity->m_BaseModel->update(0);
}

