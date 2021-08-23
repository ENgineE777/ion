#pragma once
#include "Core/Core.h"

class ScriptExecutor : public SceneEntity
{
public:
	
	PROP_START
	STRING_PROP(ScriptExecutor, funcName, "", "Properties", "FuncName")
	BOOL_PROP(ScriptExecutor, act_once, false, "Properties", "ActOnce")
	FLOAT_PROP(ScriptExecutor, delayTime, 0.0f, "Properties", "Delay")
	INT_PROP(ScriptExecutor, priority, 0, "Properties", "Priority")

	ENTITY_DEFC(ScriptExecutor)

	ScriptFunction callback;

	virtual void RegisterProperties();
	virtual void Init();
	virtual void Activate(bool set_active);	
	virtual bool Start();
	virtual void Finish();
	virtual void SetCallback(const char* callbackname);
	virtual void Work(float dt, int level);	
};
