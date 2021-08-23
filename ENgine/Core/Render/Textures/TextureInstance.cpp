
#include "TextureInstance.h"
#include "TextureRes.h"
#include "Texture.h"
#include "Core/Core.h"

TextureInstance::TextureInstance(TextureRes* rs) : Texture(0, 0, 0, 0, tex2d)
{
	res = rs;
	cur_frame = 0;
	cur_time = 0; // need rnd
	anim_speed = 1.0f;	
	looped = true;
	playing = true;
    
    need_load = false;	
}

void TextureInstance::RegisterWork()
{
	executor.worker = this;
	
	core->AddTask(&executor,&executor,(TaskHolder::Task)&Executor::Work, -1500, -1.0f);
}

void TextureInstance::SetSpeed(float speed)
{
	anim_speed = speed;
}

void TextureInstance::SetLooped(bool set_looped)
{
	looped = set_looped;
}

void TextureInstance::Work(float dt, int level)
{
	if (!playing) return;

	cur_time += dt * anim_speed;		

	if (cur_time>res->total_time)
	{
		if (looped)
		{
			int nums = (int)(cur_time / res->total_time);
			cur_time -= res->total_time * nums;
			cur_frame = 0;
		}
		else
		{
			cur_time = 0.99f * res->total_time;
		}
	}

	while (cur_time>res->frames[cur_frame].time)
	{
		cur_frame++;
	}
}

void TextureInstance::Pause(bool pause)
{
	//core->TraceTo("log", " TextureInstance::Pause");
	playing = !pause;
}

void TextureInstance::Restart()
{
	cur_time = 0.0f;
	cur_frame = 0;
}

bool TextureInstance::IsAnimated()
{
	return false;
}	

bool TextureInstance::IsVideo()
{
	return false;
}	

float TextureInstance::GetDuration()
{
	/*if (IsAnimated() == false)
	{
		return 0; //TODO: Check it
	}
	else
	{
		//return res->videoTexture->GetDuration();
	}*/

	return res->total_time;
}

float TextureInstance::GetCurTime()
{
	/*if (IsAnimated() == false)
	{
		return 0; //TODO: Check it
	}
	else
	{
		//return res->videoTexture->GetCurrentTime();
	}*/

	return cur_time;
}

int TextureInstance::GetWidth()
{
	if (IsAnimated())
	{
		//return res->videoTexture->GetWidth();
	}
	
	return res->frames[cur_frame].texture->GetWidth();	
}

int TextureInstance::GetHeight()
{
	if (IsAnimated())
	{
		//return res->videoTexture->GetHeight();
	}
	
	return res->frames[cur_frame].texture->GetHeight();	
}

void* TextureInstance::GetData()
{    	
    if (res->frames[cur_frame].texture)
    {
		return res->frames[cur_frame].texture->GetData();
    }
    
    return null;    
}

int TextureInstance::GetLods()
{
	if(IsAnimated())
	{
		return 1;
	}
	
	return res->frames[cur_frame].texture->GetLods();	
}

void* TextureInstance::GetHackData()
{
	if (res->frames[cur_frame].texture)
	{
		return res->frames[cur_frame].texture->GetHackData();
	}

	return null;
}

void TextureInstance::ApplyStates(int slot)
{
	if (res->frames[cur_frame].texture)
	{
		return res->frames[cur_frame].texture->ApplyStates(slot);
	}
}

void TextureInstance::SetFilters(FilterType magmin, FilterType mipmap)
{
	magminf = magmin;
	mipmapf = mipmap;

	if (IsAnimated())
	{
		//res->videoTexture->SetFilters(magmin, mipmap);
	}
	
	res->frames[cur_frame].texture->SetFilters(magmin, mipmap);	
}

void TextureInstance::SetAdress(TextureAddress adress)
{
	adressUVW = adress;


	if(IsAnimated())
	{
		//res->videoTexture->SetAdress(adress);
	}
	
    if (res->frames[cur_frame].texture)
    {
		res->frames[cur_frame].texture->SetAdress(adress);
    }	
}

bool TextureInstance::IsReady()
{
    if (res->frames[cur_frame].texture)
    {
		return res->frames[cur_frame].texture->IsReady();
    }
    
    return false;
}

void TextureInstance::Release()
{   
	core->DelTasks(&executor);
	res->DecRef();

	delete this;
	//DEL(this, TextureInstance)	
}
