
#include "SSaver.h"

#define COMPILE_MULTIMON_STUBS
#include <multimon.h>

#include <tchar.h>
#include <commctrl.h>
#include <regstr.h>

#include "resource.h"

char Name[256]="";
char RegKey[16]="";

const char* SSaver::windowName = "ScrSaver ENgineX";

SSaver::ScreenSaverDefOptions DefaultOptions =
{
	true,
	true,
	100,	
	800,
	600,
	32,
	true,	
	2,
	100
};	

SSaver *saver;

BOOL WINAPI SSConfigProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return saver->Proc_Config( hDlg, uMsg, wParam, lParam );
}

BOOL WINAPI SSRegProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return saver->Proc_Reg( hDlg, uMsg, wParam, lParam );
}

LRESULT CALLBACK SSProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return saver->Proc_Saver( hWnd, uMsg, wParam, lParam );
}

SSaver::SSaver()
{
	CurrentOptions=DefaultOptions;	

	NeedShowReg=false;

	DebugMode = false;

	saver=this;
	dwFakeMouseMoveMsgAllowed=65;
	
	m_SaverMode = sm_full;

	TimeToShowNot=0;		

	UseSFX=true;

	hWnd = 0;

	String::Copy(strKey,255,"keykeykeykeykeykeykeykeykey");

	String::Copy(strMainWebSite,511,"realx");
	String::Copy(strGetFullVersion,511,"realx");

	String::Copy(URL,511,"");	

	String::Copy(strMes_StartWn,255,"realx");	
	String::Copy(strMes_NotCaption,255,"realx");
	
	String::Copy(str_RegSt,255,"realx");
	String::Copy(str_RegStPersL,255,"realx");	
	String::Copy(str_UnregSt,255,"realx");	

	String::Copy(strMes_RegValid,256,"realx");
	String::Copy(strMes_RegInvalid,256,"realx");

	String::Copy(strScreenRegName,511,"Software\\RealX\\3D Saver");
	String::Copy(strDigitalRiverRegName,511,"Software\\RealX\\3D Saver");
}

void SSaver::GetWebStoreURL()
{ 
	String::Copy(URL,511, "");

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

	String::Copy(URL,511,strGetFullVersion);
}

void SSaver::SettingsRead( ScreenSaverDefOptions* pOptions )
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

		RegQueryValueEx( hkey, TEXT("Key"), NULL, &dwType2, (BYTE*)&RegKey, &dwLength2 );
		RegQueryValueEx( hkey, TEXT("Name"), NULL, &dwType2, (BYTE*)&Name  , &dwLength1 );
		
		RegCloseKey( hkey );

		//core.SetValue("SaverRegistered", 0);
		//if (CheckReadedRegKey()) core.SetValue("SaverRegistered", 1);
	}
}
//-----------------------------------------------------------------------------
void SSaver::SettingsWrite( ScreenSaverDefOptions* pOptions )
{
	HKEY hkey;
	DWORD value;
    if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_CURRENT_USER, strScreenRegName, 
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL ) )
    {
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

		value=pOptions->MusicVolume;
		RegSetValueEx( hkey, TEXT("MusicVolume"), NULL, REG_DWORD, (BYTE*)&value, sizeof(DWORD) );

		RegSetValueEx( hkey, TEXT("Name"), NULL, REG_SZ, (BYTE*)&Name, sizeof(char)*85 );
		RegSetValueEx( hkey, TEXT("Key"), NULL, REG_SZ, (BYTE*)&RegKey, sizeof(char)*16 );

		RegFlushKey( hkey );
        RegCloseKey( hkey );
    }
}

void SSaver::LocalizeConfig(HWND hDlg)
{
/*	IIniFile* ini = Files()->LoadIniFile("Data\\cfg\\text.txt");

	if (ini) 
	{	
		SetWindowText(hDlg, ini->Read("text","DIALOG", "OptionsCaption"));
		SetDlgItemText(hDlg, IDOpt_Gr, ini->Read("text","DIALOG", "OptionsSelection"));

		SetDlgItemText(hDlg, IDCB_UDESKSET, ini->Read("text","DIALOG", "UseDesktop"));

		SetDlgItemText(hDlg, IDS_MUSICVOLUME, ini->Read("text","DIALOG", "MusicVolume"));
		SetDlgItemText(hDlg, IDS_STGC, ini->Read("text","DIALOG", "Gamma"));
		SetDlgItemText(hDlg, IDCB_EXITONMOUSE, ini->Read("text","DIALOG", "ExitMouseMove"));
		SetDlgItemText(hDlg, IDCB_SHOWTIME, ini->Read("text","DIALOG", "ShowTime"));

		SendDlgItemMessage( hDlg, IDCB_CLOCKTYPE, CB_RESETCONTENT, 0, 0 );
		SendDlgItemMessage( hDlg, IDCB_CLOCKTYPE, CB_ADDSTRING, 0, (LPARAM)ini->Read("text","DIALOG", "clck_12h"));
		SendDlgItemMessage( hDlg, IDCB_CLOCKTYPE, CB_ADDSTRING, 0, (LPARAM)ini->Read("text","DIALOG", "clck_24h"));
		SendDlgItemMessage( hDlg, IDCB_CLOCKTYPE, CB_ADDSTRING, 0, (LPARAM)ini->Read("text","DIALOG", "clck_analog"));

		SetDlgItemText(hDlg, IDB_RESTOREDEF, ini->Read("text","DIALOG", "RestoreDef"));
		SetDlgItemText(hDlg, IDReg_Gr, ini->Read("text","DIALOG", "Registration"));
		SetDlgItemText(hDlg, IDB_REGISTER, ini->Read("text","DIALOG", "Register"));
		SetDlgItemText(hDlg, IDSC_OK, ini->Read("text","DIALOG", "Ok"));
		SetDlgItemText(hDlg, IDSC_CANCEL, ini->Read("text","DIALOG", "Cancel"));	

		ini->Release();
	}*/
}

void SSaver::LocalizeRegistration(HWND hDlg)
{
/*	IIniFile* ini = Files()->LoadIniFile("Data\\cfg\\text.txt");

	if (ini) 
	{
		SetWindowText(hDlg, ini->Read("text","DIALOG", "RegCaption"));
		SetDlgItemText(hDlg, IDS_RegName, ini->Read("text","DIALOG", "RegName"));
		SetDlgItemText(hDlg, IDS_RegCode, ini->Read("text","DIALOG", "RegKey"));
		SetDlgItemText(hDlg, IDR_OK, ini->Read("text","DIALOG", "RegOk"));
		SetDlgItemText(hDlg, IDR_HOW, ini->Read("text","DIALOG", "RegBuy"));		
		SetDlgItemText(hDlg, IDR_CANCEL, ini->Read("text","DIALOG", "RegCancel"));

		String::Copy(strMes_RegValid,255,ini->Read("text","DIALOG", "RegValid"));
		String::Copy(strMes_RegInvalid,255,ini->Read("text","DIALOG", "RegInvalide"));

		ini->Release();
	}*/
}

int SSaver::CheckEnteredRegKey(HWND hDlg)
{
	char Key[256];	
	String::Copy(Key,255,strKey);

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
	for(int i=0;i<16;i++)
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

	int KeyLen=(int)strlen(Key);
	int RealKeyLen=KeyLen;
	if (KeyLen>16) RealKeyLen=16;

	cur_index=0;
	for(int i=0;i<16;i++)
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

	for (int i=0;i<8;i++)
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

	for(int i=0;i<16;i++)
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

	for (int i=0;i<16;i++)
	{
		Codes3[i]-=Codes2[i];
		if (Codes3[i]<0) Codes3[i]+=63;
	}
	
	for (int i=0;i<8;i++)
	{
		int tmp=Codes3[i];
		Codes3[i]=Codes3[8+i];
		Codes3[8+i]=tmp;
	}

	for (int i=0;i<8;i++)
	{
		if (Codes3[i]!=Codes3[15-i]) return 4;
	}

	return 0;
}
//-----------------------------------------------------------------------------
SSaver::SaverMode SSaver::ParseCommandLine( TCHAR* pstrCommandLine )
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

bool SSaver::InterruptSaver()
{
	ShutdownSaver();

	return true;
}

void SSaver::ShutdownSaver()
{
	Release();
	Quit=true;
}
//-----------------------------------------------------------------------------
bool SSaver::Proc_Saver( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch ( uMsg )
    {
		case WM_CREATE:
		{
			if (m_SaverMode==sm_preview)
			{		
				FileBuffer* bmp = core.Files()->LoadFile("Bitmap/Preview.bmp");
			
				if (bmp)
				{
					hPreview.LoadBMP("Bitmap/Preview.bmp", bmp->GetBuffer());
					bmp->Release();
				}			
			}	
		}
		break;
        
		case WM_CLOSE:
		case WM_DESTROY:
		{					
			Quit=true;

			return TRUE;
		}
		break;
        
		case WM_SETCURSOR:
        if ( m_SaverMode == sm_full)
        {
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
		{
			if ( m_SaverMode != sm_preview && CurrentOptions.ExitOnMouseMove)
			{	
				dwFakeMouseMoveMsgAllowed--;
				if (dwFakeMouseMoveMsgAllowed<0) return InterruptSaver();				
			}
		}
		break;

		case WM_KEYDOWN:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
		{
			if (wParam==VK_SPACE) 
			{	
				//ShutdownSaver();
				
				//ShowWindow(hWnd,SW_HIDE);
				//DoAbout();
				//ShutdownSaver();

				//render->WNDPresent();

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
		}	
        break;

        case WM_ACTIVATEAPP:
		{
            if( wParam == FALSE && m_SaverMode != sm_test ) return InterruptSaver();
		}
        break;

        case WM_POWERBROADCAST:
		{
            if( wParam == PBT_APMSUSPEND) return InterruptSaver();
		}
        break;

        case WM_SYSCOMMAND: 
		{
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
                }
            }
		}
        break;
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

bool SSaver::CreateSaverWindow()
{
	// Register an appropriate window class
	WNDCLASS    cls;
	cls.hCursor        = LoadCursor( NULL, IDC_ARROW );
	cls.hIcon          = NULL;//LoadIcon( m_hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON) ); 
	cls.lpszMenuName   = NULL;
	cls.lpszClassName  = TEXT(windowName);
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
		{
			GetClientRect( m_hWndParent, &rc );			
			dwStyle = WS_VISIBLE | WS_CHILD;
			AdjustWindowRect( &rc, dwStyle, FALSE );

			hWnd = CreateWindow( TEXT(windowName), windowName, dwStyle, 
								 rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, 
								 m_hWndParent, NULL, m_hInstance, NULL );
		}
		break;
		case sm_test:
		{
			rc.left = rc.top = 50;
			rc.right = rc.left+600;
			rc.bottom = rc.top+400;
			dwStyle = WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
			AdjustWindowRect( &rc, dwStyle, FALSE );
			hWnd = CreateWindow( TEXT(windowName), windowName, dwStyle, 
								 rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, 
								 NULL, NULL, m_hInstance, NULL );
		}
		break;
		case sm_full:
		{
			dwStyle = WS_VISIBLE | WS_POPUP;
			hWnd = NULL;

			rc.left = rc.top = 50;
			rc.right = rc.left+600;
			rc.bottom = rc.top+400;             

			if (DebugMode)
			{				
				hWnd = CreateWindow( TEXT(windowName), windowName, dwStyle, 
									 rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, 
									 NULL, NULL, m_hInstance, NULL );
			}
			else
			{
				//rc = pMonitorInfo->rcScreen;
				hWnd = CreateWindowEx( WS_EX_TOPMOST, TEXT(windowName), 
									   windowName, dwStyle, rc.left, rc.top, rc.right - rc.left, 
									   rc.bottom - rc.top, hWnd, NULL, m_hInstance, NULL );
			}
		}
		break;
	}

	if ( hWnd == NULL )
	{
		return false;
	}

	SetCursor( NULL );

	return true;
}

bool SSaver::CheckReadedRegKey()
{
	/*if (Mode == svm_full)
	{
		return true;
	}*/

	char Key[256];	
	String::Copy(Key,255,strKey);

	int  Codes[16];
	int  Codes2[16];
	int  Codes3[16];

	int NameLen = (int)strlen(Name);
	
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

	int KeyLen=(int)strlen(Key);
	int RealKeyLen=KeyLen;
	if (KeyLen>16) RealKeyLen=16;

	cur_index=0;
	for(int i=0;i<16;i++)
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

	for (int i=0;i<8;i++)
	{
		int tmp=Codes2[i];
		Codes2[i]=Codes2[8+i];
		Codes2[8+i]=tmp;
	}

	int RegKeyLen = (int)strlen(RegKey);
	
	if (RegKeyLen!=16) return false;

	for(int i=0;i<16;i++)
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

	for (int i=0;i<16;i++)
	{
		Codes3[i]-=Codes2[i];
		if (Codes3[i]<0) Codes3[i]+=63;
	}
	
	for (int i=0;i<8;i++)
	{
		int tmp=Codes3[i];
		Codes3[i]=Codes3[8+i];
		Codes3[8+i]=tmp;
	}

	for (int i=0;i<8;i++)
	{
		if (Codes3[i]!=Codes3[15-i]) return false;
	}

	return true;
}

void SSaver::ReadCFG()
{
	/*IIniFile* ini = Files()->LoadIniFile("Data\\cfg\\settings.txt");

	if (ini) 
	{
		String::Copy(strMainWebSite,512,ini->Read("RealX.com","settings", "main_web_site_url"));
		String::Copy(strGetFullVersion,512,ini->Read("RealX.com", "settings", "get_full_ver_url"));		

		String::Copy(strScreenRegName,512,ini->Read("RealX.com", "settings", "Reg_Path"));

		String::Copy(strKey,256,ini->Read("RealX.com", "settings", "Key"));

		String::Copy(strDigitalRiverRegName,512,ini->Read("RealX.com", "settings", "digital_River"));
		

		char buffer[64];

		String::Copy(buffer,64,ini->Read("normal", "settings", "version"));

		if (String::IsEqual(buffer,"normal")==0)
		{
			Mode = svm_normal;
		}
		else
		if (String::IsEqual(buffer,"full")==0)
		{
			Mode = svm_full;
		}	
		
		String::Copy(buffer,64,ini->Read("false", "settings", "ExitOnMouseMove"));
		if (String::IsEqual(buffer,"true")==0)
		{
			DefaultOptions.ExitOnMouseMove = true;
		}
		else
		{
			DefaultOptions.ExitOnMouseMove = false;
		}
		
		String::Copy(buffer,64,ini->Read("100", "settings", "MusicVolume"));
		String::Scanf(buffer,64,"%i",&DefaultOptions.MusicVolume);		

		String::Copy(buffer,64,ini->Read("800", "settings", "Width"));
		String::Scanf(buffer,64,"%i",&DefaultOptions.Width);

		String::Copy(buffer,64,ini->Read("600", "settings", "Height"));
		String::Scanf(buffer,64,"%i",&DefaultOptions.Height);

		String::Copy(buffer,64,ini->Read("32", "settings", "BitCount"));
		String::Scanf(buffer,64,"%i",&DefaultOptions.BitCount);

		String::Copy(buffer,64,ini->Read("false", "settings", "UseDktpSet"));
		if (String::IsEqual(buffer,"true")==0)
		{
			DefaultOptions.UseDktpSet = true;
		}
		else
		{
			DefaultOptions.UseDktpSet = false;
		}

		String::Copy(buffer,64,ini->Read("0", "settings", "TimerType"));
		String::Scanf(buffer,64,"%i",&DefaultOptions.TimerType);		

		String::Copy(buffer,64,ini->Read("100", "settings", "GammControl"));
		String::Scanf(buffer,64,"%i",&DefaultOptions.GammControl);

		ini->Release();
	}

	ini = Files()->LoadIniFile("Data\\cfg\\text.txt");

	if (ini) 
	{
		String::Copy(strMes_StartWn,256,ini->Read("text","DIALOG", "welcome"));
		String::Copy(strMes_NotCaption,256,ini->Read("text","DIALOG", "welcome_caption"));		

		String::Copy(str_Bits,64,ini->Read("text","DIALOG", "Bits"));
		
		String::Copy(str_RegSt,256,ini->Read("text","DIALOG", "RegState"));
		String::Copy(str_RegStPersL,256,ini->Read("text","DIALOG", "PersLicence"));		
		String::Copy(str_UnregSt,256,ini->Read("text","DIALOG", "UnregState"));

		String::Copy(strMes_RegValid,256,ini->Read("text","DIALOG", "RegValid"));
		String::Copy(strMes_RegInvalid,256,ini->Read("text","DIALOG", "RegInvalide"));
		
		ini->Release();
	}*/

	CurrentOptions=DefaultOptions;
}

void SSaver::Init( HINSTANCE hInstance )
{
	InitCommonControls();

	HRESULT hr;

    m_hInstance = hInstance;

    // Parse the command line and do the appropriate thing
    TCHAR* pstrCmdLine = GetCommandLine();
    m_SaverMode = ParseCommandLine( pstrCmdLine );

	if (DebugMode) m_SaverMode = sm_test;

	Init_Success=true;
	
	HWND hwnd = FindWindow (TEXT(windowName), NULL);
	if (hwnd) Init_Success=false;

	//m_SaverMode = sm_preview;

	core.Files()->SetLoadFromRes(true);

	core.Trace("Starting");

	if (Init_Success)
	{
		// Create the screen saver window(s)
		if( m_SaverMode == sm_preview || 
			m_SaverMode == sm_test    || 
			m_SaverMode == sm_full )
		{
			if( FAILED( hr = CreateSaverWindow() ) )
			{
				core.Trace("Cannot create Saver Window!");
			}
			else
			{	
				core.Trace("Create Saver Window complete");
			}
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

	/*
	// find Program Manager
	HWND hWndDst = FindWindowEx(GetDesktopWindow() , 0 , "Progman" , "Program Manager");
	// find SHELLDLL_DefView
	hWndDst = FindWindowEx(hWndDst , 0 , "SHELLDLL_DefView" , 0);
	// find Desktop Folder!!
	hWndDst = FindWindowEx(hWndDst , 0 , "SysListView32" , "FolderView");
	// initd3d by desktop forlder window!!
	// You must use this hWnd to create D3DDevice when rendering to desktop !!
*/

	core.SetHWND(hWnd);

	if ((m_SaverMode == sm_test || m_SaverMode == sm_full) && !DebugMode)
	{
		/*RenderService::VideoMode mode;
		mode.windowed = false;

		HWND hDsk = GetDesktopWindow();
		RECT rc;
		GetClientRect(hDsk, &rc);
		mode.width = rc.right;
		mode.height = rc.bottom;
		mode.bit_count = 32;

		core.Render()->Setw(mode);*/
	}

	core.Init();

	ReadCFG();
}
//-----------------------------------------------
void SSaver::DoSaver()
{
	if (m_SaverMode!=sm_preview)
	{
		SetVideoMode();
		LoadData();
	}

    // Message pump
    BOOL bGotMsg;
    MSG msg;
    msg.message = WM_NULL;

	Quit=false;

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
			if (m_SaverMode!=sm_preview) MainLoop();
        }
    }

	UpdateWindow(HWND_DESKTOP);
}

void SSaver::InitConfigControls(HWND hDlg)
{
	HWND hDLG = GetDlgItem( hDlg, IDCB_SHOWTIME );
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

	/*hDLG = GetDlgItem( hDlg, IDR_24H );
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
	SendMessage(hDLG, TBM_SETPAGESIZE, 0, (LPARAM) 10);	

	ShowWindow(hDLG, UseSFX); 

	if (CheckReadedRegKey()) core.SetValue("SaverRegistered", 1);

	hDLG = GetDlgItem( hDlg, IDB_REGISTER);
	EnableWindow( hDLG, !(core.GetValue("SaverRegistered",0)>0) );
		
	hDLG = GetDlgItem( hDlg, IDB_HOW);
	EnableWindow( hDLG, !(core.GetValue("SaverRegistered",0)>0) );

	char RegStatusStr[100];
	String::Copy(RegStatusStr,99,"");
	
	if (!(core.GetValue("SaverRegistered",0)>0))
	{
		String::Copy(RegStatusStr,99,str_UnregSt);
	}
	else
	{		
		if (strcmp(Name,"")==0)
		{
			String::Copy(RegStatusStr,99,str_RegStPersL);
		}
		else
		{
			String::Copy(RegStatusStr,99,str_RegSt);
			String::Cat(RegStatusStr,99," ");
			String::Cat(RegStatusStr,99,Name);
		}		
	}

	SetDlgItemText(hDlg, IDS_RSTATUS, RegStatusStr);	


	hDLG = GetDlgItem( hDlg, IDCB_CLOCKTYPE);
	EnableWindow( hDLG, CurrentOptions.ShowTime );

	SendDlgItemMessage( hWnd, IDCB_CLOCKTYPE, CB_SETCURSEL, 
						(WPARAM)CurrentOptions.TimerType, 0L );*/
}

VOID SSaver::DoConfig()
{
	DialogBox( m_hInstance, MAKEINTRESOURCE(IDD_SSCONFIG), NULL, SSConfigProc );
}

bool SSaver::Proc_Config( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
		case WM_INITDIALOG:
		{		
			FileBuffer* bmp = core.Files()->LoadFile("Bitmap/screen_settings.bmp");
			
			if (bmp)
			{
				hDialogSetting.LoadBMP("Bitmap/screen_settings.bmp", bmp->GetBuffer());
				bmp->Release();
			}
	
			hWnd=hDlg;			
			
			SettingsRead( &CurrentOptions );

			/*if (!(core.GetValue("SaverRegistered",0)>0))
			{
				::MessageBox(hDlg,strMes_StartWn,strMes_NotCaption,NULL);
			}*/
		
			LocalizeConfig(hDlg);
			UpdateComboBoxes();
			InitConfigControls(hDlg);
		}
		break;
		case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;

			hdc=BeginPaint (hDlg,&ps);			

			WINDOWINFO info;
			GetWindowInfo(hDlg,&info);		
			
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
				int tb_pos = (int)SendMessage(hDLG, TBM_GETPOS, 0, 0); 

				CurrentOptions.MusicVolume = tb_pos;				
			}

			if( TB_THUMBTRACK == LOWORD(wParam) ||
				TB_PAGEDOWN == LOWORD(wParam) ||
				TB_PAGEUP == LOWORD(wParam)||
				TB_THUMBPOSITION == LOWORD(wParam)||
				TB_ENDTRACK == LOWORD(wParam))
			{
				HWND hDLG = GetDlgItem( hDlg, IDTB_GCONTROL );
				int tb_pos = (int)SendMessage(hDLG, TBM_GETPOS, 0, 0); 

				CurrentOptions.GammControl=tb_pos;
			}
		}
		break;
		case WM_COMMAND:
		{
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

			if ( IDB_REGISTER ==LOWORD(wParam))
			{	

				DoReg();

				InitConfigControls(hDlg);
			}
		
			if ( IDB_RESTOREDEF ==LOWORD(wParam))
			{
				CurrentOptions=DefaultOptions;
				UpdateComboBoxes();
				InitConfigControls(hDlg);
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
					CurrentOptions.MusicVolume = (int)SendMessage(hDLG, TBM_GETPOS, 0, 0); 
				}
			}

			if( IDSC_OK == LOWORD(wParam) )
			{			
				SettingsWrite( &CurrentOptions );
				EndDialog( hDlg, IDOK );
				
				return true;
			}
			
			if( IDSC_CANCEL == LOWORD(wParam) )
			{
				EndDialog( hDlg, IDCANCEL );
			
				return true;
			}

			if( CBN_SELENDOK == HIWORD(wParam) )
			{
				dword dwIndex = (dword)SendMessage( (HWND)lParam, CB_GETCURSEL, 0, 0 );
				dword dwData  = (dword)SendMessage( (HWND)lParam, CB_GETITEMDATA, dwIndex, 0 );
            
				if( CB_ERR == dwIndex ) return TRUE;
            
				switch( LOWORD( wParam ) )
				{
					case IDC_VIDEOMODE:
					{
						/*VideoMode mode;
						Render()->GetMode(mode, dwData);

						CurrentOptions.Width=mode.width;
						CurrentOptions.Height=mode.height;
						CurrentOptions.BitCount=mode.bitCount;*/
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

    return false;
}

void SSaver::DoReg()
{	
	DialogBox( (HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE ),
			   MAKEINTRESOURCE(IDD_REGISTER), hWnd, SSRegProc );
}

bool SSaver::Proc_Reg( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( WM_INITDIALOG == uMsg )
    {
		SetFocus(hDlg);

		LocalizeRegistration(hDlg);

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
					//core.SetValue("SaverRegistered",1);
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
		
			/*if (core.GetValue("SaverRegistered",0)>0) 
			{
				EndDialog( hDlg, IDOK );
				return TRUE;
			}*/
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

INT SSaver::Run()
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
			SettingsRead( &CurrentOptions );
	
			/*Renderer.FindMode(CurrentOptions.Width,
							  CurrentOptions.Height,
							  CurrentOptions.BitCount,
							  CurrentOptions.FSAA);

			CurrentOptions.Width=Renderer.Cur_VMode.Width;
			CurrentOptions.Height=Renderer.Cur_VMode.Height;
		
			if (Renderer.Cur_VMode.Format==D3DFMT_A8R8G8B8||
				Renderer.Cur_VMode.Format==D3DFMT_X8R8G8B8) CurrentOptions.BitCount=32;
			else CurrentOptions.BitCount=16;
		
			CurrentOptions.FSAA=Renderer.Cur_VMode.MultiSampleType;*/
		
			DoSaver();
			
			DestroyWindow(hWnd);
			//SetWindowPos(m_hWnd, HWND_NOTOPMOST, 
			//			 0,0,1,1,SWP_SHOWWINDOW );

			//DialogBox( NULL, MAKEINTRESOURCE(IDD_REGISTER), NULL, SSRegProc );
		
            /*if( FAILED( hr = DoSaver() ) )
                DisplayErrorMsg( hr, 0 );*/
        }
		break;
    }	
	
	if (NeedShowReg)
	{		
		/*if (!(core.GetValue("SaverRegistered",0)>0))
		{		
			hWnd = NULL;

			DoReg();

			SettingsWrite( &CurrentOptions );
		}*/
	}

    return 0;
}

void SSaver::SetVideoMode()
{	
	/*if (DebugMode)
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

	Renderer.GammaCtrl=CurrentOptions.GammControl;*/
	
	//Render()->SetGamma(CurrentOptions.GammControl);
	InitRenderDevice();
}

void SSaver::InitRenderDevice()
{
	/*Render()->SetRenderState( RST_SrcBlend,  RBlend_SrcAlpha );
	Render()->SetRenderState( RST_DestBlend, RBlend_One );

	Render()->SetTextureStageState( 1, RTSS_TexCoordIndex, 0 );

	Render()->SetRenderState( RST_FogVertexMode,  RFog_None );
	Render()->SetRenderState( RST_FogTableMode,   RFog_Linear );

	Render()->SetTextureStageState( 0, RTSS_ColorArg1, RTA_Texture );
	Render()->SetTextureStageState( 0, RTSS_ColorOp,   RTOp_Modulate );
	Render()->SetTextureStageState( 0, RTSS_ColorArg2, RTA_Diffuse );

	Render()->SetTextureStageState( 0, RTSS_AlphaOp,   RTOp_Modulate );
	Render()->SetTextureStageState( 0, RTSS_AlphaArg1, RTA_Texture );  
	Render()->SetTextureStageState( 0, RTSS_AlphaArg2, RTA_Diffuse );  

	Render()->SetTextureStageState( 1, RTSS_ColorOp,   RTOp_Disable );
	Render()->SetTextureStageState( 1, RTSS_AlphaOp,   RTOp_Disable);	

	Render()->SetRenderState( RST_AlphaRef, 0x30 );
	Render()->SetRenderState( RST_AlphaFunc, RCmp_GreaterEqual );

	Render()->SetWorld(Matrix());
	Render()->SetProjection( 60, Render()->GetAspectRatio(),0.05f, 10000.0f);

	Render()->SetSamplerState( 0, RSamp_MinFilter, RTexF_Linear );
	Render()->SetSamplerState( 0, RSamp_MipFilter, RTexF_Linear );
	Render()->SetSamplerState( 0, RSamp_MagFilter, RTexF_Linear );
	Render()->SetSamplerState( 0, RSamp_AddressU , RTAddress_Wrap);
	Render()->SetSamplerState( 0, RSamp_AddressV , RTAddress_Wrap);

	Render()->SetSamplerState( 1, RSamp_MinFilter, RTexF_Linear );
	Render()->SetSamplerState( 1, RSamp_MipFilter, RTexF_Linear );
	Render()->SetSamplerState( 1, RSamp_MagFilter, RTexF_Linear );
	Render()->SetSamplerState( 1, RSamp_AddressU , RTAddress_Wrap);
	Render()->SetSamplerState( 1, RSamp_AddressV , RTAddress_Wrap);

	Render()->SetSamplerState( 2, RSamp_MinFilter, RTexF_Linear );
	Render()->SetSamplerState( 2, RSamp_MipFilter, RTexF_Linear );
	Render()->SetSamplerState( 2, RSamp_MagFilter, RTexF_Linear );
	Render()->SetSamplerState( 2, RSamp_AddressU , RTAddress_Wrap);
	Render()->SetSamplerState( 2, RSamp_AddressV , RTAddress_Wrap);

	Render()->SetSamplerState( 3, RSamp_MinFilter, RTexF_Linear );
	Render()->SetSamplerState( 3, RSamp_MipFilter, RTexF_Linear );
	Render()->SetSamplerState( 3, RSamp_MagFilter, RTexF_Linear );
	Render()->SetSamplerState( 3, RSamp_AddressU , RTAddress_Wrap);
	Render()->SetSamplerState( 3, RSamp_AddressV , RTAddress_Wrap);

	Render()->SetRenderState( RST_DitherEnable, true );
	Render()->SetRenderState( RST_ZEnable, true );
	Render()->SetRenderState( RST_Lighting, false );*/
}

void SSaver::LoadData()
{	
	const char* scene = "loader.plb";
	core.SceneManager()->Load(scene, StreamLoader::binary);
}

void SSaver::MainLoop()
{
	core.CountDeltaTime();

	//core.Render()->Begin();		
	//core.Render()->SetBackColor(0xff000000);

	core.Update(core.GetDeltaTime());		

	//core.Render()->End();

	// Test the cooperative level to see if it's okay to render
	/*if( S_OK != Render()->IsDeviceLost() )
    {		
        // If the device was lost, do not render until we get it back
        //if( D3DERR_DEVICELOST == hr )
		{					
			return;
		}

        // Check if the device needs to be resized.
        //if( D3DERR_DEVICENOTRESET == hr )
        {					
			//ResetDevice();
        }

        return;
    }
	else
	{
		core.CountDeltaTime();

		float timedelta = core.GetDeltaTime();
		if (timedelta<0) timedelta=0;

		Draw(timedelta);
	}*/
}

void SSaver::UpdateComboBoxes()
{  
	/*VideoMode mode;
	bool mode_finded = false;

	for (int i = 0; i< Render()->GetModesCount(); i++ )
	{
		Render()->GetMode(mode, i);

		if (CurrentOptions.Width==mode.width &&
			CurrentOptions.Height==mode.height &&
			CurrentOptions.BitCount==mode.bitCount)
		{
			mode_finded = true;
		}
	}

	if (!mode_finded)
	{
		Render()->GetMode(mode, 0);

		CurrentOptions.Width=mode.width;
		CurrentOptions.Height=mode.height;
		CurrentOptions.BitCount=mode.bitCount;
	}


	for (int i = 0; i< Render()->GetModesCount(); i++ )
    {
		Render()->GetMode(mode, i);

		char strMode[80];
                
        String::Printf( strMode, 79, "%ld x %ld %ld %s",
					    mode.width, mode.height, mode.bitCount, str_Bits);
                
        DWORD dwItem = SendDlgItemMessage( hWnd, IDC_VIDEOMODE, 
                                           CB_ADDSTRING, 0,
                                           (LPARAM)strMode );

        SendDlgItemMessage( hWnd, IDC_VIDEOMODE, CB_SETITEMDATA, 
                            (WPARAM)dwItem, (LPARAM)i );

        if( CurrentOptions.Width==mode.width &&
			CurrentOptions.Height==mode.height &&
			CurrentOptions.BitCount==mode.bitCount)
		{
			SendDlgItemMessage( hWnd, IDC_VIDEOMODE, CB_SETCURSEL, 
                                (WPARAM)dwItem, 0L );
		}
    }*/
}

void SSaver::Release()
{
}

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
	//srand((dword)time(NULL));

	SSaver svr;
	svr.Init(hInst);
	svr.Run();		

	return 0;
}
