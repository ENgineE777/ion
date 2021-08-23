
#pragma once

#include "BaseWidget.h"

class DEF_EXPORT TabSheetWidget : public BaseWidget
{
	struct Tab
	{
		char name[64];
		vector<BaseWidget*> childs;

        Tab(const char* txt);
	};

	vector<Tab*> tabs;

	int cur_tab;

	void ShowTab(int index, bool show);

public:
	TabSheetWidget(int id, BaseWidget* parent, const char* txt, float x, float y, float w, float h,EAutoSizeType sizerType = BaseWidget::eAutoSizeNone);
	virtual ~TabSheetWidget();

	virtual float GetWidth();
	virtual float GetHeight();

	virtual float GetXOffset();
	virtual float GetYOffset();

	virtual void ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam);
	virtual unsigned AddTab(const char* txt);
	virtual void AddTabWidget(int index, BaseWidget* widget);

	virtual void SetCurrentTab(int index);
	virtual int  GetCurrentTab();

    bool CatchEraseBkgnd(){return false;}
    void ClearTabs();
    void CloseTab(int index);
    void SetTabName(int index,const char* sName);
    int  GetCurTabIndex(){return cur_tab;}

    static float gY_Offset;
};
