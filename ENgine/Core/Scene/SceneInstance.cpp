#include "Core/Core.h"
#include "SceneInstance.h"
#include "SceneManager.h"

#include "Core/Script/ScriptModule.h"
#include "Core/Script/ScriptMachine.h"

#include "Entities/NGUI/NGUIWidget.h"
#include "Entities/NGUI/NGUISymbolInstance.h"

int scene_find_counter = 0;

SceneInstance::SceneLoader::SceneLoader() : ThreadWorker()
{
	owner = NULL;
	progress = 0.0f;
	state = idle;	
}

void SceneInstance::SceneLoader::Prepare()
{
	progress = 0.0f;
	state = loading;
}

void SceneInstance::SceneLoader::Loop()
{
	if (!owner->LoadImpl())
	{
		state = loading_failed;
	}

	state = complete;
}

void SceneInstance::SceneLoader::Flush()
{
	state = idle;
	owner->ApplyEntityTaskList();
}

SceneInstance::SceneInstance(SceneManager* mng, const char* scn_name)
{
	String::Copy(name, 512, scn_name);

	manager = mng;	

	srt_filename[0] = 0;

	lvl_filename[0] = 0;

	scriptModule = null;

	need_delete = false;

	running = false;

#if defined(PC) || defined(OSUNIX)	
	num_inlcuded_entities = 0;
	start_fn_name[0] = 0;
#endif

	scene_names_already_binded = false;

	SetTaskHoldersOwnerActive(false);
}

bool SceneInstance::Init()
{	
	return true;
}

bool SceneInstance::IsRunning()
{
	return running;
}

bool SceneInstance::Load(const char* filename, IStreamLoader::FileType type)
{
	if (IsLoading()) return false;	

	sc_type = type;
	String::Copy(sc_filename, 512, filename);

	sc_loader.owner = this;
	sc_loader.state = SceneLoader::pending;	

	return true;
}
bool SceneInstance::LoadImpl()
{
	bool res = true;
	
	scene_find_counter = 0;

#if defined(PC) || defined(OSUNIX)	

	for (unsigned i=0;i<included_scenes.size();i++)
	{
		if (!LoadImpl(included_scenes[i].data,true))
		{
			res = false;
			break;
		}
	}
	num_inlcuded_entities = entities.size();
	if (num_inlcuded_entities > 0)
	{
		char filename_clean[256];
		String::Copy(filename_clean, 256, sc_filename);
		String::RemoveExctention(filename_clean);

		char filename_inc[256];
		String::Printf(filename_inc, 256, "%s.isi", filename_clean);

		if (manager->IsNeedIsi())
		{
			IStreamLoader* loader = core->Files()->CreateStreamLoader(filename_inc, "include", sc_type);				
					
			if (loader)
			{
				loader->SetNeedLogWarnings(false);

				while (loader->EnterBlock("Entity"))
				{
					char name[64];
					name[0] = 0;
					loader->Read("entityName", name, 64);

					SceneEntity* entity = Find(name);

					if (entity)
						entity->LoadProperties(loader);

                    loader->LeaveBlock();
                }

			    loader->Release();	
			}				

			
		}
	}

	if (res)
#endif
	{		
		res |= LoadImpl(sc_filename);
	}	

	if (res)
	{
		//#if defined(PC) || defined(OSUNIX)
		en_core.CountDeltaTime(false);
		en_core.TraceTo("Scene", "ReInitad of %s stared", sc_filename);
		//#endif				

		for (unsigned i=0;i<entities.size();i++)
			entities[i]->ReInit();		

        for (int i=0;i<(int)entities.size();i++)
        {
			if (entities[i]->CheckClassName("NGUIWidget"))
			{
				NGUIWidget* pWidget = static_cast<NGUIWidget*>(entities[i]);
				if (!pWidget->parent && !pWidget->IsSymbol())
				{
					NGUIService::instance->AddTopWidget(pWidget);
				}
            }
        }
		//#if defined(PC) || defined(OSUNIX)
		en_core.CountDeltaTime(false);
		en_core.TraceTo("Scene", "Loading of %s finished", sc_filename);

	}

	en_core.TraceTo("Scene", "finder counter - %i", scene_find_counter);	
	return res;
}
#if defined(PC) || defined(OSUNIX)
bool SceneInstance::ReloadIncludedEntity(SceneEntity* pEntity,const char* sScenePath) const
{
	if (sScenePath)
	{
		IStreamLoader* pLoader = core->Files()->CreateStreamLoader(sScenePath, "scene", sc_type);
		if (pLoader)
		{
			while (pLoader->EnterBlock("Entity"))
			{
				char name[64];
				name[0] = 0;
				pLoader->Read("entityName", name, 64);
				if (String::IsEqual(pEntity->GetName(), name))
				{
					Matrix tr;
					pLoader->Read("entityTransform", tr);
					pEntity->SetTransform(tr);

					pLoader->Read("active", pEntity->activeOnStart);
					pLoader->Read("show", pEntity->showOnStart);

					bool use_assest = false;
					pLoader->Read("UseAssest", use_assest);

					if (use_assest)
					{
						char assest_name[64];
						pLoader->Read("Assest", assest_name, 64);

						ApplyAssest(pEntity, assest_name);
					}
					else
					{
						pEntity->SetDefault();
						pEntity->LoadProperties(pLoader);
					}
					pEntity->ReInit();
					return true;
				}
                pLoader->LeaveBlock();
			}
		}
	}
	return false;
}
#endif
bool SceneInstance::LoadImpl(const char* filename,bool loading_include)
{
	IStreamLoader* loader = core->Files()->CreateStreamLoader(filename, "scene", sc_type);

	if (!loader)
		return false;	

	int counter = 0;
	int scene_number = 0;
	loader->Read("scene_number", scene_number);


	en_core.CountDeltaTime(false);
	en_core.TraceTo("Scene", "Load of %s stared", filename);

	scene_number *= 2;

	while (loader->EnterBlock("Entity"))
	{			
        char className[32] , name[64];
        name[0] = className[0] = 0;
		loader->Read("className", className, 32);		
        loader->Read("entityName", name, 64);
        if(!Find(name))
        {
            SceneEntity* entity = AddSceneEntity(className);
            if (entity)
            {                
                entity->SetName(name);

                Matrix tr;
                loader->Read("entityTransform", tr);
                entity->SetTransform(tr);		

                loader->Read("active", entity->activeOnStart);
                loader->Read("show", entity->showOnStart);

                bool use_assest = false;			
                loader->Read("UseAssest",use_assest);

                if (use_assest)
                {	
                    char assest_name[64];
                    loader->Read("Assest", assest_name, 64);

                    ApplyAssest(entity, assest_name);
                }
                else
                {
#if defined(PC) || defined(OSUNIX)
                    entity->SetDefault();
#endif
                    entity->LoadProperties(loader);
                }

#if defined(PC) || defined(OSUNIX)	
                if (loading_include)
                    included_entities.push_back(entity->Clone());
#endif
            }
            else
            {
                core->TraceTo("Scene", "Can't Create Entity with className - %s",className);
            }

            counter++;
            sc_loader.progress = (float)counter/(float)scene_number;
        }

		loader->LeaveBlock();
	}		

	String::ExtractFileName(sc_filename, lvl_filename);
	String::RemoveExctention(lvl_filename);	

	loader->Release();

	return true;
}

#if defined(PC) || defined(OSUNIX)
void SceneInstance::IncludeScene(const char* filename)
{
	included_scenes.push_back(Str256());
	String::Copy(included_scenes[included_scenes.size()-1].data, 256, filename);	
}

void SceneInstance::ResetIncludedScene()
{
	included_scenes.clear();
}

void SceneInstance::SetStartFunc(const char* func_name, const char* param)
{
	String::Copy(start_fn_name, 256, func_name);
	start_fn_param = param;
}

bool SceneInstance::IsIncludedEntity(SceneEntity* entity) const
{
	for (unsigned i=num_inlcuded_entities; i<entities.size(); i++)
	{
		if (entities[i] == entity)
		{
			return false;
		}
	}

	return true;
}
#endif

bool SceneInstance::IsLoading()
{
	bool res = sc_loader.state != SceneLoader::idle;

	return res;
}

bool SceneInstance::IsLoadingPending()
{
	bool res = sc_loader.state == SceneLoader::pending;

	return res;
}

bool SceneInstance::IsLoadingComplete()
{
	bool res = sc_loader.state == SceneLoader::complete;

	return res;
}

bool SceneInstance::IsLoadingFailed()
{
	bool res = sc_loader.state == SceneLoader::loading_failed;

	return res;
}

float SceneInstance::GetLoadingProgress()
{
	float res = sc_loader.progress;

	return res;
}

std::vector<SceneEntity*> saved_entities;

void SaveEntity(IStreamSaver* saver, SceneEntity* entity)
{	
	saver->MarkBeginBlock("Entity");				

	saver->Write("className", entity->className);
	saver->Write("entityName", entity->GetName());

	Matrix tr;
	entity->GetTransform(tr);
	saver->Write("entityTransform", tr);

	saver->Write("active", entity->activeOnStart);
	saver->Write("show", entity->showOnStart);

	bool use_assest = true;

	if (String::IsEmpty(entity->assetName))
	{
		use_assest = false;
	}

	saver->Write("UseAssest",use_assest);

	if (use_assest)
	{	
		saver->Write("Assest", entity->assetName);
	}
	else
	{
		entity->SaveProperties(saver);		
	}		

	saver->MarkEndBlock("Entity");

	saved_entities.push_back(entity);
}

void SaveReverseChilds(IStreamSaver* saver, SceneEntity* entity)
{
	NGUIWidget* wgt = (NGUIWidget*)entity;

	for (unsigned j=0;j<saved_entities.size();j++)
	{
		if (saved_entities[j] == entity)
		{
			return;			
		}
	}	
	
	if (String::IsEqual(entity->className, "NGUISymbolInstance"))	
	{
		bool skip = false;

		NGUISymbolInstance* inst = (NGUISymbolInstance*)entity;		

		for (unsigned j=0;j<saved_entities.size();j++)
		{
			if (saved_entities[j] == inst->wgt_symbol)
			{
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			SaveReverseChilds(saver, inst->wgt_symbol);
		}
	}

	for (unsigned i=wgt->childs.size() - wgt->childs_names.size(); i<wgt->childs.size(); i++)
	{		
		SaveReverseChilds(saver, wgt->childs[i]);		
	}

	SaveEntity(saver, wgt);		
}

bool SceneInstance::HaveDifferences(SceneEntity* pEntity)
{
#if defined(PC) || defined(OSUNIX)
    for (int i=0; i<num_inlcuded_entities; i++)
    {				
        if(pEntity == entities[i])
        {
            return pEntity->HaveDifferences(included_entities[i]);
        }
    }
#endif
    return false;
}

void SceneInstance::Save(const char* filename, IStreamSaver::FileType type)
{	
#ifdef PC
	if (type == IStreamSaver::xml)
	{
		//String::ExtractFileName(filename, srt_filename);
		//String::RemoveExctention(srt_filename);			
	}
#endif	

	IStreamSaver* saver = core->Files()->CreateStreamSaver(filename, type);

	if (!saver)
	{		
		return;
	}	

	saver->MarkBeginBlock("scene");	

	int scene_number = entities.size();
	int from_number = 0;

#if defined(PC) || defined(OSUNIX)
	if (type == IStreamSaver::xml)
	{	
		if (num_inlcuded_entities > 0)
		{
			char filename_clean[256];
			String::Copy(filename_clean, 256, filename);
			String::RemoveExctention(filename_clean);

			if (manager->IsNeedIsi())
			{
				char filename_inc[256];
				String::Printf(filename_inc, 256, "%s.isi", filename_clean);

				IStreamSaver* saver_inc = core->Files()->CreateStreamSaver(filename_inc, type);
		
				saver_inc->MarkBeginBlock("include");

				for (int i=0; i<num_inlcuded_entities; i++)
				{				
					SceneEntity* entity = entities[i];								
					SceneEntity* entity_origin = included_entities[i];				
				
					if (entity->HaveDifferences(entity_origin))
					{
						saver_inc->MarkBeginBlock("Entity");
						saver_inc->Write("entityName", entity->GetName());

						entity->SaveDiffProperties(saver_inc, entity_origin);							

						saver_inc->MarkEndBlock("Entity");
					}
				}
	
				saver_inc->MarkEndBlock("include");

				saver_inc->Release();
			}
		}

		from_number = num_inlcuded_entities;
		scene_number = scene_number - num_inlcuded_entities;
	}
#endif

	saver->Write("scene_number", scene_number);

	saved_entities.clear();	

	if (type == IStreamSaver::binary)
	{
		for (int i=from_number; i<(int)entities.size(); i++)
		{	
			SceneEntity* entity = entities[i];

			SceneEntity* wgt = entity;
			
			if (String::IsEqual(entity->className, "NGUISymbolInstance"))	
			{
				wgt = ((NGUISymbolInstance*)wgt)->wgt_symbol;					
			}
			else
			if (!String::IsEqual(entity->className, "NGUISymbol"))
			{
				continue;
			}	

			bool skip = false;

			for (unsigned j=0;j<saved_entities.size();j++)
			{
				if (saved_entities[j] == wgt)
				{
					skip = true;
					break;
				}
			}
	
			if (skip)
			{
				continue;
			}				

			SaveReverseChilds(saver, entity);		
		}
	}

	for (int i=from_number; i<(int)entities.size(); i++)
	{		
		SceneEntity* entity = entities[i];
		
		bool skip = false;

		if (type == IStreamSaver::binary)
		{
			for (unsigned j=0;j<saved_entities.size();j++)
			{
				if (saved_entities[j] == entity)
				{
					skip = true;
					break;
				}
			}
		}

		if (skip)
		{
			continue;
		}

		SaveEntity(saver, entity);	
	}		

	saver->MarkEndBlock("scene");

	saver->Release();
}

void SceneInstance::AddEntityUpdate(SceneEntity* entity, TaskHolder::Task task, int level, float freq = -1.0f)
{
	taskList.push_back(EntityTaskList());

	EntityTaskList* entry = &taskList[taskList.size()-1];

	entry->entity = entity;
	entry->task = task;

	entry->add = true;

	entry->level = level;
	entry->freq = freq;
}

void SceneInstance::DelEntityUpdate(SceneEntity* entity, TaskHolder::Task task)
{
	taskList.push_back(EntityTaskList());

	EntityTaskList* entry = &taskList[taskList.size()-1];

	entry->entity = entity;
	entry->task = task;

	entry->add = false;
}

void SceneInstance::ApplyEntityTaskList()
{
	for (unsigned i=0; i<taskList.size();i++)
	{
		EntityTaskList* entry = &taskList[i];

		if (entry->add)
		{
			core->AddTask(entry->entity,this,entry->task,entry->level,entry->freq);
		}
		else
		{
			core->DelTask(entry->entity,entry->task);
		}
	}

	taskList.clear();
}

void SceneInstance::PreStart()
{
	if (!lvl_filename[0]) return;
	
//#ifdef PC
	en_core.CountDeltaTime(false);
	en_core.TraceTo("Scene", "Start script binding of %s", lvl_filename);
//#endif

	{
		asIScriptEngine* engine = ((ScriptMachine*)(core->Script()->Machine()))->Machine();
		engine->RemoveConfigGroup(lvl_filename);

		//engine->GarbageCollect();	
	}

	asIScriptEngine* engine = ((ScriptMachine*)(core->Script()->Machine()))->Machine();

	int k = engine->BeginConfigGroup(lvl_filename);

	k = engine->SetDefaultNamespace(lvl_filename);

	for (int i=0;i<(int)entities.size();i++)
	{
		if (entities[i]->AllowRegisterSelfToScript()) 
            entities[i]->SelfToScript();
	}

	engine->EndConfigGroup();

	engine->SetDefaultNamespace("");

//#ifdef PC
	en_core.CountDeltaTime(false);
	en_core.TraceTo("Scene", "script binding of %s finished", lvl_filename);
//#endif
}

bool SceneInstance::PostStart()
{
	if (!lvl_filename[0]) return false;

//#ifdef PC
	en_core.CountDeltaTime(false);
	en_core.TraceTo("Scene", "Start compiling script of %s", lvl_filename);
//#endif

	scriptModule = core->Script()->LoadModule(lvl_filename);		

//#ifdef PC
	en_core.CountDeltaTime(false);
	en_core.TraceTo("Scene", "compiling script of %s finished", lvl_filename);
//#endif

	if (!scriptModule)
	{		
		asIScriptEngine* engine = ((ScriptMachine*)(core->Script()->Machine()))->Machine();

		engine->RemoveConfigGroup(lvl_filename);
		//engine->GarbageCollect();

		return false;
	}

	for (int i=0;i<(int)entities.size();i++)
	{
		entities[i]->sv_transform = entities[i]->transform;		
		entities[i]->Activate(entities[i]->activeOnStart);		
		entities[i]->Show(entities[i]->showOnStart);
	}

	running = true;

	SetTaskHoldersOwnerActive(true);

	int k = entities.size();	
	for (int i=0;i<(int)entities.size();i++)
	{
		if (!entities[i]->Start())
		{
			return false;
		}
	}

#if defined(PC) || defined(OSUNIX)
	if (start_fn_name[0])
	{
		if (scriptModule)
		{
			ScriptFunction sfunc;			
			ScriptFunction::Find1S(&sfunc, scriptModule, start_fn_name);

			core->Script()->Machine()->AddStrArg(start_fn_param.c_str());
			core->Script()->Machine()->Call(&sfunc);

			sfunc.Reset();
		}		
	}	
#endif

	return true;
}

bool SceneInstance::Start()
{		
	if (!scene_names_already_binded)
	{
		PreStart();
		scene_names_already_binded = true;
	}

	return PostStart();	
}

void SceneInstance::Finish()
{
	if (scriptModule)
	{
		RELEASE(scriptModule)				
	}

	running = false;

	for (int i=0;i<(int)entities.size();i++)
	{
		entities[i]->Finish();
		entities[i]->SetTransform(entities[i]->sv_transform);
		//entities[i]->TieValues();
	}	
}

void SceneInstance::Clear()
{
#if defined(PC) || defined(OSUNIX)
	num_inlcuded_entities = 0;

	for (int i=0;i<(int)included_entities.size();i++)
	{
		included_entities[i]->Release();
	}
	
	included_entities.clear();
#endif


	for (int i=0;i<(int)entities.size();i++)
	{
        if(entities[i]->CheckClassName("NGUIWidget"))
        {
            NGUIWidget* pWidget = static_cast<NGUIWidget*>(entities[i]);
            if(!pWidget->parent)
                NGUIService::instance->DelTopWidget(pWidget);
        }
		core->DelTasks(entities[i]);
		en_core.Render()->Tasks()->DelAllTasks(entities[i]);
		entities[i]->Release();
	}

	entities.clear();

	RELEASE(scriptModule);

	{		
		asIScriptEngine* engine = ((ScriptMachine*)(core->Script()->Machine()))->Machine();
		//engine->GarbageCollect();	
		engine->RemoveConfigGroup(name);		
	}
}

SceneEntity* SceneInstance::AddSceneEntity(const char* name)
{
	SceneEntity* entity = manager->CreateEntity(this, name, false);
	
	if (entity)
	{
		entities.push_back(entity);	
	}

	return entity;
}

SceneEntity* SceneInstance::AddSceneEntityFromLibrary(const char* assest_name)
{	
	SceneEntity* assest = manager->FindAssest(assest_name);	

	if (!assest)
	{
		assest = manager->AddAssestToLibrary(assest_name);
		if (!assest) return null;
	}	
	
	SceneEntity* entity = AddSceneEntity(assest->className);
	entity->SetAsset(assest);	

	return entity;
}

void SceneInstance::AssestChanged(SceneEntity* assest)
{
	for (int i=0;i<(int)entities.size();i++)
	{
		if (String::IsEqual(entities[i]->assetName, assest->GetName()))
		{
			entities[i]->SetAsset(assest);				
		}
	}
}

void SceneInstance::ApplyAssest(SceneEntity* entity, const char* assest_name) const
{
	if (String::IsEmpty(assest_name))
	{
		entity->assetName[0] = 0;
	}
	else
	{
		SceneEntity* assest = manager->FindAssest(assest_name);

		if (!assest)
		{
			assest = manager->AddAssestToLibrary(assest_name);
		}

		if (assest)
		{	
			entity->SetAsset(assest);
		}
	}
}

SceneInstance::EntityGroup* SceneInstance::GetGroup(const char* name)
{
	for (int i=0;i<(int)groups.size();i++)
	{
		if (String::IsEqual(groups[i].name,name))
		{
			return &groups[i];
		}
	}

	return NULL;
}

void SceneInstance::RegisterEntityInGroup(SceneEntity* entity,const char* name)
{
	EntityGroup* group = GetGroup(name);

	if (!group)
	{
		groups.push_back(EntityGroup());
		group = &groups[groups.size()-1];
		strcpy(group->name,name);
	}

	group->entities.push_back(entity);
}

void SceneInstance::UnregisterEntityInGroup(SceneEntity* entity,const char* name)
{
	EntityGroup* group = GetGroup(name);

	if (group)
	{
		for (int i=0;i<(int)group->entities.size();i++)
		{
			if (group->entities[i] == entity)
			{
				if (i != (int)group->entities.size()-1)
				{
					group->entities[i] = group->entities[(int)group->entities.size()-1];
				}					

				group->entities.pop_back();				
			}
		}
	}
}

void SceneInstance::UnregisterEntityFromGroups(SceneEntity* entity)
{
	for (int i=0;i<(int)groups.size();i++)
	{
		UnregisterEntityInGroup(entity, groups[i].name);
	}
}

IScriptModule* SceneInstance::ScriptModule()
{
	return scriptModule;
}

SceneEntity* SceneInstance::Find(const char* name, const char* className)
{
	scene_find_counter++;

	for (int i=0;i<(int)entities.size();i++)
	{
		if (String::IsEqual(entities[i]->GetName(), name))
		{
			if (className)
			{				
				if (!entities[i]->CheckClassName(className))
				{
					return NULL;
				}				
			}
			
			return entities[i];			
		}
	}

	return NULL;
}

#if defined(PC) || defined(OSUNIX)
void SceneInstance::SaveScriptByteCode(const char* name)
{	
	if (scriptModule)
	{
		scriptModule->SaveByteCode(name);
	}
}

#endif

#ifdef EDITOR
void SceneInstance::AddEntity(SceneEntity* entity)
{
	entities.push_back(entity);

	if (entity->scene)
	{
		SceneInstance* scene = (SceneInstance*)entity->scene;

		for (int i=0; i<scene->entities.size();i++)
		{
			if (scene->entities[i] == entity)
			{
				scene->entities.erase(scene->entities.begin() + i);
			}
		}
	}

	core->ChangeTaskHolderOwner(entity, this);
	entity->scene = this;
}
#endif

void SceneInstance::Release()
{
	if (IsLoading())
	{
		sc_loader.Stop();
		sc_loader.Flush();
	}

	Clear();

	delete this;
}

void SceneInstance::RemoveEntity(SceneEntity* pEntity)
{
    UnregisterEntityFromGroups(pEntity);
    for (unsigned i=0;i<entities.size();i++)
    {
        if (entities[i] == pEntity)
        {
            if (i != entities.size() - 1)
                entities[i] = entities[entities.size() - 1];
            entities.pop_back();
            break;
        }
    }
    pEntity->Release();
}
