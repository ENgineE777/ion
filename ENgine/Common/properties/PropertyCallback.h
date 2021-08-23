
#pragma once

#include "PropertyBase.h"

#ifdef EDITOR

#include "PropertyCallbackWidget.h"

class PropertyCallback : public PropertyBase
{
public:

	PropertyCallbackWidget* widgets;

	virtual void Load(IStreamLoader* loader, PropertiesOwner* object);	
	virtual void Save(IStreamSaver* saver, PropertiesOwner* object);
    virtual void SaveDiff(IStreamSaver* saver, PropertiesOwner* object, PropertiesOwner* origin){Save(saver,object);}	
	virtual void Copy(PropertiesOwner* object, PropertiesOwner* sec_object);	
	virtual void EmbedingScript(const char* script_class_name);	
	virtual bool IsEqual(PropertiesOwner* object, PropertiesOwner* sec_object);
	virtual CallbackProperty Callback() { return null; };

	virtual void CreateWidget(RolloutWidget* parent, const char* script_class_name);	
	virtual bool UpdateWidget();	
	virtual void SetSkipWidget(RolloutWidget* parent, bool set);	
	virtual void PrepareWidgets(PropertiesOwner* object);		
};

#define CALLBACK_PROP( class_name, value_name, set_callback, category_name, prop_name )\
class value_name##prop_cl : public PropertyCallback\
{\
public:\
	virtual byte* GetPtr(PropertiesOwner* object) { return null; };\
	virtual CallbackProperty Callback() { return set_callback; };\
	virtual int GetOffset() { return 0; };\
	virtual void SetDefault(PropertiesOwner* object) { };\
	virtual const char* GetName() { return #value_name; };\
	virtual const char* GetScriptName() { return prop_name; };\
	virtual const char* GetCategory() { return category_name; };\
};\
PropertyBase* get_##value_name##prop_inst()\
{\
	static value_name##prop_cl value_name##prop_inst;\
	return &(value_name##prop_inst);\
};

#endif