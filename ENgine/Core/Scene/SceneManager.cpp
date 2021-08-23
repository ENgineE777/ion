#include "Core/Core.h"
#include "SceneInstance.h"

SceneManager::SceneManager() : ISceneManager(), services(_FL_)
#if defined(PC) || defined(OSUNIX)
	, scene_includes(_FL_)
#endif
{
	load_type = IStreamLoader::binary;

#if defined(PC) || defined(OSUNIX)
	need_isi = true;
#endif
}

bool SceneManager::Init()
{	
	for (SSDeclBase* decl = SSDeclBase::GetFirst(); decl != NULL; decl = decl->GetNext())
	{
		ISceneService* service = decl->Create();
		service->Init();

		services.Add(decl->Name(), service);
	}

	core->AddTask(this, this, (TaskHolder::Task)&SceneManager::Work, lvl_pre1, -1.0f);
	return true;
}

ISceneService* SceneManager::GetService(const char* name)
{
	return services.Get(name);
}

void SceneManager::SetSceneLoadType(IStreamLoader::FileType type)
{
	load_type = type;
}

SceneInstance* SceneManager::Load(const char* name)
{
	const char* filename = null;	

	if (core->IsEditorMode())
	{
		filename = name;		
	}
	else
	{
		filename = GetSceneFileName(name);		

		if (!filename)
		{
			core->TraceTo("Scene", "Error: scene %s not registered", name);
			return null;
		}		
	}

	SceneInstance* scene = FindScene(name);
    
	if (scene)
	{
		core->TraceTo("Scene", "Scene %s already loaded", name);		
		if (!core->IsEditorMode() && !scene->IsRunning())
		{
			scene->Start();
		}
	}
	else
	{
		scene = new SceneInstance(this, name);
		scenes.push_back(scene);

#ifdef EDITOR
		char path[256];
		String::Copy(path, 256, filename);

		if (load_type == IStreamLoader::xml)
		{
			path[strlen(path) - 1] = 'x';
		}

		scene->Load(path, load_type);
#else
		scene->Load(filename, load_type);
#endif

#if defined(PC) || defined(OSUNIX)
		SceneIncludes* scn_inc = scene_includes.Get(name);

		if (scn_inc)
		{
			for (int i=0;i<scn_inc->includes.size();i++)
			{
				scene->IncludeScene(scn_inc->includes[i].data);
			}		
		}
#endif
	}


	return scene;
}

void SceneManager::Unload(const char* name)
{	
	for (int i=0;i<scenes.size();i++)
	{
		if (String::IsEqual(scenes[i]->GetName(), name))
		{
			scenes[i]->need_delete = true;			

			break;
		}
	}	
}

SceneInstance* SceneManager::FindScene(const char* name)
{
	for (int i=0;i<scenes.size();i++)
	{
		if (String::IsEqual(scenes[i]->GetName(), name))
		{
			return scenes[i];
		}
	}

	return null;
}

void SceneManager::Clear()
{
	for (int i=0;i<scenes.size();i++)
	{
		if (scenes[i]->IsRunning()) scenes[i]->Finish();
		scenes[i]->Release();
	}

	scenes.clear();
}

SceneEntity* SceneManager::CreateEntity(ISceneInstance* scene, const char* name, bool isAssest)
{	
	for (EntityDedeclarator* decl = EntityDedeclarator::GetFirst(); decl != NULL; decl = decl->GetNext())
	{
		if (String::IsEqual(decl->GetName(), name))
		{
			SceneEntity* entity = decl->Create(scene);

			String::Copy(entity->className, 31, name);
			entity->allowTasks = !false;
			entity->Init();	

			return entity;
		}
	}
	
	core->TraceTo("Scene", "can't create entity %s", name);
	return null;
}

SceneEntity* SceneManager::FindAssest(const char* assest_name)
{
	for (int i=0;i<(int)assests.size();i++)
	{
		if (String::IsEqual(assests[i]->GetName(), assest_name))
		{
			return assests[i];
		}
	}

	return null;
}

SceneEntity* SceneManager::AddAssestToLibrary(const char* assest_name)
{	
	SceneEntity* assest = FindAssest(assest_name);	

	char lua_file[256];
	String::Copy(lua_file, 255, "Entities/");
	String::Cat(lua_file, 255, assest_name);
	String::Cat(lua_file, 255, ".lua");

	/*CLuaTable assestTable;

	if (assestTable.LoadScript( lua_file, LUALIB_DOFILE ))
	{	
		const char *className = "";
		assestTable.Get("classname", className);

		if (!assest)
		{
			assest = CreateEntity(null, className, true);
			
			if (assest)
			{				
				assest->SetName(assest_name);
				assests.push_back(assest);
			}
		}

		if (assest)
		{			
			assest->LoadProperties(&assestTable);			
		}		

		for (int i=0;i<(int)scenes.size();i++)
		{
			scenes[i]->AssestChanged(assest);

		}

		return assest;
	}*/

	return null;
}

void SceneManager::ApplyAssest(SceneEntity* entity, const char* assest_name)
{
	if (String::IsEmpty(assest_name))
	{
		entity->assetName[0] = 0;
	}
	else
	{
		SceneEntity* assest = FindAssest(assest_name);

		if (!assest)
		{
			assest = AddAssestToLibrary(assest_name);
		}

		if (assest)
		{	
			entity->SetAsset(assest);
		}
	}
}

#if defined(PC) || defined(OSUNIX)
void SceneManager::RegisterIncludeInScene(const char* name, const char* filename)
{
	SceneIncludes* scn_inc = scene_includes.Get(name);

	if (!scn_inc)
	{
		scn_inc = scene_includes.Add(name);
	}

	Str512 str;
	String::Copy(str.data, 512, filename);

	scn_inc->includes.push_back(str);
}
#endif

void SceneManager::Work(float dt, int level)
{
	int index = -1;

	for (int i=0;i<scenes.size();i++)
	{		
		if (scenes[i]->IsLoadingFailed() || scenes[i]->IsLoadingComplete())	
		{
			scenes[i]->sc_loader.Flush();

			if (!core->IsEditorMode())
			{
				if (!scenes[i]->Start())
				{
					core->Exit();
				}
				else				
				{
					for (int j=0;j<scenes.size();j++)
					{
						if (scenes[j]->scriptModule) scenes[j]->scriptModule->BindImportFunctions();					
					}
				}
			}
		}
		else
		if (scenes[i]->IsLoadingPending())
		{
			if (index==-1)
			{
				index = i;
			}
		}
		else
		if (scenes[i]->IsLoading() && !scenes[i]->IsLoadingPending())
		{
			index = -2;
		}
	}
//
//#ifdef PC
	//if (index >= 0) scenes[index]->sc_loader.Start(ThreadWorker::normal);
//#else
	if (index >= 0)
	{
		scenes[index]->sc_loader.Prepare();
		scenes[index]->sc_loader.Loop();
	}
//#endif

	for (int i=0;i<scenes.size();i++)
	{
		if (scenes[i]->need_delete && !scenes[i]->IsLoading())
		{			
			if (scenes[i]->IsRunning()) scenes[i]->Finish();
			scenes[i]->Release();

			for (int j=i;j<scenes.size()-1;j++)
			{
				scenes[j] = scenes[j+1];
			}

			scenes.pop_back();
			i--;
		}
	}	
}

void SceneManager::RegisterSceneName(const char* filename)
{
	scene_names.push_back(SceneName());

	SceneName* scene_name = &scene_names[scene_names.size() - 1];

	String::Copy(scene_name->filename.data, 256, filename);	        
	String::ExtractFileName(filename, scene_name->name.data);
	String::RemoveExctention(scene_name->name.data);
}

const char* SceneManager::GetSceneFileName(const char* name)
{
	for (int i=0;i<scene_names.size();i++)
	{
		if (String::IsEqual(scene_names[i].name.data, name))
		{
			return scene_names[i].filename.data;
		}		
	}

	return null;
}

void SceneManager::UnregisterSceneNames()
{
	scene_names.clear();
#ifdef PC
	scene_includes.Clear();
#endif
}

#if defined(PC) || defined(OSUNIX)
bool SceneManager::IsNeedIsi()
{
	return need_isi;
}

void SceneManager::SetIsNeedIsi(bool set_need)
{
	need_isi = set_need;
}
#endif

void SceneManager::Release()
{
	scene_names.clear();

	Clear();

	for (int i=services.Size()-1;i>=0;i--)
	{
		services.Get(i)->Release();
	}

	services.Clear();	
}
