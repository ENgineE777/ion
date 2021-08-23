#pragma once

#include "Core/Network/NetworkService.h"

class JSONFile;

class DEF_EXPORT FacebookNet : public TaskHoldersOwner
{
public:

	enum SignInState
	{
		ss_not_signed,		
		ss_signing,
		ss_signed,
		ss_signouting
	};

	string guest_access_token_url;
	string api_url;

	class FacebookRequestListiner
	{
	public:
		
		virtual void OnRequestFinish(int res, void* data, const char* out_file) = 0;		
	};

	class FacebookSignoutListiner
	{
	public:
		
		virtual void OnFinish(void* data) = 0;		
	};

private:
	
	struct Request
	{
		int ID;
		int res;		
		FacebookRequestListiner* listiner;
		void* data;
		char res_file[512];

		int index;

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
			index = -1;
#ifdef PC
			timeout = 0.0f;
			fired = false;
#endif
		};
	};

	enum RequestType
	{
		req_sigin = 0,	
		req_getpermission,
		req_post_message,
		req_graphapi
	};

	void* sign_out_data;
	FacebookSignoutListiner* sign_out_listiner;
	vector<Request*> requests;	

	class FacebookInnerRequestListiner : public NetworkService::RequestListiner
	{
	public:

		FacebookNet* owner;

		void OnRequestFinish(int res, void* data, const char* file_name);		
	};	
	
	SignInState signin_state;
	std::string accessToken;
	FacebookInnerRequestListiner listener;

#ifdef PC
	std::string url_signin;
	std::string url_signin_res;
#endif

	Request* AddRequest(int set_id, FacebookRequestListiner* listiner, void* data);
	Request* FindRequest(int set_id);

	bool req_indices[64];
	string req_out_url;
	char sub_params[16386];
	int  always_fail_sigin;
	vector<string> req_permissions;

public:		

	void Init();

	void SignIn(FacebookRequestListiner* listiner, void* data);

	void AddRequestPermistion(const char* name);
	void StartRequestPermistions(FacebookRequestListiner* listiner, void* data);

	void SetRequestPermistionsRes(bool res);		

	void SignOut(FacebookSignoutListiner* listiner, void* data);

	SignInState GetSignInStatus();
	void SetSignInStatus(bool in_singin, const char* token);		

	void StartRequest(const char* method, const char* url, const char* params, const char* post_params, FacebookRequestListiner* listiner, void* data);	
	void UploadFile(const char* url, const char* file_name, const char* param_name, const char* message, FacebookRequestListiner* listiner, void* data);

	void Work(float dt);

	void SetGuestAccessTokenURL(const char* url);
	void SetApiURL(const char* url);

	void WaitReqest();
	void Reset();
};
