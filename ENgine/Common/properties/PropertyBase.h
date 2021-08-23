
#pragma once

#include "Core/Files/IStreamLoader.h"
#include "Core/Files/IStreamSaver.h"

#ifdef EDITOR

#include "Platforms/PC/GUI/RolloutWidget.h"

#endif

class PropertiesOwner;

class PropertyBase
{
public:		
		
	/*enum ProperyType
	{
		+prop_bool = 0,
		+prop_int = 1,
		+prop_enum = 2,
		+prop_string_enum = 3,
		+prop_float = 4,
		+prop_string = 5,
		+prop_filename = 6,
		+prop_color = 7,
		prop_font = 8,
		+prop_callback=9,
		prop_forcedword = 0xffffffff
	};

	virtual ProperyType GetType() = 0;*/
	virtual void Load(IStreamLoader* loader, PropertiesOwner* object) = 0;
	virtual void Save(IStreamSaver* saver, PropertiesOwner* object) = 0;	
    virtual void SaveDiff(IStreamSaver* saver, PropertiesOwner* object, PropertiesOwner* origin) = 0;	
	virtual void Copy(PropertiesOwner* object, PropertiesOwner* sec_object) = 0;		
	virtual void EmbedingScript(const char* script_class_name) = 0;
	virtual bool IsEqual(PropertiesOwner* object, PropertiesOwner* sec_object) = 0;	

	virtual byte* GetPtr(PropertiesOwner* object) = 0;
	virtual int GetOffset() = 0;
	virtual const char* GetName() = 0;
	virtual const char* GetScriptName() = 0;

	virtual void SetDefault(PropertiesOwner* object) = 0;
	
#ifdef EDITOR
	virtual const char* GetCategory() = 0;	


	virtual void CreateWidget(RolloutWidget* parent, const char* script_class_name) {};	
	virtual void PrepareWidgets(PropertiesOwner* object) {};
	virtual int  WidgetHeight() { return 25; };
	virtual bool UpdateWidget() { return false; };	
	virtual void SetSkipWidget(RolloutWidget* parent, bool set) {};

#endif
};