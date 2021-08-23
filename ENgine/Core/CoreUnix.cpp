#pragma once

#include "CoreUnix.h"
#include "Common/Utils.h"

#include <openssl/sha.h>

ICore* core = null;
CoreUnix en_core;

CoreUnix::CoreUnix():CoreBase(), logs(_FL_)
{	
	core = this;

	skip_render = false;
}

bool CoreUnix::Init()
{
    CoreBase::Init();
	String::Copy(info.device_vendor, 128, "user");
	String::Copy(info.device_name, 128, "PC");
		
	GenerateDeviceID();

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

	if (!userstats.Init()) return false;	
	if (!online_userstats.Init()) return false;		
	if (!network.Init()) return false;	
	if (!mediaLibrary.Init()) return false;	
	if (!mobile.Init()) return false;

	return true;
}

void CoreUnix::CountDeltaTime(bool need_clamp)
{
	/*float cur = 0.001f * GetTickCount();

	if ( lastTime < -0.5f )
	{
		deltaTime = 0.0f;
	}
	else
	{
		deltaTime = ( cur - lastTime );
	}

	CoreBase::CountDeltaTime();	

	/*if (controls.ControlState(DIK_F3, ControlsService::Active))
	{
		deltaTime *= 0.2f;
	}
	else
	if (controls.ControlState(DIK_F2, ControlsService::Active))
	{
		deltaTime *= 3.0f;
	}*/

	//lastTime = cur;
}

void CoreUnix::TraceToImpl(const char* log, const char* text)
{
	char str[64];

	String::Copy(str, 64, log);
	String::Cat(str, 64, ".txt");	

	if (!logs.Get(log))
	{
		logs.Add(log);
		
		FILE * file = fopen(str, "w");
	
		if(file)
		{					
			fclose(file);
		}
	}
			
	printf(text);
	printf( "\n" );

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

void CoreUnix::ResetLogs()
{
	logs.Clear();
}

void CoreUnix::Work()
{
	ExecuteTasks();

	if (!skip_render)
	{
		RenderFrame();
	}
}

void CoreUnix::RenderFrame()
{
	render.Begin();
	render.End();	
}

void CoreUnix::GenerateDeviceID()
{
	/*PIP_ADAPTER_INFO AdapterInfo;
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

	free(AdapterInfo);*/
}

void CoreUnix::SetSkipRender(bool set)
{
	skip_render = set;
}

void CoreUnix::Release()
{	
    CoreBase::Release();
	network.Release();
	online_userstats.Release();	
	sceneManager.Release();
	script.Release();
	fonts.Release();
	controls.Release();		
	render.Release();
	files.Release();

	DumpMemoryLeaks();
}
