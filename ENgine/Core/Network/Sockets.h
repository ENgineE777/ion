#pragma once

extern bool InitSockets();
extern void ReleaseSockets();
extern int  SendAll(int hSockFd,const char *pBuff, int *pSize); 
extern int  SendAllTo(int hSockFd,const char *pBuff, int *pSize,struct sockaddr * pAddress,int nAddressSize );
#if defined(UNIX) || defined(ANDROID)

struct LocalAddrs 
{
    struct LocalAddrs * pNext;
    char                pAddress[0];
};
/*
 * Look up the address of the outgoing interface for destination @addr for
 * the given protocol @family.  This address is the most likely address
 * under which this host is reachable from @addr on a multihomed machine.
 * Copy the outgoing address into @outgoing.
 */
extern int GetOutgoingAddr(int family, void *addr, void *outgoing);
/*
 * Determine all the local addresses this host has for the given protocol
 * @family. The result is a linked list of addresses allocated with malloc.
 * The addresses are 4 bytes each for @family AF_INET and 16 bytes each for
 * @family AF_INET6, in network byte order as returned by gethostbyname2.
 */
extern struct LocalAddrs* GetLocalAddrs(int family);
#endif