#pragma once

#include "Common/Common.h"
#include "Common/TaskHolder.h"

class PushNotificationService : public TaskHoldersOwner
{
public:
	class Listener
	{
	public:
		virtual void OnGetUserData(void* data, const char* userdata) = 0;
	};

	struct ListenerDesc
	{
		ListenerDesc() : data(NULL)
		{
		}

		void* data;
		Listener* listener;
	};

public:
	PushNotificationService();

	virtual bool Init();
	virtual void Work(float dt, int level);
	virtual void Release();

	void AddListener(Listener* listener, void* data);
	void DelListener(Listener* listener);

	void ScheduleData(const char* userdata);
	void ProcessData(const char* userdata);

private:
	bool need_process_scheduled_data;
	std::string scheduled_data;

	typedef std::vector<ListenerDesc> ListenerVector;
	ListenerVector listeners;
};
