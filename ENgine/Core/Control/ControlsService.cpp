#include "Core/Core.h"

float ms_cooldown = 0.6f;

#ifdef IOS

#include "Platforms/IOS/IOSWrapper.h"

#endif

#ifdef ANDROID
extern void CallFromJava(const char* function);
extern void CallFromJava(const char* function, const char* param);
#endif

//#define DRAW_TOUCH_TEXTURE

#ifdef DRAW_TOUCH_TEXTURE
TextureInstance* keyboard_tex_sec;
#endif 



ControlsService::ControlsService()
{	
	ResetStates();

	is_soft_keyboard_show = false;

	soft_keyboard_height = 0;

	gyro[0] = gyro[1] = gyro[2] = 0.0f;

    hack_show_keyboard = 50;
#ifdef PC
	anim_time = 0.0f;
	keyboard_tex = null;
	soft_keyboard_height = 345;
#endif
    
	cur_kb_type = IControlsService::kb_Text;
	cur_kb_ret_key = IControlsService::rk_Default;

#ifdef ANDROID
	back_state = last_back_state = 0;
#endif

	prev_keyboard_height = 0;
	disable_fast_activated = false;
}

bool ControlsService::Init()
{	
	is_soft_keyboard_show = false;

#ifdef PC
	
	if( FAILED( DirectInput8Create( GetModuleHandle( NULL ), DIRECTINPUT_VERSION,
				IID_IDirectInput8, ( VOID** )&pDI, NULL ) ) )
	{
		return false;
	}
	
	if( FAILED( pDI->CreateDevice( GUID_SysKeyboard, &pKeyboard, NULL ) ) )
	{
		return false;
	}
	
	if( FAILED( pKeyboard->SetDataFormat( &c_dfDIKeyboard ) ) )
	{
		return false;
	}

	pKeyboard->SetCooperativeLevel( en_core.GetHWND(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND );

	DIPROPDWORD dipdw;

	dipdw.diph.dwSize = sizeof( DIPROPDWORD );
	dipdw.diph.dwHeaderSize = sizeof( DIPROPHEADER );
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = 16;

	if( FAILED( pKeyboard->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph ) ) )
	{
		return false;
	}	

	pKeyboard->Acquire();

	if( FAILED( pDI->CreateDevice( GUID_SysMouse, &pMouse, NULL ) ) )
	{
		return false;
	}

	if( FAILED( pMouse->SetDataFormat( &c_dfDIMouse2 ) ) )
	{
		return false;
	}
	
	pMouse->SetCooperativeLevel( en_core.GetHWND(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND );
			
	dipdw.diph.dwSize = sizeof( DIPROPDWORD );
	dipdw.diph.dwHeaderSize = sizeof( DIPROPHEADER );
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = 16;

	if( FAILED( pMouse->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph ) ) )
	{
		return false;
	}
	
	pMouse->Acquire();

	en_core.Render()->Tasks()->AddTask("gui", this, this, (TaskHolder::Task)&ControlsService::DrawSoftKeyboard, 20);

	keyboard_tex = en_core.Render()->LoadTexture("Editor/soft_keyboard.png", false, _FL_);
#endif
#ifdef DRAW_TOUCH_TEXTURE
	keyboard_tex_sec = en_core.Render()->LoadTexture("Editor/soft_keyboard.png", false, _FL_);
#endif
	core->AddTask(this, this, (TaskHolder::Task)&ControlsService::Work, lvl_camera1-1, -1.f);

#ifdef ANDROID
	core->AddTask(this, this, (TaskHolder::Task)&ControlsService::CheckSoftKeyboardHeight, -330, 0.02f);	
#endif

	return true;
}

void ControlsService::Work(float dt, int level)
{
#ifdef ANDROID
	if (back_state > 0)
	{
		back_state--;
	}	
#endif

	for (int i=0;i<10;i++)
	{
        if (touches[i].skip_update)
        {
            touches[i].skip_update = false;
            continue;
        }
        
		if (touches[i].last_cur_state == 1 && touches[i].cur_state == 1)
		{		
			touches[i].cur_state = 2;
		}
	
		if (touches[i].last_cur_state == 0 && touches[i].cur_state == 1)
		{
			if (touches[i].ms_timer < -0.001f) touches[i].ms_timer = 0.0f;		
		}
	
		touches[i].last_cur_state = touches[i].cur_state;		

		if (touches[i].cur_state == 1)
		{
			touches[i].init_pos = touches[i].cur_pos;
		}
	}

	

#ifdef PC
							
	static byte tmp_diks[256];
	ZeroMemory( tmp_diks, sizeof( tmp_diks ) );
	HRESULT hr = pKeyboard->GetDeviceState( sizeof( tmp_diks ), tmp_diks );
	
	for (int i=0;i<256;i++)
	{
		if (tmp_diks[i])
		{
			btns[i]++;
			
			if (btns[i]>2)
			{
				btns[i] = 2;
			}
		}
		else
		{
			btns[i] = 0;
		}
	}	

	if( FAILED( hr ) )
	{				
		hr = pKeyboard->Acquire();
		
		while( hr == DIERR_INPUTLOST ) hr = pKeyboard->Acquire();
		
		if( hr == DIERR_OTHERAPPHASPRIO || hr == DIERR_NOTACQUIRED )
		{
			return;
		}
	}
		
	ZeroMemory( &dims2, sizeof( dims2 ) );
	hr = pMouse->GetDeviceState( sizeof( DIMOUSESTATE2 ), &dims2 );

	for (int i=0;i<4;i++)
	{
		if (dims2.rgbButtons[i] & 0x80)
		{				
			ms_bts[i]++;
			
			if (i==0 && ms_bts[i] == 1)
			{
				if (simulate_sec_touch_stage == 0)
				{
					if (touches[0].cur_state == 0)
					{						
						touches[0].cur_state = 1;
					}
				}
				else
				{
					if (touches[1].cur_state == 0)
					{						
						touches[1].cur_state = 1;
					}
				}
			}

			if (ms_bts[i]>2)
			{
				ms_bts[i] = 2;					
			}			
		}
		else
		{			
			ms_bts[i] = 0;
			//ms_bts_owners[i] = null;

			if (simulate_sec_touch_stage == 0)
			{
				if (i==0)
				{									
					touches[0].cur_state = 0;
				}
			}
			else
			{
				if (i==0) touches[1].cur_state = 0;
			}
		}
	}

	if( FAILED( hr ) )
	{		
		hr = pMouse->Acquire();
		
		while( hr == DIERR_INPUTLOST ) hr = pMouse->Acquire();

		// Update the dialog text 
		if( hr == DIERR_OTHERAPPHASPRIO ||
			hr == DIERR_NOTACQUIRED )			
		{
			//SetDlgItemText( hDlg, IDC_DATA, TEXT( "Unacquired" ) );
		}		

		return;
	}	
#endif	

	for (int i=0;i<10;i++)
	{
		if (touches[i].ms_timer>-0.001f)
		{
			touches[i].ms_timer += dt;
			
            if (touches[i].last_cur_state == 0)
			{
				if (touches[i].ms_timer<ms_cooldown)
				{
					if ((touches[i].init_pos - touches[i].cur_pos).Length() < 25 * en_core.Render()->Display()->GetScreenDPI() / 326.0f)
					{
						touches[i].fast_ms = true;
					}
                    else
                    {
                        //core->TraceTo("debug", "FastActivateWasBlocked by Dist ix %4.3f i%4.3f cx %4.3f cy %4.3f", touches[i].init_pos.x, touches[i].init_pos.y, touches[i].cur_pos.x, touches[i].cur_pos.y);
                    }
				}
                
				touches[i].ms_timer = -1.0f;
			}
            
            if (disable_fast_activated)
            {
                touches[i].fast_ms = false;
                touches[i].ms_timer = -1.0f;
                
                //core->TraceTo("debug", "FastActivateWasBlocked by Permission");
            }
		}
		else
		{
			touches[i].fast_ms = false;
		}

		if (touches[i].last_cur_state == 0 && !touches[i].fast_ms)
		{	
			touches[i].owner = null;
			touches[i].ms_timer = -1.0f;
		}
	}

#ifdef PC
	if (simulate_sec_touch_stage == 0)
	{
		if (CheckTouch(0, Active, null) && ControlState(DIK_LALT, Active))// && ControlState(DIK_X, Active))
		{
			simulate_sec_touch_stage = 1;
		}
	}
	else
	if (simulate_sec_touch_stage == 1)
	{
		if (!ControlState(DIK_LALT, Active))// || !ControlState(DIK_X, Active))
		{			
			touches[1].cur_state = 0;
			touches[1].last_cur_state = 0;
			simulate_sec_touch_stage = 0;
		}
	}	
#endif
        
	for (int i=0;i<10;i++)
	{
		touches[i].last_pos = touches[i].cur_pos;
	}

	if (last_char_state>0)
	{
		last_char_state--;
	}
#ifdef DRAW_TOUCH_TEXTURE
	en_core.Render()->Debug()->DrawSprite(keyboard_tex_sec, Vector2(touches[0].cur_pos.x - 25, touches[0].cur_pos.y - 25), Vector2(50.0f), up_left, COLOR_WHITE, 0.0f);
#endif
}

bool ControlsService::ControlState(int index, KeyAction action)
{
#ifdef PC
	HWND active_window = ::GetActiveWindow();
	HWND focus_window = ::GetFocus();

	if (!active_window)
	{
		// In foreground!
		return false;
	}
#endif

	if (action == Activated)
	{		
		return (btns[index]==1);
	}

	if (action == ActivatedExclusive)
	{		
		if (btns[index]==1)
		{
			btns[index]++;			
			return true;
		}

		return false;		
	}

	if (action == Active)
	{
		return (btns[index]>0);
	}

	return false;	
}

float ControlsService::ControlValue(int index)
{	
	if (index<255)
	{
		if (btns[index]) return 1.0f;
		return 0.0f;
	}

	return 0.0f;
}

bool ControlsService::IsAnyKeyPressed()
{
#ifdef PC
	for (int i=0;i<256;i++)
	{
		if (ControlState(i, Activated))
		{
			return true;
		}
	}
#endif

	return false;
}

bool ControlsService::CheckTouch(int index, KeyAction action, void* ask_owner)
{	
	if (action == FastActivated || action == FastActivatedExclusive)
	{
		if (disable_fast_activated)
		{
			return false;
		}

		bool res = touches[index].fast_ms;

		if (action == FastActivated)
		{
			if (touches[index].owner)
			{
				res = false;
			}
		}
		else
		{
			if (action == FastActivatedExclusive)
			{			
				touches[index].fast_ms = false;
			}
			/*else
			{
				res = false;
			}*/
		}

		if (res)
		{
			res = res;
		}

		return res;
	}

	if (action == Activated)
	{
		return (touches[index].last_cur_state == 1);
	}

	if (action == ActivatedExclusive || action == ActivatedExclusiveNonBlockFastActivated)
	{
		if (touches[index].last_cur_state == 1)
		{
			touches[index].cur_state = 2;
			touches[index].last_cur_state = 2;
			touches[index].owner = ask_owner;

			if (action == ActivatedExclusive) touches[index].ms_timer = 10.0f;	
			return true;
		}
		
		return false;
	}
	
	if (!touches[index].owner || (ask_owner && touches[index].owner == ask_owner))
	{
		if (action == Active)
		{
			return (touches[index].last_cur_state != 0);
		}

		if (action == ActiveExclusive)
		{
			bool res = (touches[index].last_cur_state != 0);

			if (res)
			{
				touches[index].owner = ask_owner;					
			}

			return res;
		}	
	}

	return false;
}

bool ControlsService::CheckTouchIgnoreOwner(int index, KeyAction action)
{	
	if (action == Active)
	{
		return (touches[index].last_cur_state != 0);
	}

	if (action == ActiveExclusive)
	{
		bool res = (touches[index].last_cur_state != 0);
		return res;
	}	

	return false;
}

#ifdef PC
bool ControlsService::MouseState(int index, KeyAction action)
{	
	if (action == Activated)
	{		
		return (ms_bts[index]==1);
	}

	if (action == ActivatedExclusive)
	{		
		if (ms_bts[index]==1)
		{
			ms_bts[index]++;			
			return true;
		}

		return false;		
	}

	if (action == Active)
	{
		return (ms_bts[index]>0);
	}

	return false;
}

void ControlsService::DrawSoftKeyboard(float dt, int level)
{	
	if (is_soft_keyboard_show)
	{
		anim_time += dt * 2.0f;

		if (anim_time > 1.0f)
		{
			anim_time = 1.0f;
		}
	}
	else
	{
		anim_time -= dt * 2.0f;

		if (anim_time < 0.0f)
		{
			anim_time = 0.0f;
		}
	}

	if (anim_time > 0.01f)
	{
		float k = EasyInOut::CubicEaseIn(anim_time);

		Vector2 size(en_core.Render()->Display()->GetWidth(), 345.0f / (float)en_core.Render()->Display()->GetReferenceHeight() * (float)en_core.Render()->Display()->GetHeight());
	
		Vector2 pos(0.0f, en_core.Render()->Display()->GetHeight() - size.y * k);
	
		en_core.Render()->Debug()->DrawSprite(keyboard_tex, pos, size, up_left, COLOR_WHITE, 0.0f);

		const char* str_kb_type = "Text";		
		
		if (cur_kb_type == kb_URL) str_kb_type = "URL";
		if (cur_kb_type == kb_Number) str_kb_type = "Number";
		if (cur_kb_type == kb_Phone) str_kb_type = "Phone";
		if (cur_kb_type == kb_Email) str_kb_type = "Email";

		const char* str_kb_ret_key = "Default";

		if (cur_kb_ret_key == rk_Next) str_kb_ret_key = "Next";
		if (cur_kb_ret_key == rk_Done) str_kb_ret_key = "Done";
		if (cur_kb_ret_key == rk_Go) str_kb_ret_key = "Go";
		if (cur_kb_ret_key == rk_Search) str_kb_ret_key = "Search";

		en_core.Render()->Debug()->PrintText(pos.x + 10, pos.y + 10, up_left, COLOR_BLUE, "Type: %s Return Key: %s", str_kb_type, str_kb_ret_key);
	}

}

#endif

#ifdef ANDROID
void ControlsService::SetBackBtnStateOn()
{
	if (back_state == 0)
	{
		back_state = 4;
	}
	else
	if (back_state < 3)
	{
		back_state = 2;
	}
}

void ControlsService::CheckSoftKeyboardHeight(float dt, int level)
{
	CallFromJava("GetSoftKeyboardHeight");
}
#endif

void ControlsService::SetTouchPos(float x, float y)
{
#ifdef PC
	if (simulate_sec_touch_stage == 0)
	{
		touches[0].cur_pos.x = x;
		touches[0].cur_pos.y = y;	
	}
	else
	{
		touches[1].cur_pos.x = x;
		touches[1].cur_pos.y = y;	
	}
#endif
}

float ControlsService::GetTouchX(int index)
{
	return touches[index].last_pos.x;
}

float ControlsService::GetTouchY(int index)
{
	return touches[index].last_pos.y;
}

float ControlsService::GetTouchInitX(int index)
{
	return touches[index].init_pos.x;
}

float ControlsService::GetTouchInitY(int index)
{
	return touches[index].init_pos.y;
}

float ControlsService::GetGyro(int index)
{
	if (index<0 || index>2) return 0.0f;

	return gyro[index];
}

void ControlsService::SetGyro(int index, float val)
{
	gyro[index] = val;
}

bool ControlsService::IsBackPressed()
{
#ifdef PC
	return ControlState(DIK_F2);
#endif

#ifdef ANDROID
	return (back_state == 3);
#endif

	return false;
}

void ControlsService::TouchStart(int index, float x, float y)
{
	touches[index].cur_state = 1;
	touches[index].cur_pos.x = x;
	touches[index].cur_pos.y = y;
    touches[index].init_pos = touches[index].cur_pos;
}

void ControlsService::TouchMove(int index, float x, float y)
{	
	touches[index].cur_pos.x = x;
	touches[index].cur_pos.y = y;
}

void ControlsService::TouchEnd(int index)
{
    //FIX FOR IOS 7
    if (touches[index].last_cur_state == 0 && touches[index].cur_state > 0)
    {
        touches[index].last_cur_state = touches[index].cur_state;
        touches[index].ms_timer = 0.001f;
        touches[index].skip_update = true;
    }
    
	touches[index].cur_state = 0;
}

void ControlsService::SetLastChar(char chr)
{
	char str[4];

	str[0] = chr;
	str[1] = 0;

	std::wstring wstr;
	String::Utf8toUtf16(wstr, str);

	last_char_state = 2;
	last_char = wstr.c_str()[0];
}

void ControlsService::SetLastChar(wchar_t chr)
{
	last_char_state = 2;
	last_char = chr;
}

bool ControlsService::GetLastChar(wchar_t& chr)
{
	if (last_char_state == 0) return false;

	chr = last_char;
	last_char_state = 0;

	return true;
}

void ControlsService::SetSoftKeyboardHeight(int set_hgt)
{		
	prev_keyboard_height = soft_keyboard_height;

	float ref_height = en_core.Render()->Display()->GetReferenceHeight();
		
	float cur_rheight = en_core.Render()->Display()->GetHeight();
			
#ifdef IOS
   	float tmp = set_hgt / (float)cur_rheight * ref_height;
    
    //if (tmp > soft_keyboard_height)
    {
        soft_keyboard_height = tmp;
    }
#else
	soft_keyboard_height = set_hgt / (float)cur_rheight * ref_height;
#endif
}

void ControlsService::SetDisableFastActivated(bool set)
{
	disable_fast_activated = set;
}

void ControlsService::ShowSoftKeyboard(IControlsService::KeyboardType kb_type, IControlsService::KeyboardReturnKey kb_ret_key)
{
	is_soft_keyboard_show = true;

	cur_kb_type = kb_type;
	cur_kb_ret_key = kb_ret_key;
	
#ifdef IOS    
    ios_wrapper->ShowSoftKeyboard(kb_type, kb_ret_key);
#endif  

#if defined(ANDROID)
	
	const char* str_kb_type = "Text";		
		
	if (kb_type == IControlsService::kb_URL) str_kb_type = "URL";
	if (kb_type == IControlsService::kb_Number) str_kb_type = "Number";
	if (kb_type == IControlsService::kb_Phone) str_kb_type = "Phone";
	if (kb_type == IControlsService::kb_Email) str_kb_type = "Email";

	const char* str_kb_ret_key = "Default";

	if (kb_ret_key == IControlsService::rk_Next) str_kb_ret_key = "Next";
	if (kb_ret_key == IControlsService::rk_Done) str_kb_ret_key = "Done";
	if (kb_ret_key == IControlsService::rk_Go) str_kb_ret_key = "Go";
	if (kb_ret_key == IControlsService::rk_Search) str_kb_ret_key = "Search";

	char str[64];
	sprintf(str, "%s;%s", str_kb_type, str_kb_ret_key);

	CallFromJava("ShowKeyboard", str);
#endif
}

void ControlsService::HideSoftKeyboard()
{
	is_soft_keyboard_show = false;

#ifdef IOS    
	ios_wrapper->HideSoftKeyboard();
#endif  

#if defined(ANDROID)
	CallFromJava("HideKeyboard");	
#endif
}

bool ControlsService::IsShowSoftKeyboard()
{
#ifdef ANDROID
	if (is_soft_keyboard_show)
	{
		if (prev_keyboard_height != soft_keyboard_height)
		{
			return false;
		}
	}
#endif

	return is_soft_keyboard_show;
}

int ControlsService::GetSoftKeyboardHeight()
{
	if (!IsShowSoftKeyboard()) return 0;
        
	return soft_keyboard_height;
}

void ControlsService::Release()
{
#ifdef PC
	
	RELEASE(keyboard_tex)
	
	if ( pKeyboard )
	{
		pKeyboard->Unacquire();	
		pKeyboard->Release();
	}
	
	if ( pMouse )
	{
		pMouse->Unacquire();	
		pMouse->Release();
	}

	if ( pDI )
	{
		pDI->Release();
	}

#endif

	ResetStates();
}

void ControlsService::ResetStates()
{
#ifdef PC
	pDI = null;
	pKeyboard = null;	
	pMouse = null;
	dwElements = 0;
	dwMsElements = 0;	
	ZeroMemory( &btns, sizeof( btns ) );

	ZeroMemory( &ms_bts, sizeof( ms_bts ) );

	simulate_sec_touch_stage = 0;
#endif

	memset(touches, 0, sizeof(touches));

	for (int i=0;i<10;i++)
	{
		touches[i].ms_timer = -1.0f;
		touches[i].cur_pos = 0.0f;
		touches[i].last_pos = 0.0f;
		touches[i].init_pos = 0.0f;
		touches[i].cursorPos = 0.0f;
        touches[i].skip_update = false;
	}

	last_char_state = 0;
	last_char = 0;
}
