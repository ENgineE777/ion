#include "Core/Core.h"

#include "AdsService.h"

#ifdef ANDROID
extern void CallFromJava(const char* function);
extern void CallFromJava(const char* function, const char* param);
#endif

#ifdef IOS
//#include "AppFlood.h"
#import "IONViewController.h"

//EXT
/*
extern IONViewController* ion_view_controller;
 */

typedef void (^AdsDelegate)(id);

static AdsDelegate ads_delegate = ^(id data)
{
    NSString *string = data;
    NSError *error = NULL;
    NSRegularExpression *regex = NULL;
    
    regex = [NSRegularExpression regularExpressionWithPattern:@"\\s*" options:NSRegularExpressionCaseInsensitive error:&error];
    string = [regex stringByReplacingMatchesInString:string options:0 range:NSMakeRange(0, [string length]) withTemplate:@""];
    
    regex = [NSRegularExpression regularExpressionWithPattern:@"ret:" options:NSRegularExpressionCaseInsensitive error:&error];
    string = [regex stringByReplacingMatchesInString:string options:0 range:NSMakeRange(0, [string length]) withTemplate:@"\"data\":"];
    
    regex = [NSRegularExpression regularExpressionWithPattern:@"\\bdesc\\b" options:NSRegularExpressionCaseInsensitive error:&error];
    string = [regex stringByReplacingMatchesInString:string options:0 range:NSMakeRange(0, [string length]) withTemplate:@"\"desc\""];
    
    regex = [NSRegularExpression regularExpressionWithPattern:@"\\bname\\b" options:NSRegularExpressionCaseInsensitive error:&error];
    string = [regex stringByReplacingMatchesInString:string options:0 range:NSMakeRange(0, [string length]) withTemplate:@"\"name\""];
    
    regex = [NSRegularExpression regularExpressionWithPattern:@"success:" options:NSRegularExpressionCaseInsensitive error:&error];
    string = [regex stringByReplacingMatchesInString:string options:0 range:NSMakeRange(0, [string length]) withTemplate:@"\"success\":"];
    
    regex = [NSRegularExpression regularExpressionWithPattern:@"\"=" options:NSRegularExpressionCaseInsensitive error:&error];
    string = [regex stringByReplacingMatchesInString:string options:0 range:NSMakeRange(0, [string length]) withTemplate:@"\":"];
    
    regex = [NSRegularExpression regularExpressionWithPattern:@":(\\w+);" options:NSRegularExpressionCaseInsensitive error:&error];
    string = [regex stringByReplacingMatchesInString:string options:0 range:NSMakeRange(0, [string length]) withTemplate:@":\"$1\";"];
    
    regex = [NSRegularExpression regularExpressionWithPattern:@"\";\"" options:NSRegularExpressionCaseInsensitive error:&error];
    string = [regex stringByReplacingMatchesInString:string options:0 range:NSMakeRange(0, [string length]) withTemplate:@"\",\""];
    
    regex = [NSRegularExpression regularExpressionWithPattern:@"\";" options:NSRegularExpressionCaseInsensitive error:&error];
    string = [regex stringByReplacingMatchesInString:string options:0 range:NSMakeRange(0, [string length]) withTemplate:@"\""];
    
    regex = [NSRegularExpression regularExpressionWithPattern:@"\\(" options:NSRegularExpressionCaseInsensitive error:&error];
    string = [regex stringByReplacingMatchesInString:string options:0 range:NSMakeRange(0, [string length]) withTemplate:@"["];
    
    regex = [NSRegularExpression regularExpressionWithPattern:@"\\)" options:NSRegularExpressionCaseInsensitive error:&error];
    string = [regex stringByReplacingMatchesInString:string options:0 range:NSMakeRange(0, [string length]) withTemplate:@"]"];
    
    IFile* file = core->Files()->OpenFile("ads_data", IFile::write);
    
	if (file)
	{
		file->Write([string UTF8String], [string length]);
		file->Release();
	}
    
    en_core.Ads()->ProcessData("ads_data");
};

#endif

#define GetAdsValue(f) \
	if (index < 0 || index >= (int)ads_data.size()) return false; \
	value = ads_data[index].f; \
	return true;

AdsService::AdsService()
{
}

bool AdsService::Init()
{
	core->AddTask(this, this, (TaskHolder::Task)&AdsService::Work, -320, -1.f);
	return true;
}

void AdsService::Reset()
{
	request.used = false;
	request.done = false;
}

void AdsService::Release()
{
}

void AdsService::Work(float dt, int level)
{
	if (request.done && request.used)
	{
		request.listener->OnRequestFinish(0, request.data);
		request.used = false;
		request.done = false;
	}
}

void AdsService::RequestAds(Listener* listener, void* reqest_data)
{
#ifdef PC
	request.used = true;
	ProcessData("Editor/ads_data");

	request.listener = listener;
	request.data = reqest_data;

	return;
#endif

	if (request.used)
	{
		core->TraceTo("Ads", "Request in progress.");
		return;
	}

	request.used = true;
	request.done = false;

	request.listener = listener;
	request.data = reqest_data;

#ifdef ANDROID
	CallFromJava("GetAdsData");
#endif
        
#ifdef IOS
    //AFRequestDelegateWrapper* delegate = [[[AFRequestDelegateWrapper alloc] initWithBlock:ads_delegate] autorelease];
    //[AppFlood getRawData:delegate];
#endif
}

void AdsService::ProcessData(const char* filename)
{
	if (!request.used)
	{
		return;
	}

	ads_data.clear();

	IStreamLoader* loader = en_core.Files()->CreateStreamLoader(filename, "", IStreamLoader::json);

	if (loader)
	{
		while (loader->EnterBlock("data"))
		{
			ads_data.push_back(AdsData());
			AdsData& d = ads_data.back();

			char buffer[4096];

			loader->Read("back_url", buffer, 4096);
			d.back_url.assign(buffer);
			loader->Read("click_url", buffer, 4096);
			d.click_url.assign(buffer);
			loader->Read("dev_name", buffer, 4096);
			d.dev_name.assign(buffer);
			loader->Read("desc", buffer, 4096);
			d.desc.assign(buffer);
			loader->Read("panel_small", buffer, 4096);
			d.panel_small.assign(buffer);
			loader->Read("panel_large", buffer, 4096);
			d.panel_large.assign(buffer);
			loader->Read("app_type", buffer, 4096);
			d.app_type.assign(buffer);
			loader->Read("name", buffer, 4096);
			d.name.assign(buffer);
			loader->Read("icon_url", buffer, 4096);
			d.icon_url.assign(buffer);
			loader->Read("banner_small", buffer, 4096);
			d.banner_small.assign(buffer);
			loader->Read("banner_middle", buffer, 4096);
			d.banner_middle.assign(buffer);
			loader->Read("banner_large", buffer, 4096);
			d.banner_large.assign(buffer);
			loader->Read("show_cb_url", buffer, 4096);
			d.show_cb_url.assign(buffer);

			loader->LeaveBlock();
		}

		loader->Release();
	}

	request.done = true;
}

void AdsService::HandleAdsClick(const char* back_url, const char* click_url)
{
#ifdef ANDROID
	CallFromJava("SetAdsBackUrl", back_url);
	CallFromJava("SetAdsClickUrl", click_url);

	CallFromJava("HandleAdsClick");
#endif
    
#ifdef IOS
    //[AppFlood handleAFClick:[NSString stringWithUTF8String:back_url] clickUrl:[NSString stringWithUTF8String:click_url]];
#endif
}

void AdsService::HandleAdsShow(const char* show_cb_url)
{
	en_core.Network()->StartRequest(&network_listener, this, "GET", show_cb_url, null, "dummy");
}

int AdsService::GetAdsCount()
{
	return ads_data.size();
}

bool AdsService::GetAdsBackUrl(int index, std::string& value)
{
	GetAdsValue(back_url);
}

bool AdsService::GetAdsClickUrl(int index, std::string& value)
{
	GetAdsValue(click_url);
}

bool AdsService::GetAdsDevName(int index, std::string& value)
{
	GetAdsValue(dev_name);
}

bool AdsService::GetAdsDesc(int index, std::string& value)
{
	GetAdsValue(desc);
}

bool AdsService::GetAdsPanelSmall(int index, std::string& value)
{
	GetAdsValue(panel_small);
}

bool AdsService::GetAdsPanelLarge(int index, std::string& value)
{
	GetAdsValue(panel_large);
}

bool AdsService::GetAdsAppType(int index, std::string& value)
{
	GetAdsValue(app_type);
}

bool AdsService::GetAdsName(int index, std::string& value)
{
	GetAdsValue(name);
}

bool AdsService::GetAdsIconUrl(int index, std::string& value)
{
	GetAdsValue(icon_url);
}

bool AdsService::GetAdsBannerSmall(int index, std::string& value)
{
	GetAdsValue(banner_small);
}

bool AdsService::GetAdsBannerMiddle(int index, std::string& value)
{
	GetAdsValue(banner_middle);
}

bool AdsService::GetAdsBannerLarge(int index, std::string& value)
{
	GetAdsValue(banner_large);
}

bool AdsService::GetAdsShowCbUrl(int index, std::string& value)
{
	GetAdsValue(show_cb_url);
}

void AdsService::ShowInterstitial()
{
#ifdef ANDROID
	CallFromJava("ShowInterstitial");
#endif
    
#ifdef IOS
    //EXT
    /*
    [ion_view_controller showInterstitial];
     */
    
#endif
}