
#ifdef PC
#include <WinSock2.h>
#endif

#ifndef PC
#include <alloca.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef IOS
#include <malloc.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#endif

#endif

#include "Sockets.h"



int  SendAll(int hSockFd,const char *pBuff, int *pSize)
{
    int total = 0;
    int bytesleft = *pSize; 
    int n;
    while(total < *pSize) 
    {
        n = send(hSockFd, pBuff+total, bytesleft, 0);
        if (n == -1)break;
        total += n;
        bytesleft -= n;
    }
    *pSize = total; 
    return n==-1?-1:0; 
}

int  SendAllTo(int hSockFd,const char *pBuff, int *pSize,struct sockaddr * pAddress,int nAddressSize )
{
    int total = 0;
    int bytesleft = *pSize; 
    int n;
    while(total < *pSize) 
    {
        n = sendto(hSockFd, pBuff+total, bytesleft,0,pAddress,nAddressSize);
        if (n == -1)break;
        total += n;
        bytesleft -= n;
    }
    *pSize = total; 
    return n==-1?-1:0;
}


static bool g_sockets_inited = false;

bool InitSockets()
{
	if (g_sockets_inited)return true;
#ifdef PC
	WSADATA wsaData;
    g_sockets_inited = WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
#else
    g_sockets_inited = true;
#endif
    return g_sockets_inited;
}

void ReleaseSockets()
{
	/*if (!g_sockets_inited)
	{
		return;
	}

	g_sockets_inited = false;

	WSACleanup();*/
}

#if defined(UNIX) || defined(ANDROID)
int NetLinkQuery(int flags, int type, void *pArgument, int pArgumentSize,int (*pFunc)(struct nlmsghdr *))
{
	int nBufferSize;
	static int hSockFd = -1;
	static uint32_t seq;
	struct msghdr MsgHeader;
	struct sockaddr_nl sock;
	struct nlmsghdr *pMsgHeader;
	struct iovec iov;
	int ret = 0;

	memset(&MsgHeader, 0, sizeof(MsgHeader));
	MsgHeader.msg_name = (void *)&sock;
	MsgHeader.msg_namelen = sizeof(sock);
	MsgHeader.msg_iov = &iov;
	MsgHeader.msg_iovlen = 1;

	nBufferSize = NLMSG_LENGTH(pArgumentSize);
	if (nBufferSize < 4096)
		nBufferSize = 4096;
	pMsgHeader = (struct nlmsghdr *)alloca(nBufferSize);
	memset(pMsgHeader, 0, sizeof(*pMsgHeader));
	pMsgHeader->nlmsg_len = NLMSG_LENGTH(pArgumentSize);
	pMsgHeader->nlmsg_flags = flags;
	pMsgHeader->nlmsg_type = type;
	pMsgHeader->nlmsg_seq = ++seq;
	memcpy(NLMSG_DATA(pMsgHeader), pArgument, pArgumentSize);

	memset(&sock, 0, sizeof(sock));
	sock.nl_family = AF_NETLINK;

	iov.iov_base = pMsgHeader;
	iov.iov_len = NLMSG_LENGTH(pArgumentSize);

	if (hSockFd < 0) 
    {
		hSockFd = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
		if (hSockFd < 0)
			return -1;
	}
	pArgumentSize = sendmsg(hSockFd, &MsgHeader, 0);
	if (pArgumentSize < 0)
		return -1;

	iov.iov_len = nBufferSize;
	for(;;)
    {
		do 
        {
			pArgumentSize = recvmsg(hSockFd, &MsgHeader, 0);
		} 
        while (pArgumentSize < 0 && errno == EINTR);

		for (pMsgHeader = (struct nlmsghdr *)iov.iov_base; NLMSG_OK(pMsgHeader, pArgumentSize);pMsgHeader = NLMSG_NEXT(pMsgHeader, pArgumentSize)) 
        {
			if (pMsgHeader->nlmsg_seq != seq)
				continue;
			switch(pMsgHeader->nlmsg_type) 
            {
				case NLMSG_ERROR:
				case NLMSG_OVERRUN:errno = EIO;return -1;
				case NLMSG_DONE:return ret;
				default:
					if (pFunc) 
                    {
						ret = pFunc(pMsgHeader);
						if (ret) 
                        {
							if (flags & NLM_F_ROOT)
								pFunc = NULL;
							else
								return ret;
						}
					}
			}
		}
	}
}

static void *g_pOutgoingAddr;

int OutgoingAddrFunc(struct nlmsghdr *pMsgHeader)
{
	struct rtmsg *pMessage = (struct rtmsg *)NLMSG_DATA(pMsgHeader);
	int len = RTM_PAYLOAD(pMsgHeader);
	struct rtattr *pAttribute;
	if (pMsgHeader->nlmsg_type != RTM_NEWROUTE)return -1;
	for (pAttribute = RTM_RTA(pMessage); RTA_OK(pAttribute, len); pAttribute = RTA_NEXT(pAttribute, len)) 
    {
		if (pAttribute->rta_type == RTA_PREFSRC)
        {
			g_pOutgoingAddr = RTA_DATA(pAttribute);
			return 1;
		}
	}
	return 0;
}

/*
 * Look up the address of the outgoing interface for destination @addr for
 * the given protocol @family.  This address is the most likely address
 * under which this host is reachable from @addr on a multihomed machine.
 * Copy the outgoing address into @outgoing.
 */
int GetOutgoingAddr(int family, void *pAddr, void *pOutgoing)
{
	int len, addr_len;
	struct
    {
		struct rtmsg rm;
		struct rtattr ra;
		char pAddr[16];
	} arg;

	switch(family) 
    {
	case AF_INET:
		addr_len = 4;
		break;
#ifdef WITH_IPV6
	case AF_INET6:
		addr_len = 16;
		break;
#endif
	default:
		return -1;
	}

	memset(&arg, 0, sizeof(arg));
	arg.rm.rtm_family = family;
	arg.ra.rta_len = RTA_LENGTH(addr_len);
	arg.ra.rta_type = RTA_DST;
	memcpy(RTA_DATA(&arg.ra), pAddr, addr_len);

	len =  NLMSG_ALIGN(sizeof(struct rtmsg)) + RTA_LENGTH(addr_len);
	if (NetLinkQuery(NLM_F_REQUEST, RTM_GETROUTE, &arg, len,OutgoingAddrFunc) != 1)
		return -1;
	memcpy(pOutgoing, g_pOutgoingAddr, addr_len);
	return 0;
}


static struct LocalAddrs *g_pLocalAddrs;

int GetLocalAddrIpv4Func(struct nlmsghdr *pMsgHeader)
{
	struct ifaddrmsg *pAddresMsg = (struct ifaddrmsg *)NLMSG_DATA(pMsgHeader);
	int len = IFA_PAYLOAD(pMsgHeader);
	struct rtattr *pAttribute;

	if (pMsgHeader->nlmsg_type != RTM_NEWADDR)
		return -1;
	for (pAttribute = IFA_RTA(pAddresMsg); RTA_OK(pAttribute, len); pAttribute = RTA_NEXT(pAttribute, len)) 
    {
		if (pAttribute->rta_type == IFA_LOCAL)
        {
			struct LocalAddrs *pAddr = (struct LocalAddrs *)malloc(sizeof(struct LocalAddrs) + 4);
			if (!pAddr)
				return -1;
			memcpy(pAddr->pAddress, RTA_DATA(pAttribute), 4);
			pAddr->pNext = g_pLocalAddrs;
			g_pLocalAddrs = pAddr;
			return 0;
		}
	}
	return 0;
}

#ifdef WITH_IPV6
int GetLocalAddrIpv6Func(struct nlmsghdr *pMsgHeader)
{
	struct ifaddrmsg *pAddresMsg = NLMSG_DATA(pMsgHeader);
	int len = IFA_PAYLOAD(pMsgHeader);
	struct rtattr *pAttribute;

	if (pMsgHeader->nlmsg_type != RTM_NEWADDR)
		return -1;
	for (pAttribute = IFA_RTA(pAddresMsg); RTA_OK(pAttribute, len); pAttribute = RTA_NEXT(pAttribute, len))
    {
		if (pAttribute->rta_type == IFA_ADDRESS) 
        {
			struct LocalAddrs *pAddr =	malloc(sizeof(struct LocalAddrs) + 16);
			if (!pAddr)
				return -1;
			memcpy(pAddr->pAddress, RTA_DATA(pAttribute), 16);
			pAddr->pNext = g_pLocalAddrs;
			g_pLocalAddrs = pAddr;
			return 0;
		}
	}
	return 0;
}
#endif

/*
 * Determine all the local addresses this host has for the given protocol
 * @family. The result is a linked list of addresses allocated with malloc.
 * The addresses are 4 bytes each for @family AF_INET and 16 bytes each for
 * @family AF_INET6, in network byte order as returned by gethostbyname2.
 */
struct LocalAddrs *GetLocalAddrs(int family)
{
	int len;
	int (*pLocalAddrFunc)(struct nlmsghdr *pMsgHeader);
	struct 
    {
		struct ifaddrmsg ia;
	} arg;

	switch(family) 
    {
	case AF_INET:
		pLocalAddrFunc = GetLocalAddrIpv4Func;
		break;
#ifdef WITH_IPV6
	case AF_INET6:
		pLocalAddrFunc = GetLocalAddrIpv6Func;
		break;
#endif
	default:
		return NULL;
	}
	memset(&arg, 0, sizeof(arg));
	arg.ia.ifa_family = family;
	g_pLocalAddrs = NULL;
	len = NLMSG_ALIGN(sizeof(struct ifaddrmsg));
	if (NetLinkQuery(NLM_F_REQUEST | NLM_F_ROOT, RTM_GETADDR, &arg, len,pLocalAddrFunc) != 0) 
    {
		while (g_pLocalAddrs) 
        {
			struct LocalAddrs *pNext = g_pLocalAddrs->pNext;
			free(g_pLocalAddrs);
			g_pLocalAddrs = pNext;
		}
		return NULL;
	}
	return g_pLocalAddrs;
}

#endif