/**
The Win32 Dock Host control API header file. The dock host is a custom control which hosts other
windows. The client docks other windows in a dock host, and the dock host will display splitters
and tab controls.

-Adel Amro
http://code-section.com
*/


# ifndef DOCKHOST_H
# define DOCKHOST_H



# include <windows.h>


#ifndef WMN_FIRST
#define WMN_FIRST               (0U-1000U)
#define WMN_LAST                (0U-1200U)
#endif


/// @name Dockhost API
/// @{

/// A handle to a node in a dockhost control. The control uses a tree structure to arrange its contents.
DECLARE_HANDLE( HDHNODE );

/// The dock host window class name, registered by DOCKHOST_Init(). Pass this to CreateWindowEx()
/// to create a dock host control.
# define WC_DOCKHOST		TEXT("code-section-dock-host")

# define DH_DIVIDER_WIDTH		6 ///< The behavioral width of a vertical divider.
# define DH_DIVIDER_HEIGHT		5 ///< The behavioral height of a horizontal divider.
# define DH_DEFAULT_MIN_WIDTH	100
# define DH_DEFAULT_MIN_HEIGHT	75
# define DH_DEFAULT_RESIZABILITY 2

/// @name Dock node styles.
/// @{

# define DHNS_NO_TEAROFF		0x0001 ///< The docked window can not be undocked (torn off) by the user.
# define DHNS_NO_CLOSE		0x0002 ///< The dock node will not have a caption bar.
/// @}

/// Registers the dock host window class.
BOOL DockHost_Init( HINSTANCE );

/// Unregisters the dock host window class.
VOID DockHost_Uninit( HINSTANCE );

/// Returns whether the specified window is a dock host or not.
BOOL DockHost_IsDockHost( HWND hWnd );


/// Enum used to specify where to dock a particular item in the control. DH_CLEFT, DH_CTOP,
/// DH_CRIGHT, and DH_CBOTTOM are used to dock an item relative to another item (they correspond
/// to the dock diamond's central part).
enum DH_DockLocation
{
	//DH_NOWHERE = 0,
	DH_LEFT = 0,
	DH_TOP,
	DH_RIGHT,
	DH_BOTTOM,
	DH_CLEFT,
	DH_CTOP,
	DH_CRIGHT,
	DH_CBOTTOM,
	DH_CENTER
};


/// The dock host contents are arranged as a tree. This enum lists the types of nodes in the tree.
enum DH_NODE_TYPE
{
	DH_LEAF = 0, ///< Leaf node (has a window in it).
	DH_DIVIDER_H, ///< Horizontal divider.
	DH_DIVIDER_V ///< Vertical divider.
};


/// @{ Notifications

# define DHN_FIRST			(WMN_LAST - 7)
# define DHN_ITEMDOCK		(DHN_FIRST + 1) ///< An item has docked. @see NM_DOCKHOST.
# define DHN_ITEMUNDOCK		(DHN_FIRST + 2) ///< An item has been undocked. @see NM_DOCKHOST.
/// Sent when the close button for a leaf node has been pressed.
/// Return nonzero to prevnt further processing, otherwise the dock host will undock the window
/// and then send it a WM_CLOSE message.
/// This notification message is sent to both the parent of the dockhost control and the window being closed.
# define DHN_CLOSE			(DHN_FIRST + 3) 
/// Sent when the user has clicked the caption bar of a docked window and dragged it to undock
/// the corresponding window. Return 0 to prevent further processing, otherwise the dock host
/// will undock the window and send it a SC_MOVE system command to allow the user to continue dragging it.
/// This notification message is sent to both the parent of the dockhost and the window being torn off.
# define DHN_TEAROFF		(DHN_FIRST + 4)

# define DHN_START_RESIZE   (DHN_FIRST + 5)
# define DHN_END_RESIZE 	(DHN_FIRST + 6)

/// @}

/// Notification message structure passed in the lParam parameter of WM_NOTIFY to the parent
/// of the dock host control and/or the relevant docked window.
struct NM_DOCKHOST
{
	NMHDR	hdr;
	HDHNODE	hNode; ///< Handle to the dock host node of the notification, if applicable. Can be NULL.
	HWND	itemWnd; ///< The window docked at the dock host node, or NULL.
	VOID*	nodeUserData;
};


/// Docks a window in the dock host control.
/// @param hDockHost		Handle to the dock host control window.
/// @param hWnd				The window that will be docked.
/// @param dockLocation		Specifies where to dock the window in the dock host control.
/// @param hBaseNode		Used with relative dock locations (DH_CLEFT, DH_CTOP... etc), this specifies
///							which node the location is relative to. It has to be a leaf node.
/// @param dwNodeStyle		The new node style. See Dock Node Styles and the DHNS_* constants.
/// @param requestedWidth	The requested width of the docked window. Can be set to -1 for default value.
/// @param requestedHeight	The requested height of the docked window. Can be set to -1 for default value.
/// @param minWidth			The minimum width that the user will be able to resize the node to. Default is -1.
/// @param minHeight		The minimum height that hte user will be able to resize the node to. Default is -1.
/// @param resizability		The resizability of the node. Nodes with higher resizability are resized first when
///							a resizing operation is performed on the nodes. Nodes with lower resizability are only
///							resized only when nodes with higher resizability can not be resized instead.
/// @param userData			User-defined data associated with the node.
/// @return					The function returns a handle to the new dock host leaf node that is created on success.
///							It returns NULL on failure.
HDHNODE DockHost_Dock( HWND hDockHost,HWND hWnd,DH_DockLocation dockLocation,HDHNODE hBaseNode = NULL,DWORD dwNodeStyle = 0,INT requestedWidth = -1, INT requestedHeight = -1,INT minWidth = -1, INT minHeight = -1,INT resizability = DH_DEFAULT_RESIZABILITY,	VOID* userData = NULL );

VOID    DockHost_RemoveNode( HWND hDockHost,struct DHNode* pNode );

/// Undocks an item. The passed node handle will become invalid after this call.
BOOL	DockHost_Undock( HWND hDockHost, HDHNODE hNode, HWND hNewOwner = NULL );


/// Searches for and undocks the specified window.
BOOL	DockHost_UndockWindow( HWND hDockHost, HWND hWnd, HWND hNewOwner = NULL );

/// Transfers nodes (and associated windows) from one dock host window to another.
/// @param hSrcDockHost		The source dock host window - the dock host window containing the nodes that are going to be moved.
/// @param hSrcNode			The node that will be moved to the new window. All children are also moved.
/// @param hDestDockHost	The destination dock host window.
/// @param dockLocation		The location where the node will be inserted in the destination dock host window.
/// @param hBaseNode		The node in the destination dock host window, relative to which the node will be inserted.
///							This can be NULL.
BOOL	DockHost_TransferNodes( HWND hSrcDockHost, HDHNODE hSrcNode,HWND hDestDockHost, DH_DockLocation dockLocation, HDHNODE hBaseNode );


/// Sets the position of a divider node. The position is relative.
/// This will fail if the new position violates the minimum size restrictions of child nodes
/// unless bForce is TRUE. It will also fail if the specified node is not a divider node.
/// The function will also fail if the specified position is outside of the node's rectangle space.
BOOL	DockHost_SetDividerPosition( HWND hDockHost, HDHNODE hDivider, INT pos,
									BOOL bForce = FALSE, BOOL bUpdate = TRUE );


/// Returns the divider's position. See DockHost_SetDividerPosition() for more information on divider positions.
INT		DockHost_GetDividerPosition( HDHNODE hDivider );


/// Returns the range of valid values for divider positions that will not violate minimum size requirements
/// of child nodes.
/// Returns FALSE if the divider node is too small to contain both children without breaking minimum size
/// requirements. If that's the case, max will be smaller or equal to min. Returns TRUE otherwise.
BOOL	DockHost_GetDividerPositionRange( HWND hDockHost, HDHNODE hDivider, INT* min, INT* max );



/// @{ Node Info
/// Structure that holds information about a node, used with DockHost_GetNodeInfo().
/// If the node is a divider node (nodeType != DH_LEAF), then the rect member is the
/// rectangle that encompasses the divider and both its children, while the dividerRect
/// will contain the rect of just the divider.
struct DHNodeInfo
{
	DH_NODE_TYPE	nodeType; ///< The type of the node. See the DH_NODE_TYPE enumeration type.
	RECT			rect; ///< The node's rectangle. If the node is a divider, it encapsulates nodes on both sides.
	HDHNODE			parent; ///< The parent node. Can be NULL.
	HDHNODE			childA; ///< For divider nodes only. This is the node to the left or above the divider.
	HDHNODE			childB; ///< For divider nodes only. This is the node to the right or below the divider.
	RECT			dividerRect; ///< For divider nodes only. The divider's actual rectangle.
	LONG			resizability;	///< Node resizing priority. Higher values mean more likely to be resized.
									///< A divider's resizability is equal to the smallest resizability in its descentents.
	HWND			hWnd; ///< For leaf nodes only. The window docked in the node.
	HWND			hTab; ///< For leaf nodes only that have more than one window. Otherwise it's NULL.
	DWORD			dwStyle; ///< For leaf nodes only. The style of the node. See the DHNS_* constants.
	VOID*			userData; ///< For leaf nodes only. The user-defined value supplied to DochHost_Dock().
};


/// Retrieves info about a node. See the DHNodeInfo structure.
VOID			DockHost_GetNodeInfo( HWND hDockHost, HDHNODE, DHNodeInfo* pNI );

/// Returns the type of the specified node.
DH_NODE_TYPE	DockHost_GetNodeType( HDHNODE hNode );

HDHNODE	DockHost_GetRootNode( HWND hDockHost ); ///< Returns the root node, or NULL if the control is empty.
HDHNODE	DockHost_GetNextNode( HWND hDockHost, HDHNODE hNode ); ///< Returns the next node in depth-first traversal.
HDHNODE DockHost_GetNodeParent( HDHNODE node ); ///< Retursn the parent of a node.
HDHNODE DockHost_GetNodeChild1( HDHNODE node ); ///< For dividers only. Returns the node above or to the left of the divider.
HDHNODE DockHost_GetNodeChild2( HDHNODE node ); ///< For dividers only. Returns the node below or to the right of the divider.
HDHNODE DockHost_GetNodeSibling( HDHNODE node ); ///< Returns the sibling (other child of its parent) if it has one.

inline BOOL DockHost_IsEmpty( HWND hDockHost )
{ return NULL == DockHost_GetRootNode( hDockHost ); }

/// @name Multi-window Leaf Nodes
/// @{
UINT DockHost_GetNumWindows( HDHNODE hNode );
HWND DockHost_GetWindow( HDHNODE hNode, UINT index );
VOID DockHost_RemoveWindow( HWND hDockHost, HDHNODE hNode, UINT windowIndex );
/// @}

/// Searches for and returns the node containing the specified window.
HDHNODE	DockHost_FindNodeByHWND( HWND hDockHost, HWND hWnd );

/// Returns the window associated with the specified leaf node. If it doesn't have a window, NULL is returned.
/// If it has more than one window (uses a tab control), the active window is returned.
HWND DockHost_GetNodeWindow( HDHNODE hNode );

/// If the specified node is a leaf node which has more than one window associated with it, this function
/// returns a handle to the tab control that the node uses. Otherwise it returns NULL.
HWND DockHost_GetTabControl( HDHNODE hNode );

/// Gets a node's rect in client coordinates. If the node is a divider, the function returns
/// the rectangle encapsulating the contents on both sides of the divider.
VOID	DockHost_GetNodeRect( HWND hDockHost, HDHNODE hNode, RECT* prNode );

/// Returns the rectangle of a divider node. This function returns the rectangle of the actual
/// divider node and not the rectangle encapsulating its children like DockHost_GetNodeRect().
/// See DHNodeInfo::dividerRect and DHNodeInfo::rect.
VOID	DockHost_GetDividerRect( HWND hDockHost, HDHNODE hDividerNode, RECT* prDivider );

/// Returns all component rectangles of a particular leaf node.
BOOL	DockHost_GetLeafNodeRects( HWND hDockHost, HDHNODE hNode,
								RECT* prNode,
								RECT* prClient,
								RECT* prTab,
								RECT* prCaptionBar,
								RECT* prCloseButton,
								RECT* prText,
								RECT* prIcon );

/// Returns the style of a leaf node. See the DHNS_** constants.
DWORD	DockHost_GetNodeStyle( HDHNODE hNode );

/// Modifies the resizability of a node. If bRelative is true, then newResizability will be added
/// to the current resizability.
VOID	DockHost_SetNodeResizability( HDHNODE hNode, INT newResizability, BOOL bRelative );

/// @} Node info


/// Dock host leaf node component enumeration for hit-testing.
enum DHHT_Component
{
	DHHT_NOTHING,
	DHHT_CLIENT,
	DHHT_CAPTION_BAR,
	DHHT_TAB
};

/// Caption bar component enumeration for hit-testing.
enum DHHT_CBComponent
{
	DHHT_CB_NOTHING,
	DHHT_CB_BAR,
	DHHT_CB_CLOSE,
	DHHT_CB_ICON,
};

struct DH_HitTestInfo
{
	HDHNODE				hNode;
	DHHT_Component		component;
	DHHT_CBComponent	captionBarComponent; ///< Used when component == DHHT_CAPTION_BAR
	INT					iTab; ///< Used when component == DHHT_TAB
};

///// Hit testing. Assumes pt is in client coordinates.
//HDHNODE	DockHost_HitTest( HWND hDockHost, POINT pt );

/// Hit testing. Assumes pt is in client coordinates. pHTI can be NULL.
HDHNODE DockHost_HitTest( HWND hDockHost, POINT pt, DH_HitTestInfo* pHTI );

/// Returns the leaf node from the specified screen-space point. Can return NULL.
inline HDHNODE DockHost_LeafNodeFromPoint( HWND hDockHost, POINT ptScreen )
{
	ScreenToClient( hDockHost, &ptScreen );
	HDHNODE node = DockHost_HitTest( hDockHost, ptScreen, NULL );
	return (node && DockHost_GetNodeType( node ) == DH_LEAF) ? node : NULL;
}

inline BOOL DockHost_IsDockLocationRelative( DH_DockLocation dockLocation )
{ return (INT)dockLocation >= (INT)DH_CLEFT; }


/// @} // end dockhost group



# endif // include guard