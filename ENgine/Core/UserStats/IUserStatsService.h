
#pragma once

class IUserStatsService
{
public :

	virtual bool Init() = 0;
	
	virtual void SetFloat(const char* id, float def_value, float value, bool add) = 0;
	virtual void SetInt(const char* id, int def_value, int value, bool add) = 0;
	virtual void SetString(const char* id, const char* def_value, const char* value) = 0;	
	virtual void SetVector(const char* id, Vector& def_value, Vector& value) = 0;
	virtual void SetQuaternion(const char* id, Quaternion& def_value, Quaternion& value) = 0;

	virtual float		   GetFloat(const char* id, float def_value) = 0;
	virtual int		       GetInt(const char* id, int def_value) = 0;
	virtual const char*    GetString(const char* id, const char* def_value) = 0;	
	virtual Vector		   GetVector(const char* id, Vector& def_value) = 0;
	virtual Quaternion     GetQuaternion(const char* id, Quaternion& def_value) = 0;
	
	virtual void SetFloat(int hash1, int hash2, float def_value, float value, bool add) = 0;	
	virtual void SetInt(int hash1, int hash2, int def_value, int value, bool add) = 0;
	virtual void SetString(int hash1, int hash2, const char* def_value, const char* value) = 0;		
	virtual void SetVector(int hash1, int hash2, Vector& def_value, Vector& value) = 0;
	virtual void SetQuaternion(int hash1, int hash2, Quaternion& def_value, Quaternion& value) = 0;

	virtual float		   GetFloat(int hash1, int hash2, float def_value) = 0;
	virtual int		       GetInt(int hash1, int hash2, int def_value) = 0;
	virtual const char*    GetString(int hash1, int hash2, const char* def_value) = 0;	
	virtual Vector		   GetVector(int hash1, int hash2, Vector& def_value) = 0;
	virtual Quaternion     GetQuaternion(int hash1, int hash2, Quaternion& def_value) = 0;
		
	virtual void Reset() = 0;

	virtual bool Load() = 0;
	virtual void Save() = 0;
	virtual void GetHashes(const char* id, int& hash1, int& hash2) = 0;

	virtual void SetNamespace(const char* ns) = 0;

	virtual void Release() = 0;
};