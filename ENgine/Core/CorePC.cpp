#pragma once

#include "CorePC.h"
#include "Windowsx.h"
#include "commctrl.h"

#include "Common/ExceptionReport.h"

#include "Common/Utils.h"

#include <openssl/sha.h>

ExceptionReport report;

ICore* core = null;
CorePC en_core;

#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")

CorePC::CorePC():CoreBase(), logs(_FL_)
{
	hwnd = 0;	
	hInstance = 0;

	core = this;

	skip_render = false;

	render = null;
}

bool CorePC::Init()
{
    CoreBase::Init();
	String::Copy(info.device_vendor, 128, "user");
	String::Copy(info.device_name, 128, "PC");
		
	GenerateDeviceID();

	res_manager.Init();	
	if (!files.Init()) return false;	
	if (!fonts.Init()) return false;

#ifdef SCR_SAVER

	HWND hDsk = GetDesktopWindow();
	RECT rc;
	GetClientRect(hDsk, &rc);

	if (!render.Init(rc.right, rc.bottom, false)) return false;

#else

	int width  = ReadINI("settings.ini", 1024, "RENDER", "Width");
	int height = ReadINI("settings.ini", 768 , "RENDER", "Height");

	String::Copy(locale, 16, ReadINI("settings.ini", "en" , "Editor", "Locale"));

	render = new RenderServiceD3D11();

	if (!render->Init(width, height)) return false;

#endif
		
	if (!en_core.Render()->IsUseDummyRender())
	{	
		if (!controls.Init())
		{
			//return false;	
		}
	}

	script.skip_help = false;

	if (!sceneManager.Init()) return false;	
	if (!script.Init()) return false;	
	if (!models.Init()) return false;	
	if (!online_userstats.Init()) return false;		
	if (!network.Init()) return false;				
	if (!mediaLibrary.Init()) return false;
	if (!mobile.Init()) return false;
	if (!mobile.Init()) return false;

	report.SetLogFileName( "crash" );

	script.skip_help = true;	

    CreateNetLogger();
	return true;
}

void CorePC::CountDeltaTime(bool need_clamp)
{
	float cur = 0.001f * GetTickCount();

	if ( lastTime < -0.5f )
	{
		deltaTime = 0.0f;
	}
	else
	{
		deltaTime = ( cur - lastTime );
	}

	CoreBase::CountDeltaTime(need_clamp);	

	if (controls.ControlState(DIK_F3, ControlsService::Active))
	{
		deltaTime *= 0.2f;
	}
	else
	if (controls.ControlState(DIK_F2, ControlsService::Active))
	{
		deltaTime *= 3.0f;
	}	

	lastTime = cur;	
}

void CorePC::TraceToImpl(const char* log, const char* text)
{
	char str[512];

	String::Copy(str, 512, log);
	String::Cat(str, 512, ".txt");	

	if (!logs.Get(log))
	{
		logs.Add(log);
		
		FILE * file = fopen(str, "w");
	
		if(file)
		{					
			fclose(file);
		}
	}
		
	OutputDebugString(text);
	OutputDebugString( "\n" );

	FILE * file = fopen(str, "a");
	if(file)
	{
		fseek(file, 0, SEEK_END);			

		long len = strlen(text);
		fwrite(text, len, 1, file);
		fwrite("\n", 1, 1, file);
		fclose(file);
	}
}

void CorePC::SetHWND(HWND wnd)
{
	hwnd = wnd;
}

HWND CorePC::GetHWND()
{
	return hwnd;
}

void CorePC::SetInstance(HINSTANCE instance)
{
	hInstance = instance;
}

HINSTANCE CorePC::GetInstance()
{
	return hInstance;
}

void CorePC::ResetLogs()
{
	logs.Clear();
}

void CorePC::Work()
{
	ExecuteTasks();

	if (!skip_render)
	{
		RenderFrame();
	}
	
	res_manager.CheckModify();
}

void CorePC::RenderFrame()
{
	render->Begin();
	render->End();	
}


void CorePC::GenerateDeviceID()
{
	PIP_ADAPTER_INFO AdapterInfo;
	DWORD dwBufLen = sizeof(AdapterInfo);
	char *mac_addr = (char*)malloc(17);

	AdapterInfo = (IP_ADAPTER_INFO *) malloc(sizeof(IP_ADAPTER_INFO));

	if (AdapterInfo == NULL)
	{
		printf("Error allocating memory needed to call GetAdaptersinfo\n");
	}

	// Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen     variable
	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		AdapterInfo = (IP_ADAPTER_INFO *) malloc(dwBufLen);

		if (AdapterInfo == NULL)
		{
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
		}
	}

	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR)
	{
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info
		
		unsigned char obuf[30];
		unsigned char ibuf[30];

		sprintf((char*)&ibuf[0], "%02X:%02X:%02X:%02X:%02X:%02X",
				pAdapterInfo->Address[0], pAdapterInfo->Address[1],
				pAdapterInfo->Address[2], pAdapterInfo->Address[3],
				pAdapterInfo->Address[4], pAdapterInfo->Address[5]);										
			
		SHA1(ibuf, strlen((const char*)ibuf), obuf);

		int i;
				
		info.device_id[0] = 0;

		for (i = 0; i < 20; i++)
		{
			char tmp[8];
				
			sprintf(tmp, "%02x", obuf[i]);
			strcat(info.device_id, tmp);				
		}
	}               

	free(AdapterInfo);
}

void CorePC::SetSkipRender(bool set)
{
	skip_render = set;
}

void CorePC::Release()
{	
    CoreBase::Release();
	network.Release();
	online_userstats.Release();	
	sceneManager.Release();
	script.Release();
	fonts.Release();
	controls.Release();		
	render->Release();
	files.Release();		

	DumpMemoryLeaks();
}
