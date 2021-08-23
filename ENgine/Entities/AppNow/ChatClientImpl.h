#pragma once

#include "ChatCommon.h"


enum ChatState
{
	kChatInit,
	kChatError,

	kChatProcessMessages,
	kChatReadMessageMarker,
	kChatReadMessageBody,

	kChatGotNewMessage,
};

class ChatClientImpl
{	
public:
	ChatClientImpl();
	virtual ~ChatClientImpl();

	ChatState GetState() const { return state; }
	bool IsDisconnected() const { return disconnected; }

	bool Connect(const string& host, int port);
	void Disconnect();

	bool ProcessDataStream(std::vector<std::string>& new_messages);

	void SendMessage(const char* message);
	void SendCommand(const char* message);

private:
	ChatClientImpl(const ChatClientImpl&);
	void operator=(const ChatClientImpl&);

	void SendData(const char* header, const char* message);

	bool ProcessMessage(const NetStaticBuffer& buffer);
	bool ProcessChar(byte ch, NetStaticBuffer& buffer);

	bool disconnected;

	ChatState state;
	int sockfd;

	NetBlockBuffer last_message;
};
