#pragma once

#include "Common/Common.h"
#include "Common/TaskHolder.h"

#include "Core/Control/IControlsService.h"
#include "Core/Localize/ILocalizeService.h"
#include "Core/UserStats/IUserStatsService.h"
#include "Core/Network/INetworkService.h"
#include "Core/Files/IFileService.h"
#include "Core/Script/IScriptService.h"
#include "Core/Scene/ISceneManager.h"
#include "Render/RenderService.h"
class MobileService;
class ICore
{	
public:	

	struct Info
	{
		char device_name[256];
		char device_vendor[256];
		char device_id[256];
		
        Info()
        {
           device_id[0] = 0;
           device_name[0] = 0;
           device_vendor[0] = 0;
        }
	};

	Info info;

    virtual void SetProjectPath(const char* sPath) = 0;
    virtual const char* GetProjectPath() const = 0;

	virtual void AddTask(TaskHolder* taskHolder, TaskHoldersOwner* taskHolderOwner, TaskHolder::Task task, int level, float freq) = 0;
	virtual void ChangeTaskHolderOwner(TaskHolder* taskHolder, TaskHoldersOwner* taskHolderOwner) = 0;
	virtual void DelTask(TaskHolder* taskHolder, TaskHolder::Task task) = 0;

	virtual void DelTasks(TaskHolder* taskHolder) = 0;

	virtual IControlsService* Controls() = 0;
	virtual IUserStatsService* UserStats() = 0;	
	virtual	INetworkService* Network() = 0;
	virtual IFileService* Files() = 0;
	virtual IScriptService* Script() = 0;
	virtual ISceneManager* Scene() = 0;
    virtual MobileService* Mobile() = 0;
    virtual RenderService* Render() = 0;

    virtual void CreateNetLogger() = 0;

	
    virtual ILocalizeService* Localize()  = 0;		

	virtual bool Init() = 0;
	virtual void CountDeltaTime(bool need_clamp) = 0;
	virtual float GetDeltaTime() = 0;
    virtual double GetElapsedTime() = 0;
	virtual int	  GetFPS() = 0;
	virtual bool IsEditorMode() = 0;
	virtual void TraceToImpl(const char* log, const char* text) = 0;
	virtual void TraceTo(const char* log, const char* format, ...) = 0;
	virtual void Work() = 0;
	virtual void Exit() = 0;
	virtual const char* GetLocale() = 0;
	virtual void Release() = 0;		

	virtual Platform GetCurrentPlatform() = 0;	
    
};

extern DEF_EXPORT ICore* core;
