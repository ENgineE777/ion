#include "Core/Core.h"
#include "MediaPlayer.h"


#ifdef ANDROID
extern void CallFromJava(const char* function);
extern void CallFromJava(const char* function, const char* param);
#endif

#ifdef IOS

#include "Platforms/IOS/IOSWrapper.h"

#endif

MediaPlayer::MediaPlayer()
{
	requestType = eNone;
}

bool  MediaPlayer::Init()
{
	core->AddTask(this, this, (TaskHolder::Task)&MediaPlayer::Work, -320, -1.f);

	return true;
}


void MediaPlayer::Work(float dt, int level)
{
#ifdef PC
#endif

}



void MediaPlayer::OpenVideo(const char* path, const char* showName)
{
	if (showName == null)
		String::Copy(this->showName.data, 512, "");
	else if (showName != null && showName[0] == 0)
		String::Copy(this->showName.data, 512, " ");
	else
		String::Copy(this->showName.data, 512, showName);

	String::Copy(this->path.data, 512, path);

	core->TraceTo("log", "MediaPlayer::OpenVideo path=%s,  showName=%d",this->path.data, this->showName.data);
#ifdef PC
	ShellExecute(null, "open", path, NULL, NULL, SW_SHOWNORMAL);	
#endif



#if defined(IOS) || defined(ANDROID)
//#ifdef PC
	//http://vk.com/video-29544671_166730922
	if (String::IsSubstr(path, "http://vk.com/video"))
	{
		requestType = eVk;
		core->Network()->StartRequest(&listiner, this, "GET",
			path, null, null, null, "vk_response.html");

		return;
	}
	

	//http://www.youtube.com/watch?v=OSa0X96w8iM
	Str512 code;
	sscanf(path, "http://www.youtube.com/watch?v=%s", &code.data);
	//http://www.youtube.com/embed/OSa0X96w8iM
	if (code.data[0] == 0)
		sscanf(path, "http://www.youtube.com/embed/%s", &code.data);

	if (code.data[0] != 0)
	{
		requestType = eYoutube;
		string link = string("http://gdata.youtube.com/feeds/api/videos/") + code.data + "?alt=json";
		core->Network()->StartRequest(&listiner, this, "GET",
			link.c_str(), null, null, null, "youtube_response.json");

		return;
	}
    

#ifdef ANDROID	
    if ( String::IsSubstr(path, "http://video.rutube.ru") 
		 || String::IsSubstr(path, "http://coub.com"))
	{
		bool inner = true;
		if (inner)
		{
			CallFromJava("OpenURLInner", path);
		}
		else
		{
			CallFromJava("OpenURL", path);
		}
        
		return;
	}
    
#endif

#ifdef ANDROID
	string str = path;
	if (str[str.size()-1] != ';')
		str += ";";
	str += this->showName.data;

	CallFromJava("OpenVideo", str.c_str());
#endif
#ifdef IOS    
	ios_wrapper->OpenVideo(path, this->showName.data);    
#endif
#endif

}



void  MediaPlayer::Release()
{

}

void MediaPlayer::ReqListiner::OnRequestFinish(int res, void* data, const char* file_name)
{
	if (res != 0)	
	{
		core->TraceTo("log", "MediaPlayer::ReqListiner::OnRequestFinish ERROR res=%d",res);
		return;
	}

	string url;
	MediaPlayer* player = (MediaPlayer*)data;
    

    
	if (player->requestType == eYoutube)
	{
		IStreamLoader* loader = core->Files()->CreateStreamLoader(file_name, "", IStreamLoader::json);	

		if (loader)
		{
			if (loader->EnterBlock("entry"))
				if (loader->EnterBlock("media$group"))
					while (loader->EnterBlock("media$content"))
					{
						char data[512];
						loader->Read("url", data, 512);
						url = data;
						//core->TraceTo("log", "MediaPlayer::ReqListiner::OnRequestFinish url.data=%s",url.data );
						loader->LeaveBlock();
					}


					loader->Release();
		}
	}
	else if (player->requestType == eVk)
	{
		IFileBuffer* fb = core->Files()->LoadFile(file_name);
		byte *buf = new byte[fb->GetSize()+1];
		buf[fb->GetSize()] = 0;
		memcpy(buf, fb->GetBuffer(), fb->GetSize());
		string htmlstr = (const char*)buf;
		delete[] buf;

		int pos = string::npos;
		if ( htmlstr.find("url480") != string::npos 
			|| htmlstr.find("url240") != string::npos
			|| htmlstr.find("url360") != string::npos
			|| htmlstr.find("url720") != string::npos)
		{
			for (int i = 0; i < 4; ++i)
			{
				string qual_prefix = "quallity";
				Str128 str_int;
				String::Printf(str_int.data, 128, "%d", i);
				qual_prefix += str_int.data;
				
					 
				if (i == 0)
				{
					pos =  htmlstr.find("url240");
					qual_prefix += "_240";
				}
				if (i == 1)
				{
					pos =  htmlstr.find("url360");
					qual_prefix += "_360";
				}
				if (i == 2)
				{
					pos =  htmlstr.find("url480");
					qual_prefix += "_480";
				}
				if (i == 3)
				{
					pos =  htmlstr.find("url720");
					qual_prefix += "_720";
				}

				qual_prefix += "=";

				if (pos == string::npos)
					continue;

				int pos1 = htmlstr.find("http", pos);
				int pos2 = htmlstr.find("&", pos);
				if (pos1 != string::npos && pos2 != string::npos)
				{
					string strurl =  htmlstr.substr(pos1, pos2 - pos1);
					int flashPos = strurl.find("\\", 0);
					while(flashPos != string::npos)
					{
						strurl.replace(flashPos, 1, "");
						flashPos = strurl.find("\\", 0);
					}

					url += (qual_prefix + strurl + ";").c_str();
					//TEMP for ios work
					//url = strurl;
					//break;
				}
			} 
		}
		else if ( htmlstr.find("http:\\/\\/www.youtube.com\\/embed") != string::npos)
		{
			pos = htmlstr.find("http:\\/\\/www.youtube.com\\/embed"); 
			int pos1 =  pos;
			int pos2 = htmlstr.find("?", pos);
			if (pos1 != string::npos && pos2 != string::npos)
			{
				string strurl =  htmlstr.substr(pos1, pos2 - pos1);
				int flashPos = strurl.find("\\", 0);
				while(flashPos != string::npos)
				{
					strurl.replace(flashPos, 1, "");
					flashPos = strurl.find("\\", 0);
				}


				url = strurl.c_str();
			}
		}
	}
	else
		return;
    


	player->OpenVideo(url.c_str(), player->showName.data);
}





