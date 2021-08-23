// Implementation file for the non-inline functions and methods in Win32Util.h.

# include "Win32Util.h"




/*
# include <GdiPlus.h>
# pragma comment( lib, "gdiplus.lib" )
HBITMAP GdiplusLoadImage( LPCTSTR sFile, unsigned* pcx, unsigned* pcy, bool bInit )
{
	ULONG_PTR gdipToken = 0;
	Gdiplus::GdiplusStartupInput gdipStartupInput;

	if( bInit )
	{
		if( Gdiplus::Ok != Gdiplus::GdiplusStartup( &gdipToken, &gdipStartupInput, NULL ) )
			return NULL;
	}

	HBITMAP hBitmap = NULL;
	Gdiplus::Bitmap* pImage = Gdiplus::Bitmap::FromFile( sFile, FALSE );
	if( pImage )
	{
		if( pcx ) *pcx = pImage->GetWidth();
		if( pcy ) *pcy = pImage->GetHeight();
		Gdiplus::Color bg( 0, 0, 0, 0 );
		pImage->GetHBITMAP( bg, &hBitmap );
		delete pImage;
	}


	if( bInit )
		Gdiplus::GdiplusShutdown( gdipToken );

	return hBitmap;
}
*/



WindowSubclass::WindowSubclass( HWND _hWnd )
{
	hWnd = _hWnd;
	if( GetWindowLongPtr( hWnd, GWLP_USERDATA ) )
	{
		GuiAssert( "WindowSubclass(): user data field must not be used" == 0 );
		return;
	}
	originalProc = (WNDPROC)SetWindowLongPtr( hWnd, GWLP_WNDPROC, (LONG_PTR)sMsgProc );
	SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)this );
}



WindowSubclass::~WindowSubclass()
{
	SetWindowLongPtr( hWnd, GWLP_WNDPROC, (LONG_PTR)originalProc );
	SetWindowLongPtr( hWnd, GWLP_USERDATA, 0 );
}


LRESULT CALLBACK WindowSubclass::sMsgProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	WindowSubclass* p = (WindowSubclass*)GetWindowLongPtr( hWnd, GWLP_USERDATA );
	if( !p )
		return DefWindowProc( hWnd, Msg, wParam, lParam );
	return p->MsgProc( Msg, wParam, lParam );
}




CHighlightRect::CHighlightRect()
{
	hPen = NULL;
	penColor = GetSysColor( COLOR_HIGHLIGHT );
	hdcMem = NULL;
	bmpMem = NULL;
	oldBmp = NULL;
	Init();
}





BOOL CHighlightRect::Init( )
{
	Cleanup();
	HDC hDC = GetDC( GetDesktopWindow() );
	penColor = GetSysColor( COLOR_HIGHLIGHT );
	hPen = CreatePen( PS_SOLID, 1, penColor );
	hdcMem = CreateCompatibleDC( hDC );
	bmpMem = CreateCompatibleBitmap( hDC, 1, 1 );
	ReleaseDC( GetDesktopWindow(), hDC );
	oldBmp = (HBITMAP)SelectObject( hdcMem, bmpMem );
	SetPixel( hdcMem, 0, 0, penColor );
	if( NULL == hPen || NULL == hdcMem || NULL == bmpMem )
	{
		Cleanup();
		return FALSE;
	}
	return TRUE;
}




VOID CHighlightRect::Cleanup()
{
	if( hPen )		{ DeleteObject( hPen ); hPen = NULL; }
	if( oldBmp )	{ SelectObject( hdcMem, oldBmp ); }
	if( bmpMem )	{ DeleteObject( bmpMem ); }
	if( hdcMem )	{ DeleteDC( hdcMem ); }
}





VOID CHighlightRect::FillRect( HDC hdc, INT x, INT y, INT cx, INT cy, BYTE blend )
{
	// We also do this to avoid having to call Init() when the theme changes, which
	// doesn't work always anyway for some reason!
	if( GetSysColor( COLOR_HIGHLIGHT ) != penColor )
		if( FALSE == Init() )
			return;

	BLENDFUNCTION blendFunction;
	blendFunction.SourceConstantAlpha = blend;
	blendFunction.BlendOp = AC_SRC_OVER;
	blendFunction.BlendFlags = 0;
	blendFunction.AlphaFormat = 0;
	AlphaBlend( hdc, x, y, cx, cy, hdcMem, 0, 0, 1, 1, blendFunction );
}




VOID CHighlightRect::OutlineRect( HDC hdc, INT x, INT y, INT cx, INT cy )
{
	HPEN oldPen = (HPEN)SelectObject( hdc, hPen );
	MoveToEx( hdc, x, y, NULL );
	LineTo( hdc, x + cx - 1,	y );
	LineTo( hdc, x + cx - 1,	y + cy - 1 );
	LineTo( hdc, x,				y + cy - 1 );
	LineTo( hdc, x,				y );
	SelectObject( hdc, oldPen );
}






BOOL GradientFill( HDC hdc, RECT* pRect, COLORREF c1, COLORREF c2, BOOL vertical )
{
	TRIVERTEX vert[2];

	vert[0].x		= pRect->left;
	vert[0].y		= pRect->top;
	vert[0].Red		= GetRValue( c1 ) << 8;
	vert[0].Green	= GetGValue( c1 ) << 8;
	vert[0].Blue	= GetBValue( c1 ) << 8;
	vert[0].Alpha	= 0;

	vert[1].x		= pRect->right;
	vert[1].y		= pRect->bottom;
	vert[1].Red		= GetRValue( c2 ) << 8;
	vert[1].Green	= GetGValue( c2 ) << 8;
	vert[1].Blue	= GetBValue( c2 ) << 8;
	vert[1].Alpha	= 0;

	GRADIENT_RECT gRect;
	gRect.UpperLeft = 0;
	gRect.LowerRight = 1;

	return ::GradientFill( hdc, vert, 2, &gRect, 1,
		vertical ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H );
}





VOID AdjustPopupRect( RECT& rWindow, BOOL bCenter )
{
	INT cx = GetSystemMetrics( SM_CXFULLSCREEN );
	INT cy = GetSystemMetrics( SM_CYFULLSCREEN );

	// Move the window up and to the left if needed.
	INT dx = MIN( 0, cx - rWindow.right );
	INT dy = MIN( 0, cy - rWindow.bottom );
	OffsetRect( &rWindow, dx, dy );
	dx = 0 - MIN(rWindow.left, 0);
	dy = 0 - MIN(rWindow.top, 0 );
	OffsetRect( &rWindow, dx, dy );
	rWindow.right = MIN( rWindow.right, cx );
	rWindow.bottom = MIN( rWindow.bottom, cy );
	if( bCenter )
	{
		LONG w = RectWidth( rWindow );
		LONG h = RectHeight( rWindow );
		rWindow.left = cx/2 - w/2;
		rWindow.top = cy/2 - h/2;
		rWindow.right = rWindow.left + w;
		rWindow.bottom = rWindow.top + h;
	}
}






VOID DrawXorFrame( RECT *rect, int border )
{
	static WORD _dotPatternBmp[] = 
	{
		0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055
		//0xaaaa, 0x5555, 0xaaaa, 0x5555, 0xaaaa, 0x5555, 0xaaaa, 0x5555
	};

	HBITMAP hbm;
	HBRUSH  hbr;
	HANDLE  hbrushOld;
	WORD    *bitmap;

	int width, height, x, y;

	HDC hdc = GetDC( NULL );

	bitmap = _dotPatternBmp;
	
	x = rect->left;
	y = rect->top;
	width  = rect->right-rect->left;
	height = rect->bottom-rect->top;

	hbm = CreateBitmap(8, 8, 1, 1, bitmap);
	hbr = CreatePatternBrush(hbm);

	SetBrushOrgEx(hdc, x, y, 0);
	hbrushOld = SelectObject(hdc, hbr);

	PatBlt(hdc, x+border,       y,               width-border,  border,        PATINVERT);
	PatBlt(hdc, x+width-border, y+border,        border,        height-border, PATINVERT);
	PatBlt(hdc, x,              y+height-border, width-border,  border,        PATINVERT);
	PatBlt(hdc, x,              y,               border,        height-border, PATINVERT);

	SelectObject(hdc, hbrushOld);
	DeleteObject(hbr);
	DeleteObject(hbm);
	ReleaseDC(0, hdc);
}




HBITMAP RotateBitmap( HDC hdcMem, HBITMAP bmpMem, INT width, INT height )
{
	// inline assembly can't be used in x64. For LSED, this function is not needed at all.
	return NULL;
	/*
	// Init some extra parameters.
	BITMAPINFO bmiOut;
	BITMAPINFOHEADER& bmihOut = bmiOut.bmiHeader;
	ZeroMemory( &bmiOut, sizeof( BITMAPINFO ) );
	bmihOut.biSize = sizeof( BITMAPINFOHEADER );
	bmihOut.biCompression = BI_RGB;
	bmihOut.biPlanes = 1;
	bmihOut.biBitCount = 24;
	bmihOut.biWidth = width;
	bmihOut.biHeight = height;

	INT bmpWidthPS = Align( width * 3, sizeof(DWORD) );
	INT nBufferSize = ((bmpWidthPS * height)/8 + 2) * 8; // beats me!
	LPBYTE pixels = new BYTE[ nBufferSize ];

	GetDIBits( hdcMem, bmpMem, 0, height, pixels, &bmiOut, DIB_RGB_COLORS );


	// Swap width and height for use with the rotated bitmap.
	INT temp = width; width = height; height = temp;

	INT bmpWidthPadded = Align( width * 3, sizeof(DWORD) );
	INT padding = bmpWidthPadded - width * 3;
	INT bufferSize = ((bmpWidthPadded * height)/8 + 2)*8;
	LPBYTE rotatedPixels = new BYTE[ bufferSize ];
	INT nWidth, nHeight = height, nHeight1 = nHeight - 1;

	bmpWidthPS -= 4;
	nWidth = width;
	__asm
	{
			mov		esi, pixels			// source index
			mov		edi, rotatedPixels	// destination index
			xor		ebx, ebx			// vertical counter
		loop_height:
			mov		ecx, nWidth			// horizontal counter
			cmp		ebx, nHeight1		// check is it the last line
			jne		loop_width
			dec		ecx					// if it is decremnt for the last pixel

		loop_width:
			movsd						// copies 4 bytes and increments source and destination by 4 (we need only 3 bytes copied 'one pixel' RGB triplet)
			dec		edi					// adjust edi to 'as incremented by 3'
			add		esi,bmpWidthPS		// adjust esi to the next source line
			loop	loop_width			// loop one hotizontal destination line 

			cmp		ebx, nHeight1		// check is it the last line
			je		do_last				// if not last, do incrementing here

			inc		ebx					// increment vertical counter
			add		edi, padding		// adjust destination index by possible padding to DWORD
			mov		esi, ebx			// reset the source index: add vertical counter * 3 
			shl		esi, 1				// (is the same as * 2 +1*)
			add		esi, ebx			// +1*
			add		esi, pixels			// add to the beginning of the source
			jmp		loop_height			// loop whole height

		do_last:						// the last pixel is done by
			movsw						// moving first two bytes
			movsb						// and than by moving the very last byte
	}

	// Ok, now that we have a rotated version of the original bmp, we can return it.
	delete [] pixels;

	HBITMAP bmpRotated = CreateCompatibleBitmap( hdcMem, width, height );
	HBITMAP bmpOld = (HBITMAP)SelectObject( hdcMem, bmpRotated );
	bmihOut.biWidth = width;
	bmihOut.biHeight = height;
	SetDIBits( hdcMem, bmpRotated, 0, height, rotatedPixels, &bmiOut, DIB_RGB_COLORS );
	SelectObject( hdcMem, bmpOld );
	delete [] rotatedPixels;
	return bmpRotated;
	*/
}



VOID GetKeyName( WORD virtualKey, TCHAR* sName, INT cch )
{
    unsigned int scanCode = MapVirtualKey(virtualKey, 0);

    // because MapVirtualKey strips the extended bit for some keys
    switch (virtualKey)
    {
        case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN: // arrow keys
        case VK_PRIOR: case VK_NEXT: // page up and page down
        case VK_END: case VK_HOME:
        case VK_INSERT: case VK_DELETE:
        case VK_DIVIDE: // numpad slash
        case VK_NUMLOCK:
        {
            scanCode |= 0x100; // set extended bit
            break;
        }
    }
	GetKeyNameText( scanCode << 16, sName, cch );
}




VOID GetAccelName( const ACCEL& accel, TCHAR* sText, INT cch )
{
	TCHAR keyName[16];
	keyName[0] = 0;
	GetKeyName( accel.key, keyName, ARRAY_COUNT(keyName) );
	sText[0] = 0;
	StringCchPrintf( sText, cch, TEXT("%s%s%s%s"),
	//_sntprintf( sText, cch, TEXT("%s%s%s%s"),
		(accel.fVirt & FCONTROL) ? TEXT("Ctrl + ") : TEXT(""),
		(accel.fVirt & FALT) ? TEXT("Alt + ") : TEXT(""),
		(accel.fVirt & FSHIFT) ? TEXT( "Shift + " ) : TEXT(""),
		keyName
		);
}




BOOL SetMenuItemShortcutText( HMENU hMenu, UINT item, BOOL byPosition, LPCTSTR shortcutText )
{
	TCHAR sText[ MAX_PATH ];
	sText[0] = 0;
	INT len = GetMenuString( hMenu, item, sText, ARRAY_SIZE(sText), byPosition ? MF_BYPOSITION : MF_BYCOMMAND );
	if( len == 0 )
		return FALSE;
	// Look for first occurance of \t or append at the end of the name.
	INT i = 0;
	for( ; i<len; i++ )
		if( sText[i] == TEXT('\t') )
			break;
	sText[i] = TEXT('\t');
	sText[i+1] = 0;
	if( shortcutText && shortcutText[0] != 0 )
		//_tcsncat( sText + i, shortcutText, MAX_PATH - i );
		StringCchCat( sText + i, ARRAY_COUNT(sText) - i, shortcutText );
	return SetMenuItemText( hMenu, item, byPosition, sText, ARRAY_SIZE(sText) );
}



LPTSTR GetMenuItemShortcutText( HMENU hMenu, UINT item, BOOL byPosition, TCHAR* sBuffer, INT maxBuffer )
{
	if( sBuffer == NULL || maxBuffer <= 0 ) return NULL;
	sBuffer[0] = 0;
	INT len = GetMenuString( hMenu, item, sBuffer, maxBuffer, byPosition ? MF_BYPOSITION : MF_BYCOMMAND );
	if( len == 0 )
		return sBuffer;
	INT iTab;
	for( iTab=0; iTab<len; iTab++ )
		if( sBuffer[iTab] == TEXT('\t') )
			break;
	if( iTab >= len )
	{
		sBuffer[0] = 0;
		return sBuffer;
	}
	return &sBuffer[iTab];
}




LPTSTR GetMenuItemTextWithoutShortcut( HMENU hMenu, UINT item, BOOL byPosition, TCHAR* sBuffer, INT maxBuffer )
{
	if( !hMenu )
		return 0;

	INT len = GetMenuString( hMenu, item, sBuffer, maxBuffer, byPosition ? MF_BYPOSITION : MF_BYCOMMAND );
	if( len == 0 )
		return 0;
	for( INT i=0; i<len; i++ )
	{
		if( sBuffer[i] == TEXT('\t') )
		{
			sBuffer[i] = 0;
			return sBuffer;
		}
	}
	return sBuffer;
}





bool StringStartsWith( const char* sText, int textLen, const char* sPrefix, int prefixLen )
{
	// loop until either of the strings run out, or we find a mismatch.
	for( const char* s = sText, *sp = sPrefix;
		1;
		s++, sp++ )
	{
		if( *sp == 0 || (prefixLen >= 0 && sp == sPrefix + prefixLen) )
			return true; // sPrefix has run out without finding mismatches. It's a prefix.
		if( *s == 0 || (textLen >= 0 && s >= sText + textLen) )
			return false; // sText has run out before matching sPrefix.
		if( *sp != *s )
			return false;
	}
	return false;
}



bool StringStartsWith( const WCHAR* sText, int textLen, const WCHAR* sPrefix, int prefixLen )
{
	// loop until either of the strings run out, or we find a mismatch.
	for( const WCHAR* s = sText, *sp = sPrefix;
		1;
		s++, sp++ )
	{
		if( *sp == 0 || (prefixLen >= 0 && sp == sPrefix + prefixLen) )
			return true; // sPrefix has run out without finding mismatches. It's a prefix.
		if( *s == 0 || (textLen >= 0 && s >= sText + textLen) )
			return false; // sText has run out before matching sPrefix.
		if( *sp != *s )
			return false;
	}
	return false;
}



int FindSubstring( const char* sText, int textLen, const char* sFind, int findLen )
{
	if( !sText && sFind && sFind[0] != 0 )
		return -1;
	for( int i = 0; sText[i] != 0 && !(textLen >= 0 && i >= textLen); i++ )
		if( StringStartsWith( &sText[i], textLen >= 0 ? (textLen-i) : -1, sFind, findLen ) )
			return i;
	return -1;
}



int FindSubstring( const WCHAR* sText, int textLen, const WCHAR* sFind, int findLen )
{
	if( !sText && sFind && sFind[0] != 0 )
		return -1;
	for( int i = 0; sText[i] != 0 && !(textLen >= 0 && i >= textLen); i++ )
		if( StringStartsWith( &sText[i], textLen >= 0 ? (textLen-i) : -1, sFind, findLen ) )
			return i;
	return -1;
}


/// Copies one string to another. The function will 0-terminate the destination buffer,
/// unless its size is 0. It returns the number of characters copied to the destination
/// buffer, not including the terminating 0.
/// The length of the source string can be -1, but the function will stop copying when it
/// encounters a 0 in the source string.
int StrncpyEx( char* dest, int destSize, const char* src, int srcLen )
{
	if( !dest || destSize <= 0 || !src )
		return 0;

	int i = 0;
	for( ; i<destSize-1; i++ )
	{
		if( (srcLen >= 0 && i >= srcLen) || *src == 0 )
			break;
		*dest++ = *src++;
	}
	*dest = 0;
	return i;
}



int ReplaceString( char* dest, int destSize, const char* src, const char* find, const char* replace,
				  int maxOccurrances )
{
	if( destSize <= 0 )
		return 0;

	if( maxOccurrances == 0 )
		return StrncpyEx( dest, destSize, src, -1 );

	int findLen = strlen( find );
	int replaceLen = strlen( replace );
	int numWritten = 0;
	int occurrances = 0;
	do
	{
		int pos = FindSubstring( src, -1, find, findLen );
		int n = StrncpyEx( dest + numWritten, destSize - numWritten, src, pos );
		numWritten += n;
		src += n;

		if( pos < 0 )
			return numWritten;

		n = StrncpyEx( dest + numWritten, destSize - numWritten, replace, replaceLen );
		numWritten += n;
		src += findLen;

		if( ++occurrances == maxOccurrances )
			return numWritten;

		if( numWritten >= destSize-1 )
			return numWritten;
	} while(1);
	return 0;
}






VOID DebugPrintf( LPCSTR sFmt, ... )
{
	if( !sFmt || sFmt[0] == 0 )
		return;
	va_list vList;
	va_start( vList, sFmt );
	static char sBuffer[ 1024 ];
	sBuffer[0] = 0;

	StringCchVPrintfA( sBuffer, ARRAY_SIZE(sBuffer), sFmt, vList );
	OutputDebugStringA( sBuffer );
}



VOID DebugPrintf( LPCWSTR sFmt, ... )
{
	if( !sFmt || sFmt[0] == 0 )
		return;
	va_list vList;
	va_start( vList, sFmt );
	static WCHAR sBuffer[ 1024 ];
	sBuffer[0] = 0;
	StringCchVPrintfW( sBuffer, ARRAY_SIZE(sBuffer), sFmt, vList );
	OutputDebugStringW( sBuffer );
}



INT PrintfMessageBox( HWND hWnd, const TCHAR* sFmt, const TCHAR* sCaption, UINT uType, ... )
{
	/*if( !sFmt || sFmt[0] == 0 )
		return MessageBox( hWnd, sFmt, sCaption, uType );

	va_list vList;
	va_start( vList, uType );
	static WCHAR sBuffer[ 1024 ];
	sBuffer[0] = 0;
	StringCchVPrintfW( sBuffer, ARRAY_SIZE(sBuffer), sFmt, vList );
	return MessageBox( hWnd, sBuffer, sCaption, uType );*/
	return 0;
}

# ifdef DEBUG
static DWORD _stdcall GuiAssertThreadProc( void *param )
{
  return MessageBoxA(NULL,(const char *)param,"Assertion Failed",
      MB_ABORTRETRYIGNORE|MB_TASKMODAL|MB_ICONERROR);
}

bool GuiAssertFunc( const char *exp, const char *file, unsigned line )
{
  char buf[2048];
  StringCchPrintfA( buf, ARRAY_SIZE(buf), "Expression: %s\r\nFile: %s\r\nLine: %d\n", exp, file, line);
  //sprintf(buf,"Expression: %s\r\nFile: %s\r\nLine: %d\n",exp,file,line);
  HANDLE h=CreateThread(NULL,0,GuiAssertThreadProc,buf,0,NULL);
  if (h) {
    WaitForSingleObject(h,INFINITE);
    DWORD res=IDRETRY;
    GetExitCodeThread(h,&res);
    if (res==IDABORT)
      TerminateProcess(GetCurrentProcess(),3);
    return (res==IDIGNORE); // true will continue, false will cause DebugBreak()
  }
  return true;
}
# endif