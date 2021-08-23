#pragma once
#include "Core/Core.h"

#include "ChatCommon.h"

class ChatClient;
class ChatClientImpl;

class ChatClientWorker : public ThreadWorker
{
public:
	bool ready_for_read;
	bool connected;

	ChatClient* client;
	ChatClientImpl* client_impl;
	std::vector<std::string> new_messages;

	ChatCommandPool command_pool;

	ChatClientWorker();

	virtual void Prepare();
	virtual void Loop();

	void SendCommand(const std::string& cmd);
	void SendCommand(const char* cmd);
};

class ChatClient : public SceneEntity
{	
public:

	PROP_START
	STRING_PROP(ChatClient, func_on_connect, "", "Properties", "OnConnect")
	STRING_PROP(ChatClient, func_on_message, "", "Properties", "OnMessage")
	STRING_PROP(ChatClient, func_on_error, "", "Properties", "OnError")
	STRING_PROP(ChatClient, func_on_disconnect, "", "Properties", "OnDisconnect")
	
	ENTITY_DEFC(ChatClient)

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

	void Connect1(std::string& url, int port);
	void Disconnect1();
	void SendMessage1(std::string& txt);
	void SendCommand1(std::string& cmd);
	int SendFile(std::string& cmd, std::string& path);

public:
	std::string url;
	int port;

	bool call_on_error;
	bool on_error_called;

private:
	bool on_conncet_called;
	bool on_disconncet_called;

	ChatClientWorker client_worker;
};
