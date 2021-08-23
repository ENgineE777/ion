
#pragma once

#include "Common/Common.h"
#include "Common/TaskHolder.h"
#include "Date.h"

#ifdef IOS
#import <AssetsLibrary/AssetsLibrary.h>
#endif

#ifndef MAX_PATH
#define MAX_PATH          2048
#endif

#ifdef IOS
@interface ImageRotation : NSObject
{
}
+ (CGImageRef)CGImageRotatedByAngle:(CGImageRef)imgRef angle:(CGFloat)angle;
@end
#endif

class MediaLibraryService : public TaskHoldersOwner
{	
public:
	
	MediaLibraryService();
	virtual bool  Init();
	virtual void Work(float dt, int level);
	virtual void  Release();

	class FinishListiner
	{
	public:

		virtual void OnFinish(void* data, int res) = 0;		
	};
	
	void StartScanning(FinishListiner* listiner, void* data);
	int  GetFilesCount();
	void GetFileName(int i, std::string& name, bool prepareForLoading);
	void GetFileNameThumbnail(int i, std::string& name, bool prepareForLoading);

	float scanning_time;
	FinishListiner* on_end_callback;
	void* on_end_callback_data;	
    
    enum State
	{
		Idle,
		Done,
		ReadyToFinalize,
		ScanInProgress,
		LastScanFailed
	};
#ifdef IOS
	void SetState(MediaLibraryService ::State state) { this->state = state; }
	void AddItem(int day, int month, int year, const char* path, ALAsset* asset);
	void AddApp(unsigned long app_id, const char* name, const char* icon);
	void SetScanningAppsDone(bool value) { scanning_apps_done = value; }
    #endif

	void Scan();
	void FinalizeScan();

	int GetYearCount();
	int GetMonthCount(int year);
	int GetDayCount(int year, int month);
	int GetImageCount(int year, int month, int day);

	int GetYearByIndex(int index);
	int GetMonthByIndex(int year, int index);
	int GetDayByIndex(int year, int month, int index);

	int GetIndexByYear(int year);
	int GetIndexByMonth(int year, int month);
	int GetIndexByDay(int year, int month, int day);

	int GetImageFullNameByYear(int index, std::string& image_full_name);
	int GetImageFullNameByMonth(int year, int index, std::string& image_full_name);
	int GetImageFullNameByDay(int year, int month, int index, std::string& image_full_name);
	int GetImageFullName(int year, int month, int day, int index, std::string& image_full_name);

	bool GetDate(int index, int& day, int& month, int& year);
	bool GetTime(int index, int& hour, int& minute, int& second);

	void ScanApplications();
	bool IsScanningApplicationsDone() { return scanning_apps_done; }

#ifdef ANDROID
	void SetScanApplicationResult(const char* result_str);
#endif

	int GetApplicationCount();
	bool GetApplicationId(int index, std::string& app_id);
	bool GetApplicationName(int index, std::wstring& app_name);
	bool GetApplicationIcon(int index, std::string& icon_name);


	class RequestListener
	{
	public:
		virtual void OnRequestSuccess(void* data) = 0;
		virtual void OnRequestFail(void* data) = 0;
	};

	class ReqestWorker : public ThreadWorker
	{
	public:
		MediaLibraryService* owner;

		virtual void Prepare();
		virtual void Loop();
	};

	struct ImageDesc
	{
		ImageDesc() : index(0), name(NULL), next_by_year(NULL), next_by_month(NULL), next_by_day(NULL), next(NULL)
        {
#ifdef IOS
            asset = nil;
#endif        
        };
        
        ~ImageDesc()
        {
#ifdef IOS
            //if (asset != nil)
                //[asset release];
            asset = nil;
#endif
        }
		ImageDesc(const ImageDesc& assign)
		{
			Copy(assign);
		}

		ImageDesc& operator=(const ImageDesc& assign)
		{
			if (&assign == this) return *this;
			Copy(assign);
			return *this;
		}

		void Copy(const ImageDesc& assign)
		{
			creation_date = assign.creation_date;

			String::Copy(full_name, sizeof(full_name), assign.full_name);
			name = full_name + (assign.name - assign.full_name);

			index = assign.index;

			next_by_year = assign.next_by_year;
			next_by_month = assign.next_by_month;
			next_by_day = assign.next_by_day;
			next = assign.next;
#ifdef IOS
            asset = assign.asset;
            [asset retain];
            
#endif
		}

		int index;

		Date creation_date;

		char* name;
		char full_name[MAX_PATH];

		ImageDesc* next_by_year;
		ImageDesc* next_by_month;
		ImageDesc* next_by_day;
		ImageDesc* next;
        
#ifdef IOS
        ALAsset* asset;
#endif
	};

	struct AppDesc
	{
		std::string app_id;
		std::wstring full_name;
		std::string icon_full_name;
	};

	private:
		void Scan(const char* path, int depth);

		ImageDesc* GetImageByYear(int year);
		ImageDesc* GetImageByMonth(int year, int month);
		ImageDesc* GetImageByDay(int year, int month, int day);
		ImageDesc* GetImage(int year, int month, int day, int index);

		State state;

		std::vector<ImageDesc> images;

		bool scanning_apps_done;
		std::vector<AppDesc> apps;

		ReqestWorker worker;

};
