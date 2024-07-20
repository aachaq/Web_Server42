#include "ClientData.hpp"
#include <sched.h>


ClientData::ClientData()
{
    SocketFd = -1;
    StartLineIsDone = false;
    Method = "Not Supported";
    Url ="";
    Protocl = "Not Supported";
    Cookie ="";
    BodyTmpFileName = "./InternalUse/TMP_";
    UploadFileName = "";
    IsFirstOpen = true;
    ContentLength = -1;
    ContentType = "";
    Query = "";
    ErrorCode = -1;

}


ClientData::ClientData(const ClientData &obj)
{
    *this = obj;
}

ClientData & ClientData::operator=(const ClientData &obj)
{
    if(this != &obj)
    {
        Config = obj.Config;
        SocketFd = obj.SocketFd;
        StartLineIsDone = obj.StartLineIsDone;
        Method = obj.Method;
        Url = obj.Url;
        Protocl = obj.Protocl;
        Cookie = obj.Cookie;
        BodyTmpFileName = obj.BodyTmpFileName;
        UploadFileName = obj.UploadFileName;
        IsFirstOpen = obj.IsFirstOpen;
        ContentLength = obj.ContentLength;
        ContentType = obj.ContentType;
        Query = obj.Query;
        ErrorCode = obj.ErrorCode;
        Headers = obj.Headers;
        Configs = obj.Configs;

    }
    return *this;
}

ClientData::~ClientData()
{

}
