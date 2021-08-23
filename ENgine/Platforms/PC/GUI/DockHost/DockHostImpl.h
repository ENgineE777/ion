// Header file used by the implementation (cpp) files for the dock host control.

# ifndef DOCKHOST_IMPL_H
# define DOCKHOST_IMPL_H

# include "DockHost.h"

# include "..\CSTB\IntrusiveTree.h"
# include "..\CSTB\Win32Util.h"




struct DHNode
{
	DHNode( DH_NODE_TYPE _type ) : type(_type) { cachedSize.cx = cachedSize.cy = 0; }
	CSTB::TreeHook	hook;
	DH_NODE_TYPE	type;
	SIZE			cachedSize;
	virtual SIZE	GetMinSize() const = 0;
	virtual LONG	GetResizability() const = 0;

    virtual DH_NODE_TYPE	GetType() const {return type;}
};



typedef CSTB::IntrusiveWeakTreeT< DHNode, offsetof( DHNode, DHNode::hook ) > DHNodeTree;



struct DHLeafNode : public DHNode
{
	SIZE	minSize;
	LONG	resizability;
	HWND	hWnd;
	DWORD	dwUndockedStyle;
	DWORD	dwStyle;
	RECT	undockedRect;
	VOID*	userData;
    DH_DockLocation location;

	DHLeafNode() : DHNode( DH_LEAF )
	{
		minSize.cx = minSize.cy = 0;
		resizability = DH_DEFAULT_RESIZABILITY;
		hWnd = NULL;
		dwUndockedStyle = 0;
		dwStyle = 0;
		SetRect( &undockedRect, 0, 0, 0, 0 );
		userData = 0;
        location = DH_LEFT;
	}

	SIZE GetMinSize() const { return this->minSize; }
	LONG GetResizability() const { return this->resizability; }
};





class DHDividerNode : public DHNode
{
public:
	INT pos;

	DHDividerNode( DH_NODE_TYPE dividerType ) : DHNode( dividerType ) { pos = -1; }
	DHNode* ChildA() const { return DHNodeTree::FirstChild( this ); }
	DHNode* ChildB() const { return DHNodeTree::LastChild( this ); }

	SIZE GetMinSize() const
	{
		SIZE minSizeA = ChildA()->GetMinSize();
		SIZE minSizeB = ChildB()->GetMinSize();
		SIZE retSize;

		if( type == DH_DIVIDER_H )
		{
			retSize.cx = MAX( minSizeA.cx, minSizeB.cx );
			retSize.cy = minSizeA.cy + minSizeB.cy + DH_DIVIDER_HEIGHT;
		}
		else
		{
			retSize.cx = minSizeA.cx + minSizeB.cx + DH_DIVIDER_WIDTH;
			retSize.cy = MAX( minSizeA.cy, minSizeB.cy );
		}
		return retSize;
	}

	LONG GetResizability() const
	{
		INT ra = ChildA()->GetResizability();
		INT rb = ChildB()->GetResizability();
		return MAX( ra, rb );
	}
};




// Structure holding information about the dock host window. Each dock host window will
// have a pointer to an instance of this structure in the extra class bytes.
// TODO: The structure of the tree should be kept in check by hiding the nodes member
// and having clients modify it through methods in the class that ensure tree integrity.
struct TDockHost
{
private:
	DHNodeTree m_NodeTree;
public:
	HWND		hWnd;
	INT			numNodes;
	INT			numLeaves;
	INT			numDividers;
	DHNode*		pLastHitNode;

	TDockHost( HWND _hWnd )
	{
		hWnd = _hWnd;
		numNodes = 0;
		numLeaves = 0;
		numDividers = 0;
		pLastHitNode = NULL;
	}

	DHNodeTree& GetNodeTree() { return m_NodeTree; }
};



inline TDockHost*	GetControlData( HWND hWnd )					{ return (TDockHost*)GetWindowLongPtr( hWnd, 0 ); }
inline VOID			SetControlData( HWND hWnd, TDockHost* pDH )	{ SetWindowLongPtr( hWnd, 0, (LONG)pDH ); }



inline TDockHost*	CONV( HWND hWnd )			{ return GetControlData(hWnd); }
inline DHNode*		CONV( HDHNODE hNode )		{ return (DHNode*)hNode; }
inline HDHNODE		CONV( DHNode* pNode )		{ return (HDHNODE)pNode; }


inline DHLeafNode* GetLeafNode( DHNode* pNode )
{ return pNode && pNode->type == DH_LEAF ? (DHLeafNode*)pNode : NULL; }


inline DHDividerNode* GetDividerNode( DHNode* pNode )
{ return pNode && pNode->type == DH_LEAF ? NULL : (DHDividerNode*)pNode; }


inline DHLeafNode*		GetLeafNode( HDHNODE hNode )	{ return GetLeafNode( CONV(hNode) ); }
inline DHDividerNode*	GetDividerNode( HDHNODE hNode ) { return GetDividerNode( CONV(hNode) ); }



LRESULT DockHostMouseMsgProc( TDockHost* pDH, UINT Msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

VOID DockHost_OffsetRect( TDockHost* pDH, DHNode* pNode, INT dx, INT dy );
BOOL DockHost_Resize( TDockHost* pDH, DHNode* pNode, RECT& newRect, BOOL forceIt );

VOID CalculateDockedCaptionRects( HWND hWnd, const RECT& rClient,RECT* prCaption, RECT* prText, RECT* prIcon, RECT* prClose ,BOOL  bCloseButton);

BOOL DrawDockedCaption( HWND hWnd, HDC hdc, const RECT& rClient, BOOL bActive, BOOL bCloseHot,BOOL  bCloseButton);


LRESULT DockHost_Notify( HWND hDockHost, UINT code, HDHNODE hNode );



struct CloseButton
{
	HDHNODE hNode;
	HWND hWnd;
	RECT rClose;
	CloseButton() { hNode = NULL; hWnd = NULL; }
};
const CloseButton&	DockHost_GetHotCloseButton();



# endif // inclusion guard.