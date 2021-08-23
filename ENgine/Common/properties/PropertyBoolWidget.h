
#pragma once

#ifdef EDITOR

#include "PropertyBaseWidget.h"
#include "Editor\Helpers\HistoryObject.h"

class DEF_EXPORT PropertyBoolWidget : public PropertyBaseWidget,public PropertiesHistoryObject<bool>
{
	static int used_id;
	int id;

public:

	PropertyBoolWidget(BaseWidget* parent);
	bool* value;

	LabelWidget*   prop_label;	
	CheckBoxWidget* prop_ed;

	void InitControls();	
	void StartEdit(bool* vl, const char* name);

	virtual void Show(bool show);
	virtual bool CheckChange();
    virtual void Apply(const bool& data);
};

#endif