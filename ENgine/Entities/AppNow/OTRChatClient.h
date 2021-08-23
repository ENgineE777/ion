#pragma once
#include "Core/Core.h"

#include "ChatCommon.h"

#ifdef SendMessage
#undef SendMessage
#endif

class OTRChatClient;
class OTRChatClientImpl;

class OTRChatClientWorker : public ThreadWorker
{
public:
	bool ready_for_read;
	bool connected;

	OTRChatClient* client;
	OTRChatClientImpl* client_impl;
	std::vector<std::string> new_messages;

	ChatCommandPool command_pool;

	OTRChatClientWorker();

	virtual void Prepare();
	virtual void Loop();

	void SendCommand(const std::string& cmd);
	void SendCommand(const char* cmd);
};

class OTRChatClient : public SceneEntity
{	
public:

	PROP_START
	STRING_PROP(OTRChatClient, func_on_connect, "", "Properties", "OnConnect")
	STRING_PROP(OTRChatClient, func_on_message, "", "Properties", "OnMessage")
	STRING_PROP(OTRChatClient, func_on_error, "", "Properties", "OnError")
	STRING_PROP(OTRChatClient, func_on_disconnect, "", "Properties", "OnDisconnect")
	
	ENTITY_DEFC(OTRChatClient)

	ScriptFunction callback_connect;
	ScriptFunction callback_message;
	ScriptFunction callback_error;
	ScriptFunction callback_disconect;

	virtual void RegisterProperties();
	virtual void Init();	
	virtual void ReInit();	
	virtual bool Start();
	virtual void Finish();

	virtual void MethodsToScript(const char* script_class_name);

	virtual void Work(float dt, int level);

	virtual void Release();

	void Connect(std::string& url, int port);
	void Disconnect();
	void SendMessage(std::string& txt);
	void SendCommand(std::string& cmd);
	int SendFile1(std::string& cmd, std::string& path);

public:
	std::string url;
	int port;

	bool call_on_error;
	bool on_error_called;

private:
	bool on_conncet_called;
	bool on_disconncet_called;

	OTRChatClientWorker client_worker;
};