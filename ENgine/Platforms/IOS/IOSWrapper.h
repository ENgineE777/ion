
#pragma once
#include "Core/ICore.h"

class IOSWrapper
{
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

	IOSWrapper();

	void VKSignIn();	
	void VKSignOut();
	void FBSignIn(vector<string>& req_permissions);
	void FBStartRequestPermistions(vector<string>& req_permissions);
	void FBSignOut();
    void InstagramSignIn();
	void OpenVideo(const char* path, const char* showName);
	void ShowPopup(const char* title, const char* text, const char* btn_text);
	void ShowPopup(const char* title, const char* text, const char* btn_text, const char* btn_cancel_text);
	void DismissModalViewControllerAnimated();
	void CallNumber(const char* number);
	void StartSelectImage(int type);
	void ShowSoftKeyboard(IControlsService::KeyboardType kb_type, IControlsService::KeyboardReturnKey kb_ret_key);
	void HideSoftKeyboard();
	void OpenURL(const char* url, bool inner);
    void OpenURL(const char* url, const char* stop_url);

	void SendMail(const char* address, const char* subject, const char* body);
	void StartShareText(const char* text, const char* extra_data);
	void StartShareImage(const char* image);

    float GetBatteryLevel();
    int   GetBatteryStatus();
    
	void StartMonitoringRegion(const char* id, const char* uuid);
	void StopMonitoringRegion(const char* id);
	void SetCallbackOnEnterRegion(const char* id, const char* func);
	void SetCallbackOnExitRegion(const char* id, const char* func);
	int GetRegionBeaconsCount(const char* id);
	int GetRegionBeaconMinor(const char* id, int index);
	int GetRegionBeaconMajor(const char* id, int index);
	int GetRegionBeaconProximity(const char* id, int index);	
	float GetRegionBeaconAccuracy(const char* id, int index);
	void SetPushServerURL(const char* url);
	void StartRegisterInPush();
	int GetTittleBarHeight();
	void AddIOSEvent();
	void AddEventToCalendar(int day, int month, int year, int hour, int minute, int second,
                            const char* name, const char* desc, const char* location);
			
	void ShowMap(bool show);
	void SetMapPosition(float x, float y, float w, float h);
	void AddObjectToMap(float x, float y, const char* text);
	void DeleteAllMapObjects();

	void StartPurchase(const char* id);
	void StartRestorePurchase();
	void TrackUserAge(int age);
	void TrackUserGender(const char* gender);
	void AddEventParam(const char* name, const char* param);
	void TrackEvent(const char* name);
    
    void EnableTrackGPS(bool enable);
    void PrepareCameraFeed();
};

extern IOSWrapper* ios_wrapper;