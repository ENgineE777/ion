
#pragma once

#ifdef EDITOR

#include "PropertyBaseWidget.h"
#include "Editor\Helpers\HistoryObject.h"

class DEF_EXPORT PropertyStringWidget : public PropertyBaseWidget,public PropertiesHistoryObject<string>
{
	static int used_id;
	int id;

public:

	PropertyStringWidget(BaseWidget* parent);
	string* value;

	LabelWidget*   prop_label;	
	TextBoxWidget* prop_ed;

	void InitControls();	
	void StartEdit(string* vl, const char* name);

	virtual void Show(bool show);
	virtual bool CheckChange();
    virtual void Apply(const string& data);
};

#endif