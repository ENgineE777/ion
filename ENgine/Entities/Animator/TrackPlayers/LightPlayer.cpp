
#include "LightPlayer.h"
#include "DeepBlack/World.h"
#include "DeepBlack/WorldCamera.h"
#include "DeepBlack/GameLight.h"


TRACKDESC(LightPlayer, "LightPlayer", 0)

void LightPlayer::Init()
{	
	GameBaseEntityPlayer::Init();

	AddTransformDataTrack( Transform ,5, false,(TrackPlayer::KeyFunc)&LightPlayer::ApplyTrans, NULL)
	AddVector4DataTrack( DiffuseColor , (TrackPlayer::KeyFunc)&LightPlayer::ApplyColor, NULL, IVector4(0.f, 0.f, 0.f, 1.f))	
	AddFloatDataTrack( Distance , (TrackPlayer::KeyFunc)&LightPlayer::ApplyLightAtten, NULL, 10.f)	
	AddFloatDataTrack( ConeAngle , (TrackPlayer::KeyFunc)&LightPlayer::ApplyLightCone, NULL, 2.f)	
	AddFloatDataTrack( Intensity , (TrackPlayer::KeyFunc)&LightPlayer::ApplyIntensity, NULL, 1.f)	
	AddBoolDataTrack( Activity , NULL, (TrackPlayer::ChangeKeyFunc)&LightPlayer::ApplyActivity, true, false)	


	PROP_REG(m_PropType)
	PROP_REG(m_PropName)
	PROP_REG(m_PropLightShadow);
	//PROP_REG(m_PropRenderOffscreen);
	PROP_REG(m_PropSpecular);
	
	CScene* scene = CWorld::instance()->getUpdateScene();

	if(CWorld::instance()->getLoadScene() && !CWorld::instance()->getLoadScene()->getSceneSource().empty())
		scene = CWorld::instance()->getLoadScene();


	if (!scene)
		return;

	m_GameLight = scene->createLight(m_PropName);
	m_GameLight->setFlag( true, CBaseEntity::eSimulate );

	if (g_GameProxy.EditorMode)
		ReInit();
}

CGameBaseEntity* LightPlayer::GetEntity()
{
	return m_GameLight;
}

void LightPlayer::ReInit()
{
	GameBaseEntityPlayer::ReInit();

	m_GameLight->onSetShadowCast(m_PropLightShadow);
	m_GameLight->onSetLightColor(&IVector4(1.f, 1.f, 1.f, m_PropSpecular), NULL);
	//m_GameLight->onSetOffscreenRender(m_PropRenderOffscreen);

	if (m_PropType == ePoint)
		m_GameLight->onSetLightVolume(&IVector4(3.15f, 3.15f, 3.15f, 15.f));
	else if (m_PropType == eSpot)
		m_GameLight->onSetLightVolume(&IVector4(1.f, 1.f, 1.f, 15.f));

	m_GameLight->onSetEnable(true);
};

void LightPlayer::ApplyTrans(TransformTrackPoint* tr)
{
	if (m_GameLight)
	{		
		IMatrix mat;
		tr->rot.GetMatrix(mat);
		mat.Pos() = tr->pos;

		NxMat34 nxmat;
		mat.Get(nxmat);
		m_GameLight->setTransform(nxmat);
		
	}
}

void LightPlayer::ApplyColor(Vector4DataTrackPoint* _value)
{
	if (m_GameLight)
	{		
		m_GameLight->onSetLightColor(&IVector4(_value->m_value.x,
			_value->m_value.y, _value->m_value.z, m_PropSpecular), NULL);
	}
}


void LightPlayer::ApplyLightAtten(TrackFloatPoint* _value)
{
	if (m_GameLight)
	{		
		IVector4 v;
		m_GameLight->onGetLightVolume(&v);
		v.w = _value->val;
		m_GameLight->onSetLightVolume(&v);
	}
}

void LightPlayer::ApplyLightCone(TrackFloatPoint* _value)
{
	if (m_GameLight)
	{		
		//Spot
		if (m_PropType == eSpot)
		{
			IVector4 v;
			m_GameLight->onGetLightVolume(&v);
			v.x = v.y = v.z = _value->val;
			m_GameLight->onSetLightVolume(&v);
		}
		//Point
		else if (m_PropType == ePoint)
		{
			IVector4 v;
			m_GameLight->onGetLightVolume(&v);
			v.x = v.y = v.z = 3.15f;
			m_GameLight->onSetLightVolume(&v);			
		}

	}
}



void LightPlayer::ApplyIntensity(TrackFloatPoint* _value)
{
	if (m_GameLight)
	{		
		IVector4 v;
		m_GameLight->onGetLightColor(&v, NULL);
		v = v * _value->val;
		v.w = m_PropSpecular;
		m_GameLight->onSetLightColor(&v, NULL);
	}
}

void LightPlayer::ApplyActivity(BoolDataTrackPoint* _value)
{
	if (m_GameLight)
	{		
		m_GameLight->onSetEnable(_value->m_value);
	}
}

void LightPlayer::StartPlay()
{
}

void LightPlayer::EndPlay()
{
	if (m_GameLight && !m_isEntityUsedAfterEndPlay)
	{		
		m_GameLight->onSetEnable(false);
	}
}

void LightPlayer::ApplyDefault()
{

}

#ifdef EDITOR

void LightPlayer::InitControls(wxWindow* prop_dock)
{
	TrackPlayer::InitControls(prop_dock);
}

void LightPlayer::StartEditProp()
{	
	__super::StartEditProp();
	
}

void LightPlayer::StopEditProp()
{	
	__super::StopEditProp();
}

void LightPlayer::DebugDraw()
{
	TrackPlayer::DebugDraw();

	if (!m_GameLight)
		return;

	if (m_PropType == eSpot)
	{
		IVector4 volume;
		m_GameLight->onGetLightVolume(&volume);
		float cone = volume.x;
		float atten = volume.w;

		IVector dir(0, 0, 1);
		IVector coneLine = dir;
		IQuaternion coneLineQuat;
		coneLineQuat.Set(	cone/2.f, IVector(1.f, 0.f, 0.f) );
		coneLineQuat.Rotate(coneLine);


		IMatrix mat(m_GameLight->getTransform());
		
		IQuaternion quat;
		quat.Set(mat);
		quat.Rotate(dir);
		quat.Rotate(coneLine);

		IQuaternion rotateBydir;
		rotateBydir.Set(	0.2f, dir	);

		IVector prevPoint = mat.Pos() + coneLine * (atten/cos(cone/2.f));
		for (UINT i = 0; i < 30; ++i)
		{
			rotateBydir.Rotate(coneLine);
			IVector point2 = mat.Pos() + coneLine * (atten/cos(cone/2.f));

			core.Render()->DrawLine(mat.Pos(), COLOR_WHITE, point2, COLOR_WHITE);
			prevPoint = point2;
		}

	}

	else if (m_PropType == ePoint)
	{
		IVector4 volume;
		m_GameLight->onGetLightVolume(&volume);
		float cone = volume.x;
		float atten = volume.w;

		IMatrix curmat(m_GameLight->getTransform());

		for (UINT i = 0; i < 30; ++i)
		{
			IVector dir(oleg_math::rangeRand(-1.f, 1.f), oleg_math::rangeRand(-1.f, 1.f), oleg_math::rangeRand(-1.f, 1.f));
			IVector point2 = curmat.Pos() + dir;

			core.Render()->DrawLine(curmat.Pos(), COLOR_WHITE, point2, COLOR_WHITE);			
		}

		IMatrix mat;
		m_GameLight->onGetPlace(&mat);
		IMatrix imat(mat);
		core.Render()->DrawSphere(imat.Pos(), atten, COLOR_WHITE);
		//g_GameProxy.DebugSphere->AddCircle(0, imat, atten, 0xffffffff, 0xaaaaaaaa);
		//g_GameProxy.DebugSphere->AddCircle(1, imat, atten, 0xffffffff, 0xaaaaaaaa);
		//g_GameProxy.DebugSphere->AddCircle(2, imat, atten, 0xffffffff, 0xaaaaaaaa);

	}
	
	





}


#endif

void LightPlayer::Release()
{
	ApplyDefault();	
	if (m_GameLight)
	{
		CScene* scene = CWorld::instance()->getUpdateScene();
		if (scene)
		{
			scene->deleteLight(m_GameLight);
			m_GameLight = NULL;
		}
	}

	GameBaseEntityPlayer::Release();
}


