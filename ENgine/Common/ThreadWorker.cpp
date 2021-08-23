
#include "ThreadWorker.h"
#include "Core/ICore.h"

#if defined(OSUNIX) || defined(ANDROID)
#include <unistd.h>
#endif

ThreadWorker::ThreadWorker()
{
	processor = 1;
	thread = 0;
	working = false;
	need_exit = false;
}

ThreadWorker::~ThreadWorker()
{

}

///dsgdsfgdfsgsd
void ThreadWorker::SetProcessor(int set_processor)
{
	processor = set_processor;
}

void ThreadWorker::Start(Priority priority)
{
	Stop();
	Prepare();

	working = true;

#if defined(PC)
	thread = CreateThread (0, // Security attributes
						   256*1024, // Stack size
						   ThreadEntry, 
						   this, 
						   CREATE_SUSPENDED, 
						   NULL);

	int pr = THREAD_PRIORITY_NORMAL;

	if (priority == lowest)
	{
		pr = THREAD_PRIORITY_LOWEST;
	}
	else
	if (priority == highest)
	{
		pr = THREAD_PRIORITY_HIGHEST;
	}
	
	SetThreadPriority(thread,pr);	

#if defined(PC)
	SetThreadIdealProcessor(thread, processor-1);	
#endif

	working = true;
	ResumeThread (thread);
#endif	


#if defined(ANDROID) || defined(IOS) || defined(OSX)|| defined(OSUNIX)	
    pthread_create(&thread, NULL, ThreadEntry, (void*)this);    
	//Loop();   
	//working = false;
#endif
}

void ThreadWorker::Sleep(int mili_sec)
{
#ifdef PC
	::Sleep(mili_sec);
#else
    ::usleep(mili_sec * 1000);
#endif
}

#if defined(PC)
DWORD WINAPI ThreadWorker::ThreadEntry ( void *pArg)
{
	ThreadWorker* worker = (ThreadWorker*) pArg;
	worker->Loop();

	worker->working = false;

	return 0;
}
#endif

#if defined(ANDROID) || defined(IOS) || defined(OSX) || defined(OSUNIX)
void *ThreadWorker::ThreadEntry(void* arg)
{
	ThreadWorker* worker = (ThreadWorker*)arg;
	worker->Loop();

	worker->working = false;

	pthread_exit(NULL);
}
#endif

bool ThreadWorker::NeedExit()
{
	return need_exit;
}

void ThreadWorker::Stop()
{
	need_exit = true;

	while(working)
	{
		Sleep(20);
	}

#if defined(PC)
	if (thread != 0)
	{
		CloseHandle(thread);
		thread = 0;
	}
#endif

	need_exit = false;
}

bool ThreadWorker::IsWorking()
{
	return working;
}