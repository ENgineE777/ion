
#include "ScriptModulecore.h"
#include "ScriptMachine.h"
#include "ScriptModule.h"
#include "Core/Core.h"
#include "Common/Utils.h"
#include "Entities/NGUI/NGUIWidget.h"
#include <time.h>
#include "Core/Scene/SceneInstance.h"

#if defined(OSUNIX)
#include <sys/stat.h>
#endif

#if defined(OSX) || defined(IOS)
#include <sys/time.h>
#include <sys/stat.h>
#endif

#if defined(IOS)

#include "Platforms/IOS/IOSWrapper.h"

#endif

#if defined(PC)
#include <time.h>

#endif

#if defined(ANDROID)
#include <sys/stat.h>
extern void CallFromJava(const char* function);
extern void CallFromJava(const char* function,  const char* param);
#endif

extern int64 android_extern_installationtime;

bool ScriptModuleCore::Controls::IsKey(int key, int action)
{
    return core->Controls()->ControlState(key,(ControlsService::KeyAction)action);
}

float ScriptModuleCore::Controls::GetControlValue(int key)
{
	return core->Controls()->ControlValue(key);
}

bool ScriptModuleCore::Controls::CheckTouch(int index, int action)
{
	return core->Controls()->CheckTouch(index, (ControlsService::KeyAction)action, null);
}

float ScriptModuleCore::Controls::GetTouchX(int index)
{
	return core->Controls()->GetTouchX(index);
}

float ScriptModuleCore::Controls::GetTouchY(int index)
{
	return core->Controls()->GetTouchY(index);
}

int ScriptModuleCore::Controls::GetSoftKeyboardHeight()
{
	return core->Controls()->GetSoftKeyboardHeight();
}

bool ScriptModuleCore::Controls::IsAnyKeyPressed()
{
	for (int i=0;i<3;i++)
	{
		if (CheckTouch(i, 1)) return true;
	}

	return core->Controls()->IsAnyKeyPressed();	
}

void ScriptModuleCore::Controls::HideSoftKeyboard()
{
	core->Controls()->HideSoftKeyboard();
}

bool ScriptModuleCore::Controls::IsShowSoftKeyboard()
{
	return core->Controls()->IsShowSoftKeyboard();
}

float ScriptModuleCore::Controls::GetGyro(int index)
{
	return core->Controls()->GetGyro(index);
}

void ScriptModuleCore::Controls::SetGyro(int index, float val)
{
	core->Controls()->SetGyro(index, val);
}

bool ScriptModuleCore::Controls::IsBackPressed()
{
	return core->Controls()->IsBackPressed();
}

void ScriptModuleCore::Scene::LoadScene(std::string& sceneName)
{    
	en_core.EN_Scene()->Load(sceneName.c_str());
}

void ScriptModuleCore::Scene::UnloadScene(std::string& sceneName)
{
	en_core.EN_Scene()->Unload(sceneName.c_str());
}

bool ScriptModuleCore::Scene::IsSceneLoading(std::string& sceneName)
{
	SceneInstance* scene = en_core.EN_Scene()->FindScene(sceneName.c_str());
	
	if (scene)
	{
		return scene->IsLoading();
	}

	return false;
}

float ScriptModuleCore::Scene::GetLoadingProgress(std::string& sceneName)
{
	SceneInstance* scene = en_core.EN_Scene()->FindScene(sceneName.c_str());

	if (scene)
	{
		return scene->GetLoadingProgress();
	}

	return -1.0f;
}

bool ScriptModuleCore::Scene::ActivateInScene(std::string& sceneName, std::string& entityName, bool act)
{
	SceneInstance* scene = en_core.EN_Scene()->FindScene(sceneName.c_str());

	if (scene)
	{
		SceneEntity* entity = scene->Find(entityName.c_str());

		if (entity)
		{
			entity->Activate(act);
			return true;
		}
	}

	return false;
}

float ScriptModuleCore::Render::GetWidth()
{
	return en_core.Render()->Display()->GetWidth();
}

float ScriptModuleCore::Render::GetHeight()
{
	return en_core.Render()->Display()->GetHeight();
}

float ScriptModuleCore::Render::GetReferenceWidth()
{
	return en_core.Render()->Display()->GetReferenceWidth();
}

float ScriptModuleCore::Render::GetReferenceHeight()
{
	return en_core.Render()->Display()->GetReferenceHeight();
}

float ScriptModuleCore::Render::HorzRefToScreen(float sz)
{
	return NGUIService::instance->h_mul_RefToScreen * sz;			
}

float ScriptModuleCore::Render::VertRefToScreen(float sz)
{
	return NGUIService::instance->v_mul_RefToScreen * sz;
}

float ScriptModuleCore::Render::HorzScreenToRef(float sz)
{
	return NGUIService::instance->h_mul_ScreenToRef * sz;	
}

float ScriptModuleCore::Render::VertScreenToRef(float sz)
{	
	return NGUIService::instance->v_mul_ScreenToRef * sz;	
}

void ScriptModuleCore::Render::PrintText(float x, float y, std::string& text)
{
	en_core.Render()->Debug()->PrintText(x, y, up_left, COLOR_GREEN, text.c_str());
}

void ScriptModuleCore::Render::DrawLine2D(Vector2& p1, Vector& color1, Vector2& p2, Vector& color2)
{
	en_core.Render()->Debug()->DrawLine2D(p1, Color(color1.x,color1.y,color1.z), p2, Color(color2.x,color2.y,color2.z));
}

int ScriptModuleCore::Render::GetScreenDPI()
{
	return en_core.Render()->Display()->GetScreenDPI();
}

void ScriptModuleCore::Render::MakeScreenshot(std::string& out_image)
{
	en_core.Render()->MakeScreenshot(out_image);
}

void ScriptModuleCore::UserStats::SetFloat(std::string& id, float def_value, float value, bool add)
{
	core->UserStats()->SetFloat(id.c_str(), def_value, value, add);
}

void ScriptModuleCore::UserStats::SetInt(std::string& id, int def_value, int value, bool add)
{
	core->UserStats()->SetInt(id.c_str(), def_value, value, add);
}

void ScriptModuleCore::UserStats::SetVector(std::string& id, Vector& def_value, Vector& value)
{
	core->UserStats()->SetVector(id.c_str(), def_value, value);
}

void ScriptModuleCore::UserStats::SetString(std::string& id, std::string& def_value, std::string& value)
{
	core->UserStats()->SetString(id.c_str(), def_value.c_str(), value.c_str());
}

float ScriptModuleCore::UserStats::GetFloat(std::string& id, float def_value)
{
	return core->UserStats()->GetFloat(id.c_str(), def_value);
}

int ScriptModuleCore::UserStats::GetInt(std::string& id, int def_value)
{
	return core->UserStats()->GetInt(id.c_str(), def_value);
}

Vector ScriptModuleCore::UserStats::GetVector(std::string& id, Vector& def_value)
{
	return core->UserStats()->GetVector(id.c_str(), def_value);
}

void ScriptModuleCore::UserStats::GetString(std::string& id, std::string& def_value, std::string& value)
{
	value = core->UserStats()->GetString(id.c_str(), def_value.c_str());
}

void ScriptModuleCore::UserStats::SetFloatHash(int hash1, int hash2, float def_value, float value, bool add)
{
	core->UserStats()->SetFloat(hash1, hash2, def_value, value, add);
}

void ScriptModuleCore::UserStats::SetIntHash(int hash1, int hash2, int def_value, int value, bool add)
{
	core->UserStats()->SetInt(hash1, hash2, def_value, value, add);
}

void ScriptModuleCore::UserStats::SetVectorHash(int hash1, int hash2, Vector& def_value, Vector& value)
{
	core->UserStats()->SetVector(hash1, hash2, def_value, value);
}

void ScriptModuleCore::UserStats::SetStringHash(int hash1, int hash2, std::string& def_value, std::string& value)
{
	core->UserStats()->SetString(hash1, hash2, def_value.c_str(), value.c_str());
}

float ScriptModuleCore::UserStats::GetFloatHash(int hash1, int hash2, float def_value)
{
	return core->UserStats()->GetFloat(hash1, hash2, def_value);
}

int ScriptModuleCore::UserStats::GetIntHash(int hash1, int hash2, int def_value)
{
	return core->UserStats()->GetInt(hash1, hash2, def_value);
}

Vector ScriptModuleCore::UserStats::GetVectorHash(int hash1, int hash2, Vector& def_value)
{
	return core->UserStats()->GetVector(hash1, hash2, def_value);
}

void ScriptModuleCore::UserStats::GetStringHash(int hash1, int hash2, std::string& def_value, std::string& value)
{
	value = core->UserStats()->GetString(hash1, hash2, def_value.c_str());
}

void ScriptModuleCore::UserStats::GetHashes(std::string& id, int& hash1, int& hash2)
{
	core->UserStats()->GetHashes(id.c_str(), hash1, hash2);
}

bool ScriptModuleCore::UserStats::Load()
{
	return core->UserStats()->Load();
}

void ScriptModuleCore::UserStats::Save()
{
	core->UserStats()->Save();
}

float ScriptModuleCore::Utils::Rnd()
{
	return rnd();
}

void ScriptModuleCore::Utils::GetRayDir(Vector& pos, Vector& dir)
{	
	Vector2 ps(core->Controls()->GetTouchX(0)/en_core.Render()->Display()->GetWidth(),
			   core->Controls()->GetTouchY(0)/en_core.Render()->Display()->GetHeight());

	en_core.Render()->Display()->TransformScreenToSpace(ps, 100.0f,pos, dir);
}

void ScriptModuleCore::Utils::GetScreenPos(Vector& pos, Vector& screen_pos)
{
	screen_pos = en_core.Render()->Display()->TransformToScreen(pos, 2);
}

void ScriptModuleCore::Utils::GetCamDir(Vector& dir)
{
	Matrix view;
	en_core.Render()->Display()->GetView(view);
	view.Inverse();
	dir = view.Vz();
}

float ScriptModuleCore::Utils::GetHour()
{
#ifdef EDITOR
    if(bTimeSimulation)return nHour;
#endif
	return GetTime(0);
}

float ScriptModuleCore::Utils::GetMinute()
{
#ifdef EDITOR
    if(bTimeSimulation)return nMinute;
#endif
	return GetTime(1);
}

float ScriptModuleCore::Utils::GetSecond()
{	
	return GetTime(2);
}

float ScriptModuleCore::Utils::GetDay()
{	
#ifdef EDITOR
    if(bTimeSimulation)return nDay;
#endif
	return GetTime(3);
}

float ScriptModuleCore::Utils::GetMonth()
{
#ifdef EDITOR
    if(bTimeSimulation)return nMonth;
#endif
	return GetTime(4);
}

float ScriptModuleCore::Utils::GetYear()
{	
#ifdef EDITOR
    if(bTimeSimulation)return nYear;
#endif
	return GetTime(5);
}

float ScriptModuleCore::Utils::GetFps()
{
	return core->GetFPS();
}

int ScriptModuleCore::Utils::CalcUtf8Len(std::string& text)
{
	return ::CalcUtf8Len(text.c_str());
}

void ScriptModuleCore::Utils::ClampUtf8(string& text, string& res, int clamp_len)
{
	res = text;
	::ClampUtf8(res, clamp_len);
}

int64 ScriptModuleCore::Utils::GetSystemTime()
{
#ifdef EDITOR
    if(bTimeSimulation)return GetTimestamp(nYear,nMonth,nDay,nHour,nMinute,0);
#endif
	time_t ltime;
	time( &ltime );

	return ltime;
}

int ScriptModuleCore::Utils::ParseUnixTime(int index, uint64 time)
{
	return GetTime(index, time);
}

void  ScriptModuleCore::Utils::GetDateTimeUTC(int& year, int& month, int& day, int& hour, int& min, int& sec)
{
#ifdef EDITOR
    if(bTimeSimulation)
    {
        year = nYear;
        month = nMonth;
        day = nDay;
        hour = nHour;
        min = nMinute;
        sec = 0;
    }
#endif
	year = GetTimeUTC(5);
	month = GetTimeUTC(4);
	day = GetTimeUTC(3);
	hour = GetTimeUTC(0);
	min = GetTimeUTC(1);
	sec = GetTimeUTC(2);
}

int64 ScriptModuleCore::Utils::GetTimestamp(int year, int month, int day, int hour, int minute, int second)
{
	return ::GetTimestamp(year, month, day, hour, minute, second);
}

int ScriptModuleCore::Utils::GetWeekOfDay(int day, int month, int year)
{
	int a = (14 - month) / 12;
	int y = year - a;
	int m = month + 12 * a - 2;

	int week_day = (7000 + (day + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12)) % 7 - 1;
	if (week_day < 0) week_day = 6;

	return week_day;
}

void ScriptModuleCore::Utils::Int64ToString(int64 val, std::string& str)
{
	char buff[128];
	String::Printf(buff, 128, "%lli", val);
	str = buff;
}

void ScriptModuleCore::Utils::UInt64ToString(uint64 val, std::string& str)
{
	char buff[128];
	String::Printf(buff, 128, "%llu", val);
	str = buff;

}

void ScriptModuleCore::Utils::ConvertASCIIToUTF8(std::string& ascii_str, std::string& str)
{
	String::ASCIItoUtf8(str, ascii_str.c_str());
}

void ScriptModuleCore::Utils::GetMD5Hash(std::string& str, std::string& hash)
{
	char md5[48];
	::GetMD5Hash(str.c_str(), md5);

	hash = md5;
}

void ScriptModuleCore::Utils::GetUID(std::string& postfix, std::string& uid)
{
	//time_t t;
	//time(&t);

	//uid = iso8601_datetime(t) + "_";
	//for (int i = 0; i < uid.size(); ++i)
	//{
	//	//':' not allowed in querry params
	//	if (uid[i] == ':')
	//		uid[i] = '-';
	//}
	
	uid = "";
	//from '0' to '9'
	uid += (char)( Rnd() * 9 + 48);
	uid += (char)( Rnd() * 9 + 48);
	uid += (char)( Rnd() * 9 + 48);
	uid += (char)( Rnd() * 9 + 48);
	uid += string("_") + postfix; 
}

void ScriptModuleCore::Utils::GetISO8601DateTime(std::string& str)
{
	time_t t;
	time(&t);
	
	Get_ISO8601_datetime(t, str);	
}

void ScriptModuleCore::Utils::GetISO8601DateTimeForAzureQuerry(std::string& str)
{
	time_t t;
	time(&t);

	Get_ISO8601_datetime(t, str);

	for (int i = 0; i < str.size(); ++i)
	{
		//':' not allowed in querry params
		if (str[i] == ':')
			str[i] = '-';
	}
}

void ScriptModuleCore::Utils::ExtractFileName(std::string& FullPath, std::string& FileName)
{
	char buffer[256];
	String::ExtractFileName(FullPath.c_str(), buffer);
	FileName = buffer;
}

void ScriptModuleCore::Utils::PrintCallStack()
{	
	asIScriptContext* context = ((ScriptMachine*)core->Script()->Machine())->ctxs[((ScriptMachine*)core->Script()->Machine())->cur_context-1];	
	
	core->TraceTo("Script", "Call stack:");

	// Show the call stack
	for( asUINT n = 0; n < context->GetCallstackSize(); n++ )
	{
		asIScriptFunction *func;
		const char *scriptSection;
		int line, column;
		func = context->GetFunction(n);
		line = context->GetLineNumber(n, &column, &scriptSection);
		core->TraceTo("Script", "%s:%s:%d,%d\n", scriptSection, func->GetDeclaration(), line, column);
	}
}

void ScriptModuleCore::Utils::CopyFileScript(std::string& scr, std::string& dest)
{
	IFileBuffer* file_src = core->Files()->LoadFile(scr.c_str());

	if (!file_src)
	{
		return;
	}

	IFile* file_dest = core->Files()->OpenFile(dest.c_str(), IFile::write);

	if (!file_dest)
	{
		return;
	}

	file_dest->Write(file_src->GetBuffer(), file_src->GetSize());

	file_src->Release();
	file_dest->Release();
}

void ScriptModuleCore::Utils::OpenURL(std::string& url, bool inner)
{
#ifdef PC
	ShellExecute(null, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);	
#endif

#ifdef ANDROID
	if (inner)
	{
		CallFromJava("SetStopURL", "");
		CallFromJava("OpenURLInner", url.c_str());
	}
	else
	{		
		CallFromJava("OpenURL", url.c_str());
	}
#endif

#ifdef IOS
	ios_wrapper->OpenURL(url.c_str(), inner);    
#endif
}

void ScriptModuleCore::Utils::OpenURL2(std::string& url, std::string& stop_url)
{
#ifdef PC
	ShellExecute(null, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);	
#endif

#ifdef ANDROID
	CallFromJava("SetStopURL", stop_url.c_str());
	CallFromJava("OpenURLInner", url.c_str());	
#endif

#ifdef IOS
	ios_wrapper->OpenURL(url.c_str(), stop_url.c_str());    
#endif
}

void ScriptModuleCore::Utils::OpenVideo2params(std::string& path, std::string& showName)
{
	en_core.Player()->OpenVideo(path.c_str(), showName.c_str());
}
void ScriptModuleCore::Utils::OpenVideo1param(std::string& path)
{
	en_core.Player()->OpenVideo(path.c_str());
}

void ScriptModuleCore::Utils::StartMonitoringRegion(std::string& id, std::string& uuid)
{
#ifdef IOS
	ios_wrapper->StartMonitoringRegion(id.c_str(), uuid.c_str());
#endif
}

void ScriptModuleCore::Utils::StopMonitoringRegion(std::string& id)
{
#ifdef IOS
    ios_wrapper->StopMonitoringRegion(id.c_str());    
#endif
}

void ScriptModuleCore::Utils::SetCallbackOnEnterRegion(std::string& id, std::string& func)
{
#ifdef IOS
	ios_wrapper->SetCallbackOnEnterRegion(id.c_str(), func.c_str());    
#endif
}

void ScriptModuleCore::Utils::SetCallbackOnExitRegion(std::string& id, std::string& func)
{
#ifdef IOS
	ios_wrapper->SetCallbackOnExitRegion(id.c_str(), func.c_str());    
#endif
}

int ScriptModuleCore::Utils::GetRegionBeaconsCount(std::string& id)
{
#ifdef IOS
	return ios_wrapper->GetRegionBeaconsCount(id.c_str());    
#endif
    
	return 0;
}

int ScriptModuleCore::Utils::GetRegionBeaconMinor(std::string& id, int index)
{
#ifdef IOS
	return ios_wrapper->GetRegionBeaconMinor(id.c_str(), index);    
#endif
    
	return 0;
}

int ScriptModuleCore::Utils::GetRegionBeaconMajor(std::string& id, int index)
{
#ifdef IOS
	return ios_wrapper->GetRegionBeaconMajor(id.c_str(), index);    
#endif
    
	return 0;
}

int ScriptModuleCore::Utils::GetRegionBeaconProximity(std::string& id, int index)
{
#ifdef IOS
	return ios_wrapper->GetRegionBeaconProximity(id.c_str(), index);    
#endif
    
	return 0;
}

float ScriptModuleCore::Utils::GetRegionBeaconAccuracy(std::string& id, int index)
{
#ifdef IOS
	return ios_wrapper->GetRegionBeaconAccuracy(id.c_str(), index);    
#endif
    
	return 0.0f;
}

float ScriptModuleCore::Utils::QuadraticEaseIn(float p)
{
	return EasyInOut::QuadraticEaseIn(p);
}

float ScriptModuleCore::Utils::QuadraticEaseOut(float p)
{
	return EasyInOut::QuadraticEaseOut(p);
}

float ScriptModuleCore::Utils::QuadraticEaseInOut(float p)
{
	return EasyInOut::QuadraticEaseInOut(p);
}

float ScriptModuleCore::Utils::CubicEaseIn(float p)
{
	return EasyInOut::CubicEaseIn(p);
}

float ScriptModuleCore::Utils::CubicEaseOut(float p)
{
	return EasyInOut::CubicEaseOut(p);
}

float ScriptModuleCore::Utils::CubicEaseInOut(float p)
{
	return EasyInOut::CubicEaseInOut(p);
}

float ScriptModuleCore::Utils::QuarticEaseIn(float p)
{
	return EasyInOut::QuarticEaseIn(p);
}

float ScriptModuleCore::Utils::QuarticEaseOut(float p)
{
	return EasyInOut::QuarticEaseOut(p);
}

float ScriptModuleCore::Utils::QuarticEaseInOut(float p)
{
	return EasyInOut::QuarticEaseInOut(p);
}

float ScriptModuleCore::Utils::QuinticEaseIn(float p)
{
	return EasyInOut::QuinticEaseIn(p);
}

float ScriptModuleCore::Utils::QuinticEaseOut(float p)
{
	return EasyInOut::QuinticEaseOut(p);
}

float ScriptModuleCore::Utils::QuinticEaseInOut(float p)
{
	return EasyInOut::QuinticEaseInOut(p);
}

float ScriptModuleCore::Utils::SineEaseIn(float p)
{
	return EasyInOut::SineEaseIn(p);
}

float ScriptModuleCore::Utils::SineEaseOut(float p)
{
	return EasyInOut::SineEaseOut(p);
}

float ScriptModuleCore::Utils::SineEaseInOut(float p)
{
	return EasyInOut::SineEaseInOut(p);
}

float ScriptModuleCore::Utils::CircularEaseIn(float p)
{
	return EasyInOut::CircularEaseIn(p);
}

float ScriptModuleCore::Utils::CircularEaseOut(float p)
{
	return EasyInOut::CircularEaseOut(p);
}

float ScriptModuleCore::Utils::CircularEaseInOut(float p)
{
	return EasyInOut::CircularEaseInOut(p);
}

float ScriptModuleCore::Utils::ExponentialEaseIn(float p)
{
	return EasyInOut::ExponentialEaseIn(p);
}

float ScriptModuleCore::Utils::ExponentialEaseOut(float p)
{
	return EasyInOut::ExponentialEaseOut(p);
}

float ScriptModuleCore::Utils::ExponentialEaseInOut(float p)
{
	return EasyInOut::ExponentialEaseInOut(p);
}

float ScriptModuleCore::Utils::ElasticEaseIn(float p)
{
	return EasyInOut::ElasticEaseIn(p);
}

float ScriptModuleCore::Utils::ElasticEaseOut(float p)
{
	return EasyInOut::ElasticEaseOut(p);
}

float ScriptModuleCore::Utils::ElasticEaseInOut(float p)
{
	return EasyInOut::ElasticEaseInOut(p);
}

float ScriptModuleCore::Utils::BackEaseIn(float p)
{
	return EasyInOut::BackEaseIn(p);
}

float ScriptModuleCore::Utils::BackEaseOut(float p)
{
	return EasyInOut::BackEaseOut(p);
}

float ScriptModuleCore::Utils::BackEaseInOut(float p)
{
	return EasyInOut::BackEaseInOut(p);
}

float ScriptModuleCore::Utils::BounceEaseIn(float p)
{
	return EasyInOut::BounceEaseIn(p);
}

float ScriptModuleCore::Utils::BounceEaseOut(float p)
{
	return EasyInOut::BounceEaseOut(p);
}

float ScriptModuleCore::Utils::BounceEaseInOut(float p)
{
	return EasyInOut::BounceEaseInOut(p);
}

void ScriptModuleCore::Utils::HTMLToText(string& in, string& out)
{
	HTML2Text(in ,out);
}

void ScriptModuleCore::Utils::Work()
{
}

void ScriptModuleCore::Utils::UpperCase(string& in, string& out)
{
	out = in;
	String::UpperCase(out);
}
void ScriptModuleCore::Utils::LowerCase(string& in, string& out)
{
	out = in;
    String::LowerCase(out);
}

void ScriptModuleCore::Utils::Replace(string& str,string& find,string& replace)
{
	String::ReplaceAll(str,find.c_str(),replace.c_str());
}


void ScriptModuleCore::Utils::PrintTextOnPhoto(std::string& text, std::string& input_photo,  std::string& output_photo, std::string& output_ext)
{
#ifdef PC
	//"Bin/nconvert.exe"-overwrite  -text oleg -text_font Arrial 12 -text_color 1 1 1  -text_back 0 0 0 -text_flag center -out  png -o new 1.png
	string cmd = " -overwrite ";
	cmd += string(" -text ") + text +  " -text_font Arrial 12 -text_color 0 255 0  -text_back 0 1 0 -text_flag bottom-left -text_pos 0 50  -text_rotation 0"; 
	cmd +=	string(" -out ") + output_ext;
	cmd += string(" -o ") + output_photo + " ";
	cmd += input_photo;


	HINSTANCE code = ShellExecute(null, null, "Bin\\nconvert.exe", cmd.c_str(), NULL, SW_SHOWNORMAL);
	ThreadWorker::Sleep(1000);

#endif
}
#ifdef EDITOR
void ScriptModuleCore::Utils::SetSimulationTime(float year, float month, float day, float hour, float minute)
{
    nYear = year;
    nMonth = month;
    nDay = day;
    nHour = hour;
    nMinute = minute;
}
#endif
ScriptModuleCore::DataFile::DataFile()
{
	loader = 0;
}

bool ScriptModuleCore::DataFile::Start(std::string& name, std::string& start_block)
{
	if (loader)
	{
		core->TraceTo("Script", "Warning: previous DataFile was not freed");
		RELEASE(loader)
	}


	/*char str[256];

	String::Copy(str, 256, name.c_str());
	str[strlen(str)-1] = 'b';

	loader = core->Files()->CreateStreamLoader(str, start_block.c_str(), StreamLoader::binary);*/	
		
#ifdef EDITOR
	loader = core->Files()->CreateStreamLoader(name.c_str(), start_block.c_str(), IStreamLoader::xml);
#else
	loader = core->Files()->CreateStreamLoader(name.c_str(), start_block.c_str(), IStreamLoader::binary);
#endif		

#ifdef EDITOR	
	char str[256];

	String::Copy(str, 256, name.c_str());
	str[strlen(str)-1] = 'b';

	//loader = core->Files()->CreateStreamLoader(str, start_block.c_str(), StreamLoader::binary);

	if (loader)
	{
		loader->InitSaver(str);
	}
#endif	

	if (loader) return true;

	return false;
}

bool ScriptModuleCore::DataFile::StartEx(std::string& name, std::string& start_block, int file_type)
{
	if (loader)
	{
		core->TraceTo("Script", "Warning: previous DataFile was not freed");
		RELEASE(loader)
	}	
		
	loader = core->Files()->CreateStreamLoader(name.c_str(), start_block.c_str(), (IStreamLoader::FileType)file_type);
	
#ifdef EDITOR
	if (file_type == IStreamLoader::binary)
	{
		char str[256];

		String::Copy(str, 256, name.c_str());
		str[strlen(str)-1] = 'b';

		if (loader) loader->InitSaver(str);
	}	
#endif

	if (loader) return true;

	return false;
}

bool ScriptModuleCore::DataFile::StartFromRawData(std::string& buffer, std::string& start_block, int file_type)
{
    if (loader)
    {
        core->TraceTo("Script", "Warning: previous DataFile was not freed");
        RELEASE(loader)
    }	
    loader = core->Files()->CreateStreamLoader(buffer.c_str(), start_block.c_str(), (IStreamLoader::FileType)file_type,true);
    if (loader) return true;
    return false;
}

void ScriptModuleCore::DataFile::Finish()
{
	RELEASE(loader)
}

bool ScriptModuleCore::DataFile::EnterBlock(std::string& name)
{
	if (!loader) return false;

	return loader->EnterBlock(name.c_str());
}

void ScriptModuleCore::DataFile::LeaveBlock()
{
	if (loader) loader->LeaveBlock();
}

bool ScriptModuleCore::DataFile::ReadString(std::string& name, std::string& val)
{
	if (!loader) return false;

	int sz = loader->GetStringSize(name.c_str());

	char* txt = new char[sz + 10];
	txt[0] = 0;

	bool res = loader->Read(name.c_str(), txt, sz + 5);

	val = txt;

	delete[] txt;

	return res;
}

bool ScriptModuleCore::DataFile::ReadBool(std::string& name, bool& val)
{
	if (!loader) return false;

	return loader->Read(name.c_str(), val);
}

bool ScriptModuleCore::DataFile::ReadFloat(std::string& name, float& val)
{
	if (!loader) return false;

	return loader->Read(name.c_str(), val);
}

bool ScriptModuleCore::DataFile::ReadInt(std::string& name, int& val)
{
	if (!loader) return false;

	return loader->Read(name.c_str(), val);
}

bool ScriptModuleCore::DataFile::ReadInt64(std::string& name, int64& val)
{
	if (!loader) return false;

	return loader->Read(name.c_str(), val);
}

bool ScriptModuleCore::DataFile::ReadUInt(std::string& name, dword& val)
{
	if (!loader) return false;

	return loader->Read(name.c_str(), val);
}

bool ScriptModuleCore::DataFile::ReadUInt64(std::string& name, uint64& val)
{
	if (!loader) return false;

	return loader->Read(name.c_str(), val);
}

bool ScriptModuleCore::DataFile::ReadVector(std::string& name, Vector& val)
{
	if (!loader) return false;

	return loader->Read(name.c_str(), val);
}

void ScriptModuleCore::Facebook::ReqListiner::OnRequestFinish(int res, void* data, const char* out_file)
{		
	core->Script()->Machine()->AddIntArg(res);
	core->Script()->Machine()->AddStrArg(out_file);

	if (param[0])
	{
		core->Script()->Machine()->AddStrArg(param);
	}

	core->Script()->Machine()->Call(&callback);		

	used = false;
}

void ScriptModuleCore::Facebook::SigoutListiner::OnFinish(void* data)
{	
	core->Script()->Machine()->Call(&callback);		
}

ScriptModuleCore::Facebook::ReqListiner* ScriptModuleCore::Facebook::GetLisiten(asIScriptFunction* cb)
{
	ReqListiner* listiner = null;

	for (int i=0;i<listiners.size();i++)
	{
		if (!listiners[i].used)
		{
			listiner = &listiners[i];
			break;
		}
	}

	if (!listiner)
	{
		listiners.push_back(ReqListiner());

		listiner = &listiners[listiners.size()-1];
	}

	listiner->used = true;
	listiner->callback.Set(cb);			
	listiner->param[0] = 0;

	return listiner;
}


void ScriptModuleCore::Facebook::SignIn(asIScriptFunction* cb)
{
	ReqListiner* listiner = GetLisiten(cb);	

	en_core.OnlineUserStats()->Facebook()->SignIn(listiner, null);
}

void ScriptModuleCore::Facebook::AddRequestPermistion(std::string& name)
{
	en_core.OnlineUserStats()->Facebook()->AddRequestPermistion(name.c_str());
}

void ScriptModuleCore::Facebook::StartRequestPermistions(asIScriptFunction* cb)
{
	ReqListiner* listiner = GetLisiten(cb);	

	en_core.OnlineUserStats()->Facebook()->StartRequestPermistions(listiner, null);
}

void ScriptModuleCore::Facebook::SignOut(asIScriptFunction* cb)
{
	sign_out_listiner.callback.Set(cb);
	en_core.OnlineUserStats()->Facebook()->SignOut(&sign_out_listiner, null);
}

bool ScriptModuleCore::Facebook::GetSignInStatus()
{
	return en_core.OnlineUserStats()->Facebook()->GetSignInStatus();
}

void ScriptModuleCore::Facebook::FBStartRequest(std::string& method, std::string& url, std::string& params, std::string& post_params, asIScriptFunction* cb)
{
	StartRequestInner(method, url, params, post_params, cb, null);
}

void ScriptModuleCore::Facebook::FBStartRequest2(std::string& method, std::string& url, std::string& params, std::string& post_params, asIScriptFunction* cb, std::string& req_param)
{
	StartRequestInner(method, url, params, post_params, cb, req_param.c_str());
}

void ScriptModuleCore::Facebook::StartRequestInner(std::string& method, std::string& url, std::string& params, std::string& post_params, asIScriptFunction* cb, const char* req_param)
{
	ReqListiner* listiner = GetLisiten(cb);	

	const char* pst_params = null;	

	if (post_params.size()>0)
	{				
		pst_params = post_params.c_str();
	}
	
	if (req_param)
	{
		String::Copy(listiner->param, 128, req_param);
	}
	else
	{
		listiner->param[0] = 0;
	}

	en_core.OnlineUserStats()->Facebook()->StartRequest(method.c_str(), url.c_str(), params.c_str(), pst_params, listiner, null);	
}

void ScriptModuleCore::Facebook::UploadFile(std::string& url, std::string& file_name, std::string& param_name, std::string& message, asIScriptFunction* cb)
{
	ReqListiner* listiner = GetLisiten(cb);	
		
	en_core.OnlineUserStats()->Facebook()->UploadFile(url.c_str(), file_name.c_str(), param_name.c_str(), message.c_str(), listiner, null);	
}

void ScriptModuleCore::Facebook::SetGuestAccessTokenURL(std::string& url)
{
	en_core.OnlineUserStats()->Facebook()->SetGuestAccessTokenURL(url.c_str());
}

void ScriptModuleCore::Facebook::SetApiURL(std::string& url)
{
	en_core.OnlineUserStats()->Facebook()->SetApiURL(url.c_str());
}

void ScriptModuleCore::Facebook::Reset()
{
	for (int i=0;i<listiners.size();i++)
	{
		listiners[i].callback.Reset();
	}

	sign_out_listiner.callback.Reset();
}

void ScriptModuleCore::VK::ReqListiner::OnRequestFinish(int res, void* data, const char* out_file)
{	
	core->Script()->Machine()->AddIntArg(res);
	core->Script()->Machine()->AddStrArg(out_file);

	if (param[0])
	{
		core->Script()->Machine()->AddStrArg(param);		
	}

	core->Script()->Machine()->Call(&callback);	
	
	used = false;
}

void ScriptModuleCore::VK::SigoutListiner::OnFinish(void* data)
{	
	core->Script()->Machine()->Call(&callback);		
}

ScriptModuleCore::VK::ReqListiner* ScriptModuleCore::VK::GetLisiten(asIScriptFunction* cb)
{
	ReqListiner* listiner = null;

	for (int i=0;i<listiners.size();i++)
	{
		if (!listiners[i].used)
		{
			listiner = &listiners[i];
			break;
		}
	}

	if (!listiner)
	{
		listiners.push_back(ReqListiner());

		listiner = &listiners[listiners.size()-1];
	}

	listiner->used = true;
	listiner->callback.Set(cb);	
	listiner->param[0] = 0;

	return listiner;
}


void ScriptModuleCore::VK::VkSignIn(asIScriptFunction* cb)
{
	ReqListiner* listiner = GetLisiten(cb);	

	en_core.OnlineUserStats()->VK()->SignIn(listiner, null);
}

void ScriptModuleCore::VK::VkSignOut(asIScriptFunction* cb)
{
	sign_out_listiner.callback.Set(cb);
	en_core.OnlineUserStats()->VK()->SignOut(&sign_out_listiner, null);	
}

void ScriptModuleCore::VK::VkGetUserID(std::string& user_id)
{
	user_id = en_core.OnlineUserStats()->VK()->GetUserID();
}

int ScriptModuleCore::VK::VkGetSignInStatus()
{
	return en_core.OnlineUserStats()->VK()->GetSignInStatus();
}

void ScriptModuleCore::VK::VkStartRequest(std::string& url, std::string& params, std::string& post_params, asIScriptFunction* cb)
{
	StartRequestInner(url, params, post_params, cb, null);
}

void ScriptModuleCore::VK::VkStartRequest2(std::string& url, std::string& params, std::string& post_params, asIScriptFunction* cb, std::string& req_param)
{
	StartRequestInner(url, params, post_params, cb, req_param.c_str());
}

void ScriptModuleCore::VK::VkSetApiURL(std::string& url)
{
	en_core.OnlineUserStats()->VK()->SetApiURL(url.c_str());
}

void ScriptModuleCore::VK::Reset()
{
	for (int i=0;i<listiners.size();i++)
	{
		listiners[i].callback.Reset();
	}

	sign_out_listiner.callback.Reset();
}

void ScriptModuleCore::VK::StartRequestInner(std::string& url, std::string& params, std::string& post_params, asIScriptFunction* cb, const char* req_param)
{
	ReqListiner* listiner = GetLisiten(cb);	

	const char* pst_params = null;

	if (post_params.size()>0)
	{		
		pst_params = post_params.c_str();
	}

	if (req_param)
	{
		String::Copy(listiner->param, 128, req_param);
	}
	else
	{
		listiner->param[0] = 0;
	}

	en_core.OnlineUserStats()->VK()->StartRequest(url.c_str(), params.c_str(), pst_params, listiner, null);	
}

void ScriptModuleCore::VK::VKUploadFile(std::string& url, std::string& file_name, asIScriptFunction* cb)
{
	ReqListiner* listiner = GetLisiten(cb);	

	listiner->param[0] = 0;	

	en_core.OnlineUserStats()->VK()->UploadFile(url.c_str(), file_name.c_str(), listiner, null);	
}

void ScriptModuleCore::Instagram::SigInOutListiner::OnFinish(void* data)
{	
	core->Script()->Machine()->Call(&callback);		
}

void ScriptModuleCore::Instagram::InstagramSignIn(asIScriptFunction* cb)
{
	sign_in_listiner.callback.Set(cb);
	en_core.OnlineUserStats()->Instagram()->SignIn(&sign_in_listiner, null);	
}

void ScriptModuleCore::Instagram::InstagramSignOut(asIScriptFunction* cb)
{
	sign_out_listiner.callback.Set(cb);
	en_core.OnlineUserStats()->Instagram()->SignOut(&sign_out_listiner, null);	
}

void ScriptModuleCore::Instagram::InstagramGetAccessToken(std::string& token)
{	
	token = en_core.OnlineUserStats()->Instagram()->GetAccessToken();	
}

void ScriptModuleCore::Instagram::Reset()
{
	sign_in_listiner.callback.Reset();
	sign_out_listiner.callback.Reset();
}

void ScriptModuleCore::OnlineUserStats::TrackUserAge(int age)
{
	en_core.OnlineUserStats()->TrackUserAge(age);
}

void ScriptModuleCore::OnlineUserStats::TrackUserGender(std::string& gender)
{
	en_core.OnlineUserStats()->TrackUserGender(gender.c_str());
}

void ScriptModuleCore::OnlineUserStats::AddEventParam(std::string& name, std::string& param)
{
    en_core.OnlineUserStats()->AddEventParam(name.c_str(), param.c_str());
}

void ScriptModuleCore::OnlineUserStats::TrackEvent(std::string& name)
{
	en_core.OnlineUserStats()->TrackEvent(name.c_str());
}

ScriptModuleCore::StreamSaver::StreamSaver()
{
	saver = 0;
}

bool ScriptModuleCore::StreamSaver::StartSS(std::string& name, int file_type)
{
	if (saver)
	{
		core->TraceTo("Script", "Warning: previous StreamSaver was not freed");
		RELEASE(saver)
	}
		
	saver = core->Files()->CreateStreamSaver(name.c_str(), (IStreamSaver::FileType)file_type);


	if (saver) return true;

	return false;
}


void ScriptModuleCore::StreamSaver::FinishSS()
{
	RELEASE(saver)
}

void ScriptModuleCore::StreamSaver::EnterBlockSS(std::string& name)
{
	saver->MarkBeginBlock(name.c_str());
}

void ScriptModuleCore::StreamSaver::LeaveBlockSS(std::string& name)
{
	saver->MarkEndBlock(name.c_str());
}

void ScriptModuleCore::StreamSaver::WriteString(std::string& name, std::string& val)
{
	saver->Write(name.c_str(), val.c_str());
}

void ScriptModuleCore::StreamSaver::WriteBool(std::string& name, bool& val)
{
	saver->Write(name.c_str(), val);
}

void ScriptModuleCore::StreamSaver::WriteFloat(std::string& name, float& val)
{
	saver->Write(name.c_str(), val);
}

void ScriptModuleCore::StreamSaver::WriteInt(std::string& name, int& val)
{
	saver->Write(name.c_str(), val);
}

void ScriptModuleCore::StreamSaver::WriteInt64(std::string& name, int64& val)
{
	saver->Write(name.c_str(), val);
}

void ScriptModuleCore::StreamSaver::WriteVector(std::string& name, Vector& val)
{
	saver->Write(name.c_str(), val);
}

////////////////////////////////////////////////////////////////////////// MediaLibrary
void ScriptModuleCore::MediaLibrary::MLFinishListiner::OnFinish(void* data, int res)
{
	core->Script()->Machine()->AddIntArg(res);
	core->Script()->Machine()->Call(&callback);	
}

void ScriptModuleCore::MediaLibrary::StartScanning(asIScriptFunction* cb)
{
	listiner.callback.Set(cb);
	en_core.MediaLibrary()->StartScanning(&listiner, null);
}

int ScriptModuleCore::MediaLibrary::GetFilesCount()
{
	return en_core.MediaLibrary()->GetFilesCount();
}

void ScriptModuleCore::MediaLibrary::GetFileName(int i, std::string& name, bool prepareForLoading)
{
	en_core.MediaLibrary()->GetFileName(i, name, prepareForLoading);
}

void ScriptModuleCore::MediaLibrary::GetFileNameThumbnail(int i, std::string& name, bool prepareForLoading)
{
	en_core.MediaLibrary()->GetFileNameThumbnail(i, name, prepareForLoading);
}

int ScriptModuleCore::MediaLibrary::GetYearCount()
{
	return en_core.MediaLibrary()->GetYearCount();
}

int ScriptModuleCore::MediaLibrary::GetMonthCount(int year)
{
	return en_core.MediaLibrary()->GetMonthCount(year);
}
int ScriptModuleCore::MediaLibrary::GetDayCount(int year, int month)
{
	return en_core.MediaLibrary()->GetDayCount(year, month);
}
int ScriptModuleCore::MediaLibrary::GetImageCount(int year, int month, int day)
{
	return en_core.MediaLibrary()->GetImageCount(year, month, day);
}

int ScriptModuleCore::MediaLibrary::GetIndexByYear(int year)
{
	return en_core.MediaLibrary()->GetIndexByYear(year);
}
int ScriptModuleCore::MediaLibrary::GetIndexByMonth(int year, int month)
{
	return en_core.MediaLibrary()->GetIndexByMonth(year, month);
}
int ScriptModuleCore::MediaLibrary::GetIndexByDay(int year, int month, int day)
{
	return en_core.MediaLibrary()->GetIndexByDay(year, month, day);
}

bool ScriptModuleCore::MediaLibrary::GetDate(int index, int& day, int& month, int& year)
{
	return en_core.MediaLibrary()->GetDate(index, day, month, year);
}

bool ScriptModuleCore::MediaLibrary::GetTime(int index, int& hour, int& minute, int& second)
{
	return en_core.MediaLibrary()->GetTime(index, hour, minute, second);
}

void ScriptModuleCore::MediaLibrary::Reset()
{
	listiner.callback.Reset();
}

void ScriptModuleCore::Network::ReqListiner::OnRequestFinish(int res, void* data, const char* file_name)
{
	core->Script()->Machine()->AddIntArg(res);
	core->Script()->Machine()->AddStrArg(file_name);

	if (param[0])
	{
		core->Script()->Machine()->AddStrArg(param);		
	}

	core->Script()->Machine()->Call(&callback);			
	used = false;
}

bool ScriptModuleCore::Network::IsInternetConnected()
{
	return core->Network()->IsInternetConnected();
}

void ScriptModuleCore::Network::StartRequestInner(std::string& method, std::string& request, std::string& post_params, std::string& file_name, asIScriptFunction* cb, const char* req_param)
{
	ReqListiner* listiner = null;

	for (int i=0;i<listiners.size();i++)
	{
		if (!listiners[i].used)
		{
			listiner = &listiners[i];
			break;
		}
	}

	if (!listiner)
	{
		listiners.push_back(ReqListiner());

		listiner = &listiners[listiners.size()-1];
	}

	listiner->used = true;
	
	listiner->callback.Set(cb);

	if (req_param)
	{
		String::Copy(listiner->param, 128, req_param);
	}
	else
	{
		listiner->param[0] = 0;
	}

	core->Network()->StartRequest(listiner, null, method.c_str(), request.c_str(), post_params.c_str(), file_name.c_str());
}

void ScriptModuleCore::Network::NetStartRequest(std::string& method, std::string& request, std::string& post_params, std::string& file_name, asIScriptFunction* cb)
{
	StartRequestInner(method, request, post_params, file_name, cb, null);
}

void ScriptModuleCore::Network::NetStartRequest2(std::string& method, std::string& request, std::string& post_params, std::string& file_name, asIScriptFunction* cb, std::string& req_param)
{
	StartRequestInner(method, request, post_params, file_name, cb, req_param.c_str());
}

void ScriptModuleCore::Network::Reset()
{
	for (int i=0;i<listiners.size();i++)
	{
		listiners[i].used = false;
		listiners[i].callback.Reset();
	}
}

void ScriptModuleCore::Mobile::PushListener::OnGetUserData(void* data, const char* userdata)
{
	core->Script()->Machine()->AddStrArg(userdata);
	core->Script()->Machine()->Call(&callback);
	used = false;
}

void ScriptModuleCore::Mobile::SetPushListener(asIScriptFunction* cb)
{
	PushListener* listener = null;

	for (int i = 0, size = listeners.size(); i < size; ++i)
	{
		if (!listeners[i].used)
		{
			listener = &listeners[i];
			break;
		}
	}

	if (!listener)
	{
		listeners.push_back(PushListener());

		listener = &listeners[listeners.size() - 1];
	}

	listener->used = true;
	listener->callback.Set(cb);	

	en_core.Mobile()->AddPushListener(listener, null);
}

void ScriptModuleCore::Mobile::ReqListiner::OnRequestFinish(void* data, int res)
{
	core->Script()->Machine()->AddIntArg(res);
	core->Script()->Machine()->Call(&callback);	
}

void ScriptModuleCore::Mobile::ImageReqListiner::OnRequestFinish(void* data, int res, const char* image)
{
	core->Script()->Machine()->AddIntArg(res);
	core->Script()->Machine()->AddStrArg(image);
	core->Script()->Machine()->Call(&callback);	
}

void ScriptModuleCore::Mobile::PopupListiner::OnOkPressed(void* data)
{
	used = false;
	core->Script()->Machine()->Call(&callback_ok);	
}

void ScriptModuleCore::Mobile::PopupListiner::OnCancelPressed(void* data)
{
	used = false;
	core->Script()->Machine()->Call(&callback_cancel);	
}

void ScriptModuleCore::Mobile::SetOnResumeCallback(asIScriptFunction* cb)
{
	onresume_listiner.callback.Set(cb);	
	en_core.Mobile()->SetOnResumeListener(&onresume_listiner, null);
}

void ScriptModuleCore::Mobile::SetOnPauseCallback(asIScriptFunction* cb)
{
	onpause_listiner.callback.Set(cb);	
	en_core.Mobile()->SetOnPauseListener(&onpause_listiner, null);
}

bool ScriptModuleCore::Mobile::IsReadContactsAllowed()
{
	return en_core.Mobile()->IsReadContactsAllowed();
}

void ScriptModuleCore::Mobile::RequestAccessToContacts(asIScriptFunction* cb)
{
	req_access.callback.Set(cb);	
	en_core.Mobile()->RequestAccessToContacts(&req_access, null);
}

void ScriptModuleCore::Mobile::StartReadContacts(asIScriptFunction* cb)
{
	req_read_contacts.callback.Set(cb);	
	en_core.Mobile()->StartReadContacts(&req_read_contacts, null);
}

void ScriptModuleCore::Mobile::StartReadContactsAvatars(asIScriptFunction* cb)
{	
	req_read_avatars.callback.Set(cb);	
	en_core.Mobile()->StartReadContactsAvatars(&req_read_avatars, null);	
}

int ScriptModuleCore::Mobile::GetContactsCount()
{
	return en_core.Mobile()->GetContactsCount();
}

void ScriptModuleCore::Mobile::GetContactData(int i, std::string& name, std::string& texture, std::string& numbers)
{
	en_core.Mobile()->GetContactData(i, name, texture, numbers);
}

void ScriptModuleCore::Mobile::SendSms(std::string& number, std::string& text, asIScriptFunction* cb)
{	
	req_send_sms.callback.Set(cb);	
	en_core.Mobile()->SendSms(number.c_str(), text.c_str(), &req_send_sms, null);		
}

void ScriptModuleCore::Mobile::CallNumber(std::string& number)
{	
	en_core.Mobile()->CallNumber(number.c_str());
}

bool ScriptModuleCore::Mobile::IsAllowCall()
{
	return en_core.Mobile()->IsAllowCall();
}

void ScriptModuleCore::Mobile::GetSimOperatorName(std::string& name)
{
	name = en_core.Mobile()->GetSimOperatorName();
}

void ScriptModuleCore::Mobile::StartSelectImage(int type, asIScriptFunction* cb)
{	
	req_select_image.callback.Set(cb);	
	en_core.Mobile()->StartSelectImage(type, &req_select_image, null);
}

void ScriptModuleCore::Mobile::SetPushServerURL(std::string& url)
{	
#ifdef ANDROID	
	CallFromJava("SetPushServerURL", url.c_str());
#endif
    
#ifdef IOS
	ios_wrapper->SetPushServerURL(url.c_str());    
#endif
}

void ScriptModuleCore::Mobile::ShareListener::OnEvent(void* data, const char* share_type)
{			
	core->Script()->Machine()->AddStrArg(share_type);
	core->Script()->Machine()->Call(&callback);		
}

void ScriptModuleCore::Mobile::SystemListener::OnEvent(void* data)
{	
	core->Script()->Machine()->Call(&callback);	
}

void ScriptModuleCore::Mobile::ScanListener::OnEvent(void* data, const char* scaned_data)
{
	core->Script()->Machine()->AddStrArg(scaned_data);
	core->Script()->Machine()->Call(&callback);
}

void ScriptModuleCore::Mobile::GPSListiner::OnUpdate(void* data, int state, float latitude, float longitude)
{
	core->Script()->Machine()->AddIntArg(state);
	core->Script()->Machine()->AddFloatArg(latitude);
	core->Script()->Machine()->AddFloatArg(longitude);
	core->Script()->Machine()->Call(&callback);
}

void ScriptModuleCore::Mobile::StartRegisterInPushService()
{	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
#ifdef ANDROID	
    CallFromJava("StartRegisterInPushService");
#endif	
    
#ifdef IOS
	ios_wrapper->StartRegisterInPush();    
#endif
}

int ScriptModuleCore::Mobile::GetTittleBarHeight()
{
#ifdef IOS
	return ios_wrapper->GetTittleBarHeight();
#endif

	return 0;
}

void ScriptModuleCore::Mobile::SendMail(std::string& address, std::string& subject, std::string& body)
{	
#ifdef ANDROID
	std::string mail_data = address + "|<" + subject + "|<" + body;
	CallFromJava("SendMail", mail_data.c_str());
#endif

#ifdef IOS
	ios_wrapper->SendMail(address.c_str(), subject.c_str(), body.c_str());	
#endif
}

void ScriptModuleCore::Mobile::StartShareText(std::string& text, std::string& extra_data, asIScriptFunction* callback)
{
	share_listiner.callback.Set(callback);
	en_core.Mobile()->StartShareText(text.c_str(), extra_data.c_str(), &share_listiner, null);
}

void ScriptModuleCore::Mobile::StartShareImage(std::string& image, asIScriptFunction* callback)
{	
	share_listiner.callback.Set(callback);
	en_core.Mobile()->StartShareImage(image.c_str(), &share_listiner, null);
}

void ScriptModuleCore::Mobile::AddEventToCalendar(int day, int month, int year, int hour, int minute, int second,
											     std::string& name, std::string& desc, std::string& location)
{
#ifdef IOS

	ios_wrapper->AddEventToCalendar(day, month, year, hour, minute, second, name.c_str(), desc.c_str(), location.c_str());       
    
#endif
    
#ifdef ANDROID

	string xml;	
	char int_str[64];	
		
	sprintf(int_str, "%i", day);
	xml = xml + int_str + "|";

	sprintf(int_str, "%i", month);
	xml = xml + int_str + "|";

	sprintf(int_str, "%i", year);
	xml = xml + int_str + "|";

	sprintf(int_str, "%i", hour);
	xml = xml + int_str + "|";

	sprintf(int_str, "%i", minute);
	xml = xml + int_str + "|";

	sprintf(int_str, "%i", second);
	xml = xml + int_str + "|";

	xml = xml + name + "|";
	
	xml = xml + desc + "|";
	
	xml = xml + location + "|";

	CallFromJava("AddEventToCalendar", xml.c_str());
#endif
}

void ScriptModuleCore::Mobile::ShowMap(bool show)
{	
	en_core.Mobile()->ShowMap(show);
}
	
void ScriptModuleCore::Mobile::SetMapPosition(float x, float y, float w, float h)
{
	en_core.Mobile()->SetMapPosition(x, y, w, h);
}

void ScriptModuleCore::Mobile::AddObjectToMap(float x, float y, std::string& text)
{
	en_core.Mobile()->AddObjectToMap(x, y, text.c_str());
}

void ScriptModuleCore::Mobile::DeleteAllMapObjects()
{
	en_core.Mobile()->DeleteAllMapObjects();
}

void ScriptModuleCore::Mobile::ShowNativeMap(float x, float y, std::string& text)
{
	en_core.Mobile()->ShowNativeMap(x, y, text.c_str());
}

void ScriptModuleCore::Mobile::ShowPopup(std::string& title, std::string& text, std::string& btn_text, asIScriptFunction* callback_ok)
{
	if (popup_listiner.used)
	{
		core->TraceTo("script", "ERROR: Previous popup still showed");
		return;
	}

	popup_listiner.callback_ok.Set(callback_ok);
	popup_listiner.used = true;

	en_core.Mobile()->ShowPopup(title.c_str(), text.c_str(), btn_text.c_str(), &popup_listiner, null);
}

void ScriptModuleCore::Mobile::ShowPopup2(std::string& title, std::string& text, std::string& btn_ok_text, asIScriptFunction* callback_ok, std::string& btn_cancel_text, asIScriptFunction* callback_cancel)
{
	if (popup_listiner.used)
	{
		core->TraceTo("script", "ERROR: Previous popup still showed");
		return;
	}

	popup_listiner.callback_ok.Set(callback_ok);
	popup_listiner.callback_cancel.Set(callback_cancel);	
	popup_listiner.used = true;

	en_core.Mobile()->ShowPopup(title.c_str(), text.c_str(), btn_ok_text.c_str(), btn_cancel_text.c_str(), &popup_listiner, null);
}

float ScriptModuleCore::Mobile::GetBatteryLevel()
{
	return core->Mobile()->GetBatteryLevel();
}

int ScriptModuleCore::Mobile::GetBatteryStatus()
{
	return core->Mobile()->GetBatteryStatus();
}

void ScriptModuleCore::Mobile::GetInstallReferer(string& referer)
{
	referer = "unknown";
}

void ScriptModuleCore::Mobile::StartScanBarcode(asIScriptFunction* cb)
{
	scane_listiner.callback.Set(cb);
	en_core.Mobile()->StartScanBarcode(&scane_listiner, null);
}

void ScriptModuleCore::Mobile::GetCameraTextureSize(int& width, int& height)
{
	en_core.Mobile()->GetCameraTextureSize(width, height);
}

void ScriptModuleCore::Mobile::EnableTrackGPS(bool enable, asIScriptFunction* callback)
{	
	gps_listiner.callback.Set(callback);
	en_core.Mobile()->EnableTrackGPS(enable, &gps_listiner, null);
}

void ScriptModuleCore::Mobile::Reset()
{
	req_access.callback.Reset();
	req_read_contacts.callback.Reset();
	req_read_avatars.callback.Reset();
	req_send_sms.callback.Reset();
	popup_listiner.callback_ok.Reset();
	popup_listiner.callback_cancel.Reset();
	req_select_image.callback.Reset();
	share_listiner.callback.Reset();

	for (int i=0; i<listeners.size(); i++)
	{
		listeners[i].callback.Reset();
	}

	onpause_listiner.callback.Reset();	
	onresume_listiner.callback.Reset();	
}

CALLWRAPPERARG2(ScriptModuleCore::Controls, IsKey, CALLWRAPPERGETPARAM(int, arg1), CALLWRAPPERGETPARAM(int, arg2), CALLWRAPPERRET(bool))
CALLWRAPPERARG1(ScriptModuleCore::Controls, GetControlValue, CALLWRAPPERGETPARAM(int, arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG2(ScriptModuleCore::Controls, CheckTouch, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERRET(bool))
CALLWRAPPERARG1(ScriptModuleCore::Controls, GetTouchX, CALLWRAPPERGETPARAM(int, arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Controls, GetTouchY, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERRET(float))
CALLWRAPPER(ScriptModuleCore::Controls, IsAnyKeyPressed, CALLWRAPPERRET(bool))
CALLWRAPPER(ScriptModuleCore::Controls, HideSoftKeyboard, CALLWRAPPERNORET())
CALLWRAPPER(ScriptModuleCore::Controls, GetSoftKeyboardHeight, CALLWRAPPERRET(int))
CALLWRAPPER(ScriptModuleCore::Controls, IsShowSoftKeyboard, CALLWRAPPERRET(bool))
CALLWRAPPERARG1(ScriptModuleCore::Controls, GetGyro, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG2(ScriptModuleCore::Controls, SetGyro, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERNORET())
CALLWRAPPER(ScriptModuleCore::Controls, IsBackPressed, CALLWRAPPERRET(bool))

CALLWRAPPERARG1(ScriptModuleCore::Scene, LoadScene, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG1(ScriptModuleCore::Scene, UnloadScene, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG1(ScriptModuleCore::Scene, IsSceneLoading, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERRET(bool))
CALLWRAPPERARG1(ScriptModuleCore::Scene, GetLoadingProgress, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG3(ScriptModuleCore::Scene, ActivateInScene, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERGETPARAM(bool,arg3), CALLWRAPPERRET(bool))


CALLWRAPPER(ScriptModuleCore::Render, GetWidth, CALLWRAPPERRET(float))
CALLWRAPPER(ScriptModuleCore::Render, GetHeight, CALLWRAPPERRET(float))
CALLWRAPPER(ScriptModuleCore::Render, GetReferenceWidth, CALLWRAPPERRET(float))
CALLWRAPPER(ScriptModuleCore::Render, GetReferenceHeight, CALLWRAPPERRET(float))


CALLWRAPPERARG1(ScriptModuleCore::Render, HorzRefToScreen, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Render, VertRefToScreen, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Render, HorzScreenToRef, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Render, VertScreenToRef, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))

CALLWRAPPERARG3(ScriptModuleCore::Render, PrintText, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETADRESS(string,arg3), CALLWRAPPERNORET())
CALLWRAPPERARG4(ScriptModuleCore::Render, DrawLine2D, CALLWRAPPERGETADRESS(Vector2,arg1), CALLWRAPPERGETADRESS(Vector,arg2), CALLWRAPPERGETADRESS(Vector2,arg3), CALLWRAPPERGETADRESS(Vector,arg4), CALLWRAPPERNORET())
CALLWRAPPER(ScriptModuleCore::Render, GetScreenDPI, CALLWRAPPERRET(int))
CALLWRAPPERARG1(ScriptModuleCore::Render, MakeScreenshot, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())


CALLWRAPPERARG4(ScriptModuleCore::UserStats, SetFloat, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERGETPARAM(bool,arg4), CALLWRAPPERNORET())
CALLWRAPPERARG4(ScriptModuleCore::UserStats, SetInt, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETPARAM(int,arg3), CALLWRAPPERGETPARAM(bool,arg4), CALLWRAPPERNORET())
CALLWRAPPERARG3(ScriptModuleCore::UserStats, SetVector, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(Vector,arg2), CALLWRAPPERGETADRESS(Vector,arg3), CALLWRAPPERNORET())
CALLWRAPPERARG3(ScriptModuleCore::UserStats, SetString, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERGETADRESS(string,arg3), CALLWRAPPERNORET())

CALLWRAPPERARG2(ScriptModuleCore::UserStats, GetFloat, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERRET(float))
CALLWRAPPERARG2(ScriptModuleCore::UserStats, GetInt, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERRET(int))
CALLWRAPPERARG2(ScriptModuleCore::UserStats, GetVector, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(Vector,arg2), CALLWRAPPERRET(Vector))
CALLWRAPPERARG3(ScriptModuleCore::UserStats, GetString, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERGETADRESS(string,arg3), CALLWRAPPERNORET())

CALLWRAPPERARG5(ScriptModuleCore::UserStats, SetFloatHash, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETPARAM(int,arg3), CALLWRAPPERGETPARAM(int,arg4), CALLWRAPPERGETPARAM(int,arg5), CALLWRAPPERNORET())
CALLWRAPPERARG5(ScriptModuleCore::UserStats, SetIntHash, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETPARAM(int,arg3), CALLWRAPPERGETPARAM(int,arg4), CALLWRAPPERGETPARAM(bool,arg5), CALLWRAPPERNORET())
CALLWRAPPERARG4(ScriptModuleCore::UserStats, SetVectorHash, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETADRESS(Vector,arg3), CALLWRAPPERGETADRESS(Vector,arg4), CALLWRAPPERNORET())
CALLWRAPPERARG4(ScriptModuleCore::UserStats, SetStringHash, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETADRESS(string,arg3), CALLWRAPPERGETADRESS(string,arg4), CALLWRAPPERNORET())

CALLWRAPPERARG3(ScriptModuleCore::UserStats, GetFloatHash, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERRET(float))
CALLWRAPPERARG3(ScriptModuleCore::UserStats, GetIntHash, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETPARAM(int,arg3), CALLWRAPPERRET(int))
CALLWRAPPERARG3(ScriptModuleCore::UserStats, GetVectorHash, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETADRESS(Vector,arg3), CALLWRAPPERRET(Vector))
CALLWRAPPERARG4(ScriptModuleCore::UserStats, GetStringHash, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETADRESS(string,arg3), CALLWRAPPERGETADRESS(string,arg4), CALLWRAPPERNORET())

CALLWRAPPERARG3(ScriptModuleCore::UserStats, GetHashes, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(int,arg2), CALLWRAPPERGETADRESS(int,arg3), CALLWRAPPERNORET())
CALLWRAPPER(ScriptModuleCore::UserStats, Load, CALLWRAPPERRET(bool))
CALLWRAPPER(ScriptModuleCore::UserStats, Save, CALLWRAPPERNORET())


CALLWRAPPER(ScriptModuleCore::Utils, Rnd, CALLWRAPPERRET(float))
CALLWRAPPERARG2(ScriptModuleCore::Utils, GetRayDir, CALLWRAPPERGETADRESS(Vector,arg1), CALLWRAPPERGETADRESS(Vector,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG2(ScriptModuleCore::Utils, GetScreenPos, CALLWRAPPERGETADRESS(Vector,arg1), CALLWRAPPERGETADRESS(Vector,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG1(ScriptModuleCore::Utils, GetCamDir, CALLWRAPPERGETADRESS(Vector,arg1), CALLWRAPPERNORET())
CALLWRAPPER(ScriptModuleCore::Utils, GetHour, CALLWRAPPERRET(float))
CALLWRAPPER(ScriptModuleCore::Utils, GetMinute, CALLWRAPPERRET(float))
CALLWRAPPER(ScriptModuleCore::Utils, GetSecond, CALLWRAPPERRET(float))
CALLWRAPPER(ScriptModuleCore::Utils, GetDay, CALLWRAPPERRET(float))
CALLWRAPPER(ScriptModuleCore::Utils, GetMonth, CALLWRAPPERRET(float))
CALLWRAPPER(ScriptModuleCore::Utils, GetYear, CALLWRAPPERRET(float))
CALLWRAPPER(ScriptModuleCore::Utils, GetSystemTime, CALLWRAPPERRET(int64))

	
CALLWRAPPERARG2(ScriptModuleCore::Utils, ParseUnixTime, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(uint64,arg2), CALLWRAPPERRET(int))
CALLWRAPPERARG6(ScriptModuleCore::Utils, GetDateTimeUTC, CALLWRAPPERGETADRESS(int,arg1), CALLWRAPPERGETADRESS(int,arg2), CALLWRAPPERGETADRESS(int,arg3), CALLWRAPPERGETADRESS(int,arg4), CALLWRAPPERGETADRESS(int,arg5), CALLWRAPPERGETADRESS(int,arg6), CALLWRAPPERNORET())
CALLWRAPPERARG6(ScriptModuleCore::Utils, GetTimestamp, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETPARAM(int,arg3), CALLWRAPPERGETPARAM(int,arg4), CALLWRAPPERGETPARAM(int,arg5), CALLWRAPPERGETPARAM(int,arg6), CALLWRAPPERRET(int64))

CALLWRAPPERARG3(ScriptModuleCore::Utils, GetWeekOfDay, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETPARAM(int,arg3), CALLWRAPPERRET(int))

CALLWRAPPER(ScriptModuleCore::Utils, GetFps, CALLWRAPPERRET(float))


CALLWRAPPERARG1(ScriptModuleCore::Utils, CalcUtf8Len, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERRET(int))
CALLWRAPPERARG3(ScriptModuleCore::Utils, ClampUtf8, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERGETPARAM(int,arg3), CALLWRAPPERNORET())

CALLWRAPPERARG2(ScriptModuleCore::Utils, ConvertASCIIToUTF8, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())	
CALLWRAPPERARG2(ScriptModuleCore::Utils, Int64ToString, CALLWRAPPERGETPARAM(int64,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())	
CALLWRAPPERARG2(ScriptModuleCore::Utils, UInt64ToString, CALLWRAPPERGETPARAM(uint64,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())
	
CALLWRAPPERARG2(ScriptModuleCore::Utils, GetMD5Hash, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG2(ScriptModuleCore::Utils, GetUID, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG1(ScriptModuleCore::Utils, GetISO8601DateTime, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG1(ScriptModuleCore::Utils, GetISO8601DateTimeForAzureQuerry, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
	
CALLWRAPPERARG2(ScriptModuleCore::Utils, ExtractFileName, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())
	

CALLWRAPPERARG2(ScriptModuleCore::Utils, OpenURL, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETPARAM(bool,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG2(ScriptModuleCore::Utils, OpenURL2, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG2(ScriptModuleCore::Utils, OpenVideo2params, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG1(ScriptModuleCore::Utils, OpenVideo1param, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())	

CALLWRAPPERARG2(ScriptModuleCore::Utils, StartMonitoringRegion, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG1(ScriptModuleCore::Utils, StopMonitoringRegion, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG2(ScriptModuleCore::Utils, SetCallbackOnEnterRegion, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG2(ScriptModuleCore::Utils, SetCallbackOnExitRegion, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG1(ScriptModuleCore::Utils, GetRegionBeaconsCount, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERRET(int))
CALLWRAPPERARG2(ScriptModuleCore::Utils, GetRegionBeaconMinor, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERRET(int))
CALLWRAPPERARG2(ScriptModuleCore::Utils, GetRegionBeaconMajor, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERRET(int))
CALLWRAPPERARG2(ScriptModuleCore::Utils, GetRegionBeaconProximity, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERRET(int))
CALLWRAPPERARG2(ScriptModuleCore::Utils, GetRegionBeaconAccuracy, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERRET(float))	
CALLWRAPPERARG2(ScriptModuleCore::Utils, HTMLToText, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())

CALLWRAPPERARG3(ScriptModuleCore::Utils, Replace, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERGETADRESS(string,arg3), CALLWRAPPERNORET())
CALLWRAPPERARG2(ScriptModuleCore::Utils, LowerCase, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG2(ScriptModuleCore::Utils, UpperCase, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG4(ScriptModuleCore::Utils, PrintTextOnPhoto, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERGETADRESS(string,arg3), CALLWRAPPERGETADRESS(string,arg4), CALLWRAPPERNORET())
CALLWRAPPER(ScriptModuleCore::Utils, PrintCallStack, CALLWRAPPERNORET())

CALLWRAPPERARG2(ScriptModuleCore::Utils, CopyFileScript, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())

	
CALLWRAPPERARG1(ScriptModuleCore::Utils, QuadraticEaseIn, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Utils, QuadraticEaseOut, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Utils, QuadraticEaseInOut, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))

CALLWRAPPERARG1(ScriptModuleCore::Utils, CubicEaseIn, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Utils, CubicEaseOut, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Utils, CubicEaseInOut, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
		
CALLWRAPPERARG1(ScriptModuleCore::Utils, QuarticEaseIn, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Utils, QuarticEaseOut, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Utils, QuarticEaseInOut, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))

CALLWRAPPERARG1(ScriptModuleCore::Utils, QuinticEaseIn, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Utils, QuinticEaseOut, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Utils, QuinticEaseInOut, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))

CALLWRAPPERARG1(ScriptModuleCore::Utils, SineEaseIn, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Utils, SineEaseOut, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Utils, SineEaseInOut, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))

CALLWRAPPERARG1(ScriptModuleCore::Utils, CircularEaseIn, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Utils, CircularEaseOut, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Utils, CircularEaseInOut, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))

CALLWRAPPERARG1(ScriptModuleCore::Utils, ExponentialEaseIn, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Utils, ExponentialEaseOut, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Utils, ExponentialEaseInOut, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))

CALLWRAPPERARG1(ScriptModuleCore::Utils, ElasticEaseIn, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Utils, ElasticEaseOut, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Utils, ElasticEaseInOut, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
	
CALLWRAPPERARG1(ScriptModuleCore::Utils, BackEaseIn, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Utils, BackEaseOut, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Utils, BackEaseInOut, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))

CALLWRAPPERARG1(ScriptModuleCore::Utils, BounceEaseIn, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Utils, BounceEaseOut, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(ScriptModuleCore::Utils, BounceEaseInOut, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERRET(float))	

CALLWRAPPER(ScriptModuleCore::Utils, GetInstallTime, CALLWRAPPERRET(int64))

CALLWRAPPERARG2(ScriptModuleCore::DataFile, Start, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERRET(bool))	
CALLWRAPPERARG3(ScriptModuleCore::DataFile, StartEx, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERGETPARAM(int,arg3), CALLWRAPPERRET(bool))	
CALLWRAPPERARG3(ScriptModuleCore::DataFile, StartFromRawData, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERGETPARAM(int,arg3), CALLWRAPPERRET(bool))	
CALLWRAPPER(ScriptModuleCore::DataFile, Finish, CALLWRAPPERNORET())	
CALLWRAPPERARG1(ScriptModuleCore::DataFile, EnterBlock, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERRET(bool))	
CALLWRAPPER(ScriptModuleCore::DataFile, LeaveBlock, CALLWRAPPERNORET())
	
CALLWRAPPERARG2(ScriptModuleCore::DataFile, ReadString, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERRET(bool))	
CALLWRAPPERARG2(ScriptModuleCore::DataFile, ReadBool, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(bool,arg2), CALLWRAPPERRET(bool))	

CALLWRAPPERARG2(ScriptModuleCore::DataFile, ReadFloat, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(float,arg2), CALLWRAPPERRET(bool))	

CALLWRAPPERARG2(ScriptModuleCore::DataFile, ReadInt, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(int,arg2), CALLWRAPPERRET(bool))	
CALLWRAPPERARG2(ScriptModuleCore::DataFile, ReadInt64, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(int64,arg2), CALLWRAPPERRET(bool))	
CALLWRAPPERARG2(ScriptModuleCore::DataFile, ReadUInt, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(dword,arg2), CALLWRAPPERRET(bool))	
CALLWRAPPERARG2(ScriptModuleCore::DataFile, ReadUInt64, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(uint64,arg2), CALLWRAPPERRET(bool))	
CALLWRAPPERARG2(ScriptModuleCore::DataFile, ReadVector, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(Vector,arg2), CALLWRAPPERRET(bool))		


CALLWRAPPERARG2(ScriptModuleCore::StreamSaver, StartSS, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERRET(bool))	
CALLWRAPPER(ScriptModuleCore::StreamSaver, FinishSS, CALLWRAPPERNORET())	
CALLWRAPPERARG1(ScriptModuleCore::StreamSaver, EnterBlockSS, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())	
CALLWRAPPERARG1(ScriptModuleCore::StreamSaver, LeaveBlockSS, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())	

CALLWRAPPERARG2(ScriptModuleCore::StreamSaver, WriteString, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())	
CALLWRAPPERARG2(ScriptModuleCore::StreamSaver, WriteBool, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(bool,arg2), CALLWRAPPERNORET())	
CALLWRAPPERARG2(ScriptModuleCore::StreamSaver, WriteFloat, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(float,arg2), CALLWRAPPERNORET())	
CALLWRAPPERARG2(ScriptModuleCore::StreamSaver, WriteInt, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(int,arg2), CALLWRAPPERNORET())	
CALLWRAPPERARG2(ScriptModuleCore::StreamSaver, WriteInt64, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(int64,arg2), CALLWRAPPERNORET())	
CALLWRAPPERARG2(ScriptModuleCore::StreamSaver, WriteVector, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(Vector,arg2), CALLWRAPPERNORET())	

void CALLWRAPPERNAME(StartScanning)(asIScriptGeneric *gen)
{
	ScriptModuleCore::MediaLibrary* obj = (ScriptModuleCore::MediaLibrary*)gen->GetObject();
	obj->StartScanning((asIScriptFunction*)(gen->GetArgAddress(0)));
}

CALLWRAPPER(ScriptModuleCore::MediaLibrary, GetFilesCount, CALLWRAPPERRET(int))	
CALLWRAPPERARG3(ScriptModuleCore::MediaLibrary, GetFileName, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERGETPARAM(bool,arg3), CALLWRAPPERNORET())	
CALLWRAPPERARG3(ScriptModuleCore::MediaLibrary, GetFileNameThumbnail, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERGETPARAM(bool,arg3), CALLWRAPPERNORET())	

CALLWRAPPER(ScriptModuleCore::MediaLibrary, GetYearCount, CALLWRAPPERRET(int))	
CALLWRAPPERARG1(ScriptModuleCore::MediaLibrary, GetMonthCount, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERRET(int))	
CALLWRAPPERARG2(ScriptModuleCore::MediaLibrary, GetDayCount, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERRET(int))	
CALLWRAPPERARG3(ScriptModuleCore::MediaLibrary, GetImageCount, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETPARAM(int,arg3), CALLWRAPPERRET(int))	

CALLWRAPPERARG1(ScriptModuleCore::MediaLibrary, GetIndexByYear, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERRET(int))	
CALLWRAPPERARG2(ScriptModuleCore::MediaLibrary, GetIndexByMonth, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERRET(int))	
CALLWRAPPERARG3(ScriptModuleCore::MediaLibrary, GetIndexByDay, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETPARAM(int,arg3), CALLWRAPPERRET(int))	

CALLWRAPPERARG4(ScriptModuleCore::MediaLibrary, GetDate, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETADRESS(int,arg2), CALLWRAPPERGETADRESS(int,arg3), CALLWRAPPERGETADRESS(int,arg4), CALLWRAPPERRET(bool))	
CALLWRAPPERARG4(ScriptModuleCore::MediaLibrary, GetTime, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETADRESS(int,arg2), CALLWRAPPERGETADRESS(int,arg3), CALLWRAPPERGETADRESS(int,arg4), CALLWRAPPERRET(bool))	

void CALLWRAPPERNAME(SignIn)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Facebook* obj = (ScriptModuleCore::Facebook*)gen->GetObject();

	int argIndex = 0;
	
	asIScriptFunction* arg1 = (asIScriptFunction*)(gen->GetArgAddress(argIndex));
	argIndex++;

	obj->SignIn(arg1);
}

CALLWRAPPERARG1(ScriptModuleCore::Facebook, AddRequestPermistion, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())	

void CALLWRAPPERNAME(StartRequestPermistions)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Facebook* obj = (ScriptModuleCore::Facebook*)gen->GetObject();
	obj->StartRequestPermistions((asIScriptFunction*)(gen->GetArgAddress(0)));
}

CALLWRAPPER(ScriptModuleCore::Facebook, GetSignInStatus, CALLWRAPPERRET(bool))	

void CALLWRAPPERNAME(SignOut)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Facebook* obj = (ScriptModuleCore::Facebook*)gen->GetObject();
	obj->SignOut((asIScriptFunction*)(gen->GetArgAddress(0)));
}

void CALLWRAPPERNAME(FBStartRequest)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Facebook* obj = (ScriptModuleCore::Facebook*)gen->GetObject();

	int argIndex = 0;

	CALLWRAPPERGETADRESS(string,arg1)
	CALLWRAPPERGETADRESS(string,arg2)
	CALLWRAPPERGETADRESS(string,arg3)
	CALLWRAPPERGETADRESS(string,arg4)				

	asIScriptFunction* arg5 = (asIScriptFunction*)(gen->GetArgAddress(argIndex));
	argIndex++;
	
	obj->FBStartRequest(*arg1, *arg2, *arg3, *arg4, arg5);
}

void CALLWRAPPERNAME(FBStartRequest2)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Facebook* obj = (ScriptModuleCore::Facebook*)gen->GetObject();

	int argIndex = 0;

	CALLWRAPPERGETADRESS(string,arg1)
	CALLWRAPPERGETADRESS(string,arg2)
	CALLWRAPPERGETADRESS(string,arg3)
	CALLWRAPPERGETADRESS(string,arg4)				

	asIScriptFunction* arg5 = (asIScriptFunction*)(gen->GetArgAddress(argIndex));
	argIndex++;

	CALLWRAPPERGETADRESS(string,arg6)		

	obj->FBStartRequest2(*arg1, *arg2, *arg3, *arg4, arg5, *arg6);
}

void CALLWRAPPERNAME(UploadFile)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Facebook* obj = (ScriptModuleCore::Facebook*)gen->GetObject();

	int argIndex = 0;

	CALLWRAPPERGETADRESS(string,arg1)
	CALLWRAPPERGETADRESS(string,arg2)
	CALLWRAPPERGETADRESS(string,arg3)
	CALLWRAPPERGETADRESS(string,arg4)		

	asIScriptFunction* arg5 = (asIScriptFunction*)(gen->GetArgAddress(argIndex));
	argIndex++;

	obj->UploadFile(*arg1, *arg2, *arg3, *arg4, arg5);
}


CALLWRAPPERARG1(ScriptModuleCore::Facebook, SetGuestAccessTokenURL, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())	
CALLWRAPPERARG1(ScriptModuleCore::Facebook, SetApiURL, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())		

void CALLWRAPPERNAME(VkSignIn)(asIScriptGeneric *gen)
{
	ScriptModuleCore::VK* obj = (ScriptModuleCore::VK*)gen->GetObject();
	obj->VkSignIn((asIScriptFunction*)(gen->GetArgAddress(0)));
}

CALLWRAPPER(ScriptModuleCore::VK, VkGetSignInStatus, CALLWRAPPERRET(int))

void CALLWRAPPERNAME(VkSignOut)(asIScriptGeneric *gen)
{
	ScriptModuleCore::VK* obj = (ScriptModuleCore::VK*)gen->GetObject();
	obj->VkSignOut((asIScriptFunction*)(gen->GetArgAddress(0)));
}

CALLWRAPPERARG1(ScriptModuleCore::VK, VkGetUserID, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())

void CALLWRAPPERNAME(VkStartRequest)(asIScriptGeneric *gen)
{
	ScriptModuleCore::VK* obj = (ScriptModuleCore::VK*)gen->GetObject();

	int argIndex = 0;

	CALLWRAPPERGETADRESS(string,arg1)
	CALLWRAPPERGETADRESS(string,arg2)
	CALLWRAPPERGETADRESS(string,arg3)
				
	asIScriptFunction* arg4 = (asIScriptFunction*)(gen->GetArgAddress(argIndex));
	argIndex++;

	obj->VkStartRequest(*arg1, *arg2, *arg3, arg4);
}

void CALLWRAPPERNAME(VkStartRequest2)(asIScriptGeneric *gen)
{
	ScriptModuleCore::VK* obj = (ScriptModuleCore::VK*)gen->GetObject();

	int argIndex = 0;

	CALLWRAPPERGETADRESS(string,arg1)
	CALLWRAPPERGETADRESS(string,arg2)
	CALLWRAPPERGETADRESS(string,arg3)	
	
	asIScriptFunction* arg4 = (asIScriptFunction*)(gen->GetArgAddress(argIndex));
	argIndex++;

	CALLWRAPPERGETADRESS(string,arg5)	

	obj->VkStartRequest2(*arg1, *arg2, *arg3, arg4, *arg5);
}

void CALLWRAPPERNAME(VKUploadFile)(asIScriptGeneric *gen)
{
	ScriptModuleCore::VK* obj = (ScriptModuleCore::VK*)gen->GetObject();

	int argIndex = 0;

	CALLWRAPPERGETADRESS(string,arg1)
	CALLWRAPPERGETADRESS(string,arg2)	

	asIScriptFunction* arg3 = (asIScriptFunction*)(gen->GetArgAddress(argIndex));	

	obj->VKUploadFile(*arg1, *arg2, arg3);
}

CALLWRAPPERARG1(ScriptModuleCore::VK, VkSetApiURL, CALLWRAPPERGETADRESS(string,arg1),CALLWRAPPERNORET())

void CALLWRAPPERNAME(InstagramSignIn)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Instagram* obj = (ScriptModuleCore::Instagram*)gen->GetObject();
	obj->InstagramSignIn((asIScriptFunction*)(gen->GetArgAddress(0)));
}

void CALLWRAPPERNAME(InstagramSignOut)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Instagram* obj = (ScriptModuleCore::Instagram*)gen->GetObject();
	obj->InstagramSignOut((asIScriptFunction*)(gen->GetArgAddress(0)));
}

CALLWRAPPERARG1(ScriptModuleCore::Instagram, InstagramGetAccessToken, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())


CALLWRAPPERARG1(ScriptModuleCore::OnlineUserStats, TrackUserAge, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERNORET())	
CALLWRAPPERARG1(ScriptModuleCore::OnlineUserStats, TrackUserGender, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())	
CALLWRAPPERARG2(ScriptModuleCore::OnlineUserStats, AddEventParam, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())	
CALLWRAPPERARG1(ScriptModuleCore::OnlineUserStats, TrackEvent, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())	


CALLWRAPPER(ScriptModuleCore::Network, IsInternetConnected, CALLWRAPPERRET(bool))

void CALLWRAPPERNAME(NetStartRequest)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Network* obj = (ScriptModuleCore::Network*)gen->GetObject();

	int argIndex = 0;

	CALLWRAPPERGETADRESS(string,arg1)
	CALLWRAPPERGETADRESS(string,arg2)
	CALLWRAPPERGETADRESS(string,arg3)
	CALLWRAPPERGETADRESS(string,arg4)		

	asIScriptFunction* arg5 = (asIScriptFunction*)(gen->GetArgAddress(argIndex));
	argIndex++;

	obj->NetStartRequest(*arg1, *arg2, *arg3, *arg4, arg5);
}

void CALLWRAPPERNAME(NetStartRequest2)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Network* obj = (ScriptModuleCore::Network*)gen->GetObject();

	int argIndex = 0;

	CALLWRAPPERGETADRESS(string,arg1)
	CALLWRAPPERGETADRESS(string,arg2)
	CALLWRAPPERGETADRESS(string,arg3)
	CALLWRAPPERGETADRESS(string,arg4)	

	asIScriptFunction* arg5 = (asIScriptFunction*)(gen->GetArgAddress(argIndex));
	argIndex++;

	CALLWRAPPERGETADRESS(string,arg6)	

	obj->NetStartRequest2(*arg1, *arg2, *arg3, *arg4, arg5, *arg6);
}

void CALLWRAPPERNAME(SetPushListener)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Mobile* obj = (ScriptModuleCore::Mobile*)gen->GetObject();
	obj->SetPushListener((asIScriptFunction*)(gen->GetArgAddress(0)));
}

void CALLWRAPPERNAME(SetOnResumeCallback)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Mobile* obj = (ScriptModuleCore::Mobile*)gen->GetObject();
	obj->SetOnResumeCallback((asIScriptFunction*)(gen->GetArgAddress(0)));
}

void CALLWRAPPERNAME(SetOnPauseCallback)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Mobile* obj = (ScriptModuleCore::Mobile*)gen->GetObject();
	obj->SetOnPauseCallback((asIScriptFunction*)(gen->GetArgAddress(0)));
}


CALLWRAPPER(ScriptModuleCore::Mobile, IsReadContactsAllowed, CALLWRAPPERRET(bool))

void CALLWRAPPERNAME(RequestAccessToContacts)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Mobile* obj = (ScriptModuleCore::Mobile*)gen->GetObject();
	obj->RequestAccessToContacts((asIScriptFunction*)(gen->GetArgAddress(0)));
}

void CALLWRAPPERNAME(StartReadContacts)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Mobile* obj = (ScriptModuleCore::Mobile*)gen->GetObject();
	obj->StartReadContacts((asIScriptFunction*)(gen->GetArgAddress(0)));
}

void CALLWRAPPERNAME(StartReadContactsAvatars)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Mobile* obj = (ScriptModuleCore::Mobile*)gen->GetObject();
	obj->StartReadContactsAvatars((asIScriptFunction*)(gen->GetArgAddress(0)));
}

CALLWRAPPER(ScriptModuleCore::Mobile, GetContactsCount, CALLWRAPPERRET(int))
CALLWRAPPERARG4(ScriptModuleCore::Mobile, GetContactData,  CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERGETADRESS(string,arg3), CALLWRAPPERGETADRESS(string,arg4), CALLWRAPPERNORET())

void CALLWRAPPERNAME(SendSms)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Mobile* obj = (ScriptModuleCore::Mobile*)gen->GetObject();

	int argIndex = 0;

	CALLWRAPPERGETADRESS(string,arg1)	
		CALLWRAPPERGETADRESS(string,arg2)	

		asIScriptFunction* arg3 = (asIScriptFunction*)(gen->GetArgAddress(argIndex));
	argIndex++;

	obj->SendSms(*arg1, *arg1, arg3);
}

CALLWRAPPERARG1(ScriptModuleCore::Mobile, GetSimOperatorName, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())

void CALLWRAPPERNAME(StartSelectImage)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Mobile* obj = (ScriptModuleCore::Mobile*)gen->GetObject();

	int argIndex = 0;

	CALLWRAPPERGETPARAM(int,arg1)	

	asIScriptFunction* arg2 = (asIScriptFunction*)(gen->GetArgAddress(argIndex));
	argIndex++;

	obj->StartSelectImage(*arg1, arg2);
}

CALLWRAPPER(ScriptModuleCore::Mobile, IsAllowCall, CALLWRAPPERRET(bool))
CALLWRAPPERARG1(ScriptModuleCore::Mobile, CallNumber, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG1(ScriptModuleCore::Mobile, SetPushServerURL, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPER(ScriptModuleCore::Mobile, StartRegisterInPushService, CALLWRAPPERNORET())

CALLWRAPPERARG3(ScriptModuleCore::Mobile, SendMail, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERGETADRESS(string,arg3), CALLWRAPPERNORET())	

void CALLWRAPPERNAME(StartShareText)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Mobile* obj = (ScriptModuleCore::Mobile*)gen->GetObject();

	int argIndex = 0;

	CALLWRAPPERGETADRESS(string,arg1)	
	CALLWRAPPERGETADRESS(string,arg2)	

	asIScriptFunction* arg3 = (asIScriptFunction*)(gen->GetArgAddress(argIndex));
	argIndex++;

	obj->StartShareText(*arg1, *arg1, arg3);
}

void CALLWRAPPERNAME(StartShareImage)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Mobile* obj = (ScriptModuleCore::Mobile*)gen->GetObject();

	int argIndex = 0;

	CALLWRAPPERGETADRESS(string,arg1)	

	asIScriptFunction* arg2 = (asIScriptFunction*)(gen->GetArgAddress(argIndex));
	argIndex++;

	obj->StartShareImage(*arg1, arg2);
}


CALLWRAPPERARG9(ScriptModuleCore::Mobile, AddEventToCalendar, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(int,arg2), CALLWRAPPERGETPARAM(int,arg3), CALLWRAPPERGETPARAM(int,arg4), CALLWRAPPERGETPARAM(int,arg5), CALLWRAPPERGETPARAM(int,arg6), CALLWRAPPERGETADRESS(string,arg7), CALLWRAPPERGETADRESS(string,arg8), CALLWRAPPERGETADRESS(string,arg9), CALLWRAPPERNORET())


CALLWRAPPER(ScriptModuleCore::Mobile, GetTittleBarHeight, CALLWRAPPERRET(float))

CALLWRAPPER(ScriptModuleCore::Mobile, GetBatteryLevel, CALLWRAPPERRET(float))
CALLWRAPPER(ScriptModuleCore::Mobile, GetBatteryStatus, CALLWRAPPERRET(int))
CALLWRAPPERARG1(ScriptModuleCore::Mobile, GetInstallReferer, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())

CALLWRAPPERARG2(ScriptModuleCore::Mobile, GetCameraTextureSize, CALLWRAPPERGETADRESS(int,arg1), CALLWRAPPERGETADRESS(int,arg2), CALLWRAPPERNORET())

void CALLWRAPPERNAME(StartScanBarcode)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Mobile* obj = (ScriptModuleCore::Mobile*)gen->GetObject();

	int argIndex = 0;
	
	asIScriptFunction* arg1 = (asIScriptFunction*)(gen->GetArgAddress(argIndex));
	argIndex++;

	obj->StartScanBarcode(arg1);
}

CALLWRAPPERARG1(ScriptModuleCore::Mobile, ShowMap, CALLWRAPPERGETPARAM(bool,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG4(ScriptModuleCore::Mobile, SetMapPosition,  CALLWRAPPERGETPARAM(float,arg1),  CALLWRAPPERGETPARAM(float,arg2),  CALLWRAPPERGETPARAM(float,arg3),  CALLWRAPPERGETPARAM(float,arg4), CALLWRAPPERNORET())
CALLWRAPPERARG3(ScriptModuleCore::Mobile, AddObjectToMap, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETADRESS(string,arg3), CALLWRAPPERNORET())
CALLWRAPPER(ScriptModuleCore::Mobile, DeleteAllMapObjects, CALLWRAPPERNORET())

CALLWRAPPERARG3(ScriptModuleCore::Mobile, ShowNativeMap, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETADRESS(string,arg3), CALLWRAPPERNORET())

void CALLWRAPPERNAME(ShowPopup)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Mobile* obj = (ScriptModuleCore::Mobile*)gen->GetObject();

	int argIndex = 0;

	CALLWRAPPERGETADRESS(string,arg1)
	CALLWRAPPERGETADRESS(string,arg2)
	CALLWRAPPERGETADRESS(string,arg3)

	asIScriptFunction* arg4 = (asIScriptFunction*)(gen->GetArgAddress(argIndex));
	argIndex++;

	obj->ShowPopup(*arg1, *arg2, *arg3, arg4);
}


void CALLWRAPPERNAME(ShowPopup2)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Mobile* obj = (ScriptModuleCore::Mobile*)gen->GetObject();

	int argIndex = 0;

	CALLWRAPPERGETADRESS(string,arg1)
	CALLWRAPPERGETADRESS(string,arg2)
	CALLWRAPPERGETADRESS(string,arg3)
	
	asIScriptFunction* arg4 = (asIScriptFunction*)(gen->GetArgAddress(argIndex));
	argIndex++;
	
	CALLWRAPPERGETADRESS(string,arg5)	

	asIScriptFunction* arg6 = (asIScriptFunction*)(gen->GetArgAddress(argIndex));
	argIndex++;

	obj->ShowPopup2(*arg1, *arg2, *arg3, arg4, *arg5, arg6);
}

void CALLWRAPPERNAME(EnableTrackGPS)(asIScriptGeneric *gen)
{
	ScriptModuleCore::Mobile* obj = (ScriptModuleCore::Mobile*)gen->GetObject();

	int argIndex = 0;

	CALLWRAPPERGETPARAM(float,arg1)		
		
	asIScriptFunction* arg2 = (asIScriptFunction*)(gen->GetArgAddress(argIndex));
	argIndex++;	

	obj->EnableTrackGPS(arg1, arg2);
}



CALLWRAPPER(ScriptModuleCore, Exit, CALLWRAPPERNORET())	
CALLWRAPPER(ScriptModuleCore, GetDeltaTime, CALLWRAPPERRET(float))	
CALLWRAPPERARG1(ScriptModuleCore, Trace, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())	
CALLWRAPPERARG2(ScriptModuleCore, TraceTo, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())	
CALLWRAPPER(ScriptModuleCore, GetCurrentPlatform, CALLWRAPPERRET(int))	
CALLWRAPPERARG1(ScriptModuleCore, GetDeviceVendor, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())	
CALLWRAPPERARG1(ScriptModuleCore, GetDeviceName, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())	
CALLWRAPPERARG1(ScriptModuleCore, GetDeviceID, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())	
CALLWRAPPERARG1(ScriptModuleCore, GetLocale, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())	

extern void HackFinishScriptHlp();

void ScriptModuleCore::Init(ScriptMachine* script_machine)
{	
	core->AddTask(this, this, (TaskHolder::Task)&ScriptModuleCore::Work, -10, -1.f);

	mobile.popup_listiner.used = false;    
    
	script_machine->RegisterFunctionDef("void CallbackWOP()");
	script_machine->RegisterFunctionDef("void Callback1I(int)");
	script_machine->RegisterFunctionDef("void Callback1F(float)");
	script_machine->RegisterFunctionDef("void Callback1S(string&in)");
	script_machine->RegisterFunctionDef("void Callback1I1S(int, string&in)");
	script_machine->RegisterFunctionDef("void Callback1I2S(int, string&in, string&in)");
	script_machine->RegisterFunctionDef("void Callback1I2F(int, float, float)");

	//Controls
	script_machine->RegisterObjectType("SceneScriptCoreControls", sizeof(ScriptModuleCore::Controls), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA);

	script_machine->RegisterObjectMethod("SceneScriptCoreControls", "bool IsKey(int key, int act)", CALLWRAPPERNAMEAS(IsKey));
	script_machine->RegisterObjectMethod("SceneScriptCoreControls", "float GetControlValue(int key)", CALLWRAPPERNAMEAS(GetControlValue));
	script_machine->RegisterObjectMethod("SceneScriptCoreControls", "bool CheckTouch(int index, int act)", CALLWRAPPERNAMEAS(CheckTouch));
	script_machine->RegisterObjectMethod("SceneScriptCoreControls", "float GetTouchX(int index)", CALLWRAPPERNAMEAS(GetTouchX));
	script_machine->RegisterObjectMethod("SceneScriptCoreControls", "float GetTouchY(int index)", CALLWRAPPERNAMEAS(GetTouchY));
	script_machine->RegisterObjectMethod("SceneScriptCoreControls", "bool IsAnyKeyPressed()", CALLWRAPPERNAMEAS(IsAnyKeyPressed));	
	script_machine->RegisterObjectMethod("SceneScriptCoreControls", "void HideSoftKeyboard()", CALLWRAPPERNAMEAS(HideSoftKeyboard));
	script_machine->RegisterObjectMethod("SceneScriptCoreControls", "int  GetSoftKeyboardHeight()", CALLWRAPPERNAMEAS(GetSoftKeyboardHeight));	
	script_machine->RegisterObjectMethod("SceneScriptCoreControls", "bool IsShowSoftKeyboard()", CALLWRAPPERNAMEAS(IsShowSoftKeyboard));	

	script_machine->RegisterObjectMethod("SceneScriptCoreControls", "float GetGyro(int index)", CALLWRAPPERNAMEAS(GetGyro));
	script_machine->RegisterObjectMethod("SceneScriptCoreControls", "void  SetGyro(int index, float val)", CALLWRAPPERNAMEAS(SetGyro));	

	script_machine->RegisterObjectMethod("SceneScriptCoreControls", "bool  IsBackPressed()", CALLWRAPPERNAMEAS(IsBackPressed));

	//Scene
	script_machine->RegisterObjectType("SceneScriptCoreScene", sizeof(ScriptModuleCore::Scene), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA);

	script_machine->RegisterObjectMethod("SceneScriptCoreScene", "void LoadScene(string&in sceneName)", CALLWRAPPERNAMEAS(LoadScene));
	script_machine->RegisterObjectMethod("SceneScriptCoreScene", "void UnloadScene(string&in sceneName)", CALLWRAPPERNAMEAS(UnloadScene));
	script_machine->RegisterObjectMethod("SceneScriptCoreScene", "bool IsSceneLoading(string&in sceneName)", CALLWRAPPERNAMEAS(IsSceneLoading));
	script_machine->RegisterObjectMethod("SceneScriptCoreScene", "float GetLoadingProgress(string&in sceneName)", CALLWRAPPERNAMEAS(GetLoadingProgress));
	script_machine->RegisterObjectMethod("SceneScriptCoreScene", "bool ActivateInScene(string&in sceneName, string&in entityName, bool act)", CALLWRAPPERNAMEAS(ActivateInScene));		

	//Render
	script_machine->RegisterObjectType("SceneScriptCoreRender", sizeof(ScriptModuleCore::Render), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA);

	script_machine->RegisterObjectMethod("SceneScriptCoreRender", "float GetWidth()", CALLWRAPPERNAMEAS(GetWidth));
	script_machine->RegisterObjectMethod("SceneScriptCoreRender", "float GetHeight()", CALLWRAPPERNAMEAS(GetHeight));	
	script_machine->RegisterObjectMethod("SceneScriptCoreRender", "float GetReferenceWidth()", CALLWRAPPERNAMEAS(GetReferenceWidth));
	script_machine->RegisterObjectMethod("SceneScriptCoreRender", "float GetReferenceHeight()", CALLWRAPPERNAMEAS(GetReferenceHeight));	

	script_machine->RegisterObjectMethod("SceneScriptCoreRender", "float HorzRefToScreen(float sz)", CALLWRAPPERNAMEAS(HorzRefToScreen));
	script_machine->RegisterObjectMethod("SceneScriptCoreRender", "float VertRefToScreen(float sz)", CALLWRAPPERNAMEAS(VertRefToScreen));	
	script_machine->RegisterObjectMethod("SceneScriptCoreRender", "float HorzScreenToRef(float sz)", CALLWRAPPERNAMEAS(HorzScreenToRef));
	script_machine->RegisterObjectMethod("SceneScriptCoreRender", "float VertScreenToRef(float sz)", CALLWRAPPERNAMEAS(VertScreenToRef));

	script_machine->RegisterObjectMethod("SceneScriptCoreRender", "void PrintText(float x, float y, string&in text)", CALLWRAPPERNAMEAS(PrintText));
	script_machine->RegisterObjectMethod("SceneScriptCoreRender", "void DrawLine2D(Vector2& p1, Vector& color1, Vector2& p2, Vector& color2)", CALLWRAPPERNAMEAS(DrawLine2D));	
	script_machine->RegisterObjectMethod("SceneScriptCoreRender", "int GetScreenDPI()", CALLWRAPPERNAMEAS(GetScreenDPI));	
	script_machine->RegisterObjectMethod("SceneScriptCoreRender", "void MakeScreenshot(string&in out_image)", CALLWRAPPERNAMEAS(MakeScreenshot));	

	//UserStats
	script_machine->RegisterObjectType("SceneScriptCoreUserStats", sizeof(ScriptModuleCore::UserStats), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA);

	script_machine->RegisterObjectMethod("SceneScriptCoreUserStats", "void SetFloat(string&in id, float def_value, float value, bool add)", CALLWRAPPERNAMEAS(SetFloat));
	script_machine->RegisterObjectMethod("SceneScriptCoreUserStats", "void SetInt(string&in id, int def_value, int value, bool add)", CALLWRAPPERNAMEAS(SetInt));	
	script_machine->RegisterObjectMethod("SceneScriptCoreUserStats", "void SetVector(string&in id, Vector def_value, Vector value)", CALLWRAPPERNAMEAS(SetVector));
	script_machine->RegisterObjectMethod("SceneScriptCoreUserStats", "void SetString(string&in id, string&in def_value, string&in value)", CALLWRAPPERNAMEAS(SetString));	
	
	script_machine->RegisterObjectMethod("SceneScriptCoreUserStats", "float GetFloat(string&in id, float def_value)", CALLWRAPPERNAMEAS(GetFloat));
	script_machine->RegisterObjectMethod("SceneScriptCoreUserStats", "int GetInt(string&in id, int def_value)", CALLWRAPPERNAMEAS(GetInt));	
	script_machine->RegisterObjectMethod("SceneScriptCoreUserStats", "Vector GetVector(string&in id, Vector def_value)", CALLWRAPPERNAMEAS(GetVector));
	script_machine->RegisterObjectMethod("SceneScriptCoreUserStats", "void GetString(string&in id, string&in def_value, string& value)", CALLWRAPPERNAMEAS(GetString));
	
	script_machine->RegisterObjectMethod("SceneScriptCoreUserStats", "void SetFloat(int hash1, int hash2, float def_value, float value, bool add)", CALLWRAPPERNAMEAS(SetFloatHash));
	script_machine->RegisterObjectMethod("SceneScriptCoreUserStats", "void SetInt(int hash1, int hash2, int def_value, int value, bool add)", CALLWRAPPERNAMEAS(SetIntHash));	
	script_machine->RegisterObjectMethod("SceneScriptCoreUserStats", "void SetVector(int hash1, int hash2, Vector def_value, Vector value)", CALLWRAPPERNAMEAS(SetVectorHash));
	script_machine->RegisterObjectMethod("SceneScriptCoreUserStats", "void SetString(int hash1, int hash2, string&in def_value, string&in value)", CALLWRAPPERNAMEAS(SetStringHash));	
	
	script_machine->RegisterObjectMethod("SceneScriptCoreUserStats", "float GetFloat(int hash1, int hash2, float def_value)", CALLWRAPPERNAMEAS(GetFloatHash));
	script_machine->RegisterObjectMethod("SceneScriptCoreUserStats", "int GetInt(int hash1, int hash2, int def_value)", CALLWRAPPERNAMEAS(GetIntHash));	
	script_machine->RegisterObjectMethod("SceneScriptCoreUserStats", "Vector GetVector(int hash1, int hash2, Vector def_value)", CALLWRAPPERNAMEAS(GetVectorHash));
	script_machine->RegisterObjectMethod("SceneScriptCoreUserStats", "void GetString(int hash1, int hash2, string&in def_value, string& value)", CALLWRAPPERNAMEAS(GetStringHash));	

	script_machine->RegisterObjectMethod("SceneScriptCoreUserStats", "void GetHashes(string&in id, int& hash1, int& hash2)", CALLWRAPPERNAMEAS(GetHashes));

	script_machine->RegisterObjectMethod("SceneScriptCoreUserStats", "bool Load()", CALLWRAPPERNAMEAS(Load));	
	script_machine->RegisterObjectMethod("SceneScriptCoreUserStats", "void Save()", CALLWRAPPERNAMEAS(Save));

	//Utils
	script_machine->RegisterObjectType("SceneScriptCoreUtils", sizeof(ScriptModuleCore::Utils), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA);

	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float Rnd()", CALLWRAPPERNAMEAS(Rnd));	
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void GetRayDir(Vector& pos, Vector& dir)", CALLWRAPPERNAMEAS(GetRayDir));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void GetScreenPos(Vector&in pos, Vector& screen_pos)", CALLWRAPPERNAMEAS(GetScreenPos));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void GetCamDir(Vector& dir)", CALLWRAPPERNAMEAS(GetCamDir));			
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float GetHour()", CALLWRAPPERNAMEAS(GetHour));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float GetMinute()", CALLWRAPPERNAMEAS(GetMinute));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float GetSecond()", CALLWRAPPERNAMEAS(GetSecond));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float GetDay()", CALLWRAPPERNAMEAS(GetDay));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float GetMonth()", CALLWRAPPERNAMEAS(GetMonth));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float GetYear()", CALLWRAPPERNAMEAS(GetYear));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "int64 GetSystemTime()", CALLWRAPPERNAMEAS(GetSystemTime));	

	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "int ParseUnixTime(int index, uint64 time)", CALLWRAPPERNAMEAS(ParseUnixTime));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void GetDateTimeUTC(int& year, int& month, int& day, int& hour, int& min, int& sec)", CALLWRAPPERNAMEAS(GetDateTimeUTC));		
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "int64 GetTimestamp(int year, int month, int day, int hour, int min, int sec)", CALLWRAPPERNAMEAS(GetTimestamp));

	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "int GetWeekOfDay(int day, int month, int year)", CALLWRAPPERNAMEAS(GetWeekOfDay));	

	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float GetFps()", CALLWRAPPERNAMEAS(GetFps));

	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "int CalcUtf8Len(string& text)", CALLWRAPPERNAMEAS(CalcUtf8Len));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void ClampUtf8(string&in text, string& res, int clamp_len)", CALLWRAPPERNAMEAS(ClampUtf8));
	

	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void ConvertASCIIToUTF8(string&in wstr, string& str)", CALLWRAPPERNAMEAS(ConvertASCIIToUTF8));	
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void Int64ToString(int64 val, string& str)", CALLWRAPPERNAMEAS(Int64ToString));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void UInt64ToString(uint64 val, string& str)", CALLWRAPPERNAMEAS(UInt64ToString));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void GetMD5Hash(string&in str, string& hash)", CALLWRAPPERNAMEAS(GetMD5Hash));		
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void GetUID(string&in postfix, string& uid)", CALLWRAPPERNAMEAS(GetUID));		
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void GetISO8601DateTime(string& str)", CALLWRAPPERNAMEAS(GetISO8601DateTime));		
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void GetISO8601DateTimeForAzureQuerry(string& str)", CALLWRAPPERNAMEAS(GetISO8601DateTimeForAzureQuerry));		
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void ExtractFileName(string&in FullPath, string& FileName)", CALLWRAPPERNAMEAS(ExtractFileName));		
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void OpenURL(string&in url, bool inner)", CALLWRAPPERNAMEAS(OpenURL));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void OpenURL(string&in url, string&in stop_url)", CALLWRAPPERNAMEAS(OpenURL2));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void OpenVideo(string&in path, string&in showName)", CALLWRAPPERNAMEAS(OpenVideo2params));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void OpenVideo(string&in path)", CALLWRAPPERNAMEAS(OpenVideo1param));

	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void StartMonitoringRegion(string&in id, string&in uuid)", CALLWRAPPERNAMEAS(StartMonitoringRegion));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void StopMonitoringRegion(string&in id)", CALLWRAPPERNAMEAS(StopMonitoringRegion));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void SetCallbackOnEnterRegion(string&in id, string&in func)", CALLWRAPPERNAMEAS(SetCallbackOnEnterRegion));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void SetCallbackOnExitRegion(string&in id, string&in func)", CALLWRAPPERNAMEAS(SetCallbackOnExitRegion));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "int GetRegionBeaconsCount(string&in id)", CALLWRAPPERNAMEAS(GetRegionBeaconsCount));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "int GetRegionBeaconMinor(string&in id, int index)", CALLWRAPPERNAMEAS(GetRegionBeaconMinor));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "int GetRegionBeaconMajor(string&in id, int index)", CALLWRAPPERNAMEAS(GetRegionBeaconMajor));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "int GetRegionBeaconProximity(string&in id, int index)", CALLWRAPPERNAMEAS(GetRegionBeaconProximity));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float GetRegionBeaconDistance(string&in id, int index)", CALLWRAPPERNAMEAS(GetRegionBeaconAccuracy));	

	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void HTMLToText(string& in, string& out)", CALLWRAPPERNAMEAS(HTMLToText));

	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void LowerCase(string& in, string& out)", CALLWRAPPERNAMEAS(LowerCase));		
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void UpperCase(string& in, string& out)", CALLWRAPPERNAMEAS(UpperCase));		
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void Replace(string& str, string&in find, string&in replace)", CALLWRAPPERNAMEAS(Replace));		
	
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void PrintTextOnPhoto(string&in text, string&in input_photo,  string&in output_photo, string&in output_ext)", CALLWRAPPERNAMEAS(PrintTextOnPhoto));			
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void PrintCallStack()", CALLWRAPPERNAMEAS(PrintCallStack));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "void CopyFile(string& src, string& dest)", CALLWRAPPERNAMEAS(CopyFileScript));	
	
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float QuadraticEaseIn(float f)", CALLWRAPPERNAMEAS(QuadraticEaseIn));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float QuadraticEaseOut(float f)", CALLWRAPPERNAMEAS(QuadraticEaseOut));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float QuadraticEaseInOut(float f)", CALLWRAPPERNAMEAS(QuadraticEaseInOut));
		
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float CubicEaseIn(float f)", CALLWRAPPERNAMEAS(CubicEaseIn));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float CubicEaseOut(float f)", CALLWRAPPERNAMEAS(CubicEaseOut));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float CubicEaseInOut(float f)", CALLWRAPPERNAMEAS(CubicEaseInOut));

	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float QuarticEaseIn(float f)", CALLWRAPPERNAMEAS(QuarticEaseIn));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float QuarticEaseOut(float f)", CALLWRAPPERNAMEAS(QuarticEaseOut));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float QuarticEaseInOut(float f)", CALLWRAPPERNAMEAS(QuarticEaseInOut));

	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float QuinticEaseIn(float f)", CALLWRAPPERNAMEAS(QuinticEaseIn));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float QuinticEaseOut(float f)", CALLWRAPPERNAMEAS(QuinticEaseOut));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float QuinticEaseInOut(float f)", CALLWRAPPERNAMEAS(QuinticEaseInOut));

	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float SineEaseIn(float f)", CALLWRAPPERNAMEAS(SineEaseIn));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float SineEaseOut(float f)", CALLWRAPPERNAMEAS(SineEaseOut));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float SineEaseInOut(float f)", CALLWRAPPERNAMEAS(SineEaseInOut));

	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float CircularEaseIn(float f)", CALLWRAPPERNAMEAS(CircularEaseIn));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float CircularEaseOut(float f)", CALLWRAPPERNAMEAS(CircularEaseOut));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float CircularEaseInOut(float f)", CALLWRAPPERNAMEAS(CircularEaseInOut));

	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float ExponentialEaseIn(float f)", CALLWRAPPERNAMEAS(ExponentialEaseIn));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float ExponentialEaseOut(float f)", CALLWRAPPERNAMEAS(ExponentialEaseOut));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float ExponentialEaseInOut(float f)", CALLWRAPPERNAMEAS(ExponentialEaseInOut));
	
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float ElasticEaseIn(float f)", CALLWRAPPERNAMEAS(ElasticEaseIn));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float ElasticEaseOut(float f)", CALLWRAPPERNAMEAS(ElasticEaseOut));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float ElasticEaseInOut(float f)", CALLWRAPPERNAMEAS(ElasticEaseInOut));

	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float BackEaseIn(float f)", CALLWRAPPERNAMEAS(BackEaseIn));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float BackEaseOut(float f)", CALLWRAPPERNAMEAS(BackEaseOut));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float BackEaseInOut(float f)", CALLWRAPPERNAMEAS(BackEaseInOut));

	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float BounceEaseIn(float f)", CALLWRAPPERNAMEAS(BounceEaseIn));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float BounceEaseOut(float f)", CALLWRAPPERNAMEAS(BounceEaseOut));
	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "float BounceEaseInOut(float f)", CALLWRAPPERNAMEAS(BounceEaseInOut));

	script_machine->RegisterObjectMethod("SceneScriptCoreUtils", "int64 GetInstallTime()", CALLWRAPPERNAMEAS(GetInstallTime));	

	//DataFile
	script_machine->RegisterObjectType("SceneScriptCoreDataFile", sizeof(ScriptModuleCore::DataFile), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA);

	script_machine->RegisterObjectMethod("SceneScriptCoreDataFile", "bool Start(string&in name, string&in start_block)", CALLWRAPPERNAMEAS(Start));	

	script_machine->RegisterObjectMethod("SceneScriptCoreDataFile", "bool Start(string&in name, string&in start_block, int file_type)", CALLWRAPPERNAMEAS(StartEx));		
    script_machine->RegisterObjectMethod("SceneScriptCoreDataFile", "bool StartFromRawData(string&in buffer, string&in start_block, int file_type)", CALLWRAPPERNAMEAS(StartFromRawData));		

	script_machine->RegisterObjectMethod("SceneScriptCoreDataFile", "void Finish()", CALLWRAPPERNAMEAS(Finish));	

	script_machine->RegisterObjectMethod("SceneScriptCoreDataFile", "bool EnterBlock(string&in name)", CALLWRAPPERNAMEAS(EnterBlock));	
	script_machine->RegisterObjectMethod("SceneScriptCoreDataFile", "void LeaveBlock()", CALLWRAPPERNAMEAS(LeaveBlock));	

	script_machine->RegisterObjectMethod("SceneScriptCoreDataFile", "bool Read(string&in name, string& val)", CALLWRAPPERNAMEAS(ReadString));		
	script_machine->RegisterObjectMethod("SceneScriptCoreDataFile", "bool Read(string&in name, bool& val)", CALLWRAPPERNAMEAS(ReadBool));	
	script_machine->RegisterObjectMethod("SceneScriptCoreDataFile", "bool Read(string&in name, float& val)", CALLWRAPPERNAMEAS(ReadFloat));	
	script_machine->RegisterObjectMethod("SceneScriptCoreDataFile", "bool Read(string&in name, int& val)", CALLWRAPPERNAMEAS(ReadInt));	
	script_machine->RegisterObjectMethod("SceneScriptCoreDataFile", "bool Read(string&in name, int64& val)", CALLWRAPPERNAMEAS(ReadInt64));	

	script_machine->RegisterObjectMethod("SceneScriptCoreDataFile", "bool Read(string&in name, uint& val)", CALLWRAPPERNAMEAS(ReadUInt));	
	script_machine->RegisterObjectMethod("SceneScriptCoreDataFile", "bool Read(string&in name, uint64& val)", CALLWRAPPERNAMEAS(ReadUInt64));	

	script_machine->RegisterObjectMethod("SceneScriptCoreDataFile", "bool Read(string&in name, Vector& val)", CALLWRAPPERNAMEAS(ReadVector));

	//StreamSaver
	script_machine->RegisterObjectType("SceneScriptCoreStreamSaver", sizeof(ScriptModuleCore::StreamSaver), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA);

	script_machine->RegisterObjectMethod("SceneScriptCoreStreamSaver", "bool Start(string&in name, int file_type)", CALLWRAPPERNAMEAS(StartSS));		

	script_machine->RegisterObjectMethod("SceneScriptCoreStreamSaver", "void Finish()", CALLWRAPPERNAMEAS(FinishSS));	

	script_machine->RegisterObjectMethod("SceneScriptCoreStreamSaver", "void EnterBlock(string&in name)", CALLWRAPPERNAMEAS(EnterBlockSS));	
	script_machine->RegisterObjectMethod("SceneScriptCoreStreamSaver", "void LeaveBlock(string&in name)", CALLWRAPPERNAMEAS(LeaveBlockSS));	

	script_machine->RegisterObjectMethod("SceneScriptCoreStreamSaver", "void Write(string&in name, string&in val)", CALLWRAPPERNAMEAS(WriteString));		
	script_machine->RegisterObjectMethod("SceneScriptCoreStreamSaver", "void Write(string&in name, bool&in val)", CALLWRAPPERNAMEAS(WriteBool));	
	script_machine->RegisterObjectMethod("SceneScriptCoreStreamSaver", "void Write(string&in name, float&in val)", CALLWRAPPERNAMEAS(WriteFloat));	
	script_machine->RegisterObjectMethod("SceneScriptCoreStreamSaver", "void Write(string&in name, int&in val)", CALLWRAPPERNAMEAS(WriteInt));	
	script_machine->RegisterObjectMethod("SceneScriptCoreStreamSaver", "void Write(string&in name, int64&in val)", CALLWRAPPERNAMEAS(WriteInt64));	
	script_machine->RegisterObjectMethod("SceneScriptCoreStreamSaver", "void Write(string&in name, Vector&in val)", CALLWRAPPERNAMEAS(WriteVector));

	// MediaLibrary
	script_machine->RegisterObjectType("SceneScriptCoreMediaLibrary", sizeof(ScriptModuleCore::MediaLibrary), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA);

	script_machine->RegisterObjectMethod("SceneScriptCoreMediaLibrary", "void StartScanning(CallbackWOP @cb)", CALLWRAPPERNAMEAS(StartScanning));	
	script_machine->RegisterObjectMethod("SceneScriptCoreMediaLibrary", "int GetFilesCount()", CALLWRAPPERNAMEAS(GetFilesCount));		
	script_machine->RegisterObjectMethod("SceneScriptCoreMediaLibrary", "void GetFileName(int i, string& name, bool prepareForLoading)", CALLWRAPPERNAMEAS(GetFileName));		
	script_machine->RegisterObjectMethod("SceneScriptCoreMediaLibrary", "void GetFileNameThumbnail(int i, string& name, bool prepareForLoading)", CALLWRAPPERNAMEAS(GetFileNameThumbnail));		

	script_machine->RegisterObjectMethod("SceneScriptCoreMediaLibrary", "int GetYearCount()", CALLWRAPPERNAMEAS(GetYearCount));		
	script_machine->RegisterObjectMethod("SceneScriptCoreMediaLibrary", "int GetMonthCount(int year)", CALLWRAPPERNAMEAS(GetMonthCount));		
	script_machine->RegisterObjectMethod("SceneScriptCoreMediaLibrary", "int GetDayCount(int year, int month)", CALLWRAPPERNAMEAS(GetDayCount));		
	script_machine->RegisterObjectMethod("SceneScriptCoreMediaLibrary", "int GetImageCount(int year, int month, int day)", CALLWRAPPERNAMEAS(GetImageCount));		


	script_machine->RegisterObjectMethod("SceneScriptCoreMediaLibrary", "int GetIndexByYear(int year)", CALLWRAPPERNAMEAS(GetIndexByYear));		
	script_machine->RegisterObjectMethod("SceneScriptCoreMediaLibrary", "int GetIndexByMonth(int year, int month)", CALLWRAPPERNAMEAS(GetIndexByMonth));		
	script_machine->RegisterObjectMethod("SceneScriptCoreMediaLibrary", "int GetIndexByDay(int year, int month, int day)", CALLWRAPPERNAMEAS(GetIndexByDay));		

	script_machine->RegisterObjectMethod("SceneScriptCoreMediaLibrary", "bool GetDate(int index, int& day, int& month, int& year)", CALLWRAPPERNAMEAS(GetDate));		
	script_machine->RegisterObjectMethod("SceneScriptCoreMediaLibrary", "bool GetTime(int index, int& hour, int& minute, int& second)", CALLWRAPPERNAMEAS(GetTime));		


	//Facebook
	script_machine->RegisterObjectType("SceneScriptCoreFacebook", sizeof(ScriptModuleCore::Facebook), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA);
			
	script_machine->RegisterObjectMethod("SceneScriptCoreFacebook", "void SignIn(Callback1I1S @cb)", CALLWRAPPERNAMEAS(SignIn));	
	
	script_machine->RegisterObjectMethod("SceneScriptCoreFacebook", "void AddRequestPermistion(string&in name)", CALLWRAPPERNAMEAS(AddRequestPermistion));
	script_machine->RegisterObjectMethod("SceneScriptCoreFacebook", "void StartRequestPermistions(Callback1I1S @cb)", CALLWRAPPERNAMEAS(StartRequestPermistions));	

	script_machine->RegisterObjectMethod("SceneScriptCoreFacebook", "bool GetSignInStatus()", CALLWRAPPERNAMEAS(GetSignInStatus));	
	script_machine->RegisterObjectMethod("SceneScriptCoreFacebook", "void SignOut(CallbackWOP @cb)", CALLWRAPPERNAMEAS(SignOut));	
	
	script_machine->RegisterObjectMethod("SceneScriptCoreFacebook", "void StartRequest(string&in method, string&in url, string&in params, string&in post_params, Callback1I1S @cb)", CALLWRAPPERNAMEAS(FBStartRequest));
	script_machine->RegisterObjectMethod("SceneScriptCoreFacebook", "void StartRequest(string&in method, string&in url, string&in params, string&in post_params, Callback1I2S @cb, string&in req_param)", CALLWRAPPERNAMEAS(FBStartRequest2));
	script_machine->RegisterObjectMethod("SceneScriptCoreFacebook", "void UploadFile(string& url, string& file_name, string& param_name, string& message, Callback1I1S @cb)", CALLWRAPPERNAMEAS(UploadFile));
	script_machine->RegisterObjectMethod("SceneScriptCoreFacebook", "void SetGuestAccessTokenURL(string& url)", CALLWRAPPERNAMEAS(SetGuestAccessTokenURL));
	script_machine->RegisterObjectMethod("SceneScriptCoreFacebook", "void SetApiURL(string& url)", CALLWRAPPERNAMEAS(SetApiURL));

	//VK
	script_machine->RegisterObjectType("SceneScriptCoreVK", sizeof(ScriptModuleCore::VK), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA);

	script_machine->RegisterObjectMethod("SceneScriptCoreVK", "void SignIn(Callback1I1S @cb)", CALLWRAPPERNAMEAS(VkSignIn));	
	script_machine->RegisterObjectMethod("SceneScriptCoreVK", "int  GetSignInStatus()", CALLWRAPPERNAMEAS(VkGetSignInStatus));	
	script_machine->RegisterObjectMethod("SceneScriptCoreVK", "void SignOut(CallbackWOP @cb)", CALLWRAPPERNAMEAS(VkSignOut));		
	script_machine->RegisterObjectMethod("SceneScriptCoreVK", "void GetUserID(string& user_id)", CALLWRAPPERNAMEAS(VkGetUserID));		

	script_machine->RegisterObjectMethod("SceneScriptCoreVK", "void StartRequest(string&in url, string&in params, string&in post_params, Callback1I1S @cb)", CALLWRAPPERNAMEAS(VkStartRequest));		
	script_machine->RegisterObjectMethod("SceneScriptCoreVK", "void StartRequest(string&in url, string&in params, string&in post_params, Callback1I2S @cb, string& req_param)", CALLWRAPPERNAMEAS(VkStartRequest2));		

	script_machine->RegisterObjectMethod("SceneScriptCoreVK", "void UploadFile(string&in url, string&in file_name, Callback1I1S @cb)", CALLWRAPPERNAMEAS(VKUploadFile));			

	script_machine->RegisterObjectMethod("SceneScriptCoreVK", "void SetApiURL(string& url)", CALLWRAPPERNAMEAS(VkSetApiURL));	
		
	//Instagram
	script_machine->RegisterObjectType("SceneScriptCoreInstagram", sizeof(ScriptModuleCore::Instagram), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA);

	script_machine->RegisterObjectMethod("SceneScriptCoreInstagram", "void SignIn(CallbackWOP @cb)", CALLWRAPPERNAMEAS(InstagramSignIn));		
	script_machine->RegisterObjectMethod("SceneScriptCoreInstagram", "void SignOut(CallbackWOP @cb)", CALLWRAPPERNAMEAS(InstagramSignOut));		
	script_machine->RegisterObjectMethod("SceneScriptCoreInstagram", "void GetAccessToken(string& token)", CALLWRAPPERNAMEAS(InstagramGetAccessToken));


	//OnlineUserStats
	script_machine->RegisterObjectType("SceneScriptCoreOnlineUserStats", sizeof(ScriptModuleCore::OnlineUserStats), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA);		
	
	script_machine->RegisterObjectMethod("SceneScriptCoreOnlineUserStats", "void TrackUserAge(int age)", CALLWRAPPERNAMEAS(TrackUserAge));		
	script_machine->RegisterObjectMethod("SceneScriptCoreOnlineUserStats", "void TrackUserGender(string& gender)", CALLWRAPPERNAMEAS(TrackUserGender));		
	script_machine->RegisterObjectMethod("SceneScriptCoreOnlineUserStats", "void AddEventParam(string&in name, string&in param)", CALLWRAPPERNAMEAS(AddEventParam));		
	script_machine->RegisterObjectMethod("SceneScriptCoreOnlineUserStats", "void TrackEvent(string&in name)", CALLWRAPPERNAMEAS(TrackEvent));		

	//Network
	script_machine->RegisterObjectType("SceneScriptCoreNetwork", sizeof(ScriptModuleCore::Network), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA);

	script_machine->RegisterObjectMethod("SceneScriptCoreNetwork", "bool IsInternetConnected()", CALLWRAPPERNAMEAS(IsInternetConnected));	
	script_machine->RegisterObjectMethod("SceneScriptCoreNetwork", "void StartRequest(string&in method, string&in request, string&in post_params, string&in file_name, Callback1I1S @cb)", CALLWRAPPERNAMEAS(NetStartRequest));
	script_machine->RegisterObjectMethod("SceneScriptCoreNetwork", "void StartRequest(string&in method, string&in request, string&in post_params, string&in file_name, Callback1I2S @cb, string&in req_param)", CALLWRAPPERNAMEAS(NetStartRequest2));			

	// Mobile
	script_machine->RegisterObjectType("SceneScriptCoreMobile", sizeof(ScriptModuleCore::Mobile), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA);	

	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void SetPushListener(Callback1S @cb)", CALLWRAPPERNAMEAS(SetPushListener));

	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void SetOnResumeCallback(CallbackWOP @cb)", CALLWRAPPERNAMEAS(SetOnResumeCallback));
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void SetOnPauseCallback(CallbackWOP @cb)", CALLWRAPPERNAMEAS(SetOnPauseCallback));	
	
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "bool IsReadContactsAllowed()", CALLWRAPPERNAMEAS(IsReadContactsAllowed));	
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void RequestAccessToContacts(Callback1I @cb)", CALLWRAPPERNAMEAS(RequestAccessToContacts));	

	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void StartReadContacts(Callback1I @cb)", CALLWRAPPERNAMEAS(StartReadContacts));
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void StartReadContactsAvatars(Callback1I @cb)", CALLWRAPPERNAMEAS(StartReadContactsAvatars));	
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "int GetContactsCount()", CALLWRAPPERNAMEAS(GetContactsCount));		
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void GetContactData(int i, string& name, string& texture, string& numbers)", CALLWRAPPERNAMEAS(GetContactData));		
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void SendSms(string&in number, string&in text,  Callback1I @cb)", CALLWRAPPERNAMEAS(SendSms));				
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void GetSimOperatorName(string& name)", CALLWRAPPERNAMEAS(GetSimOperatorName));		
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void StartSelectImage(int type, Callback1I1S @cb)", CALLWRAPPERNAMEAS(StartSelectImage));
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "bool IsAllowCall()", CALLWRAPPERNAMEAS(IsAllowCall));
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void CallNumber(string&in number)", CALLWRAPPERNAMEAS(CallNumber));

	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void SendMail(string&in address, string&in subject, string&in body)", CALLWRAPPERNAMEAS(SendMail));			
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void StartShareText(string&in text, string&in extra_data, Callback1S@ cb)", CALLWRAPPERNAMEAS(StartShareText));			
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void StartShareImage(string&in image, Callback1S@ cb)", CALLWRAPPERNAMEAS(StartShareImage));			
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void AddEventToCalendar(int day, int month, int year, int hour, int minute, int second, string&in name, string&in desc, string&in location)", CALLWRAPPERNAMEAS(AddEventToCalendar));

	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void SetPushServerURL(string&in number)", CALLWRAPPERNAMEAS(SetPushServerURL));	
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void StartRegisterInPushService()", CALLWRAPPERNAMEAS(StartRegisterInPushService));
	
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "float GetTittleBarHeight()", CALLWRAPPERNAMEAS(GetTittleBarHeight));			
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void ShowMap(bool show)", CALLWRAPPERNAMEAS(ShowMap));			
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void SetMapPosition(float x, float y, float w, float h)", CALLWRAPPERNAMEAS(SetMapPosition));			
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void AddObjectToMap(float x, float y, string&in text)", CALLWRAPPERNAMEAS(AddObjectToMap));			
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void DeleteAllMapObjects()", CALLWRAPPERNAMEAS(DeleteAllMapObjects));				

	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void ShowNativeMap(float x, float y, string&in text)", CALLWRAPPERNAMEAS(ShowNativeMap));				

	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void ShowPopup(string&in title, string&in text, string&in btn_text, CallbackWOP @cb)", CALLWRAPPERNAMEAS(ShowPopup));
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void ShowPopup(string&in title, string&in text, string&in btn_ok_text, CallbackWOP @cb_ok, string&in btn_cancel_text, CallbackWOP @cb_cancel)", CALLWRAPPERNAMEAS(ShowPopup2));	

	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "float GetBatteryLevel()", CALLWRAPPERNAMEAS(GetBatteryLevel));			
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "int   GetBatteryStatus()", CALLWRAPPERNAMEAS(GetBatteryStatus));		
	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void  GetInstallReferer(string& referer)", CALLWRAPPERNAMEAS(GetInstallReferer));	

	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void StartScanBarcode(Callback1S@ cb)", CALLWRAPPERNAMEAS(StartScanBarcode));				

	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void GetCameraTextureSize(int& width, int& height)", CALLWRAPPERNAMEAS(GetCameraTextureSize));				

	script_machine->RegisterObjectMethod("SceneScriptCoreMobile", "void EnableTrackGPS(bool enable, Callback1I2F@ cb)", CALLWRAPPERNAMEAS(EnableTrackGPS));		

	//Core
	script_machine->RegisterObjectType("SceneScriptCore", sizeof(ScriptModuleCore), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA);

	script_machine->RegisterObjectMethod("SceneScriptCore", "void Exit()", CALLWRAPPERNAMEAS(Exit));
	script_machine->RegisterObjectMethod("SceneScriptCore", "float GetDeltaTime()", CALLWRAPPERNAMEAS(GetDeltaTime));
	script_machine->RegisterObjectMethod("SceneScriptCore", "void Trace(string&in text)", CALLWRAPPERNAMEAS(Trace));
	script_machine->RegisterObjectMethod("SceneScriptCore", "void TraceTo(string&in log, string&in text)", CALLWRAPPERNAMEAS(TraceTo));	
	script_machine->RegisterObjectMethod("SceneScriptCore", "int GetCurrentPlatform()", CALLWRAPPERNAMEAS(GetCurrentPlatform));
	script_machine->RegisterObjectMethod("SceneScriptCore", "void GetDeviceVendor(string& device_vendor)", CALLWRAPPERNAMEAS(GetDeviceVendor));	
	script_machine->RegisterObjectMethod("SceneScriptCore", "void GetDeviceName(string& device_name)", CALLWRAPPERNAMEAS(GetDeviceName));
	script_machine->RegisterObjectMethod("SceneScriptCore", "void GetDeviceID(string& device_id)", CALLWRAPPERNAMEAS(GetDeviceID));
	script_machine->RegisterObjectMethod("SceneScriptCore", "void GetLocale(string& text)", CALLWRAPPERNAMEAS(GetLocale));	

	core->Script()->Machine()->RegisterObjectProperty("SceneScriptCore", "SceneScriptCoreControls Controls",offsetof(ScriptModuleCore, controls));
	core->Script()->Machine()->RegisterObjectProperty("SceneScriptCore", "SceneScriptCoreScene Scene",offsetof(ScriptModuleCore, scene));
	core->Script()->Machine()->RegisterObjectProperty("SceneScriptCore", "SceneScriptCoreRender Render",offsetof(ScriptModuleCore, render));
	core->Script()->Machine()->RegisterObjectProperty("SceneScriptCore", "SceneScriptCoreUserStats UserStats",offsetof(ScriptModuleCore, user_stats));	
	core->Script()->Machine()->RegisterObjectProperty("SceneScriptCore", "SceneScriptCoreUtils Utils",offsetof(ScriptModuleCore, utils));
	core->Script()->Machine()->RegisterObjectProperty("SceneScriptCore", "SceneScriptCoreDataFile DataFile",offsetof(ScriptModuleCore, data_file));
	core->Script()->Machine()->RegisterObjectProperty("SceneScriptCore", "SceneScriptCoreStreamSaver StreamSaver",offsetof(ScriptModuleCore, stream_saver));
	core->Script()->Machine()->RegisterObjectProperty("SceneScriptCore", "SceneScriptCoreMediaLibrary MediaLibrary",offsetof(ScriptModuleCore, media_library));
	
	core->Script()->Machine()->RegisterObjectProperty("SceneScriptCore", "SceneScriptCoreOnlineUserStats Online",offsetof(ScriptModuleCore, online_user_stats));
	core->Script()->Machine()->RegisterObjectProperty("SceneScriptCoreOnlineUserStats", "SceneScriptCoreFacebook Facebook",offsetof(OnlineUserStats, facebook));
	core->Script()->Machine()->RegisterObjectProperty("SceneScriptCoreOnlineUserStats", "SceneScriptCoreVK VK",offsetof(OnlineUserStats, vk));
	core->Script()->Machine()->RegisterObjectProperty("SceneScriptCoreOnlineUserStats", "SceneScriptCoreInstagram Instagram",offsetof(OnlineUserStats, instagram));
	
	core->Script()->Machine()->RegisterObjectProperty("SceneScriptCore", "SceneScriptCoreNetwork Network",offsetof(ScriptModuleCore, network));	
	core->Script()->Machine()->RegisterObjectProperty("SceneScriptCore", "SceneScriptCoreMobile Mobile",offsetof(ScriptModuleCore, mobile));	

	script_machine->RegisterGlobalProperty("SceneScriptCore core", this);		

	HackFinishScriptHlp();
}

void ScriptModuleCore::Exit()
{	
#if defined(PC) || defined(OSUNIX)
	core->Exit();
#endif

#if defined(ANDROID)
	CallFromJava("FinishActivity");
#endif
}

float ScriptModuleCore::GetDeltaTime()
{
	return core->GetDeltaTime();
}

static void EscapeStr(std::string& text)
{
	for (int i = 0, size = text.length(); i < size; ++i)
	{
		if (text[i] == '%')
		{
			text.replace(i, 1, "%%");

			++i;
			++size;
		}
	}
}

void ScriptModuleCore::Trace(std::string& text)
{
	std::string copy(text);
	EscapeStr(copy);

	core->TraceTo("Script", copy.c_str());
}

void ScriptModuleCore::TraceTo(std::string& to, std::string& text)
{
	std::string copy(text);
	EscapeStr(copy);

	core->TraceTo(to.c_str(), copy.c_str());
}

Platform ScriptModuleCore::GetCurrentPlatform()
{
	return core->GetCurrentPlatform();
}

void ScriptModuleCore::Work(int level, float dt)
{
	utils.Work();
}

void ScriptModuleCore::Reset()
{	
	network.Reset();
	online_user_stats.vk.Reset();
	online_user_stats.facebook.Reset();
	online_user_stats.instagram.Reset();
	mobile.Reset();
	media_library.Reset();

	core->DelTasks(this);
}

void ScriptModuleCore::GetDeviceVendor(std::string& device_vendor)
{
	device_vendor = core->info.device_vendor;
}

void ScriptModuleCore::GetDeviceName(std::string& device_name)
{
	device_name = core->info.device_name;
}

void ScriptModuleCore::GetDeviceID(std::string& device_id)
{
	device_id = core->info.device_id;
}

int64 ScriptModuleCore::Utils::GetInstallTime()
{
#ifdef ANDROID
	return android_extern_installationtime;
#endif

	struct stat flInfo;

#if defined(PC) || defined (OSUNIX)
	if (!stat("D3DX9_42.dll", &flInfo))
	{
		return flInfo.st_ctime;		
	}
#endif
    
#ifdef IOS
    NSString* nsName = [[NSString alloc] initWithFormat:@"project"];
    NSString* filePath = [[NSBundle mainBundle] pathForResource:nsName ofType:@""];
    
    if (filePath)
    {
        if (!stat([filePath UTF8String], &flInfo))
        {
            return flInfo.st_ctime;
        }
        
    }
    
    
#endif

	return 0;
}

void ScriptModuleCore::GetLocale(std::string& locale)
{
	locale = core->GetLocale();
}
