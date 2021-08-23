
#include "SSWraper.h"

#include <windowsx.h>

#define COMPILE_MULTIMON_STUBS
#include <multimon.h>

#include <tchar.h>
#include <commctrl.h>
#include <regstr.h>

#include "resource.h"

char Name[256]="";
char RegKey[16]="";

float alpha,betta;
float xc,yc,zc;
bool  allowFreeCamera = true;

#include "RealX.h"

CSSWrapper::ScreenSaverDefOptions DefaultOptions =
{
	FALSE, 
	TRUE, 
	false,
	100,
	100,	
	800,
	600,
	32,
	0,
	TRUE,	
	2,
	100
};	

CSSWrapper *Wrap;

//-----------------------------------------------------------------------------
BOOL WINAPI SSRegProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return Wrap->Proc_Reg( hDlg, uMsg, wParam, lParam );
}
//-----------------------------------------------
CSSWrapper::CSSWrapper()
{
	SetDefSettings();
	CurrentOptions=DefaultOptions;	

	NeedShowReg=false;

	DebugMode = false;
	EnableWReporter=DebugMode;

	Wrap=this;
	dwFakeMouseMoveMsgAllowed=65;
	
	dig_R=dig_G=dig_B=255;
	
	digit_dy=0.064f*0.75f;
	digit_dx=0.037f;
	
	digit_density=1.0f;

	clock_x=0.70f;
	clock_y=0.92f;

	fps_x=0.01f;
	fps_y=0.01f;

	counter_y=0.01f;
	counter_x=0.995f;

	Mode = svm_normal;

	Swap=1;

	TimeToShowNot=0;		

	UseSFX=true;

	CurFps = fps = 0;

	bWindowHasResized = false;	

	strcpy(strKey,"keykeykeykeykeykeykeykeykey");

	strcpy(strMainWebSite,"realx");
	strcpy(strGetFullVersion,"realx");

	strcpy(URL,"");	

	strcpy(strMes_StartWn,"realx");	
	strcpy(strMes_NotCaption,"realx");	
	strcpy(str_OptionsCaption,"realx");	
	

	strcpy(str_OptionsCaption,"realx");	
	strcpy(str_RegSt,"realx");
	strcpy(str_RegStPersL,"realx");	
	strcpy(str_UnregSt,"realx");	
	strcpy(str_VideoModeSelection,"realx");	
	strcpy(str_Bits,"realx");
	strcpy(str_UseDesktop,"realx");	
	strcpy(str_SoundControl,"realx");	
	strcpy(str_MusicVolume,"realx");
	strcpy(str_SoundVolume,"realx");
	strcpy(str_EnableMusic,"realx");	
	strcpy(str_EnableSFX,"realx");	
	strcpy(str_Graphics,"realx");	
	strcpy(str_Gamma,"realx");	
	strcpy(str_FullscreenAA,"realx");
	strcpy(str_AASamples,"realx");	
	strcpy(str_AANone,"realx");		
	strcpy(str_MiscOptions,"realx");	
	strcpy(str_ShowFPS,"realx");	
	strcpy(str_ShowTime,"realx");		
	strcpy(str_clck_12h,"realx");	
	strcpy(str_clck_24h,"realx");
	strcpy(str_clck_analog,"realx");
	strcpy(str_ExitMouseMove,"realx");
												

	strcpy(str_Products,"realx");	
	strcpy(str_Visitsite,"realx");	
	strcpy(str_Visit,"realx");	
	strcpy(str_About,"realx");	

	strcpy(str_Registration,"realx");		
	strcpy(str_RegSt,"realx");	
	strcpy(str_UnregSt,"realx");	
	strcpy(str_Register,"realx");	
	strcpy(str_GetFull,"realx");

	strcpy(str_RestoreDef,"realx");	
	strcpy(str_Ok,"realx");	
	strcpy(str_Cancel,"realx");
		
	strcpy(strMes_RegCaption,"realx");

	strcpy(strMes_RegName,"realx");
	strcpy(strMes_RegKey,"realx");
	strcpy(strMes_RegOk,"realx");
	strcpy(strMes_RegBuy,"realx");	
	strcpy(strMes_RegCancel,"realx");	

	strcpy(strMes_RegValid,"realx");
	strcpy(strMes_RegInvalid,"realx");
	
	strcpy(strMes_AboutCaption,"realx");
	strcpy(strMes_About,"realx");
	strcpy(strMes_AboutButton,"realx");	

	strcpy(strScreenRegName,"Software\\RealX\\3D Saver");
	strcpy(strDigitalRiverRegName,"Software\\RealX\\3D Saver");
}

void CSSWrapper::GetWebStoreURL ()
{ 
	strcpy(URL, "");

	HKEY hkey; 
	DWORD dwType = REG_SZ;
	DWORD dwLength=sizeof(strDigitalRiverRegName);	

	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, strDigitalRiverRegName, 0, KEY_READ, &hkey ) )
	{
		RegQueryValueEx( hkey, TEXT("BuyURL"), NULL, &dwType, (BYTE*)&URL  , &dwLength );
		RegCloseKey( hkey );

		if (strcmp(URL,"")!=0) return;
	}

	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CURRENT_USER, strDigitalRiverRegName, 0, KEY_READ, &hkey ) )
	{
		RegQueryValueEx( hkey, TEXT("BuyURL"), NULL, &dwType, (BYTE*)&URL  , &dwLength );
		RegCloseKey( hkey );

		if (strcmp(URL,"")!=0) return;		
	}

	strcpy(URL,strGetFullVersion);
}

void CSSWrapper::SettingsRead()
{
}

void CSSWrapper::SettingsDefRead( ScreenSaverDefOptions* pOptions )
{
	GetWebStoreURL();

	HKEY hkey;
    DWORD dwType = REG_DWORD;
	DWORD dwType2 = REG_SZ;
    DWORD dwLength;
	DWORD dwLength1=sizeof(char)*86;
	DWORD dwLength2=sizeof(char)*17;
	DWORD Value=0;

    if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_CURRENT_USER, strScreenRegName, 
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hkey, NULL ) )
    {
        dwLength = sizeof(DWORD);
		RegQueryValueEx( hkey, TEXT("ShowFPS"), NULL, &dwType, (BYTE*)&Value, &dwLength );
		if (Value==2) pOptions->ShowFPS=true;
		if (Value==1) pOptions->ShowFPS=false;
		Value=0;

		RegQueryValueEx( hkey, TEXT("TimerType"), NULL, &dwType, (BYTE*)&Value, &dwLength );
		pOptions->TimerType=Value;
		

		dwLength = sizeof(DWORD);
		RegQueryValueEx( hkey, TEXT("GmCntrl"), NULL, &dwType, (BYTE*)&Value, &dwLength );
		if (Value!=0) pOptions->GammControl=(byte)Value;
		Value=0;

		RegQueryValueEx( hkey, TEXT("ShowTime"), NULL, &dwType, (BYTE*)&Value, &dwLength );
		if (Value==2) pOptions->ShowTime=true;
		if (Value==1) pOptions->ShowTime=false;
		Value=0;

		RegQueryValueEx( hkey, TEXT("ExitOnMouseMove"), NULL, &dwType, (BYTE*)&Value, &dwLength );
		if (Value==2) pOptions->ExitOnMouseMove=true;
		if (Value==1) pOptions->ExitOnMouseMove=false;
		Value=0;

		RegQueryValueEx( hkey, TEXT("MusicVolume"), NULL, &dwType, (BYTE*)&Value, &dwLength );
		if (Value!=0) pOptions->MusicVolume=Value;
		Value=0;

		RegQueryValueEx( hkey, TEXT("SoundVolume"), NULL, &dwType, (BYTE*)&Value, &dwLength );
		if (Value!=0) pOptions->SoundVolume=Value;
		Value=0;

		RegQueryValueEx( hkey, TEXT("UseDesktopSettings"), NULL, &dwType, (BYTE*)&Value, &dwLength );
		if (Value==2) pOptions->UseDktpSet=true;
		if (Value==1) pOptions->UseDktpSet=false;
		Value=0;		

		RegQueryValueEx( hkey, TEXT("Height"), NULL, &dwType, (BYTE*)&Value, &dwLength );
		if (Value!=0) pOptions->Height=Value;
		Value=0;

		RegQueryValueEx( hkey, TEXT("Width"), NULL, &dwType, (BYTE*)&Value, &dwLength );
		if (Value!=0) pOptions->Width=Value;
		Value=0;

		RegQueryValueEx( hkey, TEXT("BitCount"), NULL, &dwType, (BYTE*)&Value, &dwLength );
		if (Value!=0) pOptions->BitCount=Value;
		Value=0;

		RegQueryValueEx( hkey, TEXT("FSAA"), NULL, &dwType, (BYTE*)&Value, &dwLength );
		if (Value!=0) pOptions->FSAA=Value;
		Value=0;

		RegQueryValueEx( hkey, TEXT("Key"), NULL, &dwType2, (BYTE*)&RegKey, &dwLength2 );
		RegQueryValueEx( hkey, TEXT("Name"), NULL, &dwType2, (BYTE*)&Name  , &dwLength1 );
		
		RegCloseKey( hkey );

		SceneManager.bIsSaverRegistered = false;

		if (CheckReadedRegKey()) SceneManager.bIsSaverRegistered=true;
	}
}
//-----------------------------------------------------------------------------
void CSSWrapper::SettingsWrite()
{
}
//-----------------------------------------------------------------------------
void CSSWrapper::SetDefSettings()
{
}
//-----------------------------------------------------------------------------
void CSSWrapper::SettingsDefWrite( ScreenSaverDefOptions* pOptions )
{
	HKEY hkey;
	DWORD value;
    if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_CURRENT_USER, strScreenRegName, 
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL ) )
    {
		if (pOptions->ShowFPS)
		{
			value=2;
		}
		else 
		{
			value=1;
		}
		RegSetValueEx( hkey, TEXT("ShowFPS"), NULL, REG_DWORD, (BYTE*)&value, sizeof(DWORD) );

		value=pOptions->TimerType;		
		RegSetValueEx( hkey, TEXT("TimerType"), NULL, REG_DWORD, (BYTE*)&value, sizeof(DWORD) );		

		if (pOptions->ShowTime)
		{
			value=2;
		}
		else 
		{
			value=1;
		}
		RegSetValueEx( hkey, TEXT("ShowTime"), NULL, REG_DWORD, (BYTE*)&value, sizeof(DWORD) );

		if (pOptions->ExitOnMouseMove)
		{
			value=2;
		}
		else 
		{
			value=1;
		}
		RegSetValueEx( hkey, TEXT("ExitOnMouseMove"), NULL, REG_DWORD, (BYTE*)&value, sizeof(DWORD) );
		
		if (pOptions->UseDktpSet)
		{
			value=2;
		}
		else 
		{
			value=1;
		}
		RegSetValueEx( hkey, TEXT("UseDesktopSettings"), NULL, REG_DWORD, (BYTE*)&value, sizeof(DWORD) );

		value=pOptions->GammControl;
		RegSetValueEx( hkey, TEXT("GmCntrl"), NULL, REG_DWORD, (BYTE*)&value, sizeof(DWORD) );

		value=pOptions->Height;
		RegSetValueEx( hkey, TEXT("Height"), NULL, REG_DWORD, (BYTE*)&value, sizeof(DWORD) );

		value=pOptions->Width;
		RegSetValueEx( hkey, TEXT("Width"), NULL, REG_DWORD, (BYTE*)&value, sizeof(DWORD) );

		value=pOptions->BitCount;
		RegSetValueEx( hkey, TEXT("BitCount"), NULL, REG_DWORD, (BYTE*)&value, sizeof(DWORD) );

		value=pOptions->FSAA;
		RegSetValueEx( hkey, TEXT("FSAA"), NULL, REG_DWORD, (BYTE*)&value, sizeof(DWORD) );

		value=pOptions->MusicVolume;
		RegSetValueEx( hkey, TEXT("MusicVolume"), NULL, REG_DWORD, (BYTE*)&value, sizeof(DWORD) );

		value=pOptions->SoundVolume;
		RegSetValueEx( hkey, TEXT("SoundVolume"), NULL, REG_DWORD, (BYTE*)&value, sizeof(DWORD) );

		RegSetValueEx( hkey, TEXT("Name"), NULL, REG_SZ, (BYTE*)&Name, sizeof(char)*85 );
		RegSetValueEx( hkey, TEXT("Key"), NULL, REG_SZ, (BYTE*)&RegKey, sizeof(char)*16 );

		RegFlushKey( hkey );
        RegCloseKey( hkey );
    }
}
//-----------------------------------------------------------------------------
int CSSWrapper::CheckEnteredRegKey(HWND hDlg)
{
	char Key[256];	
	strcpy(Key,strKey);

	int  Codes[16];
	int  Codes2[16];
	int  Codes3[16];

	int NameLen = GetWindowTextLength(GetDlgItem(hDlg, IDE_NAME));
	
	//if (NameLen<=0||NameLen>80) return 1;
	
	char* NameBuf;

	NameBuf = (char*)GlobalAlloc(GPTR, NameLen + 1);
	GetDlgItemText(hDlg, IDE_NAME, NameBuf, NameLen + 1);

	for(int i=0;i<NameLen;i++)
	{
		Name[i]=NameBuf[i];
	}
	Name[NameLen]=0;

	if (NameLen>25) NameLen=25;

	int RealNameLen=NameLen;
	if (NameLen>15) RealNameLen=15;

	int cur_index=0;
	for(i=0;i<16;i++)
	{
		if (i==15) Codes[i]=NameLen+65;
		else
		Codes[i]=NameBuf[cur_index];

		if (Codes[i]==32)
		{
			Codes[i]=0;
		}
		else
		if (48<=Codes[i]&&Codes[i]<=57)
		{
			Codes[i]=Codes[i]-48+1;
		}
		else
		if (65<=Codes[i]&&Codes[i]<=90)
		{
			Codes[i]=Codes[i]-65+11;
		}
		else
		if (97<=Codes[i]&&Codes[i]<=122)
		{
			Codes[i]=Codes[i]-97+37;
		}
		else
		{
			//return 2;
		}
		cur_index++;
		if (cur_index>=RealNameLen) cur_index=0;
	}

	GlobalFree((HANDLE)NameBuf);

	int KeyLen=strlen(Key);
	int RealKeyLen=KeyLen;
	if (KeyLen>16) RealKeyLen=16;

	cur_index=0;
	for(i=0;i<16;i++)
	{
		Codes2[i]=Key[cur_index];
		if (Codes2[i]==32)
		{
			Codes2[i]=0;
		}
		else
		if (48<=Codes2[i]&&Codes2[i]<=57)
		{
			Codes2[i]=Codes2[i]-48+1;
		}
		else
		if (65<=Codes2[i]&&Codes2[i]<=90)
		{
			Codes2[i]=Codes2[i]-65+11;
		}
		else
		if (97<=Codes2[i]&&Codes2[i]<=122)
		{
			Codes2[i]=Codes2[i]-97+37;
		}
		else
		{
			return 5;
		}
		cur_index++;
		if (cur_index>=RealKeyLen) cur_index=0;
	}

	for (i=0;i<8;i++)
	{
		int tmp=Codes2[i];
		Codes2[i]=Codes2[8+i];
		Codes2[8+i]=tmp;
	}

	int RegKeyLen = GetWindowTextLength(GetDlgItem(hDlg, IDE_REGKEY));
		
	char* RegKeyBuf;

	RegKeyBuf = (char*)GlobalAlloc(GPTR, RegKeyLen + 1);
	GetDlgItemText(hDlg, IDE_REGKEY, RegKeyBuf, RegKeyLen + 1);
	
	int index=0;

	while (RegKeyBuf[index]==' '&&index<RegKeyLen) index++;

	if (RegKeyLen-index<16)
	{
		GlobalFree((HANDLE)RegKeyBuf);
		return 3;
	}

	for(i=0;i<16;i++)
	{
		Codes3[i]=RegKeyBuf[index];
		RegKey[i]=RegKeyBuf[index];

		index++;

		if (Codes3[i]==45)
		{
			Codes3[i]=0;
		}
		else
		if (48<=Codes3[i]&&Codes3[i]<=57)
		{
			Codes3[i]=Codes3[i]-48+1;
		}
		else
		if (65<=Codes3[i]&&Codes3[i]<=90)
		{
			Codes3[i]=Codes3[i]-65+11;
		}
		else
		if (97<=Codes3[i]&&Codes3[i]<=122)
		{
			Codes3[i]=Codes3[i]-97+37;
		}
		else
		{
			return 4;
		}
	}
	GlobalFree((HANDLE)RegKeyBuf);

	for (i=0;i<16;i++)
	{
		Codes3[i]-=Codes2[i];
		if (Codes3[i]<0) Codes3[i]+=63;
	}
	
	for (i=0;i<8;i++)
	{
		int tmp=Codes3[i];
		Codes3[i]=Codes3[8+i];
		Codes3[8+i]=tmp;
	}

	for (i=0;i<8;i++)
	{
		if (Codes3[i]!=Codes3[15-i]) return 4;
	}

	return 0;
}
//-----------------------------------------------------------------------------
CSSWrapper::SaverMode CSSWrapper::ParseCommandLine( TCHAR* pstrCommandLine )
{
    m_hWndParent = NULL;

    // Skip the first part of the command line, which is the full path 
    // to the exe.  If it contains spaces, it will be contained in quotes.
    if (*pstrCommandLine == TEXT('\"'))
    {
        pstrCommandLine++;
        while (*pstrCommandLine != TEXT('\0') && *pstrCommandLine != TEXT('\"'))
            pstrCommandLine++;
        if( *pstrCommandLine == TEXT('\"') )
            pstrCommandLine++;
    }
    else
    {
        while (*pstrCommandLine != TEXT('\0') && *pstrCommandLine != TEXT(' '))
            pstrCommandLine++;
        if( *pstrCommandLine == TEXT(' ') )
            pstrCommandLine++;
    }

    // Skip along to the first option delimiter "/" or "-"
    while ( *pstrCommandLine != TEXT('\0') && *pstrCommandLine != TEXT('/') && *pstrCommandLine != TEXT('-') )
        pstrCommandLine++;

    // If there wasn't one, then must be config mode
    if ( *pstrCommandLine == TEXT('\0') )
        return sm_config;

    // Otherwise see what the option was
    switch ( *(++pstrCommandLine) )
    {
        case 'c':
        case 'C':
            pstrCommandLine++;
            while ( *pstrCommandLine && !isdigit(*pstrCommandLine) )
                pstrCommandLine++;
            if ( isdigit(*pstrCommandLine) )
            {
#ifdef _WIN64
                CHAR strCommandLine[2048];
                DXUtil_ConvertGenericStringToAnsiCb( strCommandLine, pstrCommandLine, sizeof(strCommandLine) );
                m_hWndParent = (HWND)(_atoi64(strCommandLine));
#else
                m_hWndParent = (HWND)LongToHandle(_ttol(pstrCommandLine));
#endif
            }
            else
            {
                m_hWndParent = NULL;
            }
            return sm_config;

        case 't':
        case 'T':
            return sm_test;

        case 'p':
        case 'P':
            // Preview-mode, so option is followed by the parent HWND in decimal
            pstrCommandLine++;
            while ( *pstrCommandLine && !isdigit(*pstrCommandLine) )
                pstrCommandLine++;
            if ( isdigit(*pstrCommandLine) )
            {
#ifdef _WIN64
                CHAR strCommandLine[2048];
                DXUtil_ConvertGenericStringToAnsiCb(strCommandLine, pstrCommandLine, sizeof(strCommandLine));
                m_hWndParent = (HWND)(_atoi64(strCommandLine));
#else
                m_hWndParent = (HWND)LongToHandle(_ttol(pstrCommandLine));
#endif
            }
            return sm_preview;

        case 'a':
        case 'A':
            // Password change mode, so option is followed by parent HWND in decimal
            pstrCommandLine++;
            while ( *pstrCommandLine && !isdigit(*pstrCommandLine) )
                pstrCommandLine++;
            if ( isdigit(*pstrCommandLine) )
            {
#ifdef _WIN64
                CHAR strCommandLine[2048];
                DXUtil_ConvertGenericStringToAnsiCb(strCommandLine, pstrCommandLine, sizeof(strCommandLine));
                m_hWndParent = (HWND)(_atoi64(strCommandLine));
#else
                m_hWndParent = (HWND)LongToHandle(_ttol(pstrCommandLine));
#endif
            }
            return sm_passwordchange;

        default:
            // All other options => run the screensaver (typically this is "/s")
            return sm_full;
    }
}
//-------------------------------------------------
BOOL CSSWrapper::InterruptSaver()
{
	ShutdownSaver();

	return true;	

    BOOL bPasswordOkay = FALSE;

    if( m_SaverMode == sm_test ||
        m_SaverMode == sm_full)// && !m_bCheckingSaverPassword )
    {
        if( m_bIs9x && m_SaverMode == sm_full )
        {
            // If no VerifyPassword function, then no password is set 
            // or we're not on 9x. 
            if ( m_VerifySaverPassword != NULL )
            {
                bool m_bCheckingSaverPassword = TRUE;

				if (Swap==0) Renderer.Present( NULL, NULL, NULL, NULL );
                int bPasswordOkay = m_VerifySaverPassword( hWnd );

                m_bCheckingSaverPassword = FALSE;

                if ( bPasswordOkay )
                {
                    // D3D devices are all torn down, so it's safe
                    // to discard all render units now (so we don't
                    // try to clean them up again later).
                    //m_dwNumRenderUnits = 0;
                }
                else
                {
                    // Back to screen saving...
                    SetCursor( NULL );
                    dwFakeMouseMoveMsgAllowed = 0;
					Swap=1;
					LastTime=(FLOAT)timeGetTime();

                    return false;
                }
            }			
        }
	}
		
	ShutdownSaver();

	return true;	
}
//-----------------------------------------------------------------------------
// Name: ShutdownSaver()
// Desc: 
//-----------------------------------------------------------------------------
VOID CSSWrapper::ShutdownSaver()
{
    // Unflag screensaver running if in full on mode
    //if ( m_SaverMode == sm_full )
    //{
      //  BOOL bUnused;
//        SystemParametersInfo( SPI_SCREENSAVERRUNNING, FALSE, &bUnused, 0 );
  //  }

	if (Renderer.pd3dDevice!=NULL)
	{		
		Renderer.GammaCtrl=100;
		
		Renderer.SetGamma();
	}

	EE_Core_UnloadDefD3DData();
    EE_Core_UnloadAllD3DData();
	EE_Core_DestroyD3D();
	
	KillTimer( hWnd, 7 );
	
	//PostQuitMessage(0);

    // Unload the password DLL (if we loaded it)
    if ( m_hPasswordDLL != NULL )
    {
        FreeLibrary( m_hPasswordDLL );
        m_hPasswordDLL = NULL;
    }

	Quit=true;

	WReporter.close();
    // Post message to drop out of message loop
	
    //PostQuitMessage( 1 );
}
//-----------------------------------------------------------------------------
BOOL WINAPI CSSWrapper::Proc_SaverDef( HWND hWnd, UINT uMsg, WPARAM wParam,
 			                    LPARAM lParam )
{
    switch ( uMsg )
    {
		case WM_CREATE:
			{
				if (m_SaverMode==sm_preview)
				{		
					byte *data_ptr;                                           
					unsigned long   data_size;			
					MemoryFile      rarfile;		

					OpenRARResource(&rarfile, IDD_DATA);

					if(urarlib_get(&data_ptr, &data_size,
								   "Data/Bitmap/Preview.bmp", &rarfile, "")) 
					{
						hPreview.LoadBMP("Bitmap/Preview.bmp",data_ptr);

						if(data_ptr != NULL) free(data_ptr);
					}					
				}
			
			}
			//SetCursor( NULL );

			break;
        case WM_USER:
            // All initialization messages have gone through.  Allow
            // 500ms of idle time, then proceed with initialization.
            SetTimer( hWnd, 1, 500, NULL );
            break;

        case WM_TIMER:
            // Initial idle time is done, proceed with initialization.
			if (1 ==LOWORD(wParam))
			//if (m_bWaitForInputIdle)
			{
				m_bWaitForInputIdle = FALSE;
				KillTimer( hWnd, 1 );
			}

			if (7 == LOWORD(wParam))
			{
				CurFps=fps;
				fps=0;
			}

            break;

        case WM_CLOSE:
		case WM_DESTROY:
		{					
			Quit=true;

			return TRUE;
		}

        case WM_SETCURSOR:
            if ( m_SaverMode == sm_full)// && !m_bCheckingSaverPassword )
            {
                // Hide cursor
                //SetCursor( NULL );
                return TRUE;
            }
            break;

        case WM_PAINT:
        {
			if (m_SaverMode==sm_preview)
			{
				HDC hdc;
				PAINTSTRUCT ps;

				hdc=BeginPaint (hWnd,&ps);				

				RECT rc;				
				GetClientRect( hWnd, &rc);

				hPreview.GDIPaint (hdc,rc.left,rc.top,rc.right,rc.bottom);
								

				EndPaint (hWnd,&ps);

				return 0;
			}
        }
        break;
		case WM_MOUSEMOVE:
			// Ignore any fake msgs triggered by DDraw::SetDisplayMode()
			if ( m_SaverMode != sm_preview && CurrentOptions.ExitOnMouseMove)
			{	
				dwFakeMouseMoveMsgAllowed--;

				if (dwFakeMouseMoveMsgAllowed<0)
					return InterruptSaver();				
			}
			break;

/*		case WM_MINIMAZE:
			break;*/
		case WM_KEYDOWN:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
			if (wParam==VK_SPACE) 
			{	
				//ShutdownSaver();
				
				//ShowWindow(hWnd,SW_HIDE);
				//DoAbout();
				//ShutdownSaver();

				if (Swap==0) Renderer.Present( NULL, NULL, NULL, NULL );

				NeedShowReg=true;

				ShutdownSaver();

				//DestroyMusicStuff();

				//ShowWindow(hWnd, SW_HIDE);

				//DialogBox( m_hInstance, MAKEINTRESOURCE(IDD_REGISTER), hWnd, SSRegProc );
				
				//DoReg();

				//ShellExecute(hWnd,NULL,g_strGetFullVerNag,NULL,NULL,SW_HIDE);
				//ShutdownSaver();
				//InterruptSaver();
				//ShutdownSaver();
				//PostQuitMessage(0);												
				return true;
			}		
			else
			if( m_SaverMode != sm_test && !DebugMode) return InterruptSaver();
			
            break;

        case WM_ACTIVATEAPP:
            if( wParam == FALSE && m_SaverMode != sm_test ) return InterruptSaver();
            break;

        case WM_POWERBROADCAST:
            if( wParam == PBT_APMSUSPEND && m_VerifySaverPassword == NULL ) return InterruptSaver();
            break;

        case WM_SYSCOMMAND: 
            if ( m_SaverMode == sm_full )
            {
                switch ( wParam )
                {
                    case SC_NEXTWINDOW:
                    case SC_PREVWINDOW:
                    case SC_SCREENSAVE:
					{
						if( !DebugMode) return InterruptSaver();
					}						
                    case SC_CLOSE:
					{						 
						 return InterruptSaver();
					}
                };
            }
            break;
    }

    return Proc_Saver( hWnd, uMsg, wParam, lParam );
}
//-----------------------------------------------------------------------------
LRESULT CALLBACK CSSWrapper::Proc_Saver( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
//-----------------------------------------------------------------------------
VOID CSSWrapper::EnumMonitors( VOID )
{
    DWORD iDevice = 0;
    DISPLAY_DEVICE_FULL dispdev;
    DISPLAY_DEVICE_FULL dispdev2;
    DEVMODE devmode;
    dispdev.cb = sizeof(dispdev);
    dispdev2.cb = sizeof(dispdev2);
    devmode.dmSize = sizeof(devmode);
    devmode.dmDriverExtra = 0;
	m_dwNumMonitors=0;
    MonitorInfo* pMonitorInfoNew;
    while( EnumDisplayDevices(NULL, iDevice, (DISPLAY_DEVICE*)&dispdev, 0) )
    {
        // Ignore NetMeeting's mirrored displays
        if( (dispdev.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) == 0 )
        {
            // To get monitor info for a display device, call EnumDisplayDevices
            // a second time, passing dispdev.DeviceName (from the first call) as
            // the first parameter.
            EnumDisplayDevices(dispdev.DeviceName, 0, (DISPLAY_DEVICE*)&dispdev2, 0);

            pMonitorInfoNew = &m_Monitors[m_dwNumMonitors];
            ZeroMemory( pMonitorInfoNew, sizeof(MonitorInfo) );
            lstrcpy( pMonitorInfoNew->strDeviceName, dispdev.DeviceString );
            lstrcpy( pMonitorInfoNew->strMonitorName, dispdev2.DeviceString );
            pMonitorInfoNew->iAdapter = NO_ADAPTER;
            
            if( dispdev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP )
            {
                EnumDisplaySettings( dispdev.DeviceName, ENUM_CURRENT_SETTINGS, &devmode );
                if( dispdev.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE )
                {
                    // For some reason devmode.dmPosition is not always (0, 0)
                    // for the primary display, so force it.
                    pMonitorInfoNew->rcScreen.left = 0;
                    pMonitorInfoNew->rcScreen.top = 0;
                }
                else
                {
                    pMonitorInfoNew->rcScreen.left = devmode.dmPosition.x;
                    pMonitorInfoNew->rcScreen.top = devmode.dmPosition.y;
                }
                pMonitorInfoNew->rcScreen.right = pMonitorInfoNew->rcScreen.left + devmode.dmPelsWidth;
                pMonitorInfoNew->rcScreen.bottom = pMonitorInfoNew->rcScreen.top + devmode.dmPelsHeight;
                pMonitorInfoNew->hMonitor = MonitorFromRect( &pMonitorInfoNew->rcScreen, MONITOR_DEFAULTTONULL );
            }
            m_dwNumMonitors++;
            if( m_dwNumMonitors == MAX_DISPLAYS )
                break;
        }
        iDevice++;
    }
}

LRESULT CALLBACK SSProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return Wrap->Proc_SaverDef( hWnd, uMsg, wParam, lParam );
}

HRESULT CSSWrapper::CreateSaverWindow()
{
	// Uncomment this code to allow stepping thru code in the preview case
	/*if( m_SaverMode == sm_preview )
	{
	WNDCLASS cls;
	cls.hCursor        = NULL; 
	cls.hIcon          = NULL; 
	cls.lpszMenuName   = NULL;
	cls.lpszClassName  = TEXT("Parent"); 
	cls.hbrBackground  = (HBRUSH) GetStockObject(WHITE_BRUSH);
	cls.hInstance      = m_hInstance; 
	cls.style          = CS_VREDRAW|CS_HREDRAW|CS_SAVEBITS|CS_DBLCLKS;
	cls.lpfnWndProc    = DefWindowProc;
	cls.cbWndExtra     = 0; 
	cls.cbClsExtra     = 0; 
	RegisterClass( &cls );

	// Create the window
	RECT rect;
	HWND hwnd;
	rect.left = rect.top = 40;
	rect.right = rect.left+200;
	rect.bottom = rect.top+200;
	AdjustWindowRect( &rect, WS_VISIBLE|WS_OVERLAPPED|WS_CAPTION|WS_POPUP, FALSE );
	hwnd = CreateWindow( TEXT("Parent"), TEXT("FakeShell"),
	WS_VISIBLE|WS_OVERLAPPED|WS_CAPTION|WS_POPUP, rect.left, rect.top,
	rect.right-rect.left, rect.bottom-rect.top, NULL,
	NULL, m_hInstance, NULL );
	m_hWndParent = hwnd;
	}*/

	// Register an appropriate window class
	WNDCLASS    cls;
	cls.hCursor        = LoadCursor( NULL, IDC_ARROW );
	cls.hIcon          = LoadIcon( m_hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON) ); 
	cls.lpszMenuName   = NULL;
	cls.lpszClassName  = TEXT("RealX 3D Saver");
	cls.hbrBackground  = (HBRUSH) GetStockObject(BLACK_BRUSH);
	cls.hInstance      = m_hInstance; 
	cls.style          = CS_VREDRAW|CS_HREDRAW;
	cls.lpfnWndProc    = SSProc;
	cls.cbWndExtra     = 0; 
	cls.cbClsExtra     = 0; 
	RegisterClass( &cls );

	// Create the window
	RECT rc;
	DWORD dwStyle;
	switch ( m_SaverMode )
	{		
	case sm_preview:

		/*WNDCLASS cls;
		cls.hCursor        = NULL; 
		cls.hIcon          = NULL; 
		cls.lpszMenuName   = NULL;
		cls.lpszClassName  = TEXT("Parent"); 
		cls.hbrBackground  = (HBRUSH) GetStockObject(WHITE_BRUSH);
		cls.hInstance      = m_hInstance; 
		cls.style          = CS_VREDRAW|CS_HREDRAW|CS_SAVEBITS|CS_DBLCLKS;
		cls.lpfnWndProc    = DefWindowProc;
		cls.cbWndExtra     = 0; 
		cls.cbClsExtra     = 0; 
		RegisterClass( &cls );

		// Create the window
		RECT rect;
		HWND hwnd;
		rect.left = rect.top = 40;
		rect.right = rect.left+200;
		rect.bottom = rect.top+200;
		AdjustWindowRect( &rect, WS_VISIBLE|WS_OVERLAPPED|WS_CAPTION|WS_POPUP, FALSE );
		hwnd = CreateWindow( TEXT("Parent"), TEXT("FakeShell"),
		WS_VISIBLE|WS_OVERLAPPED|WS_CAPTION|WS_POPUP, rect.left, rect.top,
		rect.right-rect.left, rect.bottom-rect.top, NULL,
		NULL, m_hInstance, NULL );
		m_hWndParent = hwnd;*/


		GetClientRect( m_hWndParent, &rc );			
		dwStyle = WS_VISIBLE | WS_CHILD;
		AdjustWindowRect( &rc, dwStyle, FALSE );

		hWnd = CreateWindow( TEXT("RealX 3D Saver"), "RealX 3D Saver", dwStyle, 
			rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, 
			m_hWndParent, NULL, m_hInstance, NULL );


		break;

	case sm_test:
		rc.left = rc.top = 50;
		rc.right = rc.left+600;
		rc.bottom = rc.top+400;
		dwStyle = WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
		AdjustWindowRect( &rc, dwStyle, FALSE );
		hWnd = CreateWindow( TEXT("RealX 3D Saver"), "RealX 3D Saver", dwStyle, 
			rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, 
			NULL, NULL, m_hInstance, NULL );

		break;

	case sm_full:
		// Create windows for each monitor.  Note that m_hWnd is NULL when CreateWindowEx
		// is called for the first monitor, so that window has no parent.  Windows for
		// additional monitors are created as children of the window for the first monitor.
		dwStyle = WS_VISIBLE | WS_POPUP;
		hWnd = NULL;

		rc.left = rc.top = 50;
		rc.right = rc.left+600;
		rc.bottom = rc.top+400;

		for( DWORD iMonitor = 0; iMonitor < m_dwNumMonitors; iMonitor++ )
		{
			MonitorInfo* pMonitorInfo;
			pMonitorInfo = &m_Monitors[iMonitor];
			if( pMonitorInfo->hMonitor == NULL )
				continue;                

			if (DebugMode)
			{				
				hWnd = CreateWindow( TEXT("RealX 3D Saver"), "RealX 3D Saver", dwStyle, 
					rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, 
					NULL, NULL, m_hInstance, NULL );
			}
			else
			{
				rc = pMonitorInfo->rcScreen;
				pMonitorInfo->hWnd = CreateWindowEx( WS_EX_TOPMOST, TEXT("RealX 3D Saver"), 
					"RealX 3D Saver", dwStyle, rc.left, rc.top, rc.right - rc.left, 
					rc.bottom - rc.top, hWnd, NULL, m_hInstance, NULL );
			}

			if( pMonitorInfo->hWnd == NULL )
				return E_FAIL;
			if( hWnd == NULL )
				hWnd = pMonitorInfo->hWnd;
		}
	}

	if ( hWnd == NULL )
	{
		return E_FAIL;
	}

	SetCursor( NULL );

	return S_OK;
}
//-----------------------------------------------------------------------------
// Name: ReadSettings()
// Desc: 
//-----------------------------------------------------------------------------
BOOL CSSWrapper::CheckReadedRegKey()
{
	if (Mode == svm_full)
	{
		return true;
	}

	char Key[256];	
	strcpy(Key,strKey);

	int  Codes[16];
	int  Codes2[16];
	int  Codes3[16];

	int NameLen = strlen(Name);
	
	//if (NameLen<=0||NameLen>80) return false;
	
	if (NameLen>25) NameLen=25;

	int RealNameLen=NameLen;
	if (NameLen>15) RealNameLen=15;

	int cur_index=0;
	for(int i=0;i<16;i++)
	{
		if (i==15) Codes[i]=NameLen+65;
		else
		Codes[i]=Name[cur_index];

		if (Codes[i]==32)
		{
			Codes[i]=0;
		}
		else
		if (48<=Codes[i]&&Codes[i]<=57)
		{
			Codes[i]=Codes[i]-48+1;
		}
		else
		if (65<=Codes[i]&&Codes[i]<=90)
		{
			Codes[i]=Codes[i]-65+11;
		}
		else
		if (97<=Codes[i]&&Codes[i]<=122)
		{
			Codes[i]=Codes[i]-97+37;
		}
		else
		{
			//return false;
		}
		cur_index++;
		if (cur_index>=RealNameLen) cur_index=0;
	}

	int KeyLen=strlen(Key);
	int RealKeyLen=KeyLen;
	if (KeyLen>16) RealKeyLen=16;

	cur_index=0;
	for(i=0;i<16;i++)
	{
		Codes2[i]=Key[cur_index];
		if (Codes2[i]==32)
		{
			Codes2[i]=0;
		}
		else
		if (48<=Codes2[i]&&Codes2[i]<=57)
		{
			Codes2[i]=Codes2[i]-48+1;
		}
		else
		if (65<=Codes2[i]&&Codes2[i]<=90)
		{
			Codes2[i]=Codes2[i]-65+11;
		}
		else
		if (97<=Codes2[i]&&Codes2[i]<=122)
		{
			Codes2[i]=Codes2[i]-97+37;
		}
		else
		{
			return false;
		}
		cur_index++;
		if (cur_index>=RealKeyLen) cur_index=0;
	}

	for (i=0;i<8;i++)
	{
		int tmp=Codes2[i];
		Codes2[i]=Codes2[8+i];
		Codes2[8+i]=tmp;
	}

	int RegKeyLen = strlen(RegKey);
	
	if (RegKeyLen!=16) return false;

	for(i=0;i<16;i++)
	{
		Codes3[i]=RegKey[i];
		if (Codes3[i]==45)
		{
			Codes3[i]=0;
		}
		else
		if (48<=Codes3[i]&&Codes3[i]<=57)
		{
			Codes3[i]=Codes3[i]-48+1;
		}
		else
		if (65<=Codes3[i]&&Codes3[i]<=90)
		{
			Codes3[i]=Codes3[i]-65+11;
		}
		else
		if (97<=Codes3[i]&&Codes3[i]<=122)
		{
			Codes3[i]=Codes3[i]-97+37;
		}
		else
		{
			return false;
		}
	}

	for (i=0;i<16;i++)
	{
		Codes3[i]-=Codes2[i];
		if (Codes3[i]<0) Codes3[i]+=63;
	}
	
	for (i=0;i<8;i++)
	{
		int tmp=Codes3[i];
		Codes3[i]=Codes3[8+i];
		Codes3[8+i]=tmp;
	}

	for (i=0;i<8;i++)
	{
		if (Codes3[i]!=Codes3[15-i]) return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
void CSSWrapper::ReadCFG()
{
	byte *data_ptr;                                           
	unsigned long   data_size;                                         
	ArchiveList_struct *List = NULL;
	MemoryFile      rarfile;   

	OpenRARResource(&rarfile, IDD_DATA);

	if(urarlib_get(&data_ptr, &data_size,
				   "Data\\cfg\\settings.txt", &rarfile, "")) 
	{
		char temppath[MAX_PATH];		
		char tempfile[MAX_PATH];

		/* get a temporary filename */
		GetTempPath(MAX_PATH,temppath);
		GetTempFileName(temppath,"saver_cfg",0,tempfile);

		HANDLE hfile;

		/* write BASS.DLL to the temporary file */
		if (INVALID_HANDLE_VALUE==(hfile=CreateFile(tempfile,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY,NULL)))
		{
			//printf("Error: Can't write BASS.DLL\n");
			//ExitProcess(0);
		}

		DWORD c;

		WriteFile(hfile,data_ptr,data_size,&c,NULL);
		CloseHandle(hfile);

		strcpy(strMainWebSite,ReadINI(tempfile, "RealX.com","settings", "main_web_site_url"));
		strcpy(strGetFullVersion,ReadINI(tempfile, "RealX.com", "settings", "get_full_ver_url"));		

		strcpy(strScreenRegName,ReadINI(tempfile, "RealX.com", "settings", "Reg_Path"));

		strcpy(strKey,ReadINI(tempfile, "RealX.com", "settings", "Key"));

		strcpy(strDigitalRiverRegName,ReadINI(tempfile, "RealX.com", "settings", "digital_River"));
		

		char buffer[64];

		strcpy(buffer,ReadINI(tempfile, "normal", "settings", "version"));

		if (stricmp(buffer,"normal")==0)
		{
			Mode = svm_normal;
		}
		else
		if (stricmp(buffer,"full")==0)
		{
			Mode = svm_full;
		}


		strcpy(buffer,ReadINI(tempfile, "false", "settings", "ShowFPS"));
		if (stricmp(buffer,"true")==0)	DefaultOptions.ShowFPS = true;		
		else DefaultOptions.ShowFPS = false;	
		
		strcpy(buffer,ReadINI(tempfile, "false", "settings", "ExitOnMouseMove"));
		if (stricmp(buffer,"true")==0)	DefaultOptions.ExitOnMouseMove = true;		
		else DefaultOptions.ExitOnMouseMove = false;	
		
		strcpy(buffer,ReadINI(tempfile, "100", "settings", "MusicVolume"));
		sscanf(buffer,"%i",&DefaultOptions.MusicVolume);

		strcpy(buffer,ReadINI(tempfile, "100", "settings", "SoundVolume"));
		sscanf(buffer,"%i",&DefaultOptions.SoundVolume);		

		strcpy(buffer,ReadINI(tempfile, "800", "settings", "Width"));
		sscanf(buffer,"%i",&DefaultOptions.Width);

		strcpy(buffer,ReadINI(tempfile, "600", "settings", "Height"));
		sscanf(buffer,"%i",&DefaultOptions.Height);

		strcpy(buffer,ReadINI(tempfile, "32", "settings", "BitCount"));
		sscanf(buffer,"%i",&DefaultOptions.BitCount);

		strcpy(buffer,ReadINI(tempfile, "0", "settings", "FSAA"));
		sscanf(buffer,"%i",&DefaultOptions.FSAA);

		strcpy(buffer,ReadINI(tempfile, "false", "settings", "UseDktpSet"));
		if (stricmp(buffer,"true")==0)	DefaultOptions.UseDktpSet = true;		
		else DefaultOptions.UseDktpSet = false;

		strcpy(buffer,ReadINI(tempfile, "0", "settings", "TimerType"));
		sscanf(buffer,"%i",&DefaultOptions.TimerType);		

		strcpy(buffer,ReadINI(tempfile, "100", "settings", "GammControl"));
		sscanf(buffer,"%i",&DefaultOptions.GammControl);


		

		if(data_ptr != NULL) free(data_ptr);

		DeleteFile(tempfile);
	}

	if(urarlib_get(&data_ptr, &data_size,
		"Data\\cfg\\text.txt", &rarfile, "")) 
	{
		char temppath[MAX_PATH];		
		char tempfile[MAX_PATH];

		/* get a temporary filename */
		GetTempPath(MAX_PATH,temppath);
		GetTempFileName(temppath,"saver_cfg",0,tempfile);

		HANDLE hfile;

		/* write BASS.DLL to the temporary file */
		if (INVALID_HANDLE_VALUE==(hfile=CreateFile(tempfile,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY,NULL)))
		{
			//printf("Error: Can't write BASS.DLL\n");
			//ExitProcess(0);
		}

		DWORD c;

		WriteFile(hfile,data_ptr,data_size,&c,NULL);
		CloseHandle(hfile);

		strcpy(strMes_StartWn,ReadINI(tempfile, "text","DIALOG", "welcome"));
		strcpy(strMes_NotCaption,ReadINI(tempfile, "text","DIALOG", "welcome_caption"));		

		strcpy(strMes_RegCaption,ReadINI(tempfile, "text","DIALOG", "RegCaption"));

		strcpy(strMes_RegName,ReadINI(tempfile, "text","DIALOG", "RegName"));
		strcpy(strMes_RegKey,ReadINI(tempfile, "text","DIALOG", "RegKey"));
		strcpy(strMes_RegOk,ReadINI(tempfile, "text","DIALOG", "RegOk"));
		strcpy(strMes_RegBuy,ReadINI(tempfile, "text","DIALOG", "RegBuy"));
		strcpy(strMes_RegCancel,ReadINI(tempfile, "text","DIALOG", "RegCancel"));		

		strcpy(strMes_RegValid,ReadINI(tempfile, "text","DIALOG", "RegValid"));
		strcpy(strMes_RegInvalid,ReadINI(tempfile, "text","DIALOG", "RegInvalide"));
		
		strcpy(strMes_AboutCaption,ReadINI(tempfile, "text","DIALOG", "AboutCaption"));
		strcpy(strMes_About,ReadINI(tempfile, "text","DIALOG", "AboutCredits"));
		strcpy(strMes_AboutButton,ReadINI(tempfile, "text","DIALOG", "AboutButton"));

		strcpy(str_OptionsCaption,ReadINI(tempfile, "text","DIALOG", "OptionsCaption"));
		
		strcpy(str_VideoModeSelection,ReadINI(tempfile, "text","DIALOG", "VideoModeSelection"));
		strcpy(str_Bits,ReadINI(tempfile, "text","DIALOG", "Bits"));		
		strcpy(str_UseDesktop,ReadINI(tempfile, "text","DIALOG", "UseDesktop"));

		strcpy(str_SoundControl,ReadINI(tempfile, "text","DIALOG", "SoundControl"));
		strcpy(str_MusicVolume,ReadINI(tempfile, "text","DIALOG", "MusicVolume"));
		strcpy(str_SoundVolume,ReadINI(tempfile, "text","DIALOG", "SoundVolume"));
		strcpy(str_EnableMusic,ReadINI(tempfile, "text","DIALOG", "EnableMusic"));
		strcpy(str_EnableSFX,ReadINI(tempfile, "text","DIALOG", "EnableSFX"));
		
		strcpy(str_Graphics,ReadINI(tempfile, "text","DIALOG", "Graphics"));
		strcpy(str_Gamma,ReadINI(tempfile, "text","DIALOG", "Gamma"));
		strcpy(str_FullscreenAA,ReadINI(tempfile, "text","DIALOG", "FullscreenAA"));
		strcpy(str_AASamples,ReadINI(tempfile, "text","DIALOG", "AASamples"));
		strcpy(str_AANone,ReadINI(tempfile, "text","DIALOG", "AANone"));		

		strcpy(str_MiscOptions,ReadINI(tempfile, "text","DIALOG", "MiscOptions"));
		strcpy(str_ShowFPS,ReadINI(tempfile, "text","DIALOG", "ShowFPS"));
		strcpy(str_ShowTime,ReadINI(tempfile, "text","DIALOG", "ShowTime"));		

		strcpy(str_clck_12h,ReadINI(tempfile, "text","DIALOG", "clck_12h"));
		strcpy(str_clck_24h,ReadINI(tempfile, "text","DIALOG", "clck_24h"));
		strcpy(str_clck_analog,ReadINI(tempfile, "text","DIALOG", "clck_analog"));
		
		strcpy(str_ExitMouseMove,ReadINI(tempfile, "text","DIALOG", "ExitMouseMove"));
		
		strcpy(str_Products,ReadINI(tempfile, "text","DIALOG", "Products"));
		strcpy(str_Visitsite,ReadINI(tempfile, "text","DIALOG", "Visitsite"));
		strcpy(str_Visit,ReadINI(tempfile, "text","DIALOG", "Visit"));
		strcpy(str_About,ReadINI(tempfile, "text","DIALOG", "About"));
		
		strcpy(str_Registration,ReadINI(tempfile, "text","DIALOG", "Registration"));
		strcpy(str_RegSt,ReadINI(tempfile, "text","DIALOG", "RegState"));
		strcpy(str_RegStPersL,ReadINI(tempfile, "text","DIALOG", "PersLicence"));		
		strcpy(str_UnregSt,ReadINI(tempfile, "text","DIALOG", "UnregState"));
		strcpy(str_Register,ReadINI(tempfile, "text","DIALOG", "Register"));
		strcpy(str_GetFull,ReadINI(tempfile, "text","DIALOG", "GetFull"));
		
		strcpy(str_RestoreDef,ReadINI(tempfile, "text","DIALOG", "RestoreDef"));
		strcpy(str_Ok,ReadINI(tempfile, "text","DIALOG", "Ok"));
		strcpy(str_Cancel,ReadINI(tempfile, "text","DIALOG", "Cancel"));		


		for (int i=0;i<(int)strlen(strMes_About);i++)
		{
			if (strMes_About[i]=='%')
			{
				strMes_About[i] = '\n';
			}
		}

		if(data_ptr != NULL) free(data_ptr);

		DeleteFile(tempfile);
	}

	CurrentOptions=DefaultOptions;
}
//-----------------------------------------------------------------------------
void CSSWrapper::Init( HINSTANCE hInstance )
{
	InitCommonControls();

	ReadCFG();

	HRESULT hr;

    //SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_IDLE );

    m_hInstance = hInstance;

    // Parse the command line and do the appropriate thing
    TCHAR* pstrCmdLine = GetCommandLine();
    m_SaverMode = ParseCommandLine( pstrCmdLine );

	if (DebugMode) m_SaverMode = sm_test;	

	WReporter.Enabled=EnableWReporter;

	if( m_SaverMode == sm_preview || 
        m_SaverMode == sm_test    || 
        m_SaverMode == sm_full )
	{
		WReporter.open("Saver_Log.txt");
	}
	if (m_SaverMode == sm_config)
	{
		WReporter.open("Config_Log.txt");
	}

	hr=Renderer.Init();

	if (hr==S_OK)
	{
		Init_Success=true;
	}
	else
	{
		Init_Success=false;
	}
	
	HWND hwnd = FindWindow (TEXT("RealX 3D Saver"), NULL);
	if (hwnd) Init_Success=false;

	hwnd = FindWindow (NULL, str_OptionsCaption);
	if (hwnd) Init_Success=false;

	if (Init_Success)
	{
		CheckDeviceCapabilitys();

		EnumMonitors();

		// Create the screen saver window(s)
		if( m_SaverMode == sm_preview || 
			m_SaverMode == sm_test    || 
			m_SaverMode == sm_full )
		{
			if( FAILED( hr = CreateSaverWindow() ) )
			{
				WReporter.writetext("Cannot create Saver Window!");
			}
			
			WReporter.writetext("Create Saver Window complete");
		}
		
		/*if( m_SaverMode == sm_preview )
		{
			// In preview mode, "pause" (enter a limited message loop) briefly 
			// before proceeding, so the display control panel knows to update itself.
			m_bWaitForInputIdle = TRUE;
			
			// Post a message to mark the end of the initial group of window messages
			PostMessage( hWnd, WM_USER, 0, 0 );
			
			MSG msg;
			while( m_bWaitForInputIdle )
			{
				// If GetMessage returns FALSE, it's quitting time.
				if( !GetMessage( &msg, hWnd, 0, 0 ) )
				{
					// Post the quit message to handle it later
					PostQuitMessage(0);
					break;
				}
				
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}*/
	}
}
//-----------------------------------------------
HRESULT CSSWrapper::DoSaver()
{
    HRESULT hr;

    // Figure out if we're on Win9x
    OSVERSIONINFO osvi; 
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    GetVersionEx( &osvi );
    m_bIs9x = (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS);

    // If we're in full on mode, and on 9x, then need to load the password DLL
    if ( m_SaverMode == sm_full && m_bIs9x )
    {
        // Only do this if the password is set - check registry:
        HKEY hKey; 
        if ( RegCreateKeyEx( HKEY_CURRENT_USER, REGSTR_PATH_SCREENSAVE, 0, NULL, 0, KEY_READ, NULL, &hKey, NULL ) == ERROR_SUCCESS ) 
        { 
            DWORD dwVal;
            DWORD dwSize = sizeof(dwVal); 
 
            if ( (RegQueryValueEx( hKey, REGSTR_VALUE_USESCRPASSWORD, NULL, NULL,
                                   (BYTE *)&dwVal, &dwSize ) == ERROR_SUCCESS) && dwVal ) 
            { 
                m_hPasswordDLL = LoadLibrary( TEXT("PASSWORD.CPL") );
                if ( m_hPasswordDLL )
                    m_VerifySaverPassword = (VERIFYPWDPROC)GetProcAddress( m_hPasswordDLL, "VerifyScreenSavePwd" );
                RegCloseKey( hKey );
            }
        }
    }

    // Flag as screensaver running if in full on mode
    //if ( m_SaverMode == sm_full )
    //{
      //  BOOL bUnused;
        //SystemParametersInfo( SPI_SCREENSAVERRUNNING, TRUE, &bUnused, 0 );
    //}

	if (m_SaverMode!=sm_preview)
	{
		hr=EE_Core_CreateD3Device(hWnd);

		if (hr==1)
		{
			TCHAR strMsg[512];

			strcpy(strMsg,"Cant get Desktop DisplayMode.");

			MessageBox( hWnd, strMsg, "Error!", MB_ICONERROR|MB_OK );

			return 0;
		}

		if (hr==2)
		{
			TCHAR strMsg[512];

			strcpy(strMsg,"Could not find any compatible Direct3D\n");
			strcat(strMsg,"devices.");

			MessageBox( hWnd, strMsg, "Error!", MB_ICONERROR|MB_OK );

			return 0;
		}

		EE_Core_LoadDefMedia();
		EE_Core_LoadData();		

		SceneManager.Init();


		byte *data_ptr;                                           
		unsigned long   data_size;                                         
		ArchiveList_struct *List = NULL;
		MemoryFile      rarfile;   
		
		OpenRARResource(&rarfile, IDD_DATA);

		if(urarlib_get(&data_ptr, &data_size,
					   "Data\\Scene.xml", &rarfile, "")) 
		{
			char temppath[MAX_PATH];		
			char tempfile[MAX_PATH];

			/* get a temporary filename */
			GetTempPath(MAX_PATH,temppath);
			GetTempFileName(temppath,"scene",0,tempfile);

			HANDLE hfile;

			/* write BASS.DLL to the temporary file */
			if (INVALID_HANDLE_VALUE==(hfile=CreateFile(tempfile,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY,NULL)))
			{
				//printf("Error: Can't write BASS.DLL\n");
				//ExitProcess(0);
			}

			DWORD c;

			WriteFile(hfile,data_ptr,data_size,&c,NULL);
			CloseHandle(hfile);

			if(data_ptr != NULL) free(data_ptr);

			SceneManager.Scene_Load(tempfile);

			DeleteFile(tempfile);
		}

		SceneManager.Begin();
		
		WReporter.writetext("All Data Was Loaded.");
	}

	SetTimer(hWnd,7,1000,NULL);

    // Message pump
    BOOL bGotMsg;
    MSG msg;
    msg.message = WM_NULL;

	Quit=false;

	LastTime=(FLOAT)timeGetTime();

    while ( msg.message != WM_QUIT && !Quit)
    {
        bGotMsg = PeekMessage( &msg, NULL, 0, 0, PM_REMOVE );
        if( bGotMsg )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
			if (m_SaverMode!=sm_preview) EE_Core_MainLoop();
        }
    }

	UpdateWindow(HWND_DESKTOP);

    return S_OK;
}
//-----------------------------------------------------------------------------
// Name: ChangePassword()
// Desc:
//-----------------------------------------------------------------------------
VOID CSSWrapper::ChangePassword()
{
    // Load the password change DLL
    HINSTANCE mpr = LoadLibrary( TEXT("MPR.DLL") );

    if ( mpr != NULL )
    {
        // Grab the password change function from it
        typedef DWORD (PASCAL *PWCHGPROC)( LPCSTR, HWND, DWORD, LPVOID );
        PWCHGPROC pwd = (PWCHGPROC)GetProcAddress( mpr, "PwdChangePasswordA" );

        // Do the password change
        if ( pwd != NULL )
            pwd( "SCRSAVE", m_hWndParent, 0, NULL );

        // Free the library
        FreeLibrary( mpr );
    }
}
//-----------------------------------------------
BOOL WINAPI SSConfigProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return Wrap->Proc_ConfigDef( hDlg, uMsg, wParam, lParam );
}
//-----------------------------------------------------------------------------
VOID CSSWrapper::InitConfigControls(HWND hDlg)
{
}
//-----------------------------------------------------------------------------
VOID CSSWrapper::InitDefConfigControls(HWND hDlg)
{
	HWND hDLG = GetDlgItem( hDlg, IDCB_SHOWFPS );
	Button_SetCheck( hDLG, CurrentOptions.ShowFPS);

	hDLG = GetDlgItem( hDlg, IDCB_SHOWTIME );
	Button_SetCheck( hDLG, CurrentOptions.ShowTime);	

	/*if (Mode == svm_full && !bWindowHasResized)
	{
		hDLG= GetDlgItem( hDlg, IDReg_Gr );
		ShowWindow(hDLG, false); 
		
		hDLG= GetDlgItem( hDlg, IDS_RSTATUS );
		ShowWindow(hDLG, false); 

		hDLG= GetDlgItem( hDlg, IDB_REGISTER );
		ShowWindow(hDLG, false); 

		hDLG= GetDlgItem( hDlg, IDB_HOW );
		ShowWindow(hDLG, false); 

		hDLG= GetDlgItem( hDlg, IDB_HOW );
		ShowWindow(hDLG, false);

		RECT rc;
		

		hDLG= GetDlgItem( hDlg, IDB_RESTOREDEF );		
		GetWindowRect( hDLG, &rc);
		MoveWindow( hDLG,15,340,rc.right-rc.left, rc.bottom-rc.top,true);

		hDLG= GetDlgItem( hDlg, IDSC_OK );		
		GetWindowRect( hDLG, &rc);
		MoveWindow( hDLG,321,340,rc.right-rc.left, rc.bottom-rc.top,true);

		hDLG= GetDlgItem( hDlg, IDSC_CANCEL );		
		GetWindowRect( hDLG, &rc);
		MoveWindow( hDLG,417,340,rc.right-rc.left, rc.bottom-rc.top,true);		
		
				
		GetWindowRect( hDlg, &rc);

		SetWindowPos( hDlg, HWND_NOTOPMOST,
		              0,0, rc.right-rc.left, rc.bottom-rc.top-40,SWP_NOMOVE);	

		bWindowHasResized = true;
	}*/

	hDLG = GetDlgItem( hDlg, IDR_24H );
	EnableWindow( hDLG, CurrentOptions.ShowTime );

	hDLG = GetDlgItem( hDlg, IDR_12H );
	EnableWindow( hDLG, CurrentOptions.ShowTime );

	hDLG = GetDlgItem( hDlg, IDCB_EXITONMOUSE );
	Button_SetCheck( hDLG, CurrentOptions.ExitOnMouseMove);

	hDLG = GetDlgItem( hDlg, IDTB_GCONTROL );
	SendMessage(hDLG, TBM_SETRANGE, 
			    (WPARAM) TRUE,             
                (LPARAM) MAKELONG(10, 200));
	SendMessage(hDLG, TBM_SETPOS, 
				(WPARAM) TRUE,             
                (LPARAM) CurrentOptions.GammControl);
	SendMessage(hDLG, TBM_SETPAGESIZE, 0, (LPARAM) 1);

	char GammaSt[100];
	sprintf(GammaSt,"%s %i",str_Gamma,CurrentOptions.GammControl);
	strcat(GammaSt,"%");
	
	SetDlgItemText(hDlg, IDS_STGC, GammaSt);
		
	hDLG = GetDlgItem( hDlg, IDCB_UDESKSET );
	Button_SetCheck( hDLG, CurrentOptions.UseDktpSet);

	hDLG = GetDlgItem( hDlg, IDC_VIDEOMODE );
	EnableWindow( hDLG, !CurrentOptions.UseDktpSet );

	hDLG = GetDlgItem( hDlg, IDTB_MUSICVOLUME );
	SendMessage(hDLG, TBM_SETRANGE, 
			    (WPARAM) TRUE,             
                (LPARAM) MAKELONG(1, 100));
	SendMessage(hDLG, TBM_SETPOS, 
				(WPARAM) TRUE,             
                (LPARAM) CurrentOptions.MusicVolume);
	SendMessage(hDLG, TBM_SETPAGESIZE, 0, (LPARAM) 10);	

	hDLG = GetDlgItem( hDlg, IDTB_SOUNDVOLUME );
	SendMessage(hDLG, TBM_SETRANGE, 
				(WPARAM) TRUE,             
				(LPARAM) MAKELONG(1, 100));
	SendMessage(hDLG, TBM_SETPOS, 
				(WPARAM) TRUE,             
				(LPARAM) CurrentOptions.SoundVolume);
	SendMessage(hDLG, TBM_SETPAGESIZE, 0, (LPARAM) 10);	

	ShowWindow(hDLG, UseSFX); 

	if (CheckReadedRegKey()) SceneManager.bIsSaverRegistered=true;

	hDLG = GetDlgItem( hDlg, IDB_REGISTER);
	EnableWindow( hDLG, !SceneManager.bIsSaverRegistered );
		
	hDLG = GetDlgItem( hDlg, IDB_HOW);
	EnableWindow( hDLG, !SceneManager.bIsSaverRegistered );

	char RegStatusStr[100];
	strcpy(RegStatusStr,"");
	
	if (!SceneManager.bIsSaverRegistered)
	{
		strcpy(RegStatusStr,str_UnregSt);
	}
	else
	{		
		if (strcmp(Name,"")==0)
		{
			strcpy(RegStatusStr,str_RegStPersL);
		}
		else
		{
			strcpy(RegStatusStr,str_RegSt);
			strcat(RegStatusStr," ");
			strcat(RegStatusStr,Name);
		}		
	}

	SetDlgItemText(hDlg, IDS_RSTATUS, RegStatusStr);	


	hDLG = GetDlgItem( hDlg, IDCB_CLOCKTYPE);
	EnableWindow( hDLG, CurrentOptions.ShowTime );	

	SendDlgItemMessage( hWnd, IDCB_CLOCKTYPE, CB_RESETCONTENT, 0, 0 );
	SendDlgItemMessage( hDlg, IDCB_CLOCKTYPE, CB_ADDSTRING, 0, (LPARAM)str_clck_12h );
	SendDlgItemMessage( hDlg, IDCB_CLOCKTYPE, CB_ADDSTRING, 0, (LPARAM)str_clck_24h );
	SendDlgItemMessage( hDlg, IDCB_CLOCKTYPE, CB_ADDSTRING, 0, (LPARAM)str_clck_analog );	

	SendDlgItemMessage( hWnd, IDCB_CLOCKTYPE, CB_SETCURSEL, 
						(WPARAM)CurrentOptions.TimerType, 0L );

	ReadDialogTexts();
}
//-----------------------------------------------------------------------------
void CSSWrapper::ReadDialogTexts()
{
	byte *data_ptr;                                           
	unsigned long   data_size;                                         
	ArchiveList_struct *List = NULL;
	MemoryFile      rarfile;   

	OpenRARResource(&rarfile, IDD_DATA);

	if(urarlib_get(&data_ptr, &data_size,
		"Data\\cfg\\Text.txt", &rarfile, "")) 
	{
		char temppath[MAX_PATH];		
		char tempfile[MAX_PATH];

		/* get a temporary filename */
		GetTempPath(MAX_PATH,temppath);
		GetTempFileName(temppath,"dialog_text",0,tempfile);

		HANDLE hfile;

		/* write BASS.DLL to the temporary file */
		if (INVALID_HANDLE_VALUE==(hfile=CreateFile(tempfile,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY,NULL)))
		{
			//printf("Error: Can't write BASS.DLL\n");
			//ExitProcess(0);
		}

		DWORD c;

		WriteFile(hfile,data_ptr,data_size,&c,NULL);
		CloseHandle(hfile);

		if(data_ptr != NULL) free(data_ptr);

		
		//
		//SceneManager.Scene_Load(tempfile);
		//
		
		
		DeleteFile(tempfile);
	}
}
//-----------------------------------------------------------------------------
VOID CSSWrapper::DoConfig()
{
	DialogBox( m_hInstance, MAKEINTRESOURCE(IDD_SSCONFIG), NULL, SSConfigProc );
}
//-----------------------------------------------------------------------------
LRESULT CALLBACK CSSWrapper::Proc_ConfigDef( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
		case WM_INITDIALOG:
		{		
			byte *data_ptr;                                           
			unsigned long   data_size;			
			MemoryFile      rarfile;		

			OpenRARResource(&rarfile, IDD_DATA);

			if(urarlib_get(&data_ptr, &data_size,
						   "Data/Bitmap/screen_settings.bmp", &rarfile, "")) 
			{
				hDialogSetting.LoadBMP("Bitmap/screen_settings.bmp",data_ptr);

				if(data_ptr != NULL) free(data_ptr);
			}

			if(urarlib_get(&data_ptr, &data_size,
						   "Data/Bitmap/Logo.bmp", &rarfile, "")) 
			{
				hLogo.LoadBMP("Data/Bitmap/Logo.bmp",data_ptr);

				if(data_ptr != NULL) free(data_ptr);
			}
			
	
			hWnd=hDlg;

			SetWindowText(hDlg, str_OptionsCaption);			
			
			SettingsDefRead( &CurrentOptions );
		
			SetDefSettings();
			SettingsRead();

			WReporter.writetext("Read settings from reg complete.");

			if (!SceneManager.bIsSaverRegistered)
			{
				::MessageBox(hDlg,strMes_StartWn,strMes_NotCaption,NULL);
			}

			Renderer.FindMode(CurrentOptions.Width,
							  CurrentOptions.Height,
							  CurrentOptions.BitCount,
						      CurrentOptions.FSAA);

			CurrentOptions.Width=Renderer.Cur_VMode.Width;
			CurrentOptions.Height=Renderer.Cur_VMode.Height;
		
			if (Renderer.Cur_VMode.Format==D3DFMT_A8R8G8B8||
				Renderer.Cur_VMode.Format==D3DFMT_X8R8G8B8) CurrentOptions.BitCount=32;
			else CurrentOptions.BitCount=16;
		
			CurrentOptions.FSAA=Renderer.Cur_VMode.MultiSampleType;
		
			UpdateComboBoxes();

			WReporter.writetext("Video mode controls init complete.");

			InitConfigControls(hDlg);
			WReporter.writetext("Dialog controls init complete.");
			InitDefConfigControls(hDlg);
			WReporter.writetext("Def Dialog controls init complete.");

			WReporter.writetext("Create Config Window complete.");


			SetDlgItemText(hDlg, IDVM_Gr, str_VideoModeSelection);
			SetDlgItemText(hDlg, IDCB_UDESKSET, str_UseDesktop);
				
			SetDlgItemText(hDlg, IDSND_Gr, str_SoundControl);
			SetDlgItemText(hDlg, IDS_MUSICVOLUME, str_MusicVolume);
			SetDlgItemText(hDlg, IDS_SOUNDVOLUME, str_SoundVolume);
			SetDlgItemText(hDlg, IDCB_MUSIC, str_EnableMusic);
			SetDlgItemText(hDlg, IDCB_SFX, str_EnableSFX);	

			SetDlgItemText(hDlg, IDGRPH_Gr, str_Graphics);
			//SetDlgItemText(hDlg, IDCB_MUSIC, str_Gamma);
			SetDlgItemText(hDlg, IDS_FSAA, str_FullscreenAA);	
	
			SetDlgItemText(hDlg, IDMisc_Gr, str_MiscOptions);
			SetDlgItemText(hDlg, IDCB_SHOWFPS, str_ShowFPS);
			SetDlgItemText(hDlg, IDCB_SHOWTIME, str_ShowTime);	
			SetDlgItemText(hDlg, IDCB_EXITONMOUSE, str_ExitMouseMove);	


			SetDlgItemText(hDlg, IDPr_Gr, str_Products);	
			SetDlgItemText(hDlg, IDST_Visit, str_Visitsite);
			SetDlgItemText(hDlg, IDB_VISIT, str_Visit);
			SetDlgItemText(hDlg, IDB_ABOUT, str_About);	

			
			SetDlgItemText(hDlg, IDReg_Gr, str_Registration);
			SetDlgItemText(hDlg, IDB_REGISTER, str_Register);
			SetDlgItemText(hDlg, IDB_GETFULL, str_GetFull);	

			SetDlgItemText(hDlg, IDB_RESTOREDEF, str_RestoreDef);
			SetDlgItemText(hDlg, IDSC_OK, str_Ok);
			SetDlgItemText(hDlg, IDSC_CANCEL, str_Cancel);	
		}
		break;
		case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;

			hdc=BeginPaint (hDlg,&ps);			

			WINDOWINFO info;
			GetWindowInfo(hDlg,&info);
			

			/*hDialogSetting.GDIPaint(hdc,
									0,0,
									info.rcClient.right-info.rcClient.left,
									info.rcClient.bottom-info.rcClient.top);*/			
			
			RECT rc;
			HWND hDLG= GetDlgItem( hDlg, IDD_LEFTCORNER );		
			GetWindowRect( hDLG, &rc);
								
			hDialogSetting.GDIPaint (hdc,
									 rc.left-info.rcClient.left,rc.top-info.rcClient.top,
									 rc.right-rc.left,rc.bottom-rc.top);			
			
			EndPaint (hDlg,&ps);
		}
		break;
		case WM_HSCROLL:
		{
			if( TB_THUMBTRACK == LOWORD(wParam) ||
				TB_PAGEDOWN == LOWORD(wParam) ||
				TB_PAGEUP == LOWORD(wParam)) 
			{
				HWND hDLG = GetDlgItem( hDlg, IDTB_MUSICVOLUME );
				int tb_pos = SendMessage(hDLG, TBM_GETPOS, 0, 0); 

				CurrentOptions.MusicVolume = tb_pos;				
			}
	
			if( TB_THUMBTRACK == LOWORD(wParam) ||
				TB_PAGEDOWN == LOWORD(wParam) ||
				TB_PAGEUP == LOWORD(wParam)) 
			{
				HWND hDLG = GetDlgItem( hDlg, IDTB_SOUNDVOLUME );
				int tb_pos = SendMessage(hDLG, TBM_GETPOS, 0, 0); 

				CurrentOptions.SoundVolume = tb_pos;				
			}

			if( TB_THUMBTRACK == LOWORD(wParam) ||
				TB_PAGEDOWN == LOWORD(wParam) ||
				TB_PAGEUP == LOWORD(wParam)||
				TB_THUMBPOSITION == LOWORD(wParam)||
				TB_ENDTRACK == LOWORD(wParam))
			{
				HWND hDLG = GetDlgItem( hDlg, IDTB_GCONTROL );
				int tb_pos = SendMessage(hDLG, TBM_GETPOS, 0, 0); 

				CurrentOptions.GammControl=tb_pos;

				char GammaSt[100];
				sprintf(GammaSt,"%s %i",str_Gamma,CurrentOptions.GammControl);
				strcat(GammaSt,"%");
	
				SetDlgItemText(hDlg, IDS_STGC, GammaSt);
			}
		}
		break;
		case WM_COMMAND:
		{
			if ( IDCB_SHOWFPS ==LOWORD(wParam))
			{
				CurrentOptions.ShowFPS=!CurrentOptions.ShowFPS;
				HWND hwndMode= GetDlgItem( hDlg, IDCB_SHOWFPS );
				Button_SetCheck( hwndMode, CurrentOptions.ShowFPS);
			}

			if ( IDCB_SHOWTIME ==LOWORD(wParam))
			{
				CurrentOptions.ShowTime=!CurrentOptions.ShowTime;
				HWND hDLG = GetDlgItem( hDlg, IDCB_SHOWTIME );
				Button_SetCheck( hDLG, CurrentOptions.ShowTime);

				hDLG = GetDlgItem( hDlg, IDCB_CLOCKTYPE);
				EnableWindow( hDLG, CurrentOptions.ShowTime );				
			}			

			if ( IDCB_EXITONMOUSE ==LOWORD(wParam))
			{
				CurrentOptions.ExitOnMouseMove=!CurrentOptions.ExitOnMouseMove;
				HWND hDLG = GetDlgItem( hDlg, IDCB_EXITONMOUSE );
				Button_SetCheck( hDLG, CurrentOptions.ExitOnMouseMove);
			}		

			if ( IDCB_UDESKSET ==LOWORD(wParam))
			{
				CurrentOptions.UseDktpSet=!CurrentOptions.UseDktpSet;
				HWND hDLG = GetDlgItem( hDlg, IDCB_UDESKSET );
				Button_SetCheck( hDLG, CurrentOptions.UseDktpSet);
				hDLG = GetDlgItem( hDlg, IDC_VIDEOMODE );
				EnableWindow( hDLG, !CurrentOptions.UseDktpSet );
			}

			if ( IDB_ABOUT ==LOWORD(wParam))
			{
				DoAbout();
			}

			if ( IDB_REGISTER ==LOWORD(wParam))
			{
				//DialogBox( (HINSTANCE)GetWindowLong(  hDlg, GWL_HINSTANCE ),
				//         MAKEINTRESOURCE(IDD_REGISTER),  hDlg,
                //       SSRegProc );

				DoReg();

				InitDefConfigControls(hDlg);
				InitConfigControls(hDlg);
			}
		
			if ( IDB_RESTOREDEF ==LOWORD(wParam))
			{
				SetDefSettings();
				CurrentOptions=DefaultOptions;

				Renderer.FindMode(CurrentOptions.Width,
								  CurrentOptions.Height,
							      CurrentOptions.BitCount,
								  CurrentOptions.FSAA);

				CurrentOptions.Width=Renderer.Cur_VMode.Width;
				CurrentOptions.Height=Renderer.Cur_VMode.Height;
		
				if (Renderer.Cur_VMode.Format==D3DFMT_A8R8G8B8||
					Renderer.Cur_VMode.Format==D3DFMT_X8R8G8B8) CurrentOptions.BitCount=32;
				else CurrentOptions.BitCount=16;
		
				CurrentOptions.FSAA=Renderer.Cur_VMode.MultiSampleType;

				UpdateComboBoxes();

				InitConfigControls(hDlg);
				InitDefConfigControls(hDlg);
			}

			if ( IDB_HOW ==LOWORD(wParam))
			{
				ShellExecute(hDlg,"open",URL,NULL,NULL,SW_SHOWNORMAL);
			}
		
			if ( IDB_VISIT ==LOWORD(wParam))
			{
				ShellExecute(hDlg,"open",strMainWebSite,NULL,NULL,SW_SHOWNORMAL);
			}

			if (IDB_GETFULL==LOWORD(wParam))
			{
				ShellExecute(hDlg,"open",URL,NULL,NULL,SW_SHOWNORMAL);
			}
		
			if ( IDTB_MUSICVOLUME == LOWORD(wParam))
			{
				if( TB_THUMBTRACK == HIWORD(wParam) ) 
				{
					HWND hDLG = GetDlgItem( hDlg, IDTB_MUSICVOLUME );
					CurrentOptions.MusicVolume = SendMessage(hDLG, TBM_GETPOS, 0, 0); 
				}
			}

			if ( IDTB_SOUNDVOLUME == LOWORD(wParam))
			{
				if( TB_THUMBTRACK == HIWORD(wParam) ) 
				{
					HWND hDLG = GetDlgItem( hDlg, IDTB_SOUNDVOLUME );
					CurrentOptions.SoundVolume = SendMessage(hDLG, TBM_GETPOS, 0, 0); 
				}
			}

			if( IDSC_OK == LOWORD(wParam) )
			{			
				SettingsDefWrite( &CurrentOptions );
				SettingsWrite();
				WReporter.writetext("Write settings to reg complete.");
				WReporter.close();
				EndDialog( hDlg, IDOK );
				
				return TRUE;
				//PostQuitMessage(0);
			}
			
			if( IDSC_CANCEL == LOWORD(wParam) )
			{
				WReporter.close();
				EndDialog( hDlg, IDCANCEL );
			
				return TRUE;
				//PostQuitMessage(0);
			}

			if( CBN_SELENDOK == HIWORD(wParam) )
			{
				DWORD dwIndex = SendMessage( (HWND)lParam, CB_GETCURSEL, 0, 0 );
				DWORD dwData  = SendMessage( (HWND)lParam, CB_GETITEMDATA, dwIndex, 0 );
            
				if( CB_ERR == dwIndex ) return TRUE;
            
				// Handle the case where one of these may have changed. The
				// combo boxes will need to be updated to reflect the changes.
				switch( LOWORD( wParam ) )
				{
					case IDC_VIDEOMODE:
					{
						D3DDeviceInfo *Device;
						Device=&Renderer.Adapters[Renderer.Cur_Adapter].devices;

						for( int i = 0; i< (int)Device->dwNumModes; i++ )
						{			
							D3DModeInfo *pMode = &Device->modes[i];

							int BitCount=16;
						
							if (pMode->Format==D3DFMT_A8R8G8B8||pMode->Format==D3DFMT_X8R8G8B8)
							{							
								BitCount=32;
							}
							else
							{							
								BitCount=16;
							}
								
							if (dwData==i)
							{						
								CurrentOptions.Width=pMode->Width;
								CurrentOptions.Height=pMode->Height;
								CurrentOptions.BitCount=BitCount;

								break;						
							}
						}
					}
					break;
					case IDC_FSAA:
					{
						const D3DMULTISAMPLE_TYPE msTypeArray[] = { 
							D3DMULTISAMPLE_NONE,
							D3DMULTISAMPLE_2_SAMPLES,
							D3DMULTISAMPLE_3_SAMPLES,
							D3DMULTISAMPLE_4_SAMPLES,
							D3DMULTISAMPLE_5_SAMPLES,
							D3DMULTISAMPLE_6_SAMPLES,
							D3DMULTISAMPLE_7_SAMPLES,
							D3DMULTISAMPLE_8_SAMPLES,
							D3DMULTISAMPLE_9_SAMPLES,
							D3DMULTISAMPLE_10_SAMPLES,
							D3DMULTISAMPLE_11_SAMPLES,
							D3DMULTISAMPLE_12_SAMPLES,
							D3DMULTISAMPLE_13_SAMPLES,
							D3DMULTISAMPLE_14_SAMPLES,
							D3DMULTISAMPLE_15_SAMPLES,
							D3DMULTISAMPLE_16_SAMPLES};

						D3DDeviceInfo *Device;
						Device=&Renderer.Adapters[Renderer.Cur_Adapter].devices;

						unsigned int j=-1;
						for (int i=0;i<16;i++)
						if (Device->bMultiSampleTypeConfirmed[i])
						{
							j++;
	
							if (dwData==j)
							{
								CurrentOptions.FSAA=msTypeArray[i];
								break;
							}
						}					
					}
					break;
					case IDCB_CLOCKTYPE:
					{
						CurrentOptions.TimerType = dwIndex;
					}
				}
			}			
        }
		break;
    }

    return Proc_Config(hDlg, uMsg, wParam, lParam );
}
//-----------------------------------------------------------------------------
LRESULT CALLBACK CSSWrapper::Proc_Config( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return FALSE;
}
//-----------------------------------------------------------------------------
VOID CSSWrapper::DoReg()
{	
	DialogBox( (HINSTANCE)GetWindowLong(  hWnd, GWL_HINSTANCE ),
			   MAKEINTRESOURCE(IDD_REGISTER), hWnd, SSRegProc );
}
//-----------------------------------------------
LRESULT CALLBACK CSSWrapper::Proc_Reg( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( WM_INITDIALOG == uMsg )
    {
		SetFocus(hDlg);

		SetWindowText(hDlg, strMes_RegCaption);
		
		SetDlgItemText(hDlg, IDS_RegName, strMes_RegName);
		SetDlgItemText(hDlg, IDS_RegCode, strMes_RegKey);
		SetDlgItemText(hDlg, IDR_OK, strMes_RegOk);
		SetDlgItemText(hDlg, IDR_HOW, strMes_RegBuy);		
		SetDlgItemText(hDlg, IDR_CANCEL, strMes_RegCancel);

        return TRUE;
    }
    
    if( WM_COMMAND == uMsg )
    {
        if( IDR_OK == LOWORD(wParam) )
        {
			int ret=CheckEnteredRegKey(hDlg);
			switch (ret)
			{
				case 0:
				{
					::MessageBox(hDlg,strMes_RegValid,strMes_RegCaption,NULL);
					SceneManager.bIsSaverRegistered=true;
				}
				break;
				case 1: ::MessageBox(hDlg,strMes_RegInvalid,strMes_RegCaption,NULL);
				break;
				case 2:	::MessageBox(hDlg,strMes_RegInvalid,strMes_RegCaption,NULL);
				break;
				case 3:	::MessageBox(hDlg,strMes_RegInvalid,strMes_RegCaption,NULL);
				break;
				case 4:	::MessageBox(hDlg,strMes_RegInvalid,strMes_RegCaption,NULL);
				break;
			}
		
			if (SceneManager.bIsSaverRegistered) 
			{
				EndDialog( hDlg, IDOK );

				return TRUE;
			}
        }		
        
		if( IDR_CANCEL == LOWORD(wParam) )
        {
            EndDialog( hDlg, IDCANCEL );
            return TRUE;
        }

		if ( IDR_HOW ==LOWORD(wParam))
		{			
			ShellExecute(hDlg,"open",URL,NULL,NULL,SW_SHOWNORMAL);

			return TRUE;
		}
    }

    return FALSE;
}
//-----------------------------------------------------------------------------
BOOL WINAPI SSAboutProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return Wrap->Proc_About( hDlg, uMsg, wParam, lParam );
}
//-----------------------------------------------
VOID CSSWrapper::DoAbout()
{
	DialogBox( m_hInstance, MAKEINTRESOURCE(IDD_ABOUT), hWnd, SSAboutProc );
}
//-----------------------------------------------
LRESULT CALLBACK CSSWrapper::Proc_About( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( WM_INITDIALOG == uMsg )
    {		
		SetDlgItemText(hDlg, IDS_About, strMes_About);
		SetWindowText(hDlg, strMes_AboutCaption);
		SetDlgItemText(hDlg, ID_ABOUTOK, strMes_AboutButton);		

        return TRUE;
    }
    
	if( uMsg == WM_PAINT)
	{
		HDC hdc;
		PAINTSTRUCT ps;

		hdc=BeginPaint (hDlg,&ps);
		hLogo.GDIPaint (hdc,302,31);

		EndPaint (hDlg,&ps);
		
		return 0;		
	}

    if( WM_COMMAND == uMsg )
    {
        if( ID_ABOUTOK == LOWORD(wParam) )
        {   
			EndDialog( hDlg, IDOK );
            return TRUE;
        }
    }

    return FALSE;
}
//-----------------------------------------------------------------------------
INT CSSWrapper::Run()
{
	if (!Init_Success) return 0;	

    // Parse the command line and do the appropriate thing
    switch ( m_SaverMode )
    {
        case sm_config:
        {			
            DoConfig();
        }
        break;
        case sm_preview:
        case sm_test:
        case sm_full:
        {
			SettingsDefRead( &CurrentOptions );
			SettingsRead();

	
			Renderer.FindMode(CurrentOptions.Width,
							  CurrentOptions.Height,
							  CurrentOptions.BitCount,
							  CurrentOptions.FSAA);

			CurrentOptions.Width=Renderer.Cur_VMode.Width;
			CurrentOptions.Height=Renderer.Cur_VMode.Height;
		
			if (Renderer.Cur_VMode.Format==D3DFMT_A8R8G8B8||
				Renderer.Cur_VMode.Format==D3DFMT_X8R8G8B8) CurrentOptions.BitCount=32;
			else CurrentOptions.BitCount=16;
		
			CurrentOptions.FSAA=Renderer.Cur_VMode.MultiSampleType;
		
			DoSaver();
			
			DestroyWindow(hWnd);
			//SetWindowPos(m_hWnd, HWND_NOTOPMOST, 
			//			 0,0,1,1,SWP_SHOWWINDOW );

			//DialogBox( NULL, MAKEINTRESOURCE(IDD_REGISTER), NULL, SSRegProc );
		
            /*if( FAILED( hr = DoSaver() ) )
                DisplayErrorMsg( hr, 0 );*/
        }
		break;
        
        case sm_passwordchange:
        {
            ChangePassword();            
        }
		break;
    }	
	
	if (NeedShowReg)
	{		
		if (!SceneManager.bIsSaverRegistered)
		{		
			hWnd = NULL;

			DoReg();

			SettingsDefWrite( &CurrentOptions );
		}
	}

    return 0;
}
//-----------------------------------------------
void CSSWrapper::CheckDeviceCapabilitys()
{
}
//-----------------------------------------------
HRESULT CSSWrapper::EE_Core_CreateD3Device(HWND hWnd)
{	
	if (DebugMode)
	{
		Renderer.Cur_VMode.bWindowed=true;
	}
	else
	{
		if (!CurrentOptions.UseDktpSet)
		{
			Renderer.FindMode(CurrentOptions.Width,
							  CurrentOptions.Height,
							  CurrentOptions.BitCount,
							  CurrentOptions.FSAA);
		}
		else
		{
			RECT m_rcWindowClient;
			GetClientRect( hWnd, &m_rcWindowClient );

			Renderer.FindMode(m_rcWindowClient.right  - m_rcWindowClient.left,
							  m_rcWindowClient.bottom - m_rcWindowClient.top,
							  32,
							  CurrentOptions.FSAA);
		}
	}

	Renderer.CreateD3DDevice(hWnd);

	
	TexManager.SetLoadFromResource(true);
	TexManager.Init(Renderer.Cur_VMode.Format,hWnd);	

	VBuffManager.Init(hWnd);	

	Renderer.GammaCtrl=CurrentOptions.GammControl;
	
	Renderer.SetGamma();	

	WReporter.writetext("Create D3D_Device Complete");
	EE_Core_InitD3Device();
	WReporter.writetext("Init D3D_Device Complete");

	EE_Core_DrawLoading();

	ShaderManager.SetLoadFromResource(true);
	ShaderManager.Init(hWnd);	

	EMDManager.SetLoadFromResource(true);
	EMDManager.Init(hWnd);

	ESAManager.SetLoadFromResource(true);
	ESAManager.Init(hWnd);

	SNDManager.SetLoadFromResource(true);
	SNDManager.Init(hWnd);

	SNDManager.SetMasterVolume(100);
	SNDManager.SetMusicVolume(CurrentOptions.MusicVolume);
	SNDManager.SetSFXVolume(CurrentOptions.MusicVolume);

	PostProcesing.Init(Renderer);

	return 0;
}
//-----------------------------------------------
HRESULT CSSWrapper::EE_Core_InitD3Device()
{
	Renderer.SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	Renderer.SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	Renderer.SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );

	Renderer.SetRenderState( D3DRS_FOGSTART,  FtoDW(0) );
	Renderer.SetRenderState( D3DRS_FOGEND,    FtoDW(2200) );

	Renderer.SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_NONE );
	Renderer.SetRenderState( D3DRS_FOGTABLEMODE,   D3DFOG_LINEAR );

	Renderer.SetRenderState( D3DRS_FOGCOLOR,  0xffa0a0a0);

	Renderer.SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	Renderer.SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	Renderer.SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	Renderer.SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	Renderer.SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );  
	Renderer.SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );  

	Renderer.SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	Renderer.SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	Renderer.SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

	Renderer.SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);	

	Renderer.SetRenderState( D3DRS_ALPHAREF,        0x30 );
	Renderer.SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

	Renderer.SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	Renderer.SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	Renderer.SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	Renderer.SetSamplerState( 0, D3DSAMP_ADDRESSU , D3DTADDRESS_WRAP);
	Renderer.SetSamplerState( 0, D3DSAMP_ADDRESSV , D3DTADDRESS_WRAP);

	Renderer.SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	Renderer.SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	Renderer.SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	Renderer.SetSamplerState( 1, D3DSAMP_ADDRESSU , D3DTADDRESS_WRAP);
	Renderer.SetSamplerState( 1, D3DSAMP_ADDRESSV , D3DTADDRESS_WRAP);


	Renderer.SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	Renderer.SetSamplerState( 2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	Renderer.SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	Renderer.SetSamplerState( 2, D3DSAMP_ADDRESSU , D3DTADDRESS_CLAMP);
	Renderer.SetSamplerState( 2, D3DSAMP_ADDRESSV , D3DTADDRESS_CLAMP);

	Renderer.SetRenderState( D3DRS_DITHERENABLE,   TRUE );
	Renderer.SetRenderState( D3DRS_SPECULARENABLE, FALSE );    
	Renderer.SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
	Renderer.SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

	Renderer.SetRenderState( D3DRS_DITHERENABLE, TRUE );
	Renderer.SetRenderState( D3DRS_ZENABLE,      TRUE );
	Renderer.SetRenderState( D3DRS_LIGHTING,     FALSE);


	/*D3DMATERIAL8 matl;
	ZeroMemory( &matl, sizeof(matl) );

	// Set the RGBA for diffuse reflection. 
	matl.Ambient.r = 0.65f; 
	matl.Ambient.g = 0.65f; 
	matl.Ambient.b = 0.65f; 
	matl.Ambient.a = 1.0f; 

	// Set the RGBA for ambient reflection. 
	matl.Diffuse.r = 0.95f; 
	matl.Diffuse.g = 0.95f; 
	matl.Diffuse.b = 0.95f; 	
	matl.Diffuse.a = 1.0f; 

	// Set the color and sharpness of specular highlights. 
	matl.Specular.r = 0.85f; 
	matl.Specular.g = 0.85f; 
	matl.Specular.b = 0.85f; 
	matl.Specular.a = 1.0f; 

	matl.Power = 75.0f;

	Renderer.pd3dDevice->SetMaterial( &matl);


	D3DLIGHT8 d3dLight;
	ZeroMemory( &d3dLight, sizeof(D3DLIGHT8) );
	d3dLight.Type        = D3DLIGHT_POINT;
	d3dLight.Diffuse.r   = 1.0f;
	d3dLight.Diffuse.g   = 1.0f;
	d3dLight.Diffuse.b   = 1.0f;
	d3dLight.Specular    = d3dLight.Diffuse;
	d3dLight.Position.x   = d3dLight.Direction.x = 0.0f;
	d3dLight.Position.y   = d3dLight.Direction.y = 100.0f;
	d3dLight.Position.z   = d3dLight.Direction.z = 0.0f;
	d3dLight.Attenuation0 = 1.0f;
	d3dLight.Range        = 500;


	ZeroMemory(&d3dLight, sizeof(D3DLIGHT8));

	// Set up for a white point light.
	d3dLight.Type = D3DLIGHT_DIRECTIONAL;

	float dk=1.0f;
	d3dLight.Ambient.r = 0.45f;//45f*dk;
	d3dLight.Ambient.g = 0.45f;//45f*dk;
	d3dLight.Ambient.b = 0.45f;//45f;
	d3dLight.Diffuse.r = 0.95f;//85f*dk;
	d3dLight.Diffuse.g = 0.95f;//85f*dk;
	d3dLight.Diffuse.b = 0.95f;//85f;    
	d3dLight.Specular.r = 0.9f;//91f*dk;
	d3dLight.Specular.g = 0.9f;//91f*dk;
	d3dLight.Specular.b = 0.9f;//91f;

	d3dLight.Position.x =261.4f;
	d3dLight.Position.y = 55.3f;
	d3dLight.Position.z =265.5f;

	d3dLight.Direction.x=0.5f;
	d3dLight.Direction.y=-0.75f;
	d3dLight.Direction.z=0.5f;

	// attenuate.
	d3dLight.Attenuation0 = 0.5f; 
	d3dLight.Attenuation1 = 0.01f; 
	d3dLight.Attenuation2 = 0.0001f; 
	d3dLight.Range = 250;

	Renderer.pd3dDevice->SetLight(0, &d3dLight);
	Renderer.LightEnable( 0, TRUE );

	d3dLight.Type = D3DLIGHT_DIRECTIONAL;

	Renderer.SetRenderState( D3DRS_LIGHTING, TRUE );*/

	D3DMATRIX mat2;
	mat2._11 = mat2._22 = mat2._33 = mat2._44 = 1.0f;
	mat2._12 = mat2._13 = mat2._14 = mat2._41 = 0.0f;
	mat2._21 = mat2._23 = mat2._24 = mat2._42 = 0.0f;
	mat2._31 = mat2._32 = mat2._34 = mat2._43 = 0.0f;

	Renderer.SetWorld( &mat2 );

	Renderer.SetProjection( 45, 0.05f, 300.0f);

	Renderer.SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	Renderer.SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	Renderer.SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	Renderer.SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	Renderer.SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	Renderer.SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	Renderer.SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	Renderer.SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );


	Renderer.SetRenderState( D3DRS_ZENABLE, FALSE );
	Renderer.SetRenderState( D3DRS_LIGHTING, FALSE );

	return S_OK;
}
//-----------------------------------------------
HRESULT CSSWrapper::EE_Core_DrawLoading()
{
	WReporter.writetext("Load Not_table_alpha texture complete");
	
	VBuffManager.CreateVertexBuff(HUDMesh,ETL_DESC,4);
	
	VOID* pVertices=VBuffManager.LockVertexBuff(HUDMesh);
		
	E_TLVERTEX  *Vertex = ( E_TLVERTEX* )pVertices;

	Vertex[0] = E_TLVERTEX( E_VECTOR(0,0,0.99f), 0.5f, 0, 1,0xffffffff );
	Vertex[1] = E_TLVERTEX( E_VECTOR(0,0,0.99f), 0.5f, 0, 0,0xffffffff );
	Vertex[2] = E_TLVERTEX( E_VECTOR(0,0,0.99f), 0.5f, 1, 1,0xffffffff );
	Vertex[3] = E_TLVERTEX( E_VECTOR(0,0,0.99f), 0.5f, 1, 0,0xffffffff );

	VBuffManager.UnlockVertexBuff(HUDMesh);

	TTextureReference LoadingTex;
	TTextureReference Corner_ld;
	TTextureReference Corner_lu;
	TTextureReference Corner_rd;
	TTextureReference Corner_ru;	

	LoadingTex=TexManager.LoadTextureFromFile("Loader.jpg","Data\\Notification\\",true);

	Corner_ld=TexManager.LoadTextureFromFile("loader_ld.tga","Data\\Notification\\",true);
	Corner_lu=TexManager.LoadTextureFromFile("loader_lu.tga","Data\\Notification\\",true);
	Corner_rd=TexManager.LoadTextureFromFile("loader_rd.tga","Data\\Notification\\",true);
	Corner_ru=TexManager.LoadTextureFromFile("loader_ru.tga","Data\\Notification\\",true);

	WReporter.writetext("Load Loading Texture complete.");

	for (int i=0;i<2;i++)
	{
		Renderer.Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0 );
		
		Renderer.BeginScene();
				
		float scale = 4.0f / 3.0f * Renderer.Cur_VMode.Height / Renderer.Cur_VMode.Width;

		Renderer.SetRenderState( D3DRS_ZENABLE,      FALSE );
		VBuffManager.SetHUDMesh(HUDMesh,(1 - scale) * 0.5f,0,scale,1,
								D3DCOLOR_ARGB(255,255,255,255),0,0,1,1);
		
		VBuffManager.SetStreamSource( 0, HUDMesh);
		TexManager.SetTexture( 0, LoadingTex );
		Renderer.DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
		

		Renderer.SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
		Renderer.SetRenderState( D3DRS_ALPHAREF, 1 );

		Renderer.SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		Renderer.SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		/*TexManager.SetTexture(0,Corner_lu);

		VBuffManager.SetHUDMesh(HUDMesh,
								0.0f,0.0f,
								256.0f/1024.0f,128.0f/768.0f,
								D3DCOLOR_ARGB(255,255,255,255),0,0,1,1);

		Renderer.DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );*/

		TexManager.SetTexture(0,Corner_ld);

		VBuffManager.SetHUDMesh(HUDMesh,
							    0.0f,1.0f-128.0f/768.0f,
								scale,128.0f/768.0f,
								D3DCOLOR_ARGB(255,255,255,255),0,0,1,1);

		Renderer.DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

		/*TexManager.SetTexture(0,Corner_ru);

		VBuffManager.SetHUDMesh(HUDMesh,
								1.0f-256.0f/1024.0f,0.0f,
								256.0f/1024.0f,128.0f/768.0f,
								D3DCOLOR_ARGB(255,255,255,255),0,0,1,1);

		Renderer.DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

		TexManager.SetTexture(0,Corner_rd);

		VBuffManager.SetHUDMesh(HUDMesh,
								1.0f-256.0f/1024.0f,1.0f-128.0f/768.0f,
								256.0f/1024.0f,128.0f/768.0f,
								D3DCOLOR_ARGB(255,255,255,255),0,0,1,1);*/

		Renderer.DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
		Renderer.SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		Renderer.SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

		Renderer.SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );

		Renderer.SetRenderState( D3DRS_ZENABLE,      TRUE );
		
		Renderer.EndScene();
		
		Swap=1-Swap;
		
		Renderer.Present( NULL, NULL, NULL, NULL );
	}

	WReporter.writetext("Draw Loading Screen complete.");	

	WReporter.writetext("Unload Loading Data complete.");
	WReporter.writetext("");

	return S_OK;
}
//-----------------------------------------------
HRESULT CSSWrapper::EE_Core_LoadDefMedia()
{	
	DigitsTex=TexManager.LoadTextureFromFile("digits.tga","Data\\Digits\\",true);			

	WReporter.writetext("Load Digit textures complete");

	NagTex=TexManager.LoadTextureFromFile("nag.tga","Data\\Notification\\",true);	
	UnregNagTex=TexManager.LoadTextureFromFile("unregistered.tga","Data\\Notification\\",true);	

	dclk_Table=TexManager.LoadTextureFromFile("Table.tga","Data\\Clock\\",true);	
	dclk_HourArrow=TexManager.LoadTextureFromFile("HourArrow.tga","Data\\Clock\\",true);	
	dclk_MinuteArrow=TexManager.LoadTextureFromFile("MinuteArrow.tga","Data\\Clock\\",true);	
	dclk_SecondArrow=TexManager.LoadTextureFromFile("SecondArrow.tga","Data\\Clock\\",true);	

	WReporter.writetext("Load Not_table texture complete");
	
	return S_OK;
}
//-----------------------------------------------
HRESULT CSSWrapper::EE_Core_LoadData()
{	
	return S_OK;
}
//-----------------------------------------------
HRESULT CSSWrapper::EE_Core_MainLoop()
{
	if (Renderer.pd3dDevice==NULL) return E_FAIL;

	float timedelta=(timeGetTime()-LastTime);
	LastTime=(FLOAT)timeGetTime();

	if (timedelta<0) 
	{
		timedelta=0;
	}

	if (!SceneManager.bIsSaverRegistered) 
	{
		TimeToShowNot+=timedelta;
		if (TimeToShowNot>1000*(SceneManager.SecToShowNot+4.0f)) TimeToShowNot=1000*(SceneManager.SecToShowNot);
	}

	// Test the cooperative level to see if it's okay to render
	HRESULT hr;

	if( FAILED( hr = Renderer.TestCooperativeLevel() ) )
    {		
        // If the device was lost, do not render until we get it back
        if( D3DERR_DEVICELOST == hr )
            return hr;

        // Check if the device needs to be resized.
        if( D3DERR_DEVICENOTRESET == hr )
        {
			WReporter.writetext("Device was lost.");
			WReporter.writetext("Try to restore.");

			//pd3dDevice->Reset(&d3dpp);
			
			//EE_Core_InitD3Device(pd3dDevice);

			Swap=1;

			WReporter.writetext("Restore state to complete.");
			WReporter.writetext("");

			LastTime=(FLOAT)timeGetTime();
        }
        return hr;
    }
	else
	{
		TexManager.Update(timedelta*0.001f);
		SceneManager.UpdateScene(timedelta*0.001f);
		
		EE_Core_AdvanceScene(timedelta*0.001f);
		EE_Core_Render(timedelta*0.001f);
		Swap=1-Swap;
		fps++;
	}	
	
	return S_OK;
}
//-----------------------------------------------
HRESULT CSSWrapper::EE_Core_AdvanceScene(float timedelta)
{
	{
		D3DVECTOR vFrom;
		D3DVECTOR vAt;

		vFrom.x=xc;
		vFrom.y=yc;
		vFrom.z=zc;


		vAt.x=xc+(FLOAT)cos(alpha*Radian)*cos(betta*Radian);
		vAt.y=yc+(FLOAT)sin(betta*Radian);
		vAt.z=zc+(FLOAT)sin(alpha*Radian)*cos(betta*Radian);	

		D3DVECTOR vWorldUp;
		vWorldUp.x=0.0f;
		vWorldUp.y=1.0f;
		vWorldUp.z=0.0f;

		D3DXMATRIX mat;

		SetViewMatrix( mat, vFrom, vAt, vWorldUp);
		Renderer.SetView( &mat );
	}	

	return S_OK;
}
//-----------------------------------------------
HRESULT CSSWrapper::EE_Core_Render(float timedelta)
{	
	Renderer.Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
       
    Renderer.BeginScene();
    
	Renderer.SetRenderState( D3DRS_LIGHTING, false );
	Renderer.SetRenderState( D3DRS_ALPHABLENDENABLE,   false );	
	Renderer.SetRenderState( D3DRS_ZENABLE,   true );	

	Renderer.SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	Renderer.SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	Renderer.SetRenderState( D3DRS_ALPHAREF, 5 );

	SceneManager.RenderScene(timedelta);
	
	EE_Core_RenderDef();

	Renderer.EndScene();

	Renderer.Present( NULL, NULL, NULL, NULL );

	return S_OK;
}
//-----------------------------------------------
HRESULT CSSWrapper::EE_Core_RenderDef()
{
	Renderer.SetRenderState( D3DRS_ZENABLE,      FALSE );
	Renderer.SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	Renderer.SetTexture( 0, NULL );

	Renderer.SetRenderState( D3DRS_ALPHAREF, 1 );

	VBuffManager.SetStreamSource( 0, HUDMesh );

	char tmpbuf[128];
	_strtime( tmpbuf );

	int index;

	float digit_x=clock_x;
	float digit_y=clock_y;

	Renderer.SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
	Renderer.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	/*if (CurrentOptions.ShowTime)
	{
		if (CurrentOptions.TimerType == 1)
		{
			TexManager.SetTexture(0,DigitsTex);	

			for(int i=0;i<8;i++)
			{
				index=tmpbuf[i]-48;				

				VBuffManager.SetHUDMesh(HUDMesh,
										digit_x+i*digit_dx*digit_density,digit_y,
										digit_dx,digit_dy,
										D3DCOLOR_ARGB((byte)255,dig_R,dig_G,dig_B),1.0f/15.0f*index,0,1.0f/15.0f*(index+1),1);

				Renderer.DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
			}
		}
		else
		if (CurrentOptions.TimerType == 0)
		{
			TexManager.SetTexture(0,DigitsTex);	

			digit_x-=digit_dx*2*digit_density;

			int h_index1=tmpbuf[0]-48;
			int h_index2=tmpbuf[1]-48;

			bool am=true;
			int hh=h_index1*10+h_index2;

			if (hh>=12)
			{
				am=false;

				if (hh!=12) hh-=12;

				if (hh<10)
				{
					h_index1=0;
					h_index2=hh;

				}
				else
				{
					h_index1=1;
					h_index2=hh-10;
				}
			}
			else
				if (hh==0)
				{
					h_index1=1;
					h_index2=2;
				}


				for(int i=0;i<8;i++)
				{
					index=tmpbuf[i]-48;

					if (i==0) index=h_index1;
					if (i==1) index=h_index2;

					if (i!=0||index!=0)
					{
						VBuffManager.SetHUDMesh(HUDMesh,
							digit_x+i*digit_dx*digit_density,digit_y,
							digit_dx,digit_dy,
							D3DCOLOR_ARGB((byte)255,dig_R,dig_G,dig_B),1.0f/15.0f*index,0,1.0f/15.0f*(index+1),1);

						Renderer.DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
					}				
				}

				if (am)
				{
					h_index1=12;
					h_index2=13;
				}
				else
				{
					h_index1=14;
					h_index2=13;
				}

				for(i=0;i<2;i++)
				{
					if (i==0) index=h_index1;
					if (i==1) index=h_index2;

					VBuffManager.SetHUDMesh(HUDMesh,
						digit_x+(i+8)*digit_dx*digit_density,digit_y,
						digit_dx,digit_dy,
						D3DCOLOR_ARGB((byte)255,dig_R,dig_G,dig_B),1.0f/15.0f*index,0,1.0f/15.0f*(index+1),1);

					Renderer.DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
				}
		}
		else
		{
			float clock_wd=0.25f * 0.75f;
			float clock_hgt=0.25f;

			float clock_pos_x= 1.0f - clock_wd;
			float clock_pos_y= 1.0f - clock_hgt;

			{
				TexManager.SetTexture(0,dclk_Table);			

				VBuffManager.SetHUDMesh(HUDMesh,
					clock_pos_x,clock_pos_y,
					clock_wd,clock_hgt,
					D3DCOLOR_ARGB((byte)255,dig_R,dig_G,dig_B),0,0,1,1);

				Renderer.DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

				char tmpbuf[16];
				_strtime( tmpbuf );

				char tmp[3];

				tmp[0]=tmpbuf[0];
				tmp[1]=tmpbuf[1];
				tmp[2]=0;

				float hh= (float)atof(tmp);

				if (hh>12)
				{
					hh-=12;
				}

				tmp[0]=tmpbuf[3];
				tmp[1]=tmpbuf[4];

				float mm= (float)atof(tmp);

				tmp[0]=tmpbuf[6];
				tmp[1]=tmpbuf[7];

				float ss= (float)atof(tmp);

				float arrow_pos_x = clock_pos_x + clock_wd * 0.5f;
				float arrow_pos_y = clock_pos_y + clock_hgt * 0.5f;
				{		
					TexManager.SetTexture(0,dclk_HourArrow);

					VBuffManager.SetRotatedHUDMesh(HUDMesh,
						(hh + mm/60.0f)/12.0f * 360.0f,
						arrow_pos_x,arrow_pos_y,
						clock_wd,clock_hgt,
						D3DCOLOR_ARGB((byte)255,dig_R,dig_G,dig_B),0,0,1,1);

					Renderer.DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
				}

				{		
					TexManager.SetTexture(0,dclk_MinuteArrow);

					VBuffManager.SetRotatedHUDMesh(HUDMesh,
						(mm + ss/60.0f)/60.0f * 360.0f,
						arrow_pos_x,arrow_pos_y,
						clock_wd,clock_hgt,
						D3DCOLOR_ARGB((byte)255,dig_R,dig_G,dig_B),0,0,1,1);

					Renderer.DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
				}

				{		
					TexManager.SetTexture(0,dclk_SecondArrow);

					VBuffManager.SetRotatedHUDMesh(HUDMesh,
						ss/60.0f * 360.0f,
						arrow_pos_x,arrow_pos_y,
						clock_wd,clock_hgt,
						D3DCOLOR_ARGB((byte)255,dig_R,dig_G,dig_B),0,0,1,1);

					Renderer.DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
				}
			}
		}
	}*/

	if (CurrentOptions.ShowFPS)
	{
		TCHAR fps_str[80];
		strcpy(fps_str,"");
		sprintf( fps_str, "%i",CurFps);

		digit_x=fps_x;
		digit_y=fps_y;

		TexManager.SetTexture(0,DigitsTex);

		for (unsigned int i=0;i<strlen(fps_str);i++)
		{
			index=fps_str[i]-48;
			if (index<0) index=11;			

			VBuffManager.SetHUDMesh(HUDMesh,
				digit_x+i*digit_dx*digit_density,digit_y,
				digit_dx,digit_dy,
				D3DCOLOR_ARGB((byte)255,dig_R,dig_G,dig_B),1.0f/15.0f*index,0,1.0f/15.0f*(index+1),1);

			Renderer.DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
		}
	}

	if ((TimeToShowNot<1000*SceneManager.SecToShowNot)&&!SceneManager.bIsSaverRegistered)
	{
		TCHAR fps_str[80];
		strcpy(fps_str,"");
		float tmp=SceneManager.SecToShowNot;
		float draw=tmp-TimeToShowNot/1000.0f;
		sprintf( fps_str, "%1.0f",draw);

		float digit_x=counter_x-(strlen(fps_str))*digit_dx*digit_density;
		float digit_y=counter_y;

		TexManager.SetTexture(0,DigitsTex);

		for (unsigned int i=0;i<strlen(fps_str);i++)
		{
			index=fps_str[i]-48;
			if (index<0) index=11;			

			VBuffManager.SetHUDMesh(HUDMesh,
									digit_x+i*digit_dx*digit_density,digit_y,
									digit_dx,digit_dy,
									D3DCOLOR_ARGB((byte)255,dig_R,dig_G,dig_B),1.0f/15.0f*index,0,1.0f/15.0f*(index+1),1);

			Renderer.DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
		}
	}	

	Renderer.SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	Renderer.SetRenderState( D3DRS_DESTBLEND,D3DBLEND_ONE );	

	Renderer.SetRenderState( D3DRS_ZENABLE,      TRUE );
	Renderer.SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );


	return true;
}
//-----------------------------------------------
void CSSWrapper::UpdateComboBoxes()
{    
	D3DAdapterInfo *Adapter;

	
	Adapter=&Renderer.Adapters[Renderer.Cur_Adapter];
   
	D3DDeviceInfo *Device;

	Device=&Adapter->devices;

	SendDlgItemMessage( hWnd, IDC_FSAA, CB_RESETCONTENT, 0, 0 );

	const D3DMULTISAMPLE_TYPE msTypeArray[] = { 
        D3DMULTISAMPLE_NONE,
        D3DMULTISAMPLE_2_SAMPLES,
        D3DMULTISAMPLE_3_SAMPLES,
        D3DMULTISAMPLE_4_SAMPLES,
        D3DMULTISAMPLE_5_SAMPLES,
        D3DMULTISAMPLE_6_SAMPLES,
        D3DMULTISAMPLE_7_SAMPLES,
        D3DMULTISAMPLE_8_SAMPLES,
        D3DMULTISAMPLE_9_SAMPLES,
        D3DMULTISAMPLE_10_SAMPLES,
        D3DMULTISAMPLE_11_SAMPLES,
        D3DMULTISAMPLE_12_SAMPLES,
        D3DMULTISAMPLE_13_SAMPLES,
        D3DMULTISAMPLE_14_SAMPLES,
        D3DMULTISAMPLE_15_SAMPLES,
        D3DMULTISAMPLE_16_SAMPLES,
    };

	int j=-1;
	for (DWORD i=0;i<16;i++)
	if (Device->bMultiSampleTypeConfirmed[i])
	{
		j++;

		CHAR strMultiSample[80];		

		if (i==0)
		{
			strcpy(strMultiSample,str_AANone);
		}
		else
		{
            sprintf(strMultiSample,"%i %s",i+1,str_AASamples);
		}
                
        DWORD dwItem = SendDlgItemMessage( hWnd, IDC_FSAA, 
                                           CB_ADDSTRING, 0,
                                           (LPARAM)strMultiSample );

		SendDlgItemMessage( hWnd, IDC_FSAA, CB_SETITEMDATA, 
                            (WPARAM)dwItem, (LPARAM)j );

		if (CurrentOptions.FSAA==msTypeArray[i])
		{
			SendDlgItemMessage( hWnd, IDC_FSAA, CB_SETCURSEL, (WPARAM)dwItem, 0L );
		}
	}

	SendDlgItemMessage( hWnd, IDC_VIDEOMODE, CB_RESETCONTENT, 0, 0 );

    for( i = 0; i< Device->dwNumModes; i++ )
    {
		CHAR            strMode[80];
        
		D3DModeInfo *pMode = &Device->modes[i];
                
        sprintf( strMode, "%ld x %ld ",
                          pMode->Width, pMode->Height);

		int BitCount=16;

		if (pMode->Format==D3DFMT_A8R8G8B8||pMode->Format==D3DFMT_X8R8G8B8)
		{
			strcat(strMode,"32 ");
			strcat(strMode,str_Bits);
			BitCount=32;
		}
		else
		{
			strcat(strMode,"16 ");
			strcat(strMode,str_Bits);
			BitCount=16;
		}
                
        DWORD dwItem = SendDlgItemMessage( hWnd, IDC_VIDEOMODE, 
                                                 CB_ADDSTRING, 0,
                                                 (LPARAM)strMode );

        SendDlgItemMessage( hWnd, IDC_VIDEOMODE, CB_SETITEMDATA, 
                                    (WPARAM)dwItem, (LPARAM)i );

        if( CurrentOptions.Width==pMode->Width &&
			CurrentOptions.Height==pMode->Height &&
			CurrentOptions.BitCount==BitCount)
		{
			SendDlgItemMessage( hWnd, IDC_VIDEOMODE, CB_SETCURSEL, 
                                (WPARAM)dwItem, 0L );
		}
    }

	//Fill_CurVMode();
}
//-----------------------------------------------
HRESULT CSSWrapper::EE_Core_UnloadDefD3DData()
{
	TexManager.UnloadTextures();

	return S_OK;
}
//-----------------------------------------------
HRESULT CSSWrapper::EE_Core_UnloadAllD3DData()
{
	return S_OK;
}
//-----------------------------------------------
HRESULT CSSWrapper::EE_Core_DestroyD3D()
{
	Renderer.DestroyD3D();
	SNDManager.Destroy();

	return S_OK;
}

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
	srand(time(NULL));

	CSSWrapper SSaver;
	SSaver.Init(hInst);
	SSaver.Run();		

	return 0;
}
