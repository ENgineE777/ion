
#pragma once

#include "BaseWidget.h"

class DEF_EXPORT SizerWidget : public BaseWidget
{	
	bool vert_sizer;
	int  size;	

public:
	
	SizerWidget(int id, BaseWidget* parent, bool vert_sizer, int size);	
	virtual ~SizerWidget();

	virtual void ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam);

	bool IsVertical();
	int  GetSize();

	virtual void Resize();

};
