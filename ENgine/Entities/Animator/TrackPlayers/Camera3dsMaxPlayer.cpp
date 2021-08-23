
#include "Camera3dsMaxPlayer.h"
#include "DeepBlack/World.h"
#include "DeepBlack/WorldCamera.h"

#include "Core/Core.h"

#ifdef EDITOR
#include "Platforms/PC/Editor/PanelCutScene.h"
#include "Platforms/PC/Editor/EditorMainWnd.h"
#endif

TRACKDESC(Camera3dsMaxPlayer, "Camera3dsMax", 0)

Camera3dsMaxPlayer::SCameraAnimationRes::SCameraAnimationRes()
{
	NumKeys = 0;
	Keys = 0;
}

void Camera3dsMaxPlayer::SCameraAnimationRes::Release()
{
	if (Keys) delete[] Keys;
	Keys  = NULL;
}

bool Camera3dsMaxPlayer::SCameraAnimationRes::Load(const char* animFileName)
{
	Release();

#ifdef PC
	static const uint g_AnimationMagicID = MAKEFOURCC('A', 'N', 'I', 'M');
	static const uint g_AnimationMotionID = MAKEFOURCC('M', 'O', 'T', 'A');
#endif

#if defined(_XBOX) || defined(PS3)
	static const uint g_AnimationMagicID = MAKEFOURCC('M', 'I', 'N', 'A');
	static const uint g_AnimationMotionID = MAKEFOURCC('A', 'T', 'O', 'M');
#endif

	File* hFile = core.Files()->Open(animFileName, "anim", _FL_);

	if (!hFile)
	{		
		return false;
	}

	core.Trace("SplineCamera::Load %s ", animFileName);

	Mesh::AnimHeader Header;
	hFile->Read(&Header, (dword)sizeof(Header));

	if ( (g_AnimationMagicID != Header.magicID) && (g_AnimationMotionID != Header.magicID) )
	{		
		core.Trace( "Invcorect format for m_animation file - %s", animFileName);
		hFile->Release();

		return false;
	}

	NumKeys = Header.numKeys;	

	if (NumKeys)
	{
		Keys = new Mesh::AnimKey[NumKeys];
		hFile->Seek( Header.keysOffset, 0);
		hFile->Read( Keys, (dword)sizeof(Mesh::AnimKey) * NumKeys);		
	}

	hFile->Release();

	return true;
}

void Camera3dsMaxPlayer::Init()
{		
	CameraPlayer::Init();

	PROP_REG(m_FileNameProp)
}

void Camera3dsMaxPlayer::ReInit()
{
	CameraPlayer::ReInit();

	m_animationRes.Load(m_FileNameProp);
	TransformDataTrack* track = (TransformDataTrack*)GetDataTrack("Transform");
	track->ClearPoints();

	for (uint i = 0; i < m_animationRes.NumKeys; ++i)
	{
		 TransformTrackPoint point;
		 Mesh::AnimKey& key = m_animationRes.Keys[i];

		 point.rot = key.rotation;
		 point.pos = key.translation;
		 point.time = key.time / 192000;
		 track->AddSortedPoint(point);
	}

	track->Prepare();

#ifdef EDITOR
	if (MainFrame::tools)
		MainFrame::tools->cutscene->UpdateControls();
#endif


}

void Camera3dsMaxPlayer::Release()
{
	m_animationRes.Release();

	CameraPlayer::Release();
}

