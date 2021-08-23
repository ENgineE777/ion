#pragma once

#include "Utils/Common.h"

class IPropertyHolder;

class IProperty
{
	public:

	virtual ~IProperty() {};
	virtual void Init(IPropertyHolder* own) = 0;		

	virtual void TieValues() = 0;
	virtual float GetAsFloat() = 0;
	virtual void SetAsFloat(float val) = 0;
	virtual const char* GetAsString() = 0;
	virtual void  SetAsString(const char* val) = 0;

	virtual const char* GetType() = 0;
	virtual const char* GetName() = 0;
	//virtual void LoadValue() = 0;
	//virtual void SaveValue() = 0;
};

class IPropertyHolder
{
public:

	std::vector<IProperty*> prop;

	virtual ~IPropertyHolder() {};
	virtual void Init()
	{
	};

	virtual void ReInit()
	{
	};

	int GetPropCount()
	{
		return (int)prop.size();
	}

	int GetPropIndex(const char* name)
	{
		for (int i=0;i<(int)prop.size();i++)
		{
			if (strcmp(prop[i]->GetName(), name) == 0)
			{
				return i;
			}
		}

		return -1;
	}

	const char* GetPropName(int index)
	{
		return prop[index]->GetName();
	}

	const char* GetPropType(int index)
	{
		return prop[index]->GetType();
	}

	float GetPropAsFloat(int index)
	{
		return prop[index]->GetAsFloat();
	}

	void SetPropAsFloat(int index, float val)
	{
		prop[index]->SetAsFloat(val);
	}

	const char* GetPropAsString(int index)
	{
		return prop[index]->GetAsString();
	}

	void SetPropAsString(int index, const char* val)
	{
		prop[index]->SetAsString(val);
	}
};

template<class T>
class PropertyTemplate : public IProperty
{
public:

	T def_value;
	T value;
	char name[32];

	IPropertyHolder* owner;	

	virtual void Init(IPropertyHolder* own) = 0;

	virtual void TieValues() = 0;
	virtual float GetAsFloat() { return 0.0f; };
	virtual void  SetAsFloat(float val) {};
	virtual const char* GetAsString() { return ""; };
	virtual void  SetAsString(const char* val) {};
	virtual const char* GetType() = 0;
	virtual const char* GetName() { return name; };
};

#define AF_FLOAT_PROP( value_name, class_name, def_val, prop_name )\
class value_name##prop_cl : public PropertyTemplate<float>\
{\
public:\
	virtual void Init(IPropertyHolder* own)\
{\
	owner = own;\
	strcpy(name, prop_name);\
	def_value = def_val;\
	value = def_val;\
	TieValues();\
}\
	virtual void TieValues()\
{\
	((##class_name##*)owner)->##value_name = value;\
}\
	virtual float GetAsFloat()\
{\
	return value;\
};\
	virtual void SetAsFloat(float val)\
{\
	value = val;\
	TieValues();\
	owner->ReInit();\
};\
	virtual const char* GetType() { return "float"; };\
};\
	value_name##prop_cl value_name##prop_inst;\
	float value_name;



#define AF_BONENAME_PROP( value_name, class_name, def_val, prop_name )\
class value_name##prop_cl : public PropertyTemplate<std::string>\
{\
public:\
	virtual void Init(IPropertyHolder* own)\
{\
	owner = own;\
	strcpy(name, prop_name);\
	def_value = def_val;\
	value = def_val;\
	TieValues();\
}\
	virtual void TieValues()\
{\
	((##class_name##*)owner)->##value_name = value;\
}\
	virtual const char* GetAsString()\
{\
	return value.c_str();\
};\
	virtual void  SetAsString(const char* val)\
{\
	value = val;\
	TieValues();\
	owner->ReInit();\
};\
	virtual const char* GetType() { return "bonename"; };\
};\
	value_name##prop_cl value_name##prop_inst;\
	std::string value_name;



#define PROP_REG( value_name )\
	value_name##prop_inst.Init(this);\
	prop.push_back(&value_name##prop_inst);