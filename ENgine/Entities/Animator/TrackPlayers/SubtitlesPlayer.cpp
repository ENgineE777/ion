
#include "SubtitlesPlayer.h"
#include "DeepBlack/World.h"
#include "DeepBlack/Cutscene/CutsceneManager.h"

TRACKDESC(SubtitlesPlayer, "SubtitlesPlayer", 0)

void SubtitlesPlayer::Init()
{	
	AddStringDataTrack( Name , NULL, (TrackPlayer::ChangeKeyFunc)&SubtitlesPlayer::ExecuteFunc)	
}

void SubtitlesPlayer::EndPlay()
{
	SSubtitlesData Data;
	strcpy(Data.Name, ((StringDataTrack*)tracks[0])->GetCurValue().val);
	CEvent::instance()->Throw(EVENT_HIDE_SUBTITLES, (SEventContainer*)&Data);
}

void SubtitlesPlayer::ExecuteFunc(TrackStringPoint* func)
{	
	if ( !CWorld::instance()->getUpdateScene() )
		return;

	SSubtitlesData Data;
	strcpy(Data.Name, func->val);
	

#ifdef EDITOR
	if (g_GameProxy.EditorMode)
	{
		if (CWorld::instance()->getUpdateScene()->getCutSceneManager()->ed_played && !CWorld::instance()->getUpdateScene()->getCutSceneManager()->ed_playBack)
			CEvent::instance()->Throw(EVENT_SHOW_SUBTITLES, (SEventContainer*)&Data);
	}
	else
		CEvent::instance()->Throw(EVENT_SHOW_SUBTITLES, (SEventContainer*)&Data);

#else
		CEvent::instance()->Throw(EVENT_SHOW_SUBTITLES, (SEventContainer*)&Data);
#endif
			
}