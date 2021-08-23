
#pragma once

#ifdef EDITOR

#include "PropertyBaseWidget.h"
#include "Editor\Helpers\HistoryObject.h"

class DEF_EXPORT PropertyFloatWidget : public PropertyBaseWidget,public PropertiesHistoryObject<float>
{
	static int used_id;
	int id;

public:

	PropertyFloatWidget(BaseWidget* parent);
	float* value;
	float  newValue;

	LabelWidget*   prop_label;	
	TextBoxWidget* prop_ed;

	void InitControls();	
	void StartEdit(float* vl, const char* name);
	virtual void Apply();

	virtual void Show(bool show);
	virtual bool CheckChange();
    virtual void Apply(const float& data);
};


#endif