#pragma once

#include "TrackPlayers/TrackPlayer.h"
#include "Core/Core.h"
#include "AnimatorWindow.h"

class Animator : public SceneEntity
{		
	bool need_call_post_load;

#ifdef EDITOR
	AnimatorWindow* wnd;
#endif

public:
	
	float cur_time;
	float ed_cur_time;
	float from_time;
	float to_time;

	bool  playing;

	std::vector<TrackPlayer*> players;

	FLOAT_PROP(Animator, timeLenght, 3.0f, "Properties", "TimeLenght")
	BOOL_PROP(Animator, looped, false, "Properties", "Looped")
	STRING_PROP(Animator, funcName, "", "Properties", "FinishFunc")

	ENTITY_DEFC(Animator)

	virtual void Init();
	virtual void ReInit();

	virtual void Activate(bool set_active);

	virtual bool Start();

	virtual void LoadProperties(IStreamLoader* loader);
	virtual void SaveProperties(IStreamSaver* saver);

	virtual void SetEditMode(bool ed);
	void Worker(float dt, int level);
	void Reset();

	virtual void MethodsToScript();
};
