#pragma once

#include "SceneEntity.h"
#include "ISceneService.h"

class ISceneManager : public TaskHoldersOwner
{		
public:

	virtual bool Init() = 0;

	virtual void SetSceneLoadType(IStreamLoader::FileType type) = 0;
	virtual ISceneService* GetService(const char* name) = 0;

	virtual SceneEntity* CreateEntity(ISceneInstance* scene, const char* name, bool isAssest) = 0;

	virtual SceneEntity* FindAssest(const char* assest_name) = 0;
	virtual SceneEntity* AddAssestToLibrary(const char* assest_name) = 0;
	virtual void ApplyAssest(SceneEntity* entity, const char* assest_name) = 0;

	virtual void Release() = 0;
};
