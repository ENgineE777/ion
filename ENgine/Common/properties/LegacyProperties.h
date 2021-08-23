
#pragma once

#include <vector>

#include "Common/TaskHolder.h"

#include "Core/Files/IStreamLoader.h"
#include "Core/Files/IStreamSaver.h"

#ifdef EDITOR	

#include "Common/properties/Properties2.h"

#include "Platforms/PC/GUI/ButtonWidget.h"
#include "Platforms/PC/GUI/LabelWidget.h"
#include "Platforms/PC/GUI/TextBoxWidget.h"
#include "Platforms/PC/GUI/CheckBoxWidget.h"
#include "Platforms/PC/GUI/ComboBoxWidget.h"
#include "Platforms/PC/GUI/RolloutWidget.h"
#include "Common/Containers/HashMap.h"
#include "Common/TaskHolder.h"

class DEF_EXPORT PropertyWidgetsHolder
{
public:
	static HashMap<BaseWidget> widgets;

	static BaseWidget* GetWidget(const char* owner_name, const char* prop_name);
	static void AddWidget(BaseWidget* widget, const char* owner_name, const char* prop_name);
};

#endif

class PropertyHolder;

#ifdef EDITOR

#endif

class Property
{
public:
	
	char name[32];
	PropertyHolder* owner;

	virtual ~Property() {};
	virtual void Init(PropertyHolder* own) = 0;
	virtual void TieValues() = 0;	
	virtual void ReverseTieValues() {};
	
	const char* GetName() { return name; };

	virtual void Load(IStreamLoader* loader) {};
	virtual void Save(IStreamSaver* saver) {};
	
	virtual void Copy(Property* prop) {};		

	virtual void EmbedingScript() {};

#ifdef EDITOR	
	virtual void CreateWidget(RolloutWidget* parent) {};	
	virtual void PrepareWidgets() {};
	virtual int  WidgetHeight() { return 25; };
	virtual bool UpdateWidget() { return false; };	
	virtual void SetSkipWidget(RolloutWidget* parent, bool set) {};
#endif

	virtual bool IsEqual(Property* prop) { return true; };
	virtual void* GetValue() { return null; };
	virtual bool GetAsBool() { return false; };
	virtual int  GetAsInt() { return 0; };
	virtual float GetAsFloat() { return 0.0f; };
	virtual const char* GetAsString() { return ""; };

};

#ifdef EDITOR
class DEF_EXPORT PropertyWidget : public LabelWidget 
{
    class Task :public TaskHoldersOwner
    {
    public:
        Task():propertyOwner(NULL),active(false),pOwner(NULL),id(0){}
        void ApplyChanges(float dt, int level);
        void Start(float delay);
        void Init(Property* prop,PropertyWidget* owner,unsigned _id = 0){propertyOwner = prop;pOwner = owner; id = _id;}
    private:
        Property* propertyOwner;
        PropertyWidget * pOwner;
        bool      active; 
        unsigned  id;
    };

public:
	PropertyWidget(BaseWidget* parent) : LabelWidget(2000, parent, "", false, 0, 0, 200,18, false),propertyOwner(NULL),activeTask(false)
	{}
    void ApplyChanges(float dt, int level);
    void StartApplyTask(float delay);
	void SetPropertyOwner(Property* prop)
	{
		propertyOwner = prop;
        applyTask.Init(prop,this);
	}
    virtual bool CanApply(unsigned taskId) {return true;}

protected:	
	Property* propertyOwner;
    bool      activeTask; 
    Task      applyTask;
};
#endif


class PropertyHolder : public TaskHolder, public PropertiesOwner
{
public:

	std::vector<Property*> prop;	
	bool allow_reinit;
	char script_class_name[128];

#ifdef EDITOR
	RolloutWidget* rollout;
#endif

	PropertyHolder()
	{		
		allow_reinit = true;

#ifdef EDITOR
		rollout = null;
#endif
	};

	virtual ~PropertyHolder() {}

	virtual void Init()
	{
	};

	virtual void OnPropertiesChanged()
	{
	};
	
	virtual void LoadProperties(IStreamLoader* loader)
	{
		for (int i=0;i<(int)prop.size();i++)
		{
			prop[i]->Load(loader);
		}		
	}	

	virtual void SaveProperties(IStreamSaver* saver)
	{
		for (int i=0;i<(int)prop.size();i++)
		{
			prop[i]->Save(saver);
		}
	}

	virtual void Copy(PropertyHolder* propHolder)
	{
		for (int i=0;i<(int)prop.size();i++)
		{
			prop[i]->Copy(propHolder->prop[i]);
		}
	}

	void TieValues()
	{
		for (int i=0;i<(int)prop.size();i++)
		{
			prop[i]->TieValues();
		}		
	};

	virtual bool IsEqual(PropertyHolder* holder)
	{
		for (int j=0;j<(int)prop.size();j++)
		{
			if (!prop[j]->IsEqual(holder->prop[j]))
			{
				return false;
			}
		}

		return true;
	}

	virtual void SaveDiffProperties(IStreamSaver* saver, PropertyHolder* holder)
	{
		for (int i=0;i<(int)prop.size();i++)
		{
			if (!prop[i]->IsEqual(holder->prop[i]))
			{
				prop[i]->Save(saver);
			}			
		}
	}

#ifdef EDITOR

	void CreateWidgets(BaseWidget* parent)
	{		
		if (rollout) return;

		rollout = (RolloutWidget*)PropertyWidgetsHolder::GetWidget(script_class_name, "");

		if (!rollout)
		{
			rollout = new RolloutWidget(111, parent, "", 0, 0, 205, 1000, false);		
			PropertyWidgetsHolder::AddWidget(rollout, script_class_name, "");
		}


		for (int i=0;i<(int)prop.size();i++)
		{	
			prop[i]->CreateWidget(rollout);			
		}		
	}	
	
	void ShowWidgets(bool show)
	{		
		if (!rollout) return;

		rollout->Show(show);

		if (rollout)
		{
			for (int i=0;i<(int)prop.size();i++)
			{
				prop[i]->PrepareWidgets();
			}
		}
	}	

	virtual bool UpdateWidgets()
	{
		bool res = false;

		for (int i=0;i<(int)prop.size();i++)
		{
			res |=prop[i]->UpdateWidget();
		}		

		return res;
	};	

	void ReverseTieValues()
	{
		for (int i=0;i<(int)prop.size();i++)
		{
			prop[i]->ReverseTieValues();
		}		
	};	

#endif
};	

#ifdef EDITOR

class DEF_EXPORT BoolPropWidget : public PropertyWidget
{
	static int used_id;
	int id;

public:

	BoolPropWidget( BaseWidget* parent);
	bool* value;

	LabelWidget*   prop_label;	
	CheckBoxWidget* prop_ed;

	void InitControls();	
	void StartEdit(bool* vl, const char* name);
	
	virtual void Show(bool show);
	virtual bool CheckChange();
};

#define BOOL_PROP( class_name, value_name, def_val, category_name, prop_name )\
class value_name##prop_cl : public Property\
{\
public:\
	bool def_value;\
	bool value;\
	BoolPropWidget* widgets;\
	virtual void Init(PropertyHolder* own)\
	{\
		owner = own;\
		strcpy(name, prop_name);\
		def_value = def_val;\
		value = def_val;\
		TieValues();\
		widgets = null;\
	}\
	virtual void TieValues()\
	{\
		((class_name*)owner)->value_name = value;\
	}\
	virtual void TieValuesReversely()\
	{\
		value = ((class_name*)owner)->value_name;\
	}\
	virtual void ReverseTieValues()\
	{\
		value = ((class_name*)owner)->value_name;\
		if (widgets) widgets->StartEdit(&value, name);\
	}\
	virtual void Load(IStreamLoader* loader)\
	{\
		loader->Read(#value_name, value);\
		TieValues();\
	};\
	virtual void Save(IStreamSaver* saver)\
	{\
		saver->Write(#value_name, value);\
	};\
	virtual void Copy(Property* prop)\
	{\
		value_name##prop_cl* prp = (value_name##prop_cl*)prop;\
		prp->value = value;\
		prp->TieValues();\
	};\
	virtual void EmbedingScript()\
	{\
		core->Script()->Machine()->RegisterObjectProperty(owner->script_class_name, "bool "prop_name,offsetof(class_name, value_name));\
	};\
	virtual void CreateWidget(RolloutWidget* parent)\
	{\
		widgets = (BoolPropWidget*)PropertyWidgetsHolder::GetWidget(owner->script_class_name, #value_name);\
		if (!widgets)\
		{\
			widgets = new BoolPropWidget(parent);\
			PropertyWidgetsHolder::AddWidget(widgets, owner->script_class_name, #value_name);\
			widgets->InitControls();\
			widgets->SetSize(220, WidgetHeight());\
			parent->AddRolloutChild(category_name, widgets);\
		}\
	};\
	virtual bool UpdateWidget()\
	{\
		if (widgets) widgets->CheckChange();\
		return false;\
	};\
	virtual void SetSkipWidget(RolloutWidget* parent, bool set)\
	{\
		parent->SetSkipChild(widgets, set);\
	};\
	virtual void PrepareWidgets()\
	{\
		widgets->SetPropertyOwner(this);\
		TieValuesReversely();\
		widgets->StartEdit(&value, name);\
	};\
	virtual bool IsEqual(Property* prop)\
	{\
		return value == ((value_name##prop_cl*)prop)->value;\
	};\
};\
value_name##prop_cl value_name##prop_inst;\
bool value_name;

#else

#define BOOL_PROP( class_name, value_name, def_val, category_name, prop_name )\
class value_name##prop_cl : public Property\
{\
public:\
	bool def_value;\
	bool value;\
	virtual void Init(PropertyHolder* own)\
	{\
		owner = own;\
		strcpy(name, prop_name);\
		def_value = def_val;\
		value = def_val;\
		TieValues();\
	};\
	virtual void TieValues()\
	{\
		((class_name*)owner)->value_name = value;\
	};\
	virtual void Load(IStreamLoader* loader)\
	{\
		loader->Read(#value_name, value);\
		TieValues();\
	};\
	virtual void Save(IStreamSaver* saver)\
	{\
		saver->Write(#value_name, value);\
	};\
	virtual void Copy(Property* prop)\
	{\
		value_name##prop_cl* prp = (value_name##prop_cl*)prop;\
		prp->value = value;\
		prp->TieValues();\
	};\
	virtual void EmbedingScript()\
	{\
		core->Script()->Machine()->RegisterObjectProperty(owner->script_class_name, "bool "prop_name,offsetof(class_name, value_name));\
	};\
	virtual bool IsEqual(Property* prop)\
	{\
		return value == ((value_name##prop_cl*)prop)->value;\
	};\
};\
value_name##prop_cl value_name##prop_inst;\
bool value_name;

#endif


#ifdef EDITOR

class DEF_EXPORT IntPropWidget : public PropertyWidget
{
	static int used_id;
	int id;

public:

	IntPropWidget( BaseWidget* parent);
	int* value;
    int  newValue;

	LabelWidget*   prop_label;	
	TextBoxWidget* prop_ed;

	void InitControls();
	void StartEdit(int* vl, const char* name);
    virtual bool CanApply(unsigned taskId);
  
	
	virtual void Show(bool show);
	virtual bool CheckChange();
};

#define INT_PROP( class_name, value_name, def_val, category_name, prop_name )\
class value_name##prop_cl : public Property\
{\
	public:\
	int def_value;\
	int value;\
	IntPropWidget* widgets;\
	virtual void Init(PropertyHolder* own)\
	{\
		owner = own;\
		strcpy(name, prop_name);\
		def_value = def_val;\
		value = def_val;\
		TieValues();\
		widgets = null;\
	}\
	virtual void TieValues()\
	{\
		((class_name*)owner)->value_name = value;\
	}\
	virtual void TieValuesReversely()\
	{\
		value = ((class_name*)owner)->value_name;\
	}\
	virtual void ReverseTieValues()\
	{\
		value = ((class_name*)owner)->value_name;\
		if (widgets) widgets->StartEdit(&value, name);\
	}\
	virtual void Load(IStreamLoader* loader)\
	{\
		loader->Read(#value_name, value);\
		TieValues();\
	};\
	virtual void Save(IStreamSaver* saver)\
	{\
		saver->Write(#value_name, value);\
	};\
	virtual void Copy(Property* prop)\
	{\
		value_name##prop_cl* prp = (value_name##prop_cl*)prop;\
		prp->value = value;\
		prp->TieValues();\
	};\
	virtual void EmbedingScript()\
	{\
		core->Script()->Machine()->RegisterObjectProperty(owner->script_class_name, "int "prop_name,offsetof(class_name, value_name));\
	};\
	virtual void CreateWidget(RolloutWidget* parent)\
	{\
		widgets = (IntPropWidget*)PropertyWidgetsHolder::GetWidget(owner->script_class_name, #value_name);\
		if (!widgets)\
		{\
			widgets = new IntPropWidget(parent);\
			PropertyWidgetsHolder::AddWidget(widgets, owner->script_class_name, #value_name);\
			widgets->InitControls();\
			widgets->SetSize(220, WidgetHeight());\
			parent->AddRolloutChild(category_name, widgets);\
		}\
	};\
	virtual bool UpdateWidget()\
	{\
		if (widgets) widgets->CheckChange();\
		return false;\
	};\
	virtual void SetSkipWidget(RolloutWidget* parent, bool set)\
	{\
		parent->SetSkipChild(widgets, set);\
	};\
	virtual void PrepareWidgets()\
	{\
		widgets->SetPropertyOwner(this);\
		TieValuesReversely();\
		widgets->StartEdit(&value, name);\
	};\
	virtual bool IsEqual(Property* prop)\
	{\
		return value == ((value_name##prop_cl*)prop)->value;\
	};\
};\
value_name##prop_cl value_name##prop_inst;\
int value_name;

#else

#define INT_PROP( class_name, value_name, def_val, category_name, prop_name )\
class value_name##prop_cl : public Property\
{\
public:\
	int def_value;\
	int value;\
	virtual void Init(PropertyHolder* own)\
	{\
		owner = own;\
		strcpy(name, prop_name);\
		def_value = def_val;\
		value = def_val;\
		TieValues();\
	}\
	virtual void TieValues()\
	{\
		((class_name*)owner)->value_name = value;\
	}\
	virtual void Load(IStreamLoader* loader)\
	{\
		loader->Read(#value_name, value);\
		TieValues();\
	};\
	virtual void Save(IStreamSaver* saver)\
	{\
		saver->Write(#value_name, value);\
	};\
	virtual void Copy(Property* prop)\
	{\
		value_name##prop_cl* prp = (value_name##prop_cl*)prop;\
		prp->value = value;\
		prp->TieValues();\
	};\
	virtual void EmbedingScript()\
	{\
		core->Script()->Machine()->RegisterObjectProperty(owner->script_class_name, "int "prop_name,offsetof(class_name, value_name));\
	};\
	virtual bool IsEqual(Property* prop)\
	{\
		return value == ((value_name##prop_cl*)prop)->value;\
	};\
};\
value_name##prop_cl value_name##prop_inst;\
int value_name;

#endif


#ifdef EDITOR

class DEF_EXPORT EnumPropWidget : public PropertyWidget
{
	static int used_id;
	int id;
	bool need_sort;

public:

	EnumPropWidget(BaseWidget* parent, bool need_sort);
	int* value;

	LabelWidget*   prop_label;	
	ComboBoxWidget* prop_ed;

	void InitControls();	
	void StartEdit(int* vl, const char* name);
	
	virtual void Show(bool show);
	virtual bool CheckChange();
};

typedef void (*EnumPropEnumerator)(ComboBoxWidget* widget);

#define ENUM_PROP( class_name, value_name, def_val, prop_name )\
class value_name##prop_cl : public Property\
{\
public:\
	int def_value;\
	int value;\
	EnumPropWidget* widgets;\
	vector<int> table;\
	vector<const char*> tableNames;\
	int version;\
	int cur_version;\
	EnumPropEnumerator enumerator;\
	struct CompatabilityTable\
	{\
		int version;\
		vector<int> table;\
	};\
	vector<CompatabilityTable> cmtb_tables;\
	virtual void EmbedingScript()\
	{\
		core->Script()->Machine()->RegisterObjectProperty(owner->script_class_name, "int "prop_name,offsetof(class_name, value_name));\
	};\
	virtual void Init(PropertyHolder* own)\
	{\
		owner = own;\
		strcpy(name, prop_name);\
		def_value = def_val;\
		value = def_val;\
		TieValues();\
		widgets = null;\
		enumerator = null;\
		version = 1;\
		cur_version = 1;

#define ENUM_VERSION( ver ) version = ver;

#define CMT_TABLE_START( ver ) \
		{\
			cmtb_tables.push_back(CompatabilityTable());\
			CompatabilityTable& tbl = cmtb_tables[cmtb_tables.size() - 1];\
			tbl.version = ver;

#define CMT_TABLE_ITEM( item_value ) \
			tbl.table.push_back(item_value);

#define CMT_TABLE_END() \
		}

#define ENUM_ITEM( item_name, item_value)\
	    table.push_back(item_value);\
		tableNames.push_back(item_name);
#define ENUM_ENUMERATOR(enumerator_name)\
		enumerator = enumerator_name;

#define ENUM_END( class_name, value_name, need_sort, category_name)\
	}\
	virtual void TieValues()\
	{\
		if(value < table.size())\
			((class_name*)owner)->value_name = table[value];\
		else\
			((class_name*)owner)->value_name = def_value;\
	}\
	virtual void ReverseTieValues()\
	{\
		int owner_value = ((class_name*)owner)->value_name;\
		value = owner_value;\
		for (int i=0;i<table.size();i++)\
		{\
			if (table[i] == owner_value)\
			{\
				value = i;\
				break;\
			}\
		}\
		if (widgets) widgets->StartEdit(&value, name);\
	}\
	virtual void Load(IStreamLoader* loader)\
	{\
		loader->Read("version", cur_version);\
		loader->Read(#value_name, value);\
		if (cur_version < version)\
		{\
			for (int i=0;i<cmtb_tables.size();i++)\
			{\
				if (cmtb_tables[i].version == cur_version)\
				{\
					value = cmtb_tables[i].table[value];\
					break;\
				}\
			}\
		}\
		TieValues();\
	};\
	virtual void Save(IStreamSaver* saver)\
	{\
		if (saver->GetType() == IStreamSaver::xml) saver->Write("version", version);\
		saver->Write(#value_name, value);\
	};\
	virtual void Copy(Property* prop)\
	{\
		value_name##prop_cl* prp = (value_name##prop_cl*)prop;\
		prp->value = value;\
		prp->TieValues();\
	};\
	virtual void CreateWidget(RolloutWidget* parent)\
	{\
		widgets = (EnumPropWidget*)PropertyWidgetsHolder::GetWidget(owner->script_class_name, #value_name);\
		if (!widgets)\
		{\
			widgets = new EnumPropWidget(parent, need_sort);\
			PropertyWidgetsHolder::AddWidget(widgets, owner->script_class_name, #value_name);\
			widgets->InitControls();\
			widgets->SetSize(220, WidgetHeight());\
			if (enumerator)\
			{\
				enumerator(widgets->prop_ed);\
			}\
			else\
			{\
				for (int i=0;i<tableNames.size();i++) widgets->prop_ed->AddString(tableNames[i]);\
			}\
			parent->AddRolloutChild(category_name, widgets);\
		}\
	};\
	virtual bool UpdateWidget()\
	{\
		if (widgets) widgets->CheckChange();\
		return false;\
	};\
	virtual void SetSkipWidget(RolloutWidget* parent, bool set)\
	{\
		parent->SetSkipChild(widgets, set);\
	};\
	virtual void PrepareWidgets()\
	{\
		widgets->SetPropertyOwner(this);\
		widgets->StartEdit(&value, name);\
	};\
	virtual bool IsEqual(Property* prop)\
	{\
		return value == ((value_name##prop_cl*)prop)->value;\
	};\
};\
value_name##prop_cl value_name##prop_inst;\
int value_name;

#else

#define ENUM_PROP( class_name, value_name, def_val, prop_name )\
class value_name##prop_cl : public Property\
{\
public:\
	int def_value;\
	int value;\
	vector<int> table;\
	virtual void EmbedingScript()\
	{\
		core->Script()->Machine()->RegisterObjectProperty(owner->script_class_name, "int "prop_name,offsetof(class_name, value_name));\
	};\
	virtual void Init(PropertyHolder* own)\
	{\
		owner = own;\
		strcpy(name, prop_name);\
		def_value = def_val;\
		value = def_val;

#define ENUM_VERSION( ver ) ;
#define CMT_TABLE_START( ver ) ;
#define CMT_TABLE_ITEM( item_value ) ;
#define CMT_TABLE_END() ;

#define ENUM_ITEM( item_name , item_value)  table.push_back(item_value);

#define ENUM_ENUMERATOR( enumerator_name) ;

#define ENUM_END( class_name, value_name, need_sort, category_name)\
		TieValues();\
	}\
	virtual void TieValues()\
	{\
		if(value < table.size())\
			((class_name*)owner)->value_name = table[value];\
		else\
			((class_name*)owner)->value_name = def_value;\
	}\
	virtual void Load(IStreamLoader* loader)\
	{\
		loader->Read(#value_name, value);\
		TieValues();\
	};\
	virtual void Save(IStreamSaver* saver)\
	{\
		saver->Write(#value_name, value);\
	};\
	virtual void Copy(Property* prop)\
	{\
		value_name##prop_cl* prp = (value_name##prop_cl*)prop;\
		prp->value = value;\
		prp->TieValues();\
	};\
	virtual bool IsEqual(Property* prop)\
	{\
		return value == ((value_name##prop_cl*)prop)->value;\
	};\
};\
value_name##prop_cl value_name##prop_inst;\
int			value_name;


#endif


#ifdef EDITOR

class DEF_EXPORT StringEnumPropWidget : public PropertyWidget
{
	static int used_id;
	int id;
	bool need_sort;

public:

	StringEnumPropWidget(BaseWidget* parent, bool need_sort);
	char* value;

	LabelWidget*   prop_label;	
	ComboBoxWidget* prop_ed;

	void InitControls();	
	void StartEdit(char* vl, const char* name);
	
	virtual void Show(bool show);
	virtual bool CheckChange();
};

typedef void (*EnumPropEnumerator)(ComboBoxWidget* widget);

#define STRING_ENUM_PROP( class_name, value_name, def_val, prop_name )\
class value_name##prop_cl : public Property\
{\
public:\
	char def_value[512];\
	char value[512];\
	vector<const char*> tableNames;\
	StringEnumPropWidget* widgets;\
	EnumPropEnumerator enumerator;\
	virtual void Init(PropertyHolder* own)\
	{\
		owner = own;\
		strcpy(name, prop_name);\
		strcpy(def_value, def_val);\
		strcpy(value, def_val);\
		TieValues();\
		widgets = null;

#define STRING_ENUM_ITEM(item_name)\
		tableNames.push_back(item_name);

#define STRING_ENUM_ENUMERATOR(enumerator_name)\
		enumerator = enumerator_name;

#define STRING_ENUM_END( class_name, value_name, need_sort, category_name)\
	}\
	virtual void TieValues()\
	{\
		strcpy(((class_name*)owner)->value_name, value);\
	}\
	virtual void TieValuesReversely()\
	{\
		strcpy(value, ((class_name*)owner)->value_name);\
	}\
	virtual void ReverseTieValues()\
	{\
		strcpy(value, ((class_name*)owner)->value_name);\
		if (widgets) widgets->StartEdit(value, name);\
	}\
	virtual void Load(IStreamLoader* loader)\
	{\
		loader->Read(#value_name, value, 512);\
		TieValues();\
	};\
	virtual void Save(IStreamSaver* saver)\
	{\
		saver->Write(#value_name, value);\
	};\
	virtual void Copy(Property* prop)\
	{\
		value_name##prop_cl* prp = (value_name##prop_cl*)prop;\
		strcpy(prp->value, value);\
		prp->TieValues();\
	};\
	virtual void CreateWidget(RolloutWidget* parent)\
	{\
		widgets = (StringEnumPropWidget*)PropertyWidgetsHolder::GetWidget(owner->script_class_name, #value_name);\
		if (!widgets)\
		{\
			widgets = new StringEnumPropWidget(parent, need_sort);\
			PropertyWidgetsHolder::AddWidget(widgets, owner->script_class_name, #value_name);\
			widgets->InitControls();\
			widgets->SetSize(220, WidgetHeight());\
			if (enumerator)\
			{\
				enumerator(widgets->prop_ed);\
			}\
			else\
			{\
				for (int i=0;i<tableNames.size();i++) widgets->prop_ed->AddString(tableNames[i]);\
			}\
			parent->AddRolloutChild(category_name, widgets);\
		}\
	};\
	virtual bool UpdateWidget()\
	{\
		if (widgets) widgets->CheckChange();\
		return false;\
	};\
	virtual void SetSkipWidget(RolloutWidget* parent, bool set)\
	{\
		parent->SetSkipChild(widgets, set);\
	};\
	virtual void PrepareWidgets()\
	{\
		if (enumerator)\
		{\
			widgets->prop_ed->ClearList();\
			enumerator(widgets->prop_ed);\
		}\
		widgets->SetPropertyOwner(this);\
		widgets->StartEdit(value, name);\
	};\
	virtual bool IsEqual(Property* prop)\
	{\
		return strcmp(value, ((value_name##prop_cl*)prop)->value) == 0;\
	};\
};\
value_name##prop_cl value_name##prop_inst;\
char value_name[512];

#else

#define STRING_ENUM_PROP( class_name, value_name, def_val, prop_name )\
class value_name##prop_cl : public Property\
{\
public:\
	char def_value[512];\
	char value[512];\
	virtual void Init(PropertyHolder* own)\
	{\
		owner = own;\
		strcpy(name, prop_name);\
		strcpy(def_value, def_val);\
		strcpy(value, def_val);\
		TieValues();

#define STRING_ENUM_ITEM(item_name) ;

#define STRING_ENUM_ENUMERATOR(enumerator_name) ;

#define STRING_ENUM_END( class_name, value_name, need_sort, category_name)\
	}\
	virtual void TieValues()\
	{\
		strcpy(((class_name*)owner)->value_name, value);\
	}\
	virtual void TieValuesReversely()\
	{\
		strcpy(value, ((class_name*)owner)->value_name);\
	}\
	virtual void ReverseTieValues()\
	{\
		strcpy(value, ((class_name*)owner)->value_name);\
	}\
	virtual void Load(IStreamLoader* loader)\
	{\
		loader->Read(#value_name, value, 512);\
		TieValues();\
	};\
	virtual void Save(IStreamSaver* saver)\
	{\
		saver->Write(#value_name, value);\
	};\
	virtual void Copy(Property* prop)\
	{\
		value_name##prop_cl* prp = (value_name##prop_cl*)prop;\
		strcpy(prp->value, value);\
		prp->TieValues();\
	};\
	virtual bool IsEqual(Property* prop)\
	{\
		return strcmp(value, ((value_name##prop_cl*)prop)->value) == 0;\
	};\
};\
value_name##prop_cl value_name##prop_inst;\
char value_name[512];


#endif


#ifdef EDITOR

class DEF_EXPORT FloatPropWidget : public PropertyWidget
{
	static int used_id;
	int id;

public:

	FloatPropWidget( BaseWidget* parent);
	float* value;
    float  newValue;

	LabelWidget*   prop_label;	
	TextBoxWidget* prop_ed;

	void InitControls();	
	void StartEdit(float* vl, const char* name);
    virtual bool CanApply(unsigned taskId);
	
	virtual void Show(bool show);
	virtual bool CheckChange();
};

#define FLOAT_PROP( class_name, value_name, def_val, category_name, prop_name )\
class value_name##prop_cl : public Property\
{\
public:\
	float def_value;\
	float value;\
	FloatPropWidget* widgets;\
	virtual void Init(PropertyHolder* own)\
	{\
		owner = own;\
		strcpy(name, prop_name);\
		def_value = def_val;\
		value = def_val;\
		TieValues();\
		widgets = null;\
	}\
	virtual void TieValues()\
	{\
		((class_name*)owner)->value_name = value;\
	}\
	virtual void TieValuesReversely()\
	{\
		value = ((class_name*)owner)->value_name;\
	}\
	virtual void ReverseTieValues()\
	{\
		value = ((class_name*)owner)->value_name;\
		if (widgets) widgets->StartEdit(&value, name);\
	}\
	virtual float GetAsFloat() { return value; };\
	virtual void* GetValue() { return &((class_name*)owner)->value_name; };\
	virtual void Load(IStreamLoader* loader)\
	{\
		loader->Read(#value_name, value);\
		TieValues();\
	};\
	virtual void Save(IStreamSaver* saver)\
	{\
		saver->Write(#value_name, value);\
	};\
	virtual void Copy(Property* prop)\
	{\
		value_name##prop_cl* prp = (value_name##prop_cl*)prop;\
		prp->value = value;\
		prp->TieValues();\
	};\
	virtual void EmbedingScript()\
	{\
		core->Script()->Machine()->RegisterObjectProperty(owner->script_class_name, "float "prop_name,offsetof(class_name, value_name));\
	};\
	virtual void CreateWidget(RolloutWidget* parent)\
	{\
		widgets = (FloatPropWidget*)PropertyWidgetsHolder::GetWidget(owner->script_class_name, #value_name);\
		if (!widgets)\
		{\
			widgets = new FloatPropWidget(parent);\
			PropertyWidgetsHolder::AddWidget(widgets, owner->script_class_name, #value_name);\
			widgets->InitControls();\
			widgets->SetSize(220, WidgetHeight());\
			parent->AddRolloutChild(category_name, widgets);\
		}\
	};\
	virtual bool UpdateWidget()\
	{\
		if (widgets) widgets->CheckChange();\
		return false;\
	};\
	virtual void SetSkipWidget(RolloutWidget* parent, bool set)\
	{\
		parent->SetSkipChild(widgets, set);\
	};\
	virtual void PrepareWidgets()\
	{\
		widgets->SetPropertyOwner(this);\
		TieValuesReversely();\
		widgets->StartEdit(&value, name);\
	};\
	virtual bool IsEqual(Property* prop)\
	{\
		return fabs(value - ((value_name##prop_cl*)prop)->value) < 0.001f;\
	};\
};\
value_name##prop_cl value_name##prop_inst;\
float value_name;

#else

#define FLOAT_PROP( class_name, value_name, def_val, category_name, prop_name )\
class value_name##prop_cl : public Property\
{\
public:\
	float def_value;\
	float value;\
	virtual void Init(PropertyHolder* own)\
	{\
		owner = own;\
		strcpy(name, prop_name);\
		def_value = def_val;\
		value = def_val;\
		TieValues();\
	}\
	virtual void TieValues()\
	{\
		((class_name*)owner)->value_name = value;\
	}\
	virtual float GetAsFloat() { return value; };\
	virtual void* GetValue() { return &((class_name*)owner)->value_name; };\
	virtual void Load(IStreamLoader* loader)\
	{\
		loader->Read(#value_name, value);\
		TieValues();\
	};\
	virtual void Save(IStreamSaver* saver)\
	{\
		saver->Write(#value_name, value);\
	};\
	virtual void Copy(Property* prop)\
	{\
		value_name##prop_cl* prp = (value_name##prop_cl*)prop;\
		prp->value = value;\
		prp->TieValues();\
	};\
	virtual void EmbedingScript()\
	{\
		core->Script()->Machine()->RegisterObjectProperty(owner->script_class_name, "float "prop_name,offsetof(class_name, value_name));\
	};\
	virtual bool IsEqual(Property* prop)\
	{\
		return fabs(value - ((value_name##prop_cl*)prop)->value) < 0.001f;\
	};\
};\
value_name##prop_cl value_name##prop_inst;\
float value_name;

#endif


#ifdef EDITOR

class DEF_EXPORT StringPropWidget : public PropertyWidget
{
	static int used_id;
	int id;

public:

	StringPropWidget( BaseWidget* parent);
	char* value;

	LabelWidget*   prop_label;	
	TextBoxWidget* prop_ed;

	void InitControls();	
	void StartEdit(char* vl, const char* name);
	
	virtual void Show(bool show);
	virtual bool CheckChange();
};


#define STRING_PROP( class_name, value_name, def_val, category_name, prop_name )\
class value_name##prop_cl : public Property\
{\
public:\
	char def_value[512];\
	char value[512];\
	StringPropWidget* widgets;\
	virtual void Init(PropertyHolder* own)\
	{\
		owner = own;\
		strcpy(name, prop_name);\
		strcpy(def_value, def_val);\
		strcpy(value, def_val);\
		TieValues();\
		widgets = null;\
	}\
	virtual void TieValues()\
	{\
		strcpy(((class_name*)owner)->value_name, value);\
	}\
	virtual void TieValuesReversely()\
	{\
		strcpy(value, ((class_name*)owner)->value_name);\
	}\
	virtual void ReverseTieValues()\
	{\
		strcpy(value, ((class_name*)owner)->value_name);\
		if (widgets) widgets->StartEdit(value, name);\
	}\
	virtual void Load(IStreamLoader* loader)\
	{\
		loader->Read(#value_name, value, 512);\
		TieValues();\
	};\
	virtual void Save(IStreamSaver* saver)\
	{\
		saver->Write(#value_name, value);\
	};\
	virtual void Copy(Property* prop)\
	{\
		value_name##prop_cl* prp = (value_name##prop_cl*)prop;\
		strcpy(prp->value, value);\
		prp->TieValues();\
	};\
	virtual void CreateWidget(RolloutWidget* parent)\
	{\
		widgets = (StringPropWidget*)PropertyWidgetsHolder::GetWidget(owner->script_class_name, #value_name);\
		if (!widgets)\
		{\
			widgets = new StringPropWidget(parent);\
			PropertyWidgetsHolder::AddWidget(widgets, owner->script_class_name, #value_name);\
			widgets->InitControls();\
			widgets->SetSize(220, WidgetHeight());\
			parent->AddRolloutChild(category_name, widgets);\
		}\
	};\
	virtual bool UpdateWidget()\
	{\
		if (widgets) widgets->CheckChange();\
		return false;\
	};\
	virtual void SetSkipWidget(RolloutWidget* parent, bool set)\
	{\
		parent->SetSkipChild(widgets, set);\
	};\
	virtual void PrepareWidgets()\
	{\
		widgets->SetPropertyOwner(this);\
		TieValuesReversely();\
		widgets->StartEdit(value, name);\
	};\
	virtual bool IsEqual(Property* prop)\
	{\
		return strcmp(value, ((value_name##prop_cl*)prop)->value) == 0;\
	};\
};\
value_name##prop_cl value_name##prop_inst;\
char value_name##[512];

#else

#define STRING_PROP( class_name, value_name, def_val, category_name, prop_name )\
class value_name##prop_cl : public Property\
{\
public:\
	char def_value[512];\
	char value[512];\
	virtual void Init(PropertyHolder* own)\
	{\
		owner = own;\
		strcpy(name, prop_name);\
		strcpy(def_value, def_val );\
		strcpy(value, def_val);\
		TieValues();\
	}\
	virtual void TieValues()\
	{\
		strcpy(((class_name*)owner)->value_name, value);\
	}\
	virtual void Load(IStreamLoader* loader)\
	{\
		loader->Read(#value_name, value, 512);\
		TieValues();\
	};\
	virtual void Save(IStreamSaver* saver)\
	{\
		saver->Write(#value_name, value);\
	};\
	virtual void Copy(Property* prop)\
	{\
		value_name##prop_cl* prp = (value_name##prop_cl*)prop;\
		strcpy(prp->value, value);\
		prp->TieValues();\
	};\
	virtual bool IsEqual(Property* prop)\
	{\
		return strcmp(value, ((value_name##prop_cl*)prop)->value) == 0;\
	};\
};\
value_name##prop_cl value_name##prop_inst;\
char value_name[512];

#endif


#ifdef EDITOR

class DEF_EXPORT FileNameProp : public PropertyWidget
{
	static int used_id;
	int id;
	bool res_view_started;

public:

	char* file_name;
	char  res_type[16];

	FileNameProp( BaseWidget* parent);	

	LabelWidget*  prop_label;	
	ButtonWidget* prop_ed;
	ButtonWidget* del_ed;

	void InitControls(const char* tp);	
	void StartEdit(char* fl_name, const char* name);
	
	virtual void Show(bool show);
	virtual bool CheckChange();
};

#define FILENAME_PROP( class_name, value_name, def_val, category_name, prop_name, res_type )\
class value_name##prop_cl : public Property\
{\
public:\
	char def_value[512];\
	char value[512];\
	FileNameProp* widgets;\
	virtual void Init(PropertyHolder* own)\
	{\
		owner = own;\
		strcpy(name, prop_name);\
		strcpy(def_value, def_val);\
		strcpy(value, def_val);\
		TieValues();\
		widgets = false;\
	}\
	virtual void TieValues()\
	{\
		strcpy(((class_name*)owner)->value_name, value);\
	}\
	virtual void TieValuesReversely()\
	{\
		strcpy(value, ((class_name*)owner)->value_name);\
	}\
	virtual void ReverseTieValues()\
	{\
		strcpy(value, ((class_name*)owner)->value_name);\
		if (widgets) widgets->StartEdit(value, name);\
	}\
	virtual void Load(IStreamLoader* loader)\
	{\
		loader->Read(#value_name, value, 512);\
		TieValues();\
	};\
	virtual void Save(IStreamSaver* saver)\
	{\
		saver->Write(#value_name, value);\
	};\
	virtual void Copy(Property* prop)\
	{\
		value_name##prop_cl* prp = (value_name##prop_cl*)prop;\
		strcpy(prp->value, value);\
		prp->TieValues();\
	};\
	virtual void CreateWidget(RolloutWidget* parent)\
	{\
		widgets = (FileNameProp*)PropertyWidgetsHolder::GetWidget(owner->script_class_name, #value_name);\
		if (!widgets)\
		{\
			widgets = new FileNameProp(parent);\
			PropertyWidgetsHolder::AddWidget(widgets, owner->script_class_name, #value_name);\
			widgets->InitControls(res_type);\
			widgets->SetSize(220, WidgetHeight());\
			parent->AddRolloutChild(category_name, widgets);\
		}\
	};\
	virtual bool UpdateWidget()\
	{\
		if (widgets) widgets->CheckChange();\
		return false;\
	};\
	virtual void SetSkipWidget(RolloutWidget* parent, bool set)\
	{\
		parent->SetSkipChild(widgets, set);\
	};\
	virtual void PrepareWidgets()\
	{\
		widgets->SetPropertyOwner(this);\
		TieValuesReversely();\
		widgets->StartEdit(value, name);\
	};\
	virtual bool IsEqual(Property* prop)\
	{\
		return strcmp(value, ((value_name##prop_cl*)prop)->value) == 0;\
	};\
};\
	value_name##prop_cl value_name##prop_inst;\
	char value_name##[512];

#else

#define FILENAME_PROP( class_name, value_name, def_val, category_name, prop_name, res_type )\
class value_name##prop_cl : public Property\
{\
public:\
	char def_value[512];\
	char value[512];\
	virtual void Init(PropertyHolder* own)\
	{\
		owner = own;\
		strcpy(name, prop_name);\
		strcpy(def_value, def_val );\
		strcpy(value, def_val);\
		TieValues();\
	}\
	virtual void TieValues()\
	{\
		strcpy(((class_name*)owner)->value_name, value);\
	}\
	virtual void Load(IStreamLoader* loader)\
	{\
		loader->Read(#value_name, value, 512);\
		TieValues();\
	};\
	virtual void Save(IStreamSaver* saver)\
	{\
		saver->Write(#value_name, value);\
	};\
	virtual void Copy(Property* prop)\
	{\
		value_name##prop_cl* prp = (value_name##prop_cl*)prop;\
		strcpy(prp->value, value);\
		prp->TieValues();\
	};\
	virtual bool IsEqual(Property* prop)\
	{\
		return strcmp(value, ((value_name##prop_cl*)prop)->value) == 0;\
	};\
};\
	value_name##prop_cl value_name##prop_inst;\
	char value_name[512];

#endif

#ifdef EDITOR

class DEF_EXPORT ColorPropWidget : public PropertyWidget
{
	static int used_id;
	int id;

public:

	ColorPropWidget( BaseWidget* parent);
	Color* value;

	LabelWidget*   prop_label;	
	LabelWidget*   prop_ed;

	void InitControls();
	void StartEdit(Color* vl, const char* name);

	virtual void Show(bool show);
	virtual bool CheckChange();
};

#define COLOR_PROP( class_name, value_name, def_val, category_name, prop_name )\
class value_name##prop_cl : public Property\
{\
public:\
	Color def_value;\
	Color value;\
	ColorPropWidget* widgets;\
	virtual void Init(PropertyHolder* own)\
	{\
		owner = own;\
		strcpy(name, prop_name);\
		def_value = def_val;\
		value = def_val;\
		TieValues();\
		widgets = null;\
	}\
	virtual void TieValues()\
	{\
		((class_name*)owner)->value_name = value;\
	}\
	virtual void TieValuesReversely()\
	{\
		value = ((class_name*)owner)->value_name;\
	}\
	virtual void ReverseTieValues()\
	{\
		value = ((class_name*)owner)->value_name;\
		if (widgets) widgets->StartEdit(&value, name);\
	}\
	virtual void Load(IStreamLoader* loader)\
	{\
		if (loader->EnterBlock(#value_name))\
		{\
			loader->Read("r", value.r);\
			loader->Read("g", value.g);\
			loader->Read("b", value.b);\
			loader->LeaveBlock();\
		}\
		TieValues();\
	};\
	virtual void Save(IStreamSaver* saver)\
	{\
		saver->MarkBeginBlock(#value_name);\
		saver->Write("r", value.r);\
		saver->Write("g", value.g);\
		saver->Write("b", value.b);\
		saver->MarkEndBlock(#value_name);\
	};\
	virtual void Copy(Property* prop)\
	{\
		value_name##prop_cl* prp = (value_name##prop_cl*)prop;\
		prp->value = value;\
		prp->TieValues();\
	};\
	virtual void EmbedingScript()\
	{\
	};\
	virtual void CreateWidget(RolloutWidget* parent)\
	{\
		widgets = (ColorPropWidget*)PropertyWidgetsHolder::GetWidget(owner->script_class_name, #value_name);\
		if (!widgets)\
		{\
			widgets = new ColorPropWidget(parent);\
			PropertyWidgetsHolder::AddWidget(widgets, owner->script_class_name, #value_name);\
			widgets->InitControls();\
			widgets->SetSize(220, WidgetHeight());\
			parent->AddRolloutChild(category_name, widgets);\
		}\
	};\
	virtual bool UpdateWidget()\
	{\
		if (widgets) widgets->CheckChange();\
		return false;\
	};\
	virtual void SetSkipWidget(RolloutWidget* parent, bool set)\
	{\
		parent->SetSkipChild(widgets, set);\
	};\
	virtual void PrepareWidgets()\
	{\
		widgets->SetPropertyOwner(this);\
		TieValuesReversely();\
		widgets->StartEdit(&value, name);\
	};\
	virtual bool IsEqual(Property* prop)\
	{\
		return (value.r == ((value_name##prop_cl*)prop)->value.r) &&\
			   (value.g == ((value_name##prop_cl*)prop)->value.g) &&\
			   (value.b == ((value_name##prop_cl*)prop)->value.b);\
	};\
};\
value_name##prop_cl value_name##prop_inst;\
Color value_name;

#else

#define COLOR_PROP( class_name, value_name, def_val, category_name, prop_name )\
class value_name##prop_cl : public Property\
{\
public:\
	Color def_value;\
	Color value;\
	PropertyHolder* owner;\
	virtual void Init(PropertyHolder* own)\
	{\
		owner = own;\
		strcpy(name, prop_name);\
		def_value = def_val;\
		value = def_val;\
		TieValues();\
	}\
	virtual void TieValues()\
	{\
		((class_name*)owner)->value_name = value;\
	}\
	virtual void Load(IStreamLoader* loader)\
	{\
		if (loader->EnterBlock(#value_name))\
		{\
			loader->Read("r", value.r);\
			loader->Read("g", value.g);\
			loader->Read("b", value.b);\
			loader->LeaveBlock();\
		}\
		TieValues();\
	};\
	virtual void Save(IStreamSaver* saver)\
	{\
		saver->MarkBeginBlock(#value_name);\
		saver->Write("r", value.r);\
		saver->Write("g", value.g);\
		saver->Write("b", value.b);\
		saver->MarkEndBlock(#value_name);\
	};\
	virtual void Copy(Property* prop)\
	{\
		value_name##prop_cl* prp = (value_name##prop_cl*)prop;\
		prp->value = value;\
		prp->TieValues();\
	};\
	virtual bool IsEqual(Property* prop)\
	{\
		return (value.r == ((value_name##prop_cl*)prop)->value.r) &&\
			   (value.g == ((value_name##prop_cl*)prop)->value.g) &&\
			   (value.b == ((value_name##prop_cl*)prop)->value.b);\
	};\
};\
value_name##prop_cl value_name##prop_inst;\
Color value_name;

#endif


#ifdef EDITOR

class DEF_EXPORT FontPropWidget : public PropertyWidget
{
	static int used_id;
	int id;	

public:

	FontPropWidget(BaseWidget* parent);
	FontParams* value;

	LabelWidget*    prop_label_name;	
	ComboBoxWidget* prop_cb_name;

	LabelWidget*    prop_label_style;	
	ComboBoxWidget* prop_cb_style;	

	LabelWidget*    prop_label_height;	
	TextBoxWidget*  prop_ed_height;

	LabelWidget*    prop_label_outline;	
	TextBoxWidget*  prop_ed_outline;

	LabelWidget*    prop_label_gap;	
	TextBoxWidget*  prop_ed_gap;

	LabelWidget*    prop_label_color;	
	LabelWidget*    prop_ed_color;

	LabelWidget*    prop_label_use_shd;	
	CheckBoxWidget* prop_cb_use_shd;

	LabelWidget*    prop_label_shd_color;	
	LabelWidget*    prop_ed_shd_color;

	void InitControls();	
	void StartEdit(FontParams* vl, const char* name);
		
	void UpdateStyleWidget();

	virtual bool CheckChange();
};

#define FONT_PROP( class_name, value_name, enumerator_name, prop_name, category_name )\
class value_name##prop_fnt : public Property\
{\
public:\
	FontParams value;\
	FontPropWidget* widgets;\
	EnumPropEnumerator enumerator;\
	virtual void Init(PropertyHolder* own)\
	{\
		owner = own;\
		strcpy(name, prop_name);\
		TieValues();\
		widgets = null;\
		enumerator = enumerator_name;\
	}\
	virtual void TieValues()\
	{\
		value.Copy(((class_name*)owner)->value_name);\
	}\
	virtual void TieValuesReversely()\
	{\
		((class_name*)owner)->value_name.Copy(value);\
	}\
	virtual void ReverseTieValues()\
	{\
		((class_name*)owner)->value_name.Copy(value);\
	}\
	virtual void Load(IStreamLoader* loader)\
	{\
		if (loader->EnterBlock(#value_name))\
		{\
			loader->Read("name", value.name, 128);\
			loader->Read("bold", value.bold);\
			loader->Read("italic", value.italic);\
			loader->Read("height", value.height);\
			loader->Read("outline", value.outline);\
			loader->Read("gap", value.gap);\
			loader->Read("color_r", value.color.r);\
			loader->Read("color_g", value.color.g);\
			loader->Read("color_b", value.color.b);\
			loader->Read("use_shadow", value.use_shadow);\
			loader->Read("shd_color_r", value.shd_color.r);\
			loader->Read("shd_color_g", value.shd_color.g);\
			loader->Read("shd_color_b", value.shd_color.b);\
			loader->LeaveBlock();\
		}\
		TieValues();\
	};\
	virtual void Save(IStreamSaver* saver)\
	{\
		saver->MarkBeginBlock(#value_name);\
		saver->Write("name", value.name);\
		saver->Write("bold", value.bold);\
		saver->Write("italic", value.italic);\
		saver->Write("height", value.height);\
		saver->Write("outline", value.outline);\
		saver->Write("gap", value.gap);\
		saver->Write("color_r", value.color.r);\
		saver->Write("color_g", value.color.g);\
		saver->Write("color_b", value.color.b);\
		saver->Write("use_shadow", value.use_shadow);\
		saver->Write("shd_color_r", value.shd_color.r);\
		saver->Write("shd_color_g", value.shd_color.g);\
		saver->Write("shd_color_b", value.shd_color.b);\
		saver->MarkEndBlock(#value_name);\
	};\
	virtual void Copy(Property* prop)\
	{\
		value_name##prop_fnt* prp = (value_name##prop_fnt*)prop;\
		value.Copy(prp->value);\
		prp->TieValues();\
	};\
	virtual void CreateWidget(RolloutWidget* parent)\
	{\
		widgets = (FontPropWidget*)PropertyWidgetsHolder::GetWidget(owner->script_class_name, #value_name);\
		if (!widgets)\
		{\
			widgets = new FontPropWidget(parent);\
			PropertyWidgetsHolder::AddWidget(widgets, owner->script_class_name, #value_name);\
			widgets->InitControls();\
			widgets->SetSize(220, WidgetHeight());\
			if (enumerator)\
			{\
				enumerator(widgets->prop_cb_name);\
			}\
			parent->AddRolloutChild(category_name, widgets);\
		}\
	};\
	virtual int  WidgetHeight() { return 25 * 8; };\
	virtual bool UpdateWidget()\
	{\
		if (widgets) widgets->CheckChange();\
		return false;\
	};\
	virtual void SetSkipWidget(RolloutWidget* parent, bool set)\
	{\
		parent->SetSkipChild(widgets, set);\
	};\
	virtual void PrepareWidgets()\
	{\
		if (enumerator)\
		{\
			widgets->prop_cb_name->ClearList();\
			enumerator(widgets->prop_cb_name);\
		}\
		widgets->SetPropertyOwner(this);\
		widgets->StartEdit(&value, name);\
	};\
	virtual bool IsEqual(Property* prop)\
	{\
		return value.IsEqual(((value_name##prop_fnt*)prop)->value);\
	};\
};\
value_name##prop_fnt value_name##prop_inst;\
FontParams value_name;

#else

#define FONT_PROP( class_name, value_name, prop_name )\
class value_name##prop_fnt : public Property\
{\
public:\
	FontParams value;\
	virtual void Init(PropertyHolder* own)\
	{\
		owner = own;\
		strcpy(name, prop_name);\
		TieValues();\
	}\
	virtual void TieValues()\
	{\
		((class_name*)owner)->value_name = value;\
	}\
	virtual void Load(IStreamLoader* loader)\
	{\
		if (loader->EnterBlock(#value_name))\
		{\
			loader->Read("name", value.name, 128);\
			loader->Read("bold", value.bold);\
			loader->Read("italic", value.italic);\
			loader->Read("height", value.height);\
			loader->Read("outline", value.outline);\
			loader->Read("gap", value.gap);\
			loader->Read("color_r", value.color.r);\
			loader->Read("color_g", value.color.g);\
			loader->Read("color_b", value.color.b);\
			loader->Read("use_shadow", value.use_shadow);\
			loader->Read("shd_color_r", value.shd_color.r);\
			loader->Read("shd_color_g", value.shd_color.g);\
			loader->Read("shd_color_b", value.shd_color.b);\
			loader->LeaveBlock();\
		}\
		TieValues();\
	};\
	virtual void Save(IStreamSaver* saver)\
	{\
		saver->MarkBeginBlock(#value_name);\
		saver->Write("name", value.name);\
		saver->Write("bold", value.bold);\
		saver->Write("italic", value.italic);\
		saver->Write("height", value.height);\
		saver->Write("outline", value.outline);\
		saver->Write("gap", value.gap);\
		saver->Write("color_r", value.color.r);\
		saver->Write("color_g", value.color.g);\
		saver->Write("color_b", value.color.b);\
		saver->Write("use_shadow", value.use_shadow);\
		saver->Write("shd_color_r", value.shd_color.r);\
		saver->Write("shd_color_g", value.shd_color.g);\
		saver->Write("shd_color_b", value.shd_color.b);\
		saver->MarkEndBlock(#value_name);\
	};\
	virtual void Copy(Property* prop)\
	{\
		value_name##prop_fnt* prp = (value_name##prop_fnt*)prop;\
		prp->value = value;\
		prp->TieValues();\
	};\
	virtual bool IsEqual(Property* prop)\
	{\
		return value.IsEqual(((value_name##prop_fnt*)prop)->value);\
	};\
};\
value_name##prop_fnt value_name##prop_inst;\
FontParams value_name;

#endif

typedef void (*CallbackProp)(PropertyHolder* owner);

#ifdef EDITOR

class DEF_EXPORT CallbackPropWgt : public PropertyWidget
{
	static int used_id;
	int id;	

public:
	
	CallbackProp callback;

	CallbackPropWgt( BaseWidget* parent);
	
	ButtonWidget* prop_ed;	

	void InitControls(const char* name);	
	virtual void Show(bool show);	
	void CheckClick();
};

#define CALLBACK_PROP( class_name, value_name, set_callback, category_name, prop_name)\
class value_name##prop_cl : public Property\
{\
public:\
	CallbackPropWgt* widgets;\
	virtual void Init(PropertyHolder* own)\
	{\
		owner = own;\
		widgets = null;\
	}\
	virtual void TieValues()\
	{\
	}\
	virtual void CreateWidget(RolloutWidget* parent)\
	{\
		widgets = (CallbackPropWgt*)PropertyWidgetsHolder::GetWidget(owner->script_class_name, #value_name);\
		if (!widgets)\
		{\
			widgets = new CallbackPropWgt(parent);\
			PropertyWidgetsHolder::AddWidget(widgets, owner->script_class_name, #value_name);\
			widgets->InitControls(prop_name);\
			widgets->callback = set_callback;\
			widgets->SetSize(220, WidgetHeight());\
			parent->AddRolloutChild(category_name, widgets);\
		}\
	};\
	virtual bool UpdateWidget()\
	{\
		widgets->CheckClick();\
		return false;\
	};\
	virtual void SetSkipWidget(RolloutWidget* parent, bool set)\
	{\
		parent->SetSkipChild(widgets, set);\
	};\
	virtual void PrepareWidgets()\
	{\
		widgets->SetPropertyOwner(this);\
	};\
};\
value_name##prop_cl value_name##prop_inst;

#endif


#define PROP_REG( value_name )\
	value_name##prop_inst.Init(this);\
	prop.push_back(&value_name##prop_inst);


#define SET_SKIP( value_name, set )\
	value_name##prop_inst.SetSkipWidget(rollout, set);

