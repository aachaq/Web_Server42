#include "Client.hpp"
#include "ClientData.hpp"
#include "Request.hpp"
#include <sys/select.h>



    Client::Client() : ClientData()
    {
    }

    // Client::Client(CfgServer &Config) : ClientData(Config)
    // {
    // }
    void Client::SetData()
    {
        Request.Data = this;
        Response.Data = this;
    }
    
    // void Client::Set(CfgServer &Config)
    // {
    //     this->Config = Config;
    // }

    Client::Client(const Client &obj) : ClientData(obj)
    {
        *this = obj;
    }

    Client & Client::operator=(const Client &obj)
    {
        if( this != &obj)
        {
            ClientData::operator=(obj);
            Request = obj.Request;
            Response = obj.Response;
        }
        return *this;
    }
    
    Client::~Client()
    {
        
    }

    int Client::ReadOrWriteOrNone(const fd_set &ReadFDs,const fd_set &WriteFDs)
    {
        
        if(FD_ISSET(SocketFd, &ReadFDs))
            return 0;
        if(FD_ISSET(SocketFd, &WriteFDs))
            return 1;
        return -1;
    }





