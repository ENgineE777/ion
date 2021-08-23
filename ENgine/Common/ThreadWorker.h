
#pragma once

#include "Common/Declarations.h"

#ifdef PC
#include <windows.h>
#endif

#if defined(IOS) || defined(OSX) || defined(ANDROID) || defined(OSUNIX) || defined(OSUNIX)
#include <pthread.h>
#endif

class DEF_EXPORT ThreadLocker
{
public:	

	~ThreadLocker()
	{
#if defined(PC)
		DeleteCriticalSection (&critSection);
#endif

	}

	void Init()
	{
#if defined(PC)
		InitializeCriticalSection (&critSection);
#endif
		locked = false;
	}

	void Lock() 
	{		
#if defined(PC)
		EnterCriticalSection (&critSection);
#endif

		locked = true;
	}

	void UnLock()
	{
#if defined(PC)
		LeaveCriticalSection (&critSection);		
#endif

		locked = false;
	}

	bool IsLocked()
	{
		return locked;
	}

private:

#if defined(PC)
	CRITICAL_SECTION critSection;
#endif

	bool locked;
};

class DEF_EXPORT ThreadWorker
{
	bool working;
	int processor;
	bool need_exit;

#if defined(PC)
	static DWORD WINAPI ThreadEntry ( void *pArg);
	HANDLE thread;
#endif

#if defined(IOS) || defined(OSX) || defined(ANDROID) || defined(OSUNIX) || defined(OSUNIX)
	static void *ThreadEntry(void* arg);
	pthread_t thread;
#endif

public:

	enum Priority
	{
		lowest,
		normal,
		highest
	};

	ThreadWorker();
	virtual ~ThreadWorker();	

	void SetProcessor(int set_processor);

	void Start(Priority priority);
	
	static void Sleep(int mili_sec);

	bool NeedExit();
	void Stop();

	bool IsWorking();

	virtual void Prepare() = 0;	
	virtual void Loop() = 0;
};