
#pragma once

#include "PropertyBase.h"

#ifdef EDITOR

#include "PropertyFontWidget.h"

#endif

class PropertyFont : public PropertyBase
{
public:

#ifdef EDITOR
	PropertyFontWidget* widgets;
#endif

	virtual void Load(IStreamLoader* loader, PropertiesOwner* object);	
	virtual void Save(IStreamSaver* saver, PropertiesOwner* object);
    virtual void SaveDiff(IStreamSaver* saver, PropertiesOwner* object, PropertiesOwner* origin);	
	virtual void Copy(PropertiesOwner* object, PropertiesOwner* sec_object);	
	virtual void EmbedingScript(const char* script_class_name);	
	virtual bool IsEqual(PropertiesOwner* object, PropertiesOwner* sec_object);

#ifdef EDITOR
	virtual void CreateWidget(RolloutWidget* parent, const char* script_class_name);	
	virtual bool UpdateWidget();	
	virtual void SetSkipWidget(RolloutWidget* parent, bool set);	
	virtual void PrepareWidgets(PropertiesOwner* object);
	virtual int  WidgetHeight() { return 25 * 8; };
#endif
};

#define FONT_PROP( class_name, value_name, category_name, prop_name )\
class value_name##prop_cl : public PropertyFont\
{\
public:\
	virtual byte* GetPtr(PropertiesOwner* object) { return (byte*)(&((class_name*)object)->value_name); };\
	virtual int GetOffset() { return offsetof(class_name, value_name); };\
	virtual void SetDefault(PropertiesOwner* object) {  };\
	virtual const char* GetName() { return #value_name; };\
	virtual const char* GetScriptName() { return prop_name; };\
	virtual const char* GetCategory() { return category_name; };\
};\
PropertyBase* get_##value_name##prop_inst()\
{\
	static value_name##prop_cl value_name##prop_inst;\
	return &(value_name##prop_inst);\
};\
FontParams value_name;