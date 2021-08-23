
#include "UserStats.h"
#include "Core/Core.h"

UserStat::UserStat(StatType tp)
{		
	type = tp;
}

UserStat::StatType UserStat::GetType()
{
	return type;
}

void UserStat::SetDefault()
{

}

UserFloatStat::UserFloatStat(StatType tp, float df_value):UserStat(tp)
{
	value = df_value;	
	def_value = df_value;
}

void UserFloatStat::Set(float val)
{
	value = val;
}

float UserFloatStat::Get()
{
	return value;
}

float UserFloatStat::GetDefault()
{
	return def_value;
}

void UserFloatStat::SetDefault()
{
	value = def_value;
}

UserIntStat::UserIntStat(StatType tp, int df_value):UserStat(tp)
{
	value = df_value;	
	def_value = df_value;
}

void UserIntStat::Set(int val)
{
	value = val;
}

int UserIntStat::Get()
{
	return value;
}

int UserIntStat::GetDefault()
{
	return def_value;
}

void UserIntStat::SetDefault()
{
	value = def_value;
}

UserStringStat::UserStringStat(StatType tp, const char* df_value):UserStat(tp)
{	
	value.assign(df_value);
	def_value.assign(df_value);
}

void UserStringStat::Set(const char* val)
{		
	value.assign(val);		
}

const char* UserStringStat::Get()
{
	return value.c_str();
}

const char* UserStringStat::GetDefault()
{
	return def_value.c_str();
}

void UserStringStat::SetDefault()
{	
	value.assign(def_value);
}

UserVectorStat::UserVectorStat(StatType tp, Vector& df_value):UserStat(tp)
{
	value = df_value;
	def_value = df_value;
}

void UserVectorStat::Set(Vector& val)
{
	value = val;
}

Vector UserVectorStat::Get()
{
	return value;
}

Vector UserVectorStat::GetDefault()
{
	return def_value;
}

void UserVectorStat::SetDefault()
{
	value = def_value;
}

UserQuaternionStat::UserQuaternionStat(StatType tp, Quaternion& df_value):UserStat(tp)
{
	value = df_value;
	def_value = df_value;
}

void UserQuaternionStat::Set(Quaternion& val)
{
	value = val;
}

Quaternion UserQuaternionStat::Get()
{
	return value;
}

Quaternion UserQuaternionStat::GetDefault()
{
	return def_value;
}

void UserQuaternionStat::SetDefault()
{
	value = def_value;
}