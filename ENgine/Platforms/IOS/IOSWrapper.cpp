
#import "IOSWrapper.h"
#import "Platforms/IOS/IONViewController.h"
#import "Platforms/IOS/IOSAppDelegate.h"
#include "ScriptModulecore.h"
#include "ScriptMachine.h"
#include "ScriptModule.h"
#include "Entities/NGUI/NGUIService.h"

IOSWrapper* ios_wrapper;

IOSWrapper wrapper;

#ifndef IOSEXT

extern IONViewController* ion_view_controller;
extern IOSAppDelegate* app_delegate;

extern UITextField* tfiled;
extern float allow_first_responder;

extern string webview_url;
extern string webview_stop_url;

extern std::string ios_share_text;
extern std::string ios_share_extra_data;

extern std::string ios_share_type;
extern char push_server_url[256];

extern bool vk_login_view_logined;
extern std::string vk_login_view_token;
extern std::string vk_login_view_user_id;

#include "EventKit/EventKit.h"
EKEventStore *evnt_eventStore = null;
EKEvent *evnt_event = null;
NSError *evnt_err = null;

#import "Platforms/IOS/IOSAppDelegate.h"

InAppPurchaseManager* purchase;

extern vector<std::string> ios_event_name;
extern vector<std::string> ios_event_param;

#endif

#ifdef IOSEXT

#import "TodayViewController.h"

extern TodayViewController* tv_controller;

#endif

IOSWrapper::IOSWrapper()
{
	ios_wrapper = this;
#ifndef IOSEXT
    evnt_eventStore = [[EKEventStore alloc] init];
	
    purchase = [InAppPurchaseManager alloc];    
    [purchase loadStore];
#endif
}

void IOSWrapper::VKSignIn()
{
#ifndef IOSEXT
	//EXT        
	[ion_view_controller openVKLogin];
                
	/*const char* client_id = "3673620";
	int scope = 1+2+4+8+16+32+64+128+1024+2048+4096+8192+65536+131072+262144+524288;
	const char* redirect_url = "http://oauth.vk.com/blank.html";
    
	NSString* url = [[NSString alloc] initWithFormat:@"http://oauth.vk.com/authorize?client_id=%s&display=touch&scope=%i&redirect_uri=%s&responce_type=token", client_id, scope, redirect_url];
	[[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]];*/
#endif
}

void IOSWrapper::VKSignOut()
{
#ifndef IOSEXT
    vk_login_view_logined = false;
    
    [[NSURLCache sharedURLCache] removeAllCachedResponses];
    
    NSHTTPCookie* acookie;
    
    for (acookie in [[NSHTTPCookieStorage sharedHTTPCookieStorage] cookies])
    {
        [[NSHTTPCookieStorage sharedHTTPCookieStorage] deleteCookie:acookie];
    }
    
    NSUserDefaults* def = [NSUserDefaults standardUserDefaults];
    
    [def setObject: @"" forKey:@"vk_token"];
    [def setObject: @"" forKey:@"vk_user_id"];
    
    [def synchronize];
    
    vk_login_view_token = "";
    vk_login_view_user_id = "";
#endif
}


void IOSWrapper::FBSignIn(vector<string>& req_permissions)
{
#ifndef IOSEXT
    NSMutableArray* permissions = [[NSMutableArray alloc] init];
    
    for (int i=0;i<req_permissions.size();i++)
    {
        NSString* nsName = [[NSString alloc] initWithFormat:@"%s", req_permissions[i].c_str()];
        [permissions addObject:nsName];
    }
    
    [app_delegate LoginToFacebook:permissions];
        
    //[ion_view_controller openFBLogin];
#endif
}

void IOSWrapper::FBStartRequestPermistions(vector<string>& req_permissions)
{
#ifndef IOSEXT
	NSMutableArray* permissions = [[NSMutableArray alloc] init];
    
    for (int i=0;i<req_permissions.size();i++)
    {
        NSString* nsName = [[NSString alloc] initWithFormat:@"%s", req_permissions[i].c_str()];
        [permissions addObject:nsName];
    }
        
    [app_delegate RequestFBPermissions:permissions];
#endif
}

void IOSWrapper::FBSignOut()
{
#ifndef IOSEXT
    [app_delegate CloseFB];
#endif
}

void IOSWrapper::InstagramSignIn()
{
#ifndef IOSEXT
    [ion_view_controller checkInstagramAuth];
#endif
}

void IOSWrapper::OpenVideo(const char* path, const char* showName)
{
#ifndef IOSEXT
	string str = path;
	if (str[str.size()-1] != ';')
	{
		str += ";";
	}

	str += showName;
	[ion_view_controller openVideo:str];
#endif
}

void IOSWrapper::ShowMap(bool show)
{
#ifndef IOSEXT
	[ion_view_controller ShowMap:show];
#endif
}

void IOSWrapper::SetMapPosition(float x, float y, float w, float h)
{
#ifndef IOSEXT
	[ion_view_controller SetMapPosition:x set_y:y set_w:w set_h:h];
#endif
} 

void IOSWrapper::AddObjectToMap(float x, float y, const char* text)
{
#ifndef IOSEXT
    NSString* nsstr = [[NSString alloc] initWithUTF8String: text];    
	[ion_view_controller AddObjectToMap:x set_y:y set_text:nsstr];
#endif
}     

void IOSWrapper::DeleteAllMapObjects()
{
#ifndef IOSEXT
    [ion_view_controller DeleteAllMapObjects];
#endif
} 

void IOSWrapper::ShowPopup(const char* title, const char* text, const char* btn_text)
{
#ifndef IOSEXT
    NSString* nstitle = [[NSString alloc] initWithUTF8String: title];
    NSString* nstext = [[NSString alloc] initWithUTF8String: text];
    NSString* nsbtn_text = [[NSString alloc] initWithUTF8String: btn_text];
    
	[ion_view_controller ShowPopup:nstitle set_text:nstext set_ok_text:nsbtn_text];
#endif
}

void IOSWrapper::ShowPopup(const char* title, const char* text, const char* btn_text, const char* btn_cancel_text)
{
#ifndef IOSEXT
    NSString* nstitle = [[NSString alloc] initWithUTF8String: title];
    NSString* nstext = [[NSString alloc] initWithUTF8String: text];
    NSString* nsbtn_ok_text = [[NSString alloc] initWithUTF8String: btn_text];
    NSString* nsbtn_cancel_text = [[NSString alloc] initWithUTF8String: btn_cancel_text];
        
    [ion_view_controller ShowPopup:nstitle set_text:nstext set_ok_text:nsbtn_ok_text set_cancel_text:nsbtn_cancel_text];
#endif
}

void IOSWrapper::DismissModalViewControllerAnimated()
{
#ifndef IOSEXT
    [ion_view_controller dismissModalViewControllerAnimated:YES];
#endif
}     

void IOSWrapper::CallNumber(const char* number)
{
#ifndef IOSEXT
	char str[64];
    String::Printf(str, 64, "telprompt://%s", number);
    NSString* nsstr = [[NSString alloc] initWithUTF8String: str];
	NSURL *url = [NSURL URLWithString: nsstr];
    [[UIApplication sharedApplication] openURL:url];
    nsstr.release;
#endif
}

void IOSWrapper::StartSelectImage(int type)
{
#ifndef IOSEXT
    [ion_view_controller StartSelectImage:type];
#endif
}

void IOSWrapper::ShowSoftKeyboard(IControlsService::KeyboardType kb_type, IControlsService::KeyboardReturnKey kb_ret_key)
{
#ifndef IOSEXT
    UIKeyboardType ui_kb_type = UIKeyboardTypeDefault;
        
    if (kb_type == IControlsService::kb_URL) ui_kb_type = UIKeyboardTypeURL;
    if (kb_type == IControlsService::kb_Number) ui_kb_type = UIKeyboardTypeNumberPad;
    if (kb_type == IControlsService::kb_Phone) ui_kb_type = UIKeyboardTypePhonePad;
    if (kb_type == IControlsService::kb_Email) ui_kb_type = UIKeyboardTypeEmailAddress;
        
    [tfiled setKeyboardType:ui_kb_type];
        
    UIReturnKeyType kb_ret_type = UIReturnKeyDefault;
        
    if (kb_ret_key == IControlsService::rk_Next) kb_ret_type = UIReturnKeyNext;
    if (kb_ret_key == IControlsService::rk_Done) kb_ret_type = UIReturnKeyDone;
    if (kb_ret_key == IControlsService::rk_Go) kb_ret_type = UIReturnKeyGo;
    if (kb_ret_key == IControlsService::rk_Search) kb_ret_type = UIReturnKeySearch;
        
    [tfiled setReturnKeyType:kb_ret_type];
        
    [tfiled becomeFirstResponder];
    
    if (allow_first_responder < -2.0f)
    {
        allow_first_responder = -0.5f;
    }
#endif
}

void IOSWrapper::HideSoftKeyboard()
{
#ifndef IOSEXT
    [tfiled resignFirstResponder];
#endif
}

void IOSWrapper::OpenURL(const char* url, bool inner)
{
    core->TraceTo("App", url);
    
#ifdef IOSEXT
    NSString* str_url = [[NSString alloc] initWithFormat:@"%s", url];
    NSURL* nurl = [NSURL URLWithString:str_url];
    [tv_controller.extensionContext openURL:nurl completionHandler:nil];
#endif

#ifndef IOSEXT
    
	if (inner)
	{        
		webview_url = url;
        webview_stop_url = "";
        [ion_view_controller openURL];
	}
	else
	{
		NSString* str_url = [[NSString alloc] initWithFormat:@"%s", url];
		[[UIApplication sharedApplication] openURL:[NSURL URLWithString:str_url]];
	}
#endif
}

void IOSWrapper::OpenURL(const char* url, const char* stop_url)
{
#ifndef IOSEXT
    webview_url = url;
    webview_stop_url = stop_url;
    [ion_view_controller openURL];
#endif
}

void IOSWrapper::StartMonitoringRegion(const char* id, const char* uuid)
{
#ifdef IOS
    
#ifndef IOSEXT
    NSString* ns_uuid = [[NSString alloc] initWithFormat:@"%s", uuid];
    NSString* ns_id = [[NSString alloc] initWithFormat:@"%s", id];
    
    [ion_view_controller StartMonitoringRegion:ns_id uuid:ns_uuid];
#endif
    
#endif
}

void IOSWrapper::StopMonitoringRegion(const char* id)
{
#ifndef IOSEXT
    [ion_view_controller StopMonitoringRegion:id];
#endif
}

void IOSWrapper::SetCallbackOnEnterRegion(const char* id, const char* func)
{
#ifndef IOSEXT
    [ion_view_controller SetCallbackOnEnterRegion:id callback:func];
#endif
}

void IOSWrapper::SetCallbackOnExitRegion(const char* id, const char* func)
{
#ifndef IOSEXT
    [ion_view_controller SetCallbackOnExitRegion:id callback:func];
#endif
}

int IOSWrapper::GetRegionBeaconsCount(const char* id)
{
#ifndef IOSEXT
    return [ion_view_controller GetRegionBeaconsCount:id];
#endif
    
    return 0;
}

int IOSWrapper::GetRegionBeaconMinor(const char* id, int index)
{
#ifndef IOSEXT
    return [ion_view_controller GetRegionBeaconMinor:id index:index];
#endif
    
    return -1;
}

int IOSWrapper::GetRegionBeaconMajor(const char* id, int index)
{
#ifndef IOSEXT
    return [ion_view_controller GetRegionBeaconMajor:id index:index];
#endif
    
    return -1;
}

int IOSWrapper::GetRegionBeaconProximity(const char* id, int index)
{
#ifndef IOSEXT
    return [ion_view_controller GetRegionBeaconProximity:id index:index];
#endif
    
    return -1;
}

float IOSWrapper::GetRegionBeaconAccuracy(const char* id, int index)
{
#ifndef IOSEXT
    return [ion_view_controller GetRegionBeaconAccuracy:id index:index];
#endif
    
    return 0.0f;
}

void IOSWrapper::SendMail(const char* address, const char* subject, const char* body)
{	
#ifdef ANDROID
	std::string mail_data = address + "|<" + subject + "|<" + body;
	CallFromJava("SendMail", mail_data.c_str());
#endif

#ifdef IOS
    
#ifndef IOSEXT
	NSString* mailString = [[NSString alloc] initWithFormat:@"mailto:?to=%s&subject=%s&body=%s", address, subject, body];
	[[UIApplication sharedApplication] openURL:[NSURL URLWithString:mailString]];
#endif
    
#endif
}

void IOSWrapper::StartShareText(const char* text, const char* extra_data)
{
#ifndef IOSEXT
    ios_share_text = text;
    ios_share_extra_data = extra_data;
    
    NSString* ns_text = [NSString stringWithCString:text encoding:NSUTF8StringEncoding];
    [ion_view_controller StartShareText:ns_text];
#endif
}
    
void IOSWrapper::StartShareImage(const char* image)
{
#ifndef IOSEXT
    NSString* ns_text = [NSString stringWithCString:image encoding:NSUTF8StringEncoding];
    [ion_view_controller StartShareImage:ns_text];
#endif
}

float IOSWrapper::GetBatteryLevel()
{
    [[UIDevice currentDevice] setBatteryMonitoringEnabled:YES];
    
    return [[UIDevice currentDevice] batteryLevel] * 100.0f;
}

int IOSWrapper::GetBatteryStatus()
{
    [[UIDevice currentDevice] setBatteryMonitoringEnabled:YES];
    
    return [[UIDevice currentDevice] batteryState];
}

void IOSWrapper::SetPushServerURL(const char* url)
{
#ifndef IOSEXT
    strcpy(push_server_url, url);
#endif
}

void IOSWrapper::StartRegisterInPush()
{
#ifndef IOSEXT
    [app_delegate StartRegisterInPush];
#endif
} 

int IOSWrapper::GetTittleBarHeight()
{
    float hgt = 0.0f;
#ifndef IOSEXT
    hgt = [ion_view_controller GetTitleBarHeight];
#endif
    hgt = hgt * NGUIService::instance->v_mul_ScreenToRef;
    
    return hgt;
} 

void IOSWrapper::AddIOSEvent()
{
#ifndef IOSEXT
    evnt_event  = [EKEvent eventWithEventStore:evnt_eventStore];
    
    NSString* titleString = [NSString stringWithCString:evnt_name.c_str() encoding:NSUTF8StringEncoding];
    evnt_event.title = titleString;
    
    //NSString* descString = [[NSString alloc] initWithFormat:@"%s", evnt_desc.c_str()];
    //evnt_event.desc = descString;
    
    NSString* locString = [NSString stringWithCString:evnt_location.c_str() encoding:NSUTF8StringEncoding];
    evnt_event.location = locString;
    
    NSCalendar *calendar = [NSCalendar currentCalendar];
    NSDateComponents *components = [[NSDateComponents alloc] init];
    
    NSNumber *day = [NSNumber numberWithInt:evnt_day];
    NSNumber *month = [NSNumber numberWithInt:evnt_month];
    NSNumber *year = [NSNumber numberWithInt:evnt_year];
    NSNumber *hour = [NSNumber numberWithInt:evnt_hour];
    NSNumber *minute = [NSNumber numberWithInt:evnt_minute];
    NSNumber *second = [NSNumber numberWithInt:evnt_second];
    
    [components setDay:[day intValue]];
    [components setMonth:[month intValue]];
    [components setYear:[year intValue]];
    [components setHour:[hour intValue]];
    [components setMinute:[minute intValue]];
    [components setSecond:[second intValue]];
    
    NSDate *date = [calendar dateFromComponents:components];
    
    evnt_event.startDate = date;
    evnt_event.endDate   = [[NSDate alloc] initWithTimeInterval:600 sinceDate:evnt_event.startDate];
    
    [evnt_event setCalendar:[evnt_eventStore defaultCalendarForNewEvents]];
    [evnt_eventStore saveEvent:evnt_event span:EKSpanThisEvent error:&evnt_err];
#endif
}

void IOSWrapper::AddEventToCalendar(int day, int month, int year, int hour, int minute, int second,
									const char* name, const char* desc, const char* location)
{
#ifndef IOSEXT
	evnt_day = day;
    evnt_month = month;
    evnt_year = year;
    evnt_hour = hour;
    evnt_minute = minute;
    evnt_second = second;
    evnt_name = name;
    evnt_desc = desc;
    evnt_location = location;
    
    if([evnt_eventStore respondsToSelector:@selector(requestAccessToEntityType:completion:)])
    {
        [evnt_eventStore requestAccessToEntityType:EKEntityTypeEvent completion:^(BOOL granted, NSError *error)
         {
            if (granted)
            {
                AddIOSEvent();
            }
            else
            {
                
            }
         }];
    }
    else
    {
        AddIOSEvent();
    }
#endif
}

void IOSWrapper::StartPurchase(const char* id)
{
#ifndef IOSEXT
	NSString* nsName = [[NSString alloc] initWithFormat:@"%s", id];    
    [purchase purchase:nsName];
#endif
}
    
void IOSWrapper::StartRestorePurchase()
{
#ifndef IOSEXT
	[purchase RestorePurchase];
#endif
}

void IOSWrapper::TrackUserAge(int age)
{
#ifndef IOSEXT
    [app_delegate TrackUserAge : age];
#endif
}

void IOSWrapper::TrackUserGender(const char* gender)
{
#ifndef IOSEXT
    NSString* nsName = [[NSString alloc] initWithFormat:@"%s", gender];
    [app_delegate TrackUserGender : nsName];
#endif
} 

void IOSWrapper::AddEventParam(const char* name, const char* param)
{
#ifndef IOSEXT
    ios_event_name.push_back(name);
    ios_event_param.push_back(param);
#endif
}

void IOSWrapper::TrackEvent(const char* name)
{
#ifndef IOSEXT
	NSString* nsName = [[NSString alloc] initWithFormat:@"%s", name];    
    [app_delegate TrackEvent : nsName];
#endif
}

void IOSWrapper::EnableTrackGPS(bool enable)
{
#ifndef IOSEXT
    [ion_view_controller EnableTrackGPS : enable];
#endif
}

void IOSWrapper::PrepareCameraFeed()
{
#ifndef IOSEXT
    [ion_view_controller prepareCameraFeed];
#endif
}