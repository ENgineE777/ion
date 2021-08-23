
#pragma once

#ifdef EDITOR

#include "PropertyBaseWidget.h"
#include "Editor\Helpers\HistoryObject.h"

class DEF_EXPORT PropertyEnumWidget : public PropertyBaseWidget,public PropertiesHistoryObject<int>
{
	static int used_id;
	int id;	

public:

	PropertyEnumWidget(BaseWidget* parent);
	int* value;	
	vector<int>* table;

	LabelWidget*   prop_label;	
	ComboBoxWidget* prop_ed;

	void InitControls();	
	void StartEdit(int* vl, const char* name);

	virtual void Show(bool show);
	virtual bool CheckChange();
    virtual void Apply(const int& data);
};

#endif