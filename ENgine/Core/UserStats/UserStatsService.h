
#pragma once

#include "UserStats.h"

class IFile;
class IFileBuffer;

class UserStatsService : public IUserStatsService
{
public:

private:
		
	byte buffer[16 * 1024];

	char fileName[1024];

	struct UserStatHolder
	{
		int hash1;
		int hash2;
		UserStat* value;
	};

	dword MagicID;
	dword SubversionID;

	vector<UserStatHolder> values;

	class ThreadSaver : public ThreadWorker
	{
	public:

		char prefix[32];
		UserStatsService* owner;
		virtual void Prepare();	
		virtual void Loop();		
	};
			
	ThreadSaver saver;

	int ld_str_len;
	char* ld_str;

	int ld_def_str_len;
	char* ld_def_str;	

	int ld_wstr_len;
	wchar_t* ld_wstr;

	int ld_def_wstr_len;
	wchar_t* ld_def_wstr;	

	char name_space[256];

	UserFloatStat*      GetFloatValue(const char* id, float def_value);
	UserIntStat*		GetIntValue(const char* id, int def_value);
	UserStringStat*     GetStringValue(const char* id, const char* def_value);	
	UserVectorStat*     GetVectorValue(const char* id, Vector& def_value);
	UserQuaternionStat* GetQuaternionValue(const char* id, Quaternion& def_value);	

	UserFloatStat*      GetFloatValue(int hash1, int hash2, float def_value);
	UserIntStat*		GetIntValue(int hash1, int hash2, int def_value);
	UserStringStat*     GetStringValue(int hash1, int hash2, const char* def_value);	
	UserVectorStat*     GetVectorValue(int hash1, int hash2, Vector& def_value);
	UserQuaternionStat* GetQuaternionValue(int hash1, int hash2, Quaternion& def_value);	

public :
					
	UserStatsService();

	virtual bool Init();	
	
	virtual void SetFloat(const char* id, float def_value, float value, bool add);	
	virtual void SetInt(const char* id, int def_value, int value, bool add);
	virtual void SetString(const char* id, const char* def_value, const char* value);	
	virtual void SetVector(const char* id, Vector& def_value, Vector& value);
	virtual void SetQuaternion(const char* id, Quaternion& def_value, Quaternion& value);

	virtual float		   GetFloat(const char* id, float def_value);
	virtual int		       GetInt(const char* id, int def_value);
	virtual const char*    GetString(const char* id, const char* def_value);	
	virtual Vector		   GetVector(const char* id, Vector& def_value);
	virtual Quaternion     GetQuaternion(const char* id, Quaternion& def_value);
	
	virtual void SetFloat(int hash1, int hash2, float def_value, float value, bool add);	
	virtual void SetInt(int hash1, int hash2, int def_value, int value, bool add);
	virtual void SetString(int hash1, int hash2, const char* def_value, const char* value);		
	virtual void SetVector(int hash1, int hash2, Vector& def_value, Vector& value);
	virtual void SetQuaternion(int hash1, int hash2, Quaternion& def_value, Quaternion& value);

	virtual float		   GetFloat(int hash1, int hash2, float def_value);
	virtual int		       GetInt(int hash1, int hash2, int def_value);
	virtual const char*    GetString(int hash1, int hash2, const char* def_value);	
	virtual Vector		   GetVector(int hash1, int hash2, Vector& def_value);
	virtual Quaternion     GetQuaternion(int hash1, int hash2, Quaternion& def_value);
	
	virtual void Reset();

	virtual bool Load();
	void Load(IFileBuffer* file);	

	virtual void Save();
	void Save(IFile* file);	
	
	void Update(float dt);
	
	virtual void GetHashes(const char* id, int& hash1, int& hash2);
	virtual void Release();

	virtual void SetNamespace(const char* ns);

protected:
	UserStat* GetValue(const char* id);
	UserStat* GetValue(int hash1, int hash2);
	const char* GetProfileFileName();
};