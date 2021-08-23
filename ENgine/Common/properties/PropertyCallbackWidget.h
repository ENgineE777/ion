
#pragma once

#ifdef EDITOR

#include "PropertyBaseWidget.h"

class PropertiesOwner;
typedef void (*CallbackProperty)(PropertiesOwner* owner);

class DEF_EXPORT PropertyCallbackWidget : public PropertyBaseWidget
{
	static int used_id;
	int id;	
	CallbackProperty callback;

public:	

	PropertyCallbackWidget(BaseWidget* parent);

	ButtonWidget* prop_ed;	

	void InitControls(CallbackProperty set_callback, const char* name);	
	virtual void Show(bool show);	
	void StartEdit(PropertiesOwner* prop_owner);
	void CheckClick();
};

#endif