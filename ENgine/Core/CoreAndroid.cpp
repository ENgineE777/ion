
#include "CoreAndroid.h"
#include "Common/Utils.h"

#include <android/log.h>

ICore* core = null;
CoreAndroid en_core;

CoreAndroid::CoreAndroid():CoreBase(), logs(_FL_)
{
	core = this;
}

bool CoreAndroid::Init()
{
    CoreBase::Init();

	if (!controls.Init()) return false;
	if (!files.Init()) return false;	
	
	if (!fonts.Init()) return false;

	if (!render.Init()) return false;
	
	if (!sceneManager.Init()) return false;
	if (!script.Init()) return false;

	//if (!physics.Init()) return false;
	if (!models.Init()) return false;
	if (!sound.Init()) return false;	
	//if (!localize.Init("loc")) return false;	

	if (!user_stats.Init()) return false;	
	if (!online_user_stats.Init()) return false;		
	if (!network.Init()) return false;	
	if (!mediaLibrary.Init()) return false;
	if (!mobile.Init()) return false;

	return true;
}

void CoreAndroid::TraceToImpl(const char* log, const char *buffer)
{
	char str[64];

	//getFilesDir();
	//getExternalStorageDirectory();

	String::Copy(str, 64, "/sdcard/Android/data/ion/");
	String::Cat(str, 64, log);

	String::Cat(str, 64, ".txt");	

	if (!logs.Get(log))
	{
		logs.Add(log);
		
		FILE * file = fopen(str, "w");
	
		if(file)
		{		
			fwrite("\n", 1, 1, file);
			fclose(file);
		}
	}
	
	__android_log_print(ANDROID_LOG_INFO,"ION",buffer);

	FILE * file = fopen(str, "a");
	
	if (file)
	{
		fseek(file, 0, SEEK_END);
		
		fwrite(timeStr, strlen(timeStr), 1, file);

		long len = strlen(buffer);
		fwrite(buffer, len, 1, file);
		fwrite("\n", 1, 1, file);
		fclose(file);
	}
}

void CoreAndroid::Work()
{
	ExecuteTasks();

	render.Begin();
	render.End();	
}

void CoreAndroid::SetDeviceVendor(const char* set_device_vendor)
{
	String::Copy(info.device_vendor, 128, set_device_vendor);
}

void CoreAndroid::SetDeviceName(const char* set_device_name)
{
	String::Copy(info.device_name, 128, set_device_name);
}

void CoreAndroid::SetDeviceID(const char* set_device_id)
{
	String::Copy(info.device_id, 128, set_device_id);
}

const char* CoreAndroid::SetLocale(const char* set_locale)
{
	String::Copy(locale, 16, set_locale);
}

void CoreAndroid::Release()
{
    CoreBase::Release();

	for (int i=0;i<(int)tasks.size();i++)
	{
		tasks[i].tasks.clear();
	}

	tasks.clear();

	network.Release();
	sound.Release();
	script.Release();
	sceneManager.Release();
	render.Release();
	fonts.Release();
	files.Release();	
	controls.Release();

	DumpMemoryLeaks();
}
