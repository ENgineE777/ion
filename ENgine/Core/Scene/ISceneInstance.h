#pragma once

#include "SceneEntity.h"
#include "Common/ThreadWorker.h"

class IScriptModule;

class ISceneInstance : public TaskHoldersOwner
{	
public :

	struct EntityGroup
	{
		char name[64];
		std::vector<SceneEntity*> entities;
	};

	virtual bool Init() = 0;

	virtual const char* GetName() = 0;
	virtual void SetName(const char* set_name) = 0;

	virtual bool IsRunning() = 0;
	virtual bool IsLoading() = 0;

	virtual void AddEntityUpdate(SceneEntity* entity, TaskHolder::Task task, int level, float freq) = 0;
	virtual void DelEntityUpdate(SceneEntity* entity, TaskHolder::Task task) = 0;
	virtual void ApplyEntityTaskList() = 0;	

	virtual EntityGroup* GetGroup(const char* name) = 0;
	virtual void RegisterEntityInGroup(SceneEntity* entity,const char* name) = 0;
	virtual void UnregisterEntityInGroup(SceneEntity* entity,const char* name) = 0;
	virtual void UnregisterEntityFromGroups(SceneEntity* entity) = 0;

    virtual SceneEntity* AddSceneEntity(const char* name) = 0;	

#if defined(PC) || defined(OSUNIX)
	virtual bool IsIncludedEntity(SceneEntity* entity) const = 0;
#endif

	virtual IScriptModule* ScriptModule() = 0;

	virtual SceneEntity* Find(const char* name, const char* className = null) = 0;

    virtual void RemoveEntity(SceneEntity* entity) = 0;

	virtual void Release() = 0;
};
