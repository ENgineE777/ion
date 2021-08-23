#pragma once

#include "Core/Network/NetworkService.h"

class JSONFile;

class DEF_EXPORT InstagramNet : public TaskHoldersOwner
{
public:

	enum SignInState
	{
		ss_not_signed,				
		ss_signing,
		ss_signed,
		ss_signouting
	};
	
	class InstagramListiner
	{
	public:
		
		virtual void OnFinish(void* data) = 0;		
	};

private:
			
	int sign_in_res;
	void* sign_in_data;
	InstagramListiner* sign_in_listiner;
	void* sign_out_data;
	InstagramListiner* sign_out_listiner;
	SignInState signin_state;
	std::string accessToken;	
		
public:		

	void Init();	

	void SignIn(InstagramListiner* listiner, void* data);
	void SignOut(InstagramListiner* listiner, void* data);

	SignInState GetSignInStatus();
	void SetSignInStatus(bool in_singin, const char* token);				

	const char* GetAccessToken();
	
	void Work(float dt);		
	
	void Reset();
};
