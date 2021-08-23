#include "Core/Core.h"
#include "Common/ThreadWorker.h"
#include "Common/Utils.h"

#ifdef IOS

#import "Platforms/IOS/IOSAppDelegate.h"

//EXT
/*
extern IOSAppDelegate* app_delegate;
 */

#endif

#if defined(ANDROID)
extern void CallFromJava(const char* function);
extern void CallFromJava(const char* function,  const char* param);
#endif

void OkNet::OkInnerRequestListiner::OnRequestFinish(int res, void* data, const char* file_name)
{	
	OkNet::Request* req = (OkNet::Request*)data;
    
	if (res == 0)
	{		
		IDataFile* file = core->Files()->OpenJSONFile(file_name);	
	
		if (file)
		{
			if (file->EnterBlock("error"))
			{
				req->res = 1;		
				char error[256];
				file->ReadString("message", error, 256);

				core->TraceTo("Facebook", error);
			}
			else
			{
				req->res = 0;		
						
				if (req->ID == req_graphapi)
				{
					String::Copy(req->res_file, 512, file_name);
				}

#ifdef PC
				if (req->ID == req_sigin)
				{
					char token[256];
					file->ReadString("access_token", token, 256);

					core->TraceTo("OK", "token : %s", token);

					if (token[0])
					{
						owner->accessToken = token;
						owner->signin = true;
					}
					else
					{
						req->res = -1;
						req->timeout = 1.5f;
						req->fired = false;
					}
				}
#endif
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

void OkNet::Init()
{
	listener.owner = this;

	signin = false;

	app_id[0] = 0;
	app_key[0] = 0;
	secret_key[0] = 0;

#if defined(PC)	
	url_signin = ReadINI("settings.ini", "not found" , "OK", "url_signin");
	url_signin_res = ReadINI("settings.ini", "not found" , "OK", "url_signin_res");	
#endif
}

void OkNet::SetParams(const char* set_app_id, const char* set_app_key, const char* set_secret_key)
{
	String::Copy(app_id, 64, set_app_id);
	String::Copy(app_key, 64, set_app_key);
	String::Copy(secret_key, 64, set_secret_key);
}

void OkNet::SignIn(OkRequestListiner* listiner, void* data)
{	
	if (!app_id[0])
	{
		core->TraceTo("Ok", "Error: SetParams not called");
		return;
	}	

	if (FindRequest(req_sigin))
	{
		core->TraceTo("OK", "Error: previous call not finished");
		return;
	}

	Request* req = AddRequest(req_sigin, listiner, data);

#if defined(ANDROID)
	//CallFromJava("OpenFacebookSession");
	CallFromJava("OpenOkSession");
#endif

#if defined(PC)	
	req->timeout = 2.5f;
	ShellExecute(null, "open", url_signin.c_str(), NULL, NULL, SW_SHOWNORMAL);	
#endif	
    
#if defined(IOS)
    
    //EXT
    /*
    [app_delegate openSessionWithAllowLoginUI : YES];
     */
#endif
}

void OkNet::SignOut()
{	
	WaitReqest();
	signin = false;
}

void OkNet::StartRequest(const char* url, const char* params, const char* post_params, OkRequestListiner* listiner, void* data)
{	
	if (!signin)
	{
		core->TraceTo("Ok", "You are not signed");
		return;
	}

	Request* req = AddRequest(req_graphapi, listiner, data);	
				
	string s0 = accessToken + secret_key;

	char md5[48];
	GetMD5Hash(s0.c_str(), md5);

	s0 = "application_key=" + string(app_key) + string(md5);

	GetMD5Hash(s0.c_str(), md5);
	
	char out_url[1024];
	sprintf(out_url, "http://api.odnoklassniki.ru/api/%s?application_key=%s&sig=%s&access_token=%s%s", url, app_key, md5, accessToken.c_str(), params);		

	core->TraceTo("Ok", out_url);
	core->Network()->StartRequest(&listener, req, null, out_url, post_params, "okRequest");
}

void OkNet::SetSignInStatus(bool in_singin, const char* token)
{
	Request* request = FindRequest(req_sigin);

	if (!request)
	{
		core->TraceTo("OK", "Error: calling SetSignInStatus before SignIn");
	}
		
	if (in_singin)
	{
		request->res = 0;
	}
	else
	{
		request->res = 1;
	}

	core->TraceTo("OK", "SetSignInStatus, %d, %s", in_singin, token);

	signin = in_singin;
	accessToken = std::string("access_token=") + token;		
}

bool OkNet::GetSignInStatus()
{
	return signin; 
}

OkNet::Request* OkNet::AddRequest(int set_id, OkRequestListiner* listiner, void* data)
{
	Request* request = new Request();

	request->ID = set_id;
	request->listiner = listiner;
	request->data = data;

	request->res = -1;

	requests.push_back(request);

	return request;
}

OkNet::Request* OkNet::FindRequest(int set_id)
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

void OkNet::Work(float dt)
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
				core->Network()->StartRequest(&listener, request, null, url_signin_res.c_str(), "", "fb_szign_in");
				requests[i]->fired = true;
			}
		}
#endif
		if (requests[i]->res != -1)
		{
			requests[i]->listiner->OnRequestFinish(requests[i]->res, requests[i]->data, requests[i]->res_file);			

			if (i != requests.size() - 1)
			{
				requests[i] = requests[requests.size() - 1];
			}

			delete request;
			requests.pop_back();
			i--;
		}		
	}
}

void OkNet::WaitReqest()
{
	for (int i=0;i<requests.size();i++)
	{
		Request* request = requests[i];

		delete request;
	}

	requests.clear();
}

void OkNet::Reset()
{
	WaitReqest();
	signin = false;

	app_id[0] = 0;
	app_key[0] = 0;
	secret_key[0] = 0;
}