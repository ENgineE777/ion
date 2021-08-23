#pragma once

#include "CoreBase.h"

#include "Core/Render/RenderServiceGLES.h"
#include "Core/Control/ControlsService.h"
#include "Core/Files/FileService.h"
#include "Core/Script/ScriptService.h"
#include "Core/Scene/SceneManager.h"
#include "Core/Model/ModelService.h"
#include "Core/Localize/LocalizeService.h"
#include "Core/UserStats/UserStatsService.h"
#include "Core/Sound/SoundService.h"
#include "Core/OnlineUserStats/OnlineUserStats.h"
#include "Core/Font/FontService.h"
#include "Core/Network/NetworkService.h"
#include "MediaLibrary/MediaLibraryService.h"
#include "Mobile/MobileService.h"
#include "MediaPlayer/MediaPlayer.h"

class CoreAndroid : public CoreBase
{
	RenderServiceGLES     render;
	ControlsService       controls;
	FileService           files;
	SceneManager          sceneManager;
	ScriptService         script;
	ModelService          models;
	LocalizeService       localize;
	UserStatsService      user_stats;
	SoundService          sound;
	OnlineUserStatsSystem online_user_stats;
	NetworkService        network;
	FontService           fonts;
	MediaLibraryService   mediaLibrary;
	MobileService   mobile;
	MediaPlayer mediaPlayer;
	
	HashMap<int> logs;

public:	

	RenderServiceGLES* Render()
	{
		return &render;
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
		return &user_stats;
	};

	OnlineUserStatsSystem* OnlineUserStats()
	{
		return &online_user_stats;
	};
	
	SoundService* Sound()
	{
		return &sound;
	};

	ModelService* Models()
	{
		return &models;
	};

	virtual INetworkService* Network()
	{		
		return &network;
	}	

	FontService* Font()
	{
		return &fonts;
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

	CoreAndroid();
	virtual bool Init();	
	virtual void TraceToImpl(const char* log, const char *str);
	virtual void Work();
	virtual void Release();	

	virtual Platform GetCurrentPlatform() { return platform_android; };	
	
	void SetDeviceVendor(const char* device_vendor);
    void SetDeviceName(const char* device_name);
	void SetDeviceID(const char* device_id);

	const char* SetLocale(const char* set_locale);
};

extern CoreAndroid en_core;
