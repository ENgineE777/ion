/// @file
/// This file contains the implementation of a simpel custom win32 window. It demonstrates how to
/// create a simple control (window) and associate custom data with it.
/// @see CustomWindow.h

# include "CustomWindow.h"
# include <strsafe.h>


namespace CustomWindow
{

LPCTSTR WC_CUSTOMWINDOW = TEXT("code-section.custom-window");
/// This structure is associated with each of our custom windows. We allocate an instance
/// and assign it to the window when the window is created, and we delete it when the
/// window is destroyed.
struct TWindowInfo
{
	COLORREF	color;
	HBRUSH		hBrush;
	TWindowInfo()
	{
		color = RGB(255,255,255);
		hBrush = CreateSolidBrush( color );
	}

	~TWindowInfo()
	{
		DeleteObject( hBrush );
	}
};




// Disable "conversion from LONG to ptr of different size" warnings.
# pragma warning( push )
# pragma warning( disable: 4311 )
# pragma warning( disable: 4312 )

TWindowInfo*	GetInfo( HWND hWnd ) { return (TWindowInfo*)GetWindowLongPtr( hWnd, 0 ); }
VOID			SetInfo( HWND hWnd, TWindowInfo* pWI ) { SetWindowLongPtr( hWnd, 0, (LONG)pWI ); }

# pragma warning( pop )



/// The message processor function of the custom window.
LRESULT CALLBACK WindowProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	// Retrieve the custom window info associated with the window.
	TWindowInfo* pInfo = GetInfo( hWnd );

	// We allocate an instance of our custom window info structure (TWindowInfo) and assign
	// it to the window when the WM_NCCREATE message is received. This is not guaranteed
	// to be the first message received however, so we need to check the return value of GetInfo( HWND )
	// for NULL.
	if( Msg == WM_NCCREATE )
	{
		pInfo = new TWindowInfo;
		SetInfo( hWnd, pInfo );
	}
	else if( Msg == WM_NCDESTROY )
	{
		//assert( pInfo != NULL );
		delete pInfo;
		pInfo = NULL;
	}

	if( !pInfo )
		return DefWindowProc( hWnd, Msg, wParam, lParam );


	switch( Msg )
	{
	case WM_CLOSE:
		DestroyWindow( hWnd );
		break;


	case WM_LBUTTONDOWN:
	case WM_SIZE:
		{
			RECT rClient;
			GetClientRect( hWnd, &rClient );
            InvalidateRect( hWnd, NULL, FALSE );
			UpdateWindow( hWnd );
		}
		break;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdcPaint = BeginPaint( hWnd, &ps );
			if( !hdcPaint )
				break;
			RECT rClient;
			GetClientRect( hWnd, &rClient );
			FillRect( hdcPaint, &rClient, pInfo->hBrush );
			EndPaint( hWnd, &ps );
		}
		break;
	}

	return DefWindowProc( hWnd, Msg, wParam, lParam );
}




/// Updates the color in the custom window.
VOID SetColor( HWND hWnd, COLORREF color, BOOL bRepaint )
{
	TWindowInfo* pInfo = GetInfo( hWnd );
	pInfo->color = color;
	DeleteObject( pInfo->hBrush );
	pInfo->hBrush = CreateSolidBrush( color );
	if( bRepaint )
	{
		InvalidateRect( hWnd, NULL, TRUE );
		UpdateWindow( hWnd );
	}
}




BOOL Initialize( HINSTANCE hInstance )
{
	// Register the color box window class.
	WNDCLASSEX wcex;
	ZeroMemory( &wcex, sizeof(WNDCLASSEX) );

	wcex.cbSize = sizeof( WNDCLASSEX );
	wcex.lpfnWndProc = WindowProc;
	wcex.cbWndExtra = sizeof( TWindowInfo* );
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcex.lpszClassName = WC_CUSTOMWINDOW;
	if( 0 == RegisterClassEx( &wcex ) )
	{
		OutputDebugString( TEXT( "Failed to register the custom window class!" ) );
		return FALSE;
	}
	return TRUE;
}




VOID Uninitialize( HINSTANCE hInstance )
{
	UnregisterClass( WC_CUSTOMWINDOW, hInstance );
}


}; // namespace CustomWindow