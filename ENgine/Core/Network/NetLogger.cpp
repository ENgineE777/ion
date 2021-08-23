
#ifdef PC
#include <WinSock2.h>
#include <ws2tcpip.h>
#endif

#ifndef PC
#include <errno.h>
#include <unistd.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>

#ifndef IOS

#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#endif

#include <netdb.h>
#include <fcntl.h> 
#endif

#include "../ICore.h"
#include "NetLogger.h"
#include "Sockets.h"
#include "../Scene/ISceneManager.h"
#include "../Script/ScriptModule.h"



#define BROADCAST_PORT         4950 
#define BROADCAST_PORT_STRING "4950"
#define SERVER_PORT            3490
#define SERVER_PORT_STRING    "3490" 
#define MAXBUFLEN 100

float NetLogger::g_nBroadcastTime = 5.f;


NetLogger::NetLogger(EType type) :hBroadcastClientSockFd(-1),hBroadcastServerSockFd(-1),hServerSockFd(-1),hConnectionFd(-1),eType(type),pBroadcastHost(NULL),nIdleTime(0),vLogs(_FL_)
{
    if(InitSockets())
    {
        if(eType == eServer)
        {
            if(GetBroadcastMessage())
            {
                core->AddTask(this, this, (TaskHolder::Task)&NetLogger::BroadCast, lvl_camera1, g_nBroadcastTime);
                core->AddTask(this, this, (TaskHolder::Task)&NetLogger::ServerStep, lvl_camera1, .25f);
            }
        }
        else
            if(eType == eClient)
            {
                 core->AddTask(this, this, (TaskHolder::Task)&NetLogger::ScanBroadcast, lvl_camera1, g_nBroadcastTime);
			}

    }
}

void NetLogger::Stop()
{
    if(hBroadcastClientSockFd != -1)
        CloseSocket(hBroadcastClientSockFd);
    if(hBroadcastServerSockFd != -1)
        CloseSocket(hBroadcastServerSockFd);
    if(hServerSockFd != -1)
        CloseSocket(hServerSockFd);
    if(hConnectionFd != -1)
        CloseSocket(hConnectionFd);
    vLogs.Clear();
    pBroadcastHost = NULL;
    if(eType == eServer)
    {
            core->DelTask(this,  (TaskHolder::Task)&NetLogger::BroadCast);
            core->DelTask(this,  (TaskHolder::Task)&NetLogger::ServerStep);
    }
    else
        if(eType == eClient)
        {
            core->DelTask(this, (TaskHolder::Task)&NetLogger::ScanBroadcast);
            core->DelTask(this, (TaskHolder::Task)&NetLogger::ClientStep);
        }
}

void NetLogger::CloseSocket(int& hSockFd)
{
#ifdef PC
    closesocket(hSockFd);
#else
    close(hSockFd);
#endif 
    hSockFd = -1;
}

void NetLogger::UnBlockSocket(int hSockFd)
{
#ifdef PC
    u_long param = 1;
    ioctlsocket(hSockFd, FIONBIO, &param); 
#else
    fcntl(hSockFd, F_SETFL, O_NONBLOCK); 
#endif
}

bool NetLogger::Send(int hSockFd,const string& sMessage)
{
    static NetStaticBuffer buffer; bool bRet = true;
    buffer.PackRAW((const byte*)sMessage.c_str(),sMessage.size());
    buffer.PackDWORD(0);
    int nSendBytes  = buffer.GetSize();
    if (SendAll(hSockFd,(const char*)&buffer[0],&nSendBytes) == -1)
    {
        core->TraceTo("app","Error:server failed to send");
        bRet = false;       
    }
    buffer.Reset();
    return bRet;
}

bool NetLogger::GetBroadcastMessage()
{
#if defined(PC)
    char chInfo[64];
    if (!gethostname(chInfo,sizeof(chInfo)))
    {
        hostent *sh;
        sh=gethostbyname((char*)&chInfo);
        if (sh!=NULL)
        {
            int   nAdapter = 0;
            while (sh->h_addr_list[nAdapter])
            {
                struct   sockaddr_in   adr;
                memcpy(&adr.sin_addr,sh->h_addr_list[nAdapter],sh->h_length);
                switch(sh->h_addrtype)
                {
                    case AF_INET:
                    {
                        sBroadcastAddress = inet_ntoa(adr.sin_addr);
                        sBroadcastMessage = string(chInfo) + ":" + sBroadcastAddress;
                        size_t pos  = sBroadcastAddress.find_last_of(".");
                        sBroadcastAddress.erase(pos,sBroadcastAddress.size() - pos);
                        sBroadcastAddress += ".255";
                        core->TraceTo("app","BroadcastMessage is: %s",sBroadcastMessage.c_str());
                        core->TraceTo("app","BroadcastAddres is: %s",sBroadcastAddress.c_str());
                        return true;
                    }
                }
                nAdapter++;
            }
        }
    }
    return false;
#endif
#if defined(UNIX) || defined(ANDROID)
    struct LocalAddrs * pIter = GetLocalAddrs(AF_INET);
    for (; pIter; pIter = pIter->pNext) 
    {
        char sAddress[INET_ADDRSTRLEN];
        if (!inet_ntop(AF_INET, pIter->pAddress, sAddress, sizeof(sAddress))) 
            continue;
        if(!String::IsEqual("127.0.0.1",sAddress))
        {
            sBroadcastAddress = sAddress;
            size_t pos  = sBroadcastAddress.find_last_of(".");
            sBroadcastAddress.erase(pos,sBroadcastAddress.size() - pos);
            sBroadcastAddress += ".255";
            sBroadcastMessage = string(core->info.device_name) + ":" + sAddress;
            core->TraceTo("app","BroadcastMessage is: %s",sBroadcastMessage.c_str());
            core->TraceTo("app","BroadcastAddress is: %s",sBroadcastAddress.c_str());
            return true;
        }
    }
#endif
#if defined(IOS)
    sBroadcastMessage = string(core->info.device_name);
    sBroadcastAddress = "255.255.255.255";
    core->TraceTo("app","BroadcastMessage is: %s",sBroadcastMessage.c_str());
    core->TraceTo("app","BroadcastAddress is: %s",sBroadcastAddress.c_str());
#endif
    
    return true;
}

void NetLogger::ServerStep(float dt, int level)
{
    if(hServerSockFd == -1)
    {
        struct addrinfo hints, *pServerInfo;
        int ret;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE; // èñïîëüçîâàòü ìîé IP
        if ((ret = getaddrinfo(NULL, SERVER_PORT_STRING, &hints, &pServerInfo)) != 0) 
        {
            core->TraceTo("app", "getaddrinfo: %s", gai_strerror(ret));
            return;
        }
        hServerSockFd = BindSocket(pServerInfo);
        freeaddrinfo(pServerInfo);
        if (hServerSockFd == -1)
        {
            core->TraceTo("app","Error: server failed to bind socket");
            return;
        }
#ifdef PC
        char yes ='1';
#else
        int yes = 1;
#endif
        setsockopt(hServerSockFd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(yes));
        UnBlockSocket(hServerSockFd);
        if (listen(hServerSockFd,2) == -1) 
        {
            core->TraceTo("app","Error: server failed to listen socket");
            return;
        }
    }
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len = sizeof(struct sockaddr_storage);
    if(hConnectionFd == -1)
    {
        hConnectionFd = accept(hServerSockFd, (struct sockaddr *)&peer_addr, &peer_addr_len);
        UnBlockSocket(hConnectionFd);
        if (hConnectionFd == -1)return;
        core->DelTask(this,(TaskHolder::Task)&NetLogger::BroadCast);
    }
    else
    {
        if(vLogs.Size() > 0)
        {
            string* pMessage = vLogs.GetFirst();
            if(!Send(hConnectionFd,*pMessage))CloseServer();
            else vLogs.Delete(pMessage);
        }

        int nReceivedBytes = 0;
        static const int nBufferSize = 16384;
        static char vBuffer[nBufferSize];
        if ((nReceivedBytes = recv(hConnectionFd, vBuffer,nBufferSize - 1, 0)) == -1)return;
        if(nReceivedBytes > 0)
            ParseMessage(vBuffer,nReceivedBytes);
    }

}

void NetLogger::ConnectToAddress(const string& sAddress,int& nSockId)
{
    if(nSockId != -1)
       CloseSocket(nSockId);
    struct addrinfo hints, *pServerInfo, *pIter;
    int ret;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if ((ret = getaddrinfo(sAddress.c_str(), SERVER_PORT_STRING, &hints, &pServerInfo)) != 0) 
    {
        core->TraceTo("app", "getaddrinfo: %s", gai_strerror(ret));
        return;
    }
    for(pIter = pServerInfo; pIter != NULL; pIter = pIter->ai_next)
    {
        if ((nSockId = socket(pIter->ai_family, pIter->ai_socktype,pIter->ai_protocol)) == -1) continue;
        if (connect(nSockId,pIter->ai_addr,pIter->ai_addrlen) == -1)
        {
            CloseSocket(nSockId);
            continue;
        }
        break;
    }
    freeaddrinfo(pServerInfo); 
    UnBlockSocket(nSockId);
    if (pIter == NULL)
    {
        core->TraceTo("app","Error: client failed to connect");
        return;
    }
    sServerAddress = sAddress;
    sServerName = vServers[sAddress];
    nIdleTime = 0;
    if(hBroadcastClientSockFd != -1)
        CloseSocket(hBroadcastClientSockFd);
}

void NetLogger::ParseMessage(const char* vBuffer,int nLength)
{
    string sMessage = vBuffer;
    string sHeader,sBody;
    unsigned i = 1;
    for(;i < sMessage.size();++i)
        if(sMessage[i] == ':')break;
        else
            sHeader += sMessage[i];
    if(i + 1 < sMessage.size())
    {
        sBody.assign(&sMessage[i + 1],sMessage.size() - (i + 1));
        if(sHeader == "msg")
        {
            string sFile,sMessageBody;
            unsigned i = 0;
            for(;i < sBody.size();++i)
                if(sBody[i] == ':')break;
                else
                    sFile += sBody[i];
            sMessageBody.assign(&sBody[i + 1],sBody.size() - (i + 1));
            core->TraceTo(sFile.c_str(),"%s",sMessageBody.c_str());
        }
        else 
            if(sHeader == "cmd")
            {  
             //   core->TraceTo("Get  command %s  call %d ",sBody.c_str(),);
                if(String::IsEqual(sBody.c_str(),"close"))
                {
                    if(eType == eServer)CloseServer();
                    else
                        if(eType == eClient)CloseClient();
                }
                else
                    if(eType == eServer && sBody.find("call") != string::npos )
                    {
                        std::vector<string> vParams;
                        string sWord,sCommandBody,sFuncName;
                        sWord.reserve(128);
                        unsigned i = 0;
                        for(;i < sBody.size();++i)
                            if(sBody[i] == ' ')break;
                        sCommandBody.assign(&sBody[i + 1],sBody.size() - (i + 1));
                        bool bGetFuncName = false;
                        for(i = 0;i < sCommandBody.size();++i)
                        {
                            if(sCommandBody[i] == ' ')
                            {
                                if(bGetFuncName)vParams.push_back(sWord);
                                else 
                                {
                                    sFuncName = sWord;
                                    bGetFuncName = true;
                                }
                                sWord.clear();
                            }
                            else
                                sWord += sCommandBody[i];
                        }
                        if(!sWord.empty())
                            if(bGetFuncName)vParams.push_back(sWord);
                            else sFuncName = sWord;

                        sWord = '[';
                        for(i = 0;i < vParams.size();++i)
                        {
                            sWord += ('\"' + vParams[i] + '\"');
                            if(i + 1 <  vParams.size())sWord += ',';
                        }
                        sWord += ']';

                        if (ScriptModule::cur_exec_module)
						{
							ScriptFunction sfunc;			
							ScriptFunction::Find1S(&sfunc, ScriptModule::cur_exec_module, sFuncName.c_str());

							core->Script()->Machine()->AddStrArg(sWord.c_str());
							core->Script()->Machine()->Call(&sfunc);

							sfunc.Reset();							
						}
                    }
            }

    }
}

void NetLogger::ClientStep(float dt, int level)
{
    static const int nBufferSize = 16384;
    static char vBuffer[nBufferSize];
    if(hConnectionFd != -1)
    {
        int nReceivedBytes = 0;
        if ((nReceivedBytes = recv(hConnectionFd, vBuffer,nBufferSize - 1, 0)) == -1)return;
        if(nReceivedBytes > 0)
        {
            nIdleTime = 0;
            ParseMessage(vBuffer,nReceivedBytes);
//            core->TraceTo("app","%s",vBuffer);
        }
        else
            nIdleTime += dt;
    }
    else
        nIdleTime += dt;

    if(nIdleTime  >= 10)
    {
        ConnectToAddress(sServerAddress,hConnectionFd);
        if(hConnectionFd == -1)CloseClient();
    }
}

void NetLogger::BroadCast(float dt, int level)
{
#ifdef PC
    char broadcast = '1';
#else
    int broadcast = 1;
#endif 
    if(hBroadcastServerSockFd == -1)
    {
        if ((hBroadcastServerSockFd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        {
            core->TraceTo("app"," Error: failed to create socket");
            return;
        }

        if (setsockopt(hBroadcastServerSockFd, SOL_SOCKET, SO_BROADCAST, &broadcast,sizeof(broadcast)) == -1) // ýòîò âûçîâ ïîçâîëÿåò ïîñûëàòü øèðîêîâåùàòåëüíûå ïàêåòû:
        {
            core->TraceTo("app","Error: failed to setsockopt");
            hBroadcastServerSockFd = -1;
            return;
        }
    }
    if(pBroadcastHost == NULL)
    {
        if ((pBroadcastHost = gethostbyname(sBroadcastAddress.c_str())) == NULL) // ïîëó÷èòü èíôîðìàöèþ õîñòà
        { 
            core->TraceTo("app"," Error: gethostbyname( %s )",sBroadcastAddress.c_str());
            return;
        }
    }
    struct sockaddr_in address; // àäðåñíàÿ èíôîðìàöèÿ ïîäêëþ÷èâøåãîñÿ
    address.sin_family = AF_INET; 
    address.sin_port = htons(BROADCAST_PORT); 
    address.sin_addr = *((struct in_addr *)pBroadcastHost->h_addr);
    memset(address.sin_zero,'\0',sizeof address.sin_zero);
    static NetStaticBuffer buffer;
    buffer.PackRAW((const byte*)sBroadcastMessage.c_str(),sBroadcastMessage.size());
    buffer.PackDWORD(0);
    int nSendBytes  = buffer.GetSize();
    if (SendAllTo(hBroadcastServerSockFd,(const char*)&buffer[0],&nSendBytes,(struct sockaddr *)&address, sizeof(address)) == -1) 
         core->TraceTo("app","Error: failed to sendto");
    buffer.Reset();
}

int NetLogger::BindSocket(const struct addrinfo * pServerInfo)
{
    int  hSockFd = -1;
    // öèêë ïî âñåì ðåçóëüòàòàì è ñâÿçûâàíèå ñ ïåðâûì âîçìîæíûì
    for(const struct addrinfo* pIter = pServerInfo; pIter != NULL; pIter = pIter->ai_next)
    {
        if ((hSockFd = socket(pIter->ai_family, pIter->ai_socktype,pIter->ai_protocol)) == -1) continue;
        if (bind(hSockFd, pIter->ai_addr, pIter->ai_addrlen) == -1) 
        {
            CloseSocket(hSockFd);
            continue;
        }
        break;
    }
    return hSockFd;
}

void NetLogger::ScanBroadcast(float dt, int level)
{
    if(hBroadcastClientSockFd == -1)
    {
        struct addrinfo hints, *pServerInfo;
        int ret;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags = AI_PASSIVE;
        if ((ret = getaddrinfo(NULL, BROADCAST_PORT_STRING, &hints, &pServerInfo)) != 0) 
        {
            core->TraceTo("app","Error getaddrinfo: %s", gai_strerror(ret));
            return;
        }
        hBroadcastClientSockFd = BindSocket(pServerInfo);
        freeaddrinfo(pServerInfo);
        if (hBroadcastClientSockFd == -1)
        {
            core->TraceTo("app","Error: listener failed to bind socket");
            return;
        }
        UnBlockSocket(hBroadcastClientSockFd);
    }
    char vBuffer[MAXBUFLEN];
    char sAddress[INET_ADDRSTRLEN];
    int nReceivedBytes = 0;
    struct sockaddr_storage addres;
    socklen_t addrSize = sizeof(addres);
    if ((nReceivedBytes = recvfrom(hBroadcastClientSockFd, vBuffer, MAXBUFLEN - 1 , 0,(struct sockaddr *)&addres, &addrSize)) == -1) 
        return;
    if(nReceivedBytes > 0)
    {
        inet_ntop(addres.ss_family,&((struct sockaddr_in *)&addres)->sin_addr,sAddress,sizeof(sAddress));
       // core->TraceTo("app","listener: got Name %s from %s",vBuffer,sAddress);
        string sTitle =  string(vBuffer);
        if( sTitle.find_last_of(":") == -1 )
             sTitle += ':' + string(sAddress);
        vServers[sAddress] = sTitle;
    }
}

#define  MAX_LOG_STRINGS 1024
void NetLogger::Log(const char* sFile,const char* sMessage)
{
    if(eType == eServer)
    {
        if(vLogs.Size() >= MAX_LOG_STRINGS)
            vLogs.Delete(vLogs.GetFirst());
        string* pElement =  vLogs.Add();
        pElement->assign("#msg:" + string(sFile) +":" + sMessage);
    }
}

void NetLogger::GetDevicesNames(std::vector<string>& vList)
{
    TServers::Iterator it = vServers.Begin();
    while(it != vServers.End())
    {
        vList.push_back(*(*it));
        ++it;
    }
}

bool NetLogger::ConnectTo(const string& sAddress)
{
    if(eType == eClient)
    {
        int hSockId =  -1;
        ConnectToAddress(sAddress,hSockId);
        if(hSockId != -1)
        {
            if(hConnectionFd != -1)
                CloseClient();
            hConnectionFd = hSockId;
            core->DelTask(this, (TaskHolder::Task)&NetLogger::ScanBroadcast);
            core->AddTask(this, this, (TaskHolder::Task)&NetLogger::ClientStep, lvl_camera1,.05f);
            return true;
        }
    }
    return false;
}



void NetLogger::Disconnect()
{
   if(eType == eClient)
   {
       if(hConnectionFd != -1)
           CloseClient();
   }
   else
       if(eType == eServer)
       {
           if(hConnectionFd != -1)
               CloseServer();
       }
}

void NetLogger::CloseClient()
{
    if(hConnectionFd != -1)
    {
        Send(hConnectionFd,"#cmd:close");
        CloseSocket(hConnectionFd);
    }
    sServerAddress.clear();
    sServerName.clear();
    core->DelTask(this, (TaskHolder::Task)&NetLogger::ClientStep);
    core->AddTask(this, this, (TaskHolder::Task)&NetLogger::ScanBroadcast, lvl_camera1,g_nBroadcastTime);

}

void NetLogger::CloseServer()
{
    if(hConnectionFd != -1)
    {
        Send(hConnectionFd,"#cmd:close");
        CloseSocket(hConnectionFd);
    }
    core->DelTask(this,(TaskHolder::Task)&NetLogger::ServerStep);
    core->AddTask(this, this, (TaskHolder::Task)&NetLogger::BroadCast, lvl_camera1, g_nBroadcastTime);
}






