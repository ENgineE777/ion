#pragma once

#include "Core/Network/NetworkService.h"

class JSONFile;

class DEF_EXPORT OkNet : public TaskHoldersOwner
{
public:

	class OkRequestListiner
	{
	public:
		
		virtual void OnRequestFinish(int res, void* data, const char* out_file) = 0;		
	};

private:
	
	struct Request
	{
		int ID;
		int res;		
		OkRequestListiner* listiner;
		void* data;
		char res_file[512];

#ifdef PC
		float timeout;
		bool  fired;
#endif

		Request()
		{
			ID = -1;
			res = -1;
			listiner = null;
			data = null;
			res_file[0] = 0;
#ifdef PC
			timeout = 0.0f;
			fired = false;
#endif
		};
	};

	enum RequestType
	{
		req_sigin = 0,				
		req_graphapi
	};

	vector<Request*> requests;	

	class OkInnerRequestListiner : public NetworkService::RequestListiner
	{
	public:

		OkNet* owner;

		void OnRequestFinish(int res, void* data, const char* file_name);		
	};	

	bool signin;
	std::string accessToken;
	OkInnerRequestListiner listener;

	char app_id[64];
	char app_key[64];
	char secret_key[64];

#ifdef PC
	std::string url_signin;
	std::string url_signin_res;
#endif

	Request* AddRequest(int set_id, OkRequestListiner* listiner, void* data);
	Request* FindRequest(int set_id);

public:		

	void Init();

	void SetParams(const char* app_id, const char* app_key, const char* secret_key);

	void SignIn(OkRequestListiner* listiner, void* data);
	void SignOut();

	bool GetSignInStatus();
	void SetSignInStatus(bool in_singin, const char* token);				

	void StartRequest(const char* url, const char* params, const char* post_params, OkRequestListiner* listiner, void* data);	

	void Work(float dt);		

	void WaitReqest();
	void Reset();
};
