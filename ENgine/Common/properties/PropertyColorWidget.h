
#pragma once

#ifdef EDITOR

#include "PropertyBaseWidget.h"
#include "Editor\Helpers\HistoryObject.h"

class DEF_EXPORT PropertyColorWidget : public PropertyBaseWidget,public PropertiesHistoryObject<Color>
{
	static int used_id;
	int id;

public:

	PropertyColorWidget(BaseWidget* parent);
	Color* value;

	LabelWidget*   prop_label;	
	LabelWidget*   prop_ed;

	void InitControls();
	void StartEdit(Color* vl, const char* name);

	virtual void Show(bool show);
	virtual bool CheckChange();
    virtual void Apply(const Color& data);
};


#endif