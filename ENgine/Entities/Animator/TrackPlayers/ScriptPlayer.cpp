
#include "ScriptPlayer.h"
#include "DeepBlack/World.h"
#include "Core/Core.h"

TRACKDESC(ScriptPlayer, "ScriptPlayer", 0)

void ScriptPlayer::Init()
{	
	AddStringDataTrack( Name , NULL, (TrackPlayer::ChangeKeyFunc)&ScriptPlayer::ExecuteFunc)	
}

void ScriptPlayer::ExecuteFunc(TrackStringPoint* func)
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