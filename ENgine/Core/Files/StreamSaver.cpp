#include "Core/Core.h"
#include "StreamSaver.h"

std::string StreamSaver::sBuffer = ""; 


const char* StreamSaver::ReplaceXmlControlSymbols(const char* name)
{
    sBuffer = "";
    const char* iter = name;
    while(*iter)
    {
        switch(*iter)
        {
            case '&':sBuffer += "[0x26]";break;
            case '<':sBuffer += "[0x3C]";break;
            default:sBuffer+= *iter;break;
        }
        ++iter;
    }
    return sBuffer.c_str();
}

bool StreamSaver::Init(const char* name, FileType type)
{
	this->type = type;

	IFile::mode_type fl_type = IFile::write;
	
	if (type == xml)
	{
		fl_type = IFile::writeText;
	}

	file = core->Files()->OpenFile(name,fl_type);	

	if (file)
	{
		if (type == xml)
		{
			byte bom[3];
			bom[0] = 0xEF;
			bom[1] = 0xBB;
			bom[2] = 0xBF;
			
			file->Write(bom, 3);
			file->Print("<?xml version=\"1.0\"?>");
		}

		return true;
	}

	return false;
}

IStreamSaver::FileType StreamSaver::GetType()
{
	return type;
}

void StreamSaver::MarkBeginBlock(const char* name)
{
	if (type == xml)
	{
		file->Print("<%s>", name);		
	}
	else
	{		
		Write("", name);
	}
}

void StreamSaver::MarkEndBlock(const char* name)
{
	if (type == xml)
	{
		file->Print("</%s>", name);		
	}
	else
	{		
		Write("", name);
	}
}

void StreamSaver::Write(const char* name, bool val)
{
	if (type == xml)
	{
		XMLSaveBool(file, name, val);
	}
	else
	{
		if (val)
		{
			Write(name, 1);
		}
		else
		{
			Write(name, 0);		
		}
	}
}

void StreamSaver::Write(const char* name, const char* val)
{
	if (type == xml)
	{
		XMLSaveString(file, name, ReplaceXmlControlSymbols(val));
	}
	else
	{
		int len = strlen(val);

		len++;

		file->Write(&len, 4);

		file->Write(val, strlen(val)+1);

		len = ((int)((float)(strlen(val)+1)/4.0f) + 1)* 4 - len;
		if (len == 4) len = 0;

		for (int i=0;i<len;i++)
		{
			byte zero = 0;
			file->Write(&zero, 1);
		}
	}
}

void StreamSaver::Write(const char* name, float val)
{
	if (type == xml)
	{
		XMLSaveFloat(file, name, val);
	}
	else
	{
		file->Write(&val, sizeof(val));
	}
}

void StreamSaver::Write(const char* name, int val)
{
	if (type == xml)
	{
		XMLSaveInt(file, name, val);
	}
	else
	{
		file->Write(&val, sizeof(val));
	}
}

void StreamSaver::Write(const char* name, int64 val)
{
	if (type == xml)
	{
		XMLSaveInt(file, name, val);
	}
	else
	{
		file->Write(&val, sizeof(val));
	}
}

void StreamSaver::Write(const char* name, dword val)
{
	if (type == xml)
	{
		XMLSaveDword(file, name, val);
	}
	else
	{
		float a,r,g,b;

		COLOR2RGBA(val,r,g,b,a)	
		a = 1.0f;
		dword color = RGBA2COLORF(b,g,r,a);

		file->Write(&color, sizeof(color));
	}
}

void StreamSaver::Write(const char* name, Matrix& mat)
{
	if (type == xml)
	{
		XMLSaveMatrix(file, name, mat);
	}
	else
	{
		float k = 1.0f;

#ifdef PC
		k = -1.0f;
#endif
		
		Write("m11", mat._11);
		Write("m12", mat._12);
		Write("m13", k * mat._13);

		Write("m21", mat._21);
		Write("m22", mat._22);
		Write("m23", k * mat._23);

		Write("m31", mat._31);
		Write("m32", mat._32);
		Write("m33", k * mat._33);

		Write("m41", mat._41);
		Write("m42", mat._42);
		Write("m43", k * mat._43);	
	}
}

void StreamSaver::Write(const char* name, Vector& v)
{
	if (type == xml)
	{
		XMLSaveVector(file, name, v);
	}
	else
	{
		Write("x", v.x);
		Write("y", v.y);
		Write("z", v.z);
	}
}

void StreamSaver::Release()
{
	if (file) file->Release();
	delete this;
}



