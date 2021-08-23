#include "Core/Core.h"
#include "Common/Utils.h"

#ifdef EDITOR

struct FontDesc
{	
	char filename[128];
	char filename_bold[128];
	char filename_italic[128];
	char filename_bold_italic[128];

	void Copy(const FontDesc* other)
	{
		if(other)
		{
			String::Copy(filename,128,other->filename);
			String::Copy(filename_bold,128,other->filename_bold);
			String::Copy(filename_italic,128,other->filename_italic);
			String::Copy(filename_bold_italic,128,other->filename_bold_italic);
		}
	}
	void Set(const char* name,const bool italic,const bool bold)
	{

		if (italic && bold)
		{
			String::Copy(filename_bold_italic, 128,name);
		}
		else
			if (italic)
			{
				String::Copy(filename_italic, 128,name);
			}
			else
				if (bold)
				{
					String::Copy(filename_bold, 128,name);
				}
				else
				{
					String::Copy(filename, 128,name);
				}
	}
	FontDesc()
	{
		filename[0] = 0;
		filename_bold[0] = 0;
		filename_italic[0] = 0;
		filename_bold_italic[0] = 0;
	}
};

HashMap<FontDesc> fonts_desc(_FL_);

void ScanFontDirectory(const char* FolderName)
{
	vector<string> list;
	GetFolderFileList(FolderName,list);
	for(unsigned i  = 0;i < list.size();++i)
	{
		char buff[128];
		String::Copy(buff, 128, list[i].c_str());
		String::RemoveExctention(buff);
		bool italic = false,bold = false;
		const char* pEnd = strrchr(buff,'_');
		if(pEnd)
		{
			if(String::IsSubstr(buff,"_bi"))
			{
				italic = true;bold = true;
			}
			else
			{
				if(String::IsSubstr(buff,"_b"))
				{
					bold = true;
				}
				else
					if(String::IsSubstr(buff,"_i"))
					{
						italic = true;
					}
			}
			buff[pEnd - buff] = 0;
		}

		FontDesc* desc = fonts_desc.Get(buff);
		if (!desc)
		{
			desc = fonts_desc.Add(buff);
		}
		char file[128];
		String::Printf(file, 128, "editor\\fonts\\%s", buff);
		desc->Set(file,italic,bold);
	}
}

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

char window_dir[512];

void ParseFontFileName(char* dest,const char* source,int len,bool& italic,bool& bold)
{
	if(dest && source)
	{
		String::Copy(dest, len, source);
		char* iter = strstr(dest, "(TrueType)");

		if (iter)
		{
			dest[iter - dest] = 0;
		}
		iter = strstr(dest, "(OpenType)");

		if (iter)
		{
			dest[iter - dest] = 0;
		}

		iter = strstr(dest, "Italic");

		if (iter)
		{
			dest[iter - dest] = 0;
			italic = true;
		}

		iter = strstr(dest, "Bold");

		if (iter)
		{
			dest[iter - dest] = 0;
			bold = true;
		}

		iter = strstr(dest, "Regular");

		if (iter)
		{
			dest[iter - dest] = 0;
			bold = true;
		}

		int len = strlen(dest);

		if (dest[len-1] == ' ' || dest[len-1] == '-')
		{
			dest[len-1] = 0;
		}

		String::LowerCase(dest);
	}


}
void QueryKey(HKEY hKey) 
{ 
    TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
    DWORD    cbName;                   // size of name string 
    TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
    DWORD    cchClassName = MAX_PATH;  // size of class string 
    DWORD    cSubKeys=0;               // number of subkeys 
    DWORD    cbMaxSubKey;              // longest subkey size 
    DWORD    cchMaxClass;              // longest class string 
    DWORD    cValues;              // number of values for key 
    DWORD    cchMaxValue;          // longest value name 
    DWORD    cbMaxValueData;       // longest value data 
    DWORD    cbSecurityDescriptor; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 
 
    DWORD i, retCode; 
 
    TCHAR  achValue[MAX_VALUE_NAME]; 
    DWORD cchValue = MAX_VALUE_NAME; 
 
    // Get the class name and the value count. 
    retCode = RegQueryInfoKey(
        hKey,                    // key handle 
        achClass,                // buffer for class name 
        &cchClassName,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // number of subkeys 
        &cbMaxSubKey,            // longest subkey size 
        &cchMaxClass,            // longest class string 
        &cValues,                // number of values for this key 
        &cchMaxValue,            // longest value name 
        &cbMaxValueData,         // longest value data 
        &cbSecurityDescriptor,   // security descriptor 
        &ftLastWriteTime);       // last write time 
 
    // Enumerate the subkeys, until RegEnumKeyEx fails.
    
    if (cSubKeys)
    {
        printf( "\nNumber of subkeys: %d\n", cSubKeys);

        for (i=0; i<cSubKeys; i++) 
        { 
            cbName = MAX_KEY_LENGTH;
            retCode = RegEnumKeyEx(hKey, i,
                     achKey, 
                     &cbName, 
                     NULL, 
                     NULL, 
                     NULL, 
                     &ftLastWriteTime); 
            			
			if (retCode == ERROR_SUCCESS) 
            {
				printf(achValue);
                //cor//_tprintf(TEXT("(%d) %s\n"), i+1, achKey);
            }
        }
    } 
 
    // Enumerate the key values. 
    if (cValues) 
    {
        printf( "\nNumber of values: %d\n", cValues);

        for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
        { 
            cchValue = MAX_VALUE_NAME; 
            achValue[0] = '\0';             
 
			if (RegEnumValue(hKey, i, achValue, &cchValue, NULL, NULL, NULL,  NULL) == ERROR_SUCCESS) 
			{
				char buff[128];
				bool italic = false,bold = false;

				ParseFontFileName(buff,achValue,128,italic,bold);

				FontDesc* desc = fonts_desc.Get(buff);

				if (!desc)
				{
					desc = fonts_desc.Add(buff);
				}

				char filename[128];
				DWORD value_length = 128;

				DWORD dwType = REG_SZ;
				RegQueryValueEx(hKey, achValue, NULL, &dwType, (LPBYTE)filename, &value_length);
				String::Printf(buff, 128, "%s//Fonts//%s", window_dir, filename);
				desc->Set(buff,italic,bold);
				//core->TraceTo("app", achValue);
			}						
        }
    }
}

bool IsFontStyleSupported(const char* name, bool bold, bool italic)
{
    char sName[512];
    String::Copy(sName,512,name);
    String::LowerCase(sName);
	FontDesc* desc = fonts_desc.Get(sName);

	if (desc)
	{
        if (bold && !italic)
		{
            return desc->filename_bold[0];
		}
        else
        if (!bold && italic)
		{
			return desc->filename_italic[0];
		}
        else
		if (bold && italic)
		{
			return desc->filename_bold_italic[0];
		}		

		return desc->filename[0];
	}

	return false;
}

#endif

#ifdef EDITOR

void EditorFontEnumerator(ComboBoxWidget* widget)
{
	for (int i = 0; i<fonts_desc.Size(); i++)
	{
		widget->AddString(fonts_desc.GetName(i));			
	}
	
	widget->SetCurString(0);
}

#endif

VertexBuffer*    FontService::vbuffer = null;	
RenderTechnique* FontService::tech_nfont = null;

FontService::FontService() : fonts(_FL_)
{
}

bool FontService::Init()
{	
	locker.Init();

#ifdef EDITOR	

	GetWindowsDirectory(window_dir, 512);

	HKEY hTestKey;

	if( RegOpenKeyEx( HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"), 0, KEY_READ, &hTestKey) == ERROR_SUCCESS)	
	{
		QueryKey(hTestKey);
	}

	RegCloseKey(hTestKey);
	ScanFontDirectory("editor\\fonts");

#endif
    FT_Init_FreeType( &library );	

	core->AddTask(this, this, (TaskHolder::Task)&FontService::Work, -10, -1.f);
    
	return true;
}

void FontService::Work(float dt, int level)
{

}

FontInstance* FontService::CreateFont(const char* file_name, bool is_bold, bool is_italic, int height, float row_gap, float outline_thin)
{
    if (!file_name[0]) return null;

	if (height > 100)
	{
		height = 100;
	}

	char nm[512];
    char font_path[128];
	font_path[0] = 0;

	FontRes* res;

	//String::Printf(nm, 512, "%s%i%2.2f", file_name, height, outline_thin);	
	//String::LowerCase(nm);

	//FontRes* res = fonts.Get(nm);

	//if (!res)
	{
		const char* prefix = "";

		if (is_bold && !is_italic)
		{
			prefix = "_b";
		}
		else
		if (!is_bold && is_italic)
		{
			prefix = "_i";
		}
		else
		if (is_bold && is_italic)
		{
			prefix = "_bi";
		}
		
		String::Printf(font_path, 128, "editor/fonts/%s%s", file_name, prefix);
		String::LowerCase(font_path);		

		String::Printf(nm, 512, "%s%i%2.2f%s", font_path, height, outline_thin, prefix);	
		String::LowerCase(nm);

		res = fonts.Get(nm);

#ifdef EDITOR	
		if (!res)
		{
			IFile* fl = core->Files()->OpenFile(font_path, IFile::read);	
			if (!fl)
			{			
                char sName[512];
                String::Copy(sName,512,file_name);
                String::LowerCase(sName);
				FontDesc* desc = fonts_desc.Get(sName);
				
				/*if(!desc)
				{
					desc = fonts_desc.Add(sName);
					desc->Copy(fonts_desc.Get("arial"));
				}*/

				if (desc)
				{
                    if (is_bold && !is_italic)
                        CopyFileEX(desc->filename_bold, font_path);
                    else
                        if (!is_bold && is_italic)
                            CopyFileEX(desc->filename_italic, font_path);
                        else
                            if (is_bold && is_italic)
                                CopyFileEX(desc->filename_bold_italic, font_path);
                            else
                                CopyFileEX(desc->filename, font_path);			
				}				
			}
			else
			{
				fl->Release();
			}
			
			//String::Printf(nm, 512, "%s%i%2.2f", file_name, height, outline_thin);	
			//String::LowerCase(nm);

			//res = fonts.Get(nm);
		}
#endif		

		file_name = font_path;
	}						

	if (!res)
	{
		res = new FontRes(library, file_name, height, outline_thin);		

		locker.Lock();

		if (!res->Load())
		{
			res->Release();

			locker.UnLock();

			return null;
		}		
		
		locker.UnLock();

		fonts.Add(nm,res);
	}	
			
#if defined(PC) || defined(OSUNIX)	
	en_core.EN_Files()->RegFile(res->GetFileName());
#endif
	return res->CreateReference(row_gap);
}

void FontService::DeleteRes(FontRes* res)
{
	fonts.Delete(res,false);
}

void FontService::Reset()
{
	locker.Lock();

	for (int i=0;i<fonts.Size();i++)
	{
		fonts.Get(i)->Reset();
	}

	locker.UnLock();
}

void FontService::Restore()
{
	locker.Lock();

	for (int i=0;i<fonts.Size();i++)
	{		
		fonts.Get(i)->Restore();
	}

	locker.UnLock();
}

void FontService::Release()
{	
    FT_Done_FreeType(library);
}

