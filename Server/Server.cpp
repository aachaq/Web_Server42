#include "Server.hpp"
#include <cstring>
#include <ctime>
#include <netdb.h>
#include <sys/select.h>
#include <vector>

Server::Server()
{
    id = -1;
    SocketFd = -1;
    MaxFd = -1;
}

Server::Server(int id ,std::pair<std::string, std::string> Listen,std::vector<CfgServer> Configs)
{
    this->id= id;
    SocketFd = -1;
    MaxFd = -1;
    this->Listen = Listen; 
    this->Configs = Configs;      
}

Server::Server(const Server &obj)
{
    *this = obj;
}



Server &Server::operator=(const Server &obj)
{
    if(this != &obj)
    {
        id = obj.id;
        SocketFd = obj.SocketFd;
        MaxFd = obj.MaxFd;
        Config = obj.Config;
        Listen = obj.Listen;
        Configs = obj.Configs;
        Clients = obj.Clients;
    }
    return *this;
}

Server::~Server()
{

}

void Server::Run()
{
    struct addrinfo Hints, *Res;
    memset(&Hints, 0, sizeof(Hints));
    Hints.ai_family = AF_UNSPEC;
    Hints.ai_socktype = SOCK_STREAM;

    if (0 != getaddrinfo(Listen.first.c_str(), Listen.second.c_str(), &Hints, &Res)) 
    {
        throw Error(DEBUGE + " getaddrinfo ");
        return;
    }
    struct addrinfo *Ptr = NULL;
    int err = 0;
    for (Ptr = Res; Ptr != NULL; Ptr = Ptr->ai_next)
    {
        SocketFd = socket(Ptr->ai_family,Ptr->ai_socktype,0);
        if (SocketFd == -1)
            continue;
        int optval = 1;
        if(-1 == setsockopt(SocketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))  
        {
            close(SocketFd);
            SocketFd =-1;
            continue;
        }
        err = bind(SocketFd, Ptr->ai_addr, Ptr->ai_addrlen); 
        if (err == -1)
        {
            close(SocketFd);
            continue;
        }
        break;
    }
    freeaddrinfo(Res);
    if(err == -1 )
        throw Error(DEBUGE + " binding socket");
    if(SocketFd == -1)
        throw Error(DEBUGE + " socket()");

    if (listen(SocketFd, FD_SETSIZE) == -1) 
    {
        close(SocketFd);
        throw Error(DEBUGE + " listening on socket");
    }
}

void Server::SetReadfd_set(fd_set &strc)
{
    if(Clients.empty())
    {
        return;
    }
    for(std::vector<Client>::iterator it = Clients.begin(); it != Clients.end();it++)
    {
        if(it->Request.ReadyHandleReq)
            continue;
        FD_SET(it->SocketFd, &strc);
        if(it->SocketFd > MaxFd)
            MaxFd = it->SocketFd;
    }
}

void Server::SetWritefd_set(fd_set &strc)
{
    if(Clients.empty())
    {
        return;
    }
    for(std::vector<Client>::iterator it = Clients.begin(); it != Clients.end();it++)
    {
        if(!it->Request.ReadyHandleReq)
            continue;
        FD_SET(it->SocketFd, &strc);
        if(it->SocketFd > MaxFd)
            MaxFd = it->SocketFd;
    }
}