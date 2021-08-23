
#pragma once

#include "Texture.h"
#include "Common/TaskHolder.h"
 
class TextureRes;

class DEF_EXPORT TextureInstance : public Texture
{	
protected:

	class Executor : public TaskHoldersOwner
	{		
	public:
		TextureInstance* worker;
		void Work(float dt, int level)
		{
			worker->Work(dt, level);
		}
	};

	TextureRes* res;

	int      cur_frame;
	float    cur_time;
	float    anim_speed;
	bool     looped;
	bool     playing;
	bool     need_load;	    

	Executor executor;

public:	
	
	TextureInstance(TextureRes* rs);
	
	void RegisterWork();
	void SetSpeed(float speed);
	virtual void SetLooped(bool looped);
	void Work(float dt, int level);
	
	bool IsAnimated();
	virtual bool IsVideo();
	
	virtual void Pause(bool pause);	
	virtual void Restart();

	virtual float GetDuration();
	virtual float GetCurTime();

	virtual int GetWidth();
	virtual int GetHeight();
	virtual void* GetData();
	virtual int GetLods();

	virtual void* GetHackData();

	virtual void ApplyStates(int slot);

	virtual void SetFilters(FilterType magmin, FilterType mipmap);
	virtual void SetAdress(TextureAddress adress);
    
    virtual bool IsReady();

	virtual void Release();
};
