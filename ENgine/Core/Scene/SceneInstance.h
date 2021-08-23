#pragma once

#include "ISceneInstance.h"

class SceneManager;
class IScriptModule;

class SceneInstance : public ISceneInstance
{	
public:
	class SceneLoader : public ThreadWorker
	{
		public :

		enum State
		{
			idle = 0,
			pending = 1,
			loading = 2,
			loading_failed = 3,
			complete = 4
		};

		float progress;
		State state;

		SceneInstance* owner;

		SceneLoader();

		virtual void Prepare();
		
		virtual void Loop();

		virtual void Flush();
	};		

	bool scene_names_already_binded;

	IScriptModule* scriptModule;

	SceneManager*  manager;

	SceneLoader sc_loader;

	bool running;

	char srt_filename[512];	
	char lvl_filename[512];

	char name[512];		

	bool need_delete;	
	
#if defined(PC) || defined(OSUNIX)
	bool loading_include;
	int  num_inlcuded_entities;
	vector<Str256> included_scenes;
	char start_fn_name[256];
	string start_fn_param;
	std::vector<SceneEntity*> included_entities;
#endif

	struct EntityTaskList
	{
		SceneEntity* entity;
		TaskHolder::Task task;

		bool add;

		int level;
		float freq;
	};

	std::vector<EntityTaskList> taskList;

	std::vector<SceneEntity*> entities;	
	std::vector<EntityGroup> groups;
	
	char sc_filename[512];
	IStreamLoader::FileType sc_type;

	SceneInstance(SceneManager* mng, const char* scn_name);
	virtual bool Init();

	virtual const char* GetName() { return name; };
	virtual void SetName(const char* set_name)  { String::Copy(name, 512, set_name); };

	virtual bool IsRunning();

	bool Load(const char* filename, IStreamLoader::FileType type);	
	bool LoadImpl();
	bool LoadImpl(const char* filename,bool loading_include = false);
#if defined(PC) || defined(OSUNIX)
	void IncludeScene(const char* filename);		
	void ResetIncludedScene();		
	void SetStartFunc(const char* func_name, const char* param);	
	virtual bool IsIncludedEntity(SceneEntity* entity) const;
	bool ReloadIncludedEntity(SceneEntity* entity,const char* sScenePath) const;
#endif

	virtual bool IsLoading();
	bool IsLoadingPending();
	bool IsLoadingComplete();
	bool IsLoadingFailed();
	float GetLoadingProgress();

	void Save(const char* filename, IStreamSaver::FileType type);

    bool HaveDifferences(SceneEntity* pEntity);

	virtual void AddEntityUpdate(SceneEntity* entity, TaskHolder::Task task, int level, float freq);
	virtual void DelEntityUpdate(SceneEntity* entity, TaskHolder::Task task);
	virtual void ApplyEntityTaskList();
    virtual void RemoveEntity(SceneEntity* pEntity);


	void PreStart();	
	bool PostStart();	

	bool Start();	
	void Finish();

	void Clear();
	
	SceneEntity* AddSceneEntity(const char* name);
	SceneEntity* AddSceneEntityFromLibrary(const char* assest_name);

	void AssestChanged(SceneEntity* assest);
	void ApplyAssest(SceneEntity* entity, const char* assest_name) const;

	virtual EntityGroup* GetGroup(const char* name);
	virtual void RegisterEntityInGroup(SceneEntity* entity,const char* name);
	virtual void UnregisterEntityInGroup(SceneEntity* entity,const char* name);
	virtual void UnregisterEntityFromGroups(SceneEntity* entity);

	virtual IScriptModule* ScriptModule();
 
	SceneEntity* Find(const char* name, const char* className = null);

#if defined(PC) || defined(OSUNIX)
	void SaveScriptByteCode(const char* name);
#endif

#ifdef EDITOR
	void AddEntity(SceneEntity* entity);
#endif

	virtual void Release();
};
