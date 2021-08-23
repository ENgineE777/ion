
#include "../CutSceneManager.h"

#include "ColorCorrectionPlayer.h"
#include "../DataTrack/Vector4DataTrack.h"
#include "DeepBlack/World.h"


TRACKDESC(ColorCorrectionPlayer, "ColorCorrectionPlayer", 0)

void ColorCorrectionPlayer::Init()
{	
	AddVector4DataTrack( Bloom , (TrackPlayer::KeyFunc)&ColorCorrectionPlayer::ExecuteFunc, NULL, IVector4(0.f, 0.f, 0.f, 1.f))	
	AddVector4DataTrack( ColorBase , NULL, NULL, IVector4(0.f, 0.f, 0.f, 1.f))	
	AddVector4DataTrack( ColorTint , NULL, NULL, IVector4(0.f, 0.f, 0.f, 1.f))	
	AddVector4DataTrack( ColorTintSq , NULL, NULL, IVector4(0.f, 0.f, 0.f, 1.f))	
	AddVector4DataTrack( ColorShift , NULL, NULL, IVector4(0.f, 0.f, 0.f, 1.f))	
	AddFloatDataTrack( Weight , NULL, NULL, 1.0f)	
	AddFloatDataTrack( Brightness , NULL, NULL, 0.0f)	

#ifdef EDITOR
	tracks[0]->AddChild(tracks[1]);
	tracks[0]->AddChild(tracks[2]);
	tracks[0]->AddChild(tracks[3]);
	tracks[0]->AddChild(tracks[4]);
	tracks[0]->AddChild(tracks[5]);
	tracks[0]->AddChild(tracks[6]);

	tracks[1]->SetParent(tracks[0]);
	tracks[2]->SetParent(tracks[0]);
	tracks[3]->SetParent(tracks[0]);
	tracks[4]->SetParent(tracks[0]);
	tracks[5]->SetParent(tracks[0]);
	tracks[6]->SetParent(tracks[0]);
#endif	
}

void ColorCorrectionPlayer::ExecuteFunc(Vector4DataTrackPoint* _curvalue)
{	
	Vector4DataTrack* vector4DataTrackBloom = (Vector4DataTrack*)tracks[0];
	Vector4DataTrack* vector4DataTrackColorBase = (Vector4DataTrack*)tracks[1];
	Vector4DataTrack* vector4DataTrackColorTint = (Vector4DataTrack*)tracks[2];
	Vector4DataTrack* vector4DataTrackColorTintSq = (Vector4DataTrack*)tracks[3];
	Vector4DataTrack* vector4DataTrackColorShift = (Vector4DataTrack*)tracks[4];
	FloatDataTrack* floatDataTrackWeight = (FloatDataTrack*)tracks[5];
	FloatDataTrack* floatDataTrackBrightness = (FloatDataTrack*)tracks[6];

	CScene* UpdateScene = CWorld::instance()->getUpdateScene();

	if(UpdateScene /*&& vector4DataTrack->GetCurIndex() < floatDataTrack->GetKeysCount()*/)
	{
		UpdateScene->setActionColorZone(CColorZones::CZ_GAME_OVER, 
			(IVector4&)vector4DataTrackColorBase->GetCurValue().m_value,
			(IVector4&)vector4DataTrackColorTint->GetCurValue().m_value, 
			(IVector4&)vector4DataTrackColorTintSq->GetCurValue().m_value,
			(IVector4&)vector4DataTrackColorShift->GetCurValue().m_value, 
			(IVector4&)vector4DataTrackBloom->GetCurValue().m_value,
			floatDataTrackWeight->GetCurValue().val);

		IVector bright(floatDataTrackBrightness->GetCurValue().val,floatDataTrackBrightness->GetCurValue().val,floatDataTrackBrightness->GetCurValue().val);
		UpdateScene->SetBrightness(bright);
	}
}