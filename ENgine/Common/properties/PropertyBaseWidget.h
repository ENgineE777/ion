
#pragma once

#ifdef EDITOR

#include "PropertyBase.h"

#include "Platforms/PC/GUI/ButtonWidget.h"
#include "Platforms/PC/GUI/LabelWidget.h"
#include "Platforms/PC/GUI/TextBoxWidget.h"
#include "Platforms/PC/GUI/CheckBoxWidget.h"
#include "Platforms/PC/GUI/ComboBoxWidget.h"
#include "Platforms/PC/GUI/RolloutWidget.h"
#include "Common/Containers/HashMap.h"
#include "Common/TaskHolder.h"


class PropertiesOwner;

class DEF_EXPORT PropertyBaseWidgetsHolder
{
public:
	static HashMap<BaseWidget> widgets;

	static BaseWidget* GetWidget(const char* owner_name, const char* prop_name);
	static void AddWidget(BaseWidget* widget, const char* owner_name, const char* prop_name);
};

class PropertyBaseWidget : public LabelWidget 
{
	class Task : public TaskHoldersOwner
	{
		PropertyBase*        pProperty;
		PropertyBaseWidget*  pOwner;
		bool                 bActive; 

	public:
		Task();		
		void ApplyChanges(float dt, int level);
		void Start(float delay);
		void Init(PropertyBase* prop, PropertyBaseWidget* owner);		
    };
protected:
    void            ApplyChanges(float dt, int level){}
    void            StartApplyTask(float delay){}
    virtual void    Apply(){}

public:

	static float apply_property_delay;
    static float property_label_width;

    PropertiesOwner* owner;
    Task             applyTask;

	PropertyBaseWidget(BaseWidget* parent);

};

#endif