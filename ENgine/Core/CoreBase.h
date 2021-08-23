#pragma once

#include "ICore.h"
#include "Localize/LocalizeService.h"

class DEF_EXPORT CoreBase : public ICore
{	
public:
	typedef void (*TraceCallback)(const char* log, const char* text);

protected:
	struct Task
	{
		TaskHolder* taskHolder; 
		TaskHoldersOwner* taskHolderOwner;        
		TaskHolder::Task task;
		float freq;
		float timer;

		Task()
		{
			taskHolder = NULL;
			freq = -1.0f;
			timer = 0.0f;
		};
	};

	struct TaskGroup
	{
		int level;
		std::vector<Task> tasks;
	};

	std::vector<TaskGroup> tasks;

	float  lastTime;
	float  deltaTime;
	int    fps;
	int    cur_fps;
	float  fps_timer;
    double total_time;
 
	float total_mem;
	float used_mem;

	struct MemoryBlock
	{
		dword marker;		 
		MemoryBlock* prevPtr;
		MemoryBlock* nextPtr;
		int size;
		int elems;
		const char* file;
		int line;
	};

	MemoryBlock* firstBlock;
	MemoryBlock* lastBlock;
	int     totalUsage;	
	int     numFrameAlloc;

	int minStamp;
	float secStamp;
	char timeStr[32];

	bool exited;

	bool in_editor_mode;

	bool use_fake_exit;
	bool is_fake_exit;

	TraceCallback trace_callback;
	bool		  trace_initialized;

	char locale[16];	
    char sProjectPath[512];

    class NetLogger* pNetLogger;
    LocalizeService  rLocalizeService;

public:	

    

	CoreBase();
	virtual bool Init();

    virtual void SetProjectPath(const char* sPath);
    virtual const char* GetProjectPath() const {return sProjectPath;}

	virtual void AddTask(TaskHolder* taskHolder, TaskHoldersOwner* taskHolderOwner, TaskHolder::Task task, int level, float freq);
	virtual void ChangeTaskHolderOwner(TaskHolder* taskHolder, TaskHoldersOwner* taskHolderOwner);
	virtual void DelTask(TaskHolder* taskHolder, TaskHolder::Task task);

	virtual void DelTasks(TaskHolder* taskHolder);

	void ExecuteTask(Task* tsk,float timedelta, int level);
	virtual void ExecuteTasks();
	
	virtual void CountDeltaTime(bool need_clamp);
	void  SetDeltaTime(float dt);
	virtual float  GetDeltaTime();
	virtual int	   GetFPS();	
    virtual double GetElapsedTime(){return total_time;}

	virtual Platform GetCurrentPlatform() = 0;
	
	virtual void TraceTo(const char* logName, const char* format, ...);
	
	void SetTraceCallback(TraceCallback trace_callback);

	virtual void TraceToImpl(const char* log, const char* str) = 0;

	void* AllocMem(int size, int elems, const char* file, int line);
	int   GetMemBlockElen(void* ptr);
	void  FreeMem(void* ptr);
	void  DumpMemoryLeaks();	

	void  Exit();
	bool  IsExited();

	bool IsEditorMode();
	void SetEditorMode(bool act);
	
	void SetUseFakeExit(bool act);
	bool IsFakeExit();	
	
	virtual const char* GetLocale();

	virtual void Work() = 0;
	
	virtual void Release();

    virtual void CreateNetLogger();

    class NetLogger* GetNetLogger() const {return pNetLogger;}
    virtual ILocalizeService* Localize() {return static_cast<ILocalizeService*>(&rLocalizeService);}

};
