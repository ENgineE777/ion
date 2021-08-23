
#include "DockedPanelWidget.h"
#include "CustomWindow.h"
#include "Platforms/PC/GUI/DockHost/DockHostImpl.h"
extern HINSTANCE gEditorInstance;

DockedPanelWidget::DockedPanelWidget(BaseWidget* prnt, float set_x, float set_y, float w, float h,EAutoSizeType sizerType) : BaseWidget(EDITOR_DOCK_PANEL_ID, prnt, "",sizerType)
{
	x = set_x;
	y = set_y;
	width = w;
	height = h;	
        
	CustomWindow::Initialize( gEditorInstance );

	DockHost_Init( gEditorInstance );

	handle = CreateWindowEx( 0, WC_DOCKHOST, TEXT(""), WS_CHILD,(int)x,(int)y,(int)w,(int)h,parent->GetHandle(), NULL, gEditorInstance, NULL );

    MakeSubClassing();

	ShowWindow( handle, SW_SHOW );

}

DockedPanelWidget::DockedPanelWidget(BaseWidget* prnt, float set_x, float set_y,const string& sDockedHost,TDockedWidgets& vDockedWidgets,EAutoSizeType sizerType): BaseWidget(EDITOR_DOCK_PANEL_ID, prnt, "",sizerType)
{
    x = set_x;
    y = set_y;
    TDockedWindowDescriptors vList;
    ParseDockedWindowsLocation(sDockedHost,width,height,vList);

    CustomWindow::Initialize( gEditorInstance );

    DockHost_Init( gEditorInstance );

    handle = CreateWindowEx( 0, WC_DOCKHOST, TEXT(""), WS_CHILD,(int)x,(int)y,(int)width,(int)height,parent->GetHandle(), NULL, gEditorInstance, NULL );

    MakeSubClassing();

    ShowWindow( handle, SW_SHOW );

    CreateDockedWidgets(vList,vDockedWidgets);
}

DockedPanelWidget::~DockedPanelWidget()
{
}

void DockedPanelWidget::ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam)
{	
    if(msg == WM_NOTIFY)
    {
        switch (((LPNMHDR)lParam)->code)
        {
        case DHN_ITEMUNDOCK:
            {
                HWND hWnd = ((NM_DOCKHOST*)lParam)->itemWnd;
                for(TChilds::iterator it = vChilds.begin();it != vChilds.end();++it)
                {
                    if((*it)->GetHandle() == hWnd)
                    {
                        vUndockChilds.push_back((*it));
                        vChilds.erase(it);
                        break;
                    }
                }
                break;
            }
        case DHN_ITEMDOCK:
            {
                HWND hWnd = ((NM_DOCKHOST*)lParam)->itemWnd;
                for(TChilds::iterator it = vUndockChilds.begin();it != vUndockChilds.end();++it)
                {
                    if((*it)->GetHandle() == hWnd)
                    {
                        vChilds.push_back((*it));
                        vUndockChilds.erase(it);
                        break;
                    }
                }
                break;
            }
        default:
            {
                for(TChilds::iterator it = vChilds.begin();it != vChilds.end();++it)
                    (*it)->ProcessChildWidget(msg,wParam,lParam);
            }
        }
    }
    
}

void DockedPanelWidget::CalculateLocation(struct DHLeafNode* pNode,struct DHDividerNode* pDivider,const RECT& rect)
{
	if(pNode->location > DH_BOTTOM)
	{
		if(pDivider->GetType() == DH_DIVIDER_H)
		{
			if(rect.top > pDivider->pos)
				pNode->location = DH_BOTTOM;
			else
				pNode->location = DH_TOP;
		}
		else
			if(pDivider->GetType() == DH_DIVIDER_V)
			{
				if(rect.left > pDivider->pos)
					pNode->location = DH_RIGHT;
				else
					pNode->location = DH_LEFT;
			}
	}
}

void DockedPanelWidget::ParseDockedHostTree(HWND hDockHost,struct DHNode* pNode,TDockedWindowDescriptors& vDocs,DH_NODE_TYPE eDividerType)
{
    if( !pNode )
        return;
    RECT rNode;
    DockHost_GetNodeRect( hDockHost, CONV(pNode), &rNode );
    DHDividerNode* pDivider = GetDividerNode( pNode );
    if( pDivider )
    {
        DHNode* pChildA =  pDivider->ChildA();
        DHNode* pChildB =  pDivider->ChildB();
		if(pChildA->GetType() != DH_LEAF && pChildB->GetType() != DH_LEAF)
		{
			ParseDockedHostTree(hDockHost,pChildA,vDocs);
			ParseDockedHostTree(hDockHost,pChildB,vDocs);
		}
		else
		{
			if(pChildA->GetType() != DH_LEAF && pChildB->GetType() == DH_LEAF)
			{
				ParseDockedHostTree(hDockHost,pChildA,vDocs);
				RECT rSize;
				DockHost_GetNodeRect( hDockHost, CONV(pChildB), &rSize );
				CalculateLocation(static_cast<DHLeafNode*>(pChildB),pDivider,rSize);
				vDocs.push_back(new  DockedWidget::Descriptor(pChildB,RectWidth(rSize),RectHeight(rSize),static_cast<DHLeafNode*>(pChildB)));
			}
			else
			{
				if(pChildB->GetType() != DH_LEAF && pChildA->GetType() == DH_LEAF)
				{
					ParseDockedHostTree(hDockHost,pChildB,vDocs);
					RECT rSize;
					DockHost_GetNodeRect( hDockHost, CONV(pChildA), &rSize );
					CalculateLocation(static_cast<DHLeafNode*>(pChildA),pDivider,rSize);
					vDocs.push_back(new  DockedWidget::Descriptor(pChildA,RectWidth(rSize),RectHeight(rSize),static_cast<DHLeafNode*>(pChildA)));
				}
				else
				{
					if(pChildB->GetType() == DH_LEAF && pChildA->GetType() == DH_LEAF)
					{
						RECT rSize;
						if(pChildB->GetResizability() >=  pChildA->GetResizability())
						{
							DockHost_GetNodeRect( hDockHost, CONV(pChildA), &rSize );
							DockedWidget::Descriptor* pDocElement = new  DockedWidget::Descriptor(pChildA,RectWidth(rSize),RectHeight(rSize),static_cast<DHLeafNode*>(pChildA));
							CalculateLocation(static_cast<DHLeafNode*>(pChildB),pDivider,rSize);
							DockHost_GetNodeRect( hDockHost, CONV(pChildB), &rSize );
							vDocs.push_back(new  DockedWidget::Descriptor(pChildB,RectWidth(rSize),RectHeight(rSize),static_cast<DHLeafNode*>(pChildB),pDocElement));
						}
						else
						{
							DockHost_GetNodeRect( hDockHost, CONV(pChildB), &rSize );
							DockedWidget::Descriptor* pDocElement = new  DockedWidget::Descriptor(pChildB,RectWidth(rSize),RectHeight(rSize),static_cast<DHLeafNode*>(pChildB));
							CalculateLocation(static_cast<DHLeafNode*>(pChildA),pDivider,rSize);
							DockHost_GetNodeRect( hDockHost, CONV(pChildA), &rSize );
							vDocs.push_back(new  DockedWidget::Descriptor(pChildA,RectWidth(rSize),RectHeight(rSize),static_cast<DHLeafNode*>(pChildA),pDocElement));
						}
					}
				}
			}
		}
    }
    else
        if(pNode->GetType() == DH_LEAF)
        {
            RECT rSize;
            DockHost_GetNodeRect( hDockHost, CONV(pNode), &rSize );
            DHLeafNode* pLeaf =  static_cast<DHLeafNode*>(pNode);
            pLeaf->location = DH_CENTER;
            vDocs.push_back(new  DockedWidget::Descriptor(pNode,RectWidth(rSize),RectHeight(rSize),pLeaf));
        }

}

void DockedPanelWidget::DockedHostToString(string& sOut)
{
    TDockedWindowDescriptors vList;
    ParseDockedHostTree( handle, CONV( DockHost_GetRootNode( handle ) ),vList);
    RECT rSize;
    GetWindowRect(handle,&rSize);
    char sBuffer[64];
    String::Printf(sBuffer,64,"{%d,%d}",RectWidth(rSize),RectHeight(rSize));
    sOut = sBuffer;
    for(unsigned i = 0;i < vList.size();++i)
    {
        vList[i]->ToString(sOut);
        if(i + 1 < vList.size())
            sOut += ',';
        delete vList[i];
    }
    vList.clear();
}

void DockedPanelWidget::CreateDockedWidgets(TDockedWindowDescriptors& vList,TDockedWidgets&  vDockedWidgets)
{
    for(unsigned i = 0;i < vList.size();++i)
    {
        const DockedWidget::Descriptor* pDesc = vList[i];
        AddDockedWidget(pDesc,vDockedWidgets);
        delete vList[i];
    }
    vList.clear();
}

void DockedPanelWidget::ParseDockedWindowsLocation(const string& sDockedHost,float& width,float& height,TDockedWindowDescriptors& vList)
{
    int nState = 0;string sBuffer;
    for(unsigned i = 0;i < sDockedHost.size(); ++i)
    {
        switch(nState)
        {
        case 0:
            {
                if(sDockedHost[i] == '{')break;
                if(sDockedHost[i] == '}')
                {
                    height = atoi(sBuffer.c_str());
                    sBuffer.clear();
                    nState = 1;
                    break;
                }
                if(sDockedHost[i] == ',')
                {
                    width = atoi(sBuffer.c_str());
                    sBuffer.clear();
                    break;
                }
                else sBuffer += sDockedHost[i];
                break;
            }
        case 1:
            {
                if(sDockedHost[i] == '{')
                {
                    int nCount = 1;
                    for(unsigned j = i + 1;i < sDockedHost.size(); ++j)
                    {
                        if(sDockedHost[j] == '{')++nCount;
                        else
                            if(sDockedHost[j] == '}')--nCount;
                        if(nCount == 0)
                        {
                            string sElement;
                            sElement.assign(&sDockedHost[i + 1],j - i - 1);
                            vList.push_back(new DockedWidget::Descriptor(sElement));
                            i = j;
                            break;
                        }
                    }
                }
                break;
            }
        }

    }
}

DockedWidget* DockedPanelWidget::AddDockedWidget(const DockedWidget::Descriptor* pDesc,TDockedWidgets&  vDockedWidgets,HDHNODE hBase)
{
    DockedWidget* pChild  = NULL;
    DockedWidget* pDock = new DockedWidget(this,pDesc,hBase);
    vDockedWidgets[pDesc->sName] = pDock;
    if(pDesc->pChild) 
    {
        pChild = new DockedWidget(this,pDesc->pChild,pDock->GetDockHandle());
        vDockedWidgets[pDesc->pChild->sName] = pChild;
    }
    vChilds.push_back(pDock);
    return pDock;
}

void DockedPanelWidget::OnDockedWindowDestroy(DockedWidget* pWidget)
{
    for(TChilds::iterator it = vUndockChilds.begin();it != vUndockChilds.end();++it)
    {
        if((*it) == pWidget)
        {
            vUndockChilds.erase(it);
            break;
        }
    }
    for(TChilds::iterator it = vChilds.begin();it != vChilds.end();++it)
    {
        if((*it) == pWidget)
        {
            vChilds.erase(it);
            break;
        }
    }
}
