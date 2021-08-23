
#include "DockedWidget.h"
#include <richedit.h>
#include <sys/stat.h>


#include "DockHost\DockDiamond.h"
#include "DockHost\DockHostImpl.h"
#include "CustomWindow.h"
#include "Core\ICore.h"
#include "Editor\Editor.h"
#include "DockedPanelWidget.h"

extern HINSTANCE gEditorInstance;

extern BOOL	MakeDockable( HWND hWnd, HWND hDockHost,DWORD dwNodeStyle,INT minWidth,INT minHeight,INT resizability);

HWND CreateCustomWindow( LPCTSTR sName,const DockedWidget::Descriptor* pDesc ,HWND hWnd,HWND hDockHost)
{
	HWND hHandel = CreateWindowEx( NULL, CustomWindow::WC_CUSTOMWINDOW,sName, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_VISIBLE | WS_THICKFRAME,	CW_USEDEFAULT, CW_USEDEFAULT,pDesc->nWidth,pDesc->nHeigth,hWnd, NULL, gEditorInstance, NULL );
	if( !hHandel )
		return NULL;
	MakeDockable( hHandel, hDockHost,pDesc->dwNodeStyle,pDesc->nMinWidth,pDesc->nMinHeight,pDesc->nResizability );
	return hHandel;
}

DockedWidget::DockedWidget(BaseWidget* parent, const Descriptor* pDesc,HDHNODE hBase) : BaseWidget(EDITOR_DOCK_WINDOW_ID, NULL,pDesc->sName.c_str(), BaseWidget::eAutoSizeNone)
{
    handle = CreateCustomWindow( pDesc->sName.c_str(),pDesc,parent->GetParent()->GetHandle(),parent->GetHandle());
    CustomWindow::SetColor( handle, GetSysColor(COLOR_BTNFACE) );
    hDockHandle = DockHost_Dock(parent->GetHandle(), handle,static_cast<DH_DockLocation>(pDesc->nDockLocation),hBase,pDesc->dwNodeStyle,pDesc->nWidth,pDesc->nHeigth,pDesc->nMinWidth,pDesc->nMinHeight,pDesc->nResizability);
    RECT rSize;
    GetWindowRect(handle,&rSize);
    width  = RectWidth(rSize);
    height = RectHeight(rSize);    
    MakeSubClassing();
}

DockedWidget::~DockedWidget()
{
}

void DockedWidget::ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam)
{		
    switch(msg)
    {
        case WM_SIZE:
            {
                width = LOWORD( lParam );
                height = HIWORD( lParam );
                for (int i = 0; i < childs.size(); i++)
                    childs[i]->Resize();	
                TCHAR s[ MAX_PATH ];
                s[0] = 0;
                GetWindowText( handle, s, MAX_PATH );
                break;
            }
        case WM_DESTROY:
            {
                TCHAR s[ MAX_PATH ];
                s[0] = 0;
                GetWindowText( handle, s, MAX_PATH );
                core_ed.OnDockedWindowDestroy(s);
                break;
            }
    }

}

DockedWidget::Descriptor::Descriptor(DHNode* pDock,int width,int height,struct DHLeafNode* pNode,Descriptor* _pChild) :nWidth(width),nHeigth(height),pChild(_pChild),
    nResizability(pNode->resizability),dwNodeStyle(pNode->dwStyle),nMinWidth(pNode->minSize.cx),nMinHeight(pNode->minSize.cy),nDockLocation(pNode->location)
{
    GetName(pDock);
}

DockedWidget::Descriptor::Descriptor(DHNode* pDock,int width,int height,struct DHLeafNode* pNode):nWidth(width),nHeigth(height),pChild(NULL),
    nResizability(pNode->resizability),dwNodeStyle(pNode->dwStyle),nMinWidth(pNode->minSize.cx),nMinHeight(pNode->minSize.cy),nDockLocation(pNode->location)
{
    GetName(pDock);
}

DockedWidget::Descriptor::Descriptor(const string& sText):pChild(NULL)
{
    int nState = 0;string sBuffer;
    for(unsigned i =0;i < sText.size();++i)
    {
        if(sText[i] == ',')
        {
            switch(nState)
            {
                case 0:sName = sBuffer;break;
                case 1:nWidth = atoi(sBuffer.c_str());break;
                case 2:nHeigth = atoi(sBuffer.c_str());break;
                case 3:nMinWidth = atoi(sBuffer.c_str());break;
                case 4:nMinHeight = atoi(sBuffer.c_str());break;
                case 5:dwNodeStyle = atoi(sBuffer.c_str());break;
                case 6:nResizability = atoi(sBuffer.c_str());break;
                case 7:nDockLocation = atoi(sBuffer.c_str());break;
            }
            sBuffer.clear();
            ++nState;
        }
        else
            if(sText[i] == '{')
            {
                unsigned j =  i + 1;
                for(;j < sText.size();++j)
                    if(sText[j] == '}')break;
                if(j < sText.size())
                {
                    string sChild;
                    sChild.assign(&sText[i + 1],j - i - 1);
                    pChild = new Descriptor(sChild);
                }
                return;
            }
            else
                if(sText[i] != '}')sBuffer += sText[i];
    }
    if(!sBuffer.empty())
        nDockLocation = atoi(sBuffer.c_str());
}

void DockedWidget::Descriptor::ToString(string& sText)
{
    static char sBuffer[2048];
    memset(sBuffer,0,2048);
    String::Printf(sBuffer,2048,"{%s,%d,%d,%d,%d,%d,%d,%d",sName.c_str(),nWidth,nHeigth,nMinWidth,nMinHeight,dwNodeStyle,nResizability,nDockLocation);
    sText += sBuffer;
    if(pChild)
    {
        sText += ',';
        pChild->ToString(sText);
    }
    sText += '}';
}

void DockedWidget::Descriptor::GetName(DHNode* pDock)
{
    HWND hWnd = DockHost_GetNodeWindow( CONV(pDock) );
    if( hWnd )
    {
        TCHAR s[ MAX_PATH ];
        s[0] = 0;
        GetWindowText( hWnd, s, MAX_PATH );
        sName = s;
    }
}

DockedWidget::DockedWidgetHolder::DockedWidgetHolder() :pWidget(NULL)
{
    ResetRect(&rSize);
}

DockedWidget::DockedWidgetHolder::DockedWidgetHolder(const DockedWidgetHolder& other)
{
   rSize = other.rSize;
   pWidget = other.pWidget;
}

void DockedWidget::DockedWidgetHolder::OnWidgetDestroy()
{
    ResetRect(&rSize);
    if(pWidget)
        GetWindowRect(pWidget->handle,&rSize);
    pWidget = NULL;
}

DockedWidget::DockedWidgetHolder& DockedWidget::DockedWidgetHolder::operator=(const DockedWidgetHolder& other)
{
    rSize = other.rSize;
    pWidget = other.pWidget;
    return *this;
}

DockedWidget::DockedWidgetHolder& DockedWidget::DockedWidgetHolder::operator=(DockedWidget* wgt)
{
    pWidget = wgt;
    if(pWidget)
        GetWindowRect(pWidget->handle,&rSize);
    return *this;
}
