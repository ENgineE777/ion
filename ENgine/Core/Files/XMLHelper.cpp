
#include "XMLHelper.h"
#include "Core/Core.h"

bool XMLReadString(tinyxml2::XMLNode* node, const char* name, char* val, int val_len)
{
	tinyxml2::XMLElement* elem = node->FirstChildElement(name);
	
	if (elem)
	{
		if (elem->GetText())
		{
			String::Copy(val, val_len, elem->GetText());			
		}
		else
		{
			val[0] = 0;
		}

		return true;
	}	

	return false;
}

int XMLGetStringSize(tinyxml2::XMLNode* node, const char* name)
{
	tinyxml2::XMLElement* elem = node->FirstChildElement(name);

	if (elem)
	{
		if (elem->GetText())
		{			
			return strlen(elem->GetText());			
		}				
	}	

	return 0;
}

bool XMLReadBool(tinyxml2::XMLNode* node, const char* name, bool& val)
{
	tinyxml2::XMLElement* elem = node->FirstChildElement(name);
	if (elem)
	{
		if (elem->GetText())
		{
			if (String::IsEqual(elem->GetText(),"1"))
			{
				val = true;
			}
			else
			{
				val = false;
			}

			return true;
		}		
	}	

	return false;
}

bool XMLReadFloat(tinyxml2::XMLNode* node, const char* name, float& val)
{
	tinyxml2::XMLElement* elem = node->FirstChildElement(name);
	if (elem)
	{
		const char* str = elem->GetText();

		if (elem->GetText())
		{
			val = (float)atof(elem->GetText());
			return true;
		}		
	}	

	return false;
}

bool XMLReadInt(tinyxml2::XMLNode* node, const char* name, int& val)
{
	tinyxml2::XMLElement* elem = node->FirstChildElement(name);
	if (elem)
	{
		const char* str = elem->GetText();

		if (elem->GetText())
		{
			val = atoi(elem->GetText());			
			return true;
		}		
	}	

	return false;
}

bool XMLReadInt64(tinyxml2::XMLNode* node, const char* name, int64& val)
{
	tinyxml2::XMLElement* elem = node->FirstChildElement(name);
	if (elem)
	{
		const char* str = elem->GetText();

		if (elem->GetText())
		{
			val = atoi(elem->GetText());			
			return true;
		}		
	}	

	return false;
}

bool XMLReadDword(tinyxml2::XMLNode* node, const char* name, dword& val)
{
	tinyxml2::XMLElement* elem = node->FirstChildElement(name);
	if (elem)
	{
		const char* str = elem->GetText();

		if (elem->GetText())
		{
			val = atol(elem->GetText());	
			return true;
		}		
	}	

	return false;
}

bool XMLReadMatrix(tinyxml2::XMLNode* node, const char* name, Matrix& mat)
{
	tinyxml2::XMLElement* elem = node->FirstChildElement(name);
	
	if (elem)
	{
		mat.Identity();

		XMLReadFloat(elem, "m11", mat._11);
		XMLReadFloat(elem, "m12", mat._12);
		XMLReadFloat(elem, "m13", mat._13);

		XMLReadFloat(elem, "m21", mat._21);
		XMLReadFloat(elem, "m22", mat._22);
		XMLReadFloat(elem, "m23", mat._23);

		XMLReadFloat(elem, "m31", mat._31);
		XMLReadFloat(elem, "m32", mat._32);
		XMLReadFloat(elem, "m33", mat._33);
		
		XMLReadFloat(elem, "m41", mat._41);
		XMLReadFloat(elem, "m42", mat._42);
		XMLReadFloat(elem, "m43", mat._43);

		return true;
	}

	return false;
}

bool XMLReadVector(tinyxml2::XMLNode* node, const char* name, Vector& v)
{
	tinyxml2::XMLElement* elem = node->FirstChildElement(name);
	
	if (elem)
	{
		v = Vector(0.f, 0.f, 0.f);

		XMLReadFloat(elem, "x", v.x);
		XMLReadFloat(elem, "y", v.y);
		XMLReadFloat(elem, "z", v.z);

		return true;
	}

	return false;
}

void XMLSaveString(IFile* pFile, const char* name, const char* val)
{
	pFile->Print("<%s>%s</%s>", name, val, name);
}

void XMLSaveBool(IFile* pFile, const char* name, bool val)
{
	if (val)
	{
		pFile->Print("<%s>1</%s>", name, name);
	}
	else
	{
		pFile->Print("<%s>0</%s>", name, name);
	}
}

void XMLSaveFloat(IFile* pFile, const char* name, float val)
{
	pFile->Print("<%s>%f</%s>", name, val, name);
}

void XMLSaveInt(IFile* pFile, const char* name, int val)
{
	pFile->Print("<%s>%i</%s>", name, val, name);
}

void XMLSaveInt(IFile* pFile, const char* name, int64 val)
{
	pFile->Print("<%s>%i</%s>", name, val, name);
}

void XMLSaveDword(IFile* pFile, const char* name, dword val)
{
	pFile->Print("<%s>%d</%s>", name, val, name);
}

void XMLSaveMatrix(IFile* pFile, const char* name, Matrix& mat)
{
	pFile->Print("<%s>", name);

	XMLSaveFloat(pFile, "m11", mat._11);
	XMLSaveFloat(pFile, "m12", mat._12);
	XMLSaveFloat(pFile, "m13", mat._13);

	XMLSaveFloat(pFile, "m21", mat._21);
	XMLSaveFloat(pFile, "m22", mat._22);
	XMLSaveFloat(pFile, "m23", mat._23);

	XMLSaveFloat(pFile, "m31", mat._31);
	XMLSaveFloat(pFile, "m32", mat._32);
	XMLSaveFloat(pFile, "m33", mat._33);

	XMLSaveFloat(pFile, "m41", mat._41);
	XMLSaveFloat(pFile, "m42", mat._42);
	XMLSaveFloat(pFile, "m43", mat._43);

	pFile->Print("</%s>", name);
}

void XMLSaveVector(IFile* pFile, const char* name, Vector& v)
{
	pFile->Print("<%s>", name);

	XMLSaveFloat(pFile, "x", v.x);
	XMLSaveFloat(pFile, "y", v.y);
	XMLSaveFloat(pFile, "z", v.z);

	pFile->Print("</%s>", name);
}