#include "Core/Core.h"
#include "MobileService.h"
#include <algorithm>
#include "Common/libxml/tinyxml2.h"

#ifdef ANDROID
extern void CallFromJava(const char* function);
extern void CallFromJava(const char* function, const char* param);
#endif

extern float android_extern_batterylevel;
extern int android_extern_batterystatus;

int jhjkjkhk = 0;

#ifdef IOS

#import <AddressBook/AddressBook.h>
#import <CoreTelephony/CTCarrier.h>
#import <CoreTelephony/CTTelephonyNetworkInfo.h>

extern int sv_camera_width;
extern int sv_camera_height;
extern byte* sv_buffer;
extern bool sv_ready;


void SaveBytes(unsigned char* bytes, int size, const char* path);

#include "Platforms/IOS/IOSWrapper.h"

static MFMessageComposeViewController *g_controller = null;
static IOSMessage *g_message = null;

@implementation IOSMessage
- (void) messageComposeViewController:(MFMessageComposeViewController *)controller didFinishWithResult:(MessageComposeResult)result;
{
    switch(result)
    {
        case MessageComposeResultCancelled:
            en_core.Mobile()->SetSendSmsResult(-1);
            break; //handle cancelled event
        case MessageComposeResultFailed:
            en_core.Mobile()->SetSendSmsResult(-1);

            break; //handle failed event
        case MessageComposeResultSent:
            en_core.Mobile()->SetSendSmsResult(0);
            break; //handle sent event
    }
    
	ios_wrapper->DismissModalViewControllerAnimated();     
    
    g_controller.release;
    g_message.release;
}



- (IBAction) sendSMS:(const char*)number text:(const char *)text
{
    MFMessageComposeViewController *g_controller = [[MFMessageComposeViewController alloc] init];
    if([MFMessageComposeViewController canSendText])
    {
        g_controller.body = [NSString stringWithUTF8String: text];
        g_controller.recipients = [NSArray arrayWithObjects: [NSString stringWithUTF8String: number], nil];
        g_controller.messageComposeDelegate = self;
        
        ios_wrapper->DismissModalViewControllerAnimated();     
    }
    //[controller autorelease];
}@end
#endif

class functor_char  
{
  public:
    functor_char()
	{}

    bool operator()(const std::string &s1, const std::string &s2) const
	{		
		return String::CompareAlphapedOrder(s1.c_str(), s2.c_str());
    }
};

void MobileService::ReqestWorker::Prepare()
{
}

void MobileService::ReqestWorker::Loop()
{
	owner->ReadAvatars();
}

MobileService::MobileService() : state(Idle), state_read_contacts(Idle)
{
	scanning_time = -1;

	worker.owner = this;
	call_sms_callback_time = -1;

	reqest_access_callback = null;
	reqest_access_data = 0;

	read_contacts_callback = null;
	read_contacts_data = 0;

	read_contacts_avatars_callback = null;	
	read_contacts_avatars_data = 0;

	send_sms_callback = null;
	send_sms_data = 0;

	select_image_callback = null;
	select_image_data = 0;

	smsResultReady = false;
	smsResult = false;

	imageResult = -1;
	imageReady = false;

	show_popup = 0;
	popup_listiner = null;
	popup_listiner_data = 0;

	share_listiner = null;
	share_listiner_data = 0;

	gps_listiner = null;
	gps_data = null;

	gps_ready = false;
	gps_state = 0;
	gps_latitude = 0.0f;
	gps_longitude = 0.0f;

	set_allow_call = true;
#ifdef IOS
    access_request_finished = true;
#endif
    
	show_map = false;
	popup_result = 0;

#ifdef PC
	read_contacts_allowed = false;
	map_tex = null;	
	white_tex = null;
	map_pos = 0.0f;
	map_size = 0.0f;
#endif

	scan_listener = null;
	scan_data = null;

	is_paused = false;
	pause_trigered = false;	
	
	onpause_listiner = null;
	onpause_listiner_data = 0;

	onresume_listiner = null;
	onresume_listiner_data = 0;

	need_process_scheduled_data = false;

    clipboard_listener = null;

	cam_texture = null;
}

bool MobileService::Init()
{
	core->AddTask(this, this, (TaskHolder::Task)&MobileService::Work, lvl_camera1, -1.f);
    
#ifdef IOS
    
    CTTelephonyNetworkInfo *netinfo = [[CTTelephonyNetworkInfo alloc] init];
    CTCarrier *carrier = [netinfo subscriberCellularProvider];
    
    if ([carrier carrierName])
    {
        NSLog(@"simOperatorName: %@", [carrier carrierName]);
        String::Copy(simOperatorName.data, 64, [carrier carrierName].UTF8String);
    }
    
    [netinfo release];
    
	addressBook = ABAddressBookCreate();        
#endif

#ifdef PC
	//map_tex = en_core.Render()->LoadTexture("Editor/map.jpg", false, _FL_);
	white_tex = en_core.Render()->LoadTexture("Editor/white.png", false, _FL_);
#endif
    clipboard_listener = null;

	return true;
}

void MobileService::Work(float dt, int level)
{
#ifdef IOS
    if (sv_ready && cam_texture)
    {
        glBindTexture(GL_TEXTURE_2D, *(GLuint*)cam_texture->GetData());
        //these er, have to be set
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // This is necessary for non-power-of-two textures
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        //set the image for the currently bound texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sv_camera_width, sv_camera_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, sv_buffer);
        
        sv_ready = false;
    }
#endif
    
#ifdef PC
	if (call_sms_callback_time >= 0)
	{
		call_sms_callback_time += dt;
		
		if (call_sms_callback_time > 1)
		{
			call_sms_callback_time = -1;
			SetSendSmsResult(0);
		}
	}

	if (reqest_access_callback)
	{
		read_contacts_allowed = true;
		reqest_access_callback->OnRequestFinish(reqest_access_data, 0);
		reqest_access_callback = null;
		reqest_access_data = 0;
	}	

	if (show_map)
	{
		en_core.Render()->Debug()->DrawSprite(map_tex, map_pos, map_size, up_left, COLOR_WHITE, 0.0f);

		for (int i=0; i<mapObjects.size();i++)
		{
			en_core.Render()->Debug()->PrintText(map_pos.x + 0, map_pos.y + 30 * i, up_left, COLOR_BLUE, "%4.6f %4.6f %s", mapObjects[i].x, mapObjects[i].y, mapObjects[i].desc.c_str());
		}
	}

	if (show_popup)
	{
		float ref_height = en_core.Render()->Display()->GetReferenceHeight();
			
		float cur_rwidth = en_core.Render()->Display()->GetWidth();
		float cur_rheight = en_core.Render()->Display()->GetHeight();		
		
		float aspect = 1.0f / ((float)cur_rheight / (float)cur_rwidth);	

		float h_mul_RefToScreen = 1.0f /ref_height * (float)cur_rwidth / aspect;
		float v_mul_RefToScreen = 1.0f / ref_height * (float)cur_rheight;
		
		int scr_w = 300 * h_mul_RefToScreen; 
		int scr_h = 200 * v_mul_RefToScreen;		
		int scr_h2 = 40 * v_mul_RefToScreen;		

		en_core.Render()->Debug()->DrawSprite(white_tex, 0.0f, Vector2(cur_rwidth, cur_rheight), up_left, Color(0.0f, 0.0f, 0.0f, 0.75f), 0.0f);			

		Vector2 corner((cur_rwidth - scr_w) * 0.5f, (cur_rheight - scr_h) * 0.5f);

		en_core.Render()->Debug()->DrawSprite(white_tex, corner, Vector2(scr_w, scr_h), up_left, COLOR_WHITE, 0.0f);			
		en_core.Render()->Debug()->PrintText(corner.x, corner.y, up_left, COLOR_BLUE, popup_title.c_str());
		en_core.Render()->Debug()->PrintText(corner.x, corner.y + 30, up_left, COLOR_BLUE, popup_text.c_str());

		if (show_popup == 1)
		{
			en_core.Render()->Debug()->PrintText(corner.x, corner.y + scr_h - scr_h2, up_left, COLOR_WHITE, popup_ok.c_str());
			en_core.Render()->Debug()->DrawSprite(white_tex, corner + Vector2(0.0f, scr_h - scr_h2), Vector2(scr_w, scr_h2), up_left, COLOR_BLUE, 0.0f);			
		}
		else
		if (show_popup == 2)
		{
			en_core.Render()->Debug()->PrintText(corner.x, corner.y + scr_h - scr_h2, up_left, COLOR_WHITE, popup_ok.c_str());
			en_core.Render()->Debug()->DrawSprite(white_tex, corner + Vector2(0.0f, scr_h - scr_h2), Vector2(scr_w * 0.5f, scr_h2), up_left, COLOR_BLUE, 0.0f);			

			en_core.Render()->Debug()->PrintText(corner.x + scr_w * 0.5f, corner.y + scr_h - scr_h2, up_left, COLOR_WHITE, popup_cancel.c_str());
			en_core.Render()->Debug()->DrawSprite(white_tex, corner + Vector2(scr_w * 0.5f, scr_h - scr_h2), Vector2(scr_w * 0.5f, scr_h2), up_left, COLOR_GREEN, 0.0f);			
		}


		if (core->Controls()->CheckTouch(0, ControlsService::ActivatedExclusive, this))
		{
			float ms_pos_x = core->Controls()->GetTouchX(0);
			float ms_pos_y = core->Controls()->GetTouchY(0);		

			if (show_popup == 1)
			{
				if (corner.x < ms_pos_x && ms_pos_x < corner.x + scr_w &&
					corner.y + scr_h - scr_h2 < ms_pos_y && ms_pos_y < corner.y + scr_h)
				{
					popup_result = 1;												
				}
			}
			else
			{
				if (corner.x < ms_pos_x && ms_pos_x < corner.x + scr_w * 0.5f &&
					corner.y + scr_h - scr_h2 < ms_pos_y && ms_pos_y < corner.y + scr_h)
				{
					popup_result = 1;
				}
				else
				if (corner.x < ms_pos_x + scr_w * 0.5f && ms_pos_x < corner.x + scr_w &&
					corner.y + scr_h - scr_h2 < ms_pos_y && ms_pos_y < corner.y + scr_h)
				{
					popup_result = 2;					
				}
			}
		}
	}
#endif

	if (popup_result)
	{
		PopupListiner* tmp_popup_listiner = popup_listiner;

		show_popup = 0;
		popup_listiner = null;

		if (popup_result == 1)
		{
			tmp_popup_listiner->OnOkPressed(popup_listiner_data);				
		}
		else
		if (popup_result == 2)
		{
			tmp_popup_listiner->OnCancelPressed(popup_listiner_data);					
		}

		popup_result = 0;		
	}	

	ExecutePauseCallbacks();

	if (worker.IsWorking()) return;	
    
#ifdef IOS
    
    if (access_request_finished && reqest_access_callback)
    {
        reqest_access_callback->OnRequestFinish(reqest_access_data, 0);
        reqest_access_callback = null;
		reqest_access_data = 0;
    }
    
#endif
    
	if (read_contacts_callback && (state_read_contacts == Done))
	{		
		state_read_contacts = Idle;
		read_contacts_callback->OnRequestFinish(read_contacts_data, 0);
		read_contacts_callback = null;		
		read_contacts_data = 0;
	}	

	if (read_contacts_avatars_callback && (state == Done))
	{				
		state = Idle;
		read_contacts_avatars_callback->OnRequestFinish(read_contacts_avatars_data, 0);
		read_contacts_avatars_callback = null;
		read_contacts_avatars_data = 0;
	}

	if (smsResultReady)
	{
		smsResultReady = false;
		send_sms_callback->OnRequestFinish(send_sms_data, smsResult);
		send_sms_callback = null;
		send_sms_data = 0;
	}

	if (imageReady)
	{
        jhjkjkhk++;
        
        if (jhjkjkhk > 10)
        {
            imageReady = false;
            select_image_callback->OnRequestFinish(select_image_data, imageResult, image_name.c_str());
            select_image_callback = null;
			select_image_data = 0;
            imageResult = -1;
            
            jhjkjkhk = 0;
        }
	}

	if (share_listiner_res[0] && share_listiner)
	{
		share_listiner->OnEvent(share_listiner_data, share_listiner_res.c_str());

		share_listiner = null;
		share_listiner_data = null;
		share_listiner_res.clear();
	}	

	if (need_process_scheduled_data)
	{
		need_process_scheduled_data = false;

		ProcessData(scheduled_data.c_str());
	}		

#ifdef PC
	SetGPSData(0, 55.7616401f, 37.6193364f);
#endif

	if (gps_ready)
	{
		if (gps_listiner)
		{
			gps_listiner->OnUpdate(gps_data, gps_state, gps_latitude, gps_longitude);
		}		

		gps_ready = false;
	}
}

void MobileService::AddPushListener(PushListener* listener, void* data)
{
	listeners.push_back(ListenerDesc());
	ListenerDesc& desc = listeners.back();

	desc.listener = listener;
	desc.data = data;
}

void MobileService::DelPushListener(PushListener* listener)
{
	for (int i = 0, size = listeners.size(); i < size; ++i)
	{
		ListenerDesc& desc = listeners[i];
		if (desc.listener == listener)
		{
			listeners.erase(listeners.begin() + i);
			break;
		}
	}
}

void MobileService::ScheduleData(const char* userdata)
{
	need_process_scheduled_data = true;
	scheduled_data.assign(userdata);
}

void MobileService::ProcessData(const char* userdata)
{
	for (int i = 0, size = listeners.size(); i < size; ++i)
	{
		ListenerDesc& desc = listeners[i];

		desc.listener->OnGetUserData(desc.data, userdata);
	}
}

bool MobileService::IsReadContactsAllowed()
{
#ifdef IOS
    if (ABAddressBookGetAuthorizationStatus() != kABAuthorizationStatusAuthorized)
    {
        return false;
    }
#endif

#ifdef PC
	return read_contacts_allowed;
#endif

	return true;
}
	
void MobileService::RequestAccessToContacts(RequestListiner* request, void* data)
{
#ifdef IOS
	
    access_request_finished = false;
    
    ABAddressBookRequestAccessWithCompletion(addressBook, ^(bool granted, CFErrorRef error)
    {
        access_request_finished = true;
        core->TraceTo("app", "access to adress book completed");
    });
    
#endif
    
    reqest_access_callback = request;
	reqest_access_data = data;
}

void MobileService::StartReadContacts(RequestListiner* request, void* data)
{	
	contacts.resize(0);

	read_contacts_callback = request;
	read_contacts_data = data;

#ifdef PC

	contacts.resize(8 * 5);
	int j = 0;
	char name[128];
	for (int i = 0; i < 8 * 5; i++ )
	{
		switch(j)
		{
		case 0:
			String::Printf(name, 128, "a Fake Contact %i", i);
			SetContactData(i, name, "MAIN=89260789861;MOBILE=+79260789862");			
			break;
		case 1:
			String::Printf(name, 128, "b Fake Contact %i", i);
			SetContactData(i, name, "MOBILE=89260789861");
			break;
		case 2:
			String::Printf(name, 128, "c Fake Contact %i", i);
			SetContactData(i, name, "HOME=89260789861;WORK=89260789862;MAIN=89260789863");			
			break;
		case 3:
			String::Printf(name, 128, "d Fake Contact %i", i);
			SetContactData(i, name, "MOBILE=9260789864;MOBILE=89260789865;MOBILE=89260789866");			
			break;
		case 4:
			String::Printf(name, 128, "e Fake Contact %i", i);
			SetContactData(i, name, "HOME=89260789864;MOBILE=89260789865;WORK=89260789866;OTHER=89260789867");						
			break;
		case 5:
			String::Printf(name, 128, "f Fake Contact %i", i);
			SetContactData(i, name, "HOME=89260789864;MOBILE=89260789865;WORK=89260789866;OTHER=89260789867;OTHER=89260789869;MAIN=8926078986;MAIN=8926078987");									
			break;
		case 6:
			String::Printf(name, 128, "g Fake Contact %i", i);
			SetContactData(i, name, "HOME=89260789864;MOBILE=89260789865;WORK=89260789866;OTHER=89260789869");									
			break;
		case 7:
			String::Printf(name, 128, "k Fake Contact %i", i);
			SetContactData(i, name, "HOMEHOME=89260789864;MOBILE=89260789865;WORK=89260789866;OTHER=89260789869");									
			break;
		}
		
		j--;
		
		if (j < 0)
		{		
			j = 7;
		}
	}

	FinalizeReadContatcs();
#endif

#ifdef ANDROID
	CallFromJava( "ReadContacts");
	state_read_contacts = Reading;
#endif

#ifdef IOS    

    //ABRecordRef source = ABAddressBookCopyDefaultSource(addressBook);
    CFArrayRef allPeople = ABAddressBookCopyArrayOfAllPeople(addressBook);
    
    //CFArrayRef allPeople = ABAddressBookCopyArrayOfAllPeopleInSourceWithSortOrdering(addressBook, source, kABPersonSortByLastName);
    
    //CFIndex numberOfPeople = CFArrayGetCount(allPeople);
    
    CFIndex numberOfPeople = ABAddressBookGetPersonCount(addressBook);
    
    SetContactsCount(numberOfPeople);
    
    NSCharacterSet* notAllowedChars = [[NSCharacterSet characterSetWithCharactersInString:@"+0123456789"] invertedSet];
    
    int index = 0;
    
    for(int i = 0; i < numberOfPeople; i++)
    {
        ABRecordRef person = CFArrayGetValueAtIndex( allPeople, i );
        
        if (person == null)
        {
            contacts.pop_back();
            continue;
        }
        
        //CFTypeRef ref = ABRecordCopyValue(person, kABPersonFirstNameProperty);
        //if (ref == null)
           // continue;
        NSString *firstName = null;
        NSString *lastName = null;
        if (ABRecordCopyValue(person, kABPersonFirstNameProperty ) != null)
        {
            firstName = (__bridge NSString *)(ABRecordCopyValue(person, kABPersonFirstNameProperty));
        }
        
        if (ABRecordCopyValue(person, kABPersonLastNameProperty ) != null)
        {
            lastName = (__bridge NSString *)(ABRecordCopyValue(person, kABPersonLastNameProperty));
        }
        
        //NSLog(@"Name:%@ %@", firstName, lastName);
        
        ABMultiValueRef phoneNumbers = ABRecordCopyValue(person, kABPersonPhoneProperty);

        if (ABMultiValueGetCount(phoneNumbers) == 0)
        {
            contacts.pop_back();
            continue;
        }
        
        std::string numbers;
        for (CFIndex i = 0; i < ABMultiValueGetCount(phoneNumbers); i++)
        {
            NSString *phoneNumber = (__bridge_transfer NSString *)ABMultiValueCopyValueAtIndex(phoneNumbers, i);
           
            NSString *mobileLabel = (NSString*)ABMultiValueCopyLabelAtIndex(phoneNumbers, i);
            //NSLog(@"phone:%@ label %@", phoneNumber, mobileLabel);
            
            if ([mobileLabel isEqualToString:(NSString *)kABWorkLabel])
            {
                numbers += "WORK=";
            }
            else
            if ([mobileLabel isEqualToString:(NSString *)kABHomeLabel])
            {
                numbers += "HOME=";
            }
            else
            if ([mobileLabel isEqualToString:(NSString *)kABPersonPhoneMobileLabel])
            {
	            numbers += "MOBILE=";
            }
            else
            if ([mobileLabel isEqualToString:(NSString *)kABPersonPhoneMainLabel])
            {
	            numbers += "MAIN=";
            }
            else
            {
                numbers += "OTHER=";
            }
            
            NSString* phn_number = [[phoneNumber componentsSeparatedByCharactersInSet:notAllowedChars] componentsJoinedByString:@""];
            
            const char* phn_number_ptr = phn_number.UTF8String;
            char phn_number_tmp[32];
            int index = 0;
            
            int ln = strlen(phn_number_ptr);
            
            for (int j=0;j<ln;j++)
            {
                if (phn_number_ptr[j] == ' ' ||
                    phn_number_ptr[j] == '-' ||
                    phn_number_ptr[j] == '(' ||
                    phn_number_ptr[j] == ')')
                {
                    continue;
                }
                
                phn_number_tmp[index] = phn_number_ptr[j];
                
                index++;
            }
            
            phn_number_tmp[index] = 0;
            
            numbers += phn_number_tmp;
            
            if(i != ABMultiValueGetCount(phoneNumbers)-1)
            {
                numbers += ";";
            }
        }
    
        Str256 name;
        if (firstName != null)
        {
            String::Copy(name.data, 256, firstName.UTF8String);
        }
        
        String::Cat(name.data, 256, " ");
        
        if (lastName != null)
        {
            String::Cat(name.data, 256, lastName.UTF8String);
        }
        
        SetContactData(index, name.data, numbers.c_str());
        index++;
    }   

	FinalizeReadContatcs();
#endif
}

void MobileService::StartReadContactsAvatars(RequestListiner* request, void* data)
{	
	if (read_contacts_avatars_callback)
	{
		core->TraceTo("MobileService", "Error: previous call not ended");
		return;
	}

	read_contacts_avatars_callback = request;
	read_contacts_avatars_data = data;
	
#ifdef PC		
	for (int i = 0; i < contacts.size(); i+=7 )
	{
		contacts[i].texture = "MediaLibrary/Images/2_thumbnail.jpg";
	}

	state = Done;
#endif

#ifdef ANDROID
	CallFromJava( "ReadAvatars");
	state = Reading;
#endif

#ifdef IOS	
	worker.Start(ThreadWorker::normal);    
	state = Reading;
#endif	
}

int MobileService::GetContactsCount()
{
	return contacts.size();
}

void MobileService::GetContactData(int index, std::string& name, std::string& texture, std::string& numbers)
{
	if (index < contacts.size())
	{
		name = contacts[index].name;
		texture  = contacts[index].texture; 
		numbers  = contacts[index].out_number;       
	}
}

void MobileService::SetContactsCount(int num)
{
    if (num >= 0)
	{
        contacts.resize( num );
	}	
}

void MobileService::SetContactData(int index, const char* name, const char* numbers)
{
	contacts[index].name = name;	

	string number = numbers;

	std::vector<NumberEntry>& nmbrs = contacts[index].numbers;

	NumberEntry entry;

	if (number.size() > 0)
	{
		int from = 0;
		int pos = number.find( ";", from);

		if (pos < 0 && number.length()>0)
		{
			int eqPos = number.find( "=", from);
			entry.number = number.substr(eqPos+1, number.length()-eqPos-1);				
			entry.type = number.substr(from, eqPos - from);																				

			nmbrs.push_back(entry);
		}

		while ( pos >= 0) 
		{
			int eqPos = number.find( "=", from);
			entry.number = number.substr(eqPos+1, pos-eqPos-1);				
			entry.type = number.substr(from, eqPos - from);												
							
			nmbrs.push_back(entry);

			from = pos+1;
			pos = number.find( ";", from);
		
			if (pos == -1)
			{
				int eqPos = number.find( "=", from);
				entry.number = number.substr(eqPos+1, number.length()-eqPos-1);
				entry.type = number.substr(from, eqPos - from);										
										
				nmbrs.push_back(entry);
			}
		}
	}	
}

void MobileService::SetContactAvatar(const char* name, const char* texture)
{
	for (int i = 0; i < contacts.size(); i++)
	{
		if (String::IsEqual(contacts[i].name.c_str(), name))
		{
			contacts[i].texture = texture;
			return;
		}
	}
}

void  MobileService::Release()
{
	Reset();	
}

void MobileService::ReadAvatars()
{	    
#ifdef IOS
    CFArrayRef allPeople = ABAddressBookCopyArrayOfAllPeople(addressBook);
    
    CFIndex numberOfPeople = ABAddressBookGetPersonCount(addressBook);
    
    Str256 last_name;
    String::Copy(last_name.data, 256, "Hope!23RandomLiv@3#");
    
    for(int i = 0; i < numberOfPeople; i++)
    {
        ABRecordRef person = CFArrayGetValueAtIndex( allPeople, i );
        
        if (person == null)
        {
            continue;
        }
        
        NSString *firstName = null;
        NSString *lastName = null;
        
        if (ABRecordCopyValue(person, kABPersonFirstNameProperty ) != null)
        {
            firstName = (__bridge NSString *)(ABRecordCopyValue(person, kABPersonFirstNameProperty));
        }
        
        if (ABRecordCopyValue(person, kABPersonLastNameProperty ) != null)
        {
            lastName = (__bridge NSString *)(ABRecordCopyValue(person, kABPersonLastNameProperty));
        }
        
        Str256 name;
        
        if (firstName != null)
        {
            String::Copy(name.data, 256, firstName.UTF8String);
        }
        
        String::Cat(name.data, 256, " ");
        
        if (lastName != null)
        {
            String::Cat(name.data, 256, lastName.UTF8String);
        }
        
        if (String::IsEqual(name.data, last_name.data))
        {
            continue;
        }
        
        String::Cat(last_name.data, 256, name.data);
        
        char path[64];
        path[0] = 0;
        
        if (ABPersonHasImageData(person))
        {
            String::Printf(path, 64, "contact_%d.png", i);
            NSData* nsdata = (NSData *)ABPersonCopyImageDataWithFormat(person, kABPersonImageFormatThumbnail);
            UIImage* image = [UIImage imageWithData: nsdata];
            NSData* data = UIImagePNGRepresentation(image);
            
            SaveBytes( (Byte*)data.bytes, data.length, path);
            nsdata.release;
        }
        else
        {
            continue;
        }
        
        SetContactAvatar(name.data, path);
    }
    
#endif

    state = Done;
}

void MobileService::FinalizeReadContatcs()
{	
	vector<string> names;

	for ( int i = 0; i < contacts.size(); ++i)
	{		
		names.push_back( contacts[i].name );
	}

	sort(names.begin(), names.end(), functor_char());

	std::vector<ContactData> contacts_copy;
	for ( int i = 0; i < names.size(); ++i)
	{
		std::string str;		
		str = names[i];

		for ( int j = 0; j < contacts.size(); ++j)
		{
			if (contacts[j].name == str)
			{
				contacts_copy.push_back(contacts[j]);				
				contacts.erase(contacts.begin() + j);
				break;
			}
		}
	}

	contacts.swap(contacts_copy);
    
    for ( int i = 1; i < contacts.size(); i++)
	{
		if (String::IsEqual(contacts[i].name.c_str(), contacts[i-1].name.c_str()))
		{
			for (int j=0; j<contacts[i].numbers.size(); j++)
			{
				contacts[i-1].numbers.push_back(contacts[i].numbers[j]);
			}
						
			contacts.erase(contacts.begin() + i);
			i--;
		}
		else
		{
			for (int k=0; k<contacts[i-1].numbers.size()-1; k++)
			{
				for (int l=k+1; l<contacts[i-1].numbers.size(); l++)
				{
					if (contacts[i-1].numbers[k].number == contacts[i-1].numbers[l].number)
					{
						contacts[i-1].numbers.erase(contacts[i-1].numbers.begin() + l);
						l--;
					}
				}
			}
		}
	}

	for ( int i = 0; i < contacts.size(); i++)
	{
		int len = contacts[i].numbers.size();			

		if (contacts[i].out_number.length() > 0)
		{
			contacts[i].out_number += ";";
		}

		for (int k=0; k<len; k++)
		{
			contacts[i].out_number += contacts[i].numbers[k].type;
			contacts[i].out_number += "=";
			contacts[i].out_number += contacts[i].numbers[k].number;

			if (k < len - 1)
			{
				contacts[i].out_number += ";";
			}
		}	
	}

	state_read_contacts = Done;	
}

void MobileService::FinalizeReadAvatars()
{
	state = Done;
}

void MobileService::ShowMap(bool show)
{
	if (show_map == show)
	{
		return;
	}

	show_map = show;

#ifdef PC
	if (!show_map)
	{
		mapObjects.clear();
	}
#endif

#ifdef IOS
    ios_wrapper->ShowMap(show);
#endif
    
#ifdef ANDROID
	if (show)
	{
		CallFromJava("ShowMap");
	}
	else
	{
		CallFromJava("HideMap");
	}
#endif	
}

void MobileService::SetMapPosition(float x, float y, float w, float h)
{
    if (!show_map) return;
    
	float ref_height = en_core.Render()->Display()->GetReferenceHeight();
			
	float cur_rwidth = en_core.Render()->Display()->GetWidth();
	float cur_rheight = en_core.Render()->Display()->GetHeight();		
		
	float aspect = 1.0f / ((float)cur_rheight / (float)cur_rwidth);	

	float h_mul_RefToScreen = 1.0f /ref_height * (float)cur_rwidth / aspect;
	float v_mul_RefToScreen = 1.0f / ref_height * (float)cur_rheight;

	int scr_x = x * h_mul_RefToScreen;
	int scr_y = y * v_mul_RefToScreen;
	int scr_w = w * h_mul_RefToScreen; 
	int scr_h = h * v_mul_RefToScreen;

#ifdef ANDROID
	char str[128];
	sprintf(str, "%i;%i;%i;%i", scr_x, scr_y, scr_w, scr_h);

	CallFromJava("SetMapPosition", str);
#endif

#ifdef IOS
	ios_wrapper->SetMapPosition(scr_x,scr_y,scr_w,scr_h);
#endif
    
#ifdef PC
	map_pos = Vector2(scr_x, scr_y);
	map_size = Vector2(scr_w, scr_h);
#endif
}

void MobileService::AddObjectToMap(float x, float y, const char* text)
{
#ifdef PC
	mapObjects.push_back(MapObject(x, y, text));
#endif
    
    if (!show_map) return;
    
#ifdef ANDROID
	char str[128];
	sprintf(str, "%4.10f;%4.10f;", x, y);

	string data = string(str) + string(text);

	CallFromJava("AddObjectToMap", data.c_str());
#endif

    
#ifdef IOS
	ios_wrapper->AddObjectToMap(x, y, text);	
#endif
}

void MobileService::DeleteAllMapObjects()
{
#ifdef PC
	mapObjects.clear();	
#endif

    if (!show_map) return;
    
#ifdef ANDROID
	CallFromJava("DeleteAllMapObjects");
#endif
    
#ifdef IOS
	ios_wrapper->DeleteAllMapObjects();    
#endif
}

void MobileService::ShowNativeMap(float x, float y, const char* text)
{
#ifdef ANDROID
	char str[128];
	sprintf(str, "%4.10f;%4.10f;", x, y);

	string data = string(str) + string(text);

	CallFromJava("ShowNativeMap", data.c_str());
#endif
}

void MobileService::ShowPopup(const char* title, const char* text, const char* btn_text, PopupListiner* listiner, void* listiner_data)
{
	if (show_popup != 0)
	{
		core->TraceTo("app", "ERROR: Previous popup still showed");
		return;
	}

	show_popup = 1;	
	popup_listiner = listiner;
	popup_listiner_data = listiner_data;

#ifdef ANDROID
	string str;
	str = string(title) + ";" + string(text) + ";" + string(btn_text);	

	CallFromJava("ShowPopup", str.c_str());
#endif

#ifdef PC
	popup_title = title;
	popup_text = text;
	popup_ok = btn_text;	
#endif 
    
#ifdef IOS
	ios_wrapper->ShowPopup(title, text, btn_text); 	
#endif
}

void MobileService::ShowPopup(const char* title, const char* text, const char* btn_ok_text, const char* btn_cancel_text, PopupListiner* listiner, void* listiner_data)
{
	if (show_popup != 0)
	{
		core->TraceTo("app", "ERROR: Previous popup still showed");
		return;
	}

	show_popup = 2;
	popup_listiner = listiner;
	popup_listiner_data = listiner_data;

#ifdef ANDROID
	string str;
	str = string(title) + ";" + string(text) + ";" + string(btn_ok_text)+ ";" + string(btn_cancel_text);	

	CallFromJava("ShowPopup2", str.c_str());		
#endif

#ifdef PC
	popup_title = title;
	popup_text = text;
	popup_ok = btn_ok_text;
	popup_cancel = btn_cancel_text;
#endif 
    
#ifdef IOS
	ios_wrapper->ShowPopup(title, text, btn_ok_text, btn_cancel_text);
#endif

}

void MobileService::SetPopupResult(int result)
{
	popup_result = result;
}

void MobileService::SetSendSmsResult(int res)
{
	smsResultReady = true;
	smsResult = res;
}

bool MobileService::IsAllowCall()
{
#ifdef IOS
    if (strstr(core->info.device_name, "iPhone"))
    {
        return true;
    }
    
    return false;
#endif
    
	return set_allow_call;
}

void MobileService::SetAllowCall(bool set_allow)
{
	set_allow_call = set_allow;
}

void MobileService::CallNumber(const char* number)
{
    if (!IsAllowCall())
    {
        return;
    }
 
	char phn_number_tmp[32];
    int index = 0;
            
	const char* phn_number_ptr = number;

    int ln = strlen(phn_number_ptr);
           
    for (int j=0;j<ln;j++)
    {
		if (phn_number_ptr[j] == ' ' ||
            phn_number_ptr[j] == '-' ||
            phn_number_ptr[j] == '(' ||
            phn_number_ptr[j] == ')')
        {
			continue;
        }
                
        phn_number_tmp[index] = phn_number_ptr[j];
                
        index++;
    }
            
    phn_number_tmp[index] = 0;

#ifdef ANDROID
	CallFromJava("CallNumber", phn_number_tmp);
#endif
    
#ifdef IOS
	ios_wrapper->CallNumber(phn_number_tmp);    
#endif
}

void MobileService::SendSms(const char* number, const char* text,  RequestListiner* on_end_callback, void* callback_data)
{
	if (send_sms_callback != null)
	{
		core->TraceTo("MobileService","MobileService::SendSms previous call still not ended");
		return;
	}

	send_sms_callback = on_end_callback;
	send_sms_data = callback_data;

#ifdef PC
	call_sms_callback_time = 0;
#endif

#ifdef ANDROID
	CallFromJava("SendSms", ( std::string(number) +";"+text).c_str());
#endif

#ifdef IOS
    IOSMessage *g_message = [[IOSMessage alloc] init];
    [g_message sendSMS:number text:text];
    //[message autorelease];
#endif
}

void MobileService::StartShareText(const char* text, const char* extra_data, ShareListener* listiner, void* listiner_data)
{
	if (share_listiner != null)
	{
		core->TraceTo("MobileService","MobileService::StartShareXXX previous call still not ended");
		return;
	}

	share_listiner = listiner;
    share_listiner_data = listiner_data;

	char str_enter[2];
	str_enter[0] = 13;
	str_enter[1] = 0;

	string text_str = text;
	String::ReplaceAll(text_str, "\\n", "\n");

#ifdef ANDROID	
	string txt = text_str + "|<" + extra_data;
	CallFromJava("StartShareText", txt.c_str());
#endif

#ifdef IOS
	ios_wrapper->StartShareText(text_str.c_str(), extra_data);
	share_listiner_res = "shared";
#endif	

#if defined(PC)
	share_listiner_res = "shared";
#endif	
}

void MobileService::StartShareImage(const char* image, ShareListener* listiner, void* listiner_data)
{
	if (share_listiner != null)
	{
		core->TraceTo("MobileService","MobileService::StartShareXXX previous call still not ended");
		return;
	}

	share_listiner = listiner;
	share_listiner_data = listiner_data;

#ifdef ANDROID
	CallFromJava("StartShareImage", image);	
#endif

#ifdef IOS
	ios_wrapper->StartShareImage(image);    
#endif

#if defined(PC)
	share_listiner_res = "shared";
#endif	
}

void MobileService::StartSelectImage(int type, ImageRequestListiner* on_end_callback, void* callback_data)
{	
	if (select_image_callback != null)
	{
		core->TraceTo("MobileService","MobileService::SendSms previous call still not ended");
		return;
	}

	select_image_callback = on_end_callback;
	select_image_data = callback_data;

#ifdef PC
	SetSelectImageResult(0, "MediaLibrary/Images/2.jpg");
#endif

#ifdef ANDROID
	if (type == 0) 
	{
		CallFromJava("SelectPhoto");
	}
	else
	if (type == 1) 
	{
		CallFromJava("SelectImage");
	}
	else
	if (type == 2) 
	{
		CallFromJava("SelectImagePhoto");
	}
#endif
    
#ifdef IOS
	ios_wrapper->StartSelectImage(type);    
#endif
}

void MobileService::SetShareRes(const char* res)
{
	share_listiner_res = res;
}

void MobileService::SetSelectImageResult(int res, const char* image)
{
	imageReady = true;
	imageResult = res;
	image_name = image;
}

const char* MobileService::GetSimOperatorName()
{
	return simOperatorName.data;
}

void MobileService::SetSimOperatorName(const char* name)
{
	String::Copy(simOperatorName.data, 64, name);
}

void MobileService::SetOnResumeListener(MobileService::SystemListener* listener, void* data)
{
	onpause_listiner = listener;
	onpause_listiner_data = data;	
}

void MobileService::SetOnPauseListener(MobileService::SystemListener* listener, void* data)
{
	onresume_listiner = listener;
	onresume_listiner_data = data;
}

void MobileService::SetOnPause(bool set)
{
	pause_trigered = set;
}

void MobileService::ExecutePauseCallbacks()
{	
	if (pause_trigered != is_paused)
	{	
		is_paused = pause_trigered;		

		if (is_paused)
		{
			if (onresume_listiner)
			{		
				onresume_listiner->OnEvent(onresume_listiner_data);
			}
		}
		else
		{
			if (onpause_listiner)
			{			
				onpause_listiner->OnEvent(onpause_listiner_data);
			}
		}		
	}
}

void MobileService::Reset()
{
	while (worker.IsWorking())
	{
		ThreadWorker::Sleep(100);
	}

	reqest_access_callback = null;
	reqest_access_data = 0;

	read_contacts_callback = null;
	read_contacts_data = 0;

	read_contacts_avatars_callback = null;	
	read_contacts_avatars_data = 0;

	send_sms_callback = null;
	send_sms_data = 0;

	select_image_callback = null;  
	select_image_data = 0;

	popup_listiner = null;
	popup_listiner_data = 0;

	onpause_listiner = null;
	onpause_listiner_data = 0;

	onresume_listiner = null;
	onresume_listiner_data = 0;

#ifdef PC
	show_popup = 0;
	read_contacts_allowed = false;			
	mapObjects.clear();
#endif

	show_map = false;
	is_paused = false;
	pause_trigered = false;
}

void MobileService::PasteFromClipboard(const char* text)
{
    if(clipboard_listener)
        clipboard_listener->PasteFromClipboard(text);
    clipboard_listener = null;
}

void MobileService::OnRequestIsClipboardEmpty(int bClipboardEmpty)
{
    if(clipboard_listener)
        clipboard_listener->OnRequestIsClipboardEmpty(bClipboardEmpty);
    clipboard_listener = null;
}

void MobileService::GetTextFromClipboard(ClipboardListener*  pListener)
{
#ifdef PC
    if (!IsClipboardFormatAvailable(CF_TEXT)) return; 
    if (!OpenClipboard(NULL)) return; 

    HGLOBAL hglb = GetClipboardData(CF_TEXT); 
    if (hglb != NULL) 
    { 
        LPTSTR lptstr = (LPTSTR)GlobalLock(hglb); 
        if (lptstr != NULL) 
            pListener->PasteFromClipboard(lptstr);
        GlobalUnlock(hglb); 
    } 
    CloseClipboard();	
#endif
#ifdef ANDROID
    clipboard_listener = pListener;
    CallFromJava("GetClipboardText");
#endif
#ifdef IOS
    UIPasteboard *appPasteBoard = [UIPasteboard generalPasteboard];
    NSString *string = [appPasteBoard string];
    pListener->PasteFromClipboard([string UTF8String]);
#endif
}

void MobileService::SetTextToClipboard(const char* str)
{
#ifdef PC
    if (!OpenClipboard(NULL)) return; 
    EmptyClipboard(); 

    HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (strlen(str) + 1) * sizeof(TCHAR)); 

    if (hglbCopy == NULL) 
    { 
        CloseClipboard(); 
        return; 
    } 

    LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy); 
    memcpy(lptstrCopy, str, strlen(str) * sizeof(TCHAR)); 
    lptstrCopy[strlen(str)] = (TCHAR) 0;
    GlobalUnlock(hglbCopy); 	

    SetClipboardData(CF_TEXT, hglbCopy); 

    CloseClipboard();
#endif
#ifdef ANDROID
    CallFromJava("SetClipboardText",str);
#endif
#ifdef IOS
    NSString* data = [[NSString alloc] initWithFormat:@"%s", str];
    UIPasteboard *appPasteBoard = [UIPasteboard generalPasteboard];
    appPasteBoard.persistent = YES;
    [appPasteBoard setString:data];
#endif
}

float MobileService::GetBatteryLevel()
{
#ifdef ANDROID
	CallFromJava("GetBatteryLevel");

	return android_extern_batterylevel;	
#endif

#ifdef IOS
	return ios_wrapper->GetBatteryLevel();
#endif

	return 77.7f;
}

int MobileService::GetBatteryStatus()
{
#ifdef ANDROID
	CallFromJava("GetBatteryLevel");

	return android_extern_batterystatus;	
#endif

#ifdef IOS
	return ios_wrapper->GetBatteryStatus();
#endif

	return 1;
}

Texture* MobileService::GetCameraTexture()
{
	if (!cam_texture)
	{
        
#ifdef IOS
        ios_wrapper->PrepareCameraFeed();
        cam_texture = core->Render()->CreateTexture(0, 0, 1, Texture::fmt_a8r8g8b8, _FL_);
#endif
        
#ifdef PC
		cam_texture = core->Render()->LoadTexture("editor/Miscontrue,_Space_Art.jpg", false, _FL_);
#endif
	}

	return cam_texture;
}

void MobileService::GetCameraTextureSize(int& width, int& height)
{
	width = 100;
	height = 100;

#ifdef IOS
	width = sv_camera_width;
	height = sv_camera_height;
#endif
}

void MobileService::StartScanBarcode(ScanListener* listener, void* data)
{
	scan_listener = listener;
	scan_data = data;
}

void MobileService::EnableTrackGPS(bool enable, GPSListiner* listener, void* data)
{
#ifdef IOS
	ios_wrapper->EnableTrackGPS(enable);   
#endif

#ifdef ANDROID
	if (enable)
	{
		CallFromJava("EnableGPS");
	}
	else
	{
		CallFromJava("DisableGPS");
	}	
#endif

	if (enable)
	{
		gps_listiner = listener;
		gps_data = data;
	}
	else
	{
		gps_listiner = null;
		gps_data = null;
	}
}

void MobileService::SetGPSData(int state, float latitude, float longitude)
{
	gps_ready = true;
	gps_state = state;
	gps_latitude = latitude;
	gps_longitude = longitude;
}

void MobileService::IsClipboardEmpty(ClipboardListener* pListener)
{
 #ifdef PC    
	bool res = false;

    if (OpenClipboard(NULL))
	{			
		HGLOBAL hglb = GetClipboardData(CF_TEXT); 
    
		if (hglb != NULL) 
		{ 
			LPTSTR lptstr = (LPTSTR)GlobalLock(hglb);
			GlobalUnlock(hglb);

			res = (*lptstr == 0);
		}

		CloseClipboard();
	}
	
	pListener->OnRequestIsClipboardEmpty(res);	
    
    return;
#endif
#ifdef ANDROID
    clipboard_listener = pListener;
    CallFromJava("IsClipboardEmpty");
#endif
#ifdef IOS
    UIPasteboard *appPasteBoard = [UIPasteboard generalPasteboard];
    NSString *string = [appPasteBoard string];
    if([string length] > 0)
        pListener->OnRequestIsClipboardEmpty(0);
    else
        pListener->OnRequestIsClipboardEmpty(1);
#endif
}
