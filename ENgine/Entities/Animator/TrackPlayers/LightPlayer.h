#pragma once

#include "GameBaseEntityPlayer.h"

class LightPlayer : public GameBaseEntityPlayer
{		
public:

	enum enumType
	{
		ePoint,
		eSpot,
	};

	LightPlayer(TrackDescBase* Desc): GameBaseEntityPlayer(Desc)
	{
		m_GameLight = NULL;
		GameBaseEntityPlayer::ms_AttachedToTypes.insert("LightPlayer");
	};

	virtual void Init();

	virtual void StartPlay();
	virtual void EndPlay();

	virtual class CGameBaseEntity* GetEntity();

	void ApplyDefault();

	void ApplyTrans(TransformTrackPoint* tr);	
	void ApplyColor(class Vector4DataTrackPoint* _value);
	void ApplyLightAtten(TrackFloatPoint* _value);
	void ApplyLightCone(TrackFloatPoint* _value);
	void ApplyIntensity(TrackFloatPoint* _value);
	void ApplyActivity(class BoolDataTrackPoint* _value);

	

	virtual void ReInit();

#ifdef EDITOR
	virtual void InitControls(wxWindow* prop_dock);		
	virtual void StartEditProp();	
	virtual void StopEditProp();
	virtual void DebugDraw();	
#endif

	virtual void Release();

private:
	class CGameLight* m_GameLight;
	STRING_PROP(LightPlayer,m_PropName, "", "Name")
	ENUM_PROP(LightPlayer,m_PropType,0, "Type")
		ENUM_ITEM("Point",ePoint)
		ENUM_ITEM("Spot",eSpot)
	END_ENUM(LightPlayer,m_PropType)
	BOOL_PROP(LightPlayer, m_PropLightShadow, 0, "Shadow")
	//BOOL_PROP(LightPlayer, m_PropRenderOffscreen, 0, "RenderOffscreen")
	BOOL_PROP(LightPlayer, m_PropSpecular, 0, "Specular")

};