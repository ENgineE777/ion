#pragma once

#include "CoreBase.h"


#include "Core/Control/ControlsService.h"
#include "Files/FileService.h"
#include "Core/Render/RenderServiceGLES.h"
#include "Core/Font/FontService.h"
#include "Core/Localize/LocalizeService.h"
#include "Core/Resources/Resources.h"
#include "Mobile/MobileService.h"
#include "MediaPlayer/MediaPlayer.h"
#include "MediaLibrary/MediaLibraryService.h"
#include "Core/Model/ModelService.h"
#include "Sound/SoundService.h"
#include "Core/UserStats/UserStatsService.h"
#include "Core/Network/NetworkService.h"
#include "Core/OnlineUserStats/OnlineUserStats.h"
#include "Core/Script/ScriptService.h"
#include "Core/Scene/SceneManager.h"

class CoreUnix : public CoreBase
{		
	RenderServiceGLES   render;
	ControlsService     controls;
	FileService         files;
	FontService         fonts;
	LocalizeService     localize;
	MobileService       mobile;
	MediaLibraryService mediaLibrary;
	MediaPlayer         mediaPlayer;	
	ModelService     models;
	SoundService     sound;
	UserStatsService userstats;
	NetworkService   network;
	
	SceneManager     sceneManager;
	ScriptService    script;		
	OnlineUserStatsSystem online_userstats;	
	
	HashMap<int> logs;	

	bool skip_render;

public:	

	virtual ModelService* Models()
	{		
		return &models;
	};

	virtual SoundService* Sound()
	{
		return &sound;
	};

	virtual RenderServiceGLES* Render()
	{
		return &render;
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

	virtual ILocalizeService* Localize()
	{		
		return &localize;
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

	CoreUnix();
	virtual bool Init();
	virtual void CountDeltaTime(bool need_clamp);
	virtual void TraceToImpl(const char* log, const char* text);
	virtual void Work();
	virtual void RenderFrame();
	virtual void Release();	

	void SetSkipRender(bool set);

	virtual Platform GetCurrentPlatform() { return platform_unix; };

	void ResetLogs();

protected:

	void GenerateDeviceID();
};

extern CoreUnix en_core;
