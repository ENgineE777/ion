
#pragma once

#include <string>
class ScriptMachine;

#include "Common/Common.h"
#include "Core/OnlineUserStats/Facebook.h"
#include "Core/OnlineUserStats/VK.h"
#include "Core/OnlineUserStats/Instagram.h"
#include "Core/PushNotification/PushNotificationService.h"
#include "Core/Mobile/MobileService.h"
#include "Core/MediaLibrary/MediaLibraryService.h"
#include "Core/Script/ScriptModule.h"
#include "angelscript.h"

class ScriptModule;

class ScriptModuleCore : public TaskHoldersOwner
{
public:

	class Controls
	{
	public:

		bool  IsKey(int key, int action);
		float GetControlValue(int key);
		bool  IsAnyKeyPressed();
		bool  CheckTouch(int index, int act);
		float GetTouchX(int index);
		float GetTouchY(int index);
		void  HideSoftKeyboard();
		bool  IsShowSoftKeyboard();
		int   GetSoftKeyboardHeight();
		float GetGyro(int index);
		void  SetGyro(int index, float val);
		bool  IsBackPressed();
	};

	class Scene
	{
	public:

		void  LoadScene(std::string& sceneName);
		void  UnloadScene(std::string& sceneName);
		bool  IsSceneLoading(std::string& sceneName);
		float GetLoadingProgress(std::string& sceneName);
		bool  ActivateInScene(std::string& sceneName, std::string& entityName, bool act);		
	};

	class Render
	{
	public:

		float GetWidth();
		float GetHeight();
		float GetReferenceWidth();
		float GetReferenceHeight();

		int   GetScreenDPI();	

		float HorzRefToScreen(float sz);		
		float VertRefToScreen(float sz);		
		float HorzScreenToRef(float sz);		
		float VertScreenToRef(float sz);

		void  PrintText(float x, float y, std::string& text);
		void  DrawLine2D(Vector2& p1, Vector& color1, Vector2& p2, Vector& color2);
				
		void  MakeScreenshot(std::string& out_image);
	};

	class UserStats
	{
	public:

		void   SetFloat(std::string& id, float def_value, float value, bool add);	
		void   SetInt(std::string& id, int def_value, int value, bool add);		
		void   SetVector(std::string& id, Vector& def_value, Vector& value);		
		void   SetString(std::string& id, std::string& def_value, std::string& value);		

		float  GetFloat(std::string& id, float def_value);
		int	   GetInt(std::string& id, int def_value);		
		Vector GetVector(std::string& id, Vector& def_value);		
		void   GetString(std::string& id, std::string& def_value, std::string& value);		

		void   SetFloatHash(int hash1, int hash2, float def_value, float value, bool add);	
		void   SetIntHash(int hash1, int hash2, int def_value, int value, bool add);		
		void   SetVectorHash(int hash1, int hash2, Vector& def_value, Vector& value);		
		void   SetStringHash(int hash1, int hash2, std::string& def_value, std::string& value);		

		float  GetFloatHash(int hash1, int hash2, float def_value);
		int	   GetIntHash(int hash1, int hash2, int def_value);		
		Vector GetVectorHash(int hash1, int hash2, Vector& def_value);		
		void   GetStringHash(int hash1, int hash2, std::string& def_value, std::string& value);		

		void GetHashes(std::string& id, int& hash1, int& hash2);

		bool Load();
		void Save();
	};

	class Utils
	{
#ifdef EDITOR
    private:
        float  nYear;
        float  nMonth;
        float  nDay;
        float  nHour;
        float  nMinute;
        bool   bTimeSimulation;
	public:
        void  SetSimulationTime(float year, float month, float day, float hour, float minute);
        void  SetTimeSimulation(bool bFlag){bTimeSimulation = bFlag;}

        Utils():nYear(0),nMonth(0),nDay(0),nHour(0),nMinute(0),bTimeSimulation(false)
        {}
#endif
	public:

        int evnt_day;
        int evnt_month;
        int evnt_year;
        int evnt_hour;
        int evnt_minute;
        int evnt_second;
        string evnt_name;
        string evnt_desc;
        string evnt_location;		

		float Rnd();
		void  GetRayDir(Vector& pos, Vector& dir);	
		void  GetScreenPos(Vector& pos, Vector& screen_pos);
		void  GetCamDir(Vector& dir);		
		float GetHour();
		float GetMinute();
		float GetSecond();	
		float GetDay();	
		float GetMonth();	
		float GetYear();	
		int64 GetSystemTime();	
		int   ParseUnixTime(int index, uint64 time);	
		void  GetDateTimeUTC(int& year, int& month, int& day, int& hour, int& min, int& sec);			
		int64 GetTimestamp(int year, int month, int day, int hour, int minute, int second);
		int   GetWeekOfDay(int day, int month, int year);
		float GetFps();
		int   CalcUtf8Len(std::string& text);
		void  ClampUtf8(string& text, string& res, int clamp_len);
		void  Int64ToString(int64 val, std::string& str);
		void  UInt64ToString(uint64 val, std::string& str);		
		void  ConvertASCIIToUTF8(std::string& wstr, std::string& str);
		void  GetMD5Hash(std::string& str, std::string& hash);
		void  GetUID(std::string& postfix, std::string& uid);
		void  GetISO8601DateTime(std::string& str);
		void  GetISO8601DateTimeForAzureQuerry(std::string& str);
		void  ExtractFileName(std::string& FullPath, std::string& FileName);
		void  OpenURL(std::string& url, bool inner);	
		void  OpenURL2(std::string& url, std::string& stop_url);			
		void  Work();
		void  HTMLToText(string& in, string& out);
		void  LowerCase(string& in, string& out);        
		void  UpperCase(string& in, string& out);     
		void  Replace(string& str,string& find,string& replace);
		void  PrintTextOnPhoto(std::string& text, std::string& input_photo,  std::string& output_photo, std::string& output_ext);		
		void  OpenVideo2params(std::string& path, std::string& showName);
		void  OpenVideo1param(std::string& path);
		
		void  StartMonitoringRegion(std::string& id, std::string& uuid);
		void  StopMonitoringRegion(std::string& id);
		void  SetCallbackOnEnterRegion(std::string& id, std::string& func);
		void  SetCallbackOnExitRegion(std::string& id, std::string& func);
		int   GetRegionBeaconsCount(std::string& id);
		int   GetRegionBeaconMinor(std::string& id, int index);
		int   GetRegionBeaconMajor(std::string& id, int index);
		int   GetRegionBeaconProximity(std::string& id, int index);
		float GetRegionBeaconAccuracy(std::string& id, int index);	

		float QuadraticEaseIn(float p);
		float QuadraticEaseOut(float p);
		float QuadraticEaseInOut(float p);

		float CubicEaseIn(float p);
		float CubicEaseOut(float p);
		float CubicEaseInOut(float p);

		float QuarticEaseIn(float p);
		float QuarticEaseOut(float p);
		float QuarticEaseInOut(float p);

		float QuinticEaseIn(float p);
		float QuinticEaseOut(float p);
		float QuinticEaseInOut(float p);

		float SineEaseIn(float p);
		float SineEaseOut(float p);
		float SineEaseInOut(float p);

		float CircularEaseIn(float p);
		float CircularEaseOut(float p);
		float CircularEaseInOut(float p);
	
		float ExponentialEaseIn(float p);
		float ExponentialEaseOut(float p);
		float ExponentialEaseInOut(float p);

		float ElasticEaseIn(float p);
		float ElasticEaseOut(float p);
		float ElasticEaseInOut(float p);

		float BackEaseIn(float p);
		float BackEaseOut(float p);
		float BackEaseInOut(float p);

		float BounceEaseIn(float p);
		float BounceEaseOut(float p);
		float BounceEaseInOut(float p);	

		void PrintCallStack();
		void CopyFileScript(std::string& scr, std::string& dest);

		int64 GetInstallTime();
	};

	class DataFile
	{
		class IStreamLoader* loader;

	public:

		DataFile();

		bool Start(std::string& name, std::string& start_block);
		bool StartEx(std::string& name, std::string& start_block, int file_type);
        bool StartFromRawData(std::string& buffer, std::string& start_block, int file_type);
		void Finish();

		bool EnterBlock(std::string& name);
		void LeaveBlock();

		bool ReadString(std::string& name, std::string& val);		
		bool ReadBool(std::string& name, bool& val);
		bool ReadFloat(std::string& name, float& val);
		bool ReadInt(std::string& name, int& val);	
		bool ReadInt64(std::string& name, int64& val);	
		bool ReadUInt(std::string& name, dword& val);	
		bool ReadUInt64(std::string& name, uint64& val);			
		bool ReadVector(std::string& name, Vector& val);
	};

	class StreamSaver
	{
		class IStreamSaver* saver;

	public:

		StreamSaver();

		bool StartSS(std::string& name, int file_type);
		void FinishSS();

		void EnterBlockSS(std::string& name);
		void LeaveBlockSS(std::string& name);

		void WriteString(std::string& name, std::string& val);		
		void WriteBool(std::string& name, bool& val);
		void WriteFloat(std::string& name, float& val);
		void WriteInt(std::string& name, int& val);	
		void WriteInt64(std::string& name, int64& val);	
		void WriteVector(std::string& name, Vector& val);
	};

	class MediaLibrary
	{
	public:

		class MLFinishListiner : public MediaLibraryService::FinishListiner		
		{			
		public:
			
			ScriptFunction callback;		

			virtual void OnFinish(void* data, int res);							
		};	

		MLFinishListiner listiner;

		void StartScanning(asIScriptFunction* cb);
		int GetFilesCount();
		void GetFileName(int i, std::string& name, bool prepareForLoading);
		void GetFileNameThumbnail(int i, std::string& name, bool prepareForLoading);

		int GetYearCount();
		int GetMonthCount(int year);
		int GetDayCount(int year, int month);
		int GetImageCount(int year, int month, int day);

		int GetIndexByYear(int year);
		int GetIndexByMonth(int year, int month);
		int GetIndexByDay(int year, int month, int day);

		bool GetDate(int index, int& day, int& month, int& year);
		bool GetTime(int index, int& hour, int& minute, int& second);

		void Reset();
	};

	class Facebook
	{		
		class ReqListiner : public FacebookNet::FacebookRequestListiner		
		{			
		public:

			bool used;
			ScriptFunction callback;
			char param[128];
			
			ReqListiner()
			{
				used = false;				
				param[0] = 0;
			};

			virtual void OnRequestFinish(int res, void* data, const char* out_file);						
		};				

		std::vector<ReqListiner> listiners;		

		class SigoutListiner : public FacebookNet::FacebookSignoutListiner		
		{			
		public:
			
			ScriptFunction callback;						

			virtual void OnFinish(void* data);		
		};	

		SigoutListiner sign_out_listiner;

	public:
		
		Facebook()
		{
			listiners.resize(32);
		}

		ReqListiner* GetLisiten(asIScriptFunction* cb);

		void SignIn(asIScriptFunction* cb);
		void AddRequestPermistion(std::string& name);
		void StartRequestPermistions(asIScriptFunction* cb);
		void SignOut(asIScriptFunction* cb);
		bool GetSignInStatus();			
		void FBStartRequest(std::string& method, std::string& url, std::string& params, std::string& post_params, asIScriptFunction* cb);	
		void FBStartRequest2(std::string& method, std::string& url, std::string& params, std::string& post_params, asIScriptFunction* cb, std::string& req_param);	
		void StartRequestInner(std::string& method, std::string& url, std::string& params, std::string& post_params, asIScriptFunction* cb, const char* req_param);	
		void UploadFile(std::string& url, std::string& file_name, std::string& param_name, std::string& message, asIScriptFunction* cb);
		void SetGuestAccessTokenURL(std::string& url);
		void SetApiURL(std::string& url);
		void Reset();
	};

	class VK
	{		
		class ReqListiner : public VKNet::VKRequestListiner		
		{			
		public:

			bool used;
			ScriptFunction callback;	
			char param[128];

			ReqListiner()
			{
				used = false;				
				param[0] = 0;
			};

			virtual void OnRequestFinish(int res, void* data, const char* out_file);			
		};				

		std::vector<ReqListiner> listiners;		

		class SigoutListiner : public VKNet::VKSignoutListiner		
		{			
		public:
			
			ScriptFunction callback;						

			virtual void OnFinish(void* data);		
		};	

		SigoutListiner sign_out_listiner;

	public:

		VK()
		{
			listiners.resize(32);
		}

		ReqListiner* GetLisiten(asIScriptFunction* cb);

		void VkSignIn(asIScriptFunction* callback);
		void VkSignOut(asIScriptFunction* callback);
		void VkGetUserID(std::string& user_id);
		int  VkGetSignInStatus();		
		void VkStartRequest(std::string& url, std::string& params, std::string& post_params, asIScriptFunction* cb);	
		void VkStartRequest2(std::string& url, std::string& params, std::string& post_params, asIScriptFunction* cb, std::string& req_param);	
		void StartRequestInner(std::string& url, std::string& params, std::string& post_params, asIScriptFunction* cb, const char* req_param);	
		void VKUploadFile(std::string& url, std::string& file_name, asIScriptFunction* cb);
		
		void VkSetApiURL(std::string& url);		

		void Reset();
	};

	class Instagram
	{						
		class SigInOutListiner : public InstagramNet::InstagramListiner	
		{			
		public:

			ScriptFunction callback;			

			virtual void OnFinish(void* data);		
		};	

		SigInOutListiner sign_in_listiner;
		SigInOutListiner sign_out_listiner;

	public:
				
		void InstagramSignIn(asIScriptFunction* cb);
		void InstagramSignOut(asIScriptFunction* cb);
		void InstagramGetAccessToken(std::string& user_id);		
		void Reset();
	};

	class OnlineUserStats
	{
	public:

		Facebook facebook;		
		VK vk;	
		Instagram instagram;
		
		void TrackUserAge(int age);
		void TrackUserGender(std::string& gender);
		void AddEventParam(std::string& name, std::string& param);
		void TrackEvent(std::string& name);
		
	};

	class Network
	{
	public:				

		class ReqListiner : public NetworkService::RequestListiner
		{			
		public:

			bool used;
			ScriptFunction callback;

			char param[128];			

			ReqListiner()
			{
				used = false;				
				param[0] = 0;
			};

			virtual void OnRequestFinish(int res, void* data, const char* file_name);			
		};

		std::vector<ReqListiner> listiners;

		Network()
		{
			listiners.resize(128);
		}

		bool IsInternetConnected();		

		void NetStartRequest(std::string& method, std::string& request, std::string& post_params, std::string& file_name, asIScriptFunction* cb);
		void NetStartRequest2(std::string& method, std::string& request, std::string& post_params, std::string& file_name, asIScriptFunction* cb, std::string& req_param);
		void StartRequestInner(std::string& method, std::string& request, std::string& post_params, std::string& file_name, asIScriptFunction* cb, const char* req_param);

		void  Reset();
	};
	
	class Mobile
	{
	public:

		class ReqListiner : public MobileService::RequestListiner
		{			
		public:

			bool used;			
			ScriptFunction callback;	

			ReqListiner()
			{
				used = false;				
			};

			virtual void OnRequestFinish(void* data, int res);			
		};

		class ImageReqListiner : public MobileService::ImageRequestListiner
		{			
		public:

			bool used;			
			ScriptFunction callback;		

			ImageReqListiner()
			{
				used = false;								
			};

			virtual void OnRequestFinish(void* data, int res, const char* image);			
		};

		class PopupListiner : public MobileService::PopupListiner
		{
		public:

			bool used;

			ScriptFunction callback_ok;			
			ScriptFunction callback_cancel;			

			PopupListiner()
			{			
				used = false;				
			};

			virtual void OnOkPressed(void* data);		
			virtual void OnCancelPressed(void* data);		
		};

		class PushListener : public MobileService::PushListener
		{			
		public:
			bool used;

			ScriptFunction callback;			

			PushListener()
			{
				used = false;				
			};

			virtual void OnGetUserData(void* data, const char* userdata);			
		};				

		class ShareListener : public MobileService::ShareListener
		{			
		public:			

			ScriptFunction callback;

			virtual void OnEvent(void* data, const char* share_type);			
		};	

		class SystemListener : public MobileService::SystemListener
		{			
		public:			

			ScriptFunction callback;

			virtual void OnEvent(void* data);			
		};	

		class ScanListener : public MobileService::ScanListener
		{
		public:

			ScriptFunction callback;

			virtual void OnEvent(void* data, const char* scaned_data);
		};

		class GPSListiner : public MobileService::GPSListiner
		{
		public:

			ScriptFunction callback;

			virtual void OnUpdate(void* data, int state, float latitude, float longitude);
		};

		void SetPushListener(asIScriptFunction* cb);

		ReqListiner req_access;
		ReqListiner req_read_contacts;
		ReqListiner req_read_avatars;
		ReqListiner req_send_sms;
		PopupListiner popup_listiner;
		ImageReqListiner req_select_image;
		std::vector<PushListener> listeners;

		ShareListener share_listiner;	
		SystemListener onpause_listiner;	
		SystemListener onresume_listiner;	
		ScanListener scane_listiner;
		GPSListiner gps_listiner;

		void SetOnResumeCallback(asIScriptFunction* cb);
		void SetOnPauseCallback(asIScriptFunction* cb);

		bool IsReadContactsAllowed();		
		void RequestAccessToContacts(asIScriptFunction* cb);

		void StartReadContacts(asIScriptFunction* cb);
		void StartReadContactsAvatars(asIScriptFunction* cb);
		int  GetContactsCount();
		void GetContactData(int i, std::string& name, std::string& texture, std::string& numbers);
		void SendSms(std::string& number, std::string& text, asIScriptFunction* cb);
		bool IsAllowCall();
		void CallNumber(std::string& number);		
		void GetSimOperatorName(std::string& name);		
		void StartSelectImage(int type, asIScriptFunction* cb);

		void SetPushServerURL(std::string& url);
		void StartRegisterInPushService();

		int  GetTittleBarHeight();

		void SendMail(std::string& address, std::string& subject, std::string& body);
		void StartShareText(std::string& text, std::string& extra_data, asIScriptFunction* cb);
		void StartShareImage(std::string& image, asIScriptFunction* cb);
		void AddEventToCalendar(int day, int month, int year, int hour, int minute, int second,
								 std::string& name, std::string& desc, std::string& location);

		void ShowMap(bool show);
		void SetMapPosition(float x, float y, float w, float h);
		void AddObjectToMap(float x, float y, std::string& text);
		void DeleteAllMapObjects();	

		void ShowNativeMap(float x, float y, std::string& text);	

		void ShowPopup(std::string& title, std::string& text, std::string& btn_text, asIScriptFunction* callback_ok);
		void ShowPopup2(std::string& title, std::string& text, std::string& btn_ok_text, asIScriptFunction* callback_ok, std::string& btn_cancel_text, asIScriptFunction* callback_cancel);

		float GetBatteryLevel();
		int   GetBatteryStatus();

		void  GetInstallReferer(string& referer);

		void StartScanBarcode(asIScriptFunction* cb);

		void GetCameraTextureSize(int& width, int& height);

		void EnableTrackGPS(bool enable, asIScriptFunction* callback);		

		void Reset();
	};

	void Init(ScriptMachine* script_machine);
			
	void  Exit();
	float GetDeltaTime();		
	void  Trace(std::string& text);		
	void  TraceTo(std::string& to, std::string& text);			
	void  GetDeviceVendor(std::string& device_vendor);
	void  GetDeviceName(std::string& device_name);
	void  GetDeviceID(std::string& device_id);
	void  GetLocale(std::string& locale);	

	Platform GetCurrentPlatform();	

	void  Work(int level, float dt);
	void  Reset();

#ifdef EDITOR
        void  SetSimulationTime(float year, float month, float day, float hour, float minute){utils.SetSimulationTime(year,month,day,hour,minute);}
        void  SetTimeSimulation(bool bFlag){utils.SetTimeSimulation(bFlag);}
#endif

protected:

	Controls        controls;
	Scene           scene;
	Render          render;
	UserStats       user_stats;
	Utils           utils;	
	DataFile        data_file;
	StreamSaver     stream_saver;
	MediaLibrary    media_library;
	OnlineUserStats online_user_stats;
	Network         network;		
    Mobile          mobile;
};