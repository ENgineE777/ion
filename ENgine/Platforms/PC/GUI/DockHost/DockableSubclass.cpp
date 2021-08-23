// This contains the message processor and related functions used by the docking system for
// floating windows to display the docking diamond.


# include <Windows.h>
# include "..\CSTB\Win32Util.h"
# include "DockHost.h"
# include "DockDiamond.h"



HWND		g_hDockDiamond = NULL;
HWND		g_hDraggingWnd = NULL;



struct DockableWindowSubclass : public WindowSubclass
{
	DockableWindowSubclass( HWND _hWnd, HWND _hDockHost,DWORD _dwNodeStyle,int _minWidth, int _minHeight,int _resizability) :WindowSubclass( _hWnd ),hDockHost(_hDockHost),resizability(_resizability)
        ,dwNodeStyle(_dwNodeStyle),minWidth(_minWidth),minHeight(_minHeight)
    {}


	void OnMoving()
	{
		if( (GetWindowLong( hWnd, GWL_STYLE ) & WS_CHILD) )
			return;
		POINT ptCursor;
		GetCursorPos( &ptCursor );
		RECT rDockHost;
		GetWindowRect( hDockHost, &rDockHost );

		if( g_hDraggingWnd == NULL )
		{
			g_hDockDiamond = DockDiamond_Create(
				(HINSTANCE)GetWindowLongPtr( hWnd, GWLP_HINSTANCE ),
				hWnd, rDockHost, RectCenter( rDockHost ) );
			if( !g_hDockDiamond )
				return;
			g_hDraggingWnd = hWnd;
		}

		if( ISKEYDOWN( VK_CONTROL ) || IsRectEmpty( &rDockHost ) )
		{
			ShowWindow( g_hDockDiamond, SW_HIDE );
			return;
		}

		POINT ptCenter = RectCenter( rDockHost );
		HDHNODE hNode = DockHost_LeafNodeFromPoint( hDockHost, ptCursor );
		if( hNode )
		{
			RECT r;
			DockHost_GetNodeRect( hDockHost, hNode, &r );
			ptCenter = RectCenter( r );
			ptCenter = ClientToScreen( hDockHost, ptCenter );
		}

		// TODO: Try moving the ShowWindow() call below the SetLayout() call.
		ShowWindow( g_hDockDiamond, SW_SHOW );
		DockDiamond_SetLayout( g_hDockDiamond, rDockHost, ptCenter );
		DockDiamond_Track( g_hDockDiamond, ptCursor );
	}



	// Dock the window when ther users stops moving the window if he drops it on a dock diamond button.
	void OnExitSizeMove()
	{
		if( (GetWindowLong( hWnd, GWL_STYLE ) & WS_CHILD) || !g_hDraggingWnd )
			return;

		g_hDraggingWnd = NULL;

		DD_Button hotButton = DockDiamond_GetHotButton( g_hDockDiamond );

		SAFE_DESTROY_WINDOW( g_hDockDiamond );

		if( ISKEYDOWN( VK_CONTROL ) || hotButton == DDB_NOWHERE )
			return;
        RECT rWindow;
        GetWindowRect( hWnd, &rWindow );
		DockHost_Dock( hDockHost,hWnd, (DH_DockLocation)hotButton,DockHost_LeafNodeFromPoint( hDockHost, GetCursorPos() ),dwNodeStyle,RectWidth(rWindow),RectHeight(rWindow),minWidth,minHeight,resizability);
	}



	LRESULT MsgProc( UINT Msg, WPARAM wParam, LPARAM lParam )
	{
		switch( Msg )
		{
			case WM_MOVING: OnMoving(); break;
			case WM_EXITSIZEMOVE: OnExitSizeMove(); break;
			case WM_NCDESTROY:
				{
					LRESULT ret = WindowSubclass::MsgProc( Msg, wParam, lParam );
					delete this;
					return ret;
				}
				break;
		}
		return WindowSubclass::MsgProc( Msg, wParam, lParam );
	}


	HWND    hDockHost;
	int     resizability;
    DWORD   dwNodeStyle;
    int     minWidth;
    int     minHeight;
};



// Subclasses the window so that it will be dockable.
BOOL MakeDockable( HWND hWnd, HWND hDockHost,DWORD dwNodeStyle,INT minWidth, INT minHeight,INT resizability)
{
	if( !hWnd || !hDockHost ) return FALSE;
	if( !DOCKDIAMOND_Init( (HINSTANCE)GetWindowLongPtr( hWnd, GWLP_HINSTANCE ) ) )
	{
		OutputDebugString( TEXT("Failed to initialize dock diamond control") );
		return FALSE;
	}

	new DockableWindowSubclass( hWnd, hDockHost,dwNodeStyle, minWidth, minHeight, resizability );

	return TRUE;
}