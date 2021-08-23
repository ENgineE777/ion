
#include "CameraPlayer.h"
#include "Core/Core.h"

TRACKPLAYER(CameraPlayer, "Camera")

void CameraPlayer::Init()
{		
	AddTransformDataTrack( Transform ,0, false, (TrackPlayer::KeyFunc)&CameraPlayer::ApplyTrans, NULL)
	AddFloatDataTrack( Fov , (TrackPlayer::KeyFunc)&CameraPlayer::ApplyFov, NULL, 45.0f)
}

#ifdef EDITOR

void CameraPlayer::OnStopEdit()
{
	TrackPlayer::OnStopEdit();

	Matrix proj;
	proj.BuildProjection(45.0f * RADIAN, 0.75f, 0.1f, 1500.0f );
	en_core.Render()->Display()->SetProj( proj );
}

#endif

void CameraPlayer::ApplyTrans(TrackPoint* point)
{
	TransformTrackPoint* tr = (TransformTrackPoint*)point;

	Matrix view;
	tr->rot.GetMatrix(view);

	Vector tmp( 0, 1, 0 );

	view.BuildView( tr->pos, tr->pos + view.Vz(), tmp );
		
	en_core.Render()->Display()->SetView( view );
}

void CameraPlayer::ApplyFov(TrackPoint* point)
{ 		
	TrackFloatPoint* fov = (TrackFloatPoint*)point;

	Matrix proj;
	proj.BuildProjection(fov->val * RADIAN, 0.75f, 0.1f, 1500.0f );
	en_core.Render()->Display()->SetProj( proj );
}