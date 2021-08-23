#pragma once

#include "CoreBase.h"

#include "Core/Control/ControlsService.h"
#include "Core/Resources/Resources.h"
#include "Core/UserStats/UserStatsService.h"
#include "Core/Font/FontService.h"
#include "Core/Model/ModelService.h"
#include "Files/FileService.h"
#include "Core/OnlineUserStats/OnlineUserStats.h"
#include "Core/Render/RenderServiceD3D11.h"
#include "Core/Network/NetworkService.h"
#include "Core/Script/ScriptService.h"
#include "Core/Scene/SceneManager.h"
#include "MediaLibrary/MediaLibraryService.h"
#include "Mobile/MobileService.h"
#include "MediaPlayer/MediaPlayer.h"

class CorePC : public CoreBase
{
	RenderService*   render;	

	ControlsService  controls;
	FileService      files;
	SceneManager     sceneManager;
	ScriptService    script;
	
	ModelService     models;
	UserStatsService userstats;
	ResourceManager	 res_manager;
	OnlineUserStatsSystem online_userstats;
	NetworkService   network;
	FontService      fonts;		
	MediaLibraryService mediaLibrary;	
	MobileService mobile;
	MediaPlayer mediaPlayer;

	HWND		 hwnd;		
	HINSTANCE	 hInstance;

	HashMap<int> logs;	

	bool skip_render;

public:	

	virtual ModelService* Models()
	{
		return &models;
	};

	virtual RenderService* Render()
	{
		return render;
	};

	virtual FontService* Font()
	{		
		return &fonts;
	};

	virtual OnlineUserStatsSystem* OnlineUserStats()
	{		
		return &online_userstats;
	};

	virtual IControlsService* Controls()
	{
		return &controls;
	};

	virtual IFileService* Files()
	{
		return &files;
	};

	virtual FileService* EN_Files()
	{
		return &files;
	};

	virtual ISceneManager* Scene()
	{
		return &sceneManager;
	};

	virtual SceneManager* EN_Scene()
	{
		return &sceneManager;
	};

	virtual IScriptService* Script()
	{
		return &script;
	};


	virtual IUserStatsService* UserStats()
	{		
		return &userstats;
	};
	
	virtual INetworkService* Network()
	{		
		return &network;
	};

	virtual MediaLibraryService* MediaLibrary()
	{
		return &mediaLibrary;
	}	
		
	virtual MobileService* Mobile()
	{
		return &mobile;
	}

	virtual MediaPlayer* Player()
	{
		return &mediaPlayer;
	}

	CorePC();
	virtual bool Init();
	virtual void CountDeltaTime(bool need_clamp);
	virtual void TraceToImpl(const char* log, const char* text);
	virtual void Work();
	virtual void RenderFrame();
	virtual void Release();	

	void SetSkipRender(bool set);

	virtual Platform GetCurrentPlatform() { return platform_pc; };

	void SetHWND(HWND wnd);
	HWND GetHWND();	
	
	void SetInstance(HINSTANCE instance);
	HINSTANCE GetInstance();

	void ResetLogs();

protected:

	void GenerateDeviceID();
};

extern DEF_EXPORT CorePC en_core;
