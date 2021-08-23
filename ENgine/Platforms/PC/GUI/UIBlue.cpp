

#include "UIBlue.h"

#include "TCHAR.H"
#include "CommCtrl.H"

#define MAX max
#define MIN min
#define ASSERT(expr)  

#define IDB_ICONS16 200
#define IDB_ICONS24 201
#define IDB_ICONS32 202
#define IDB_ICONS50 203


#define lengthof(x) (sizeof(x)/sizeof(*x))

/////////////////////////////////////////////////////////////////////////////////////
//
//

#ifndef BlendRGB
   #define BlendRGB(c1, c2, factor) \
      RGB( GetRValue(c1) + ((GetRValue(c2) - GetRValue(c1)) * factor / 100L), \
           GetGValue(c1) + ((GetGValue(c2) - GetGValue(c1)) * factor / 100L), \
           GetBValue(c1) + ((GetBValue(c2) - GetBValue(c1)) * factor / 100L) )
#endif


CStdString::CStdString() : m_pstr(m_szBuffer)
{
   m_szBuffer[0] = '\0';
}

CStdString::CStdString(const TCHAR ch) : m_pstr(m_szBuffer)
{
   m_szBuffer[0] = ch;
   m_szBuffer[1] = '\0';
}

CStdString::CStdString(LPCTSTR lpsz, int nLen) : m_pstr(m_szBuffer)
{      
   ASSERT(!::IsBadStringPtr(lpsz,-1) || lpsz==NULL);
   Assign(lpsz, nLen);
}

CStdString::CStdString(const CStdString& src) : m_pstr(m_szBuffer)
{
   Assign(src.m_pstr);
}

CStdString::~CStdString()
{
   if( m_pstr != m_szBuffer ) free(m_pstr);
}

CStdString CStdString::RES(UINT nRes)
{
   TCHAR szBuffer[256];
   int cchLen = ::LoadString(CPaintManagerUI::GetLanguageInstance(), nRes, szBuffer, lengthof(szBuffer) - 1);
   ASSERT(cchLen>0);
   szBuffer[cchLen] = '\0';
   return szBuffer;
}

int CStdString::GetLength() const
{ 
   return (int) _tcslen(m_pstr); 
}

CStdString::operator LPCTSTR() const 
{ 
   return m_pstr; 
}

void CStdString::Append(LPCTSTR pstr)
{
   int nNewLength = GetLength() + (int) _tcslen(pstr);
   if( nNewLength >= MAX_LOCAL_STRING_LEN ) {
      if( m_pstr == m_szBuffer ) {
         m_pstr = static_cast<LPTSTR>(malloc((nNewLength + 1) * sizeof(TCHAR)));
         _tcscpy(m_pstr, m_szBuffer);
         _tcscat(m_pstr, pstr);
      }
      else {
         m_pstr = static_cast<LPTSTR>(realloc(m_pstr, (nNewLength + 1) * sizeof(TCHAR)));
         _tcscat(m_pstr, pstr);
      }
   }
   else {
      if( m_pstr != m_szBuffer ) {
         free(m_pstr);
         m_pstr = m_szBuffer;
      }
      _tcscat(m_szBuffer, pstr);
   }
}

void CStdString::Assign(LPCTSTR pstr, int cchMax)
{
   if( pstr == NULL ) pstr = _T("");
   cchMax = (cchMax < 0 ? (int) _tcslen(pstr) : cchMax);
   if( cchMax < MAX_LOCAL_STRING_LEN ) {
      if( m_pstr != m_szBuffer ) {
         free(m_pstr);
         m_pstr = m_szBuffer;
      }
   }
   else if( cchMax > GetLength() || m_pstr == m_szBuffer ) {
      if( m_pstr == m_szBuffer ) m_pstr = NULL;
      m_pstr = static_cast<LPTSTR>(realloc(m_pstr, (cchMax + 1) * sizeof(TCHAR)));
   }
   _tcsncpy(m_pstr, pstr, cchMax);
   m_pstr[cchMax] = '\0';
}

bool CStdString::IsEmpty() const 
{ 
   return m_pstr[0] == '\0'; 
}

void CStdString::Empty() 
{ 
   if( m_pstr != m_szBuffer ) free(m_pstr);
   m_pstr = m_szBuffer;
   m_szBuffer[0] = '\0'; 
}

LPCTSTR CStdString::GetData()
{
   return m_pstr;
}

TCHAR CStdString::GetAt(int nIndex) const
{
   return m_pstr[nIndex];
}

TCHAR CStdString::operator[] (int nIndex) const
{ 
   return m_pstr[nIndex];
}   

const CStdString& CStdString::operator=(const CStdString& src)
{      
   Assign(src);
   return *this;
}

/*
#ifndef _UNICODE

const CStdString& CStdString::operator=(LPCWSTR lpwStr)
{      
   ASSERT(!::IsBadStringPtrW(lpwStr,-1));
   int cchStr = ((int) wcslen(lpwStr) * 2) + 1;
   LPSTR pstr = (LPSTR) _alloca(cchStr);
   if( pstr != NULL ) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
   Assign(pstr);
   return *this;
}

#endif // _UNICODE

*/

const CStdString& CStdString::operator=(LPCTSTR lpStr)
{      
   ASSERT(!::IsBadStringPtr(lpStr,-1));
   Assign(lpStr);
   return *this;
}

const CStdString& CStdString::operator=(const TCHAR ch)
{
   Empty();
   m_szBuffer[0] = ch;
   m_szBuffer[1] = '\0';
   return *this;
}

CStdString CStdString::operator+(const CStdString& src)
{
   Append(src);
   return *this;
}

CStdString CStdString::operator+(LPCTSTR lpStr)
{
   ASSERT(!::IsBadStringPtr(lpStr,-1));
   Append(lpStr);
   return *this;
}

const CStdString& CStdString::operator+=(const CStdString& src)
{      
   Append(src);
   return *this;
}

const CStdString& CStdString::operator+=(LPCTSTR lpStr)
{      
   ASSERT(!::IsBadStringPtr(lpStr,-1));
   Append(lpStr);
   return *this;
}

const CStdString& CStdString::operator+=(const TCHAR ch)
{      
   TCHAR str[] = { ch, '\0' };
   Append(str);
   return *this;
}

bool CStdString::operator == (LPCTSTR str) const { return (Compare(str) == 0); };
bool CStdString::operator != (LPCTSTR str) const { return (Compare(str) != 0); };
bool CStdString::operator <= (LPCTSTR str) const { return (Compare(str) <= 0); };
bool CStdString::operator <  (LPCTSTR str) const { return (Compare(str) <  0); };
bool CStdString::operator >= (LPCTSTR str) const { return (Compare(str) >= 0); };
bool CStdString::operator >  (LPCTSTR str) const { return (Compare(str) >  0); };

void CStdString::SetAt(int nIndex, TCHAR ch)
{
   ASSERT(nIndex>=0 && nIndex<GetLength());
   m_pstr[nIndex] = ch;
}

int CStdString::Compare(LPCTSTR lpsz) const 
{ 
   return _tcscmp(m_pstr, lpsz); 
}

int CStdString::CompareNoCase(LPCTSTR lpsz) const 
{ 
   return _tcsicmp(m_pstr, lpsz); 
}

void CStdString::MakeUpper() 
{ 
   _tcsupr(m_pstr); 
}

void CStdString::MakeLower() 
{ 
   _tcslwr(m_pstr); 
}

CStdString CStdString::Left(int iLength) const
{
   if( iLength < 0 ) iLength = 0;
   if( iLength > GetLength() ) iLength = GetLength();
   return CStdString(m_pstr, iLength);
}

CStdString CStdString::Mid(int iPos, int iLength) const
{
   if( iLength < 0 ) iLength = GetLength() - iPos;
   if( iPos + iLength > GetLength() ) iLength = GetLength() - iPos;
   if( iLength <= 0 ) return CStdString();
   return CStdString(m_pstr + iPos, iLength);
}

CStdString CStdString::Right(int iLength) const
{
   int iPos = GetLength() - iLength;
   if( iPos < 0 ) {
      iPos = 0;
      iLength = GetLength();
   }
   return CStdString(m_pstr + iPos, iLength);
}

int CStdString::Find(TCHAR ch, int iPos /*= 0*/) const
{
   ASSERT(iPos>=0 && iPos<=GetLength());
   if( iPos != 0 && (iPos < 0 || iPos >= GetLength()) ) return -1;
   LPCTSTR p = _tcschr(m_pstr + iPos, ch);
   if( p == NULL ) return -1;
   return p - m_pstr;
}

int CStdString::Find(LPCTSTR pstrSub, int iPos /*= 0*/) const
{
   ASSERT(!::IsBadStringPtr(pstrSub,-1));
   ASSERT(iPos>=0 && iPos<=GetLength());
   if( iPos != 0 && (iPos < 0 || iPos > GetLength()) ) return -1;
   LPCTSTR p = _tcsstr(m_pstr + iPos, pstrSub);
   if( p == NULL ) return -1;
   return p - m_pstr;
}

int CStdString::ReverseFind(TCHAR ch) const
{
   LPCTSTR p = _tcsrchr(m_pstr, ch);
   if( p == NULL ) return -1;
   return p - m_pstr;
}

int CStdString::Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo)
{
   CStdString sTemp;
   int nCount = 0;
   int iPos = Find(pstrFrom);
   if( iPos < 0 ) return 0;
   int cchFrom = (int) _tcslen(pstrFrom);
   int cchTo = (int) _tcslen(pstrTo);
   while( iPos >= 0 ) {
      sTemp = Left(iPos);
      sTemp += pstrTo;
      sTemp += Mid(iPos + cchFrom);
      Assign(sTemp);
      iPos = Find(pstrFrom, iPos + cchTo);
      nCount++;
   }
   return nCount;
}

int CStdString::Format(LPCTSTR pstrFormat, ...)
{
   CStdString sFormat = pstrFormat;
   sFormat.ProcessResourceTokens();
   // Do ordinary printf replacements
   // NOTE: Documented max-length of wvsprintf() is 1024
   TCHAR szBuffer[1025] = { 0 };
   va_list argList;
   va_start(argList, pstrFormat);
   int iRet = ::wvsprintf(szBuffer, sFormat, argList);
   va_end(argList);
   Assign(szBuffer);
   return iRet;
}

void CStdString::ProcessResourceTokens()
{
   // Replace string-tokens: %{nnn}  where nnn is a resource string identifier
   int iPos = Find('%');
   while( iPos >= 0 ) {
      if( GetAt(iPos + 1) == '{' ) {
         int iEndPos = iPos + 2;
         while( isdigit(GetAt(iEndPos)) ) iEndPos++;
         if( GetAt(iEndPos) == '}' ) {
            CStdString sTemp = CStdString::RES((UINT)_ttoi(m_pstr + iPos + 2));
            Replace(Mid(iPos, iEndPos - iPos + 1), sTemp);
         }
      }
      iPos = Find('%', iPos + 1);
   }
}



HPEN m_hPens[UICOLOR__LAST] = { 0 };
HFONT m_hFonts[UIFONT__LAST] = { 0 };
HBRUSH m_hBrushes[UICOLOR__LAST] = { 0 };
LOGFONT m_aLogFonts[UIFONT__LAST] = { 0 };
COLORREF m_clrColors[UICOLOR__LAST][2] = { 0 };
TEXTMETRIC m_aTextMetrics[UIFONT__LAST] = { 0 };
HIMAGELIST m_himgIcons16 = NULL;
HIMAGELIST m_himgIcons24 = NULL;
HIMAGELIST m_himgIcons32 = NULL;
HIMAGELIST m_himgIcons50 = NULL;

HINSTANCE CPaintManagerUI::m_hInstance = NULL;
HINSTANCE CPaintManagerUI::m_hLangInst = NULL;


CPaintManagerUI::CPaintManagerUI() :
   m_hWndPaint(NULL),
   m_hDcPaint(NULL),
   m_hDcOffscreen(NULL),
   m_hbmpOffscreen(NULL),
   m_hwndTooltip(NULL),
   m_uTimerID(0x1000),   
   m_bFirstLayout(true),
   m_bFocusNeeded(false),
   m_bResizeNeeded(false),
   m_bMouseTracking(false),
   m_bOffscreenPaint(true) 
{
   if( m_hFonts[1] == NULL ) 
   {
      // Fill in default font information
      LOGFONT lf = { 0 };
      ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
      _tcscpy(lf.lfFaceName, _T("Tahoma"));
      // TODO: Handle "large fonts" or other font sizes when
      //       screen resolution changes!!!
      lf.lfHeight = -12;
      m_aLogFonts[UIFONT_NORMAL] = lf;
      m_aLogFonts[UIFONT_CAPTION] = lf;
      LOGFONT lfBold = lf;
      lfBold.lfWeight += FW_BOLD;
      m_aLogFonts[UIFONT_BOLD] = lfBold;
      lfBold.lfHeight -= 2;
      m_aLogFonts[UIFONT_TITLE] = lfBold;
      lfBold.lfHeight -= 4;
      m_aLogFonts[UIFONT_HEADLINE] = lfBold;
      LOGFONT lfSubscript = lf;
      lfSubscript.lfHeight -= 4;
      m_aLogFonts[UIFONT_SUBSCRIPT] = lfSubscript;
      LOGFONT lfLink = lf;
      lfLink.lfUnderline = TRUE;
      m_aLogFonts[UIFONT_LINK] = lfLink;

      // Fill the color table
      m_clrColors[UICOLOR_WINDOW_BACKGROUND][0]            = RGB(239,239,235);
      m_clrColors[UICOLOR_DIALOG_BACKGROUND][0]            = RGB(238,238,238);
      m_clrColors[UICOLOR_DIALOG_TEXT_NORMAL][0]           = RGB(0,0,0);
      m_clrColors[UICOLOR_DIALOG_TEXT_DARK][0]             = RGB(96,96,80);
      m_clrColors[UICOLOR_TITLE_BACKGROUND][0]             = RGB(114,136,172);
      m_clrColors[UICOLOR_TITLE_TEXT][0]                   = RGB(255,255,255);
      m_clrColors[UICOLOR_TITLE_BORDER_LIGHT][0]           = RGB(171,192,231);
      m_clrColors[UICOLOR_TITLE_BORDER_DARK][0]            = RGB(0,55,122);
      m_clrColors[UICOLOR_BUTTON_BACKGROUND_NORMAL][0]     = RGB(250,250,252);
       m_clrColors[UICOLOR_BUTTON_BACKGROUND_NORMAL][1]    = RGB(215,215,227);
      m_clrColors[UICOLOR_BUTTON_BACKGROUND_DISABLED][0]   = RGB(248,248,248);
       m_clrColors[UICOLOR_BUTTON_BACKGROUND_DISABLED][1]  = RGB(214,214,214);
      m_clrColors[UICOLOR_BUTTON_BACKGROUND_PUSHED][0]     = RGB(215,215,227);
       m_clrColors[UICOLOR_BUTTON_BACKGROUND_PUSHED][1]    = RGB(250,250,252);
      m_clrColors[UICOLOR_BUTTON_TEXT_NORMAL][0]           = RGB(0,0,0);
      m_clrColors[UICOLOR_BUTTON_TEXT_PUSHED][0]           = RGB(0,0,20);
      m_clrColors[UICOLOR_BUTTON_TEXT_DISABLED][0]         = RGB(204,204,204);
      m_clrColors[UICOLOR_BUTTON_BORDER_LIGHT][0]          = RGB(123,158,189);
      m_clrColors[UICOLOR_BUTTON_BORDER_DARK][0]           = RGB(123,158,189);
      m_clrColors[UICOLOR_BUTTON_BORDER_DISABLED][0]       = RGB(204,204,204);
      m_clrColors[UICOLOR_BUTTON_BORDER_FOCUS][0]          = RGB(140,140,140);
      m_clrColors[UICOLOR_TOOL_BACKGROUND_NORMAL][0]       = RGB(114,136,172);
      m_clrColors[UICOLOR_TOOL_BACKGROUND_DISABLED][0]     = RGB(100,121,156);
      m_clrColors[UICOLOR_TOOL_BACKGROUND_HOVER][0]        = RGB(100,121,156);
      m_clrColors[UICOLOR_TOOL_BACKGROUND_PUSHED][0]       = RGB(80,101,136);
      m_clrColors[UICOLOR_TOOL_BORDER_NORMAL][0]           = RGB(0,55,122);
      m_clrColors[UICOLOR_TOOL_BORDER_DISABLED][0]         = RGB(0,55,122);
      m_clrColors[UICOLOR_TOOL_BORDER_HOVER][0]            = RGB(0,55,122);
      m_clrColors[UICOLOR_TOOL_BORDER_PUSHED][0]           = RGB(0,55,122);
      m_clrColors[UICOLOR_EDIT_BACKGROUND_DISABLED][0]     = RGB(255,251,255);
      m_clrColors[UICOLOR_EDIT_BACKGROUND_READONLY][0]     = RGB(255,251,255);
      m_clrColors[UICOLOR_EDIT_BACKGROUND_NORMAL][0]       = RGB(255,255,255);
      m_clrColors[UICOLOR_EDIT_BACKGROUND_HOVER][0]        = RGB(255,251,255);
      m_clrColors[UICOLOR_EDIT_TEXT_NORMAL][0]             = RGB(0,0,0);
      m_clrColors[UICOLOR_EDIT_TEXT_DISABLED][0]           = RGB(167,166,170);
      m_clrColors[UICOLOR_EDIT_TEXT_READONLY][0]           = RGB(167,166,170);      
      m_clrColors[UICOLOR_NAVIGATOR_BACKGROUND][0]         = RGB(229,217,213);
       m_clrColors[UICOLOR_NAVIGATOR_BACKGROUND][1]        = RGB(201,199,187);
      m_clrColors[UICOLOR_NAVIGATOR_TEXT_NORMAL][0]        = RGB(102,102,102);
      m_clrColors[UICOLOR_NAVIGATOR_TEXT_SELECTED][0]      = RGB(0,0,0);
      m_clrColors[UICOLOR_NAVIGATOR_TEXT_PUSHED][0]        = RGB(0,0,0);       
      m_clrColors[UICOLOR_NAVIGATOR_BORDER_NORMAL][0]      = RGB(131,133,116);
      m_clrColors[UICOLOR_NAVIGATOR_BORDER_SELECTED][0]    = RGB(159,160,144);
      m_clrColors[UICOLOR_NAVIGATOR_BUTTON_HOVER][0]       = RGB(200,200,200);
      m_clrColors[UICOLOR_NAVIGATOR_BUTTON_PUSHED][0]      = RGB(184,184,183);
      m_clrColors[UICOLOR_NAVIGATOR_BUTTON_SELECTED][0]    = RGB(238,238,238);
      m_clrColors[UICOLOR_TAB_BACKGROUND_NORMAL][0]        = RGB(255,251,255);
      m_clrColors[UICOLOR_TAB_FOLDER_NORMAL][0]            = RGB(255,251,255);
       m_clrColors[UICOLOR_TAB_FOLDER_NORMAL][1]           = RGB(233,231,215);
      m_clrColors[UICOLOR_TAB_FOLDER_SELECTED][0]          = RGB(255,251,255);
      m_clrColors[UICOLOR_TAB_BORDER][0]                   = RGB(148,166,181);
      m_clrColors[UICOLOR_TAB_TEXT_NORMAL][0]              = RGB(0,0,0);
      m_clrColors[UICOLOR_TAB_TEXT_SELECTED][0]            = RGB(0,0,0);
      m_clrColors[UICOLOR_TAB_TEXT_DISABLED][0]            = RGB(0,0,0);      
      m_clrColors[UICOLOR_HEADER_BACKGROUND][0]            = RGB(233,231,215);
       m_clrColors[UICOLOR_HEADER_BACKGROUND][1]           = RGB(150,150,147);
      m_clrColors[UICOLOR_HEADER_BORDER][0]                = RGB(218,219,201);
      m_clrColors[UICOLOR_HEADER_SEPARATOR][0]             = RGB(197,193,177);
      m_clrColors[UICOLOR_HEADER_TEXT][0]                  = RGB(0,0,0);
      m_clrColors[UICOLOR_TASK_BACKGROUND][0]              = RGB(230,243,255);
       m_clrColors[UICOLOR_TASK_BACKGROUND][1]             = RGB(255,255,255);
      m_clrColors[UICOLOR_TASK_BORDER][0]                  = RGB(140,158,198);
      m_clrColors[UICOLOR_TASK_CAPTION][0]                 = RGB(140,158,198);
      m_clrColors[UICOLOR_TASK_TEXT][0]                    = RGB(65,65,110);
      m_clrColors[UICOLOR_LINK_TEXT_NORMAL][0]             = RGB(0,0,255);
      m_clrColors[UICOLOR_LINK_TEXT_HOVER][0]              = RGB(0,0,100);
      m_clrColors[UICOLOR_CONTROL_BACKGROUND_NORMAL][0]    = RGB(255,255,255);
      m_clrColors[UICOLOR_CONTROL_BACKGROUND_SELECTED][0]  = RGB(173,195,231);
      m_clrColors[UICOLOR_CONTROL_BACKGROUND_READONLY][0]  = RGB(255,255,255);
      m_clrColors[UICOLOR_CONTROL_BACKGROUND_DISABLED][0]  = RGB(255,255,255);
      m_clrColors[UICOLOR_CONTROL_BACKGROUND_HOVER][0]     = RGB(233,245,255);
      m_clrColors[UICOLOR_CONTROL_BACKGROUND_SORTED][0]    = RGB(242,242,246);
      m_clrColors[UICOLOR_CONTROL_BACKGROUND_EXPANDED][0]  = RGB(255,255,255);
       m_clrColors[UICOLOR_CONTROL_BACKGROUND_EXPANDED][1] = RGB(236,242,255);
      m_clrColors[UICOLOR_CONTROL_BORDER_NORMAL][0]        = RGB(123,158,189);
      m_clrColors[UICOLOR_CONTROL_BORDER_SELECTED][0]      = RGB(123,158,189);
      m_clrColors[UICOLOR_CONTROL_BORDER_DISABLED][0]      = RGB(204,204,204);
      m_clrColors[UICOLOR_CONTROL_TEXT_NORMAL][0]          = RGB(0,0,0);
      m_clrColors[UICOLOR_CONTROL_TEXT_SELECTED][0]        = RGB(0,0,0);
      m_clrColors[UICOLOR_CONTROL_TEXT_DISABLED][0]        = RGB(204,204,204);
      m_clrColors[UICOLOR_STANDARD_BLACK][0]               = RGB(0,0,0);
      m_clrColors[UICOLOR_STANDARD_YELLOW][0]              = RGB(255,255,204);
      m_clrColors[UICOLOR_STANDARD_RED][0]                 = RGB(255,204,204);
      m_clrColors[UICOLOR_STANDARD_GREY][0]                = RGB(145,146,119);
      m_clrColors[UICOLOR_STANDARD_LIGHTGREY][0]           = RGB(195,196,179);
      m_clrColors[UICOLOR_STANDARD_WHITE][0]               = RGB(255,255,255);

      // Boot Windows Common Controls (for the ToolTip control)
      ::InitCommonControls();

      // We need the image library for effects. It is however optional in Windows so
      // we'll also need to provide a gracefull fallback.
      ::LoadLibrary("msimg32.dll");
   }
   m_szMinWindow.cx = 140;
   m_szMinWindow.cy = 200;
   m_ptLastMousePos.x = m_ptLastMousePos.y = -1;
   //m_uMsgMouseWheel = ::RegisterWindowMessage(MSH_MOUSEWHEEL);
   // System Config
   m_SystemConfig.bShowKeyboardCues = false;
   m_SystemConfig.bScrollLists = false;
   // System Metrics
   m_SystemMetrics.cxvscroll = (INT) ::GetSystemMetrics(SM_CXVSCROLL);
}

CPaintManagerUI::~CPaintManagerUI()
{
   // Delete the control-tree structures
}

void CPaintManagerUI::Init(HWND hWnd)
{
   ASSERT(::IsWindow(hWnd));
   // Remember the window context we came from
   m_hWndPaint = hWnd;
   m_hDcPaint = ::GetDC(hWnd);
   // We'll want to filter messages globally too   
}

HINSTANCE CPaintManagerUI::GetResourceInstance()
{
   return m_hInstance;
}

HINSTANCE CPaintManagerUI::GetLanguageInstance()
{
   return m_hLangInst;
}

void CPaintManagerUI::SetResourceInstance(HINSTANCE hInst)
{
   m_hInstance = hInst;
   if( m_hLangInst == NULL ) m_hLangInst = hInst;
}

void CPaintManagerUI::SetLanguageInstance(HINSTANCE hInst)
{
   m_hLangInst = hInst;
}

HWND CPaintManagerUI::GetPaintWindow() const
{
   return m_hWndPaint;
}

HDC CPaintManagerUI::GetPaintDC() const
{
   return m_hDcPaint;
}

POINT CPaintManagerUI::GetMousePos() const
{
   return m_ptLastMousePos;
}

void CPaintManagerUI::SetMinMaxInfo(int cx, int cy)
{
   ASSERT(cx>=0 && cy>=0);
   m_szMinWindow.cx = cx;
   m_szMinWindow.cy = cy;
}

void CPaintManagerUI::UpdateLayout()
{
   m_bResizeNeeded = true;
   ::InvalidateRect(m_hWndPaint, NULL, FALSE);
}

void CPaintManagerUI::Invalidate(RECT rcItem)
{
   ::InvalidateRect(m_hWndPaint, &rcItem, FALSE);
}


void CPaintManagerUI::MessageLoop()
{
   MSG msg = { 0 };
   while( ::GetMessage(&msg, NULL, 0, 0) ) {
      if( !CPaintManagerUI::TranslateMessage(&msg) ) {
         ::TranslateMessage(&msg);
         ::DispatchMessage(&msg);
      }
   }
}

bool CPaintManagerUI::TranslateMessage(const LPMSG pMsg)
{

   return false;
}





TSystemSettingsUI CPaintManagerUI::GetSystemSettings() const
{
   return m_SystemConfig;
}

void CPaintManagerUI::SetSystemSettings(const TSystemSettingsUI Config)
{
   m_SystemConfig = Config;
}

TSystemMetricsUI CPaintManagerUI::GetSystemMetrics() const
{
   return m_SystemMetrics;
}

HFONT CPaintManagerUI::GetThemeFont(UITYPE_FONT Index) const
{
   if( Index <= UIFONT__FIRST || Index >= UIFONT__LAST ) return NULL;
   if( m_hFonts[Index] == NULL ) m_hFonts[Index] = ::CreateFontIndirect(&m_aLogFonts[Index]);
   return m_hFonts[Index];
}

HICON CPaintManagerUI::GetThemeIcon(int iIndex, int cxySize) const
{
   if( m_himgIcons16 == NULL ) {
      m_himgIcons16 = ImageList_LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ICONS16), 16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
      m_himgIcons24 = ImageList_LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ICONS16), 16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
      m_himgIcons32 = ImageList_LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ICONS16), 16, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
      m_himgIcons50 = ImageList_LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_ICONS50), 50, 0, RGB(255,0,255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
   }
   if( cxySize == 16 ) return ImageList_GetIcon(m_himgIcons16, iIndex, ILD_NORMAL);
   else if( cxySize == 24 ) return ImageList_GetIcon(m_himgIcons24, iIndex, ILD_NORMAL);
   else if( cxySize == 32 ) return ImageList_GetIcon(m_himgIcons32, iIndex, ILD_NORMAL);
   else if( cxySize == 50 ) return ImageList_GetIcon(m_himgIcons50, iIndex, ILD_NORMAL);
   return NULL;
}

HPEN CPaintManagerUI::GetThemePen(UITYPE_COLOR Index) const
{
   if( Index <= UICOLOR__FIRST || Index >= UICOLOR__LAST ) return NULL;
   if( m_hPens[Index] == NULL ) m_hPens[Index] = ::CreatePen(PS_SOLID, 1, m_clrColors[Index][0]);
   return m_hPens[Index];
}

HBRUSH CPaintManagerUI::GetThemeBrush(UITYPE_COLOR Index) const
{
   if( Index <= UICOLOR__FIRST || Index >= UICOLOR__LAST ) return NULL;
   if( m_hBrushes[Index] == NULL ) m_hBrushes[Index] = ::CreateSolidBrush(m_clrColors[Index][0]);
   return m_hBrushes[Index];
}

const TEXTMETRIC& CPaintManagerUI::GetThemeFontInfo(UITYPE_FONT Index) const
{
   if( Index <= UIFONT__FIRST || Index >= UIFONT__LAST ) return m_aTextMetrics[0];
   if( m_aTextMetrics[Index].tmHeight == 0 ) {
      HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaint, GetThemeFont(Index));
      ::GetTextMetrics(m_hDcPaint, &m_aTextMetrics[Index]);
      ::SelectObject(m_hDcPaint, hOldFont);
   }
   return m_aTextMetrics[Index];
}

COLORREF CPaintManagerUI::GetThemeColor(UITYPE_COLOR Index) const
{
   if( Index <= UICOLOR__FIRST || Index >= UICOLOR__LAST ) return RGB(0,0,0);
   return m_clrColors[Index][0];
}

bool CPaintManagerUI::GetThemeColorPair(UITYPE_COLOR Index, COLORREF& clr1, COLORREF& clr2) const
{
   if( Index <= UICOLOR__FIRST || Index >= UICOLOR__LAST ) return false;
   clr1 = m_clrColors[Index][0];
   clr2 = m_clrColors[Index][1];
   return true;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CRenderClip::~CRenderClip()
{
   ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
   ASSERT(::GetObjectType(hRgn)==OBJ_REGION);
   ASSERT(::GetObjectType(hOldRgn)==OBJ_REGION);
   ::SelectClipRgn(hDC, hOldRgn);
   ::DeleteObject(hOldRgn);
   ::DeleteObject(hRgn);
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

void CBlueRenderEngineUI::GenerateClip(HDC hDC, RECT rcItem, CRenderClip& clip)
{
   RECT rcClip = { 0 };
   ::GetClipBox(hDC, &rcClip);
   clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
   clip.hRgn = ::CreateRectRgnIndirect(&rcItem);
   ::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
   clip.hDC = hDC;
   clip.rcItem = rcItem;
}

void CBlueRenderEngineUI::DoFillRect(HDC hDC, CPaintManagerUI* pManager, RECT rcItem, UITYPE_COLOR Color)
{
   DoFillRect(hDC, pManager, rcItem, pManager->GetThemeColor(Color));
}

void CBlueRenderEngineUI::DoFillRect(HDC hDC, CPaintManagerUI* pManager, RECT rcItem, COLORREF clrFill)
{
   ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
   ::SetBkColor(hDC, clrFill);
   ::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rcItem, NULL, 0, NULL);
}

void CBlueRenderEngineUI::DoPaintLine(HDC hDC, CPaintManagerUI* pManager, RECT rcItem, UITYPE_COLOR Color)
{
   ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
   POINT ptTemp = { 0 };
   ::SelectObject(hDC, pManager->GetThemePen(Color));
   ::MoveToEx(hDC, rcItem.left, rcItem.top, &ptTemp);
   ::LineTo(hDC, rcItem.right, rcItem.bottom);
}

void CBlueRenderEngineUI::DoPaintRectangle(HDC hDC, CPaintManagerUI* pManager, RECT rcItem, UITYPE_COLOR Border, UITYPE_COLOR Fill)
{
   ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
   ::SelectObject(hDC, pManager->GetThemePen(Border));
   ::SelectObject(hDC, Fill == UICOLOR__INVALID ? ::GetStockObject(HOLLOW_BRUSH) : pManager->GetThemeBrush(Fill));
   ::Rectangle(hDC, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);
}

void CBlueRenderEngineUI::DoPaintPanel(HDC hDC, CPaintManagerUI* pManager, RECT rcItem)
{
   DoPaintFrame(hDC, pManager, rcItem, UICOLOR_TITLE_BORDER_LIGHT, UICOLOR_TITLE_BORDER_DARK, UICOLOR_TITLE_BACKGROUND);
}

void CBlueRenderEngineUI::DoPaintFrame(HDC hDC, CPaintManagerUI* pManager, RECT rcItem, UITYPE_COLOR Light, UITYPE_COLOR Dark, UITYPE_COLOR Background, UINT uStyle)
{
   if( Background != UICOLOR__INVALID ) {
      DoFillRect(hDC, pManager, rcItem, Background);
   }
   if( (uStyle & UIFRAME_ROUND) != 0 )
   {
      POINT ptTemp;
      ::SelectObject(hDC, pManager->GetThemePen(Light));
      ::MoveToEx(hDC, rcItem.left, rcItem.bottom - 2, &ptTemp);
      ::LineTo(hDC, rcItem.left, rcItem.top + 1);
      ::LineTo(hDC, rcItem.left + 1, rcItem.top);
      ::LineTo(hDC, rcItem.right - 2, rcItem.top);
      ::SelectObject(hDC, pManager->GetThemePen(Dark));
      ::LineTo(hDC, rcItem.right - 1, rcItem.top + 1);
      ::LineTo(hDC, rcItem.right - 1, rcItem.bottom - 2);
      ::LineTo(hDC, rcItem.right - 2, rcItem.bottom - 1);
      ::LineTo(hDC, rcItem.left, rcItem.bottom - 1);
   }
   if( (uStyle & UIFRAME_FOCUS) != 0 )
   {
      HPEN hPen = ::CreatePen(PS_DOT, 1, pManager->GetThemeColor(UICOLOR_BUTTON_BORDER_FOCUS));
      HPEN hOldPen = (HPEN) ::SelectObject(hDC, hPen);
      POINT ptTemp;
      ::MoveToEx(hDC, rcItem.left, rcItem.bottom - 1, &ptTemp);
      ::LineTo(hDC, rcItem.left, rcItem.top);
      ::LineTo(hDC, rcItem.right - 1, rcItem.top);
      ::LineTo(hDC, rcItem.right - 1, rcItem.bottom - 1);
      ::LineTo(hDC, rcItem.left, rcItem.bottom - 1);
      ::SelectObject(hDC, hOldPen);
      ::DeleteObject(hPen);
   }
   else
   {
      POINT ptTemp;
      ::SelectObject(hDC, pManager->GetThemePen(Light));
      ::MoveToEx(hDC, rcItem.left, rcItem.bottom - 1, &ptTemp);
      ::LineTo(hDC, rcItem.left, rcItem.top);
      ::LineTo(hDC, rcItem.right - 1, rcItem.top);
      ::SelectObject(hDC, pManager->GetThemePen(Dark));
      ::LineTo(hDC, rcItem.right - 1, rcItem.bottom - 1);
      ::LineTo(hDC, rcItem.left, rcItem.bottom - 1);
   }
}

void CBlueRenderEngineUI::DoPaintBitmap(HDC hDC, CPaintManagerUI* pManager, HBITMAP hBmp, RECT rcBitmap)
{
   ASSERT(::GetObjectType(hBmp)==OBJ_BITMAP);
   ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
   // Paint image
   HDC hdcBmp = ::CreateCompatibleDC(hDC);
   HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hdcBmp, hBmp);
   ::BitBlt(hDC, rcBitmap.left, rcBitmap.top, rcBitmap.right - rcBitmap.left, rcBitmap.bottom - rcBitmap.top, hdcBmp, 0, 0, SRCCOPY);
   ::SelectObject(hdcBmp, hOldBitmap);
}

void CBlueRenderEngineUI::DoPaintArcCaption(HDC hDC, CPaintManagerUI* pManager, RECT rc, LPCTSTR pstrText, UINT uStyle)
{
   CRenderClip clip;
   GenerateClip(hDC, rc, clip);

   ::SelectObject(hDC, pManager->GetThemePen(UICOLOR_TASK_CAPTION));
   ::SelectObject(hDC, pManager->GetThemeBrush(UICOLOR_TASK_CAPTION));
   ::RoundRect(hDC, rc.left, rc.top, rc.right, rc.bottom + 15, 5, 5);

   RECT rcText = { rc.left, rc.top + 3, rc.right, rc.bottom };

   if( (uStyle & UIARC_GRIPPER) != 0 ) {
      RECT rcButton1 = { rc.left + 10, rc.top + 4, rc.left + 14, rc.top + 7 };
      DoPaintFrame(hDC, pManager, rcButton1, UICOLOR_TITLE_BORDER_LIGHT, UICOLOR_TITLE_BORDER_DARK, UICOLOR__INVALID, 0);
      RECT rcButton2 = { rc.left + 6, rc.top + 8, rc.left + 10, rc.top + 11 };
      DoPaintFrame(hDC, pManager, rcButton2, UICOLOR_TITLE_BORDER_LIGHT, UICOLOR_TITLE_BORDER_DARK, UICOLOR__INVALID, 0);
      RECT rcButton3 = { rc.left + 10, rc.top + 12, rc.left + 14, rc.top + 15 };
      DoPaintFrame(hDC, pManager, rcButton3, UICOLOR_TITLE_BORDER_LIGHT, UICOLOR_TITLE_BORDER_DARK, UICOLOR__INVALID, 0);
      rcText.left += 12;
   }
   rcText.left += 8;

   if( pstrText != NULL && _tcslen(pstrText) > 0 ) {
      int nLinks = 0;
      DoPaintPrettyText(hDC, pManager, rcText, pstrText, UICOLOR_TITLE_TEXT, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE);
   }
}

void CBlueRenderEngineUI::DoPaintButton(HDC hDC, CPaintManagerUI* pManager, RECT rc, LPCTSTR pstrText, RECT rcPadding, UINT uState, UINT uStyle)
{
   ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
   // Draw focus rectangle
   if( ((uState & UISTATE_FOCUSED) != 0) && pManager->GetSystemSettings().bShowKeyboardCues ) {
      CBlueRenderEngineUI::DoPaintFrame(hDC, pManager, rc, UICOLOR_BUTTON_BORDER_FOCUS, UICOLOR_BUTTON_BORDER_FOCUS, UICOLOR__INVALID, UIFRAME_ROUND);
      ::InflateRect(&rc, -1, -1);
   }
   // Draw frame and body
   COLORREF clrColor1, clrColor2;
   UITYPE_COLOR clrBorder1, clrBorder2, clrText, clrBack;
   if( (uState & UISTATE_DISABLED) != 0 ) {
      clrBorder1 = UICOLOR_BUTTON_BORDER_DISABLED;
      clrBorder2 = UICOLOR_BUTTON_BORDER_DISABLED;
      clrText = UICOLOR_BUTTON_TEXT_DISABLED;
      clrBack = UICOLOR_BUTTON_BACKGROUND_DISABLED;
   }
   else if( (uState & UISTATE_PUSHED) != 0 ) {
      clrBorder1 = UICOLOR_BUTTON_BORDER_DARK;
      clrBorder2 = UICOLOR_BUTTON_BORDER_LIGHT;
      clrText = UICOLOR_BUTTON_TEXT_PUSHED;
      clrBack = UICOLOR_BUTTON_BACKGROUND_PUSHED;
   }
   else {
      clrBorder1 = UICOLOR_BUTTON_BORDER_LIGHT;
      clrBorder2 = UICOLOR_BUTTON_BORDER_DARK;
      clrText = UICOLOR_BUTTON_TEXT_NORMAL;
      clrBack = UICOLOR_BUTTON_BACKGROUND_NORMAL;
   }
   // Draw button
   DoPaintFrame(hDC, pManager, rc, clrBorder1, clrBorder2, UICOLOR__INVALID, UIFRAME_ROUND);
   ::InflateRect(&rc, -1, -1);
   // The pushed button has an inner light shade
   if( (uState & UISTATE_PUSHED) != 0 ) {
      DoPaintFrame(hDC, pManager, rc, UICOLOR_STANDARD_LIGHTGREY, UICOLOR_STANDARD_LIGHTGREY, UICOLOR__INVALID);
      rc.top += 1;
      rc.left += 1;
   }
   // Gradient background
   pManager->GetThemeColorPair(clrBack, clrColor1, clrColor2);
   DoPaintGradient(hDC, pManager, rc, clrColor1, clrColor2, true, 32);
   // Draw hightlight inside button
   ::SelectObject(hDC, pManager->GetThemePen(UICOLOR_DIALOG_BACKGROUND));
   POINT ptTemp;
   ::MoveToEx(hDC, rc.left, rc.top, &ptTemp);
   ::LineTo(hDC, rc.left, rc.bottom - 1);
   ::LineTo(hDC, rc.right - 2, rc.bottom - 1);
   ::LineTo(hDC, rc.right - 2, rc.top);
   // Draw text
   RECT rcText = rc;
   ::InflateRect(&rcText, -1, -1);
   rcText.left += rcPadding.left;
   rcText.top += rcPadding.top;
   rcText.right -= rcPadding.right;
   rcText.bottom -= rcPadding.bottom;
   int nLinks = 0;
   DoPaintPrettyText(hDC, pManager, rcText, pstrText, clrText, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | uStyle);
}

void CBlueRenderEngineUI::DoPaintEditBox(HDC hDC, CPaintManagerUI* pManager, RECT rcItem, LPCTSTR pstrText, UINT uState, UINT uDrawStyle, bool bPaintFrameOnly)
{
   ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
   if( (uState & UISTATE_DISABLED) != 0 ) {
      DoPaintFrame(hDC, pManager, rcItem, UICOLOR_CONTROL_BORDER_DISABLED, UICOLOR_CONTROL_BORDER_DISABLED, UICOLOR_EDIT_BACKGROUND_DISABLED);
   }
   else if( (uState & UISTATE_READONLY) != 0 ) {
      DoPaintFrame(hDC, pManager, rcItem, UICOLOR_CONTROL_BORDER_DISABLED, UICOLOR_CONTROL_BORDER_DISABLED, UICOLOR_EDIT_BACKGROUND_READONLY);
   }
   else {
      DoPaintFrame(hDC, pManager, rcItem, UICOLOR_CONTROL_BORDER_NORMAL, UICOLOR_CONTROL_BORDER_NORMAL, UICOLOR_EDIT_BACKGROUND_NORMAL);
   }
   if( bPaintFrameOnly ) return;
   // We should also draw the actual text
   COLORREF clrText = pManager->GetThemeColor(UICOLOR_EDIT_TEXT_NORMAL);
   if( (uState & UISTATE_READONLY) != 0 ) clrText = pManager->GetThemeColor(UICOLOR_EDIT_TEXT_READONLY);
   if( (uState & UISTATE_DISABLED) != 0 ) clrText = pManager->GetThemeColor(UICOLOR_EDIT_TEXT_DISABLED);
   ::SetBkMode(hDC, TRANSPARENT);
   ::SetTextColor(hDC, clrText);
   ::SelectObject(hDC, pManager->GetThemeFont(UIFONT_NORMAL));
   RECT rcEdit = rcItem;
   ::InflateRect(&rcEdit, -3, -2);
   ::DrawText(hDC, pstrText, -1, &rcEdit, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_EDITCONTROL | uDrawStyle);
}

void CBlueRenderEngineUI::DoPaintOptionBox(HDC hDC, CPaintManagerUI* pManager, RECT rcItem, LPCTSTR pstrText, UINT uState, UINT uStyle)
{
   ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
   // Determine placement of elements
   RECT rcText = rcItem;
   RECT rcButton = rcItem;
   if( (uStyle & DT_RIGHT) != 0 ) {
      rcText.right -= 18;
      rcButton.left = rcButton.right - 18;
   }
   else {
      rcText.left += 18;
      rcButton.right = rcButton.left + 18;
   }
   bool bSelected = (uState & UISTATE_CHECKED) != 0;
   int iIcon = bSelected ? 8 : 9;
   if( (uState & UISTATE_PUSHED) != 0 ) iIcon = 10;
   if( (uState & UISTATE_DISABLED) != 0 ) iIcon = bSelected ? 10 : 11;
   HICON hIcon = pManager->GetThemeIcon(iIcon, 16);
   ::DrawIconEx(hDC, rcButton.left, rcButton.top, hIcon, 16, 16, 0, NULL, DI_NORMAL);
   ::DestroyIcon(hIcon);
   // Paint text
   UITYPE_COLOR iTextColor = ((uState & UISTATE_DISABLED) != 0) ? UICOLOR_EDIT_TEXT_DISABLED : UICOLOR_EDIT_TEXT_NORMAL;
   int nLinks = 0;
   CBlueRenderEngineUI::DoPaintPrettyText(hDC, pManager, rcText, pstrText, iTextColor, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE);
   // Paint focus rectangle
   if( ((uState & UISTATE_FOCUSED) != 0) && pManager->GetSystemSettings().bShowKeyboardCues ) {
      RECT rcFocus = { 0, 0, 9999, 9999 };;
      int nLinks = 0;
      CBlueRenderEngineUI::DoPaintPrettyText(hDC, pManager, rcFocus, pstrText, iTextColor, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | DT_CALCRECT);
      rcText.right = rcText.left + (rcFocus.right - rcFocus.left);
      rcText.bottom = rcText.top + (rcFocus.bottom - rcFocus.top);
      ::InflateRect(&rcText, 2, 0);
      CBlueRenderEngineUI::DoPaintFrame(hDC, pManager, rcText, UICOLOR_STANDARD_BLACK, UICOLOR_STANDARD_BLACK, UICOLOR__INVALID, UIFRAME_FOCUS);
   }
}

void CBlueRenderEngineUI::DoPaintTabFolder(HDC hDC, CPaintManagerUI* pManager, RECT& rcItem, LPCTSTR pstrText, UINT uState)
{
   ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
   ::SetBkMode(hDC, TRANSPARENT);
   ::SelectObject(hDC, pManager->GetThemeFont(UIFONT_NORMAL));
   CStdString sText = pstrText;
   sText.ProcessResourceTokens();
   int cchText = sText.GetLength();
   SIZE szText = { 0 };
   ::GetTextExtentPoint32(hDC, sText, cchText, &szText);
   RECT rcTab = { 0 };
   if( (uState & UISTATE_PUSHED) != 0 ) 
   {
      ::SetRect(&rcTab, rcItem.left, rcItem.top + 1, rcItem.left + szText.cx + 14, rcItem.bottom);
      DoFillRect(hDC, pManager, rcTab, UICOLOR_TAB_BACKGROUND_NORMAL);
      
      ::SelectObject(hDC, pManager->GetThemePen(UICOLOR_TAB_BORDER));
      POINT ptTemp;
      ::MoveToEx(hDC, rcTab.left, rcTab.bottom, &ptTemp);
      ::LineTo(hDC, rcTab.left, rcTab.top + 2);
      ::LineTo(hDC, rcTab.left + 1, rcTab.top + 1);
      ::LineTo(hDC, rcTab.right - 1, rcTab.top + 1);
      ::LineTo(hDC, rcTab.right, rcTab.top + 2);
      ::LineTo(hDC, rcTab.right, rcTab.bottom);
      ::SelectObject(hDC, pManager->GetThemePen(UICOLOR_TAB_BACKGROUND_NORMAL));
      ::LineTo(hDC, rcTab.left, rcTab.bottom);

      RECT rcText = { rcTab.left, rcTab.top + 1, rcTab.right, rcTab.bottom };
      int nLinks = 0;
      DoPaintPrettyText(hDC, pManager, rcText, pstrText, UICOLOR_TAB_TEXT_SELECTED, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

      RECT rcTop = { rcTab.left + 1, rcTab.top, rcTab.right - 1, rcTab.top + 3 };
      DoPaintGradient(hDC, pManager, rcTop, RGB(222,142,41), RGB(255,199,25), true, 4);
   }
   else if( (uState & UISTATE_DISABLED) != 0 )
   {
   }
   else
   {
      ::SetRect(&rcTab, rcItem.left, rcItem.top + 3, rcItem.left + szText.cx + 12, rcItem.bottom);
      
      COLORREF clrFolder1, clrFolder2;
      pManager->GetThemeColorPair(UICOLOR_TAB_FOLDER_NORMAL, clrFolder1, clrFolder2);
      DoPaintGradient(hDC, pManager, rcTab, clrFolder1, clrFolder2, true, 32);

      ::SelectObject(hDC, pManager->GetThemePen(UICOLOR_TAB_BORDER));
      POINT ptTemp;
      ::MoveToEx(hDC, rcTab.left, rcTab.bottom, &ptTemp);
      ::LineTo(hDC, rcTab.left, rcTab.top);
      ::LineTo(hDC, rcTab.right, rcTab.top);
      ::LineTo(hDC, rcTab.right, rcTab.bottom);

      RECT rcText = { rcTab.left, rcTab.top + 3, rcTab.right, rcTab.bottom };
      int nLinks = 0;
      DoPaintPrettyText(hDC, pManager, rcText, pstrText, UICOLOR_TAB_TEXT_NORMAL, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
   }
   rcItem = rcTab;
}

void CBlueRenderEngineUI::DoPaintToolbarButton(HDC hDC, CPaintManagerUI* pManager, RECT rc, LPCTSTR pstrText, SIZE szPadding, UINT uState)
{
   ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
   if( (uState & UISTATE_PUSHED) != 0 ) {
      DoPaintFrame(hDC, pManager, rc, UICOLOR_TOOL_BORDER_PUSHED, UICOLOR_TOOL_BORDER_PUSHED, UICOLOR_TOOL_BACKGROUND_PUSHED, 0);
      rc.top += 2;
      rc.left++;
   }
   else if( (uState & UISTATE_HOT) != 0 ) {
      DoPaintFrame(hDC, pManager, rc, UICOLOR_TOOL_BORDER_HOVER, UICOLOR_TOOL_BORDER_HOVER, UICOLOR_TOOL_BACKGROUND_HOVER, 0);
   }
   else if( (uState & UISTATE_DISABLED) != 0 ) {
      // TODO
   }
   RECT rcText = rc;
   int nLinks = 0;
   ::InflateRect(&rcText, -szPadding.cx, -szPadding.cy);
   DoPaintPrettyText(hDC, pManager, rcText, pstrText, UICOLOR_TITLE_TEXT, UICOLOR__INVALID, NULL, nLinks, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
}

void CBlueRenderEngineUI::DoPaintQuickText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCSTR pstrText, UITYPE_COLOR iTextColor, UITYPE_FONT iFont, UINT uStyle)
{
   ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
   ::SetBkMode(hDC, TRANSPARENT);
   ::SetTextColor(hDC, pManager->GetThemeColor(iTextColor));
   ::SelectObject(hDC, pManager->GetThemeFont(iFont));
   ::DrawText(hDC, pstrText, -1, &rc, DT_SINGLELINE);
}

void CBlueRenderEngineUI::DoPaintPrettyText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, UITYPE_COLOR iTextColor, UITYPE_COLOR iBackColor, RECT* prcLinks, int& nLinkRects, UINT uStyle)
{
   ASSERT(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);

   // The string formatter supports a kind of "mini-html" that consists of various short tags:
   //
   //   Link:             <a>text</a>
   //   Change font:      <f x>        where x = font id
   //   Bold:             <b>text</b>
   //   Indent:           <x i>        where i = indent in pixels
   //   Paragraph:        <p>
   //   Horizontal line:  <h>
   //   Icon:             <i x y>      where x = icon id and (optional) y = size (16/32/50)
   //                     <i x>        where x = icon resource name
   //   Color:            <c #xxxxxx>  where x = RGB in hex
   //                     <c x>        where x = color id
   //
   // In addition the standard string resource formats apply:
   //
   //   %{n}                           where n = resource-string-id
   //

   if( ::IsRectEmpty(&rc) ) return;

   bool bDraw = (uStyle & DT_CALCRECT) == 0;

   RECT rcClip = { 0 };
   ::GetClipBox(hDC, &rcClip);
   HRGN hOldRgn = ::CreateRectRgnIndirect(&rcClip);
   HRGN hRgn = ::CreateRectRgnIndirect(&rc);
   if( bDraw ) ::ExtSelectClipRgn(hDC, hRgn, RGN_AND);

   CStdString sText = pstrText;
   sText.ProcessResourceTokens();
   pstrText = sText;

   HFONT hOldFont = (HFONT) ::SelectObject(hDC, pManager->GetThemeFont(UIFONT_NORMAL));
   ::SetBkMode(hDC, TRANSPARENT);
   ::SetTextColor(hDC, pManager->GetThemeColor(iTextColor));

   // If the drawstyle includes an alignment, we'll need to first determine the text-size so
   // we can draw it at the correct position...
   if( (uStyle & DT_SINGLELINE) != 0 && (uStyle & DT_VCENTER) != 0 && (uStyle & DT_CALCRECT) == 0 ) {
      RECT rcText = { 0, 0, 9999, 100 };
      int nLinks = 0;
      DoPaintPrettyText(hDC, pManager, rcText, pstrText, iTextColor, iBackColor, NULL, nLinks, uStyle | DT_CALCRECT);
      rc.top = rc.top + ((rc.bottom - rc.top) / 2) - ((rcText.bottom - rcText.top) / 2);
      rc.bottom = rc.top + (rcText.bottom - rcText.top);
   }
   if( (uStyle & DT_SINGLELINE) != 0 && (uStyle & DT_CENTER) != 0 && (uStyle & DT_CALCRECT) == 0 ) {
      RECT rcText = { 0, 0, 9999, 100 };
      int nLinks = 0;
      DoPaintPrettyText(hDC, pManager, rcText, pstrText, iTextColor, iBackColor, NULL, nLinks, uStyle | DT_CALCRECT);
      ::OffsetRect(&rc, (rc.right - rc.left) / 2 - (rcText.right - rcText.left) / 2, 0);
   }
   if( (uStyle & DT_SINGLELINE) != 0 && (uStyle & DT_RIGHT) != 0 && (uStyle & DT_CALCRECT) == 0 ) {
      RECT rcText = { 0, 0, 9999, 100 };
      int nLinks = 0;
      DoPaintPrettyText(hDC, pManager, rcText, pstrText, iTextColor, iBackColor, NULL, nLinks, uStyle | DT_CALCRECT);
      rc.left = rc.right - (rcText.right - rcText.left);
   }

   // Paint backhground
   if( iBackColor != UICOLOR__INVALID ) DoFillRect(hDC, pManager, rc, iBackColor);

   // Determine if we're hovering over a link, because we would like to
   // indicate it by coloring the link text.
   // BUG: This assumes that the prcLink has already been filled once with
   //      link coordinates! That is usually not the case at first repaint. We'll clear
   //      the remanining entries at exit.
   int i;
   bool bHoverLink = false;
   POINT ptMouse = pManager->GetMousePos();
   for( i = 0; !bHoverLink && i < nLinkRects; i++ ) {
      if( ::PtInRect(prcLinks + i, ptMouse) ) bHoverLink = true;
   }

   TEXTMETRIC tm = pManager->GetThemeFontInfo(UIFONT_NORMAL);
   POINT pt = { rc.left, rc.top };
   int iLineIndent = 0;
   int iLinkIndex = 0;
   int cyLine = tm.tmHeight + tm.tmExternalLeading;
   int cyMinHeight = 0;
   POINT ptLinkStart = { 0 };
   bool bInLink = false;

   while( *pstrText != '\0' ) 
   {
      if( pt.x >= rc.right || *pstrText == '\n' ) 
      {
         // A new link was detected/requested. We'll adjust the line height
         // for the next line and expand the link hitbox (if any)
         if( bInLink && iLinkIndex < nLinkRects) ::SetRect(&prcLinks[iLinkIndex++], ptLinkStart.x, ptLinkStart.y, pt.x, pt.y + tm.tmHeight);
         if( (uStyle & DT_SINGLELINE) != 0 ) break;
         if( *pstrText == '\n' ) pstrText++;
         pt.x = rc.left + iLineIndent;
         pt.y += cyLine - tm.tmDescent;
         ptLinkStart = pt;
         cyLine = tm.tmHeight + tm.tmExternalLeading;
         if( pt.x >= rc.right ) break;
         while( *pstrText == ' ' ) pstrText++;
      }
      else if( *pstrText == '<' 
               && (pstrText[1] >= 'a' && pstrText[1] <= 'z')
               && (pstrText[2] == ' ' || pstrText[2] == '>') )
      {
         pstrText++;
         switch( *pstrText++ )
         {
         case 'a':  // Link
            {
               ::SetTextColor(hDC, pManager->GetThemeColor(bHoverLink ? UICOLOR_LINK_TEXT_HOVER : UICOLOR_LINK_TEXT_NORMAL));
               ::SelectObject(hDC, pManager->GetThemeFont(UIFONT_LINK));
               tm = pManager->GetThemeFontInfo(UIFONT_LINK);
               cyLine = MAX(cyLine, tm.tmHeight + tm.tmExternalLeading);
               ptLinkStart = pt;
               bInLink = true;
            }
            break;
         case 'f':  // Font
            {
               UITYPE_FONT iFont = (UITYPE_FONT) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
               ::SelectObject(hDC, pManager->GetThemeFont(iFont));
               tm = pManager->GetThemeFontInfo(iFont);
               cyLine = MAX(cyLine, tm.tmHeight + tm.tmExternalLeading);
            }
            break;
         case 'b':  // Bold text
            {
               ::SelectObject(hDC, pManager->GetThemeFont(UIFONT_BOLD));
               tm = pManager->GetThemeFontInfo(UIFONT_BOLD);
               cyLine = MAX(cyLine, tm.tmHeight + tm.tmExternalLeading);
            }
            break;
         case 'x':  // Indent
            {
               iLineIndent = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
               if( pt.x < rc.left + iLineIndent ) pt.x = rc.left + iLineIndent;
            }
            break;
         case 'p':  // Paragraph
            {
               pt.x = rc.right;
               cyLine = MAX(cyLine, tm.tmHeight + tm.tmExternalLeading) + 5;
               iLineIndent = 0;
               ::SelectObject(hDC, pManager->GetThemeFont(UIFONT_NORMAL));
               ::SetTextColor(hDC, pManager->GetThemeColor(iTextColor));
               tm = pManager->GetThemeFontInfo(UIFONT_NORMAL);
            }
            break;
         case 'h':  // Horizontal line
            {
               ::SelectObject(hDC, pManager->GetThemePen(UICOLOR_STANDARD_GREY));
               if( bDraw ) {
                  POINT ptTemp = { 0 };
                  ::MoveToEx(hDC, pt.x, pt.y + 5, &ptTemp);
                  ::LineTo(hDC, rc.right - iLineIndent, pt.y + 5);
               }
               cyLine = 12;
            }
            break;
         case 'i':  // Icon
            {
               int iSize = 16;
               if( *pstrText == ' ' ) pstrText++;
               if( isdigit(*pstrText) ) {
                  int iIndex = (int) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
                  iSize = MAX(16, _ttoi(pstrText));
                  if( bDraw ) {
                     HICON hIcon = pManager->GetThemeIcon(iIndex, iSize);
                     ASSERT(hIcon!=NULL);
                     ::DrawIconEx(hDC, pt.x, pt.y, hIcon, iSize, iSize, 0, NULL, DI_NORMAL);
                     ::DestroyIcon(hIcon);
                  }
               }
               else {
                  if( *pstrText == ' ' ) pstrText++;
                  CStdString sRes;
                  while( _istalnum(*pstrText) || *pstrText == '.' || *pstrText == '_' ) sRes += *pstrText++;
                  HICON hIcon = (HICON) ::LoadImage(pManager->GetResourceInstance(), sRes, IMAGE_ICON, 0, 0, LR_LOADTRANSPARENT | LR_SHARED);
                  ASSERT(hIcon!=NULL);
                  ICONINFO ii = { 0 };
                  ::GetIconInfo(hIcon, &ii);
                  BITMAP bi = { 0 };
                  ::GetObject(ii.hbmColor, sizeof(BITMAP), &bi);
                  iSize = bi.bmWidth;
                  if( bDraw ) ::DrawIconEx(hDC, pt.x, pt.y, hIcon, iSize, iSize, 0, NULL, DI_NORMAL);
                  ::DestroyIcon(hIcon);
               }
               // A special feature with an icon at the left edge is that it also sets
               // the paragraph indent.
               if( pt.x == rc.left ) iLineIndent = iSize + (iSize / 8); else cyLine = MAX(iSize, cyLine);
               pt.x += iSize + (iSize / 8);
               cyMinHeight = pt.y + iSize;
            }
            break;
         case 'c':  // Color
            {
               if( *pstrText == ' ' ) pstrText++;
               if( *pstrText == '#') {
                  pstrText++;
                  COLORREF clrColor = _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 16);
                  clrColor = RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor));
                  ::SetTextColor(hDC, clrColor);
               }
               else {
                  UITYPE_COLOR Color = (UITYPE_COLOR) _tcstol(pstrText, const_cast<LPTSTR*>(&pstrText), 10);
                  ::SetTextColor(hDC, pManager->GetThemeColor(Color));
               }
            }
            break;
         }
         while( *pstrText != '\0' && *pstrText != '>' ) pstrText++;
         pstrText++;
      }
      else if( *pstrText == '<' && pstrText[1] == '/' )
      {
         pstrText += 2;
         switch( *pstrText++ )
         {
         case 'a':
            if( iLinkIndex < nLinkRects ) ::SetRect(&prcLinks[iLinkIndex++], ptLinkStart.x, ptLinkStart.y, pt.x, pt.y + tm.tmHeight + tm.tmExternalLeading);
            ::SetTextColor(hDC, pManager->GetThemeColor(iTextColor));
            ::SelectObject(hDC, pManager->GetThemeFont(UIFONT_NORMAL));
            tm = pManager->GetThemeFontInfo(UIFONT_NORMAL);
            bInLink = false;
            break;
         case 'f':
         case 'b':
            // TODO: Use a context stack instead
            ::SelectObject(hDC, pManager->GetThemeFont(UIFONT_NORMAL));
            tm = pManager->GetThemeFontInfo(UIFONT_NORMAL);
            break;
         case 'c':
            ::SetTextColor(hDC, pManager->GetThemeColor(iTextColor));
            break;
         }
         while( *pstrText != '\0' && *pstrText != '>' ) pstrText++;
         pstrText++;
      }
      else if( *pstrText == '&' )
      {
         if( (uStyle & DT_NOPREFIX) == 0 ) {
            if( bDraw  && pManager->GetSystemSettings().bShowKeyboardCues ) ::TextOut(hDC, pt.x, pt.y, _T("_"), 1);
         }
         else {
            SIZE szChar = { 0 };
            ::GetTextExtentPoint32(hDC, _T("&"), 1, &szChar);
            if( bDraw ) ::TextOut(hDC, pt.x, pt.y, _T("&"), 1);
            pt.x += szChar.cx;
         }
         pstrText++;
      }
      else if( *pstrText == ' ' )
      {
         SIZE szSpace = { 0 };
         ::GetTextExtentPoint32(hDC, _T(" "), 1, &szSpace);
         // Still need to paint the space because the font might have
         // underline formatting.
         if( bDraw ) ::TextOut(hDC, pt.x, pt.y, _T(" "), 1);
         pt.x += szSpace.cx;
         pstrText++;
      }
      else
      {
         POINT ptPos = pt;
         int cchChars = 0;
         int cchLastGoodWord = 0;
         LPCTSTR p = pstrText;
         SIZE szText = { 0 };
         if( *p == '<' ) p++, cchChars++;
         while( *p != '\0' && *p != '<' && *p != '\n' && *p != '&' ) {
            // This part makes sure that we're word-wrapping if needed or providing support
            // for DT_END_ELLIPSIS. Unfortunately the GetTextExtentPoint32() call is pretty
            // slow when repeated so often.
            // TODO: Rewrite and use GetTextExtentExPoint() instead!
            cchChars++;
            szText.cx = cchChars * tm.tmMaxCharWidth;
            if( pt.x + szText.cx >= rc.right ) {
               ::GetTextExtentPoint32(hDC, pstrText, cchChars, &szText);
            }
            if( pt.x + szText.cx >= rc.right ) {
               if( (uStyle & DT_WORDBREAK) != 0 && cchLastGoodWord > 0 ) {
                  cchChars = cchLastGoodWord;
                  pt.x = rc.right;
               }
               if( (uStyle & DT_END_ELLIPSIS) != 0 && cchChars > 2 ) {
                  cchChars -= 2;
                  pt.x = rc.right;
               }
               break;
            }
            if( *p == ' ' ) cchLastGoodWord = cchChars;
            p = ::CharNext(p);
         }
         if( cchChars > 0 ) {
            ::GetTextExtentPoint32(hDC, pstrText, cchChars, &szText);
            if( bDraw ) {
               ::TextOut(hDC, ptPos.x, ptPos.y, pstrText, cchChars);
               if( pt.x == rc.right && (uStyle & DT_END_ELLIPSIS) != 0 ) ::TextOut(hDC, rc.right - 10, ptPos.y, _T("..."), 3);
            }
            pt.x += szText.cx;
            pstrText += cchChars;
         }
      }
      ASSERT(iLinkIndex<=nLinkRects);
   }

   // Clear remaining link rects and return number of used rects
   for( i = iLinkIndex; i < nLinkRects; i++ ) ::ZeroMemory(prcLinks + i, sizeof(RECT));
   nLinkRects = iLinkIndex;

   // Return size of text when requested
   if( (uStyle & DT_CALCRECT) != 0 ) {
      rc.bottom = MAX(cyMinHeight, pt.y + cyLine);
      if( rc.right >= 9999 ) {
         if( _tcslen(pstrText) > 0 ) pt.x += 3;
         rc.right = pt.x;
      }
   }

   if( bDraw ) ::SelectClipRgn(hDC, hOldRgn);
   ::DeleteObject(hOldRgn);
   ::DeleteObject(hRgn);

   ::SelectObject(hDC, hOldFont);
}

void CBlueRenderEngineUI::DoPaintGradient(HDC hDC, CPaintManagerUI* pManager, RECT rc, COLORREF clrFirst, COLORREF clrSecond, bool bVertical, int nSteps)
{
   typedef BOOL (WINAPI *PGradientFill)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);
   static PGradientFill lpGradientFill = (PGradientFill) ::GetProcAddress(::GetModuleHandle("msimg32.dll"), "GradientFill");
   if( lpGradientFill != NULL ) 
   {
      // Use Windows gradient function from msimg32.dll
      // It may be slower than the code below but makes really pretty gradients on 16bit colors.
      TRIVERTEX triv[2] = 
      {
         { rc.left, rc.top, GetRValue(clrFirst) << 8, GetGValue(clrFirst) << 8, GetBValue(clrFirst) << 8, 0xFF00 },
         { rc.right, rc.bottom, GetRValue(clrSecond) << 8, GetGValue(clrSecond) << 8, GetBValue(clrSecond) << 8, 0xFF00 }
      };
      GRADIENT_RECT grc = { 0, 1 };
      lpGradientFill(hDC, triv, 2, &grc, 1, bVertical ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
   }
   else 
   {
      // Determine how many shades
      int nShift = 1;
      if( nSteps >= 64 ) nShift = 6;
      else if( nSteps >= 32 ) nShift = 5;
      else if( nSteps >= 16 ) nShift = 4;
      else if( nSteps >= 8 ) nShift = 3;
      else if( nSteps >= 4 ) nShift = 2;
      int nLines = 1 << nShift;
      for( int i = 0; i < nLines; i++ ) {
         // Do a little alpha blending
         BYTE bR = (BYTE) ((GetRValue(clrSecond) * (nLines - i) + GetRValue(clrFirst) * i) >> nShift);
         BYTE bG = (BYTE) ((GetGValue(clrSecond) * (nLines - i) + GetGValue(clrFirst) * i) >> nShift);
         BYTE bB = (BYTE) ((GetBValue(clrSecond) * (nLines - i) + GetBValue(clrFirst) * i) >> nShift);
         // ... then paint with the resulting color
         HBRUSH hBrush = ::CreateSolidBrush(RGB(bR,bG,bB));
         RECT r2 = rc;
         if( bVertical ) {
            r2.bottom = rc.bottom - ((i * (rc.bottom - rc.top)) >> nShift);
            r2.top = rc.bottom - (((i + 1) * (rc.bottom - rc.top)) >> nShift);
            if( (r2.bottom - r2.top) > 0 ) ::FillRect(hDC, &r2, hBrush);
         }
         else {
            r2.left = rc.right - (((i + 1) * (rc.right - rc.left)) >> nShift);
            r2.right = rc.right - ((i * (rc.right - rc.left)) >> nShift);
            if( (r2.right - r2.left) > 0 ) ::FillRect(hDC, &r2, hBrush);
         }
         ::DeleteObject(hBrush);
      }
   }
}

void CBlueRenderEngineUI::DoPaintAlphaBitmap(HDC hDC, CPaintManagerUI* pManager, HBITMAP hBitmap, RECT rc, BYTE iAlpha)
{
   // Alpha blitting is only supported of the msimg32.dll library is located on the machine.
   typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
   static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(::GetModuleHandle("msimg32.dll"), "AlphaBlend");
   if( lpAlphaBlend == NULL ) return;
   if( hBitmap == NULL ) return;
   HDC hCloneDC = ::CreateCompatibleDC(pManager->GetPaintDC());
   HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
   int cx = rc.right - rc.left;
   int cy = rc.bottom - rc.top;
   ::SetStretchBltMode(hDC, COLORONCOLOR);
   BLENDFUNCTION bf = { 0 };
   bf.BlendOp = AC_SRC_OVER; 
   bf.BlendFlags = 0; 
   bf.AlphaFormat = AC_SRC_ALPHA;
   bf.SourceConstantAlpha = iAlpha;
   lpAlphaBlend(hDC, rc.left, rc.top, cx, cy, hCloneDC, 0, 0, cx, cy, bf);
   ::SelectObject(hCloneDC, hOldBitmap);
   ::DeleteDC(hCloneDC);
}

void CBlueRenderEngineUI::DoAnimateWindow(HWND hWnd, UINT uStyle, DWORD dwTime /*= 200*/)
{
   typedef BOOL (CALLBACK* PFNANIMATEWINDOW)(HWND, DWORD, DWORD);
#ifndef AW_HIDE
   const DWORD AW_HIDE = 0x00010000;
   const DWORD AW_BLEND = 0x00080000;
#endif
   // Mix flags
   DWORD dwFlags = 0;
   if( (uStyle & UIANIM_HIDE) != 0 ) dwFlags |= AW_HIDE;
   if( (uStyle & UIANIM_FADE) != 0 ) dwFlags |= AW_BLEND;
   PFNANIMATEWINDOW pfnAnimateWindow = (PFNANIMATEWINDOW) ::GetProcAddress(::GetModuleHandle(_T("user32.dll")), "AnimateWindow");
   if( pfnAnimateWindow != NULL ) pfnAnimateWindow(hWnd, dwTime, dwFlags);
}

/*
HBITMAP CBlueRenderEngineUI::GenerateAlphaBitmap(CPaintManagerUI* pManager, CControlUI* pControl, RECT rc, UITYPE_COLOR Background)
{
   typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
   static FARPROC lpAlphaBlend = ::GetProcAddress(::GetModuleHandle("msimg32.dll"), "AlphaBlend");
   if( lpAlphaBlend == NULL ) return NULL;
   int cx = rc.right - rc.left;
   int cy = rc.bottom - rc.top;

   // Let the control paint itself onto an offscreen bitmap
   HDC hPaintDC = ::CreateCompatibleDC(pManager->GetPaintDC());
   HBITMAP hPaintBitmap = ::CreateCompatibleBitmap(pManager->GetPaintDC(), rc.right, rc.bottom);
   ASSERT(hPaintDC);
   ASSERT(hPaintBitmap);
   HBITMAP hOldPaintBitmap = (HBITMAP) ::SelectObject(hPaintDC, hPaintBitmap);
   DoFillRect(hPaintDC, pManager, rc, Background);
   pControl->DoPaint(hPaintDC, rc);

   // Create a new 32bpp bitmap with room for an alpha channel
   BITMAPINFO bmi = { 0 };
   bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   bmi.bmiHeader.biWidth = cx;
   bmi.bmiHeader.biHeight = cy;
   bmi.bmiHeader.biPlanes = 1;
   bmi.bmiHeader.biBitCount = 32;
   bmi.bmiHeader.biCompression = BI_RGB;
   bmi.bmiHeader.biSizeImage = cx * cy * sizeof(DWORD);
   LPDWORD pDest = NULL;
   HDC hCloneDC = ::CreateCompatibleDC(pManager->GetPaintDC());
   HBITMAP hBitmap = ::CreateDIBSection(pManager->GetPaintDC(), &bmi, DIB_RGB_COLORS, (LPVOID*) &pDest, NULL, 0);
   ASSERT(hCloneDC);
   ASSERT(hBitmap);
   if( hBitmap != NULL )
   {
      // Copy offscreen bitmap to our new 32bpp bitmap
      HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
      ::BitBlt(hCloneDC, 0, 0, cx, cy, hPaintDC, rc.left, rc.top, SRCCOPY);
      ::SelectObject(hCloneDC, hOldBitmap);
      ::DeleteDC(hCloneDC);  
      ::GdiFlush();

      // Make the background color transparent
      COLORREF clrBack = pManager->GetThemeColor(Background);
      DWORD dwKey = RGB(GetBValue(clrBack), GetGValue(clrBack), GetRValue(clrBack));
      DWORD dwShowColor = 0xFF000000;
      for( int y = 0; y < abs(bmi.bmiHeader.biHeight); y++ ) {
         for( int x = 0; x < bmi.bmiHeader.biWidth; x++ ) {
            if( *pDest != dwKey ) *pDest = *pDest | dwShowColor; 
            else *pDest = 0x00000000;
            pDest++;
         }
      }
   }

   // Cleanup
   ::SelectObject(hPaintDC, hOldPaintBitmap);
   ::DeleteObject(hPaintBitmap);
   ::DeleteDC(hPaintDC);

   return hBitmap;
}*/

