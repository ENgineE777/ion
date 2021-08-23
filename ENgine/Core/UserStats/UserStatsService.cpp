#include "Core/Core.h"
#include "Core/Files/File.h"

void UserStatsService::ThreadSaver::Prepare()
{

}

void UserStatsService::ThreadSaver::Loop()
{			
	core->TraceTo("UserStats", "Start ThreadSaver::Loop()");

	IFile* file = core->Files()->OpenFile(owner->GetProfileFileName(), IFile::write);			

	if (file)
	{
		core->TraceTo("UserStats", "saving profile %s with prefix %s", owner->GetProfileFileName(), prefix);

		owner->Save(file);		

		file->Release();
	}
}

UserStatsService::UserStatsService()
{	
	MagicID = MAKEFOURCC('E', 'N', 'P', 'S');	
	SubversionID = MAKEFOURCC('V', 'N', '0', '4');

	fileName[0] = 0;

#ifdef PC
	String::Copy(fileName, 1024, "UserStats/user.stat");	
#endif

#ifdef ANDROID
	String::Copy(fileName, 1024, "user.stat");
#endif
	
#ifdef IOS
	String::Copy(fileName, 1024, "user.stat");
#endif

	ld_str_len = 128;
	ld_str = new char[ld_str_len];

	ld_def_str_len = 128;
	ld_def_str = new char[ld_def_str_len];

	ld_wstr_len = 128;
	ld_wstr = new wchar_t[ld_wstr_len];	

	ld_def_wstr_len = 128;
	ld_def_wstr = new wchar_t[ld_def_wstr_len];	

	name_space[0] = 0;
}

bool UserStatsService::Init()
{		
	return true;
}

UserFloatStat* UserStatsService::GetFloatValue(const char* id, float def_value)
{
	int hash1;
	int hash2;

	GetHashes(id, hash1, hash2);	

	return GetFloatValue(hash1, hash2, def_value);
}

UserIntStat* UserStatsService::GetIntValue(const char* id, int def_value)
{
	int hash1;
	int hash2;

	GetHashes(id, hash1, hash2);	

	return GetIntValue(hash1, hash2, def_value);
}


UserStringStat* UserStatsService::GetStringValue(const char* id, const char* def_value)
{
	int hash1;
	int hash2;

	GetHashes(id, hash1, hash2);	

	return GetStringValue(hash1, hash2, def_value);
}

UserVectorStat* UserStatsService::GetVectorValue(const char* id, Vector& def_value)
{
	int hash1;
	int hash2;

	GetHashes(id, hash1, hash2);	

	return GetVectorValue(hash1, hash2, def_value);
}

UserQuaternionStat* UserStatsService::GetQuaternionValue(const char* id, Quaternion& def_value)
{
	int hash1;
	int hash2;

	GetHashes(id, hash1, hash2);	

	return GetQuaternionValue(hash1, hash2, def_value);
}

UserFloatStat* UserStatsService::GetFloatValue(int hash1, int hash2, float def_value)
{
	UserStat* value = GetValue(hash1, hash2);

	if (value)
	{
		if (value->GetType() == UserStat::vl_float)
		{
			return (UserFloatStat*)value;
		}

		return null;
	}

	UserFloatStat* flt_value = new UserFloatStat(UserStat::vl_float, def_value);
	
	UserStatHolder holder;
	holder.hash1 = hash1;
	holder.hash2 = hash2;
	holder.value = flt_value;

	values.push_back(holder);	

	return flt_value;
}

UserIntStat* UserStatsService::GetIntValue(int hash1, int hash2, int def_value)
{
	UserStat* value = GetValue(hash1, hash2);

	if (value)
	{
		if (value->GetType() == UserStat::vl_int)
		{
			return (UserIntStat*)value;
		}

		return NULL;
	}

	UserIntStat* int_value = new UserIntStat(UserStat::vl_int, def_value);
	
	UserStatHolder holder;
	holder.hash1 = hash1;
	holder.hash2 = hash2;
	holder.value = int_value;

	values.push_back(holder);	

	return int_value;
}


UserStringStat* UserStatsService::GetStringValue(int hash1, int hash2, const char* def_value)
{
	UserStat* value = GetValue(hash1, hash2);

	if (value)
	{
		if (value->GetType() == UserStat::vl_string)
		{
			return (UserStringStat*)value;
		}

		return NULL;
	}

	UserStringStat* str_value = new UserStringStat(UserStat::vl_string, def_value);
	
	UserStatHolder holder;
	holder.hash1 = hash1;
	holder.hash2 = hash2;
	holder.value = str_value;

	values.push_back(holder);	

	return str_value;
}

UserVectorStat* UserStatsService::GetVectorValue(int hash1, int hash2, Vector& def_value)
{
	UserStat* value = GetValue(hash1, hash2);

	if (value)
	{
		if (value->GetType() == UserStat::vl_vector)
		{
			return (UserVectorStat*)value;
		}

		return NULL;
	}

	UserVectorStat* v_value = new UserVectorStat(UserStat::vl_vector, def_value);
	
	UserStatHolder holder;
	holder.hash1 = hash1;
	holder.hash2 = hash2;
	holder.value = v_value;

	values.push_back(holder);	

	return v_value;
}

UserQuaternionStat* UserStatsService::GetQuaternionValue(int hash1, int hash2, Quaternion& def_value)
{
	UserStat* value = GetValue(hash1, hash2);

	if (value)
	{
		if (value->GetType() == UserStat::vl_quantarion)
		{
			return (UserQuaternionStat*)value;
		}

		return NULL;
	}

	UserQuaternionStat* q_value = new UserQuaternionStat(UserStat::vl_quantarion, def_value);

	UserStatHolder holder;
	holder.hash1 = hash1;
	holder.hash2 = hash2;
	holder.value = q_value;

	values.push_back(holder);	

	return q_value;
}

void UserStatsService::SetFloat(const char* id, float def_value, float value, bool add)
{
	int hash1;
	int hash2;

	GetHashes(id, hash1, hash2);	

	SetFloat(hash1, hash2, def_value, value, add);
}

void UserStatsService::SetInt(const char* id, int def_value, int value, bool add)
{
	int hash1;
	int hash2;

	GetHashes(id, hash1, hash2);	

	SetInt(hash1, hash2, def_value, value, add);
}

void UserStatsService::SetString(const char* id, const char* def_value, const char* value)
{
	int hash1;
	int hash2;

	GetHashes(id, hash1, hash2);	

	SetString(hash1, hash2, def_value, value);
}

void UserStatsService::SetVector(const char* id, Vector& def_value, Vector& value)
{
	int hash1;
	int hash2;

	GetHashes(id, hash1, hash2);	

	SetVector(hash1, hash2, def_value, value);
}

void UserStatsService::SetQuaternion(const char* id, Quaternion& def_value, Quaternion& value)
{
	int hash1;
	int hash2;

	GetHashes(id, hash1, hash2);	

	SetQuaternion(hash1, hash2, def_value, value);
}

float UserStatsService::GetFloat(const char* id, float def_value)
{
	int hash1;
	int hash2;

	GetHashes(id, hash1, hash2);	

	return GetFloat(hash1, hash2, def_value);
}

int UserStatsService::GetInt(const char* id, int def_value)
{
	int hash1;
	int hash2;

	GetHashes(id, hash1, hash2);	

	return GetInt(hash1, hash2, def_value);
}

const char* UserStatsService::GetString(const char* id, const char* def_value)
{
	int hash1;
	int hash2;

	GetHashes(id, hash1, hash2);	

	return GetString(hash1, hash2, def_value);
}

Vector UserStatsService::GetVector(const char* id, Vector& def_value)
{
	int hash1;
	int hash2;

	GetHashes(id, hash1, hash2);	

	return GetVector(hash1, hash2, def_value);
}

Quaternion UserStatsService::GetQuaternion(const char* id, Quaternion& def_value)
{
	int hash1;
	int hash2;

	GetHashes(id, hash1, hash2);	

	return GetQuaternion(hash1, hash2, def_value);	
}

void UserStatsService::SetFloat(int hash1, int hash2, float def_value, float value, bool add)
{
	UserFloatStat* prf_value = GetFloatValue(hash1, hash2, def_value);

	if (prf_value)
	{
		if (add)
		{
			prf_value->Set(value + prf_value->Get());
		}
		else
		{
			prf_value->Set(value);
		}
	}
}

void UserStatsService::SetInt(int hash1, int hash2, int def_value, int value, bool add)
{
	UserIntStat* prf_value = GetIntValue(hash1, hash2, def_value);

	if (prf_value)
	{
		if (add)
		{
			prf_value->Set(value + prf_value->Get());
		}
		else
		{
			prf_value->Set(value);
		}
	}
}

void UserStatsService::SetString(int hash1, int hash2, const char* def_value, const char* value)
{
	UserStringStat* prf_value = GetStringValue(hash1, hash2, def_value);		

	if (prf_value)
	{
		prf_value->Set(value);
	}	
}

void UserStatsService::SetVector(int hash1, int hash2, Vector& def_value, Vector& value)
{
	UserVectorStat* prf_value = GetVectorValue(hash1, hash2, def_value);

	if (prf_value)
	{
		prf_value->Set(value);
	}	
}

void UserStatsService::SetQuaternion(int hash1, int hash2, Quaternion& def_value, Quaternion& value)
{
	UserQuaternionStat* prf_value = GetQuaternionValue(hash1, hash2, def_value);

	if (prf_value)
	{
		prf_value->Set(value);
	}
}

float UserStatsService::GetFloat(int hash1, int hash2, float def_value)
{
	UserFloatStat* prf_value = GetFloatValue(hash1, hash2, def_value);

	if (prf_value)
	{
		return prf_value->Get();
	}

	return def_value;
}

int UserStatsService::GetInt(int hash1, int hash2, int def_value)
{
	UserIntStat* prf_value = GetIntValue(hash1, hash2, def_value);

	if (prf_value)
	{
		return prf_value->Get();
	}

	return def_value;
}

const char* UserStatsService::GetString(int hash1, int hash2, const char* def_value)
{
	UserStringStat* prf_value = GetStringValue(hash1, hash2, def_value);

	if (prf_value)
	{
		return prf_value->Get();
	}

	return def_value;
}

Vector UserStatsService::GetVector(int hash1, int hash2, Vector& def_value)
{
	UserVectorStat* prf_value = GetVectorValue(hash1, hash2, def_value);

	if (prf_value)
	{
		return prf_value->Get();
	}

	return def_value;
}

Quaternion UserStatsService::GetQuaternion(int hash1, int hash2, Quaternion& def_value)
{
	UserQuaternionStat* prf_value = GetQuaternionValue(hash1, hash2, def_value);

	if (prf_value)
	{
		return prf_value->Get();
	}

	return def_value;
}

const char* UserStatsService::GetProfileFileName()
{
	static char name[1024];

	String::Copy(name, 1024, fileName);	

	String::Cat(name, 1024, "_");	
	String::Cat(name, 1024, name_space);	
		
	return name;
}

void UserStatsService::Reset()
{	
	for (int i=0; i<values.size(); i++)
	{
		delete values[i].value;
	}

	values.clear();
}

bool UserStatsService::Load()
{    
	Reset();
	
#ifdef PC	
	char str_path[512];
	String::Printf(str_path, 512, "%s//UserStats", en_core.EN_Files()->GetAppDirectory());
	CreateDirectory(str_path, null);
#endif

	IFileBuffer* file = core->Files()->LoadFile(GetProfileFileName());		

	if (file)
	{			
		bool res = (file->GetSize() > 0);
		
		if (res)
		{
			Load(file);			
		}

		file->Release();		

		return res;
	}
	
	return false;	
}

void UserStatsService::Load(IFileBuffer* file)
{
	byte* ptr = file->GetBuffer();

	dword id = *((dword*)ptr);
	ptr += sizeof(dword);

	dword subid = *((dword*)ptr);
	ptr += sizeof(dword);

	dword d = MagicID;
	dword sd = SubversionID;

	if (id != MagicID || subid != SubversionID) return;	    
    
	int num = *((int*)ptr);	
	ptr += sizeof(int);

	for (int i=0; i<num; i++)
	{
		int hash1 = *((int*)ptr);
		ptr += sizeof(int);	

		int hash2 = *((int*)ptr);
		ptr += sizeof(int);			

		int type = *((int*)ptr);
		ptr += sizeof(int);		
		
		if (type == UserStat::vl_float)
		{			
			float val = *((float*)ptr);			
			ptr += sizeof(float);

			float def_val = *((float*)ptr);			
			ptr += sizeof(float);

			UserFloatStat* value = null;
			UserStat* vl = GetValue(hash1, hash2);

			if (!vl)
			{
				value = new UserFloatStat(UserStat::vl_float, def_val);
				
				UserStatHolder holder;
				holder.hash1 = hash1;
				holder.hash2 = hash2;
				holder.value = value;

				values.push_back(holder);
			}
			else
			{
				value = (UserFloatStat*)vl;
			}

			value->Set(val);			
		}
		else
		if (type == UserStat::vl_int)
		{			
			int val = *((int*)ptr);			
			ptr += sizeof(int);

			int def_val = *((int*)ptr);			
			ptr += sizeof(int);			

			UserIntStat* value = null;
			UserStat* vl = GetValue(hash1, hash2);

			if (!vl)
			{
				value = new UserIntStat(UserStat::vl_int, def_val);
				
				UserStatHolder holder;
				holder.hash1 = hash1;
				holder.hash2 = hash2;
				holder.value = value;

				values.push_back(holder);
			}
			else
			{
				value = (UserIntStat*)vl;
			}

			value->Set(val);	
		}
		else
		if (type == UserStat::vl_string)
		{			
			int len = *((int*)ptr);			
			ptr += sizeof(int);
			
			if (ld_str_len < len + 2)
			{
				delete[] ld_str;

				ld_str_len = len + 128;
				ld_str = new char[ld_str_len];
			}

			memcpy(ld_str, ptr, len);			
			ld_str[len] = 0;
			ptr += len;	

			int ost = ((int)((float)(len)/4.0f) + 1)* 4 - len;
			if (ost == 4) ost = 0;

			ptr += ost;			
			
			len = *((int*)ptr);			
			ptr += sizeof(int);
			
			if (ld_def_str_len < len + 2)
			{
				delete[] ld_def_str;

				ld_def_str_len = len + 128;
				ld_def_str = new char[ld_def_str_len];
			}		

			memcpy(ld_def_str, ptr, len);
			ld_def_str[len] = 0;
			ptr += len;			

			ost = ((int)((float)(len)/4.0f) + 1)* 4 - len;
			if (ost == 4) ost = 0;

			ptr += ost;	

			UserStringStat* value = null;
			UserStat* vl = GetValue(hash1, hash2);

			if (!vl)
			{
				value = new UserStringStat(UserStat::vl_string, ld_def_str);
				
				UserStatHolder holder;
				holder.hash1 = hash1;
				holder.hash2 = hash2;
				holder.value = value;

				values.push_back(holder);
			}
			else
			{
				value = (UserStringStat*)vl;
			}

			value->Set(ld_str);
		}
		else		
		if (type == UserStat::vl_vector)
		{
			Vector val = *((Vector*)ptr);			
			ptr += sizeof(Vector);

			Vector def_val = *((Vector*)ptr);			
			ptr += sizeof(Vector);

			UserVectorStat* value = null;
			UserStat* vl = GetValue(hash1, hash2);

			if (!vl)
			{
				value = new UserVectorStat(UserStat::vl_vector, def_val);
				
				UserStatHolder holder;
				holder.hash1 = hash1;
				holder.hash2 = hash2;
				holder.value = value;

				values.push_back(holder);
			}
			else
			{
				value = (UserVectorStat*)vl;
			}

			value->Set(val);	
		}
		else
		if (type == UserStat::vl_quantarion)
		{
			Quaternion val = *((Quaternion*)ptr);			
			ptr += sizeof(Quaternion);

            Quaternion def_val = *((Quaternion*)ptr);
			ptr += sizeof(Quaternion);

			UserQuaternionStat* value = null;
			UserStat* vl = GetValue(hash1, hash2);

			if (!vl)
			{
				value = new UserQuaternionStat(UserStat::vl_quantarion, def_val);

				UserStatHolder holder;
				holder.hash1 = hash1;
				holder.hash2 = hash2;
				holder.value = value;

				values.push_back(holder);
			}
			else
			{
				value = (UserQuaternionStat*)vl;
			}

			value->Set(val);
		}
	}	
}

void UserStatsService::Save()
{
	IFile* file = core->Files()->OpenFile(GetProfileFileName(), IFile::write);			

	if (file)
	{
		Save(file);

		file->Release();
	}

	/*core->Trace("saving prefix %s", prefix);

	//SetInt("FirstStart", true, false, false);
	
	String::Copy(saver.prefix, 32, prefix);	
	saver.Start(ThreadWorker::normal);*/
}

void UserStatsService::Save(IFile* file)
{		
	int len = 0;	
	byte* ptr = buffer;	

	*((dword*)ptr) = MagicID;
	ptr += sizeof(dword);
	
	len += sizeof(dword);

	*((dword*)ptr) = SubversionID;	
	ptr += sizeof(dword);
	
	len += sizeof(dword);    
    
    int num = values.size();

	*((int*)ptr) = num;			
	ptr += sizeof(int);	

	len += sizeof(int);

    
	for (int i=0; i<values.size(); i++)
	{
		UserStat* val = values[i].value;

		*((int*)ptr) = values[i].hash1;
		ptr += sizeof(int);

		*((int*)ptr) = values[i].hash2;
		ptr += sizeof(int);			

		*((int*)ptr) = val->GetType();
		ptr += sizeof(int);	

		len += sizeof(int) * 3;

		if (val->GetType() == UserStat::vl_int)
		{
			UserIntStat* value = (UserIntStat*)val;

			*((int*)ptr) = value->Get();
			ptr += sizeof(int);
			
			*((int*)ptr) = value->GetDefault();
			ptr += sizeof(int);

			len += sizeof(int) * 2;			
		}
		else
		if (val->GetType() == UserStat::vl_float)
		{
			UserFloatStat* value = (UserFloatStat*)val;

			*((float*)ptr) = value->Get();
			ptr += sizeof(float);				
			
			*((float*)ptr) = value->GetDefault();
			ptr += sizeof(float);

			len += sizeof(float) * 2;						
		}
		else
		if (val->GetType() == UserStat::vl_string)
		{
			UserStringStat* value = (UserStringStat*)val;

			int ln = 0;

			ln = strlen(value->Get());

			*((int*)ptr) = ln;
			ptr += sizeof(int);
			len += sizeof(int);

            int ost = ((int)((float)(ln)/4.0f) + 1)* 4 - ln;
            if (ost == 4 || ln == 0) ost = 0;
            
			if (len + ln > 16300)
			{
				file->Write(buffer, len);
				
				ptr = buffer;
				len = 0;

				file->Write(value->Get(), ln);
                file->Write(buffer, ost);
			}
			else
			{
				memcpy(ptr, value->Get(), ln);
				ptr += ln;
			
				len += ln;
                
                ptr += ost;
                len += ost;
			}

			ln = strlen(value->GetDefault());

			*((int*)ptr) = ln;
			ptr += sizeof(int);
			len += sizeof(int);

            ost = ((int)((float)(ln)/4.0f) + 1)* 4 - ln;
            if (ost == 4 || ln == 0) ost = 0;
            
			if (len + ln > 16300)
			{
				file->Write(buffer, len);
				
				ptr = buffer;
				len = 0;

				file->Write(value->GetDefault(), ln);
                file->Write(buffer, ost);
			}
			else
			{
				memcpy(ptr, value->GetDefault(), ln);
				
                ptr += ln;
				len += ln;
                
                ptr += ost;
                len += ost;
			}
		}
		else			
		if (val->GetType() == UserStat::vl_vector)
		{
			UserVectorStat* value = (UserVectorStat*)val;

			*((Vector*)ptr) = value->Get();
			ptr += sizeof(Vector);			
			
			*((Vector*)ptr) = value->GetDefault();
			ptr += sizeof(Vector);

			len += sizeof(Vector) * 2;			
		}
		else
		if (val->GetType() == UserStat::vl_quantarion)
		{
			UserQuaternionStat* value = (UserQuaternionStat*)val;

			*((Quaternion*)ptr) = value->Get();
			ptr += sizeof(Quaternion);			

			*((Quaternion*)ptr) = value->GetDefault();
			ptr += sizeof(Quaternion);

			len += sizeof(Quaternion) * 2;			
		}

		if (len > 16300)
		{
			file->Write(buffer, len);
				
			ptr = buffer;
			len = 0;
		}
	}	

	if (len > 0)
	{
		file->Write(buffer, len);	
	}	
}

UserStat* UserStatsService::GetValue(const char* id)
{
	int hash1 = HashValue(id);
	int hash2 = HashValue2(id);

	return GetValue(hash1, hash2);
}

void UserStatsService::GetHashes(const char* id, int& hash1, int& hash2)
{
	hash1 = HashValue(id);
	hash2 = HashValue2(id);
}

UserStat* UserStatsService::GetValue(int hash1, int hash2)
{
	for (int i=0; i<values.size(); i++)
	{
		if (values[i].hash1 == hash1)
		{
			if (values[i].hash2 == hash2)
			{
				return values[i].value;
			}
		}		
	}

	return null;
}

void UserStatsService::SetNamespace(const char* ns)
{
	String::Copy(name_space, 256, ns);
	Reset();
}

void UserStatsService::Release()
{
	Reset();
}