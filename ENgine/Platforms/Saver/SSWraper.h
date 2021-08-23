#ifndef SSWRAPER_H
#define SSWRAPER_H

#include "RealX.h"
#include "Scene\Scene_Manager.h"

typedef BOOL (PASCAL * VERIFYPWDPROC) (HWND);

class CSSWrapper
{
public:

	struct ScreenSaverDefOptions
	{
		bool  ShowFPS;
		bool  ExitOnMouseMove;
		bool  ShowTime;		
		int   MusicVolume;
		int   SoundVolume;		
		int   Width;
		int   Height;
		int   BitCount;
		int   FSAA;
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

	#define MAX_DISPLAYS 9
	#define NO_ADAPTER 0xffffffff
	#define NO_MONITOR 0xffffffff

	struct DISPLAY_DEVICE_FULL
	{
		DWORD  cb;
		TCHAR  DeviceName[32];
		TCHAR  DeviceString[128];
		DWORD  StateFlags;
		TCHAR  DeviceID[128];
		TCHAR  DeviceKey[128];
	};

	//-----------------------------------------------------------------------------
	// Name: struct MonitorInfo
	// Desc: Structure for holding information about a monitor
	//-----------------------------------------------------------------------------
	struct MonitorInfo
	{
		TCHAR          strDeviceName[128];
		TCHAR          strMonitorName[128];
		HMONITOR       hMonitor;
		RECT           rcScreen;
		DWORD          iAdapter; // Which D3DAdapterInfo corresponds to this monitor
		HWND           hWnd;

		// Error message state
		FLOAT          xError;
		FLOAT          yError;
		FLOAT          widthError;
		FLOAT          heightError;
		FLOAT          xVelError;
		FLOAT          yVelError;
	};

	enum TSaverMode
	{
		svm_normal = 0,		
		svm_full = 1
	};
	
	TSaverMode Mode;

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
	char str_OptionsCaption[256];	

	char str_VideoModeSelection[256];
	char str_Bits[256];
	char str_UseDesktop[256];

	char str_SoundControl[256];
	char str_MusicVolume[256];
	char str_SoundVolume[256];
	char str_EnableMusic[256];
	char str_EnableSFX[256];

	char str_Graphics[256];
	char str_Gamma[256];
	char str_FullscreenAA[256];
	char str_AASamples[256];
	char str_AANone[256];

	char str_MiscOptions[256];
	char str_ShowFPS[256];
	char str_ShowTime[256];
	char str_clck_12h[256];
	char str_clck_24h[256];
	char str_clck_analog[256];
	
	char str_ExitMouseMove[256];

	char str_Products[256];
	char str_Visitsite[256];
	char str_Visit[256];
	char str_About[256];

	char str_Registration[256];
	char str_RegSt[256];
	char str_RegStPersL[256];
	char str_UnregSt[256];
	char str_Register[256];
	char str_GetFull[256];

	char str_RestoreDef[256];
	char str_Ok[256];
	char str_Cancel[256];


	// Reg Messages
	char strMes_RegCaption[256];

	char strMes_RegName[256];
	char strMes_RegKey[256];

	char strMes_RegOk[256];
	char strMes_RegCancel[256];
	char strMes_RegBuy[256];

	char strMes_RegValid[256];
	char strMes_RegInvalid[256];

	// About Messages
	char strMes_AboutCaption[256];	
	char strMes_About[256];	
	char strMes_AboutButton[256];	



	char strKey[256];
	

	bool  bWindowHasResized;

	bool  Quit;
	bool  NeedShowReg;
	bool  DebugMode;
	bool  Init_Success;
	HWND m_hWndParent;
	SaverMode m_SaverMode;
	HINSTANCE m_hInstance;
	HWND  hWnd;

	MonitorInfo     m_Monitors[MAX_DISPLAYS];
	DWORD           m_dwNumMonitors;

	HINSTANCE       m_hPasswordDLL;
	BOOL m_bWaitForInputIdle;
	BOOL m_bIs9x;

	VERIFYPWDPROC   m_VerifySaverPassword;

	float TimeToShowNot;	
	ScreenSaverDefOptions CurrentOptions;	

	byte dig_R,dig_G,dig_B;
	TTextureReference DigitsTex;
	TTextureReference NagTex;
	TTextureReference UnregNagTex;

	TTextureReference dclk_Table;
	TTextureReference dclk_HourArrow;
	TTextureReference dclk_MinuteArrow;
	TTextureReference dclk_SecondArrow;

	TVBuffReference HUDMesh;

	float digit_dy;
	float digit_dx;
	
	float digit_density;

	float clock_x;
	float clock_y;

	float fps_x;
	float fps_y;

	float counter_x;
	float counter_y;	

	int Swap;
	int CurFps,fps;
	float LastTime;

	bool UseSFX;	
	
	CRaster hDialogSetting;
	CRaster hLogo;
	CRaster hPreview;	

	int dwFakeMouseMoveMsgAllowed;

	D3DPRESENT_PARAMETERS d3dpp;

	CSSWrapper();
	
	void    ReadCFG();
	void    Init( HINSTANCE hInstance );
	INT Run();
	
	bool EnableWReporter;	

	SaverMode ParseCommandLine( TCHAR* pstrCommandLine );
	
	void SettingsDefRead( ScreenSaverDefOptions* pOptions );
	virtual void SettingsRead();

	void SettingsDefWrite( ScreenSaverDefOptions* pOptions );
	virtual void SettingsWrite();
	
	virtual void SetDefSettings();

	virtual BOOL CheckReadedRegKey();
	virtual int  CheckEnteredRegKey(HWND hDlg);

	virtual void ReadDialogTexts();

	VOID InitDefConfigControls(HWND hDlg);
	virtual VOID InitConfigControls(HWND hDlg);

	void UpdateComboBoxes();

	BOOL InterruptSaver();
	VOID ShutdownSaver();
	VOID ChangePassword();

	LRESULT CALLBACK Proc_ConfigDef( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual LRESULT CALLBACK Proc_Config( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	VOID DoConfig();	

	virtual LRESULT CALLBACK Proc_Reg( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	VOID DoReg();

	LRESULT CALLBACK Proc_About( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam );
	VOID DoAbout();

	BOOL WINAPI Proc_SaverDef( HWND hWnd, UINT uMsg, WPARAM wParam,LPARAM lParam );
	virtual LRESULT CALLBACK Proc_Saver( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	HRESULT DoSaver();

	VOID EnumMonitors( VOID );
	
	HRESULT CreateSaverWindow();	

	HRESULT EE_Core_CreateD3Device(HWND hWnd);

	virtual void CheckDeviceCapabilitys();

	virtual HRESULT EE_Core_InitD3Device();
	
	virtual HRESULT EE_Core_DrawLoading();

	HRESULT EE_Core_LoadDefMedia();
	virtual HRESULT EE_Core_LoadData();	

	HRESULT EE_Core_MainLoop();

	virtual HRESULT EE_Core_AdvanceScene(float timedelta);
	virtual HRESULT EE_Core_Render(float timedelta);
	HRESULT EE_Core_RenderDef();

	virtual HRESULT EE_Core_UnloadAllD3DData();
	HRESULT EE_Core_UnloadDefD3DData();

	HRESULT EE_Core_DestroyD3D();
};

#endif