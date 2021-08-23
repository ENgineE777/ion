
#pragma once

#include "BaseWidget.h"

class DEF_EXPORT RolloutWidget : public BaseWidget
{		
	struct Rollout
	{
		bool  opened;
		char  name[128];
		vector<BaseWidget*> childs;
		float y;

		Rollout()
		{
			opened = false;
			name[0] = 0;
			y = 0;
		};
	};

	vector<Rollout> rollouts;

	vector<BaseWidget*> skipped_list;

	HFONT hFont;

    void CalculateHeigth();
public:

	Color color;

	RolloutWidget(int id, BaseWidget* parent, const char* txt, float x, float y, float w, float h,EAutoSizeType sizerType = BaseWidget::eAutoSizeNone);
	virtual ~RolloutWidget();

	virtual void ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam);	
	
	virtual void AddRolloutChild(const char* name, BaseWidget* widget);

	virtual void SetSkipChild(BaseWidget* widget, bool set);

	virtual void Draw();

	virtual void OnLeftMouseUp(int mx, int my);

	virtual void CalcPos();

};
