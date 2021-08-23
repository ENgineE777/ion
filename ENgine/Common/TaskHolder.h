#pragma once

class TaskHolder
{
public:
	typedef void (TaskHolder::*Task)(float timedelta,int level);

	virtual ~TaskHolder() {};
	virtual void Release() {};
};

class TaskHoldersOwner : public TaskHolder
{
	bool taskHoldersOwnerState;
public:

	TaskHoldersOwner()
	{
		taskHoldersOwnerState = true;
	};

	bool IsTaskHoldersOwnerActive() { return taskHoldersOwnerState; };
	void SetTaskHoldersOwnerActive( bool set) { taskHoldersOwnerState = set; };
};
