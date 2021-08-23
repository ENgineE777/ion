
#pragma once

#include "Common/CrossString.h"
#include "Common/Declarations.h"
#include "Common/Color.h"
#include "Common/Math.h"

struct BoundingBox
{
	Vector minV;
	Vector maxV;
};

BoundingBox ReCalcBBox(const BoundingBox& box, const Matrix& mat);

struct MouseData
{
	float x,y;
	float scr_x,scr_y;
	float maxX,maxY;
};

struct FontParams
{
	char  name[128];
	bool  bold;
	bool  italic;
	float height;
	float outline;
	float gap;
	Color color;
	bool  use_shadow;
	Color shd_color;
	class FontInstance* font;

    ~FontParams();
	FontParams()
	{
		strcpy(name, "Arial");		
		bold = 0;
		italic = 0;
		height = 20;
		outline = 0.0f;
		gap = 0.5f;
		color = COLOR_WHITE;
		use_shadow = false;
		shd_color = COLOR_BLACK;
		font = null;
	};

    FontParams(const FontParams& other);
    FontParams& operator = (const FontParams& other);

	void Copy(FontParams& dest) const
	{
		strcpy(dest.name, name);
		dest.bold = bold;
		dest.italic = italic;
		dest.height = height;
		dest.outline = outline;
		dest.gap = gap;
		dest.color = color;
		dest.use_shadow = use_shadow;
		dest.shd_color = shd_color;
	};

	bool IsEqual(FontParams& param)
	{
		bool res = true;
		res &= (strcmp(name, param.name) == 0);
		res &= (bold == param.bold);
		res &= (italic == param.italic);
		res &= (height == param.height);
		res &= (outline == param.outline);
		res &= (gap == param.gap);		
		res &= (color.r == param.color.r &&
				color.g == param.color.g &&
				color.b == param.color.b);
		res &= (use_shadow == param.use_shadow);
		res &= (shd_color.r == param.shd_color.r &&
				shd_color.g == param.shd_color.g &&
				shd_color.b == param.shd_color.b);		

		return res;
	}
};

Vector Bernstein(float u, Vector *p);
float SIGN(float X);
 
bool IntersectTriangle( const Vector& orig, const Vector& dir,
					    Vector& v0,Vector& v1, Vector& v2,
					    float* dist);
 
bool CubeRayIntersection(const BoundingBox& box, const Vector& origin, const Vector& dir);

void SliceChangeValue(float &value, float needed, float accel, float timedelta);

Vector getRotationDegrees(Matrix mat);
Matrix GetCubeMapViewMatrix( int dwFace, Vector vEyePt );

void LerpMatrixes(const Matrix mat_from,const Matrix mat_to, Matrix& mat_res,float blend);

void ExtractAlpaBetta(Vector dir,float& alpha,float& betta);

bool ClipSprite(float& x, float& y, float& w, float& h, float& u, float& v, float& du, float& dv, float clip_x1, float clip_x2, float clip_y1, float clip_y2);
bool ClipSprite(float& x, float& y, float& w, float& h, float clip_x1, float clip_x2, float clip_y1, float clip_y2, float rotate, float delta_x, float delta_y);

void GetMD5Hash(const char* in_str, char* res);
void GetHMac(const char* in_str, const char* key, char* res);

int GetPow(int value);

#if defined(IOS) || defined(OSUNIX) || defined(ANDROID) || defined(OSX)
void DeleteFile(const char* name);
#endif

#if defined(PC) || defined(OSUNIX)

bool DeleteDirectory(const char* dir, bool noRecycleBin = true);

int RunSilent(char* strFunct, char* strstrParams);

void GetCurDirectory(char* path, int size);

void CreateDirectories(const char* path);

bool CopyFileEX(const char* FileName,const char* NewFileName);

void DeleteFolder(const char* FolderName);

#endif

#ifdef PC

char* ReadINI(const char* INIFileName, const char *defaultvalue, const char *group, const char *name);
int   ReadINI(const char* INIFileName, int defaultvalue, const char *group, const char *name);

bool WriteINI(const char* INIFileName, const char *value, const char *group, const char *name);
bool WriteINI(const char* INIFileName, int value, const char *group, const char *name);


bool OpenFile(char* filename, const char* initdir, const char* mask, const char* caption);
bool SaveFile(char* filename, const char* initdir, const char* mask, const char* caption);

bool OpenDialog(char* file_name, bool relativePath,const char* filter = "");
void ColorDialog(Color& color);

void GetFolderFileList(const char* FolderName,vector<string>& list);

bool DialogSelectFile(bool open, char* file_name, bool full_path, const char* filter);

#endif

bool ReadArg(int numArg, char** arg, const char* name, int& value);
bool ReadArg(int numArg, char** arg, const char* name, char* value, int val_len);

int GetTime(int index);
int GetTimeUTC(int index);
int GetTime(int index, int64 unix_time);
int64 GetTimestamp(int year, int month, int day, int hour, int minute, int second);
void Get_ISO8601_datetime( time_t time, std::string& str);
bool ParseCSV(const char* csvSource, int size, vector<vector<string> >& lines);

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);

bool BuildUtf16fromUtf8(byte c, int& bytes, int& w);
void BuildUtf8fromUtf16(int c, string& dest);

void HTML2Text(string& in, string& out);

int  CalcUtf8Len(const char* text);
void ClampUtf8(string& text, int clamp_len);

bool IsPointInsideTriangle(Vector2 s, Vector2 a, Vector2 b, Vector2 c);


bool ConvertHexColor(const std::string& txt,long& red,long& green,long& blue);
bool ConvertHexColor(const std::string& txt,long& red,long& green,long& blue,float& alpha);

class EasyInOut
{
public:

	static float QuadraticEaseIn(float p);
	static float QuadraticEaseOut(float p);
	static float QuadraticEaseInOut(float p);

	static float CubicEaseIn(float p);
	static float CubicEaseOut(float p);
	static float CubicEaseInOut(float p);

	static float QuarticEaseIn(float p);
	static float QuarticEaseOut(float p);
	static float QuarticEaseInOut(float p);

	static float QuinticEaseIn(float p);
	static float QuinticEaseOut(float p);
	static float QuinticEaseInOut(float p);

	static float SineEaseIn(float p);
	static float SineEaseOut(float p);
	static float SineEaseInOut(float p);

	static float CircularEaseIn(float p);
	static float CircularEaseOut(float p);
	static float CircularEaseInOut(float p);
	
	static float ExponentialEaseIn(float p);
	static float ExponentialEaseOut(float p);
	static float ExponentialEaseInOut(float p);

	static float ElasticEaseIn(float p);
	static float ElasticEaseOut(float p);
	static float ElasticEaseInOut(float p);

	static float BackEaseIn(float p);
	static float BackEaseOut(float p);
	static float BackEaseInOut(float p);

	static float BounceEaseIn(float p);
	static float BounceEaseOut(float p);
	static float BounceEaseInOut(float p);
};