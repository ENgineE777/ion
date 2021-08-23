#pragma once

#include "ISceneManager.h"
#include "SceneInstance.h"

class SceneManager : public ISceneManager
{		
	struct SceneName
	{
		Str64  name;
		Str256 filename;		
	};

	std::vector<SceneName> scene_names;

#if defined(PC) || defined(OSUNIX)
	bool need_isi;
#endif

public:

	IStreamLoader::FileType load_type;

	std::vector<SceneInstance*> scenes;

	std::vector<SceneEntity*> assests;

	HashMap<ISceneService> services;	
	
	SceneManager();
	virtual bool Init();

	virtual void SetSceneLoadType(IStreamLoader::FileType type);
	virtual ISceneService* GetService(const char* name);
	
	SceneInstance* Load(const char* name);
	void Unload(const char* name);	

	SceneInstance* FindScene(const char* name);

	void Clear();

	virtual SceneEntity* CreateEntity(ISceneInstance* scene, const char* name, bool isAssest);

	virtual SceneEntity* FindAssest(const char* assest_name);
	virtual SceneEntity* AddAssestToLibrary(const char* assest_name);
	virtual void ApplyAssest(SceneEntity* entity, const char* assest_name);

	void RegisterSceneName(const char* filename);
	const char* GetSceneFileName(const char* name);	
	void UnregisterSceneNames();

#if defined(PC) || defined(OSUNIX)
	struct SceneIncludes
	{
		vector<Str512> includes;
	};

	HashMap<SceneIncludes> scene_includes;

	void RegisterIncludeInScene(const char* name, const char* filename);
#endif

	void Work(float dt, int level);	

#if defined(PC) || defined(OSUNIX)
	bool IsNeedIsi();
	void SetIsNeedIsi(bool set_need);
#endif

	void Release();
};
