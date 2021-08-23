#include "Core/Core.h"
#include "Common/ThreadWorker.h"
#include "Common/Utils.h"

#if defined(ANDROID)
extern void CallFromJava(const char* function);
extern void CallFromJava(const char* function,  const char* param);
#endif

#ifdef IOS

#include "Platforms/IOS/IOSWrapper.h"

#endif

void VKNet::VKInnerRequestListiner::OnRequestFinish(int res, void* data, const char* file_name)
{	
	VKNet::Request* req = (VKNet::Request*)data;
    
	if (res == 0)
	{		
        String::Copy(req->res_file, 512, file_name);

		IDataFile* file = null;
		
		if (req->is_xml)
		{
			file = core->Files()->OpenXMLFile(file_name, "response");
		}
		else
		{
			file = core->Files()->OpenJSONFile(file_name);	
		}

		if (file)
		{			
			if (file->EnterBlock("error"))
			{
				req->res = 1;		
				char error[256];
				file->ReadString("error_msg", error, 256);

				file->ReadInt("error_code", req->res);				

				core->TraceTo("VK", "error - %i, %s", req->res, error);
			}
			else
			{
				req->res = 0;										

				if (req->ID == req_sigin)
				{
					char token[256];
					file->ReadString("access_token", token, 256);

					core->TraceTo("VK", "token : %s", token);


					int64 user_id = 0;
					file->ReadInt64("user_id", user_id);

					sprintf(owner->user_id, "%i", user_id);

					if (token[0])
					{
						owner->accessToken = token;						
												
						if (owner->signin_state == ss_signing)
						{
							owner->signin_state = ss_signed;
						}
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

void VKNet::Init()
{
	for (int i=0;i<64;i++)
	{
		req_indices[i] = false;
	}

	listener.owner = this;

	signin_state = ss_not_signed;
	
	user_id[0] = 0;

#if defined(PC)
	url_signin = ReadINI("settings.ini", "not found" , "VK", "url_signin");
	url_signin_res = ReadINI("settings.ini", "not found" , "VK", "url_signin_res");	
#endif

	//guest_access_token_url.assign("http://appnow.com/vk/token.txt");
	api_url.assign("https://api.vk.com/method/");
}

void VKNet::SignIn(VKRequestListiner* listiner, void* data)
{
	if (FindRequest(req_sigin))
	{
		core->TraceTo("VK", "Error: previous sigin call not finished");
		return;
	}

	Request* req = AddRequest(req_sigin, listiner, data);

	signin_state = ss_signing;

#if defined(ANDROID)	
	CallFromJava("OpenVKSession");
#endif

#if defined(PC)	
	req->timeout = 2.5f;
	//ShellExecute(null, "open", url_signin.c_str(), NULL, NULL, SW_SHOWNORMAL);	
#endif	
    
#if defined(IOS)
	ios_wrapper->VKSignIn();        
#endif	
}

void VKNet::SignOut(VKSignoutListiner* listiner, void* data)
{	
	if (signin_state == ss_signouting)
	{
		core->TraceTo("VK", "Error: in sigout already");
		return;
	}

	sign_out_data = data;
	sign_out_listiner = listiner;	
	signin_state = ss_signouting;
    
#ifdef IOS
    ios_wrapper->VKSignOut();    
#endif

#ifdef ANDROID
	CallFromJava("ClearWebViewCache");
#endif

	accessToken = "";
}

const char* VKNet::GetUserID()
{
	return user_id;
}

void VKNet::StartRequest(const char* url, const char* params, const char* post_params, VKRequestListiner* listiner, void* data)
{	
	/*if (signin_state == 0)
	{
		core->TraceTo("VK", "You are not signed");
		return;
	}*/

	Request* req = AddRequest(req_graphapi, listiner, data);	
					
	char str[32];

	for (int i=0;i<64;i++)
	{
		if (!req_indices[i])
		{
			req->index = i;
			req_indices[i] = true;
			String::Printf(str, 32, "vk_request%i", i);

#ifdef EDITOR
			static int index = 0;
			index++;
			String::Printf(str, 32, "vk_request%i", index);
#endif
			break;
		}
	}

	req_out_url = api_url + string(url) + string("?") + string(params);
	
	if (!accessToken.empty())
	{
		req_out_url += string("&access_token=") + accessToken;
	}

	if (strstr(req_out_url.c_str(), ".xml"))
	{
		req->is_xml = true;
	}

	core->TraceTo("VK", req_out_url.c_str());

	core->Network()->StartRequest(&listener, req, null, req_out_url.c_str(), post_params, str);
}

void VKNet::SetSignInStatus(bool in_singin, const char* token, const char* set_user_id)
{
	core->TraceTo("VK", "SetSignInStatus");

	Request* request = FindRequest(req_sigin);

	if (!request)
	{
		core->TraceTo("VK", "Error: calling SetSignInStatus before SignIn");
		return;
	}
		
	if (in_singin)
	{
		request->res = 0;

		signin_state = ss_signed;		

        accessToken = token;
	}
	else
	{
		request->res = 1;
		signin_state = ss_not_signed;		
		accessToken = "";
	}

	core->TraceTo("VK", "SetSignInStatus, %d, %s", in_singin, token);

	String::Copy(user_id, 64, set_user_id);
}

void VKNet::UploadFile(const char* url, const char* file_name, VKRequestListiner* listiner, void* data)
{
	Request* req = AddRequest(req_graphapi, listiner, data);		

	char str[32];

	for (int i=0;i<64;i++)
	{
		if (!req_indices[i])
		{
			req->index = i;
			req_indices[i] = true;
			String::Printf(str, 32, "vk_request%i", i);

#ifdef EDITOR
			static int index = 0;
			index++;
			String::Printf(str, 32, "vk_request%i", index);
#endif
			break;
		}
	}	

	if (signin_state == 0)
	{
		req->res = 1;
		core->TraceTo("VK", "You are not signed");
	}
	else
	{
		core->Network()->UploadFileUsingForm(&listener, req, url, file_name, "photo", null, null, str);
	}
}

VKNet::SignInState VKNet::GetSignInStatus()
{
	return signin_state; 
}

VKNet::Request* VKNet::AddRequest(int set_id, VKRequestListiner* listiner, void* data)
{
	Request* request = new Request();

	request->ID = set_id;
	request->listiner = listiner;
	request->data = data;

	request->res = -1;

	requests.push_back(request);

	return request;
}

VKNet::Request* VKNet::FindRequest(int set_id)
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

void VKNet::Work(float dt)
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
				core->Network()->StartRequest(&listener, request, null, url_signin_res.c_str(), "", "vk_sign_in");
				requests[i]->fired = true;
			}
		}
#endif		

		if (requests[i]->res != -1)
		{
			if (signin_state == ss_signouting)
			{
				///requests[i]->res = 1;
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

void VKNet::SetApiURL(const char* url)
{
	api_url.assign(url);
}

void VKNet::WaitReqest()
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

void VKNet::Reset()
{
	WaitReqest();
	signin_state = ss_not_signed;	
	accessToken.clear();
}