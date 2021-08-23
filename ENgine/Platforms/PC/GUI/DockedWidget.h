
#pragma once

#include "BaseWidget.h"
#include "DockHost\DockHost.h"

class DEF_EXPORT DockedWidget : public BaseWidget
{
    HDHNODE  hDockHandle;
public:
    struct Descriptor 
    {
        int     nResizability;
        DWORD   dwNodeStyle;
        int     nMinWidth;
        int     nMinHeight;
        int     nWidth;
        int     nHeigth;
        string  sName;
        Descriptor* pChild;
        int          nDockLocation;

        Descriptor(const string& sText);
        Descriptor(int width,int height,DWORD style,int location,int resizability):pChild(null),
            nWidth(width),nHeigth(height),nMinWidth(-1),nMinHeight(-1),dwNodeStyle(style),nResizability(resizability),nDockLocation(location)
        {}
        Descriptor(const char* _sName,int width,int height,int minWidth,int minHeight,DWORD style,int location,int resizability,Descriptor* _pChild = NULL):sName(_sName),pChild(_pChild),
            nWidth(width),nHeigth(height),nMinWidth(minWidth),nMinHeight(minHeight),dwNodeStyle(style),nResizability(resizability),nDockLocation(location)
        {}

        Descriptor(struct DHNode* pDock,int width,int height,struct DHLeafNode* pNode);
        Descriptor(struct DHNode* pDock,int width,int height,struct DHLeafNode* pNode,Descriptor* _pChild);
        ~Descriptor(){if(pChild)delete pChild;}
        void ToString(string& sText);
    private:
        void GetName(DHNode* pDock);

    };

    struct DockedWidgetHolder
    {
        RECT          rSize;
        DockedWidget* pWidget;
        DockedWidgetHolder();
        DockedWidgetHolder(const DockedWidgetHolder& other);
        DockedWidgetHolder& operator = (const DockedWidgetHolder& other);
        DockedWidgetHolder& operator = (DockedWidget* wgt);
        void OnWidgetDestroy();

    };

	DockedWidget(BaseWidget* parent,const Descriptor* pDesc,HDHNODE hBase = NULL);
	virtual ~DockedWidget();

	virtual void ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam);

    HDHNODE GetDockHandle() const {return hDockHandle;}
};

typedef std::vector<struct DockedWidget::Descriptor*>       TDockedWindowDescriptors;
typedef HashTable<string,DockedWidget::DockedWidgetHolder>  TDockedWidgets;

