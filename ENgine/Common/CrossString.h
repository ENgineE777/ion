#pragma once

#include "Declarations.h"
#include <map>

struct Str64
{
	char data[64];

	Str64()
	{
		data[0] = 0;
	}
};

struct Str128
{
	char data[128];

	Str128()
	{
		data[0] = 0;
	}
};

struct Str256
{
	char data[256];

	Str256()
	{
		data[0] = 0;
	}
};

struct Str512
{
	char data[512];

	Str512()
	{
		data[0] = 0;
	}
};


class DEF_EXPORT String
{
public:

	static std::map<wchar_t, int> unicode_upper2lower;
	static std::map<wchar_t, int> unicode_lower2upper;
	static void Init();	
	static bool IsEmpty(const char * c);
	static long Compare(const char * s1, const char * s2);
	static bool IsEqual(const char * s1, const char * s2);
	static bool IsEqual(const char * str1, long len1, const char * str2, long len2);
	static void Copy(char * s1, int len, const char * s2);
	static void Cat(char * s1, int len, const char * s2);
	static void Printf(char * s1, int len, const char * format, ...);
	static void Scanf(const char * src, const char* format, ...);
	static void RemoveSlashes(char* FullPath);
	static void ExtractFileName(const char* FullPath, char *FileName);
	static void ExtractPath(const char* FullPath, char *Path,bool retempty);
	static void GetCropPath(const char* RelativePath,const char* FullPath, char *Path, int len);
	static void ExtractRoot(char* Path, char *root);
	static void ExtractRootPath(const char* path1,const char* path2,char* root);
	static void RemoveFirstChar(char* str);
	static void FixSlashes(char * str);
	static void ExtractExctention(const char* str,char* ext, int ext_lenght);
	static void RemoveExctention(char* str);
	static bool CompareAlphapedOrder(const char* str1, const char* str2);
	static bool IsSubstr(const char* str, const char* substr);
	static void InsertStr(char* str, const char* substr, int position);
	static void ExtractNameNumber(const char* str, char* wo_n_str, int len, int& number);
	static void LowerCase(char* str);
	static void LowerCase(string& str);
	static void UpperCase(string& str);
	static bool ParsePlatform(const char* in, char* out, int len, Platform platform);
	static void EscapeChars(const char* in, char* out, int len);
	static void ReplaceAll(string& str, const char* find,const char* replace);
    static bool GetAbsalutPath(string& sPath,const string& sBasePath);

    static void Utf8toUtf16(std::wstring& dest, const char* src);

    static  void Utf16toUtf8(std::string& dest, const wchar_t* src);

    static void ASCIItoUtf8(std::string& dest, const char* src);

};