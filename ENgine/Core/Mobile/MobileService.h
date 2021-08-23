
#pragma once

#include "Common/Common.h"
#include "Common/TaskHolder.h"

#ifdef IOS
#import "MessageUI/MessageUI.h"
#import <AddressBook/AddressBook.h>

@interface IOSMessage: NSObject <MFMessageComposeViewControllerDelegate>
- (void) messageComposeViewController:(MFMessageComposeViewController *)controller didFinishWithResult:(MessageComposeResult)result;
- (IBAction) sendSMS:(const char*)number text:(const char *)text;
@end
#endif

class MobileService : public TaskHoldersOwner
{	
public:
	
	class PushListener
	{
	public:
		virtual void OnGetUserData(void* data, const char* userdata) = 0;
	};


    class ClipboardListener
    {
    public: 
        virtual void PasteFromClipboard(const char* sBuffer) = 0;
        virtual void OnRequestIsClipboardEmpty(int bClipboardEmpty) = 0;
    };

	struct ListenerDesc
	{
		ListenerDesc() : data(NULL)
		{
		}

		void* data;
		PushListener* listener;
	};

	class SystemListener
	{
	public:
		virtual void OnEvent(void* data) = 0;
	};	

	class ShareListener
	{
	public:
		virtual void OnEvent(void* data, const char* share_type) = 0;
	};	

	class ScanListener
	{
	public:
		virtual void OnEvent(void* data, const char* scaned_data) = 0;
	};

	class ReqestWorker : public ThreadWorker
	{
	public:
		MobileService* owner;

		virtual void Prepare();
		virtual void Loop();
	};	

	class RequestListiner
	{
	public:
		virtual void OnRequestFinish(void* data, int res) = 0;		
	};

	class PopupListiner
	{
	public:
		virtual void OnOkPressed(void* data) = 0;		
		virtual void OnCancelPressed(void* data) = 0;		
	};

	class GPSListiner
	{
	public:
		virtual void OnUpdate(void* data, int state, float latitude, float longitude) = 0;		
	};

	class ImageRequestListiner
	{
	public:
		virtual void OnRequestFinish(void* data, int res, const char* image) = 0;		
	};

	struct NumberEntry
	{
		std::string type;
		std::string number;

		NumberEntry()
		{
		};

		NumberEntry(const char* set_type, const char* set_number)
		{
			type = set_type;
			number = set_number;
		};
	};

	struct ContactData
	{
		std::string name;
		std::string texture;
		vector<NumberEntry> numbers;
		std::string out_number;
	};

	enum State
	{
		Idle,
		Reading,
		Done	
	};	

	State state;
	State state_read_contacts;
	std::vector<ContactData> contacts;
	
	float call_sms_callback_time;
	
	Str64 simOperatorName;

	bool smsResultReady;
	int smsResult;

	bool   imageReady;
	int    imageResult;
	string image_name;

	bool show_map;

	int  show_popup;
	PopupListiner* popup_listiner;
	void* popup_listiner_data;
	int  popup_result;
	bool set_allow_call;
    ClipboardListener*  clipboard_listener;

	ShareListener* share_listiner;
	void* share_listiner_data;
	string share_listiner_res;

	class Texture* cam_texture;

	GPSListiner* gps_listiner;
	void*        gps_data;
	int			 gps_ready;
	int			 gps_state;
	float		 gps_latitude;
	float		 gps_longitude;

#ifdef PC
	bool read_contacts_allowed;
	class TextureInstance* map_tex;	
	class TextureInstance* white_tex;	
	string popup_title;
	string popup_text;
	string popup_ok;
	string popup_cancel;
	Vector2 map_pos;
	Vector2 map_size;	

	struct MapObject
	{
		float x,y;
		string desc;

		MapObject(float set_x, float set_y, const char* set_desc)
		{
			x = set_x;
			y = set_y;
			desc = set_desc;
		};
	};
	vector<MapObject> mapObjects;
#endif

#ifdef IOS    
    bool access_request_finished;
    ABAddressBookRef addressBook;
#endif	
		
	ReqestWorker worker;

	float scanning_time;
	RequestListiner* reqest_access_callback;
	void*            reqest_access_data;
	RequestListiner* read_contacts_callback;
	void*			 read_contacts_data;
	RequestListiner* read_contacts_avatars_callback;	
	void*            read_contacts_avatars_data;
	RequestListiner* send_sms_callback;       
	void*            send_sms_data;
	ImageRequestListiner* select_image_callback;  
	void*                 select_image_data;

	ScanListener* scan_listener;
	void* scan_data;

	MobileService();
	virtual bool Init();
	virtual void Work(float dt, int level);
	virtual void Release();	

	bool IsReadContactsAllowed();
	
	void RequestAccessToContacts(RequestListiner* on_end_callback, void* data);

	void StartReadContacts(RequestListiner* on_end_callback, void* data);
	void StartReadContactsAvatars(RequestListiner* on_end_callback, void* data);

	int  GetContactsCount();
	void GetContactData(int index, std::string& name, std::string& texture, std::string& numbers);
	
	void SetContactsCount(int num);
	void SetContactData(int index, const char* name, const char* numbers);
	void SetContactAvatar(const char* name, const char* texture);

	const char* GetSimOperatorName();	
	void SetSimOperatorName(const char* name);
		
	bool IsAllowCall();
	void SetAllowCall(bool set_allow);
	void CallNumber(const char* number);
	void SendSms(const char* number, const char* text, RequestListiner* on_end_callback, void* callback_data);
	void SetSendSmsResult(int res);		

	void StartShareText(const char* text, const char* extra_data, ShareListener* listiner, void* listiner_data);
	void StartShareImage(const char* image, ShareListener* listiner, void* listiner_data);

	void StartSelectImage(int type, ImageRequestListiner* on_end_callback, void* callback_data);
	void SetSelectImageResult(int res, const char* image);
	void SetShareRes(const char* res);

	void FinalizeReadContatcs();
	void FinalizeReadAvatars();

	void ShowMap(bool show);
	void SetMapPosition(float x, float y, float w, float h);
	void AddObjectToMap(float x, float y, const char* text);
	void DeleteAllMapObjects();	

	void ShowNativeMap(float x, float y, const char* text);	

	void ShowPopup(const char* title, const char* text, const char* btn_text, PopupListiner* listiner, void* listiner_data);
	void ShowPopup(const char* title, const char* text, const char* btn_ok_text, const char* btn_cancel_text, PopupListiner* listiner, void* listiner_data);
	
	void SetPopupResult(int result);

	void AddPushListener(PushListener* listener, void* data);
	void DelPushListener(PushListener* listener);

	void ScheduleData(const char* userdata);
	void ProcessData(const char* userdata);

	void SetOnResumeListener(SystemListener* listener, void* data);
	void SetOnPauseListener(SystemListener* listener, void* data);

	void SetOnPause(bool set);
	void ExecutePauseCallbacks();

	void Reset();	

    void PasteFromClipboard(const char* text);
    void OnRequestIsClipboardEmpty(int bClipboardEmpty);

    void SetTextToClipboard(const char* str);
    void GetTextFromClipboard(ClipboardListener*  pListener);
    void IsClipboardEmpty(ClipboardListener*  pListener);

	float GetBatteryLevel();
	int   GetBatteryStatus();
	
	class Texture* GetCameraTexture();
	void GetCameraTextureSize(int& width, int& height);
	void StartScanBarcode(ScanListener* listener, void* data);

	void EnableTrackGPS(bool enable, GPSListiner* listener, void* data);
	void SetGPSData(int state, float latitude, float longitude);

private:

	bool need_process_scheduled_data;
	std::string scheduled_data;

	typedef std::vector<ListenerDesc> ListenerVector;	
	ListenerVector listeners;

	bool  is_paused;
	bool  pause_trigered;
	
	SystemListener* onpause_listiner;	
	void* onpause_listiner_data;
	SystemListener* onresume_listiner;
	void* onresume_listiner_data;

	void ReadAvatars();	
};



