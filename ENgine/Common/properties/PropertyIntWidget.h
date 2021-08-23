
#pragma once

#ifdef EDITOR

#include "PropertyBaseWidget.h"
#include "Editor\Helpers\HistoryObject.h"

class DEF_EXPORT PropertyIntWidget : public PropertyBaseWidget,public PropertiesHistoryObject<int>
{
	static int used_id;
	int id;

public:

	PropertyIntWidget(BaseWidget* parent);
	int* value;
	int  newValue;

	LabelWidget*   prop_label;	
	TextBoxWidget* prop_ed;

	void InitControls();
	void StartEdit(int* vl, const char* name);
	virtual void Apply();


	virtual void Show(bool show);
	virtual bool CheckChange();
    virtual void Apply(const int& data);
};


#endif