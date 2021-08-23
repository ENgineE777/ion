#if !defined(AFX_BLUEUI_H__20050424_6E62_6B42_BC3F_0080AD509054__INCLUDED_)
#define AFX_BLUEUI_H__20050424_6E62_6B42_BC3F_0080AD509054__INCLUDED_

#pragma once

#include "windows.h"
#include "windowsx.h"

/////////////////////////////////////////////////////////////////////////////////////
//

typedef enum
{
   UIFONT__FIRST = 0,
   UIFONT_NORMAL,
   UIFONT_BOLD,
   UIFONT_CAPTION,
   UIFONT_MENU,
   UIFONT_LINK,
   UIFONT_TITLE,
   UIFONT_HEADLINE,
   UIFONT_SUBSCRIPT,
   UIFONT__LAST,
} UITYPE_FONT;

typedef enum
{
   UICOLOR__FIRST = 0,
   UICOLOR_WINDOW_BACKGROUND,
   UICOLOR_WINDOW_TEXT,
   UICOLOR_DIALOG_BACKGROUND,
   UICOLOR_DIALOG_TEXT_NORMAL,
   UICOLOR_DIALOG_TEXT_DARK,
   UICOLOR_MENU_BACKGROUND,
   UICOLOR_MENU_TEXT_NORMAL,
   UICOLOR_MENU_TEXT_HOVER,
   UICOLOR_MENU_TEXT_SELECTED,
   UICOLOR_TEXTCOLOR_NORMAL,
   UICOLOR_TEXTCOLOR_HOVER,
   UICOLOR_TAB_BACKGROUND_NORMAL,
   UICOLOR_TAB_BACKGROUND_SELECTED,
   UICOLOR_TAB_FOLDER_NORMAL,
   UICOLOR_TAB_FOLDER_SELECTED,
   UICOLOR_TAB_BORDER,
   UICOLOR_TAB_TEXT_NORMAL,
   UICOLOR_TAB_TEXT_SELECTED,
   UICOLOR_TAB_TEXT_DISABLED,
   UICOLOR_NAVIGATOR_BACKGROUND,
   UICOLOR_NAVIGATOR_BUTTON_HOVER,
   UICOLOR_NAVIGATOR_BUTTON_PUSHED,
   UICOLOR_NAVIGATOR_BUTTON_SELECTED,
   UICOLOR_NAVIGATOR_BORDER_NORMAL,
   UICOLOR_NAVIGATOR_BORDER_SELECTED,
   UICOLOR_NAVIGATOR_TEXT_NORMAL,
   UICOLOR_NAVIGATOR_TEXT_SELECTED,
   UICOLOR_NAVIGATOR_TEXT_PUSHED,
   UICOLOR_BUTTON_BACKGROUND_NORMAL,
   UICOLOR_BUTTON_BACKGROUND_DISABLED,
   UICOLOR_BUTTON_BACKGROUND_PUSHED,
   UICOLOR_BUTTON_TEXT_NORMAL,
   UICOLOR_BUTTON_TEXT_PUSHED,
   UICOLOR_BUTTON_TEXT_DISABLED,
   UICOLOR_BUTTON_BORDER_LIGHT,
   UICOLOR_BUTTON_BORDER_DARK,
   UICOLOR_BUTTON_BORDER_DISABLED,
   UICOLOR_BUTTON_BORDER_FOCUS,
   UICOLOR_CONTROL_BACKGROUND_NORMAL,
   UICOLOR_CONTROL_BACKGROUND_SELECTED,
   UICOLOR_CONTROL_BACKGROUND_DISABLED,
   UICOLOR_CONTROL_BACKGROUND_READONLY,
   UICOLOR_CONTROL_BACKGROUND_HOVER,
   UICOLOR_CONTROL_BACKGROUND_SORTED,
   UICOLOR_CONTROL_BACKGROUND_EXPANDED,
   UICOLOR_CONTROL_BORDER_NORMAL,
   UICOLOR_CONTROL_BORDER_SELECTED,
   UICOLOR_CONTROL_BORDER_DISABLED,
   UICOLOR_CONTROL_TEXT_NORMAL,
   UICOLOR_CONTROL_TEXT_SELECTED,
   UICOLOR_CONTROL_TEXT_DISABLED,
   UICOLOR_CONTROL_TEXT_READONLY,
   UICOLOR_TOOL_BACKGROUND_NORMAL,
   UICOLOR_TOOL_BACKGROUND_DISABLED,
   UICOLOR_TOOL_BACKGROUND_HOVER,
   UICOLOR_TOOL_BACKGROUND_PUSHED,
   UICOLOR_TOOL_BORDER_NORMAL,
   UICOLOR_TOOL_BORDER_DISABLED,
   UICOLOR_TOOL_BORDER_HOVER,
   UICOLOR_TOOL_BORDER_PUSHED,
   UICOLOR_EDIT_BACKGROUND_NORMAL,
   UICOLOR_EDIT_BACKGROUND_HOVER,
   UICOLOR_EDIT_BACKGROUND_DISABLED,
   UICOLOR_EDIT_BACKGROUND_READONLY,
   UICOLOR_EDIT_TEXT_NORMAL,
   UICOLOR_EDIT_TEXT_DISABLED,
   UICOLOR_EDIT_TEXT_READONLY,
   UICOLOR_TITLE_BACKGROUND,
   UICOLOR_TITLE_TEXT,
   UICOLOR_TITLE_BORDER_LIGHT,
   UICOLOR_TITLE_BORDER_DARK,
   UICOLOR_HEADER_BACKGROUND,
   UICOLOR_HEADER_BORDER,
   UICOLOR_HEADER_SEPARATOR,
   UICOLOR_HEADER_TEXT,
   UICOLOR_TASK_BACKGROUND,
   UICOLOR_TASK_CAPTION,
   UICOLOR_TASK_BORDER,
   UICOLOR_TASK_TEXT,
   UICOLOR_LINK_TEXT_HOVER,
   UICOLOR_LINK_TEXT_NORMAL,
   UICOLOR_STANDARD_BLACK,
   UICOLOR_STANDARD_YELLOW,
   UICOLOR_STANDARD_RED,
   UICOLOR_STANDARD_GREY,
   UICOLOR_STANDARD_LIGHTGREY,
   UICOLOR_STANDARD_WHITE,
   UICOLOR__LAST,
   UICOLOR__INVALID,
} UITYPE_COLOR;

// Styles for the DoPaintFrame() helper
#define UIFRAME_ROUND        0x00000001
#define UIFRAME_FOCUS        0x00000002

// Styles for the DoPaintArcCaption() helper
#define UIARC_GRIPPER        0x00000001
#define UIARC_EXPANDBUTTON   0x00000002
#define UIARC_COLLAPSEBUTTON 0x00000004

// Flags for CControlUI::GetControlFlags()
#define UIFLAG_TABSTOP       0x00000001
#define UIFLAG_SETCURSOR     0x00000002
#define UIFLAG_WANTRETURN    0x00000004

// Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_ME_FIRST      0x80000000

// Flags for Draw Style
#define UIDRAWSTYLE_INPLACE  0x00000001
#define UIDRAWSTYLE_FOCUS    0x00000002

// Flags for DoAnumateWindow()
#define UIANIM_FADE          0x00000001
#define UIANIM_HIDE          0x00000002

// Flags for the CDialogLayout stretching
#define UISTRETCH_NEWGROUP   0x00000001
#define UISTRETCH_NEWLINE    0x00000002
#define UISTRETCH_MOVE_X     0x00000004
#define UISTRETCH_MOVE_Y     0x00000008
#define UISTRETCH_SIZE_X     0x00000010
#define UISTRETCH_SIZE_Y     0x00000020

// Flags used for controlling the paint
#define UISTATE_FOCUSED      0x00000001
#define UISTATE_SELECTED     0x00000002
#define UISTATE_DISABLED     0x00000004
#define UISTATE_HOT          0x00000008
#define UISTATE_PUSHED       0x00000010
#define UISTATE_CHECKED      0x00000020
#define UISTATE_READONLY     0x00000040
#define UISTATE_CAPTURED     0x00000080


class CStdString
{
public:
   enum { MAX_LOCAL_STRING_LEN = 63 };

   CStdString();
   CStdString(const TCHAR ch);
   CStdString(const CStdString& src);
   CStdString(LPCTSTR lpsz, int nLen = -1);
   virtual ~CStdString();

   static CStdString RES(UINT nRes);

   void Empty();
   int GetLength() const;
   bool IsEmpty() const;
   TCHAR GetAt(int nIndex) const;
   void Append(LPCTSTR pstr);
   void Assign(LPCTSTR pstr, int nLength = -1);
   LPCTSTR GetData();
   
   void SetAt(int nIndex, TCHAR ch);
   operator LPCTSTR() const;

   TCHAR operator[] (int nIndex) const;
   const CStdString& operator=(const CStdString& src);
   const CStdString& operator=(const TCHAR ch);
   const CStdString& operator=(LPCTSTR pstr);
#ifndef _UNICODE
   const CStdString& CStdString::operator=(LPCWSTR lpwStr);
#endif
   CStdString operator+(const CStdString& src);
   CStdString operator+(LPCTSTR pstr);
   const CStdString& operator+=(const CStdString& src);
   const CStdString& operator+=(LPCTSTR pstr);
   const CStdString& operator+=(const TCHAR ch);

   bool operator == (LPCTSTR str) const;
   bool operator != (LPCTSTR str) const;
   bool operator <= (LPCTSTR str) const;
   bool operator <  (LPCTSTR str) const;
   bool operator >= (LPCTSTR str) const;
   bool operator >  (LPCTSTR str) const;

   int Compare(LPCTSTR pstr) const;
   int CompareNoCase(LPCTSTR pstr) const;
   
   void MakeUpper();
   void MakeLower();

   CStdString Left(int nLength) const;
   CStdString Mid(int iPos, int nLength = -1) const;
   CStdString Right(int nLength) const;

   int Find(TCHAR ch, int iPos = 0) const;
   int Find(LPCTSTR pstr, int iPos = 0) const;
   int ReverseFind(TCHAR ch) const;
   int Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo);
   
   void ProcessResourceTokens();
   int __cdecl Format(LPCTSTR pstrFormat, ...);

protected:
   LPTSTR m_pstr;
   TCHAR m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
};


// Structure for notifications from the system
// to the control implementation.
typedef struct tagTEventUI
{
   int Type;
   //CControlUI* pSender;
   DWORD dwTimestamp;
   POINT ptMouse;
   TCHAR chKey;
   WORD wKeyState;
   WPARAM wParam;
   LPARAM lParam;
} TEventUI;

// Structure for notifications to the outside world
typedef struct tagTNotifyUI 
{
   CStdString sType;
   //CControlUI* pSender;
   DWORD dwTimestamp;
   POINT ptMouse;
   WPARAM wParam;
   LPARAM lParam;
} TNotifyUI;

// Structure for adding alpha bitmaps on top of the window
typedef struct tagTPostPaintUI
{
   HBITMAP hBitmap;
   RECT rc;
   BYTE iAlpha;
} TPostPaintUI;

// System settings
typedef struct tagTSystemSettingsUI
{
   bool bShowKeyboardCues;
   bool bScrollLists;
} TSystemSettingsUI;

// Various system settings
typedef struct tagTSystemMetricsUI
{
   INT cxvscroll;
} TSystemMetricsUI;

// Listener interface
class INotifyUI
{
public:
   virtual void Notify(TNotifyUI& msg) = 0;
};

// MessageFilter interface
class IMessageFilterUI
{
public:
   virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;
};

class CPaintManagerUI
{
public:
   CPaintManagerUI();
   ~CPaintManagerUI();

public:
   void Init(HWND hWnd);
   void UpdateLayout();
   void Invalidate(RECT rcItem);

   HDC GetPaintDC() const;
   HWND GetPaintWindow() const;

   POINT GetMousePos() const;  

   void SetMinMaxInfo(int cx, int cy);

   static HINSTANCE GetResourceInstance();
   static HINSTANCE GetLanguageInstance();
   static void SetResourceInstance(HINSTANCE hInst);
   static void SetLanguageInstance(HINSTANCE hInst);

   HPEN GetThemePen(UITYPE_COLOR Index) const;
   HFONT GetThemeFont(UITYPE_FONT Index) const;
   HBRUSH GetThemeBrush(UITYPE_COLOR Index) const;
   COLORREF GetThemeColor(UITYPE_COLOR Index) const;
   HICON GetThemeIcon(int Index, int cxySize) const;
   const TEXTMETRIC& GetThemeFontInfo(UITYPE_FONT Index) const;
   bool GetThemeColorPair(UITYPE_COLOR Index, COLORREF& clr1, COLORREF& clr2) const;
   
   bool SetNextTabControl(bool bForward = true);
         
   static void MessageLoop();
   static bool TranslateMessage(const LPMSG pMsg);

   bool MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
   bool PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);

   TSystemMetricsUI GetSystemMetrics() const;
   TSystemSettingsUI GetSystemSettings() const;
   void SetSystemSettings(const TSystemSettingsUI Config);

private:
   

private:
   HWND m_hWndPaint;
   HDC m_hDcPaint;
   HDC m_hDcOffscreen;
   HBITMAP m_hbmpOffscreen;
   HWND m_hwndTooltip;
   
   POINT m_ptLastMousePos;
   SIZE m_szMinWindow;
   UINT m_uMsgMouseWheel;
   UINT m_uTimerID;
   bool m_bFirstLayout;
   bool m_bResizeNeeded;
   bool m_bFocusNeeded;
   bool m_bOffscreenPaint;
   bool m_bMouseTracking;
   //
   TSystemMetricsUI m_SystemMetrics;
   TSystemSettingsUI m_SystemConfig;
   //
   
   static HINSTANCE m_hLangInst;
   static HINSTANCE m_hInstance;  
};

class CRenderClip
{
public:
   ~CRenderClip();
   RECT rcItem;
   HDC hDC;
   HRGN hRgn;
   HRGN hOldRgn;
};

class CBlueRenderEngineUI
{
public:
   static void DoFillRect(HDC hDC, CPaintManagerUI* pManager, RECT rc, UITYPE_COLOR Color);
   static void DoFillRect(HDC hDC, CPaintManagerUI* pManager, RECT rc, COLORREF clrFill);
   static void DoPaintLine(HDC hDC, CPaintManagerUI* pManager, RECT rc, UITYPE_COLOR Color);
   static void DoPaintRectangle(HDC hDC, CPaintManagerUI* pManager, RECT rcItem, UITYPE_COLOR Border, UITYPE_COLOR Fill);
   static void DoPaintPanel(HDC hDC, CPaintManagerUI* pManager, RECT rc);
   static void DoPaintFrame(HDC hDC, CPaintManagerUI* pManager, RECT rc, UITYPE_COLOR Light, UITYPE_COLOR Dark, UITYPE_COLOR Background = UICOLOR__INVALID, UINT uStyle = 0);
   static void DoPaintBitmap(HDC hDC, CPaintManagerUI* pManager, HBITMAP hBmp, RECT rcBitmap);
   static void DoPaintArcCaption(HDC hDC, CPaintManagerUI* pManager, RECT rc, LPCTSTR pstrText, UINT uStyle);
   static void DoPaintButton(HDC hDC, CPaintManagerUI* pManager, RECT rc, LPCTSTR pstrText, RECT rcPadding, UINT uState, UINT uDrawStyle);
   static void DoPaintEditBox(HDC hDC, CPaintManagerUI* pManager, RECT rcItem, LPCTSTR pstrText, UINT uState, UINT uDrawStyle, bool bPaintFrameOnly);
   static void DoPaintOptionBox(HDC hDC, CPaintManagerUI* pManager, RECT rcItem, LPCTSTR pstrText, UINT uState, UINT uStyle);
   static void DoPaintTabFolder(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, UINT uState);
   static void DoPaintToolbarButton(HDC hDC, CPaintManagerUI* pManager, RECT rc, LPCTSTR pstrText, SIZE szPadding, UINT uState);
   static void DoPaintQuickText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCSTR pstrText, UITYPE_COLOR iTextColor, UITYPE_FONT iFont, UINT uStyle);
   static void DoPaintPrettyText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, UITYPE_COLOR iTextColor, UITYPE_COLOR iBackColor, RECT* pLinks, int& nLinkRects, UINT uStyle);
   static void DoPaintGradient(HDC hDC, CPaintManagerUI* pManager, RECT rc, COLORREF clrFirst, COLORREF clrSecond, bool bVertical, int nSteps);
   static void DoPaintAlphaBitmap(HDC hDC, CPaintManagerUI* pManager, HBITMAP hBitmap, RECT rc, BYTE iAlpha);
   static void DoAnimateWindow(HWND hWnd, UINT uStyle, DWORD dwTime = 200);
   static void GenerateClip(HDC hDC, RECT rcItem, CRenderClip& clip);
   //static HBITMAP GenerateAlphaBitmap(CPaintManagerUI* pManager, CControlUI* pControl, RECT rc, UITYPE_COLOR Background);
};


#endif // !defined(AFX_BLUEUI_H__20050424_6E62_6B42_BC3F_0080AD509054__INCLUDED_)
