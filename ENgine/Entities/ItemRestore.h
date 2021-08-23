#pragma once
#include "Core/Core.h"

class ItemRestore : public SceneEntity
{
	class ItemRestoreListiner : public OnlineUserStatsSystem::ItemRestoreListiner
	{
	public: 		

		virtual void OnItemRestoreSuccess(void* data, const char* id);
	};

	ItemRestoreListiner listiner;
	ScriptFunction callback_ok;

public:
		
	PROP_START
	STRING_PROP(ItemRestore, id, "", "Properties", "id")
	STRING_PROP(ItemRestore, funcSuccessName, "", "Properties", "funcSuccessName")

	ENTITY_DEFC(ItemRestore)

	virtual void RegisterProperties();
	virtual void Init();	
	virtual bool Start();
	virtual void Finish();
	virtual void MethodsToScript(const char* script_class_name);

	void StartRestore();
	void RestoreSuccesed();
};
