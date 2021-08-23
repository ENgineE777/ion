#pragma once

#include "Facebook.h"
#include "VK.h"
#include "Instagram.h"

class OnlineUserStatsSystem : public TaskHoldersOwner
{
	FacebookNet facebook;
	VKNet vk;
	InstagramNet instagram;

public: 
	
	class ItemBuyListiner
	{
	public:
		virtual void OnItemBuySuccess(void* data) = 0;
		virtual void OnItemBuyFail(void* data) = 0;
	};

	class ItemRestoreListiner
	{
	public:
		virtual void OnItemRestoreSuccess(void* data, const char* id) = 0;
	};

	OnlineUserStatsSystem();
	virtual bool Init();	
	virtual void StartItemBuy(ItemBuyListiner* listiner, void* listiner_data, const char* item_name);
	virtual void StartRestorePurchases(ItemRestoreListiner* listiner, void* listiner_data);
	virtual void Release();
	
	void Work(float dt, int level);

#if defined(ANDROID) 
	const char* GetRequestedItemName();
	void ResetItemName();
	void ResolveRequestedItem(bool state);
#endif

	void TrackUserAge(int age);
	void TrackUserGender(const char* gender);
    void AddEventParam(const char* name, const char* param);
	void TrackEvent(const char* name);

	FacebookNet* Facebook();
	VKNet* VK();
	InstagramNet* Instagram();

	void Reset();

private:

	ItemBuyListiner* last_caller;
	void* last_caller_data;
	char item_name[128];

	ItemRestoreListiner* last_rs_caller;
	void* last_rs_caller_data;
};
