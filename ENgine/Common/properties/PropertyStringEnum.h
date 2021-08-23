
#pragma once

#include "PropertyBase.h"

#ifdef EDITOR

#include "PropertyStringEnumWidget.h"

typedef void (*StringEnumPropEnumerator)(ComboBoxWidget* widget);

#endif

class PropertyStringEnum : public PropertyBase
{
public:

#ifdef EDITOR
	PropertyStringEnumWidget* widgets;
#endif
	
	virtual void Load(IStreamLoader* loader, PropertiesOwner* object);	
	virtual void Save(IStreamSaver* saver, PropertiesOwner* object);
    virtual void SaveDiff(IStreamSaver* saver, PropertiesOwner* object, PropertiesOwner* origin){Save(saver,object);}	
	virtual void Copy(PropertiesOwner* object, PropertiesOwner* sec_object);	
	virtual void EmbedingScript(const char* script_class_name);	
	virtual bool IsEqual(PropertiesOwner* object, PropertiesOwner* sec_object);

	virtual bool NeedSort() { return false; };
    
#ifdef EDITOR
	virtual StringEnumPropEnumerator Enumerator() { return null; };

	virtual void CreateWidget(RolloutWidget* parent, const char* script_class_name);	
	virtual bool UpdateWidget();	
	virtual void SetSkipWidget(RolloutWidget* parent, bool set);	
	virtual void PrepareWidgets(PropertiesOwner* object);
#endif
};


#ifdef EDITOR
#define STRING_ENUM_PROP(class_name, value_name, def_val, category_name, prop_name, need_sort, enumerator_name)\
class value_name##prop_cl : public PropertyStringEnum\
{\
public:\
	virtual bool NeedSort() { return need_sort; };\
	virtual StringEnumPropEnumerator Enumerator() { return enumerator_name; };\
	virtual byte* GetPtr(PropertiesOwner* object) { return (byte*)(&((class_name*)object)->value_name); };\
	virtual int GetOffset() { return offsetof(class_name, value_name); };\
	virtual void SetDefault(PropertiesOwner* object) { ((class_name*)object)->value_name = def_val; };\
	virtual const char* GetName() { return #value_name; };\
	virtual const char* GetScriptName() { return prop_name; };\
	virtual const char* GetCategory() { return category_name; };\
};\
PropertyBase* get_##value_name##prop_inst()\
{\
	static value_name##prop_cl value_name##prop_inst;\
	return &(value_name##prop_inst);\
};\
string value_name;
#endif

#ifndef EDITOR
#define STRING_ENUM_PROP(class_name, value_name, def_val, category_name, prop_name, need_sort, enumerator_name)\
class value_name##prop_cl : public PropertyStringEnum\
{\
public:\
	virtual bool NeedSort() { return need_sort; };\
	virtual byte* GetPtr(PropertiesOwner* object) { return (byte*)(&((class_name*)object)->value_name); };\
	virtual int GetOffset() { return offsetof(class_name, value_name); };\
	virtual void SetDefault(PropertiesOwner* object) { ((class_name*)object)->value_name = def_val; };\
	virtual const char* GetName() { return #value_name; };\
	virtual const char* GetScriptName() { return prop_name; };\
	virtual const char* GetCategory() { return category_name; };\
};\
	PropertyBase* get_##value_name##prop_inst()\
{\
	static value_name##prop_cl value_name##prop_inst;\
	return &(value_name##prop_inst);\
};\
	string value_name;
#endif