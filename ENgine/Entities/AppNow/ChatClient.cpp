
#include "ChatClient.h"
#include "ChatClientImpl.h"

#include "../../Core/Script/Libs/scriptdictionary.h"

#include "Core/Core.h"


ENTITY(ChatClient)
ENTITY_PARAMS(ChatClient, "ChatClient", "AppNow", "ChatClient")

ChatClientWorker::ChatClientWorker() : connected(false), ready_for_read(false), command_pool(32)
{
}

void ChatClientWorker::Prepare()
{
}

void ChatClientWorker::Loop()
{
	if (client_impl->Connect(client->url, client->port))
	{
		while (!NeedExit())
		{
			if (client_impl->IsDisconnected())
			{
				// en_core.TraceTo("Chat", "ChatClient::Loop: set disconnect flag");

				connected = false;
				break;
			}

			if (ready_for_read)
			{
				ThreadWorker::Sleep(100);
				continue;
			}

			ChatState chat_state = client_impl->GetState();

			if (chat_state == kChatProcessMessages)
			{
				ChatCommandSlot* slot = command_pool.GetUsedSlot();
				if (slot)
				{
					client_impl->SendCommand(slot->command.c_str());

					slot->used = false;
					slot->free = true;

					// TODO: Uncomment if all commands must be sent all at once
					// continue;
				}
			}

			ready_for_read = client_impl->ProcessDataStream(new_messages);

			chat_state = client_impl->GetState();

			if (chat_state == kChatProcessMessages)
			{
				connected = true;
			}
			else if (chat_state == kChatError)
			{
				// en_core.TraceTo("Chat", "ChatClient::Loop: process error");

				client_impl->Disconnect();

				connected = false;
				client->call_on_error = true;
				client->on_error_called = false;
				break;
			}
		}
	}
	else
	{
		// en_core.TraceTo("Chat", "ChatClient::Loop: connection is failed");

		client_impl->Disconnect();

		client->call_on_error = true;
		client->on_error_called = false;
	}
}

void ChatClientWorker::SendCommand(const char* cmd)
{
	ChatCommandSlot* slot = command_pool.GetFreeSlot();
	if (!slot)
	{
		return;
	}

	slot->free = false;
	slot->command.assign(cmd);
	slot->used = true;
}

void ChatClientWorker::SendCommand(const std::string& cmd)
{
	ChatCommandSlot* slot = command_pool.GetFreeSlot();
	if (!slot)
	{
		return;
	}

	slot->free = false;
	slot->command = cmd;
	slot->used = true;
}

void ChatClient::RegisterProperties()
{
	SceneEntity::RegisterProperties();

	PROP_REG( func_on_connect )	
	PROP_REG( func_on_message )
	PROP_REG( func_on_error )
	PROP_REG( func_on_disconnect )
}

void ChatClient::Init()
{			
	on_conncet_called = false;
	on_disconncet_called = false;
	call_on_error = false;
	on_error_called = false;

	client_worker.client = this;
	client_worker.client_impl = new ChatClientImpl;

	SetUpdate((ChatClient::Task)&ChatClient::Work, lvl_work1);

	en_core.TraceTo("Chat", "ChatClient::Init");
}

void ChatClient::ReInit()
{
#if defined(PC) || defined(OSUNIX)
#endif
}

bool ChatClient::Start()
{		
	ScriptFunction::Find(&callback_connect, Scene()->ScriptModule(), func_on_connect.c_str());
	ScriptFunction::Find1S(&callback_message, Scene()->ScriptModule(), func_on_message.c_str());
	ScriptFunction::Find(&callback_error, Scene()->ScriptModule(), func_on_error.c_str());
	ScriptFunction::Find(&callback_disconect, Scene()->ScriptModule(), func_on_disconnect.c_str());

	return true;
}

void ChatClient::Finish()
{		
	callback_connect.Reset();
	callback_message.Reset();
	callback_error.Reset();
	callback_disconect.Reset();
}

void ChatClient::Release()
{
	Disconnect1();

	if (client_worker.IsWorking())
	{
		en_core.TraceTo("Chat", "ChatClient::Release: stopping client_worker");
		client_worker.Stop();
	}

	while (client_worker.IsWorking())
	{
		en_core.TraceTo("Chat", "ChatClient::Release: waiting for client_worker");
		ThreadWorker::Sleep(100);
	}

	if (client_worker.client_impl)
	{
		delete client_worker.client_impl;
		client_worker.client_impl = NULL;
	}

	en_core.TraceTo("Chat", "ChatClient::Release");
}

void ChatClient::Work(float dt, int level)
{
	if (!on_conncet_called && client_worker.connected)
	{
		on_conncet_called = true;

		en_core.TraceTo("Chat", "ChatClient::Work: call OnConnect");
		core->Script()->Machine()->Call(&callback_connect);			
	}

	if (call_on_error && !on_error_called)
	{
		on_error_called = true;

		en_core.TraceTo("Chat", "ChatClient::Work: call OnError");
		core->Script()->Machine()->Call(&callback_error);			
	}

	if (on_conncet_called && !on_disconncet_called && !client_worker.connected)
	{
		on_disconncet_called = true;

		en_core.TraceTo("Chat", "ChatClient::Work: call OnDisconnect");
		core->Script()->Machine()->Call(&callback_disconect);			
	}

	if (client_worker.ready_for_read)
	{
		for (int i = 0, size = client_worker.new_messages.size(); i < size; ++i)
		{
			IFile* file = core->Files()->OpenFile("chat_message_file", IFile::write);
			file->Write(client_worker.new_messages[i].c_str(), client_worker.new_messages[i].size());
			file->Release();
			
			core->Script()->Machine()->AddStrArg("chat_message_file");
			core->Script()->Machine()->Call(&callback_message);	
		}

		client_worker.new_messages.clear();
		client_worker.ready_for_read = false;
	}
}

void ChatClient::Connect1(std::string& set_url, int set_port)
{
	en_core.TraceTo("Chat", "ChatClient::Connect1");

	if (client_worker.IsWorking())
	{
		en_core.TraceTo("Chat", "ChatClient::Connect1: client_worker is already working");
		return;
	}

	on_conncet_called = false;
	on_disconncet_called = false;

	url = set_url;
	port = set_port;

	client_worker.Start(ThreadWorker::normal);

	en_core.TraceTo("Chat", "ChatClient::Connect1: client_worker is started");
}

void ChatClient::Disconnect1()
{
	en_core.TraceTo("Chat", "ChatClient::Disconnect1");

	client_worker.client_impl->Disconnect();
	client_worker.connected = false;

	while (client_worker.IsWorking())
	{
		en_core.TraceTo("Chat", "ChatClient::Disconnect1: waiting for client_worker");
		ThreadWorker::Sleep(100);
	}

	en_core.TraceTo("Chat", "ChatClient::Disconnect1: disconnected");
}

void ChatClient::SendMessage1(std::string& txt)
{
	client_worker.client_impl->SendMessage(txt.c_str());
}

void ChatClient::SendCommand1(std::string& txt)
{
	client_worker.SendCommand(txt.c_str());
}

int ChatClient::SendFile(std::string& cmd, std::string& path)
{
	IFile* file = core->Files()->OpenFile(path.c_str(), IFile::read);
	if (!file)
	{
		return -1;
	}

	int filesize = file->GetSize();

	if (filesize > 10 * 1024 * 1024)
	{
		return -2;
	}

	byte* buffer = new byte[filesize];
	file->Read(buffer, filesize);
	file->Release();

	NetBlockBuffer message(8192);
	message.PackRAW((const byte*)cmd.c_str(), cmd.length());
	message.PackRAW((const byte*)"&filedata=", ::strlen("&filedata="));
	message.PackBase64(buffer, filesize);
	message.PackBYTE('\0');

	delete[] buffer;

	client_worker.SendCommand((const char*)&message[0]);

	return 0;
}

CALLWRAPPERARG2(ChatClient, Connect1, CALLWRAPPERGETADRESS(string, arg1), CALLWRAPPERGETPARAM(int, arg2), CALLWRAPPERNORET())
CALLWRAPPER(ChatClient, Disconnect1, CALLWRAPPERNORET())
CALLWRAPPERARG1(ChatClient, SendMessage1, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG1(ChatClient, SendCommand1, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG2(ChatClient, SendFile, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERRET(int))

void ChatClient::MethodsToScript(const char* script_class_name)
{
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void Connect(string&in url, int port)", CALLWRAPPERNAMEAS(Connect1));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void Disconnect()", CALLWRAPPERNAMEAS(Disconnect1));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SendMessage(string&in txt)", CALLWRAPPERNAMEAS(SendMessage1));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SendCommand(string&in txt)", CALLWRAPPERNAMEAS(SendCommand1));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "int SendFile(string&in cmd, string&in path)", CALLWRAPPERNAMEAS(SendFile));
}