
#include "AnimModelPlayer.h"
#include "DeepBlack/World.h"
#include "DeepBlack/WorldCamera.h"

#include "AnimModelPlayerWidgets.h"
#include "Entities/Render/AnimModel.h"

#include "../Entities/CutSceneAnimEntity.h"
#include "../CutScene.h"

#include "Core/Core.h"

#include <algorithm>

TRACKDESC(AnimModelPlayer, "AnimModel", 0)

void AnimModelPlayer::Init()
{	
	GameBaseEntityPlayer::Init();
#ifdef EDITOR
	m_propPanel = NULL;
#endif

	PROP_REG(m_PropOnEndPLayPointName)

	AddTransformDataTrack( Transform ,5, false,(TrackPlayer::KeyFunc)&AnimModelPlayer::ApplyTrans, NULL)
	AddAnimDataTrack(AnimCommands, animTrack)	

#ifdef EDITOR
	((TransformDataTrack*)GetDataTrack("Transform"))->invertSightInGizmo = true;
#endif

	CScene* scene = CWorld::instance()->getUpdateScene();

	if(CWorld::instance()->getLoadScene() && !CWorld::instance()->getLoadScene()->getSceneSource().empty())
		scene = CWorld::instance()->getLoadScene();


	if (!scene)
		return;


	m_cutSceneAnimEntity = (CutSceneAnimEntity*)scene->createLocalEntity(eCUTSCENEANIMENTITY, NxMat34(), "");


}

CGameBaseEntity* AnimModelPlayer::GetEntity()
{
	return m_cutSceneAnimEntity;
}


void AnimModelPlayer::SetTime(float dt)
{
	ApplyDefault();
	TrackPlayer::SetTime(dt);

	if (m_cutSceneAnimEntity->m_Anim)
		m_cutSceneAnimEntity->m_Anim->SetTime(dt);
}

void AnimModelPlayer::ApplyTrans(TransformTrackPoint* tr)
{
	if (GetEntity() && GetEntity()->getParent())
		return;

	IMatrix mat;
	tr->rot.GetMatrix(mat);
	mat.Pos() = tr->pos;

	NxMat34 nxmat;
	mat.Get(nxmat);

	m_cutSceneAnimEntity->setTransform(nxmat);
	m_cutSceneAnimEntity->update(0.f);

}


void AnimModelPlayer::StartPlay()
{
	if (m_cutSceneAnimEntity->m_Anim) m_cutSceneAnimEntity->m_Anim->Pause(false);
	if (m_cutSceneAnimEntity->m_Anim) m_cutSceneAnimEntity->m_Anim->Restart();
	if (m_cutSceneAnimEntity->m_AnimModel)
	{
		m_cutSceneAnimEntity->m_AnimModel->onSetEnable(true);
	}
}

void AnimModelPlayer::EndPlay()
{
	if ( strlen(m_PropOnEndPLayPointName) )
	{
		((TransformDataTrack*)GetDataTrack("Transform"))->SetTime( owner->timeLenght );
		const TransformTrackPoint& tr = ((TransformDataTrack*)GetDataTrack("Transform"))->GetCurValue();
		IMatrix mat;
		tr.rot.GetMatrix(mat);
		mat.Pos() = tr.pos;
		owner->AddRegisteredPoint(m_PropOnEndPLayPointName,  mat);
	}


	
	if (m_cutSceneAnimEntity->m_AnimModel && !m_isEntityUsedAfterEndPlay)
	{
		m_cutSceneAnimEntity->m_AnimModel->onSetEnable(false);
		if (m_cutSceneAnimEntity->m_Anim ) m_cutSceneAnimEntity->m_Anim->Pause(true);
	}


}

void AnimModelPlayer::Reset()
{
	GameBaseEntityPlayer::Reset();
	if (m_cutSceneAnimEntity->m_AnimModel) m_cutSceneAnimEntity->m_AnimModel->onSetEnable(true);

}

void AnimModelPlayer::ApplyDefault()
{

}

#ifdef EDITOR

void AnimModelPlayer::InitControls(wxWindow* prop_dock)
{
	TrackPlayer::InitControls(prop_dock);

	if (!m_propPanel)
	{
		delete m_propPanel;
		m_propPanel = NULL;
	};

	m_propPanel = new AnimModelPlayerWidgets(prop_dock);
	m_propPanel->InitControls();
	m_propPanel->Hide();
	m_propPanel->m_Player = this;

	m_propPanel->m_ModelNameTxt->SetLabelText(wxString(m_ModelName));
	m_propPanel->m_AnimGrafTxt->SetLabelText(wxString(m_AnimGrafName));
}

void AnimModelPlayer::StartEditProp()
{	
	__super::StartEditProp();
	m_propPanel->Show();
}

void AnimModelPlayer::StopEditProp()
{	
	__super::StopEditProp();
	m_propPanel->Hide();
}

#endif

void AnimModelPlayer::Release()
{

	ApplyDefault();	

#ifdef EDITOR
	if (!m_propPanel)
	{
		delete m_propPanel;
		m_propPanel = NULL;
	}
#endif

	if ( m_cutSceneAnimEntity->m_Anim)
	{
		m_cutSceneAnimEntity->m_Anim->Release();
		m_cutSceneAnimEntity->m_Anim = NULL;
	}

	if (m_cutSceneAnimEntity->m_AnimModel)
	{
		delete m_cutSceneAnimEntity->m_AnimModel;
		m_cutSceneAnimEntity->m_AnimModel = NULL;
	}

	if (m_cutSceneAnimEntity)
		m_cutSceneAnimEntity->setFlag( true, CBaseEntity::eDeleted);
	m_cutSceneAnimEntity = NULL;

	GameBaseEntityPlayer::Release();
}

void AnimModelPlayer::SetModelName(const char* _name)
{
	

	if (m_cutSceneAnimEntity->m_Anim)
	{
		m_cutSceneAnimEntity->m_Anim->Release();
		m_cutSceneAnimEntity->m_Anim = NULL;
		m_AnimGrafName = "";
	}

	if (m_cutSceneAnimEntity->m_AnimModel)
	{
		delete m_cutSceneAnimEntity->m_AnimModel;
		m_cutSceneAnimEntity->m_AnimModel = NULL;
	}

	m_ModelName = _name;

	m_cutSceneAnimEntity->m_AnimModel  =  new CAnimModel();
	m_cutSceneAnimEntity->m_AnimModel->onSetModel(_name);
	
	if (g_GameProxy.EditorMode)
	{
		CScene* scene = CWorld::instance()->getUpdateScene();

		//is loading
		if(CWorld::instance()->getLoadScene() && !CWorld::instance()->getLoadScene()->getSceneSource().empty())
			m_cutSceneAnimEntity->m_AnimModel->onSetEnable(false);
		else
			m_cutSceneAnimEntity->m_AnimModel->onSetEnable(true);
	}
	else
		m_cutSceneAnimEntity->m_AnimModel->onSetEnable(false);

	m_cutSceneAnimEntity->m_AnimModel->onForceEMotion(true);	
	m_cutSceneAnimEntity->m_AnimModel->onSetUpdateBB(false);

	IMatrix mat;
	mat.Identity();

	m_cutSceneAnimEntity->m_AnimModel->onSetPlace(&mat);
	m_cutSceneAnimEntity->m_AnimModel->update(0);

#ifdef EDITOR
	if (m_propPanel)
	{
		m_propPanel->m_ModelNameTxt->SetLabelText(wxString(m_ModelName));
		m_propPanel->m_AnimGrafTxt->SetLabelText(wxString(m_AnimGrafName));
	}
#endif

}

void AnimModelPlayer::SetAnimGrafName(const char* _name)
{
	IAnimationInstance* m_AnimTemp = m_cutSceneAnimEntity->m_Anim;
	m_cutSceneAnimEntity->m_Anim = NULL;

	m_AnimGrafName = _name;

	if (!m_cutSceneAnimEntity)
		return;

	m_cutSceneAnimEntity->m_Anim = core.Animation()->LoadAnimation(_name);
	if (m_cutSceneAnimEntity->m_Anim && m_cutSceneAnimEntity->m_AnimModel)
	{
		((CAnimModel*)m_cutSceneAnimEntity->m_AnimModel)->SetAnimation(m_cutSceneAnimEntity->m_Anim);
		m_cutSceneAnimEntity->m_Anim->SetEvents(m_cutSceneAnimEntity);

		if (!g_GameProxy.EditorMode)
		{
			m_cutSceneAnimEntity->m_Anim->Pause(true);
		}
	}

#ifdef EDITOR
	if (m_propPanel)
		m_propPanel->m_AnimGrafTxt->SetLabelText(wxString(m_AnimGrafName));
#endif

	animTrack->SetAnimation(m_cutSceneAnimEntity->m_Anim);

	if (m_AnimTemp)
	{
		m_AnimTemp->Release();
		m_AnimTemp = NULL;
	}
}

void AnimModelPlayer::LoadProperties(StreamLoader* stream)
{
	GameBaseEntityPlayer::LoadProperties(stream);

	char name[64];
	name[0] = 0;

	stream->ReadString("ModelName", name);
	SetModelName(name);

	name[0] = 0;
	stream->ReadString("AnimGrafName", name);
	SetAnimGrafName(name);
}

void AnimModelPlayer::SaveProperties(StreamSaver* stream)
{
	GameBaseEntityPlayer::SaveProperties(stream);

	stream->SaveString("ModelName", m_ModelName.c_str());
	stream->SaveString("AnimGrafName", m_AnimGrafName.c_str());
}