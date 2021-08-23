
#pragma once

#include "core/core.h"
#include <windows.h>
#include <windowsx.h>
#include <multimon.h>
#include "Common/BMP.h"

class SSaver
{
public:

	struct ScreenSaverDefOptions
	{
		bool  ExitOnMouseMove;
		bool  ShowTime;		
		int   MusicVolume;
		int   Width;
		int   Height;
		int   BitCount;
		bool  UseDktpSet;	
		int   TimerType;
		int   GammControl;	
	};

	enum SaverMode
	{
		sm_config,         // Config dialog box
		sm_preview,        // Mini preview window in Display Properties dialog
		sm_full,           // Full-on screensaver mode
		sm_test,           // Test mode
		sm_passwordchange  // Change password
	};

	char strScreenRegName[512];

	// Links
	char strMainWebSite[512];
	char strGetFullVersion[512];
	char strDigitalRiverRegName[512];
	char URL[512];

	void GetWebStoreURL();	

	// Welcome
	char strMes_StartWn[256];
	char strMes_NotCaption[256];

	// Main Dialog	

	char str_Bits[64];

	char str_Registration[256];
	char str_RegSt[256];
	char str_RegStPersL[256];
	char str_UnregSt[256];
	char str_Register[256];
	char str_GetFull[256];

	char strMes_RegCaption[256];
	char strMes_RegValid[256];
	char strMes_RegInvalid[256];


	static const char* windowName;


	char strKey[256];

	bool  Quit;
	bool  NeedShowReg;
	bool  DebugMode;
	bool  Init_Success;
	HWND m_hWndParent;
	SaverMode m_SaverMode;
	HINSTANCE m_hInstance;
	HWND  hWnd;

	float TimeToShowNot;	
	ScreenSaverDefOptions CurrentOptions;	

	bool UseSFX;	
	
	CRaster hDialogSetting;
	CRaster hPreview;	

	int dwFakeMouseMoveMsgAllowed;

	SSaver();
	
	void    ReadCFG();
	void    Init( HINSTANCE hInstance );
	int		Run();

	SaverMode ParseCommandLine( TCHAR* pstrCommandLine );
	
	void SettingsRead( ScreenSaverDefOptions* pOptions );
	void SettingsWrite( ScreenSaverDefOptions* pOptions );

	virtual bool CheckReadedRegKey();
	virtual int  CheckEnteredRegKey(HWND hDlg);

	void InitConfigControls(HWND hDlg);

	void UpdateComboBoxes();

	void LocalizeConfig(HWND hDlg);
	void LocalizeRegistration(HWND hDlg);

	bool InterruptSaver();
	void ShutdownSaver();

	bool Proc_Config( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void DoConfig();	

	bool Proc_Reg( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void DoReg();

	bool Proc_Saver( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void DoSaver();
	
	bool CreateSaverWindow();	

	void SetVideoMode();

	void InitRenderDevice();
	
	void LoadData();	

	void MainLoop();

	void Release();
};