#pragma once

#include "Common/Common.h"

class INetworkService : public TaskHoldersOwner
{
public: 

	class RequestListiner
	{
	public:
		virtual void OnRequestFinish(int res, void* data, const char* file_name) = 0;		
	};
	
	virtual bool Init() = 0;		
	virtual bool IsInternetConnected() = 0;
	virtual void* StartRequest(RequestListiner* listiner, void* reqest_data, const char* method,
							  const char* request, const char* post_params, const char* response_file) = 0;
	virtual void* StartRequest(RequestListiner* listiner, void* reqest_data, const char* method,
							  const char* request, const char* post_params, const char* headers, 
							  const char* upload_file, const char* response_file) = 0;

	virtual void CancelRequaest(void* req) = 0;

	virtual void UploadFileUsingForm(RequestListiner* listiner, void* reqest_data, const char* request, const char* file_name, const char* param_name, const char* param, const char* param_name2, const char* response_file) = 0;	

	virtual void Reset() = 0;
	virtual void Release() = 0;

#if defined(ANDROID) || defined(PC)
	virtual void SetInternetConnected(bool set) = 0;
#endif

#ifdef EDITOR
	virtual void SetWriteCache(bool set) = 0;
	virtual void SetUseCache(bool set) = 0;
#endif		
};
