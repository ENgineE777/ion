#pragma once
#include "Common/Declarations.h"
#include "Common/TaskHolder.h"
#include "Common/Containers/HashMap.h"
#include "Common/Containers/IList.h"
#include "Core/Network/NetworkDataBuffer.h"


class NetLogger:public TaskHoldersOwner
{
public:
    enum EType{eServer,eClient};
    NetLogger(EType type);
    ~NetLogger(){Stop();}
    void  Log(const char* sFile,const char* sMessage);
    void  GetDevicesNames(std::vector<string>& vList);
    bool  ConnectTo(const string& sAddress);
    bool  IsConnectedTo(const string& sAddress){return sAddress == sServerAddress;}
    void  Disconnect();
    void  SendCommand(const string& sCommand){Send(hConnectionFd,"#cmd:" + sCommand);}

    static  float g_nBroadcastTime;
private:
    int             hBroadcastClientSockFd;
    int             hBroadcastServerSockFd;
    int             hServerSockFd;
    int             hConnectionFd;
    string          sServerAddress;
    string          sServerName;
    EType           eType;  
    std::string     sBroadcastMessage;
    std::string     sBroadcastAddress;
    struct hostent *pBroadcastHost;
    float           nIdleTime;

    typedef HashTable<string,string> TServers;

    TServers vServers;

    List<string>  vLogs;

    void        ConnectToAddress(const string& sAddress,int& nSockId);
    bool        GetBroadcastMessage();
    void        BroadCast(float dt, int level);
    void        ScanBroadcast(float dt, int level);
    void        ServerStep(float dt, int level);
    void        ClientStep(float dt, int level);
    void        CloseSocket(int& hSockFd);
    void        UnBlockSocket(int hSockFd);
   
    bool        Send(int hSockFd,const string& sMessage);
    void        Stop();
    void        ParseMessage(const char* vBuffer,int nLength);
    int         BindSocket(const struct addrinfo * pServerInfo);  

    void        CloseClient(); 
    void        CloseServer();   
};