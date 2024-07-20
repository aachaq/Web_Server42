#ifndef  ClientData__HPP
#define  ClientData__HPP

#include "../Includes.hpp"

#include "../ConfigFile/ConfigFile.hpp"

struct ClientData
{
    CfgServer Config;
    int SocketFd;
    bool StartLineIsDone;
    std::string Method;
    std::string Url;
    std::string Protocl;
    std::string Cookie;
    std::string BodyTmpFileName;
    std::string UploadFileName;
    bool IsFirstOpen;
    long ContentLength;
    std::string ContentType;
    std::string Query;
    int ErrorCode;
    std::map<std::string, VEC_T> Headers;
    std::vector<CfgServer> Configs;

    ClientData();
    ClientData(const ClientData &obj);
    ClientData & operator=(const ClientData &obj);
    virtual ~ClientData();
    
    // ClientData(CfgServer &Config);

};
#endif