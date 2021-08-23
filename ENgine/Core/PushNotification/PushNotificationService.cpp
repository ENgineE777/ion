#include "Core/Core.h"

#include "PushNotificationService.h"

PushNotificationService::PushNotificationService()
: need_process_scheduled_data(false)
{
}

bool PushNotificationService::Init()
{
	core->AddTask(this, this, (TaskHolder::Task)&PushNotificationService::Work, -320, -1.f);
	return true;
}

void PushNotificationService::Release()
{
}

void PushNotificationService::Work(float dt, int level)
{
	if (listeners.empty())
	{
		return;
	}

	if (need_process_scheduled_data)
	{
		need_process_scheduled_data = false;

		ProcessData(scheduled_data.c_str());
	}
}

void PushNotificationService::AddListener(Listener* listener, void* data)
{
	listeners.push_back(ListenerDesc());
	ListenerDesc& desc = listeners.back();

	desc.listener = listener;
	desc.data = data;
}

void PushNotificationService::DelListener(Listener* listener)
{
	for (int i = 0, size = listeners.size(); i < size; ++i)
	{
		ListenerDesc& desc = listeners[i];
		if (desc.listener == listener)
		{
			listeners.erase(listeners.begin() + i);
			break;
		}
	}
}

void PushNotificationService::ScheduleData(const char* userdata)
{
	need_process_scheduled_data = true;
	scheduled_data.assign(userdata);
}

void PushNotificationService::ProcessData(const char* userdata)
{
	for (int i = 0, size = listeners.size(); i < size; ++i)
	{
		ListenerDesc& desc = listeners[i];

		desc.listener->OnGetUserData(desc.data, userdata);
	}
}