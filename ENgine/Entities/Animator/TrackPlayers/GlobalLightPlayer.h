#pragma once

#include "TrackPlayer.h"
#include "DeepBlack/Scene.h"

class GlobalLightPlayer : public TrackPlayer
{		
public:

	GlobalLightPlayer(TrackDescBase* Desc): TrackPlayer(Desc)
	{
	};

	virtual void Init();
	virtual void ReInit();
	virtual void Release();

	virtual void Reset();
	virtual void StartPlay();
	virtual void EndPlay();

#ifdef EDITOR
	virtual void OnStopEdit();
	virtual void DebugDraw();	
#endif

	void ApplyTrans(TransformTrackPoint* tr);	
	void ApplyOverWaterDif(class Vector4DataTrackPoint* _value);
	void ApplyUnderWaterDif(class Vector4DataTrackPoint* _value);
	//void ApplyOverWaterAmb(class Vector4DataTrackPoint* _value);
	//void ApplyUnderWaterAmb(class Vector4DataTrackPoint* _value);

	





private:
	void ResetToInitGlobalLight();

	sEnvironmentData m_initEnvironmentData;
	IVector m_InitLightPlace;
	std::string	m_InitAmbientCubeMapOW;
	std::string	m_InitAmbientCubeMapUW;


	INT_PROP(GlobalLightPlayer, m_PropShadow, 0, "Shadow")
	STRING_PROP(GlobalLightPlayer,m_PropOverWaterAmbientCubeMap, "", "AmbientCubeMap")
	STRING_PROP(GlobalLightPlayer,m_PropUnderWaterAmbientCubeMap, "", "UWAmbientCubeMap")
	//BOOL_PROP(GlobalLightPlayer, m_PropSpecular, 1, "Specular")
};