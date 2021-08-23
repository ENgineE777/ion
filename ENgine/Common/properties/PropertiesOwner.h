
#pragma once

#include "PropertyBase.h"
#include "Common/TaskHolder.h"

#ifdef EDITOR
#include "PropertyBaseWidget.h"
#endif

class PropertiesOwner : public TaskHolder
{
public:

#ifdef EDITOR
	RolloutWidget* ed_rollout;
#endif
		
	PropertiesOwner();

	virtual void RegisterProperties();
	virtual void LoadProperties(IStreamLoader* loader);
	virtual void SaveProperties(IStreamSaver* saver);	
	virtual void Copy(PropertiesOwner* propHolder);	
	virtual bool HaveDifferences(PropertiesOwner* holder);	
	virtual void SaveDiffProperties(IStreamSaver* saver, PropertiesOwner* holder);	

	virtual vector<PropertyBase*>& Properties() = 0;	

#if defined(PC) || defined(OSUNIX)
	virtual void SetDefault();
#endif

#ifdef EDITOR
	void CreateEditorWidgets(BaseWidget* parent, const char* script_class_name);
	void ShowEditorWidgets(bool show);	
	virtual bool UpdateEditorWidgets();	
	virtual void OnPropertiesChanged();
#endif
};

#define PROP_START \
virtual vector<PropertyBase*>& Properties()\
{\
	static vector<PropertyBase*> rpr;\
	return rpr;\
}

#define PROP_REG( value_name )\
Properties().push_back(get_##value_name##prop_inst());

#define SET_SKIP( value_name, set )\
	get_##value_name##prop_inst()->SetSkipWidget(ed_rollout, set);
