
#include "GlobalLightPlayer.h"
#include "AnimModelPlayer.h"
//#include "AnimModel.h"
#include "DeepBlack/World.h"
#include "DeepBlack/WorldCamera.h"
#include "../CutScene.h"
#include "Core/Core.h"
//#include "Entity.h"
//#include "GameLight.h"

TRACKDESC(GlobalLightPlayer, "GlobalLightPlayer", 0)

void GlobalLightPlayer::Init()
{	

	CScene* scene = CWorld::instance()->getUpdateScene();

	if(CWorld::instance()->getLoadScene() && !CWorld::instance()->getLoadScene()->getSceneSource().empty())
		scene = CWorld::instance()->getLoadScene();

	if (scene)
	{
		m_initEnvironmentData = scene->getEnvironmentData();
		m_InitLightPlace = scene->getLightPlace();
		scene->getAmbientCubemaps(m_InitAmbientCubeMapOW, m_InitAmbientCubeMapUW);
	}


	AddTransformDataTrackWithDefaultKey( Transform ,5, false,(TrackPlayer::KeyFunc)&GlobalLightPlayer::ApplyTrans, NULL, m_InitLightPlace)
	AddVector4DataTrack( OverWaterDif , (TrackPlayer::KeyFunc)&GlobalLightPlayer::ApplyOverWaterDif, NULL, (IVector4&)m_initEnvironmentData.m_overDif)	
	AddVector4DataTrack( UnderWaterDif , (TrackPlayer::KeyFunc)&GlobalLightPlayer::ApplyUnderWaterDif, NULL, (IVector4&)m_initEnvironmentData.m_underDif)	
	//AddVector4DataTrack( OverWaterAmb , (TrackPlayer::KeyFunc)&GlobalLightPlayer::ApplyOverWaterAmb, NULL,IVector4(0.f, 0.f, 0.f, 1.f))	
	//AddVector4DataTrack( UnderWaterAmb , (TrackPlayer::KeyFunc)&GlobalLightPlayer::ApplyUnderWaterAmb, NULL, IVector4(0.f, 0.f, 0.f, 1.f))	

	PROP_REG(m_PropShadow)
	PROP_REG(m_PropOverWaterAmbientCubeMap)
	PROP_REG(m_PropUnderWaterAmbientCubeMap);
	//PROP_REG(m_PropSpecular);

	m_PropShadow =  m_initEnvironmentData.m_sunCastShadow;
	strcpy(m_PropOverWaterAmbientCubeMap, m_InitAmbientCubeMapOW.c_str());
	strcpy(m_PropUnderWaterAmbientCubeMap, m_InitAmbientCubeMapUW.c_str());
	
	ReInit();
}


void GlobalLightPlayer::ReInit()
{
	if(CWorld::instance()->getLoadScene() && !CWorld::instance()->getLoadScene()->getSceneSource().empty())
	{
		return;
	}

	CScene* scene = CWorld::instance()->getUpdateScene();
	if (!scene)
		return;

	scene->getEnvironmentData().m_sunCastShadow = m_PropShadow;
	scene->setAmbientCubemaps(m_PropOverWaterAmbientCubeMap, m_PropUnderWaterAmbientCubeMap);
	CWorld::instance()->setAmbientShadowCast(scene->getEnvironmentData().m_sunCastShadow);
	
};

void GlobalLightPlayer::Release()
{
#ifdef EDITOR
	CScene* scene = CWorld::instance()->getUpdateScene();
	if (!scene)
		return;

	if (!scene->isSceneClearing())
		ResetToInitGlobalLight();
#endif

	TrackPlayer::Release();
}

void GlobalLightPlayer::EndPlay()
{
	TrackPlayer::EndPlay();
	ResetToInitGlobalLight();
}

#ifdef EDITOR
void GlobalLightPlayer::OnStopEdit()
{
	TrackPlayer::OnStopEdit();
	ResetToInitGlobalLight();
}
#endif

void GlobalLightPlayer::Reset()
{
	TrackPlayer::Reset();
	ReInit();
}

void GlobalLightPlayer::StartPlay()
{
	TrackPlayer::StartPlay();
	ReInit();
}

void GlobalLightPlayer::ApplyTrans(TransformTrackPoint* tr)
{
	CScene* scene = CWorld::instance()->getUpdateScene();
	if (!scene)
		return;

	scene->setLightPlace( (IVector&)tr->pos);
}

void GlobalLightPlayer::ApplyOverWaterDif(Vector4DataTrackPoint* _value)
{
	CScene* scene = CWorld::instance()->getUpdateScene();
	if (!scene)
		return;

	scene->getEnvironmentData().m_overDif = (IVector4&)_value->m_value;
	scene->getEnvironmentData().m_overDif.w = 1.f;

	if (!CWorld::instance()->getWorldCamera()->isUnderWater())
		CWorld::instance()->setOverAmbientLight(&scene->getEnvironmentData().m_overDif, &scene->getEnvironmentData().m_overAmb);
}

//void GlobalLightPlayer::ApplyOverWaterAmb(Vector4DataTrackPoint* _value)
//{
//	CScene* scene = CWorld::instance()->getUpdateScene();
//	if (!scene)
//		return;
//
//	scene->getEnvironmentData().m_overAmb = (IVector4&)_value->m_value;
//	scene->getEnvironmentData().m_overAmb.w = 1;
//
//	scene->setOverLight();
//	//CWorld::instance()->setOverAmbientLight(&scene->getEnvironmentData().m_overDif, &scene->getEnvironmentData().m_overAmb);
//}

void GlobalLightPlayer::ApplyUnderWaterDif(Vector4DataTrackPoint* _value)
{
	CScene* scene = CWorld::instance()->getUpdateScene();
	if (!scene)
		return;

	scene->getEnvironmentData().m_underDif = (IVector4&)_value->m_value;
	scene->getEnvironmentData().m_underDif.w = 1;

	if (CWorld::instance()->getWorldCamera()->isUnderWater())
		CWorld::instance()->setUnderAmbientLight(&scene->getEnvironmentData().m_underDif, &scene->getEnvironmentData().m_underAmb);
}

//void GlobalLightPlayer::ApplyUnderWaterAmb(Vector4DataTrackPoint* _value)
//{
//	CScene* scene = CWorld::instance()->getUpdateScene();
//	if (!scene)
//		return;
//
//	scene->getEnvironmentData().m_underAmb = (IVector4&)_value->m_value;
//	scene->getEnvironmentData().m_underAmb.w = 1;
//
//	CWorld::instance()->setUnderAmbientLight(&scene->getEnvironmentData().m_underDif, &scene->getEnvironmentData().m_underAmb);
//}

void GlobalLightPlayer::ResetToInitGlobalLight()
{

	CScene* scene = CWorld::instance()->getUpdateScene();
	if (!scene)
		return;

	scene->getEnvironmentData() = m_initEnvironmentData; 

	scene->setLightPlace((IVector&)m_InitLightPlace);
	scene->setAmbientCubemaps(m_InitAmbientCubeMapOW.c_str(), m_InitAmbientCubeMapUW.c_str());
	CWorld::instance()->setAmbientShadowCast(scene->getEnvironmentData().m_sunCastShadow);

	if (!CWorld::instance()->getWorldCamera()->isUnderWater())
		CWorld::instance()->setOverAmbientLight(&scene->getEnvironmentData().m_overDif, &scene->getEnvironmentData().m_overAmb);
	else
		CWorld::instance()->setOverAmbientLight(&scene->getEnvironmentData().m_underDif, &scene->getEnvironmentData().m_underAmb);
}


#ifdef EDITOR


void GlobalLightPlayer::DebugDraw()
{
	TrackPlayer::DebugDraw();
	
	TransformDataTrack* transform = (TransformDataTrack*)GetDataTrack("Transform");

	core.Render()->DrawLine(transform->GetCurValue().pos, COLOR_WHITE, 0.0f, COLOR_WHITE);	
}


#endif




