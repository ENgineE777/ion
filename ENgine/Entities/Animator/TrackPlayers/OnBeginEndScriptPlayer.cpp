
#include "OnBeginEndScriptPlayer.h"
#include "DeepBlack/World.h"
#include "Core/Core.h"

TRACKDESC(OnBeginEndScriptPlayer, "OnBeginEndScriptPlayer", 0)

void OnBeginEndScriptPlayer::Init()
{	
	//AddStringDataTrackNoBlend( Name , NULL, (TrackPlayer::ChangeKeyFunc)&ScriptPlayer::ExecuteFunc)	
	AddStringDataTrackNoBlend( Script_Functions , NULL, NULL)	
	
}

void OnBeginEndScriptPlayer::ExecuteFunc(const TrackStringPoint* func)
{	
	CScene* scene =  CWorld::instance()->getUpdateScene();
	if (!scene)
		return;
	scene->getSceneName();
	std::string filename = scene->getSceneSource() + "_script";
	CLuaScript script = core.Lua()->GetScript(filename.c_str(), LUALIB_ALL, CScriptUtils::instance()->GetLib());
	if (script)
		script.CallCoroutine(func->val);
}

void OnBeginEndScriptPlayer::StartPlay()
{
	StringDataTrack* track = static_cast<StringDataTrack*>(tracks[0]);
	if ( track->GetPoints().size() > 0 && strlen(track->GetPoints()[0].val) != 0)
	{
		ExecuteFunc(&track->GetPoints()[0]);
	}
}
void OnBeginEndScriptPlayer::EndPlay()
{
	StringDataTrack* track = static_cast<StringDataTrack*>(tracks[0]);
	if ( track->GetPoints().size() > 1 && strlen(track->GetPoints()[1].val) != 0)
		ExecuteFunc(&track->GetPoints()[1]);
}