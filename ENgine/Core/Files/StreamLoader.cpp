#include "Core/Core.h"
#include "StreamLoader.h"


std::string StreamLoader::sBuffer;
void  StreamLoader::ReplaceXmlControlSymbols(char* val, int val_len)
{
    sBuffer = "";
    const char* iter = val;
    const char* aheadIter;
    int i = 0;
    while(*iter && i < val_len)
    {
        aheadIter = iter;
        int state = 0;
        while(*aheadIter)
        {
            char oldState = state;
            switch(state)
            {
                case 0:if(*aheadIter == '[')++state;break;
                case 1:if(*aheadIter == '0')++state;break;
                case 2:if(*aheadIter == 'x')++state;break;
                case 3:
                    {
                        if(*aheadIter == '2')state = 4;
                        else
                             if(*aheadIter == '3')state = 7;
                        break;
                    }
                case 4:if(*aheadIter == '6')state = 5;break;
                case 5:if(*aheadIter == ']')state = 6;break;
                case 7:if(*aheadIter == 'C')state = 8;break;
                case 8:if(*aheadIter == ']')state = 9;break;
                    
            }
            if(oldState == state)break;
            ++aheadIter;
        }
        switch(state)
        {
            case 6:
                {
                    sBuffer += '&';
                    iter += 6;
                    break;
                }
            case 9:
                {
                    sBuffer += '<';
                    iter += 6;
                }
            default:
                {
                     sBuffer += *iter;
                     ++iter;
                }

        }
        ++i;
    }
    String::Copy(val,val_len,sBuffer.c_str());
}

bool StreamLoader::Init(const char* name, const char* enterBlock, FileType type,bool bMemory )
{		
	ptr = data = null;	

	this->type = type;

#ifdef PC
	String::Copy(sv_enterBlock, 64, enterBlock);
	saver = null;
#endif

	if (type == xml)
	{
		data_file = core->Files()->OpenXMLFile(name, enterBlock,bMemory);

		if (!data_file)
		{			
			return false;
		}
	}
	else
	if (type == json)
	{
		data_file = core->Files()->OpenJSONFile(name,bMemory);

		if (!data_file)
		{			
			return false;
		}
	}
	else
	{		
		readed = 0;
		file = core->Files()->LoadFile(name);		

		if (!file)
		{
			return false;
		}

		data = file->GetBuffer();
		ptr = data;

		if (!EnterBlock(enterBlock))
		{
			core->TraceTo("Files", "Can't find enterBlock - %s in scene %s", enterBlock, name);
			return false;
		}
	}

	return true;
}

#ifdef PC
void StreamLoader::InitSaver(const char* name)
{
	if (saver) return;
	saver = core->Files()->CreateStreamSaver(name, IStreamSaver::binary);
	saver->MarkBeginBlock(sv_enterBlock);
}
#endif

bool StreamLoader::EnterBlock(const char* name)
{	
	if (type == xml)
	{
		bool res = data_file->EnterBlock(name);	
#ifdef PC
		if (saver && res) saver->MarkBeginBlock(name);
#endif

		return res;		
	}
	else
	if (type == json)
	{
		return data_file->EnterBlock(name);	
	}
	else
	{
		byte* sv_ptr = ptr;
		int sv_readed = readed;

		char str[64];
		Read("block", str, 64);

		if (strcmp(str, name) == 0)
		{
#ifdef PC
			if (saver) saver->MarkBeginBlock(name);
#endif

			return true;
		}

		ptr = sv_ptr;		
		readed = sv_readed;
	}

	return false;
}

void StreamLoader::LeaveBlock()
{
	if (type == xml)
	{
		data_file->LeaveBlock();
	}
	else
	if (type == json)
	{
		data_file->LeaveBlock();	
	}
	else
	{
		char str[64];
		Read("block", str, 64);	
	}

#ifdef PC
	if (saver) saver->MarkEndBlock("#EndBlock#");
#endif
}

bool StreamLoader::Read(const char* name, char* val, int val_len)
{
	bool res = false;

	if (type == xml)
	{
		res = data_file->ReadString(name, val, val_len);
        ReplaceXmlControlSymbols(val,val_len);
	}
	else
	if (type == json)
	{
		res = data_file->ReadString(name, val, val_len);
	}
	else
	{		
		char ch = '7';

		int index = 0;
		
		ptr += sizeof(int);
		readed += sizeof(int);

		while (ch!=0)
		{			
			readed ++;
			if (readed>=file->GetSize()) break;

			ch = *((char*)ptr);
			ptr += sizeof(char);

			if (index < val_len - 1)
			{
				val[index] = ch;
			}
			else
			if (index == val_len - 1)
			{
				val[index] = 0;
			}
			
			index++;			
		}	
				
		int len = ((int)((float)(index)/4.0f) + 1)* 4 - index;
		if (len == 4) len = 0;

		ptr += sizeof(char) * len;
		readed += sizeof(char) * len;

		res = true;
	}

#ifdef PC
	if (saver) saver->Write(name, val);
#endif

	return res;
}

bool StreamLoader::Read(const char* name, bool& val)
{
	bool res = false;

	if (type == xml)
	{
		res = data_file->ReadBool(name, val);
	}
	else
	if (type == json)
	{
		res = data_file->ReadBool(name, val);
	}
	else
	{
		int vl = 0;
		Read(name, vl);

		val = (vl>0);		

		res = true;
	}

#ifdef PC
	if (saver) saver->Write(name, val);
#endif

	return res;
}

bool StreamLoader::Read(const char* name, float& val)
{
	bool res = false;

	if (type == xml)
	{
		res = data_file->ReadFloat(name, val);
	}
	else
	if (type == json)
	{
		res = data_file->ReadFloat(name, val);
	}
	else
	{
		val = *((float*)ptr);
		ptr += sizeof(float);
		readed += sizeof(float);

		res = true;
	}

#ifdef PC
	if (saver) saver->Write(name, val);
#endif

	return res;
}

bool StreamLoader::Read(const char* name, int& val)
{
	bool res = false;

	if (type == xml)
	{
		res = data_file->ReadInt(name, val);
	}
	else
	if (type == json)
	{
		res = data_file->ReadInt(name, val);
	}
	else
	{
		val = *((int*)ptr);
		ptr += sizeof(int);
		readed += sizeof(int);

		res = true;
	}

#ifdef PC
	if (saver) saver->Write(name, val);
#endif

	return res;
}

bool StreamLoader::Read(const char* name, int64& val)
{
	bool res = false;

	if (type == xml)
	{
		res = data_file->ReadInt64(name, val);
	}
	else
	if (type == json)
	{
		res = data_file->ReadInt64(name, val);
	}
	else
	{
		val = *((int64*)ptr);
		ptr += sizeof(int64);
		readed += sizeof(int64);

		res = true;
	}

#ifdef PC
	if (saver) saver->Write(name, val);
#endif

	return res;
}

bool StreamLoader::Read(const char* name, uint64& val)
{	
	int64 vl;
	bool res = Read(name, vl);

	val = vl;	

	return res;
}

bool StreamLoader::Read(const char* name, dword& val)
{
	bool res = false;

	if (type == xml)
	{
		res = data_file->ReadDword(name, val);
	}
	else
	if (type == json)
	{
		res = data_file->ReadDword(name, val);
	}
	else
	{
		val = *((dword*)ptr);
		ptr += sizeof(dword);
		readed += sizeof(dword);

#ifdef PC
		float a,r,g,b;

		COLOR2RGBA(val,r,g,b,a)	
		val = RGBA2COLORF(b,g,r,a);		
#endif		

		res = true;
	}

#ifdef PC
	if (saver) saver->Write(name, val);
#endif

	return res;
}

bool StreamLoader::Read(const char* name, Vector& v)
{
	bool res = false;

	if (type == xml)
	{
		res = data_file->ReadVector(name, v);
	}
	else
	if (type == json)
	{
		res = data_file->ReadVector(name, v);
	}
	else
	{
		Read("x", v.x);
		Read("y", v.y);
		Read("z", v.z);

		res = true;
	}

#ifdef PC
	if (saver) saver->Write(name, v);
#endif

	return res;
}

bool StreamLoader::Read(const char* name, Matrix& mat)
{
	bool res = false;

	if (type == xml)
	{
		res = data_file->ReadMatrix(name, mat);
	}
	else
	if (type == json)
	{
		res = data_file->ReadMatrix(name, mat);
	}
	else
	{
		Read("m11", mat._11);
		Read("m12", mat._12);
		Read("m13", mat._13);

		Read("m21", mat._21);
		Read("m22", mat._22);
		Read("m23", mat._23);

		Read("m31", mat._31);
		Read("m32", mat._32);
		Read("m33", mat._33);

		Read("m41", mat._41);
		Read("m42", mat._42);
		Read("m43", mat._43);	

#ifdef PC
		mat._13 = -mat._13;
		mat._23 = -mat._23;
		mat._33 = -mat._33;
		
		mat._43 = -mat._43;
#endif

		res = true;
	}

#ifdef PC
	if (saver) saver->Write(name, mat);
#endif

	return res;
}

int StreamLoader::GetStringSize(const char* name)
{
	if (type == xml)
	{
		return data_file->GetStringSize(name);
	}
	else
	if (type == json)
	{
		return data_file->GetStringSize(name);		
	}

	return *((int*)ptr);	
}

void StreamLoader::SetNeedLogWarnings(bool set)
{
#ifdef EDITOR
	if (type == xml)
	{
		return data_file->SetNeedLogWarnings(set);
	}
	else
	if (type == json)
	{
		return data_file->SetNeedLogWarnings(set);		
	}
#endif
}

void StreamLoader::Release()
{
	if (type == xml && data_file)
	{
		data_file->Release();
	}

	if (type == json && data_file)
	{
		data_file->Release();
	}

#ifdef PC
	if (saver) saver->Release();
#endif	

	if (type == binary && file)
	{
		file->Release();
	}

	delete this;
}


