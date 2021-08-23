
#include "VideoTextureInstance.h"
#include "TextureRes.h"
#include "Texture.h"
#include "Core/Core.h"

#ifdef OPGL
#include "Common/libgif/gif_lib.h"
#endif

#ifdef ANDROID
extern void CallFromJava(const char* function, const char* param);
#endif

#if defined(IOS) || defined(ANDROID) || defined(OSUNIX)
//#include "VideoTextureGLES.h"
#endif

int VideoTextureInstance::instCounter = 0;


VideoTextureInstance::VideoTextureInstance(TextureRes* rs) : TextureInstance(rs)
{
	video_texture =  null;
#ifdef OPGL
	//video_texture = new VideoTextureGLES( rs->GetPath() );
	Str256 str;
   // String::Printf(str.data, 256, "%d;%s", video_texture->GetInternalTexture(),res->GetPath()	);

#ifdef ANDROID
	//core->TraceTo("log", "VideoTextureInstance: str=%s", str.data);
	CallFromJava("AddMediaPlayer", str.data);
#endif
#endif

	++instCounter;

	//core->AddTask(this,this,(TaskHolder::Task)&VideoTextureInstance::Work, -1500, -1.0f);
}


bool VideoTextureInstance::IsVideo()
{
	return true;
}	

void VideoTextureInstance::SetLooped(bool set_looped)
{
	looped = set_looped;
}

void VideoTextureInstance::Work(float dt, int level)
{
	if (!playing) return;

	
}

void VideoTextureInstance::Pause(bool pause)
{
	core->TraceTo("log", "VideoTextureInstance::Pause %d", pause);
	playing = !pause;
#ifdef Andorid
	Str256 str;
	String::Printf( str.data, 256, "%d", video_texture->GetInternalTexture());
	if (playing)
		CallFromJava("StartMediaPlayer", str.data);
	else
		CallFromJava("PauseMediaPlayer", str.data);
#endif

}

/*
#ifdef OPGL
GLuint VideoTextureInstance::GetInternalTexture() 
{ 
	return video_texture?video_texture->GetInternalTexture():TextureBasePlatform::texture;
}

const char* getConstantName(GLenum target);


static bool called = false;
GLenum VideoTextureInstance::GetInternalTarget() 
{
#ifdef ANDROID
	return GL_TEXTURE_EXTERNAL_OES;
#endif
	GLenum target =  video_texture?video_texture->GetInternalTarget():TextureBasePlatform::target;
	
#ifdef ANDROID
	if (!called)
		core->TraceTo("log", "VideoTextureInstance::GetInternalTarget target = %s,video_texture=%d", getConstantName(target), &video_texture);
#endif
    
	called = true;
	return target; 
}
#endif
*/

void VideoTextureInstance::SetAdress(TextureAddress adress)
{
	adressUVW = adress;
	
    if (video_texture)
    {
		video_texture->SetAdress(adress);
    }	
}

void VideoTextureInstance::Restart()
{
}	

float VideoTextureInstance::GetDuration()
{
	return res->total_time;
}

float VideoTextureInstance::GetCurTime()
{
	return cur_time;
}

int VideoTextureInstance::GetWidth()
{
	return 100;	
}

int VideoTextureInstance::GetHeight()
{
	return 100;	
}

void* VideoTextureInstance::GetData()
{	    
	if (video_texture)
	{
		return video_texture->GetData();
	}

	return null;    
}




void VideoTextureInstance::Release()
{
	//core->DelTasks(this);
	res->DecRef();
#ifdef ANDROID
	Str256 str;
	//String::Printf(str.data, 256, "%d", video_texture->GetInternalTexture());
	CallFromJava("RemoveMediaPlayer", str.data);
#endif

	RELEASE(video_texture)

	--instCounter;
	delete this;	
}
