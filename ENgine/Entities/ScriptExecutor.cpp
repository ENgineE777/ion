
#include "ScriptExecutor.h"

ENTITY(ScriptExecutor)
ENTITY_PARAMS(ScriptExecutor,"ScriptExecutor","Logic", "Script Executor")

void ScriptExecutor::RegisterProperties()
{
	SceneEntity::RegisterProperties();

	PROP_REG( funcName )		
	PROP_REG( act_once )
	PROP_REG( delayTime )
	PROP_REG( priority )
}

void ScriptExecutor::Init()
{			
}

void ScriptExecutor::Activate(bool set_active)
{
	SceneEntity::Activate(set_active);

	if (!Scene()->IsRunning()) return;

	if (IsActive())
	{
		float freq = -1.0f;
		if (delayTime>0.01f) freq = delayTime;

		SetUpdate((ScriptExecutor::Task)&ScriptExecutor::Work, lvl_work1 - priority, freq);		
	}
	else
	{
		DelUpdate((ScriptExecutor::Task)&ScriptExecutor::Work);
	}
}

bool ScriptExecutor::Start()
{
	Activate(IsActive());

	SetCallback(funcName.c_str());	

	return true;
}

void ScriptExecutor::SetCallback(const char* callbackname)
{		
	if (act_once)
	{
		ScriptFunction::Find(&callback, Scene()->ScriptModule(), callbackname);		
	}
	else
	{
		ScriptFunction::Find1F(&callback, Scene()->ScriptModule(), callbackname);
	}
}

void ScriptExecutor::Finish()
{
	DelUpdate((ScriptExecutor::Task)&ScriptExecutor::Work);
	callback.Reset();
}

void ScriptExecutor::Work(float dt, int level)
{		
	if (act_once)
	{
		core->Script()->Machine()->Call(&callback);		

		DelUpdate((ScriptExecutor::Task)&ScriptExecutor::Work);

		return;
	}

	core->Script()->Machine()->AddFloatArg(dt);
	core->Script()->Machine()->Call(&callback);
}

