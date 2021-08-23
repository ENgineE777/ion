
#pragma once

#include "Core/Control/IControlsService.h"

#ifdef PC

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

class TextureInstance;

#endif

class ControlsService : public IControlsService
{			
	byte					btns[256];
	
	bool					is_soft_keyboard_show;
	int						prev_keyboard_height;
	int						soft_keyboard_height;

	float					gyro[3];

#ifdef PC
	LPDIRECTINPUT8          pDI;
	LPDIRECTINPUTDEVICE8    pKeyboard;
	LPDIRECTINPUTDEVICE8    pMouse;
	
	dword					dwElements;
	byte					diks[256];
	DIMOUSESTATE2			dims2;
	dword					dwMsElements;
	byte					ms_bts[10];	
	bool					simulate_sec_touch_stage;

	float					anim_time;
	TextureInstance*		keyboard_tex;
#endif
    
    int hack_show_keyboard;
    
	struct Touch
	{
        bool                    skip_update;
		void*                   owner;
		int						cur_state;
		Vector2					cur_pos;
		Vector2					last_pos;

		Vector2					init_pos;
	
		int						last_cur_state;
		
		Vector2					cursorPos;
	
		float				    ms_timer;
		bool					fast_ms;
	};

	Touch touches[10];
	int     last_char_state;
	wchar_t last_char;

	bool disable_fast_activated;

#ifdef ANDROID
	int back_state;
	int last_back_state;
#endif

public :	

    IControlsService::KeyboardType cur_kb_type;
	IControlsService::KeyboardReturnKey cur_kb_ret_key;

	ControlsService();
	virtual bool  Init();
	void  Work(float dt, int level);
	virtual bool  ControlState(int index, KeyAction action = Activated);
	virtual float ControlValue(int index);

	virtual bool  IsAnyKeyPressed();

	virtual bool  CheckTouch(int index, KeyAction action, void* ask_owner);
	virtual bool  CheckTouchIgnoreOwner(int index, KeyAction action);
	
#ifdef PC
	bool  MouseState(int index, KeyAction action = Activated);
	void  DrawSoftKeyboard(float dt, int level);
#endif

#ifdef ANDROID
	virtual void SetBackBtnStateOn();
	virtual void CheckSoftKeyboardHeight(float dt, int level);	
#endif

	virtual void  SetTouchPos(float x, float y);

	virtual float GetTouchX(int index);
	virtual float GetTouchY(int index);
	
	virtual float GetTouchInitX(int index);
	virtual float GetTouchInitY(int index);
	
	virtual float GetGyro(int index);
	virtual void  SetGyro(int index, float val);

	virtual bool  IsBackPressed();

	virtual void  TouchStart(int index, float x, float y);
	virtual void  TouchMove(int index, float x, float y);
	virtual void  TouchEnd(int index);

	virtual void  SetLastChar(char chr);
	virtual void  SetLastChar(wchar_t chr);
	virtual bool  GetLastChar(wchar_t& chr);

	virtual void  SetSoftKeyboardHeight(int hgt);

	virtual void  SetDisableFastActivated(bool set);

	virtual void  ShowSoftKeyboard(IControlsService::KeyboardType kb_type, IControlsService::KeyboardReturnKey kb_ret_key);
	virtual void  HideSoftKeyboard();
	virtual bool  IsShowSoftKeyboard();
	virtual int   GetSoftKeyboardHeight();

	virtual void  Release();

protected:
	void ResetStates();
};
