
#pragma once

#include "BaseWidget.h"
#include "DockedWidget.h"
#include <list>

class DEF_EXPORT DockedPanelWidget : public BaseWidget
{	
    typedef std::list<class DockedWidget*> TChilds;

    void CalculateLocation(struct DHLeafNode* pNode,class DHDividerNode* pDivider,const RECT& rect);
    void ParseDockedHostTree( HWND hDockHost,struct DHNode* pNode,TDockedWindowDescriptors& vDocs,DH_NODE_TYPE eDividerType = DH_LEAF);
    void ParseDockedWindowsLocation(const string& sDockedHost,float& width,float& height,TDockedWindowDescriptors& vList);

    TChilds vChilds;
    TChilds vUndockChilds;
public:
	DockedPanelWidget(BaseWidget* parent, float x, float y, float w, float h,EAutoSizeType sizerType = BaseWidget::eAutoSizeNone);
    DockedPanelWidget(BaseWidget* parent, float x, float y,const string& sDockedHost,TDockedWidgets&  vDockedWidgets,EAutoSizeType sizerType = BaseWidget::eAutoSizeNone);
	virtual ~DockedPanelWidget();

	virtual void ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam);

    void DockedHostToString(string& sOut);
    void CreateDockedWidgets(TDockedWindowDescriptors& vList,TDockedWidgets&  vDockedWidgets);
    DockedWidget* AddDockedWidget(const DockedWidget::Descriptor* pDesc,TDockedWidgets&  vDockedWidgets,HDHNODE hBase = NULL);
    void OnDockedWindowDestroy(DockedWidget* pWidget);
   
};
