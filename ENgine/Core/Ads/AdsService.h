#pragma once

#include "Common/Common.h"
#include "Common/TaskHolder.h"

class AdsService : public TaskHoldersOwner
{
public:
	class Listener
	{
	public:
		virtual void OnRequestFinish(int res, void* data) = 0;		
	};

	struct Request
	{
		Request() : data(NULL), used(false), done(false)
		{
		}

		bool used;
		bool done;

		void* data;
		Listener* listener;
	};

	class NetworkListener : public INetworkService::RequestListiner
	{
	public:
		virtual void OnRequestFinish(int res, void* data, const char* file_name) {};
	};

public:
	struct AdsData
	{
		std::string back_url;
		std::string click_url;
		std::string dev_name;
		std::string desc;
		std::string panel_small;
		std::string panel_large;
		std::string app_type;
		std::string name;
		std::string icon_url;
		std::string banner_small;
		std::string banner_middle;
		std::string banner_large;
		std::string show_cb_url;
	};

	AdsService();

	virtual bool Init();
	virtual void Work(float dt, int level);
	virtual void Reset();
	virtual void Release();

	void ProcessData(const char* data);

	virtual void RequestAds(Listener* listener, void* reqest_data);

	void HandleAdsClick(const char* back_url, const char* click_url);
	void HandleAdsShow(const char* show_cb_url);

	void ShowInterstitial();

	int GetAdsCount();

	bool GetAdsBackUrl(int index, std::string& value);
	bool GetAdsClickUrl(int index, std::string& value);
	bool GetAdsDevName(int index, std::string& value);
	bool GetAdsDesc(int index, std::string& value);
	bool GetAdsPanelSmall(int index, std::string& value);
	bool GetAdsPanelLarge(int index, std::string& value);
	bool GetAdsAppType(int index, std::string& value);
	bool GetAdsName(int index, std::string& value);
	bool GetAdsIconUrl(int index, std::string& value);
	bool GetAdsBannerSmall(int index, std::string& value);
	bool GetAdsBannerMiddle(int index, std::string& value);
	bool GetAdsBannerLarge(int index, std::string& value);
	bool GetAdsShowCbUrl(int index, std::string& value);

private:
	NetworkListener network_listener;

	Request request;
	std::vector<AdsData> ads_data;
};
