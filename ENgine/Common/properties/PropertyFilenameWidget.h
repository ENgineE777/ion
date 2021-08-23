
#pragma once

#ifdef EDITOR

#include "PropertyBaseWidget.h"

class DEF_EXPORT PropertyFilenameWidget : public PropertyBaseWidget,PropertiesHistoryObject<string>
{
	static int used_id;
	int id;	
    void UpdateFileName();

public:

	string* file_name;
	char  res_type[16];

	PropertyFilenameWidget(BaseWidget* parent);	

	LabelWidget*  prop_label;
	ButtonWidget* prop_ed;
	ButtonWidget* del_ed;

	void InitControls(const char* tp);	
	void StartEdit(string* fl_name, const char* name);

	virtual void Show(bool show);
	virtual bool CheckChange();
    virtual void Apply(const string& data);
};

#endif