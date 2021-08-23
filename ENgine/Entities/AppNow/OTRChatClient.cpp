
#include "OTRChatClient.h"

#include "OTRChatClientImpl.h"

#ifdef PC
#include "Core/Core.h"
#endif


ENTITY(OTRChatClient)
ENTITY_PARAMS(OTRChatClient, "OTRChatClient", "AppNow", "OTRChatClient")

OTRChatClientWorker::OTRChatClientWorker() : connected(false), ready_for_read(false), command_pool(32)
{
}

void OTRChatClientWorker::Prepare()
{
}

void OTRChatClientWorker::Loop()
{
	if (client_impl->Connect(client->url, client->port))
	{
		while (!NeedExit())
		{
			if (client_impl->IsDisconnected())
			{
				connected = false;
				break;
			}

			if (ready_for_read)
			{
				ThreadWorker::Sleep(100);
				continue;
			}

			OTRState chat_state = client_impl->GetState();

			if (chat_state == kOTRProcessMessages)
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

			if (chat_state == kOTRProcessMessages)
			{
				connected = true;
			}
			else if (chat_state == kOTRError)
			{
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
		client_impl->Disconnect();

		client->call_on_error = true;
		client->on_error_called = false;
	}
}

void OTRChatClientWorker::SendCommand(const char* cmd)
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

void OTRChatClientWorker::SendCommand(const std::string& cmd)
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

void OTRChatClient::RegisterProperties()
{
	SceneEntity::RegisterProperties();

	PROP_REG( func_on_connect )	
	PROP_REG( func_on_message )
	PROP_REG( func_on_error )
	PROP_REG( func_on_disconnect )
}

void OTRChatClient::Init()
{				
	on_conncet_called = false;
	on_disconncet_called = false;

	client_worker.client = this;
	client_worker.client_impl = new OTRChatClientImpl;

	SetUpdate((OTRChatClient::Task)&OTRChatClient::Work, lvl_work1);
}

void OTRChatClient::ReInit()
{
#if defined(PC) || defined(OSUNIX)
#endif
}

bool OTRChatClient::Start()
{
	ScriptFunction::Find(&callback_connect, Scene()->ScriptModule(), func_on_connect.c_str());
	ScriptFunction::Find1S(&callback_message, Scene()->ScriptModule(), func_on_message.c_str());
	ScriptFunction::Find(&callback_error, Scene()->ScriptModule(), func_on_error.c_str());
	ScriptFunction::Find(&callback_disconect, Scene()->ScriptModule(), func_on_disconnect.c_str());

	return true;
}

void OTRChatClient::Finish()
{
	callback_connect.Reset();
	callback_message.Reset();
	callback_error.Reset();
	callback_disconect.Reset();
}

void OTRChatClient::Release()
{
	Disconnect();

	if (client_worker.IsWorking())
	{
		client_worker.Stop();
	}

	while (client_worker.IsWorking())
	{
		ThreadWorker::Sleep(100);
	}

	if (client_worker.client)
	{
		delete client_worker.client_impl;
		client_worker.client_impl = NULL;
	}
}

void OTRChatClient::Work(float dt, int level)
{
	if (!on_conncet_called && client_worker.connected)
	{
		on_conncet_called = true;

		core->Script()->Machine()->Call(&callback_connect);
	}

	if (call_on_error && !on_error_called)
	{
		on_error_called = true;

		core->Script()->Machine()->Call(&callback_error);			
	}

	if (on_conncet_called && !on_disconncet_called && !client_worker.connected)
	{
		on_disconncet_called = true;

		core->Script()->Machine()->Call(&callback_disconect);			
	}

	if (client_worker.ready_for_read)
	{
		for (int i = 0, size = client_worker.new_messages.size(); i < size; ++i)
		{
			IFile* file = core->Files()->OpenFile("otr_chat_message_file", IFile::write);
			file->Write(client_worker.new_messages[i].c_str(), client_worker.new_messages[i].size());
			file->Release();			

			core->Script()->Machine()->AddStrArg("otr_chat_message_file");
			core->Script()->Machine()->Call(&callback_message);	

			// TODO: Remove otr_chat_message_file
		}

		client_worker.new_messages.clear();
		client_worker.ready_for_read = false;
	}
}

void OTRChatClient::Connect(std::string& set_url, int set_port)
{
	if (client_worker.IsWorking())
	{
		return;
	}

	on_conncet_called = false;
	on_disconncet_called = false;

	url = set_url;
	port = set_port;

	client_worker.Start(ThreadWorker::normal);
}

void OTRChatClient::Disconnect()
{
	client_worker.client_impl->Disconnect();
	client_worker.connected = false;

	while (client_worker.IsWorking())
	{
		ThreadWorker::Sleep(100);
	}
}

void OTRChatClient::SendMessage(std::string& txt)
{
	client_worker.client_impl->SendMessage(txt.c_str());
}

void OTRChatClient::SendCommand(std::string& txt)
{
	client_worker.SendCommand(txt);
}

int OTRChatClient::SendFile1(std::string& cmd, std::string& path)
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

CALLWRAPPERARG2(OTRChatClient, Connect, CALLWRAPPERGETADRESS(string, arg1), CALLWRAPPERGETPARAM(int, arg2), CALLWRAPPERNORET())
CALLWRAPPER(OTRChatClient, Disconnect, CALLWRAPPERNORET())
CALLWRAPPERARG1(OTRChatClient, SendMessage, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG1(OTRChatClient, SendCommand, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG2(OTRChatClient, SendFile1, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERRET(int))

void OTRChatClient::MethodsToScript(const char* script_class_name)
{
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void Connect(string&in url, int port)", CALLWRAPPERNAMEAS(Connect));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void Disconnect()", CALLWRAPPERNAMEAS(Disconnect));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SendMessage(string&in txt)", CALLWRAPPERNAMEAS(SendMessage));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SendCommand(string&in txt)", CALLWRAPPERNAMEAS(SendCommand));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "int SendFile(string&in cmd, string&in path)", CALLWRAPPERNAMEAS(SendFile1));
}