
#ifdef OSX
#import <Cocoa/Cocoa.h>
#endif

#include "CoreIOS.h"

#ifdef IOS
#import <UIKit/UIKit.h>
#import <sys/utsname.h>
#endif

#include "Platforms/IOS/OpenUDID.h"


ICore* core;

CoreIOS en_core;

CoreIOS::CoreIOS():CoreBase()
{
    core = this;
}

bool CoreIOS::Init()
{
    String::Init();

    CoreBase::Init();
    
    String::Copy(info.device_vendor, 128, "Apple");
    
    NSString* odin = [OpenUDID value];
    
    const char* dev_id_ptr = [odin UTF8String];
    
    String::Copy(info.device_id, 128, dev_id_ptr);
    
    struct utsname systemInfo;
    
    uname(&systemInfo);
    
    NSString* modelName = [NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding];
    
    const char* mdl_name = [modelName UTF8String];
    
    int k = strstr(mdl_name, ",") - mdl_name;
    
    memcpy(info.device_name, mdl_name, k);
    info.device_name[k] = 0;
    
    TraceTo("App", "Device: %s", info.device_name);
    TraceTo("App", "id: %s", info.device_id);
    
    
	if (!files.Init()) return false;
    if (!fonts.Init()) return false;
	if (!render.Init()) return false;	
	if (!controls.Init()) return false;
	if (!models.Init()) return false;			
	if (!sceneManager.Init()) return false;
    if (!script.Init()) return false;
	if (!userStats.Init()) return false;
	if (!sound.Init()) return false;
    if (!network.Init()) return false;
    if (!online_userstats.Init()) return false;
	if (!mediaLibrary.Init()) return false;
    if (!mobile.Init()) return false;
    
	//if (!physics.Init()) return false;
	
	return true;
}

void CoreIOS::CountDeltaTime(bool need_clamp)
{
	CoreBase::CountDeltaTime(need_clamp);
}

void CoreIOS::TraceToImpl(const char* log, const char *str )
{
	NSLog(@"%s", str);
}

void CoreIOS::Work()
{	
	ExecuteTasks();
	
	render.Begin();
	render.End();
}

Platform CoreIOS::GetCurrentPlatform()
{
#ifdef IOS
	return platform_ios;
#endif
	
#ifdef OSX
	return platform_osx;
#endif
}

void CoreIOS::SetLocale(const char* set_locale)
{
    String::Copy(locale, 16, set_locale);
    
    if (strlen(locale) > 2)
    {
        locale[2] = 0;
    }
}

void CoreIOS::Release()
{
    CoreBase::Release();
    online_userstats.Release();
    network.Release();
	sceneManager.Release();
	controls.Release();
	models.Release();
	//animation.Release();
	render.Release();
    fonts.Release();
	files.Release();
}


