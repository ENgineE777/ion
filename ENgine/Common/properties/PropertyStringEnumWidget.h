
#pragma once

#ifdef EDITOR

#include "PropertyBaseWidget.h"
#include "Editor\Helpers\HistoryObject.h"

class DEF_EXPORT PropertyStringEnumWidget : public PropertyBaseWidget,public PropertiesHistoryObject<string>
{
	static int used_id;
	int id;
	bool need_sort;

public:

	PropertyStringEnumWidget(BaseWidget* parent, bool need_sort);
	string* value;

	LabelWidget*   prop_label;	
	ComboBoxWidget* prop_ed;

	void InitControls();	
	void StartEdit(string* vl, const char* name);

	virtual void Show(bool show);
	virtual bool CheckChange();
    virtual void Apply(const string& data);
};

#endif