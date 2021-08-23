
#pragma once

#include "Common/Common.h"
#include "Common/TaskHolder.h"


class MediaPlayer : public TaskHoldersOwner
{	
public:
	//my
	MediaPlayer();
	virtual bool  Init();
	virtual void Work(float dt, int level);
	virtual void  Release();
	void OpenVideo(const char* path, const char* showName = null);

	enum RequestType
	{
		eYoutube,
		eVk,
        eSWF,
		eNone
	};
	

	class ReqListiner : public INetworkService::RequestListiner
	{			
	public:

		char req_ok[128];
		char responseFileName[128];		

		ReqListiner()
		{
			req_ok[0] = 0;	
			responseFileName[0] = 0;
		};

		virtual void OnRequestFinish(int res, void* data, const char* file_name);			
	};

	ReqListiner listiner;
	Str512 showName;
	Str512 path;
	RequestType requestType;

};



