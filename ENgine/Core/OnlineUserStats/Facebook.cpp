#include "Core/Core.h"
#include "Common/ThreadWorker.h"
#include "Common/Utils.h"

#ifdef IOS

#include "Platforms/IOS/IOSWrapper.h"

#endif

#if defined(ANDROID)
extern void CallFromJava(const char* function);
extern void CallFromJava(const char* function,  const char* param);
#endif

FacebookNet* hack_fb_net_ptr = null;

void FacebookNet::FacebookInnerRequestListiner::OnRequestFinish(int res, void* data, const char* file_name)
{	
	FacebookNet::Request* req = (FacebookNet::Request*)data;
    
	if (res == 0)
	{		
		IDataFile* file = core->Files()->OpenJSONFile(file_name);	
	
		if (file)
		{
			String::Copy(req->res_file, 512, file_name);

			if (file->EnterBlock("error"))
			{
				req->res = 1;	
				char error[512];
				file->ReadString("message", error, 512);
				file->ReadInt("code", req->res);			

				core->TraceTo("Facebook", error);
			}
			else
			{
				req->res = 0;										

				if (req->ID == req_sigin)
				{
					char token[512];
					file->ReadString("token", token, 512);

					core->TraceTo("Facebook", "token : %s", token);

					if (token[0])
					{
						hack_fb_net_ptr->accessToken = std::string("access_token=") + std::string(token);																		

						if (hack_fb_net_ptr->signin_state == ss_signing)
						{
							if (hack_fb_net_ptr->always_fail_sigin == 1)
							{
								req->res = 1;								
							}
							else
							{							
								hack_fb_net_ptr->signin_state = ss_signed;
							}
						}

						/*owner->accessToken = std::string("access_token=") + std::string(token);
												
						if (owner->signin_state == ss_signing_anonymous)
						{
							owner->signin_state = ss_signed_anonymous;
						}
						if (owner->signin_state == ss_signing)
						{
							owner->signin_state = ss_signed;
						}*/
					}
					else
					{
						req->res = -1;

#ifdef PC
						req->timeout = 1.5f;
						req->fired = false;
#endif
					}					
				}
			}
	
			file->Release();
		}
		else
		{
			req->res = 1;
		}
	}
	else
	{
		req->res = 1;
	}
}

void FacebookNet::Init()
{
	hack_fb_net_ptr = this;

	sign_out_listiner = null;

	for (int i=0;i<64;i++)
	{
		req_indices[i] = false;
	}

	listener.owner = this;

	signin_state = ss_not_signed;

	always_fail_sigin = 0;

#if defined(PC)	
	accessToken = std::string("access_token=") +  ReadINI("settings.ini", "" , "Facebook", "token");
	url_signin = ReadINI("settings.ini", "not found" , "Facebook", "url_signin");
	url_signin_res = ReadINI("settings.ini", "not found" , "Facebook", "url_signin_res");	
	always_fail_sigin = ReadINI("settings.ini", 0 , "Facebook", "always_fail_signin");	
#endif

	//guest_access_token_url.assign("http://srv.appnow.com/fb/token_desktop.txt");
	api_url.assign("https://graph.facebook.com/");
}

void FacebookNet::SignIn(FacebookRequestListiner* listiner, void* data)
{	
	if (FindRequest(req_sigin))
	{
		core->TraceTo("Facebook", "Error: previous sigin call not finished");
		return;
	}

	Request* req = AddRequest(req_sigin, listiner, data);

	signin_state = ss_signing;			

#if defined(ANDROID)
	CallFromJava("OpenFacebookSession");
#endif

#if defined(PC)	
	req->res = 0;
	req->fired = true;
	
	/*req->timeout = 2.5f;
	if (!url_signin.empty())
	{
		ShellExecute(null, "open", url_signin.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}*/
#endif	
    
#if defined(IOS)
	ios_wrapper->FBSignIn(req_permissions);
#endif
    req_permissions.clear();	
}

void FacebookNet::AddRequestPermistion(const char* name)
{    
	req_permissions.push_back(string(name)); 
}

void FacebookNet::StartRequestPermistions(FacebookRequestListiner* listiner, void* data)
{
	if (FindRequest(req_getpermission))
	{
		core->TraceTo("Facebook", "Error: previous RequestPermistions not finished");
		return;
	}

	Request* req = AddRequest(req_getpermission, listiner, data);

#if defined(PC)	|| defined(ANDROID)
	SetRequestPermistionsRes(true);
#endif

#ifdef IOS
	ios_wrapper->FBStartRequestPermistions(req_permissions);
#endif
	req_permissions.clear();
}

void FacebookNet::SetRequestPermistionsRes(bool res)
{
	Request* req = FindRequest(req_getpermission);

	if (!req)
	{
		core->TraceTo("Facebook", "Error: SetRequestPermistionsRes calling when there are no requests");
		return;
	}
	
	if (res)
	{
		req->res = 0;
	}
	else
	{
		req->res = 1;
	}
}

void FacebookNet::SignOut(FacebookSignoutListiner* listiner, void* data)
{	
	if (signin_state == ss_signouting)
	{
		core->TraceTo("Facebook", "Error: in sigout already");
		return;
	}

	sign_out_data = data;
	sign_out_listiner = listiner;	
	signin_state = ss_signouting;

#ifdef IOS
    ios_wrapper->FBSignOut();
#endif
    
#ifdef ANDROID
	CallFromJava("CloseFacebookSession");
#endif		
}

void FacebookNet::StartRequest(const char* method, const char* url, const char* params, const char* post_params, FacebookRequestListiner* listiner, void* data)
{						
    Request* req = AddRequest(req_graphapi, listiner, data);
		
	int k = strlen(params);

	if (url[0] == 'f' && url[1] == 'q' && url[2] == 'l')
	{		
		String::EscapeChars(params, sub_params, 16386);
		req_out_url = api_url + string(url) + "?" + accessToken + "&";
		req_out_url.append(sub_params);

		core->TraceTo("Facebook", params);
	}
	else
	{
		req_out_url = api_url + string(url) + "?" + accessToken + "&" + string(params);		
		core->TraceTo("Facebook", req_out_url.c_str());
	}	

	char str[32];

	for (int i=0;i<64;i++)
	{
		if (!req_indices[i])
		{
			req->index = i;
			req_indices[i] = true;
			String::Printf(str, 32, "fb_request%i", i);
			break;
		}
	}

	if (signin_state == 0)
	{
		req->res = 1;
		core->TraceTo("Facebook", "You are not signed");
	}
	else
	{
		core->Network()->StartRequest(&listener, req, method, req_out_url.c_str(), post_params, str);
	}
}

void FacebookNet::UploadFile(const char* url, const char* file_name, const char* param_name, const char* message, FacebookRequestListiner* listiner, void* data)
{
	Request* req = AddRequest(req_graphapi, listiner, data);

	char str[32];

	for (int i=0;i<64;i++)
	{
		if (!req_indices[i])
		{
			req->index = i;
			req_indices[i] = true;
			String::Printf(str, 32, "fb_request%i", i);
			break;
		}
	}

	req_out_url = api_url + string(url) + "?" + accessToken;		

	if (signin_state == 0)
	{
		req->res = 1;
		core->TraceTo("Facebook", "You are not signed");
	}
	else
	{
		core->Network()->UploadFileUsingForm(&listener, req, req_out_url.c_str(), file_name, param_name, message, "message", str);
	}
}

void FacebookNet::SetSignInStatus(bool in_singin, const char* token)
{
    Request* request = FindRequest(req_sigin);

	if (!request)
	{
        if (signin_state == ss_signed && in_singin == true)
        {
            accessToken = std::string("access_token=") + string(token);
            return;
        }
        
        core->TraceTo("Facebook", "Error: calling SetSignInStatus before SignIn");
		return;
	}
		
	if (in_singin)
	{
        request->res = 0;

		signin_state = ss_signed;		
	}
	else
	{
		request->res = 1;
		signin_state = ss_not_signed;		
	}

	core->TraceTo("Facebook", "SetSignInStatus, %d, %s", request->res, token);
	
	accessToken = std::string("access_token=") + string(token);
}

FacebookNet::SignInState FacebookNet::GetSignInStatus()
{
	return signin_state; 
}

FacebookNet::Request* FacebookNet::AddRequest(int set_id, FacebookRequestListiner* listiner, void* data)
{
	Request* request = new Request();

	request->ID = set_id;
	request->listiner = listiner;
	request->data = data;

	request->res = -1;

	requests.push_back(request);

	return request;
}

FacebookNet::Request* FacebookNet::FindRequest(int set_id)
{
	for (int i=0; i<requests.size(); i++)
	{
		if (requests[i]->ID == set_id && requests[i]->res == -1)
		{
			return requests[i];
		}
	}

	return null;
}

void FacebookNet::Work(float dt)
{
	for (int i=0;i<requests.size();i++)
	{
		Request* request = requests[i];
		
#ifdef PC
		request->timeout -= dt;
		if (request->timeout < 0.0f)
		{
			request->timeout = 0.0f;				
		
			if (requests[i]->ID == req_sigin && !requests[i]->fired)
			{
				core->Network()->StartRequest(&listener, request, null, url_signin_res.c_str(), "", "fb_sign_in");
				requests[i]->fired = true;
			}
		}
#endif
		if (requests[i]->res != -1)
		{			
			if (signin_state == ss_signouting)
			{
				//requests[i]->res = 1;
			}

			requests[i]->listiner->OnRequestFinish(requests[i]->res, requests[i]->data, requests[i]->res_file);			

			req_indices[request->index] = false;

			if (i != requests.size() - 1)
			{
				requests[i] = requests[requests.size() - 1];
			}

			delete request;
			requests.pop_back();
			i--;
		}		
	}

	if (signin_state == ss_signouting)
	{
		if (requests.size() == 0)
		{
			if (sign_out_listiner)
			{
				sign_out_listiner->OnFinish(sign_out_data);
				sign_out_listiner = null;

				signin_state = ss_not_signed;
			}
		}
	}
}

void FacebookNet::SetGuestAccessTokenURL(const char* url)
{
	guest_access_token_url.assign(url);	
}

void FacebookNet::SetApiURL(const char* url)
{
	api_url.assign(url);
}

void FacebookNet::WaitReqest()
{
	for (int i=0;i<requests.size();i++)
	{
		Request* request = requests[i];

		while (requests[i]->res == -1)
		{
			((NetworkService*)en_core.Network())->Work(0, 0);
			ThreadWorker::Sleep(100);
		}

		req_indices[request->index] = false;

		delete request;
	}

	requests.clear();
}

void FacebookNet::Reset()
{
	WaitReqest();	

	signin_state = ss_not_signed;
}