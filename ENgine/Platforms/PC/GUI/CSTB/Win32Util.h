/// @file Utility functions and macros for win32 applications.
/// -Adel Amro (http://code-section.com)


# ifndef CSTB_WIN32UTIL_H
# define CSTB_WIN32UTIL_H


# include <windows.h>
# include <CommDlg.h>
# include <CommCtrl.h>
//# include <windowsx.h>
# include <cstdlib>
# include <math.h>
# include <tchar.h>
# include <strsafe.h>
# include <wininet.h> // For LoadWebPage().


//# define WIN32UTIL_VISUAL_STYLES
# ifdef WIN32UTIL_VISUAL_STYLES
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#if defined( _DEBUG ) && !defined( DEBUG )
#define DEBUG
#endif


#ifndef PI
#define PI 3.14159265358979f
#endif

#ifndef DEGTORAD
/// Macro to convert from degrees to radians.
#define DEGTORAD(d)				( (d)*PI/180 )
#endif

#ifndef RADTODEG
/// Macro to convert from radians to degrees.
#define RADTODEG(r)				( (r)*180/PI )
#endif

#ifndef SAFE_DELETE
/// For pointers allocated with new.
#define SAFE_DELETE(p)			{ if(p) { delete (p);     (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
/// For arrays allocated with new [].
#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete[] (p);   (p)=NULL; } }
#endif

#ifndef SAFE_RELEASE
/// For use with COM pointers.
#define SAFE_RELEASE(p)			{ if(p) { (p)->Release(); (p)=NULL; } }
#endif

/// TODO: Add SAFE_DELETE_OBJECT() for GDI objects.
#define SAFE_DESTROY_WINDOW(hWnd)	{ if(hWnd) { DestroyWindow(hWnd); hWnd = NULL; } }

#ifndef ARRAY_SIZE
/// Returns the number of elements in a stack-allocated array.
#define ARRAY_SIZE(x)			( (sizeof((x))/sizeof((x)[0])) )
#endif

#ifndef ARRAY_COUNT
/// Same as ARRAY_SIZE, returns teh number of elements in a stack-allocated array.
#define ARRAY_COUNT ARRAY_SIZE
#endif

#ifndef DLG_RETURL
#define DLG_RETURN(x) {SetWindowLong(hDlg,DWL_MSGRESULT,(x));return (x);}
#endif

#ifndef MIN
#define MIN(x,y)				( (x)<(y)? (x) : (y) )
#endif

#ifndef MAX
#define MAX(x,y)				( (x)>(y)? (x) : (y) )
#endif

#ifndef CLAMP
#define CLAMP(x,a,b)			( (x)>(b) ? (b) : (x)<(a) ? (a) : (x) )
#endif

#ifndef CLAMP01
#define CLAMP01(x)				( (x)>1 ? 1 : (x) < 0 ? 0 : (x) )
#endif

#ifndef RECTWIDTH
#define RECTWIDTH(r)			( ((r).right - (r).left) ) ///< The height of a RECT structure.
#endif

#ifndef RECTHEIGHT
#define RECTHEIGHT(r)			( ((r).bottom - (r).top) ) ///< The width of a RECT structure.
#endif

#ifndef RECTCENTERX
#define RECTCENTERX(r)			( ((r).left + RECTWIDTH(r)/2) ) ///< The x coordinate of the center of a RECT structure.
#endif

#ifndef RECTCENTERY
#define RECTCENTERY(r)			( ((r).top + RECTHEIGHT(r)/2) ) ///< The y coordinate of the center of a RECT structure.
#endif

#ifndef ABS
#define ABS(x)					( (x)<0?-(x):(x) )
#endif
//#ifndef ROUND
//#define ROUND(x)				( ((x) - (INT)(x) < 0.5f ? (INT)(x) : (INT)(x) + 1) )
//#endif
#ifndef ISKEYDOWN
//#define ISKEYDOWN(x)			( (GetAsyncKeyState((x))&0x8000)!=0 ) ///< Returns whether a key or mouse button is pressed.
#define ISKEYDOWN(x)			( (GetKeyState((x))&0x8000)!=0 ) ///< Returns whether a key or mouse button is pressed.
#endif

#ifndef LINEAR_INTERPOLATE
//#define LINEAR_INTERPOLATE(a,b,t) ( (a)*(1-(t)) + (b)*(t) )
#define LINEAR_INTERPOLATE(a,b,t)	( (a)+(t)*((b)-(a)) )
#endif

#ifndef LERP
#define LERP LINEAR_INTERPOLATE ///< Linear interpolation.
#endif

// From bjarke - http://www.viksoe.dk
#ifndef BlendRGB
#define BlendRGB(c1, c2, factor) \
      RGB( GetRValue(c1) + ((GetRValue(c2) - GetRValue(c1)) * factor / 100L), \
           GetGValue(c1) + ((GetGValue(c2) - GetGValue(c1)) * factor / 100L), \
           GetBValue(c1) + ((GetBValue(c2) - GetBValue(c1)) * factor / 100L) )
#endif

# ifndef SWAP
/// Swap template function.
template <class T>
inline void SWAP( T& t1, T& t2 ) { T temp = t1; t1 = t2; t2 = temp; }
# endif


inline POINT MakePoint( LONG x, LONG y ) { POINT pt; pt.x = x; pt.y = y; return pt; }
inline void SetPoint( POINT* pt, LONG x, LONG y ) { pt->x = x; pt->y = y; }
inline void SetPoint( POINTS* pts, SHORT x, SHORT y ) { pts->x = x; pts->y = y; }

inline LONG RectWidth( const RECT& r ) { return r.right - r.left; }
inline LONG RectHeight( const RECT& r ) { return r.bottom - r.top; }
inline float RectAspect( const RECT& r ) { return RectWidth(r) / (FLOAT)RectHeight(r); }
inline POINT RectCenter( const RECT& r )
{ POINT pt; pt.x = r.left + RectWidth(r)/2; pt.y = r.top + RectHeight(r)/2; return pt; }
inline VOID SetRect( RECT* pr, POINT ptCenter, LONG width, LONG height )
{
	INT w1 = width/2; INT w2 = width - w1; INT h1 = height/2; INT h2 = height - h1;
	SetRect( pr, ptCenter.x - w1, ptCenter.y - h1, ptCenter.x + w2, ptCenter.y + h2 );
}
/// TODO: Make sure this is correct such that IsRectEmpty() returns TRUE for the rect.
inline VOID ResetRect( RECT* pr ) { pr->left = 0; pr->top = 0; pr->right = 0; pr->bottom = 0; }

inline bool operator == (const RECT& r1, const RECT& r2 )
{ return r1.left == r2.left && r1.right == r2.right && r1.top == r2.top && r1.bottom == r2.bottom; }
inline bool operator != (const RECT& r1, const RECT& r2 ) { return ! (r1 == r2); }

/// Toggles a boolean and returns the new value.
inline bool ToggleBool( bool& b ) { b = !b; return b; }
inline BOOL ToggleBool( BOOL& b ) { b = !b; return b; }


/// Gets the string describing the last error as returned by GetLastError().
inline LPCTSTR GetLastErrorString()
{
	static TCHAR s[80];
	DWORD dw = GetLastError();
	FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		s,
		80, NULL );
	return s;
}

//inline LPCTSTR LoadString( HINSTANCE hInstance, UINT id )
//{
//	static TCHAR s[MAX_PATH];
//	s[0] = 0;
//	::LoadString( hInstance, id, s, ARRAY_SIZE(s) );
//	return s;
//}


/// Sets and clears style flags for a particular window.
inline VOID ModifyWindowStyle( HWND hWnd, DWORD flagsToDisable, DWORD flagsToEnable )
{
	DWORD style = GetWindowLong( hWnd, GWL_STYLE );
	SetWindowLong( hWnd, GWL_STYLE, (style & ~flagsToDisable) | flagsToEnable );
}

/// Sets and clears extended style flags for a particular window.
inline VOID ModifyWindowExStyle( HWND hWnd, DWORD flagsToDisable, DWORD flagsToEnable )
{
	DWORD exStyle = GetWindowLong( hWnd, GWL_EXSTYLE );
	SetWindowLong( hWnd, GWL_EXSTYLE, (exStyle & ~flagsToDisable) | flagsToEnable );
}



/// Create an instance of this struct to subclass a window. The window is un-subclassed
/// when the object goes out of scope.
struct WindowSubclass
{
	WindowSubclass( HWND _hWnd );
	virtual ~WindowSubclass();

	/// Override this method to define custom behavior. Don't forget to call WindowSubclass::MsgProc()
	/// at the end of your MsgProc().
	virtual LRESULT MsgProc( UINT Msg, WPARAM wParam, LPARAM lParam )
	{ return CallWindowProc( originalProc, hWnd, Msg, wParam, lParam ); }

	HWND hWnd;
protected:
	WNDPROC originalProc;
	static LRESULT CALLBACK sMsgProc( HWND, UINT, WPARAM, LPARAM );
};




/// Tests whether a window is a fullscreen window or not.
inline BOOL IsWindowFullscreen( HWND hWnd )
{
	int screenWidth = GetSystemMetrics( SM_CXSCREEN );
	int screenHeight = GetSystemMetrics( SM_CYSCREEN );

	RECT rWindow;
	GetWindowRect( hWnd, &rWindow );

	return rWindow.left == 0 && rWindow.top == 0 &&
		rWindow.right == screenWidth && rWindow.bottom == screenHeight;
}


/// Toggles the fulscreen state of a visible top-level window. It requires a WINDOWPLACEMENT
/// structure so that it can properly save and restore the window placement.
inline void ToggleFullscreen( HWND hWnd, WINDOWPLACEMENT& windowPlacement,DWORD defaultStyle = WS_OVERLAPPEDWINDOW,DWORD fullscreenStyle = WS_POPUP )
{
	if( !IsWindowVisible( hWnd ) || GetParent( hWnd ) != NULL )
		return; // The window should be visible and top-level.

	windowPlacement.length = sizeof( windowPlacement );

	BOOL bFullscreen = IsWindowFullscreen( hWnd );	

	if( bFullscreen )
	{
		SetWindowLong( hWnd, GWL_STYLE, defaultStyle );
		SetWindowPos( hWnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE );
		SetWindowPlacement( hWnd, &windowPlacement );
	}
	else
	{
		GetWindowPlacement( hWnd, &windowPlacement );
		SetWindowLong( hWnd, GWL_STYLE, fullscreenStyle );
		ShowWindow( hWnd, SW_MAXIMIZE );
	}
}


/// Shortcut to return window text. Limited to MAX_PATH characters.
inline LPCTSTR GetWindowText( HWND hWnd )
{
	static TCHAR s[ MAX_PATH ]; s[0] = 0;
	GetWindowText( hWnd, s, MAX_PATH );
	return s;
}


/// Memory comparison. Returns 0 if memory is identical, 1 if p1 is greater (bytewise), -1 if it's smaller. 'length' is in bytes.
inline INT CompareMemory( const VOID* p1, const VOID* p2, UINT length )
{
	BYTE* b1 = (BYTE*)p1, *b2 = (BYTE*)p2;
	for( UINT i=0; i<length; i++ )
	{
		if( b1[i] > b2[i] )
			return 1;
		if( b1[i] < b2[i] )
			return -1;
	}
	return 0;
}


/// Floating point rounding.
inline LONG fround( float f )	{ if( f < 0 ) return (LONG)(f - 0.5f); return (LONG)(f + 0.5f); }

/// Returns a random positive float value ranging from 0 to 1.
inline FLOAT frand01() { return (FLOAT)rand()/RAND_MAX; }

/// Returns a random float value ranging from fMin to fMax.
inline FLOAT frand( float fMin, float fMax ) { return fMin + (fMax - fMin) * ((float)rand()/RAND_MAX); }

/// Returns a random positive float value.
inline FLOAT frand() { return (float)rand()/RAND_MAX + rand(); }

//inline INT Align( INT n, INT boundary ) { INT rem = n % boundary; if( rem ) n += (boundary-rem); return n; }
/// Used for memory alignment. Returns the result of rounding 'n' to the smallest multiple of 'boundary'.
inline UINT Align( UINT n, UINT boundary ) { if( !boundary ) return n; return ((n + boundary - 1) / boundary) * boundary; }

/// Align to DWORD boundaries. See Align().
inline size_t DwordAlign( size_t n ) { return (size_t)Align( (UINT)n, sizeof(DWORD) ); }

inline DWORD FLOAT2DWORD( FLOAT f ) { return *(DWORD*)&f; }

/// Creates a buffer and loads the specified file into it.
/// Uses malloc(), so use free() to free the buffer.
/// Returns TRUE on success.
inline BOOL LoadFileToBuffer( LPCTSTR sFileName, VOID* buffer, INT bufferSize, INT* pBytesRead )
{
	HANDLE hFile = CreateFile(	sFileName, GENERIC_READ, FILE_SHARE_READ,
								NULL, OPEN_EXISTING, 0, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;
	DWORD bytesRead = 0;
	BOOL bRet = ReadFile( hFile, buffer, bufferSize, &bytesRead, NULL );
	if( pBytesRead )
		*pBytesRead = bytesRead;
	CloseHandle( hFile );
	return bRet;
}

/// Saves a buffer to a new file (overwrites it if it exists).
inline BOOL SaveBufferToFile( LPCTSTR sFileName, const VOID* buffer, INT bufferSize, INT* pBytesWritten = NULL )
{
	HANDLE hFile = CreateFile( sFileName, GENERIC_WRITE, FILE_SHARE_WRITE,
		NULL, CREATE_ALWAYS, 0, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;
	DWORD bytesWritten = 0;
	BOOL bRet = WriteFile( hFile, buffer, bufferSize, &bytesWritten, NULL );
	if( pBytesWritten )
		*pBytesWritten = bytesWritten;
	CloseHandle( hFile );
	return bRet;
}


/// Returns -1 on failure, the size of the file otherwise.
inline INT GetFileSize( LPCTSTR sFileName )
{
	HANDLE hFile = CreateFile(	sFileName, GENERIC_READ, FILE_SHARE_READ,
								NULL, OPEN_EXISTING, 0, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
		return -1;
	INT ret = GetFileSize( hFile, NULL );
	CloseHandle( hFile );
	return ret;
}


/// Linear interpolation between two COLORREF values.
inline COLORREF InterpolateCOLORREF( COLORREF c1, COLORREF c2, float t )
{
	INT r = (INT)LINEAR_INTERPOLATE( GetRValue(c1), GetRValue(c2), t );
	INT g = (INT)LINEAR_INTERPOLATE( GetGValue(c1), GetGValue(c2), t );
	INT b = (INT)LINEAR_INTERPOLATE( GetBValue(c1), GetBValue(c2), t );
	return RGB( CLAMP(r, 0, 255), CLAMP(g, 0, 255), CLAMP(b, 0, 255 ) );
}

/// Check whether a file exists or not.
inline BOOL FileExists(LPCTSTR szPath)
{
	//return PathFileExists( szPath ); //Requires shlwapi.h and shlwapi.lib
  DWORD dwAttrib = GetFileAttributes(szPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

inline BOOL FolderExists( LPCTSTR szPath )
{
	DWORD dwAttrib = GetFileAttributes(szPath);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
			(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}


/// Shortens the code needed to use the GetOpenFileName() API.
/// Example usage:
/// TCHAR sFile[ MAX_PATH ]; sFile[0] = 0;
/// if( GetOpenFileName( sFile, ARRAY_SIZE(sFile), TEXT("Open lsm file"), hOwnerWnd,
/// 	TEXT( "lsm files (*.lsm)\0" )	TEXT( "*.lsm\0" )
/// 	TEXT( "All files (*.*)\0" )		TEXT( "*.*\0" ),
/// 	1 ) )
/// {
/// 	LoadLsmFile( sFileName );
/// }
inline BOOL GetOpenFileName( BOOL bOpen, LPTSTR sFile, UINT bufferLen, LPCTSTR sTitle, HWND hwndOwner,LPCTSTR sFilter, INT nFilterIndex = 1,DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR )
{
	OPENFILENAME ofn = {0};
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrTitle = sTitle;
	ofn.lpstrFile = sFile;
	ofn.nMaxFile = bufferLen;
	ofn.Flags = dwFlags;
	ofn.hwndOwner = hwndOwner;
	ofn.lpstrFilter = sFilter;
	ofn.nFilterIndex = nFilterIndex;
	return GetOpenFileName( &ofn );
}


/// Displays the system's color picker.
/// @param color : [in, out]: The initial color, and the output color.
inline bool ChooseColor( HWND hOwner, COLORREF& color )
{
	static COLORREF customColors[16];
	CHOOSECOLOR cc = {0};
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hOwner;
	cc.rgbResult = color;
	cc.lpCustColors = customColors;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	if( TRUE == ChooseColor( &cc ) )
	{
		color = cc.rgbResult;
		return true;
	}
	return false;
}



/// From catch22.net - could be faster than the FillRect API.
inline VOID FillRect( HDC hDC, const RECT* pRect, COLORREF color )
{
	COLORREF oldColor = SetBkColor( hDC, color );
	ExtTextOut( hDC, 0, 0, ETO_OPAQUE, pRect, TEXT(""), 0, 0 );
	SetBkColor( hDC, oldColor );
}

/// Transform a RECT from client to screen space.
inline VOID ClientToScreen( HWND hWnd, RECT* pRect )
{
	::ClientToScreen( hWnd, (LPPOINT)&pRect->left );
	::ClientToScreen( hWnd, (LPPOINT)&pRect->right );
}

/// Convert a RECT from screen to client space.
inline VOID ScreenToClient( HWND hWnd, RECT* pRect )
{
	ScreenToClient( hWnd, (LPPOINT)&pRect->left );
	ScreenToClient( hWnd, (LPPOINT)&pRect->right );
}

inline POINT ScreenToClient( HWND hWnd, POINT pt ) { ::ScreenToClient( hWnd, &pt ); return pt; }
inline POINT ClientToScreen( HWND hWnd, POINT pt ) { ::ClientToScreen( hWnd, &pt ); return pt; }

/// Get the cursor position in client space of the specified window.
inline POINT GetCursorPos( HWND hWnd )
{ POINT pt; GetCursorPos( &pt ); return ScreenToClient( hWnd, pt ); }

/// Get the cursor position in screen-space.
inline POINT GetCursorPos() { POINT pt; GetCursorPos( &pt ); return pt; }

/// Distance between two points.
inline FLOAT PointDist( POINT pt1, POINT pt2 )
{ INT dx = pt1.x - pt2.x; INT dy = pt1.y - pt2.y; return sqrtf( (FLOAT)(dx*dx + dy*dy) ); }

/// Modifies the specified window rectangle of a popup window so that it's fully visible
/// based on current desktop size. Also optionally centers the rectangle.
VOID AdjustPopupRect( RECT& rWindow, BOOL bCenter );


inline SIZE GetBitmapSize( HBITMAP hBmp )
{
	SIZE sz = {0};
	BITMAP bm;
	GetObject( hBmp, sizeof(bm), &bm );
	sz.cx = bm.bmWidth;
	sz.cy = bm.bmHeight;
	return sz;
}


inline VOID DrawBitmap( HDC hDC, HBITMAP hBitmap, INT xDest, INT yDest, INT cxDest, INT cyDest,
					   INT srcX = 0, INT srcY = 0, DWORD rop = SRCCOPY )
{
	HDC hTempDC = CreateCompatibleDC( hDC );
	HGDIOBJ hOldBMP = SelectObject( hTempDC, hBitmap );

	SIZE sz = GetBitmapSize( hBitmap );

	if( cxDest == 0 ) cxDest = sz.cx;
	if( cyDest == 0 ) cyDest = sz.cy;
	
	StretchBlt( hDC, xDest, yDest, cxDest, cyDest, hTempDC, srcX, srcY, sz.cx, sz.cy, rop );
	//BitBlt( hDC, xDest, yDest, cxDest, cyDest, hTempDC, srcX, srcY, rop );

	SelectObject( hTempDC, hOldBMP );
	DeleteDC( hTempDC );
}



/// Temporary cursor class. Create an instance of this class to change the mouse cursor, and when the instance
/// goes out of scope, the original cursor is restored.
class CTempCursor
{
public:
	HCURSOR oldCursor;
	CTempCursor( UINT cursorID ) { oldCursor = SetCursor( LoadCursor( NULL, MAKEINTRESOURCE(cursorID) ) ); }
	CTempCursor( LPCTSTR cursor ) { oldCursor = SetCursor( LoadCursor( NULL, cursor ) ); }
	~CTempCursor() { SetCursor( oldCursor ); }
};

/// Temporary busy cursor. Create an instace of this class at the beginning of a lengthy operation to display the
/// busy cursor. When the instance is destroyed, the original cursor is restored. See CTempCursor.
class CBusyCursor : public CTempCursor { public: CBusyCursor():CTempCursor(IDC_WAIT){} };


/// An object that would close the encapsulated handle when its destroyed.
class CAutoCloseHandle
{
public:
	HANDLE h;
	CAutoCloseHandle( HANDLE _h = INVALID_HANDLE_VALUE ):h(_h) { }
	~CAutoCloseHandle() { Close(); }

				operator HANDLE () const	{ return h; }
				operator bool () const		{ return h != INVALID_HANDLE_VALUE && h != NULL; }
	HANDLE		operator = ( HANDLE _h )	{ Close(); h = _h; return h; }

	VOID		Close() { if( h == INVALID_HANDLE_VALUE ) return; CloseHandle(h); h = INVALID_HANDLE_VALUE; }
};



/// A simple class to make it very easy to temporarily modify the current working directory.
/// Just instantiate an object of this class and give it the desired new path, and when
/// the object goes out of scope, it will restore the previous current directory.
class CTempCurrentDirectory
{
public:
	CTempCurrentDirectory( LPCSTR sNewDir )
	{
		m_sOldDir[0] = 0;
		GetCurrentDirectory( ARRAY_SIZE( m_sOldDir ), m_sOldDir );
		SetCurrentDirectoryA( sNewDir );
	}
	CTempCurrentDirectory( LPCWSTR sNewDir )
	{
		m_sOldDir[0] = 0;
		GetCurrentDirectory( ARRAY_SIZE( m_sOldDir ), m_sOldDir );
		SetCurrentDirectoryW( sNewDir );
	}
	~CTempCurrentDirectory() { SetCurrentDirectory( m_sOldDir ); }
	TCHAR m_sOldDir[ MAX_PATH ];
};




/// A simple class for string conversion to/from char* to wchar*. It deletes the converted string
/// when it goes out of scope. Usage is:
/// TempStrConv strconv;
/// FunctionExpectingWideString( strconv.Convert( "const char* string" ) );
/// OR: FunctionExpectingCharString( strconv.Convert( someWideString ) );
/// TODO: Consider adding a FastStrConv class which works on a local MAX_PATH length string.
class TempStrConv
{
	char* m_s;
	WCHAR* m_ws;

	void Reset() { delete [] m_s; m_s = NULL; delete [] m_ws; m_ws = NULL; }
public:
	TempStrConv() { m_s = NULL; m_ws = NULL; }
	~TempStrConv() { Reset(); }

	const char* Convert( const WCHAR* ws, UINT codePage )
	{
		Reset();
		if( ws == NULL ) return NULL;
		int requiredLen = WideCharToMultiByte( codePage, 0, ws, -1, NULL, 0, NULL, NULL );
		if( requiredLen <= 0 ) return NULL;
		m_s = new char[ requiredLen+1 ];
		m_s[requiredLen] = 0;
		WideCharToMultiByte( codePage, 0, ws, -1, m_s, requiredLen, NULL, NULL );
		return m_s;
	}

	const WCHAR* Convert( const CHAR* s, UINT codePage )
	{
		Reset();
		if( s == NULL ) return NULL;
		int requiredLen = MultiByteToWideChar( codePage, 0, s, -1, NULL, 0 );
		if( requiredLen <= 0 ) return NULL;
		m_ws = new WCHAR[ requiredLen+1 ];
		m_ws[requiredLen] = 0;
		MultiByteToWideChar( codePage, 0, s, -1, m_ws, requiredLen );
		return m_ws;
	}
};




/// A class for drawing a translucent highlighting rectangle,
/// like the one used when clicking and draggin in Windows Explorer.
/// TODO: Move this to another file (Helpers.h?).
class CHighlightRect
{
public:
# pragma comment( lib, "Msimg32.lib" ) // TODO: remove dependancy?

	HPEN		hPen;
	COLORREF	penColor;
	HDC			hdcMem;
	HBITMAP		bmpMem;
	HBITMAP		oldBmp;

	CHighlightRect();
	~CHighlightRect() { Cleanup(); }

	BOOL Init( );
	VOID Cleanup();
	


	// Fill a rectangle without an outline.
	VOID FillRect( HDC hdc, INT x, INT y, INT cx, INT cy, BYTE blend = 64 );

	VOID FillRect( HDC hdc, const RECT* pRect, BYTE blend = 64 )
	{ this->FillRect( hdc, pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top, blend ); }

	VOID OutlineRect( HDC hdc, INT x, INT y, INT cx, INT cy );

	VOID OutlineRect( HDC hdc, const RECT* pRect )
	{ OutlineRect( hdc, pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top ); }

	// Draw a highlight rectangle with an outline.
	VOID DrawRect( HDC hdc, INT x, INT y, INT cx, INT cy, BYTE blend = 64 )
	{
		this->FillRect( hdc, x, y, cx, cy, blend );
		OutlineRect( hdc, x, y, cx, cy );
	}

	VOID DrawRect( HDC hdc, const RECT* pRect, BYTE blend = 64 )
	{ DrawRect( hdc, pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top, blend ); }
};



//# pragma comment( lib, "Msimg32.lib" )
BOOL GradientFill( HDC hdc, RECT* pRect, COLORREF c1, COLORREF c2, BOOL vertical );



///	Draw a checkered frame. It inverts the target HDC pixels in a patterned shape.
VOID DrawXorFrame( RECT *rect, int border = 3 );



/// Loads an image using GDIPlus and returns the HBITMAP of it, or NULL on failure.
HBITMAP GdiplusLoadImage( LPCTSTR sFile, unsigned* pcx, unsigned* pcy, bool bInit = true );



/// Rotates a bitmap by 90 degrees.
HBITMAP RotateBitmap( HDC hdcMem, HBITMAP bmpMem, INT width, INT height );


/*
DEPRECATED: Use TempStrConv or FastStrConv
inline LPCSTR InlineStrConv( LPCWSTR sw, INT len = -1, UINT codePage = CP_ACP )
{
	static CHAR s[ MAX_PATH ]; s[0] = 0;
	WideCharToMultiByte( codePage, 0, sw, len, s, ARRAY_SIZE(s), 0, NULL );
	if( len >= 0 && len+1 < ARRAY_SIZE(s) )
		s[len]=0;
	return s;
}

inline LPCWSTR InlineStrConv( LPCSTR s, INT len = -1, UINT codePage = CP_ACP )
{
	static WCHAR sw[ MAX_PATH ]; sw[0] = 0;
	MultiByteToWideChar( codePage, 0, s, len, sw, ARRAY_SIZE(sw) );
	if( len >= 0 && len+1 < ARRAY_SIZE(s) )
		sw[len]=0;
	return sw;
}
*/


/// Adapted from http://www.ffuts.org/blog/mapvirtualkey-getkeynametext-and-a-story-of-how-to/
/// Returns name of the specified virtual key code.
VOID GetKeyName( WORD virtualKey, TCHAR* sName, INT cch );

inline LPCTSTR GetKeyName( WORD virtualKey )
{ static TCHAR s[32]; s[0] = 0; GetKeyName( virtualKey, &s[0], ARRAY_COUNT(s) ); return s; }


/// Gets the string representation (name) of an ACCEL structure.
VOID GetAccelName( const ACCEL& accel, TCHAR* sText, INT cch );

inline LPCTSTR GetAccelName( const ACCEL& accel )
{ static TCHAR s[32]; s[0] = 0; GetAccelName( accel, s, ARRAY_COUNT(s) ); return s; }

/// Convert a "hotkey" (as returned from the hotkey control using HKM_GETHOTKEY) to ACCEL.
inline ACCEL HotkeyToAccel( WORD hk )
{
	ACCEL a = {0};
	WORD hkf = HIBYTE(hk);
	if( hkf & HOTKEYF_ALT )			a.fVirt |= FALT;
	if( hkf & HOTKEYF_CONTROL )		a.fVirt |= FCONTROL;
	if( hkf & HOTKEYF_SHIFT )		a.fVirt |= FSHIFT;
	a.key = LOBYTE(hk);
	return a;
}

/// Converts from ACCEL to a hotkey value (as set by HKM_SETHOTKEY).
inline WORD AccelToHotkey( ACCEL a )
{
	WORD hkf = 0;
	if( a.fVirt & FALT )		hkf |= HOTKEYF_ALT;
	if( a.fVirt & FCONTROL )	hkf |= HOTKEYF_CONTROL;
	if( a.fVirt & FSHIFT )		hkf |= HOTKEYF_SHIFT;
	return MAKEWORD( a.key, hkf );
}




/// NOTE: cch is not used?
inline BOOL SetMenuItemText( HMENU hMenu, UINT item, BOOL byPosition, LPCTSTR sText, INT cch )
{
	MENUITEMINFO mii = {0};
	mii.cbSize = sizeof(mii);
	mii.fMask = MIIM_STRING;
	mii.dwTypeData = (LPTSTR)sText;
	mii.cch = cch;
	return 0 != SetMenuItemInfo( hMenu, item, byPosition, &mii );
}


/// Sets the "keyboard shortcut" section of the menu item name. This is the part that cames after '\t'.
BOOL SetMenuItemShortcutText( HMENU hMenu, UINT item, BOOL byPosition, LPCTSTR shortcutText );




/// Gets the shortcut text of the specified menu item, including the leading '\t'.
/// If successful, sBuffer will contain the full menu item text while the returned pointer
/// will point to the shortcut text inside sBuffer. If the menu item text doesn't contain
/// a shortcut text section, the returned pointer will point to the terminating zero.
/// On failure, sBuffer will be empty and the returned pointer is equal to sBuffer.
LPTSTR GetMenuItemShortcutText( HMENU hMenu, UINT item, BOOL byPosition, TCHAR* sBuffer, INT maxBuffer );



/// Just fills sBuffer with the menu item text and replaces the last '\t' character with 0, effectively trimming the
/// shortcut text. Returns NULL on failure. Otherwise it returns the sBuffer argument so it can be used inline.
LPTSTR GetMenuItemTextWithoutShortcut( HMENU hMenu, UINT item, BOOL byPosition, TCHAR* sBuffer, INT maxBuffer );



bool StringStartsWith( const char* sText, int textLen, const char* sPrefix, int prefixLen );
bool StringStartsWith( const WCHAR* sText, int textLen, const WCHAR* sPrefix, int prefixLen );

int FindSubstring( const char* sText, int textLen, const char* sFind, int findLen );
int FindSubstring( const WCHAR* sText, int textLen, const WCHAR* sFind, int findLen );

/// Copies one string to another. The function will 0-terminate the destination buffer,
/// unless its size is 0. It returns the number of characters copied to the destination
/// buffer, not including the terminating 0.
/// The length of the source string can be -1, but the function will stop copying when it
/// encounters a 0 in the source string.
int StrncpyEx( char* dest, int destSize, const char* src, int srcLen );

/// String replacement. Returns the number of characters written to the destination buffer, not including
/// the terminating 0.
/// maxOccurrances can optionally be used to prevent the function from replacing all occurrances
/// of the substring to find.
int ReplaceString( char* dest, int destSize, const char* src, const char* find, const char* replace,
				  int maxOccurrances = -1 );


VOID DebugPrintf( LPCSTR sFmt, ... ); ///< Formats text and sends it to OutputDebugString.
VOID DebugPrintf( LPCWSTR sFmt, ... ); ///< Formats text and sends it to OutputDebugString.

/// Outputs a formatted string to a message box.
INT PrintfMessageBox( HWND hWnd, const TCHAR* sMsgFormat, const TCHAR* sCaption, UINT uType, ... );

/// Formats text and sends it to OutputDebugString, and attaches the function name and a new line character.
/// Quite useful for outputting debugging information.
/// Note that this becomes an empty macro in non-debug builds, so beware of side effects.
# ifdef DEBUG
# define DebugPrintfLine( sFmt, ... ) { DebugPrintf( sFmt, __VA_ARGS__ ); DebugPrintf(" - "__FUNCTION__"()\n"); }
# else
# define DebugPrintfLine( sFmt, ... ) ((void)0)
# endif


#ifdef DEBUG
bool GuiAssertFunc( const char* exp, const char* sFile, unsigned line );
/// An assert which will block the current thread, which ensures the application will not
/// be receiving and processing GUI messages while the assertion dialog is shown.
/// Based on work from Ivo Beltchev
/// (http://www.codeproject.com/Articles/20853/Win32-Tips-and-Tricks#assert)
#define GuiAssert(exp) do{ if (!(exp) && !GuiAssertFunc(#exp,__FILE__,__LINE__)) \
							DebugBreak(); } while (0)
# else
#define GuiAssert(exp) ((void)0)
# endif


/// Creates a font that is the same as that used by Windows for GUI elements.
inline HFONT CreateGuiFont()
{
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
	return CreateFontIndirect(&ncm.lfMessageFont);
}


/// On the first call, it creates a font identical to what Windows used for GUI. Subsequent
/// calls return this font.
inline HFONT GetGuiFont()
{
	static HFONT hGuiFont = NULL;
	if( !hGuiFont ) hGuiFont = CreateGuiFont();
	return hGuiFont;
}


/// Returns the TEXTMETRIC object for the font used with the specified window. Uses WM_GETFONT
/// to get the window's font.
inline VOID GetTextMetrics( HWND hWnd, TEXTMETRIC* pTM )
{
	ZeroMemory( pTM, sizeof(TEXTMETRIC) );
	HDC hDC = GetDC( hWnd );
	HGDIOBJ hOldFont = SelectObject( hDC, (HGDIOBJ)SendMessage( hWnd, WM_GETFONT, 0, 0 ) );
	::GetTextMetrics( hDC, pTM );
	SelectObject( hDC, hOldFont );
	ReleaseDC( hWnd, hDC );
}




inline BOOL IsMouseMessage( UINT msg )
{ return msg > WM_MOUSEFIRST && msg < WM_MOUSELAST; }




/// Loads a web page into the specified buffer.
/// Returns TRUE on success, FALSE on failure (use GetLastError()).
inline BOOL LoadWebPage( LPCSTR sURL, CHAR* sBuffer, INT maxBuffer, DWORD* pBytesRead = NULL )
{
	DWORD bytesRead;
	pBytesRead = pBytesRead ? pBytesRead : &bytesRead;
	*pBytesRead = 0;
	BOOL bSuccess = FALSE;

	HINTERNET hInternet = InternetOpen( TEXT("win32"), INTERNET_OPEN_TYPE_PRECONFIG, 0, 0, 0 );
	if( hInternet )
	{
		HINTERNET hAddress = InternetOpenUrlA( hInternet, sURL, NULL, 0,
									INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION, 0 );
		if( hAddress )
		{
			if( InternetReadFile( hAddress, sBuffer, maxBuffer, pBytesRead ) )
				bSuccess = TRUE;

			InternetCloseHandle( hAddress );
		}
		InternetCloseHandle( hInternet );
	}

	return bSuccess;
}




/// Timer object.
struct Timer
{
	Timer()
	{
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		freq = double(li.QuadPart)/1000.0;
		counterStart = 0;
	}

	void Start()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		counterStart = li.QuadPart;
	}

	double End()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return double(li.QuadPart-counterStart)/freq;
	}

private:
	__int64 counterStart;
	double freq;
};




# endif // inclusion guard.