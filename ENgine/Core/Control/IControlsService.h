
#pragma once

#include "Common/Common.h"

#ifdef PC

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#endif

#include "Common/Common.h"

class IControlsService : public TaskHoldersOwner
{	
public:

	enum KeyAction
	{			
		Activated,
		ActivatedExclusive,
		FastActivated,
		FastActivatedExclusive,
		Active,
		ActiveExclusive,
		ActivatedExclusiveNonBlockFastActivated
	};

	enum KeyboardType
	{
		kb_Text,
		kb_URL,
		kb_Number,
		kb_Phone,
		kb_Email
	};

	enum KeyboardReturnKey
	{
		rk_Default,
		rk_Next,
		rk_Done,
		rk_Go,
		rk_Search
	};	

	virtual bool  Init() = 0;	
	virtual bool  ControlState(int index, KeyAction action = Activated) = 0;
	virtual float ControlValue(int index) = 0;

	virtual bool  IsAnyKeyPressed() = 0;
	virtual bool  CheckTouch(int index, KeyAction action, void* ask_owner) = 0; 
	virtual bool  CheckTouchIgnoreOwner(int index, KeyAction action) = 0;
	
	virtual float GetTouchX(int index) = 0;
	virtual float GetTouchY(int index) = 0;
	
	virtual float GetTouchInitX(int index) = 0;
	virtual float GetTouchInitY(int index) = 0;

	virtual float GetGyro(int index) = 0;
	virtual void  SetGyro(int index, float val) = 0;

	virtual bool  IsBackPressed() = 0;

	virtual void  TouchStart(int index, float x, float y) = 0;
	virtual void  TouchMove(int index, float x, float y) = 0;
	virtual void  TouchEnd(int index) = 0;

	virtual void  SetLastChar(char chr) = 0;
	virtual void  SetLastChar(wchar_t chr) = 0;
	virtual bool  GetLastChar(wchar_t& chr) = 0;

	virtual void  SetSoftKeyboardHeight(int hgt) = 0;

	virtual void  SetDisableFastActivated(bool set) = 0;
	virtual void  ShowSoftKeyboard(KeyboardType kb_type, KeyboardReturnKey kb_ret_key) = 0;
	virtual void  HideSoftKeyboard() = 0;
	virtual bool  IsShowSoftKeyboard() = 0;
	virtual int   GetSoftKeyboardHeight() = 0;

#ifdef ANDROID
	virtual void SetBackBtnStateOn() = 0;	
#endif

	virtual void  Release() = 0;
};
