#include "Core/Core.h"

#ifdef IOS

#include "Platforms/IOS/IOSWrapper.h"

#endif

extern void CallFromJava(const char* function, const char* param);

OnlineUserStatsSystem::OnlineUserStatsSystem()
{
	last_caller = null;
	last_caller_data = null;

	last_rs_caller = null;
    last_rs_caller_data = null;

	item_name[0] = 0;
}

bool OnlineUserStatsSystem::Init()
{
    core->AddTask(this, this, (TaskHolder::Task)&OnlineUserStatsSystem::Work, -10, -1.f);
    
	facebook.Init();
	vk.Init();
	instagram.Init();

	return true;
}

void OnlineUserStatsSystem::StartItemBuy(ItemBuyListiner* listiner, void* listiner_data, const char* set_item_name)
{
	if (!last_caller)
	{
		last_caller = listiner;	
		last_caller_data = listiner_data;
		String::Copy(item_name, 128, set_item_name);
		core->TraceTo("OnlineUserStats", "StartItemBuy %s", item_name);	
    
#ifdef IOS
		ios_wrapper->StartPurchase(set_item_name);    
#endif
	}
}

void OnlineUserStatsSystem::StartRestorePurchases(ItemRestoreListiner* listiner, void* listiner_data)
{
    #ifdef IOS
        
    if (!last_rs_caller)
    {
        last_rs_caller = listiner;	
        last_rs_caller_data = listiner_data; 
        
		ios_wrapper->StartRestorePurchase();        
    }    
    
    #endif
}

void OnlineUserStatsSystem::Work(float dt, int level)
{
#if defined(PC)
	if (last_caller)
	{
		last_caller->OnItemBuySuccess(last_caller_data);
		last_caller = null;
	}
#endif

	facebook.Work(dt);
	vk.Work(dt);
	instagram.Work(dt);
}

#if defined(ANDROID) 
const char* OnlineUserStatsSystem::GetRequestedItemName()
{
	if (last_caller)
	{
		return item_name;
	}

	return null;
}

void OnlineUserStatsSystem::ResetItemName()
{
	item_name[0] = 0;
}

void OnlineUserStatsSystem::ResolveRequestedItem(bool state)
{
	if (last_caller)
	{
		if (state)
		{
			last_caller->OnItemBuySuccess(last_caller_data);
		}
		else
		{
			last_caller->OnItemBuyFail(last_caller_data);
		}

		last_caller = null;
	}
}

#endif

void OnlineUserStatsSystem::TrackUserAge(int age)
{
#ifdef ANDROID
	char str[32];
	sprintf(str, "%i", age);
	CallFromJava("TrackUserAge", str);
#endif
        
#if defined(IOS)
	ios_wrapper->TrackUserAge(age);     
#endif
}

void OnlineUserStatsSystem::TrackUserGender(const char* gender)
{
#ifdef ANDROID
	CallFromJava("TrackUserGender", gender);
#endif
    
#if defined(IOS)
	ios_wrapper->TrackUserGender(gender);         
#endif
}

void OnlineUserStatsSystem::AddEventParam(const char* name, const char* param)
{
#ifdef ANDROID
	CallFromJava("AddEventParamName", name);
	CallFromJava("AddEventParamValue", param);
#endif

#if defined(IOS)
	ios_wrapper->AddEventParam(name, param);    
#endif
}

void OnlineUserStatsSystem::TrackEvent(const char* name)
{
#ifdef ANDROID
	CallFromJava("TrackEvent", name);
#endif
    
#if defined(IOS)    
	ios_wrapper->TrackEvent(name);    
#endif
}

FacebookNet* OnlineUserStatsSystem::Facebook()
{
	return &facebook;
}

VKNet* OnlineUserStatsSystem::VK()
{
	return &vk;
}

InstagramNet* OnlineUserStatsSystem::Instagram()
{
	return &instagram;
}

void OnlineUserStatsSystem::Reset()
{
	facebook.Reset();
	vk.Reset();
}

void OnlineUserStatsSystem::Release()
{

}