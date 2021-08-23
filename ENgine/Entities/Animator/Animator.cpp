
#include "Animator.h"

ENTITY(Animator)
ENTITY_PARAMS(Animator,"Animator","Animator", "Animator")

void Animator::Init()
{	
	need_call_post_load = false;

	PROP_REG( timeLenght )
	PROP_REG( looped )
	PROP_REG( funcName )
				
	SetUpdate((TaskHolder::Task)&Animator::Worker, lvl_work1);

#ifdef EDITOR
	wnd = null;
#endif

	cur_time = 0.0f;
	ed_cur_time = 0.0f;
	from_time = 0.0f;
	to_time = 0.0f;

	playing = false;
}

void Animator::ReInit()
{
	cur_time = 0.0f;
	ed_cur_time = 0.0f;

	from_time = 0.0f;
	to_time = timeLenght;

	if (need_call_post_load)
	{
		for (int i=0;i<players.size();i++)
		{
			players[i]->PostLoadInit();
		}

		need_call_post_load = false;
	}

#ifdef EDITOR
	if (edit_mode)
	{
		wnd->CreateAnimatorWidgets(this);
	}
#endif
}

void Animator::Activate(bool set_active)
{
	SceneEntity::Activate(set_active);

	if (set_active)
	{
		playing = true;
	}
	else
	{
		playing = false;
	}
}

bool Animator::Start()
{
	cur_time = 0.0f;

	for (int i=0;i<players.size();i++)
	{
		players[i]->Reset();
	}

	return true;
}

void Animator::LoadProperties(IStreamLoader* stream)
{
	SceneEntity::LoadProperties(stream);
	
	int num = 0;
	stream->Read("NumPlayers", num);
	players.resize(num);

	for (int i=0;i<num;i++)
	{		
		if (!stream->EnterBlock("TrackPlayer")) break;

		char tp[64];
		stream->Read("Type", tp, 64);

		players[i] = TrackPlayerDeclBase::Create(tp);

		if (players[i])
		{
			players[i]->SetOwner(this);
			players[i]->SetName(tp);
			players[i]->SetType(tp);

			players[i]->Init();

			players[i]->Load(stream);
		}

		stream->LeaveBlock();
	}

	need_call_post_load = true;
}

void Animator::SaveProperties(IStreamSaver* stream)
{
	SceneEntity::SaveProperties(stream);
	
	stream->Write("NumPlayers", (int)players.size());

	for (int i=0;i<players.size();i++)
	{
		stream->MarkBeginBlock("TrackPlayer");

		players[i]->Save(stream);

		stream->MarkEndBlock("TrackPlayer");
	}
}

void Animator::SetEditMode(bool ed)
{
	SceneEntity::SetEditMode(ed);

#ifdef EDITOR
	if (!wnd)
	{
		if (!AnimatorWindow::instance)
		{			
			AnimatorWindow::instance = new AnimatorWindow();
			AnimatorWindow::instance->Init();			
		}

		wnd = AnimatorWindow::instance;
	}

	wnd->CreateAnimatorWidgets(this);
	wnd->Show(ed);
#endif
}

void Animator::Worker(float dt, int level)
{	
#ifdef EDITOR
	if (!Scene()->IsRunning() && edit_mode)
	{
		if (wnd)
		{
			if (gizmo) wnd->SetGizmo(gizmo);
			wnd->Update();
		}
	}
#endif

	if (Scene()->IsRunning() && playing)
	{
		cur_time += dt;

		if (cur_time>timeLenght - 0.001f)
		{
			if (looped)
			{
				cur_time = 0.0f;

				for (int i=0;i<players.size();i++)
				{
					players[i]->Reset();
				}
			}
			else
			{
				cur_time = timeLenght;
		
				Activate(false);				

				Scene()->CallScriptFunc(funcName);
			}
		}

		for (int i=0;i<players.size();i++)
		{
			players[i]->SetTime(cur_time);
		}
	}
}

CALLWRAPPER(Animator, Reset, CALLWRAPPERNORET())	

void Animator::MethodsToScript()
{
	core->Script()->Machine()->RegisterObjectMethod(Desc()->GetName(), "void Reset()", CALLWRAPPERNAMEAS(Reset));
}

void Animator::Reset()
{
	cur_time = 0.0f;

	for (int i=0;i<players.size();i++)
	{
		players[i]->Reset();
	}
}