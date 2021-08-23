
#pragma once

#include "Common/Common.h"

class UserStat
{		
public:
	enum StatType
	{
		vl_float = 0,
		vl_int = 1,
		vl_string = 2,		
		vl_vector = 4,
		vl_quantarion = 5
	};	

private:			

	StatType type;

public:

	UserStat(StatType tp);
	virtual ~UserStat() {};
	virtual StatType GetType();
	virtual void SetDefault();
};

class DEF_EXPORT UserFloatStat : public UserStat
{		
	float value;
	float def_value;

public:	
	
	UserFloatStat(StatType tp, float def_value);	

	virtual ~UserFloatStat() {};

	void  Set(float val);
	float Get();
	float GetDefault();
	virtual void SetDefault();
};

class DEF_EXPORT UserIntStat : public UserStat
{		
	int value;
	int def_value;

public:	
	
	UserIntStat(StatType tp, int def_value);	

	virtual ~UserIntStat() {};

	void Set(int val);
	int Get();
	int GetDefault();
	virtual void SetDefault();
};

class UserStringStat : public UserStat
{		
	string value;
	string def_value;

public:	

	UserStringStat(StatType tp, const char* def_value);		

	virtual ~UserStringStat() {};

	void  Set(const char* val);
	const char* Get();
	const char* GetDefault();
	virtual void SetDefault();
};

class DEF_EXPORT UserVectorStat : public UserStat
{		
	Vector value;
	Vector def_value;

public:
	
	UserVectorStat(StatType tp, Vector& def_value);		

	virtual ~UserVectorStat() {};

	void  Set(Vector& val);
	Vector Get();
	Vector GetDefault();
	virtual void SetDefault();
};

class DEF_EXPORT UserQuaternionStat : public UserStat
{		
	Quaternion value;
	Quaternion def_value;

public:	

	UserQuaternionStat(StatType tp, Quaternion& def_value);	

	~UserQuaternionStat() {};

	void  Set(Quaternion& val);
	Quaternion Get();
	Quaternion GetDefault();
	virtual void SetDefault();
};