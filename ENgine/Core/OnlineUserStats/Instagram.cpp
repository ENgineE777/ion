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

void InstagramNet::Init()
{	
	signin_state = ss_not_signed;

	sign_in_data = null;
	sign_in_listiner = null;
	sign_out_data = null;
	sign_out_listiner = null;

	sign_in_res = -1;
}

void InstagramNet::SignIn(InstagramListiner* listiner, void* data)
{
	if (signin_state == ss_signouting)
	{
		core->TraceTo("Instagram", "Error: in sigout already");
		return;
	}

	sign_in_data = data;
	sign_in_listiner = listiner;	
    signin_state = ss_signing;

#ifdef IOS
    ios_wrapper->InstagramSignIn();
#endif
    
#if defined(ANDROID)
	CallFromJava("OpenInstagramSession");
#endif 

#if defined(PC)
	SetSignInStatus(true, "1720454143.a5e4c1e.eefa713624174e0db9c6b7f1ebc17ce6");
#endif 
}

void InstagramNet::SignOut(InstagramListiner* listiner, void* data)
{	
	if (signin_state != ss_not_signed)
	{
		core->TraceTo("Instagram", "Error: in signed already");
		return;
	}

	sign_out_data = data;
	sign_out_listiner = listiner;	
	signin_state = ss_signouting;    
}

const char* InstagramNet::GetAccessToken()
{
	return accessToken.c_str();
}

void InstagramNet::SetSignInStatus(bool in_singin, const char* token)
{
	core->TraceTo("Instagram", "SetSignInStatus, %d, %s", in_singin, token);
	
	if (sign_in_res)
	{
		sign_in_res = 0;
	}
	else
	{
		sign_in_res = 1;
	}

	accessToken = token;	
	signin_state = ss_signed; 
}

InstagramNet::SignInState InstagramNet::GetSignInStatus()
{
	return signin_state; 
}

void InstagramNet::Work(float dt)
{		
	if (sign_in_res != -1)
	{
		if (sign_in_listiner)
		{
			sign_in_listiner->OnFinish(sign_in_data);
			sign_in_listiner = null;			
		}	

		sign_in_res = -1;
	}

	if (signin_state == ss_signouting)
	{
		if (sign_out_listiner)
		{
			sign_out_listiner->OnFinish(sign_out_data);
			sign_out_listiner = null;

			signin_state = ss_not_signed;
		}		
	}
}

void InstagramNet::Reset()
{
	signin_state = ss_not_signed;	
}