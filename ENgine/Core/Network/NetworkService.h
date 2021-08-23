#pragma once

#include "INetworkService.h"

#ifndef IOS
typedef void CURL;
#endif

class NetworkService : public INetworkService
{
public:

    enum ReqestState
    {
        rq_waiting_start,
        rq_waiting_similar,
        rq_working,
        rq_finished
    };
    
#ifndef IOS
	struct CURLRequest;

	class ReqestWorker : public ThreadWorker
	{
	public:
		CURLRequest* owner;

		virtual void Prepare();	
		virtual void Loop();		
	};
#endif

	struct CURLRequest
	{
#ifndef IOS
		CURL*  curl;
#endif
        
#ifdef IOS
        int net_index;
#endif
        
		bool   need_del_curl;
		RequestListiner* listiner;
		void*  reqest_data;
		string uri;
		string upload_file;
		char   file_name[512];
		char   full_file_name[512];
		char*  post_params;
		FILE*  stream;
		string headers;

#ifdef EDITOR
		char  full_file_name_cache[512];
		FILE* stream_cache;
#endif

		int   res;
		char response_header[512];
        
#ifndef IOS
		ReqestWorker worker;
#endif
        
		ReqestState state;

		bool write_xml_only;
		bool start_write_xml_only;
		bool is_canceled;

		char method[16];

		CURLRequest()
		{
#ifndef IOS
			curl = null;
#endif
            
#ifdef IOS
            net_index = 0;
#endif
			listiner = null;
			reqest_data = null;
			file_name[0] = 0;			
			post_params = null;
			res = -1;
			response_header[0] = 0;
			stream = null;
			state = rq_waiting_start;

			write_xml_only = false;
			start_write_xml_only = false;

			method[0] = 0;

			need_del_curl = false;
#ifdef EDITOR
			full_file_name_cache[0] = 0;
			stream_cache = 0;
#endif
			is_canceled = 0;
		};
	};

#ifndef IOS
	struct CurlHandle
	{
		CURL* curl;
        
		bool  used;

		CurlHandle()
		{
            curl = null;
			used = false;
		};
	};
    
	static int curl_get_data(void* buffer, size_t size, size_t nmemb, void* stream);	
	static size_t read_data (char *bufptr, size_t size, size_t nitems, void *userp);
    
    CurlHandle chandles[7];
#endif

    bool  is_internet_connected;
    
    int active_requests;
    
    int num_chandles;
    
    std::vector<CURLRequest*> requests;

#ifdef EDITOR
	bool need_cache_net;
	bool use_cache_net;
#endif

public: 
	
	NetworkService();
	virtual bool Init();		
	virtual bool IsInternetConnected();
	virtual void Work(float dt, int level);
	virtual void* StartRequest(RequestListiner* listiner, void* reqest_data, const char* method,
							  const char* request, const char* post_params, const char* response_file);

	virtual void* StartRequest(RequestListiner* listiner, void* reqest_data, const char* method,
							  const char* request, const char* post_params, const char* headers, 
							  const char* upload_file, const char* response_file);
	
	virtual void CancelRequaest(void* req);

	void UploadFileUsingForm(RequestListiner* listiner, void* reqest_data, const char* request, const char* file_name, const char* param_name, const char* param, const char* param_name2, const char* response_file);

	virtual void Reset();
	virtual void Release();

    
#ifndef IOS
	void PrepareCURL(CURLRequest* curl_request);
#endif

#ifdef EDITOR
	virtual void SetWriteCache(bool set);
	virtual void SetUseCache(bool set);
#endif		

#if defined(ANDROID) || defined(PC)
	virtual void SetInternetConnected(bool set);
#endif

#if defined(ANDROID)
	virtual void InetChecker(float dt, int level);
#endif	
};
