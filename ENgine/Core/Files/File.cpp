#include "Core/Core.h"
#include "File.h"
#include "stdarg.h"

#ifdef ANDROID
extern zip* zip_arch;
#endif

#ifdef IOS
#include "sys/xattr.h"
#endif

#if defined(OSUNIX)
#include <errno.h>
#endif

bool File::Init(const char* name, mode_type mode)
{		
	return Init(name, 0, -1, mode);
}

bool File::Init(const char* name, int set_offset, int set_size, mode_type mode)
{	
#ifdef ANDROID			

	if (core->Files()->IsLoadFromApk())
	{		
		pFile = null;

		char file_name[256];
		String::ExtractFileName(name, file_name);
	
		String::Copy(file_name, 256, "assets/");
		String::Cat(file_name, 256, name);

		struct zip_stat st;
		zip_stat_init(&st);
		zip_stat(zip_arch, file_name, 0, &st);

		size = st.size;	

		zip_f = zip_fopen(zip_arch, file_name, 0);
	
		if (zip_f)
		{
			return true;
		}		
		else
		{
			zip_f = null;
			//core->TraceTo("Files", "not finded %s in apk", file_name);
		}
	}

#endif
	
 
	/*
	//header for testing if it is a png
	png_byte header[8];
 
	//read the header
	zip_fread(file, header, 8);
 
	//test if png
	int is_png = !png_sig_cmp(header, 0, 8);
	if (!is_png) {
    zip_fclose(file);
    LOGE("Not a png file : %s", filename);
    return TEXTURE_LOAD_ERROR;
  }*/	


	offset = set_offset;
	size = set_size;

	char file_name[2048];
    file_name[0] = 0;

#if defined(PC) || defined(OSUNIX)
    strcpy(file_name, name);
    en_core.EN_Files()->CorrectPath(file_name);    
#endif        

#ifdef IOS
    if (name[0] != '/' || name[1] != 'v')
    {
        char buffer[1024];
        
        if (!String::ParsePlatform(name, buffer, 1024, core->GetCurrentPlatform()))
        {
            String::Copy(buffer,1023,name);
        }
        
        String::LowerCase(buffer);
        String::RemoveSlashes(buffer);
        
        if (mode == read)
        {
            NSString* nsName = [[NSString alloc] initWithFormat:@"%s", buffer];
            NSString* filePath = [[NSBundle mainBundle] pathForResource:nsName ofType:@""];
        
            if (filePath)
            {
                strcpy(file_name, [filePath UTF8String]);
            }
        }
        else
        {
            NSString *imageFilename = [NSString stringWithFormat:@"%s", buffer];
            NSArray *paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
            NSString *path = [NSString stringWithFormat:@"%@/%@", [paths objectAtIndex:0], imageFilename];
            
            if (path)
            {
                String::Copy(file_name, 2048, [path UTF8String]);
            }                       
        }
    }
    else
    {
        strcpy(file_name, name);
    }
#endif
    
#ifdef ANDROID
		if (((name[0] == '/') || (name[0] == '\\')) &&
			 ( (((name[1] == 's') || (name[1] == 'S')) && ((name[2] == 'd') || (name[2] == 'D'))) ||
			   (((name[1] == 'm') || (name[1] == 'M')) && ((name[2] == 'n') || (name[2] == 'N'))) ||
			   (((name[1] == 's') || (name[1] == 'S')) && ((name[2] == 't') || (name[2] == 'T'))) ))
		{
			strcpy(file_name, name);
		}
		else
		{
			strcpy(file_name, core->Files()->GetBasePath());
			strcat(file_name, name);
		}
#endif

	if (mode == read)
	{
		pFile = fopen (file_name,"rb");
#if defined(OSUNIX)
		if (!pFile)
		{
			core->TraceTo("Files", "Error: %d: %s: File:'%s'", errno, strerror(errno), file_name);
		}
#endif
        
#ifdef IOS
        if (!pFile)
        {
            if (name[0] != '/' || name[1] != 'v')
            {
                char buffer[1024];
            
                if (!String::ParsePlatform(name, buffer, 1024, core->GetCurrentPlatform()))
                {
                    String::Copy(buffer,1023,name);
                }
            
                String::LowerCase(buffer);
                String::RemoveSlashes(buffer);
            
                NSString *imageFilename = [NSString stringWithFormat:@"%s", buffer];
                
                NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
                NSString *path = [NSString stringWithFormat:@"%@/%@", [paths objectAtIndex:0], imageFilename];
                
                if (path)
                {
                    String::Copy(file_name, 2048, [path UTF8String]);
                }
                
                pFile = fopen (file_name,"rb");
                
                if (!pFile)
                {
                    paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
                    path = [NSString stringWithFormat:@"%@/%@", [paths objectAtIndex:0], imageFilename];
                    
                    if (path)
                    {
                        String::Copy(file_name, 2048, [path UTF8String]);
                    }
                    
                    pFile = fopen (file_name,"rb");
                    
                    if (!pFile)
                    {
                        paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
                        path = [NSString stringWithFormat:@"%@/%@", [paths objectAtIndex:0], imageFilename];
                    
                        if (path)
                        {
                            String::Copy(file_name, 2048, [path UTF8String]);
                        }
                    
                        pFile = fopen (file_name,"rb");
                    }
                }
            }
        }
#endif
	}
	else
	{
		if (mode == writeText)
		{
			pFile = fopen (file_name,"w");
		}
		else
		if (mode == write)
		{		
			pFile = fopen (file_name,"wb");
		}
		else
		if (mode == appendText)
		{
			pFile = fopen (file_name,"a");
		}
		else		
		{		
			pFile = fopen (file_name,"ab");
		}
	}

	if (pFile)
	{
#if defined(PC) || defined(OSUNIX)
		en_core.EN_Files()->RegFile(name);
#endif        

		if (set_size == -1)
		{
			Seek(0, seek_end);
			size = GetCurrentPos();
		}

		Seek(0, seek_set);		

		return true;
	}

#ifdef PC
	en_core.EN_Files()->ReportFileError(name);
#endif	

	return false;
}

int File::GetSize()
{
	return size;
}

void File::Write(const void* data, int size)
{
	fwrite(data, size, 1, pFile);
}

void File::Print(const char *format, ...)
{
	static char* buf = null;
	static int buf_len = 0;
	
	if (!buf)
	{
		buf_len = 1024;
		buf = (char*)malloc(buf_len);
	}

	va_list	args;
	va_start( args, format );

	int len = vsnprintf(0, 0, format, args);

	if (len > buf_len + 1)
	{
		buf_len = len + 10;
		buf = (char*)realloc(buf, buf_len);		
	}

	buf[0] = 0;

	vsnprintf( buf, buf_len, format, args );	
	va_end( args );
	
	//shit happens here
	//Write(buf, strlen(buf)+1);
	fprintf(pFile,"%s",buf);
	fprintf(pFile,"\n");	
}

int File::Read(void* data, int size)
{
#ifdef ANDROID	
	if (zip_f)
	{
		return zip_fread(zip_f, data, size);
	}
#endif	

	return fread(data, 1, size, pFile);
}

void File::Read(char* val)
{
	static char str[1024];

	char ch = '7';

	int index = 0;
	while(ch!=0)
	{			
		if (!Read(&ch,1))
		{
			break;
		}

		str[index] = ch;
		index++;
	}

	strcpy(val, str);	
}

int File::Seek(dword fl_offset, seek_type seek)
{	
	if (pFile)
	{
		if (seek == seek_set)
		{
			return fseek(pFile, fl_offset + offset, SEEK_SET);
		}

		if (seek == seek_cur)
		{					
			//int pos = GetCurrentPos();
			int res = fseek(pFile, fl_offset, SEEK_CUR);
			
			//int pos_2 = GetCurrentPos();
			//core->Trace("seek_cur %i %i %i %i", offset, fl_offset, pos, pos_2);

			return res;
		}

		if (offset > 0)
		{
			return fseek(pFile, offset + size - fl_offset, SEEK_SET);
		}

		return fseek(pFile, fl_offset, SEEK_END);
	}

	return -1;
}

dword File::GetCurrentPos()
{
	if (pFile) return ftell(pFile) - offset;

	return -1;
}

void File::Release()
{
#ifdef ANDROID		
	if (zip_f) zip_fclose(zip_f);
#endif

	if (pFile) fclose(pFile);

	delete this;
}

/*
typedef int TEX_HANDLE;
typedef int SHD_HANDLE;

void kkhkkjkj(TEX_HANDLE k)
{
}

void kkhkkjkj1(SHD_HANDLE k)
{
}

void kkhkkjkj2(SHD_HANDLE k)
{
	TEX_HANDLE h1;
	SHD_HANDLE h2;

	kkhkkjkj(h2);
	kkhkkjkj1(h1);
}*/

struct TEX_HANDLE
{
	int handle;
};

struct SHD_HANDLE
{
	int handle;
};

void work_with_tex(TEX_HANDLE k)
{
}

void work_with_shd(SHD_HANDLE k)
{
}

void test()
{
	TEX_HANDLE h1;
	SHD_HANDLE h2;

	work_with_tex(h2);
	work_with_shd(h1);
}
