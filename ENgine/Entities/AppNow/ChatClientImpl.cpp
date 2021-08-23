
#ifdef PC
#include <WinSock2.h>
#endif

#include "ChatClientImpl.h"

#ifndef PC
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <poll.h>
#endif
#include "Core/Network/Sockets.h"

#if defined(OSUNIX) || defined(ANDROID)
#include <unistd.h>
#endif


ChatClientImpl::ChatClientImpl()
	:disconnected(true)
	,state(kChatInit)
	,sockfd(-1)
	,last_message(1024)
{
}

ChatClientImpl::~ChatClientImpl()
{
}

bool ChatClientImpl::Connect(const string& host, int port)
{
	InitSockets();

	if (sockfd != -1)
	{
		return true;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	struct sockaddr_in serv_addr;

	::memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(host.c_str());
    serv_addr.sin_port = htons(port);

    if (::connect(sockfd, (const sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
    {
		state = kChatError;
        return false;
    }

	state = kChatInit;
	disconnected = false;

	return true;
}

void ChatClientImpl::Disconnect()
{
	if (disconnected)
	{
		return;
	}

	disconnected = true;

	if (sockfd != -1)
	{
#ifdef PC
		::closesocket(sockfd);
#else
		::close(sockfd);
#endif
		sockfd = -1;
	}
}

bool ChatClientImpl::ProcessDataStream(std::vector<std::string>& new_messages)
{
	static NetBlockBuffer buffer(1024);
	static byte recv_buffer[16384];

	while (!disconnected)
	{
		if (state == kChatInit)
		{
			::send(sockfd, "?OTRv3?", 7, 0);

			state = kChatProcessMessages;
			break;
		}
		else if (state == kChatError)
		{
			break;
		}
		else if (state >= kChatProcessMessages)
		{
			pollfd poll_fd;
			poll_fd.fd = sockfd;
			poll_fd.events = POLLRDNORM;
			poll_fd.revents = 0;

#ifdef PC
			int res = ::WSAPoll(&poll_fd, 1, 0);
#else
            int res = poll(&poll_fd, 1, 0);
#endif
			if (res < 0)
			{
				state = kChatError;
				continue;
			}

			if ((poll_fd.revents & POLLERR) || (poll_fd.revents & POLLHUP))
			{
				state = kChatError;
				break;
			}

			if (!(poll_fd.revents & POLLRDNORM))
			{
				// Nothing to read
				ThreadWorker::Sleep(50);
				break;
			}

			const int bytes_recved = ::recv(sockfd, (char*)&recv_buffer, sizeof(recv_buffer), 0);

			if (bytes_recved <= 0)
			{
				state = kChatError;
				break;
			}

			for (int i = 0; i < bytes_recved; ++i)
			{
				if (!ProcessChar(recv_buffer[i], buffer))
				{
					break;
				}

				if (state == kChatGotNewMessage)
				{
					state = kChatProcessMessages;
					new_messages.push_back( std::string((const char*)&last_message[0], last_message.GetSize()) );

					buffer.Reset();
					last_message.Reset();
				}
			}
		}

		if (!new_messages.empty())
		{
			break;
		}
	}

	return !new_messages.empty();
}

bool ChatClientImpl::ProcessChar(byte ch, NetStaticBuffer& buffer)
{
	switch (ch)
	{
	case '?':
	case 'O':
	case 'T':
	case 'R':
		{
			if (state == kChatProcessMessages)
			{
				state = kChatReadMessageMarker;
			}

			if (state == kChatReadMessageMarker || state == kChatReadMessageBody)
			{
				buffer.PackBYTE(ch);
			}
			else
			{
				state = kChatProcessMessages;
			}
		}
		break;

	case ':':
	case '|':
		{
			if (state == kChatReadMessageMarker)
			{
				if (buffer.IsEqual("?OTR"))
				{
					if (ch == ':')
					{
						state = kChatReadMessageBody;
						buffer.Reset();
					}
					else if (ch == '|')
					{
						// TODO: Process Fragment Messages
						state = kChatProcessMessages;
					}
				}
				else
				{
					state = kChatProcessMessages;
				}
			}
			else
			{
			}
		}
		break;

	case '.':
		{
			if (state == kChatReadMessageBody)
			{
				if (ProcessMessage(buffer))
				{
					state = kChatGotNewMessage;
				}
			}
			else
			{
				state = kChatProcessMessages;
			}
		}
		break;

	default:
		{
			if (state == kChatReadMessageBody && ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '=' || ch == '/' || ch == '+'))
			{
				buffer.PackBYTE(ch);
			}
			else
			{
				state = kChatProcessMessages;
			}
		}
		break;
	}

	return state != kChatProcessMessages;
}

bool ChatClientImpl::ProcessMessage(const NetStaticBuffer& message)
{
	message.UnpackBase64(last_message);
	return last_message.GetSize() > 0;
}

void ChatClientImpl::SendMessage(const char* message)
{
	SendData("?OTR:", message);
}

void ChatClientImpl::SendCommand(const char* message)
{
	SendData("?OTRCmd:", message);
}

void ChatClientImpl::SendData(const char* header, const char* message)
{
	static NetBlockBuffer buffer(8192);

	if (!message || message[0] == '\0')
	{
		return;
	}

	buffer.ResetToSize(8192);
	
	buffer.PackRAW((const byte*)header, strlen(header));
	buffer.PackBase64((const byte*)message, strlen(message));
	buffer.PackRAW((const byte*)".", 1);

	buffer.Send(sockfd);
}