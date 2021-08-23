
#ifdef EDITOR

#include "Runner.h"
#include "EditorViewPort.h"
#include "Core/Core.h"

/*#include <openssl/evp.h>
#include <openssl/sha.h>

static char *pt(unsigned char *md)
{
	int i;
	static char buf[80];

	for (i=0; i<SHA_DIGEST_LENGTH; i++)
		sprintf(&(buf[i*2]),"%02x",md[i]);
	return(buf);
}*/

DEF_EXPORT int RunEditor( HINSTANCE hInstance, int nArgs, char** args)
{
	InitCommonControls();

	core_ed.Run(hInstance, nArgs, args);

	return 0;
}

DEF_EXPORT int RunExport(int nArgs, char** args)
{
	char exp_str[256];
	exp_str[0] = 0;

	ReadArg(nArgs, args, "export", exp_str, 256);	

	core_ed.editor_project.Export(exp_str);

	return 0;
}

DEF_EXPORT int RunENgine(HINSTANCE hInstance, int nArgs, char** args)
{
	int width = ReadINI("settings.ini", 800, "Editor", "game_wnd_width");
	int height = ReadINI("settings.ini", 600, "Editor", "game_wnd_height");
	
	char project[256];
	project[0] = 0;

	char start_func[256];
	start_func[0] = 0;	

	char start_func_params[256];
	start_func_params[0] = 0;	
	
	int is_hidden = 0;

	int net_cache_mode = 0;	

	ReadArg(nArgs, args, "scene", project, 256);
	ReadArg(nArgs, args, "width", width);
	ReadArg(nArgs, args, "height", height);
	ReadArg(nArgs, args, "is_hidden", is_hidden);
	ReadArg(nArgs, args, "net_cache_mode", net_cache_mode);
	ReadArg(nArgs, args, "sfunc", start_func, 256);
	ReadArg(nArgs, args, "sfuncp", start_func_params, 256);
	
	core->TraceTo("App", "prj: %s w: %i h: %i hd: %i nc: %i sf: %s prm: %s", project, width, height, is_hidden, net_cache_mode, start_func, start_func_params);

    core->SetProjectPath(project);

	en_core.SetInstance(hInstance);
				
	EditorWindow* game_window;
	EditorViewPort* game_viewport;			
	
	game_window = new EditorWindow(777, "Game", true, 100, 100, width, height);
	game_window->SetNeedStrictSize(true);

	game_viewport = new EditorViewPort(110, game_window, "", 0, 0, width, height);
	
	if (!is_hidden) game_window->Show(true);

	en_core.SetHWND(game_viewport->GetHandle());	

	if (!core->Init())
	{
		core->TraceTo("App", "core not inited, exiting");

		return 0;							
	}		

	//en_core.SetSkipRender(is_hidden);

	if (net_cache_mode == 1)
	{
		core->Network()->SetWriteCache(true);
	}
	
	if (net_cache_mode == 2)
	{
		core->Network()->SetUseCache(true);
	}

	if (is_hidden) game_viewport->Resize();
			
	IStreamLoader* loader = core->Files()->CreateStreamLoader(project, "project", IStreamLoader::xml);	
					
	game_viewport->Update();

	if (loader)
	{		
		int ref_height = 768;
		loader->Read("RefHeight", ref_height);

		en_core.Render()->Display()->SetReferenceHeight(ref_height);

		core->TraceTo("App", "project %s loaded", project);

		en_core.EN_Scene()->RegisterSceneName("editor/system_declations.srt");

		int start_scene = -1;
		loader->Read("start_scene", start_scene);		
			
		int index = 0;

		char start_scene_name[512];
		

		/*for (size_t i=0;i<pProjectTarget->scenes.size();i++)
		{					
			en_core.EN_Scene()->RegisterSceneName(pProjectTarget->scenes[i]->Path());

			if (!pProjectTarget->scenes[i]->IsScript())
			{
				char name[128];
				String::Copy(name, 128, pProjectTarget->scenes[i]->Name());
				name[strlen(name) - 3] = 0;

				for (size_t j=0;j<pProjectTarget->scenes[i]->included_scenes.size();j++)
				{							
					en_core.EN_Scene()->RegisterIncludeInScene(name, pProjectTarget->scenes[i]->included_scenes[j].data);
				}
			}
		}*/



		while (loader->EnterBlock("Scene"))
		{								
			char path[512];					
			loader->Read("Path", path, 512);			
			
			en_core.EN_Scene()->RegisterSceneName(path);
			
			char name[128];
			loader->Read("Name", name, 512);	
			name[strlen(name) - 3] = 0;

			if (index == start_scene)
			{
				strcpy(start_scene_name, name);
			}

			int is_srt = 0;
			loader->Read("OnlySrt", is_srt);
			
			if (is_srt == 0)
			{
				while (loader->EnterBlock("IncludedScene"))
				{										
					Str256 str;
					loader->Read("Path", str.data, 256);
				
					en_core.EN_Scene()->RegisterIncludeInScene(name, str.data);
				
					loader->LeaveBlock();
				}
			}

			index++;

			loader->LeaveBlock();
		}			

		//en_core.SetEditorMode(true);
		en_core.EN_Scene()->SetSceneLoadType(IStreamLoader::xml);
		SceneInstance* scn = en_core.EN_Scene()->Load(start_scene_name);
		
		if (!scn)
		{
			core->TraceTo("App", "project %s not founded", project);
		}
		else
		{
			if (start_func[0])
			{
				scn->SetStartFunc(start_func, start_func_params);

				core->TraceTo("App", "sf %s was callled with %s", start_func, start_func_params);
			}
		}

		loader->Release();		
	}
	else
	{
		core->TraceTo("App", "project %s not founded", project);
	}

	while( !en_core.IsExited() )
	{
		core->CountDeltaTime(true);
		core->Work();
						
		game_viewport->Update();

		EventsWidget::Update();				

		MSG msg;

		while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		} 
	}
		
	core->TraceTo("App", "exiting...");

	core->Release();
	game_viewport->Release();

	return 0;
}

#endif