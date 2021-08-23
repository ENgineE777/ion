#pragma once

#include "Core/Network/NetworkService.h"

class JSONFile;

class DEF_EXPORT VKNet : public TaskHoldersOwner
{
public:

	enum SignInState
	{
		ss_not_signed,		
		ss_signing,
		ss_signed,
		ss_signouting
	};

	class VKRequestListiner
	{
	public:
		
		virtual void OnRequestFinish(int res, void* data, const char* out_file) = 0;		
	};

	class VKSignoutListiner
	{
	public:
		
		virtual void OnFinish(void* data) = 0;		
	};

private:
	
	struct Request
	{
		int ID;
		int res;		
		VKRequestListiner* listiner;
		void* data;
		char res_file[512];
		int index;

#ifdef PC
		float timeout;
		bool  fired;
#endif

		bool is_xml;

		Request()
		{
			ID = -1;
			res = -1;
			listiner = null;
			data = null;
			res_file[0] = 0;
			index = -1;
			is_xml = false;
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

	class VKInnerRequestListiner : public NetworkService::RequestListiner
	{
	public:

		VKNet* owner;

		void OnRequestFinish(int res, void* data, const char* file_name);		
	};	

	void* sign_out_data;
	VKSignoutListiner* sign_out_listiner;
	SignInState signin_state;
	std::string accessToken;
	VKInnerRequestListiner listener;
	
	char user_id[64];

#ifdef PC
	std::string url_signin;
	std::string url_signin_res;
#endif

	Request* AddRequest(int set_id, VKRequestListiner* listiner, void* data);
	Request* FindRequest(int set_id);

	bool req_indices[64];
	string req_out_url;
	
	string api_url;	

public:		

	void Init();	

	void SignIn(VKRequestListiner* listiner, void* data);
	void SignOut(VKSignoutListiner* listiner, void* data);

	SignInState GetSignInStatus();
	void SetSignInStatus(bool in_singin, const char* token, const char* user_id);				

	const char* GetUserID();

	void StartRequest(const char* url, const char* params, const char* post_params, VKRequestListiner* listiner, void* data);	
	void UploadFile(const char* url, const char* file_name, VKRequestListiner* listiner, void* data);

	void Work(float dt);		
	
	void SetApiURL(const char* url);	

	void WaitReqest();
	void Reset();
};
