
#include "CoreBase.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include "Network/NetLogger.h"


CoreBase::CoreBase():pNetLogger(NULL),trace_callback(NULL),lastTime(-1),deltaTime(0),fps(0),cur_fps(0),fps_timer(0),minStamp(0),secStamp(0),total_time(0)
    ,exited(false),in_editor_mode(false),use_fake_exit(false),is_fake_exit(false),trace_initialized(false)
{	
	srand (time(NULL));
	String::Init();
    locale[0] = 0;	
    sProjectPath[0] = 0;
}

void CoreBase::AddTask(TaskHolder* taskHolder, TaskHoldersOwner* taskHolderOwner, TaskHolder::Task task, int level, float freq)
{	
	int i=0;
	
	for (i=0;i<(int)tasks.size();i++)
	{
		if (tasks[i].level == level)
		{
			for (int k=0;k<(int)tasks[i].tasks.size();k++)
			{
				if (tasks[i].tasks[k].taskHolder == taskHolder && tasks[i].tasks[k].task == task)
				{
					return;
				}                               
			}

			Task tsk;

			tsk.taskHolderOwner = taskHolderOwner;
			tsk.taskHolder = taskHolder;
			tsk.task = task;        
			tsk.freq = freq;

			tasks[i].tasks.push_back(tsk);

			return;
		}

		if (tasks[i].level > level)
		{
			TaskGroup taskGroup;

			tasks.push_back(taskGroup);

			for (int j=tasks.size()-2;j>=i;j--)
			{
				tasks[j+1] = tasks[j];
			}

			tasks[i].level = level;
			tasks[i].tasks.clear();

			Task tsk;

			tsk.taskHolderOwner = taskHolderOwner;
			tsk.taskHolder = taskHolder;
			tsk.task = task;
			tsk.freq = freq;

			tasks[i].tasks.push_back(tsk);

			return;
		}
	}       

	TaskGroup taskGroup;

	tasks.push_back(taskGroup);
	tasks[i].level = level;

	Task tsk;

	tsk.taskHolderOwner = taskHolderOwner;
	tsk.taskHolder = taskHolder;
	tsk.task = task;        
	tsk.freq = freq;

	tasks[i].tasks.push_back(tsk);
}

void CoreBase::ChangeTaskHolderOwner(TaskHolder* taskHolder, TaskHoldersOwner* taskHolderOwner)
{
	for (int i=0;i<(int)tasks.size();i++)
	{
		for (int k=0;k<(int)tasks[i].tasks.size();k++)
		{
			if (tasks[i].tasks[k].taskHolder == taskHolder)
			{
				tasks[i].tasks[k].taskHolderOwner = taskHolderOwner;								
			}                               
		}
	}    
}

void CoreBase::DelTask(TaskHolder* taskHolder, TaskHolder::Task task)
{	
	for (int i=0;i<(int)tasks.size();i++)
	{
		int k=0;

		for (k=0;k<(int)tasks[i].tasks.size();k++)
		{
			if (tasks[i].tasks[k].taskHolder == taskHolder &&
				tasks[i].tasks[k].task == task)
			{
				break;
			}
		}

		if (k<(int)tasks[i].tasks.size())
		{
			if (k != (int)tasks[i].tasks.size()-1)
			{
				tasks[i].tasks[k] = tasks[i].tasks[(int)tasks[i].tasks.size()-1];
			}			

			tasks[i].tasks.pop_back();

			return;
		}
	}
}

void CoreBase::DelTasks(TaskHolder* taskHolder)
{
	for (int i=0;i<(int)tasks.size();i++)
	{		
		for (int k=0;k<(int)tasks[i].tasks.size();k++)
		{
			if (tasks[i].tasks[k].taskHolder == taskHolder)
			{						
				if (k != (int)tasks[i].tasks.size()-1)
				{
					tasks[i].tasks[k] = tasks[i].tasks[(int)tasks[i].tasks.size()-1];
				}			

				tasks[i].tasks.pop_back();
			}			
		}
	}
}

void CoreBase::ExecuteTask(Task* tsk,float timedelta, int level)
{   
	if (tsk->freq>0)
	{
		tsk->timer += timedelta;

		if (tsk->timer > tsk->freq)
		{
			timedelta = tsk->timer;
			tsk->timer = 0.0f;
		}
		else
		{
			return;
		}
	}
	
	(tsk->taskHolder->*tsk->task)(timedelta,level);
}

void CoreBase::ExecuteTasks()
{	
	for (int i=0;i<(int)tasks.size();i++)
	{			
		int lvl = tasks[i].level;

		for (int j=0;j<(int)tasks[i].tasks.size();j++)
		{           
			if (tasks[i].tasks[j].taskHolderOwner->IsTaskHoldersOwnerActive()) ExecuteTask(&tasks[i].tasks[j], deltaTime, lvl);
		}		
	}
}

void CoreBase::CountDeltaTime(bool need_clamp)
{
    total_time += deltaTime;
	secStamp += deltaTime;
	
	if (secStamp>60.0f)
	{
		int num = (int)(secStamp / 60.0f);
		minStamp += num;
		secStamp -= num * 60.0f;
	}

	String::Printf(timeStr, 31, "%i:%2.3f - ", minStamp, secStamp);

	if (deltaTime<0.0f) deltaTime = 0.0f;

	if (need_clamp)
	{
		if (deltaTime>0.3f) deltaTime = 0.3f;
	}
	
	cur_fps++;
	fps_timer += deltaTime;

	if (fps_timer>1.0f)
	{
		fps = cur_fps;
		cur_fps = 0;
		fps_timer = 0.0f;
	}	
}

void CoreBase::SetDeltaTime(float dt)
{
	deltaTime = dt;
    total_time += deltaTime;
	if (deltaTime>0.2f) deltaTime = 0.2f;
}

float CoreBase::GetDeltaTime()
{	
	return deltaTime;
}

int CoreBase::GetFPS()
{
	return fps;
}

void CoreBase::TraceTo(const char* logName, const char* format, ...)
{
	char buf[8192];

	buf[0] = 0;

	va_list	args;

	strcpy(buf, timeStr);

	int len = strlen(timeStr);
	
	va_start( args, format );
		
	vsnprintf( &buf[len], 8192 - len, format, args );	
	
	va_end( args );
    if(pNetLogger)
        pNetLogger->Log(logName,buf);
#if defined(PC) || defined(OSUNIX)
	char log[512];
	GetCurDirectory(log, 512);
	String::Cat(log,512,"/Logs/");
	if(!trace_initialized)
	{
		DeleteFolder(log);
		CreateDirectories(log);
		trace_initialized = true;
	}
	String::Cat(log,512,logName);
	if (trace_callback) trace_callback(log, buf);
	TraceToImpl(log, buf);
#else
	if (trace_callback) trace_callback(logName, buf);
	TraceToImpl(logName, buf);
#endif

}

void CoreBase::SetTraceCallback(TraceCallback set_trace_callback)
{
	trace_callback = set_trace_callback;
}

void* CoreBase::AllocMem(int size, int elems, const char* file, int line)
{
	//core->Trace("alloc %i from %s, %i", size * elems, file, line);

	byte* ptr = (byte*)malloc(size * elems + sizeof(MemoryBlock));
	
	MemoryBlock* memPtr = (MemoryBlock*)ptr;

	memPtr->marker = MAKEFOURCC('M', 'G', 'B', 'K');

	memPtr->size = size;
	memPtr->elems = elems;
	memPtr->file = file;
	memPtr->line = line;
	memPtr->prevPtr = lastBlock;
	if (lastBlock) lastBlock->nextPtr = memPtr;
	memPtr->nextPtr = null;

	if (firstBlock == null) firstBlock = memPtr;
	lastBlock = memPtr;

	totalUsage += size * elems + sizeof(MemoryBlock);

	return &ptr[sizeof(MemoryBlock)];
}

int CoreBase::GetMemBlockElen(void* ptr)
{
	byte* pt = (byte*)ptr;
	MemoryBlock* memPtr = (MemoryBlock*)(pt - sizeof(MemoryBlock));

	if (memPtr->marker != MAKEFOURCC('M', 'G', 'B', 'K'))
	{	
		return -1;
	}

	return memPtr->elems;
}

void CoreBase::FreeMem(void* ptr)
{
	byte* pt = (byte*)ptr;
	MemoryBlock* memPtr = (MemoryBlock*)(pt - sizeof(MemoryBlock));

	if (memPtr->marker != MAKEFOURCC('M', 'G', 'B', 'K'))
	{
		delete pt;
		return;
	}

	totalUsage -= (memPtr->size * memPtr->elems + sizeof(MemoryBlock));
	
	if (memPtr->prevPtr)
	{
		memPtr->prevPtr->nextPtr = memPtr->nextPtr;
		if (memPtr->nextPtr) memPtr->nextPtr->prevPtr = memPtr->prevPtr;
	}
	else
	{
		firstBlock = memPtr->nextPtr;
		if (memPtr->nextPtr) memPtr->nextPtr->prevPtr = NULL;
	}

	if (!memPtr->nextPtr)
	{
		lastBlock = memPtr->prevPtr;
		if (memPtr->prevPtr) memPtr->prevPtr->nextPtr = NULL;
	}

	free(memPtr);
}

void CoreBase::DumpMemoryLeaks()
{	
	 //used_mem = (mem_info.total_user_memory - (mallocStats.current_system_size - mallocStats.current_inuse_size + mem_info.available_user_memory))/1024;	 	 
	 //total_mem = mem_info.total_user_memory/1024;

	/*MemoryBlock* block = firstBlock;

	char str[256];
	String::Printf(str, 255, "%i bytes are unrealiased", totalUsage);	
	TraceToImpl("MemoryLeaks", str);

	while (block)
	{
		String::Printf(str, 255, "Leaked %i bytes in %s, %i", block->size, block->file, block->line);
		TraceToImpl("MemoryLeaks", str);

		block = block->nextPtr;
	}*/
}

void CoreBase::Exit()
{
	if (use_fake_exit)
	{
		is_fake_exit = true;
		return;
	}

	exited = true;
}

bool CoreBase::IsExited()
{
	if (use_fake_exit)
	{
		return false;
	}

	return exited;
}

bool CoreBase::IsEditorMode()
{
	return in_editor_mode;
}

void CoreBase::SetEditorMode(bool act)
{
	in_editor_mode = act;
}

void CoreBase::SetUseFakeExit(bool act)
{
	use_fake_exit = act;
	is_fake_exit = false;
}

bool CoreBase::IsFakeExit()
{
	return is_fake_exit;
}

const char* CoreBase::GetLocale()
{
	return locale;
}

void CoreBase::SetProjectPath(const char* sPath)
{
    String::Copy(sProjectPath,512,sPath);
}

bool CoreBase::Init()
{
    rLocalizeService.Init();
    return true;
}

void CoreBase::Release()
{
    if(pNetLogger)
    {
        pNetLogger->Disconnect();
        delete pNetLogger;
    }
    pNetLogger = NULL;
}

void CoreBase::CreateNetLogger()
{
#if defined(PC) || defined(OSUNIX)
        pNetLogger = new NetLogger(NetLogger::eClient);
#else
        pNetLogger = new NetLogger(NetLogger::eServer);
#endif
}
