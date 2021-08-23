// Mouse-related functionality in the DockHost control.


# include "DockHostImpl.h"
//# include <CommCtrl.h>
//# pragma comment( lib, "commctrl.lib" )



HDHNODE hClickedNode = NULL;
POINT ptMouseStart = {0,0};
POINT ptPrevCursor = {0,0};
BOOL bDetectDrag = FALSE;


static CloseButton hotCloseButton;
const CloseButton& DockHost_GetHotCloseButton() { return hotCloseButton; }



VOID SetHotCloseButton( HWND hWnd, HDHNODE hNode, const RECT* prClose, BOOL bUpdate = TRUE )
{
	if( hotCloseButton.hNode == hNode )
		return; // Nothing to do.

	// If there was a hot button, make it cold.
	if( hotCloseButton.hNode )
		InvalidateRect( hotCloseButton.hWnd, &hotCloseButton.rClose, FALSE );

	hotCloseButton.hWnd = hWnd;
	hotCloseButton.hNode = hNode;
	if( hNode )
	{
		hotCloseButton.rClose = *prClose;
		InvalidateRect( hotCloseButton.hWnd, &hotCloseButton.rClose, FALSE );
	}
	if( bUpdate )
		UpdateWindow( hWnd );
}





// This is called by the WM_MOUSEMOVE handler if a divider is being dragged.
// It calculates an offset for the divider, and resizes the divider's children accordingly.
// ptCursor is assumed to be in client coordinates.
VOID DividerDragging( TDockHost* pDH, DHDividerNode* pDragDivider, POINT ptCursor )
{
	INT d;
	if( pDragDivider->type == DH_DIVIDER_H )
		d = ptCursor.y - ptPrevCursor.y;
	else
		d = ptCursor.x - ptPrevCursor.x;
	ptPrevCursor = ptCursor;

	if( d == 0 )
		return;

	DockHost_SetDividerPosition( pDH->hWnd, CONV(pDragDivider), DockHost_GetDividerPosition( CONV(pDragDivider) ) + d, FALSE );
}





LRESULT DockHostMouseMsgProc( TDockHost* pDH, UINT Msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	HWND hWnd = pDH->hWnd;

	switch( Msg )
	{
	case WM_LBUTTONDOWN:
		{
			bHandled = TRUE;
			POINT ptCursor = { LOWORD( lParam ), HIWORD( lParam ) };
			ptMouseStart = ptCursor;
			ptPrevCursor = ptCursor;

			hClickedNode = DockHost_HitTest( hWnd, ptCursor, NULL );

			if( !hClickedNode )
				break;

            if( GetDividerNode( hClickedNode ) )
                DockHost_Notify( hWnd, DHN_START_RESIZE, hClickedNode );

			// Capture the mouse.
			SetCapture( hWnd );

			// See if the user has clicked the close button.
			if( DockHost_GetHotCloseButton().hNode == hClickedNode &&
				GetLeafNode( hClickedNode ) &&
				GetLeafNode( hClickedNode )->hWnd != NULL )
			{
				// If the notified windows allow it, undock the window and close it.
				if( 0 == DockHost_Notify( hWnd, DHN_CLOSE, hClickedNode ) && !(DockHost_GetNodeStyle( hClickedNode ) & DHNS_NO_CLOSE))
				{
					HWND hClientWnd = GetLeafNode( hClickedNode )->hWnd;

                    if( !hClickedNode || CONV(hClickedNode)->type != DH_LEAF )
                        return FALSE;

                    if( 0 != DockHost_Notify( hWnd, DHN_ITEMUNDOCK, hClickedNode ) )
                        return FALSE;

                    CONV(hWnd)->pLastHitNode = NULL;
                    {
                        DHNode* pNode = CONV(hClickedNode);
                        DockHost_RemoveNode( hWnd, CONV(hClickedNode) );
                        delete pNode;
                    }

					SendMessage( hClientWnd, WM_CLOSE, 0, 0 );
					hClickedNode = NULL;
				}
				break;
			}

			// If the user has clicked on the caption area, set a flag to wait for him to drag it off.
			if( CONV(hClickedNode)->type == DH_LEAF &&	GetLeafNode( hClickedNode ) &&	GetLeafNode( hClickedNode )->hWnd != NULL )
			{
				RECT rCaption, rNode;
				DockHost_GetNodeRect( pDH->hWnd, hClickedNode, &rNode );
				CalculateDockedCaptionRects( GetLeafNode( hClickedNode )->hWnd, rNode, &rCaption, NULL, NULL, NULL,(DockHost_GetNodeStyle( hClickedNode ) & DHNS_NO_CLOSE) == 0);
				if( PtInRect( &rCaption, ptCursor ) )
				{
					bDetectDrag = TRUE;
					break;
				}
			}
		}
		break;



	case WM_LBUTTONUP:
        if( GetDividerNode( hClickedNode ) )
            DockHost_Notify( hWnd, DHN_END_RESIZE, hClickedNode );
        bHandled = TRUE;
		if( GetCapture() == hWnd )
			ReleaseCapture();
		hClickedNode = NULL;
		bDetectDrag = FALSE;
		break;



	case WM_CAPTURECHANGED:
		bHandled = TRUE;
		if( (HWND)lParam == hWnd ) // Don't do anything if it's us who's gaining capture.
			break;
		// Dragging has ended.
		hClickedNode = NULL;
		bDetectDrag = FALSE;
		break;



	case WM_MOUSEMOVE:
		{
			bHandled = TRUE;

			// Ensure we receive a WM_MOUSELEAVE message.
			if( 0 == (wParam & MK_LBUTTON) )
			{
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof( TRACKMOUSEEVENT );
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hWnd;
				TrackMouseEvent( &tme );
			}

			POINT ptCursor = { LOWORD( lParam ), HIWORD( lParam ) };

			// Process divider dragging.
			if( GetDividerNode( hClickedNode ) )
			{
				POINT pt;
				GetCursorPos( &pt );
				ScreenToClient( hWnd, &pt );
				DividerDragging( pDH, GetDividerNode( hClickedNode ), pt );
				break;
			}

			// Process caption area tear-off.
			if( hClickedNode && bDetectDrag )
			{
				if( 0 == (wParam & MK_LBUTTON) )
				{
					OutputDebugString( TEXT(__FUNCTION__)TEXT(": Unexpected program state!" ) );
					DebugBreak();
				}
				if( abs( ptCursor.x - ptMouseStart.x ) >= GetSystemMetrics( SM_CXDRAG ) ||
					abs( ptCursor.y - ptMouseStart.y ) >= GetSystemMetrics( SM_CYDRAG ) )
				{
					bDetectDrag = FALSE;
					if( 0 == DockHost_Notify( hWnd, DHN_TEAROFF, hClickedNode ) )
					{
						// If the notified windows do not process the notification themselves,
						// undock the corresponding window, and allow the user to continue dragging it.
						HWND hClientWnd = DockHost_GetNodeWindow( hClickedNode );
						DockHost_Undock( hWnd, hClickedNode, NULL );
						SetCapture( NULL );
						if( hClientWnd )
						{
							RECT rWnd;
							GetWindowRect( hClientWnd, &rWnd );
							POINT ptSSCursor = GetCursorPos();
							OffsetRect( &rWnd, (ptSSCursor.x - rWnd.left) - RectWidth(rWnd)/2, ptSSCursor.y - rWnd.top );
							MoveWindow( hClientWnd, rWnd.left, rWnd.top, RectWidth(rWnd), RectHeight(rWnd), FALSE );
							SendMessage( hClientWnd, WM_SYSCOMMAND, SC_MOVE, 0 );
						}
					}
					hClickedNode = NULL;
					break;
				}
			}

		
			HDHNODE hNode = DockHost_HitTest( hWnd, ptCursor, NULL );


			// Do hot-tracking for the close button in the fake caption bar.
            if((DockHost_GetNodeStyle( hClickedNode ) & DHNS_NO_CLOSE) == 0)
            {
                BOOL bCloseHot = FALSE;
                if( GetLeafNode( hNode ) && GetLeafNode( hNode )->hWnd )
                {
                    RECT rClose, rNode;
                    DockHost_GetNodeRect( pDH->hWnd, hNode, &rNode );
                    CalculateDockedCaptionRects( GetLeafNode( hNode )->hWnd, rNode, NULL, NULL, NULL, &rClose,TRUE);
                    if( PtInRect( &rClose, ptCursor ) )
                    {
                        bCloseHot = TRUE;
                        SetHotCloseButton( hWnd, hNode, &rClose, TRUE );
                    }
                }
                if( !bCloseHot )
                    SetHotCloseButton( hWnd, NULL, NULL, TRUE );
            }
			// Set the cursor to a sizing cursor if we're over a divider.
			if( !hNode || CONV(hNode)->type == DH_LEAF )
				SetCursor( LoadCursor( NULL, IDC_ARROW ) );
			else if( CONV(hNode)->type == DH_DIVIDER_H )
				SetCursor( LoadCursor( NULL, IDC_SIZENS ) );
			else if( CONV(hNode)->type == DH_DIVIDER_V )
				SetCursor( LoadCursor( NULL, IDC_SIZEWE ) );
		}
		break;

	case WM_MOUSELEAVE:
		SetHotCloseButton( hWnd, NULL, NULL, TRUE );
		break;
	}

	if( bHandled )
		return DefWindowProc( hWnd, Msg, wParam, lParam );
	return 0;
}