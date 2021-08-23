#include "Core/Core.h"

#ifndef IOS
#include <curl/curl.h>
#include <curl/easy.h>
//#include "openssl\hmac.h"
//#include "lib\curl_base64.h"
//#include "lib\memdebug.h"
#endif

#ifdef IOS
#include "sys/xattr.h"
#include "Reachability.h"
#endif

#ifdef ANDROID
extern void CallFromJava(const char* function);
#endif

#ifdef IOS
#import "NetDelegate.h"

NSMutableArray* ios_net_delegates;
NetDelegate* net_delegate[7];
NetworkService::CURLRequest* hack_CURLRequest[7];
#endif

#ifndef IOS

int curldebug_callback(CURL* curl, curl_infotype tp, char* err, size_t sz, void* data)
{
	if (sz <1024)
	{
		char str[1024];
		memcpy(str, err, sz);
		str[sz] = 0;
		core->TraceTo("Network", str);
	}

	return 0;
}

void NetworkService::ReqestWorker::Prepare()
{
	owner->state = rq_working;
}

void NetworkService::ReqestWorker::Loop()
{
	owner->res = curl_easy_perform(owner->curl);	
	//owner->res = 0;
	
	if (owner->stream)
	{		
		fclose(owner->stream);
		owner->stream = null;

		if (owner->res != 0)
		{
			char fl_nm[512];
			sprintf(fl_nm, "%s_tmp", owner->full_file_name);

			DeleteFile(fl_nm);
		}
	}

#ifdef EDITOR	
	if (owner->stream_cache) fclose(owner->stream_cache);
#endif
	
	if (owner->need_del_curl)
	{			
		curl_easy_cleanup(owner->curl);
	}	

	owner->state = rq_finished;
}

int NetworkService::curl_get_data(void *buffer, size_t size, size_t nmemb, void *stream)
{
	NetworkService::CURLRequest* out = (NetworkService::CURLRequest*)stream;
	
	/*if (out->is_canceled)
	{
		if (out->stream)
		{		
			fclose(out->stream);
			out->stream = null;

			DeleteFile(out->full_file_name);
		}		

#ifdef EDITOR	
		if (out->stream_cache)
		{
			fclose(out->stream_cache);
			DeleteFile(out->full_file_name_cache);
		}
#endif

		return -1;
	}*/

	if (out->write_xml_only && !out->start_write_xml_only)
	{
		//find <?xml
		if (size * nmemb >= 5)
		{
			char str[6];
			memcpy(str, buffer, 5);
			str[5] = 0;
			if (String::IsEqual(str, "<?xml"))
			{
				out->start_write_xml_only = true;
			}
		}
	}

	if(out && !out->stream)
	{    
		char fl_nm[512];
		sprintf(fl_nm, "%s_tmp", out->full_file_name);

		out->stream=fopen(fl_nm, "wb");
    
		if (!out->stream)
		{
			core->TraceTo("Network", "can't open %s", out->file_name);
			return -1;
		}

#ifdef EDITOR
		if (out->full_file_name_cache[0])
		{
			out->stream_cache=fopen(out->full_file_name_cache, "wb");
    
			if (!out->stream_cache)
			{
				core->TraceTo("Network", "can't open %s", out->file_name);
				return -1;
			}
		}
#endif
	}

	if (!out->write_xml_only)
	{
#ifdef EDITOR		    
		if (out->stream_cache)
		{
			fwrite(buffer, size, nmemb, out->stream_cache);					
		}
#endif

		return fwrite(buffer, size, nmemb, out->stream);
	}
	
	if (out->start_write_xml_only)
	{
		return fwrite(buffer, size, nmemb, out->stream);
	}
		
	return size * nmemb;	
}

int write_header(void *buffer, size_t size, size_t nmemb, void *stream)
{
	//NetworkService::CURLRequest* out = (NetworkService::CURLRequest*)stream;
	//char response_header[512];
	//String::Copy(response_header, 512, (const char*)buffer);
	//int total = size * nmemb;
	//if ( total <= 512 && total > 0)
	//	response_header[total - 1] = 0;

	//return size * nmemb;

	//response_header[255] = 0;
	return 0;
}

size_t NetworkService::read_data(char *bufptr, size_t size, size_t nitems, void *userp)
{
	size_t read;
	read = fread(bufptr, size, nitems, (FILE*)userp);
	return read;		
}
#endif

NetworkService::NetworkService()
{
	is_internet_connected = true;

	active_requests = 0;

#ifdef ANDROID
	is_internet_connected = true;
#endif

#ifdef EDITOR
	need_cache_net = false;
	use_cache_net = false;
#endif

	num_chandles = 7;	
}

bool NetworkService::Init()
{
#ifndef IOS
	curl_global_init(CURL_GLOBAL_ALL);

	for (int i=0; i<num_chandles; i++)
	{
		CURL* curl = curl_easy_init();		
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_get_data);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);								
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);

		chandles[i].curl = curl;
		chandles[i].used = false;
	}


	curl_version_info_data * data = curl_version_info(CURLVERSION_NOW);

	bool ip6 = data->features & CURL_VERSION_IPV6;
#endif
    
    
#ifdef IOS
    for (int i=0; i<num_chandles; i++)
    {
        net_delegate[i] = [[NetDelegate alloc] init];
        net_delegate[i].index = i;
        net_delegate[i].used = false;
    }
#endif
    
	core->AddTask(this, this, (TaskHolder::Task)&NetworkService::Work, -320, -1.f);
    
#ifdef ANDROID
	core->AddTask(this, this, (TaskHolder::Task)&NetworkService::InetChecker, -330, 2.5f);
	CallFromJava("CheckInternet");
#endif		

	//GetHMac

	return true;
}

bool NetworkService::IsInternetConnected()
{
#ifdef IOS
    
    Reachability *reachability = [Reachability reachabilityForInternetConnection];
    [reachability startNotifier];
    
    NetworkStatus status = [reachability currentReachabilityStatus];
    
    if(status == NotReachable)
    {
        return false;
    }
    else if (status == ReachableViaWiFi)
    {
        //WiFi
    }
    else if (status == ReachableViaWWAN)
    {
        //3G
    }
    
    return true;
#endif
            
	return is_internet_connected;
}

#ifdef EDITOR
void NetworkService::SetWriteCache(bool set)
{
	need_cache_net  = set;
}

void NetworkService::SetUseCache(bool set)
{
	use_cache_net  = set;
}

#endif

#if defined(ANDROID) || defined(PC)

void NetworkService::SetInternetConnected(bool set)
{	
	if (!is_internet_connected && set)
	{	
		core->TraceTo("Network", "Network on");		
	}
	else
	if (is_internet_connected && !set)
	{
		core->TraceTo("Network", "Network off");		
	}

	is_internet_connected = set;
}

#endif

#ifdef ANDROID

void NetworkService::InetChecker(float dt, int level)
{	
	CallFromJava("CheckInternet");
}

#endif

void NetworkService::Work(float dt, int level)
{
	for (int i=0;i<requests.size();i++)
	{
		CURLRequest* request = requests[i];

		if (active_requests < num_chandles && request->state == rq_waiting_start)
		{
			active_requests++;

			if (is_internet_connected)
			{
#ifndef IOS
				if (!request->need_del_curl)
				{
					PrepareCURL(request);
				}

				request->worker.Start(ThreadWorker::normal);
#endif
                
#ifdef IOS
                request->state = rq_working;
                
                int index = -1;
                
                for (int i=0; i<num_chandles; i++)
                {
                    if (!net_delegate[i].used)
                    {
                        index = i;
                        request->net_index = index;
                        net_delegate[index].used = true;
                        break;
                    }
                }
      
                hack_CURLRequest[index] = request;

                NSString *imageFilename = [NSString stringWithFormat:@"%s", request->uri.c_str()];
                NSMutableURLRequest *nsrequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:imageFilename]];
                
                //NSLog(imageFilename);
                
                if (request->post_params && request->post_params[0])
                {
                    [nsrequest setHTTPMethod:@"POST"];
                    NSString *postString = [NSString stringWithFormat:@"%s", request->post_params];
                    
                    //NSData* dt = [[NSData alloc] initWithBytes:[postString UTF8String] length:[postString length]];
                    //[nsrequest setHTTPBody:dt];
                    
                    [nsrequest setHTTPBody:[postString dataUsingEncoding:NSUTF8StringEncoding]];
                }
                
                NSURLConnection *conn = [[NSURLConnection alloc] initWithRequest:nsrequest delegate:net_delegate[index]];
    
#endif
			}
			else
			{
				request->state = rq_finished;
				request->res = 1;
			}
		}		

		if (
#ifndef IOS
            !request->worker.IsWorking() &&
#endif
            request->state == rq_finished)
		{
			core->TraceTo("Net", "%s %i", request->file_name, request->res);			

			char fl_nm[512];
			sprintf(fl_nm, "%s_tmp", request->full_file_name);

			DeleteFile(request->full_file_name);
			rename(fl_nm, request->full_file_name);

			if (request->listiner && !request->is_canceled) request->listiner->OnRequestFinish(request->res, request->reqest_data, request->file_name);			

			if (i != requests.size() - 1)
			{
				requests[i] = requests[requests.size() - 1];
			}

			requests.pop_back();

			for (int j=0;j<requests.size();j++)
			{
				CURLRequest* sim_request = requests[j];

				if (sim_request->state == rq_waiting_similar)
				{
					if (String::IsEqual(request->uri.c_str(), sim_request->uri.c_str()))
					{
						if (sim_request->listiner && !sim_request->is_canceled) sim_request->listiner->OnRequestFinish(request->res, sim_request->reqest_data, request->file_name);			

						if (j != requests.size() - 1)
						{
							requests[j] = requests[requests.size() - 1];
						};

						requests.pop_back();

						if (sim_request->post_params) free(sim_request->post_params);
						delete sim_request;
						j--;

						if (j<i) i--;
					}
				}
			}

#ifndef IOS
			if (request->post_params) free(request->post_params);

			for (int i = 0; i<num_chandles; i++)
			{
				if (chandles[i].curl == request->curl)
				{
					chandles[i].used = false;
					break;
				}
			}
#endif

#ifdef IOS
			net_delegate[request->net_index].used = false;
#endif

			delete request;
			i--;
			
			active_requests--;
		}		
	}
}

#ifndef IOS
void NetworkService::PrepareCURL(CURLRequest* curl_request)
{	
	for (int i=0; i<num_chandles; i++)
	{
		if (!chandles[i].used)
		{
			curl_request->curl = chandles[i].curl;
			chandles[i].used = true;
			break;
		}
	}

	/*curl_request->curl = curl_easy_init();		
	curl_easy_setopt(curl_request->curl, CURLOPT_WRITEFUNCTION, curl_get_data);
	curl_easy_setopt(curl_request->curl, CURLOPT_VERBOSE, 0);								
	curl_easy_setopt(curl_request->curl, CURLOPT_SSL_VERIFYPEER, false);
	curl_easy_setopt(curl_request->curl, CURLOPT_SSL_VERIFYHOST, false);

	curl_request->need_del_curl = true;*/

	curl_easy_setopt(curl_request->curl, CURLOPT_URL, curl_request->uri.c_str());

	FILE* fd = null;
	int fl_sz = 0;

	if (curl_request->upload_file.length() > 0)
	{
		fd = fopen(curl_request->upload_file.c_str(), "rb");

#ifdef ANDROID
		if (!fd)
		{
			string uploadFullPath =  core->Files()->GetBasePath();
			uploadFullPath += "/";
			uploadFullPath += curl_request->upload_file;
			fd = fopen(uploadFullPath.c_str(), "rb");
		}
#endif


#ifdef IOS
		if (!fd)
		{
            char temp[512];
            String::Copy(temp, 512, curl_request->upload_file.c_str());
            String::LowerCase(temp);
			NSString *imageFilename = [NSString stringWithFormat:@"%s", temp];
			NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
			NSString *path = [NSString stringWithFormat:@"%@/%@", [paths objectAtIndex:0], imageFilename];
            fd = fopen(path.UTF8String, "rb");			
		}		
#endif

		if (!fd)
		{
			core->TraceTo("Network", "StartRequest, cannot open %s", curl_request->upload_file.c_str());
			return;
		}	

		fseek(fd, 0, SEEK_END);
		fl_sz = ftell(fd);	
		fseek(fd, 0, SEEK_SET);	

		curl_easy_setopt(curl_request->curl, CURLOPT_UPLOAD, 1);
		curl_easy_setopt(curl_request->curl, CURLOPT_READFUNCTION, read_data);
		curl_easy_setopt(curl_request->curl, CURLOPT_READDATA, fd);
		curl_easy_setopt(curl_request->curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)fl_sz);
	}
	else
	{
		curl_easy_setopt(curl_request->curl, CURLOPT_UPLOAD, 0);
	}	
							
	curl_easy_setopt(curl_request->curl, CURLOPT_WRITEDATA, curl_request);						 				

	if (curl_request->post_params && curl_request->post_params[0])
	{									
		if (curl_request->upload_file.length() > 0)
		{
			core->TraceTo("Network", "Warning, post_params and upload_file are not compatible, CURLOPT_READFUNCTION is ignored");
		}

		curl_easy_setopt(curl_request->curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl_request->curl, CURLOPT_POSTFIELDS, curl_request->post_params);
	}				
	else
	{
		curl_easy_setopt(curl_request->curl, CURLOPT_POSTFIELDS, NULL);
		curl_easy_setopt(curl_request->curl, CURLOPT_HTTPGET, 1);		
	}
		
	if (curl_request->headers.length() > 0)
	{
		curl_easy_setopt(curl_request->curl, CURLOPT_HEADER,1);
		
		struct curl_slist *slist=NULL;

		std::string str = curl_request->headers;
		string header;
		int pos1 = 0;
		int pos2 = 0;
			
		while(1) 
		{
			pos2 = str.find_first_of("\n", pos1);

			if (pos2 == std::string::npos)
			{
				pos2 = str.size();
			}
			
			header = str.substr(pos1, pos2 - pos1);
			slist = curl_slist_append(slist, header.c_str());
			pos1 = pos2+1;

			if (pos1 >= str.size())
			{
				break;
			}
		}

		curl_easy_setopt(curl_request->curl, CURLOPT_HTTPHEADER, slist); 
	}
	else
	{
		curl_easy_setopt(curl_request->curl, CURLOPT_HEADER,0);
	}
	
	if (curl_request->method && curl_request->method[0])
	{
		curl_easy_setopt(curl_request->curl, CURLOPT_CUSTOMREQUEST, curl_request->method);
	}
	else
	{
		curl_easy_setopt(curl_request->curl, CURLOPT_CUSTOMREQUEST, NULL);
	}

	/*struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "QuickBlox-REST-API-Version: 0.1.0");	
	curl_easy_setopt(curl_request->curl, CURLOPT_HEADER,headers);*/
}
#endif

void* NetworkService::StartRequest(RequestListiner* listiner, void* reqest_data, const char* method,
								  const char* request, const char* post_params, const char* response_file)
{
	return StartRequest( listiner, reqest_data, method, request, post_params, null, null, response_file);
}


void* NetworkService::StartRequest(RequestListiner* listiner, void* reqest_data, const char* method, 
								  const char* request, const char* post_params, const char* headers, 
								  const char* upload_file, const char* response_file)
{		
	CURLRequest* curl_request = new CURLRequest();

	curl_request->listiner = listiner;
	curl_request->reqest_data = reqest_data;
	curl_request->uri = request;

	if (method)
	{
		String::Copy(curl_request->method, 16, method);
	}

	if (post_params && post_params[0])
	{	
		int len = strlen(post_params);
		curl_request->post_params = (char*)malloc(len + 1);
		memcpy(curl_request->post_params, post_params, len);
		curl_request->post_params[len]=0;
	}	

	if (headers)
	{
		curl_request->headers = headers;

		if (String::IsSubstr(headers, "Accept: application/atom+xml"))
		{
			curl_request->write_xml_only = true;
		}		
	}

	if (is_internet_connected)
	{
#ifdef EDITOR

		int need_network_req = 1;

		if (use_cache_net)
		{									
			char md1[48];
			GetMD5Hash(curl_request->uri.c_str(), md1);
			
			char md2[48];
			md2[0] = 0;

			if (curl_request->post_params)
			{
				GetMD5Hash(curl_request->post_params, md2);
			}

			String::Printf(curl_request->full_file_name, 512, "%s/net_cache/%s%s", en_core.EN_Files()->GetAppDirectory(), md1 , md2);
			String::Printf(curl_request->file_name, 512, "net_cache/%s%s", md1 , md2);
			
			FILE* fd = fopen(curl_request->full_file_name, "rb");

			if (fd)
			{
				curl_request->state = rq_finished;
				curl_request->res = 0;
				need_network_req = 0;
				fclose(fd);
			}
			else
			{
				need_network_req = 2;
			}
		}
		
		if (need_network_req > 0)
#endif
		{
			for (int i=0;i<requests.size();i++)
			{		
				if (requests[i]->state != rq_waiting_similar) 
				{
					if (String::IsEqual(requests[i]->uri.c_str(), request)
						&& String::IsEqual(requests[i]->upload_file.c_str(), curl_request->upload_file.c_str()))
					{		
						core->TraceTo("debug", requests[i]->uri.c_str());

						if (requests[i]->post_params && curl_request->post_params)
						{
							if (!String::IsEqual(requests[i]->post_params, curl_request->post_params))
							{
								continue;
							}
						}
	
						curl_request->state = rq_waiting_similar;
						break;
					}
				}
			}

			if (curl_request->state != rq_waiting_similar)
			{
#if defined(PC) || defined(OSUNIX)
#if defined(PC)
				String::Printf(curl_request->full_file_name, 512, "%s/from_net", en_core.EN_Files()->GetAppDirectory());
				CreateDirectory(curl_request->full_file_name, 0);
#endif

				String::Printf(curl_request->full_file_name, 512, "%s/from_net/%s", en_core.EN_Files()->GetAppDirectory(), response_file);
				String::Printf(curl_request->file_name, 512, "from_net/%s", response_file);
#endif
		
#ifdef ANDROID
				String::Printf(curl_request->full_file_name, 512, "%s/%s", core->Files()->GetBasePath(), response_file);	
				String::Printf(curl_request->file_name, 512, "%s", response_file);
#endif
        
#ifdef IOS        
				NSString *imageFilename = [NSString stringWithFormat:@"%s", response_file];
				NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
				NSString *path = [NSString stringWithFormat:@"%@/%@", [paths objectAtIndex:0], imageFilename];
	                        
				const char* fl_name = [path UTF8String];
		        
				String::Printf(curl_request->full_file_name, 512, "%s", fl_name);
				String::Printf(curl_request->file_name, 512, "%s", fl_name);        
#endif    				
				
				if ( upload_file )
				{
					curl_request->upload_file = upload_file;
				}

#ifndef IOS
				curl_request->worker.owner = curl_request;
#endif

#ifdef EDITOR
				if (need_cache_net || need_network_req == 2)
				{
					String::Printf(curl_request->full_file_name_cache, 512, "%s/net_cache", en_core.EN_Files()->GetAppDirectory());
					CreateDirectory(curl_request->full_file_name_cache, 0);

					char md1[48];
					GetMD5Hash(curl_request->uri.c_str(), md1);
			
					char md2[48];
					md2[0] = 0;

					if (curl_request->post_params)
					{
						GetMD5Hash(curl_request->post_params, md2);
						core->TraceTo("net_cache", "URL %s Post %s Hash - %s%s", curl_request->uri.c_str(), curl_request->post_params, md1, md2);
					}
					else
					{
						core->TraceTo("net_cache", "URL %s Hash - %s%s", curl_request->uri.c_str(), md1, md2);
					}
			
					String::Printf(curl_request->full_file_name_cache, 512, "%s/net_cache/%s%s", en_core.EN_Files()->GetAppDirectory(), md1, md2);										
				}
				else
				{
					curl_request->full_file_name_cache[0] = 0;
				}
#endif 
			}
		}
	}
	else
	{
		curl_request->res = 1;
	}		

	requests.push_back(curl_request);

	return curl_request;
}

void NetworkService::CancelRequaest(void* req)
{
	CURLRequest* out = (CURLRequest*)req;

	out->is_canceled = true;
}

void NetworkService::UploadFileUsingForm(RequestListiner* listiner, void* reqest_data, const char* request, const char* file_name, const char* param_name, const char* param, const char* param_name2, const char* response_file)
{	
	CURLRequest* curl_request = new CURLRequest();

	curl_request->listiner = listiner;
	curl_request->reqest_data = reqest_data;
	curl_request->uri = request;	
	
	if (is_internet_connected)
	{
#ifdef EDITOR
		if (use_cache_net)
		{
			curl_request->state = rq_finished;
			curl_request->res = 0;
						
			char md1[48];
			GetMD5Hash(curl_request->uri.c_str(), md1);
			
			char md2[48];
			md2[0] = 0;

			if (curl_request->post_params)
			{
				GetMD5Hash(curl_request->post_params, md2);
			}

			String::Printf(curl_request->full_file_name, 512, "%s/net_cache/%s%s", en_core.EN_Files()->GetAppDirectory(), md1 , md2);
			String::Printf(curl_request->file_name, 512, "net_cache/%s%s", md1 , md2);
		}
		else
#endif
		{			
#if defined(PC) || defined(OSUNIX)
#if defined(PC)
			String::Printf(curl_request->full_file_name, 512, "%s/from_net", en_core.EN_Files()->GetAppDirectory());
			CreateDirectory(curl_request->full_file_name, 0);
#endif

			String::Printf(curl_request->full_file_name, 512, "%s/from_net/%s", en_core.EN_Files()->GetAppDirectory(), response_file);
			String::Printf(curl_request->file_name, 512, "from_net/%s", response_file);
#endif
		
#ifdef ANDROID
			String::Printf(curl_request->full_file_name, 512, "%s/%s", core->Files()->GetBasePath(), response_file);	
			String::Printf(curl_request->file_name, 512, "%s", response_file);
#endif
        
#ifdef IOS        
			NSString *imageFilename = [NSString stringWithFormat:@"%s", response_file];
			NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
			NSString *path = [NSString stringWithFormat:@"%@/%@", [paths objectAtIndex:0], imageFilename];
	                        
			const char* fl_name = [path UTF8String];
		        
			String::Printf(curl_request->full_file_name, 512, "%s", fl_name);
			String::Printf(curl_request->file_name, 512, "%s", fl_name);        
#endif    				
							
#ifndef IOS
			curl_request->worker.owner = curl_request;
            
			curl_httppost *formpost=NULL;
			curl_httppost *lastptr=NULL;			
#endif
            

            const char* fl_name2 = file_name;

#ifdef IOS
            FILE* fd = null;
            int fl_sz = 0;
            
            fd = fopen(fl_name2, "rb");
            
            NSString* path2;
            
            if (!fd)
            {
                char temp[512];
                String::Copy(temp, 512, file_name);
                String::LowerCase(temp);
                NSString *imageFilename = [NSString stringWithFormat:@"%s", temp];
                NSArray *paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
                path2 = [NSString stringWithFormat:@"%@/%@", [paths objectAtIndex:0], imageFilename];

                fl_name2 = [path2 UTF8String];
            }
            else
            {
                fclose(fd);
            }
            
            
#endif
            
#ifndef IOS
			curl_formadd(&formpost,
						 &lastptr,
						 CURLFORM_COPYNAME, param_name,
						 CURLFORM_FILE, fl_name2,
						 CURLFORM_END);
            
			if (param_name2)
			{
				curl_formadd(&formpost,
							 &lastptr,
							 CURLFORM_COPYNAME, param_name2,
							 CURLFORM_COPYCONTENTS, param,
							 CURLFORM_END);
			}

			curl_request->curl = curl_easy_init();
			curl_request->need_del_curl = true;
	
			curl_easy_setopt(curl_request->curl, CURLOPT_URL, curl_request->uri.c_str());
							
			curl_easy_setopt(curl_request->curl, CURLOPT_WRITEFUNCTION, curl_get_data);	
			curl_easy_setopt(curl_request->curl, CURLOPT_WRITEDATA, curl_request);
						 
			curl_easy_setopt(curl_request->curl, CURLOPT_VERBOSE, 0);
		
			curl_easy_setopt(curl_request->curl, CURLOPT_SSL_VERIFYPEER, false);
			curl_easy_setopt(curl_request->curl, CURLOPT_SSL_VERIFYHOST, false);								

			curl_easy_setopt(curl_request->curl, CURLOPT_HTTPPOST, formpost);
#endif
		}
	}
	else
	{
		curl_request->res = 1;
	}		

	requests.push_back(curl_request);
}

void NetworkService::Reset()
{
	for (int i=0;i<requests.size();i++)
	{
		CURLRequest* request = requests[i];
        CancelRequaest(request);
	}
}

void NetworkService::Release()
{
	Reset();
}






