#pragma once

#include "CoreBase.h"

#include "Render/RenderServiceGLES.h"
#include "Control/ControlsService.h"
#include "Files/FileService.h"
#include "Script/ScriptService.h"
#include "Scene/SceneManager.h"
#include "Localize/LocalizeService.h"
#include "Model/ModelService.h"
#include "UserStats/UserStatsService.h"
#include "Sound/SoundService.h"
#include "OnlineUserStats/OnlineUserStats.h"
#include "Font/FontService.h"
#include "Network/NetworkService.h"
#include "MediaLibrary/MediaLibraryService.h"
#include "Mobile/MobileService.h"
#include "MediaPlayer/MediaPlayer.h"

class CoreIOS : public CoreBase
{
	RenderServiceGLES     render;
	ControlsService       controls;
	FileService           files;
	SceneManager          sceneManager;
	ScriptService         script;
	LocalizeService       localize;
	ModelService          models;
	UserStatsService      userStats;
    SoundService          sound;
    OnlineUserStatsSystem online_userstats;
    FontService           fonts;
    NetworkService        network;
	MediaLibraryService   mediaLibrary;
    MobileService          mobile;
    MediaPlayer            player;

public:	

	virtual IFileService* Files()
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

	virtual IControlsService* Controls()
	{
		return &controls;
	};
	
    virtual ControlsService* EN_Controls()
	{
		return &controls;
	};
    
	RenderServiceGLES* Render()
	{
		return &render;
	};
	
	virtual ILocalizeService* Localize()
	{
		return &localize;
	};
	
	ModelService* Models()
	{
		return &models;
	};
	
	virtual IUserStatsService* UserStats()
	{
		return &userStats;
	};
    
    SoundService* Sound()
    {
		return &sound;
	};
	
    OnlineUserStatsSystem* OnlineUserStats()
    {
		return &online_userstats;
	};    
    
    FontService* Font()
    {
		return &fonts;
	};
    
    INetworkService* Network()
    {
        return &network;
    }

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
        return &player;
    }
    
	CoreIOS();
	virtual bool Init();	
	virtual void CountDeltaTime(bool need_clamp);
	virtual void TraceToImpl(const char* log, const char *str);
	virtual void Work();
	
	virtual Platform GetCurrentPlatform();
	
    virtual void SetLocale(const char* locale);
    
	virtual void Release();
};

extern CoreIOS en_core;
