#include "Http.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "../ConfigFile/ConfigFile.hpp"
#include <cstddef>
#include <cstdio>
#include <string>
#include <unistd.h>
#include <vector>


Http::Http(std::string Filename)
{
    i = 0;
    nfds = -1;;

    ConfigFile Configs = ConfigFile(Filename);
    Configs.GenerateTokens();
    Configs.Parse();
    Configs.Set();;
    if(Configs.MServers.empty())
    {
        throw Error(DEBUGE + "internal error config");
    }
    int Id = 0;
    std::map<std::pair<std::string,std::string>, std::vector<int> >::iterator it = Configs.MServers.begin();
    while(it != Configs.MServers.end())
    {
        std::vector<CfgServer> tmp;
        std::size_t size = it->second.size();
        for(std::size_t i = 0; i < size; i++)
            tmp.push_back(Configs.Server[it->second[i]]);
        Servers.push_back(Server(++Id, it->first, tmp));
        it++;
    }
}


Http::Http(const Http &obj)
{
    *this = obj;
}


Http &Http::operator=(const Http &obj)
{
    if(this != &obj)
    {
        i = obj.i;
        nfds = obj.nfds;
        Servers = obj.Servers;
    }
    return *this;
}


Http::Http()
{
    i = 0;
    nfds = 0;
}


Http::~Http()
{

}

#include <signal.h>

void Http::core()
{
    RunServers();
    if(Servers.empty())
    {
        exit(2);
    }
    IOmultiplexer();

}

void Http::RunServers()
{
    FD_ZERO(&ReadFDs);
    FD_ZERO(&WriteFDs);
    if(Servers.empty())
        exit(3);

    for(std::size_t z = 0; z < Servers.size(); /*z++*/)
    {
        try 
        {
            Servers[z].Run();
            if(Servers[z].SocketFd > nfds)
                nfds = Servers[z].SocketFd;
            if(!FD_ISSET(Servers[z].SocketFd, &ReadFDs))
                FD_SET(Servers[z].SocketFd, &ReadFDs);
            if(!FD_ISSET(Servers[z].SocketFd, &WriteFDs))
                FD_SET(Servers[z].SocketFd, &WriteFDs);
        
        }    
        catch(Error &err)
        {
            Servers.erase(Servers.begin()+z);
            continue;
        }
        z++;
    
    }
}

void Http::AddClient(int NumServer)
{
    i++;
    int Accepted_socket = -1;
    sockaddr_in AdsSocket;
    socklen_t AddrLen = sizeof(AdsSocket);
    memset(&AdsSocket, 0, sizeof(AdsSocket));

    Accepted_socket = accept(Servers[NumServer].SocketFd , (struct sockaddr*)&AdsSocket , &AddrLen);
    if (Accepted_socket >= 0)
    {
        Client C;
        C.SocketFd = Accepted_socket;
        C.Configs = Servers[NumServer].Configs;
        // C.ListenAsString = Servers[NumServer].Listen.first + ":" + Servers[NumServer].Listen.second;
        FD_SET(C.SocketFd, &ReadFDs);
        Servers[NumServer].Clients.push_back(C);
        if(C.SocketFd > nfds)
            nfds = C.SocketFd;
    }
}

void Http::Setfd_set()
{
    nfds = 0;
    if(Servers.empty())
    {
        exit(1);
    }

    memset(&ReadFDs, 0, sizeof(ReadFDs));
    memset(&WriteFDs, 0, sizeof(WriteFDs));
    
    FD_ZERO(&ReadFDs);
    FD_ZERO(&WriteFDs);
    for (std::vector<Server>::iterator it = Servers.begin() ; it != Servers.end(); it++) 
    {
        FD_SET(it->SocketFd, &ReadFDs);
        if(it->SocketFd  > nfds)
            nfds = it->SocketFd;

        it->SetReadfd_set(ReadFDs);
        if(it->MaxFd > nfds)
            nfds = it->MaxFd;
        it->MaxFd = -1;

        it->SetWritefd_set(WriteFDs);
        if(it->MaxFd > nfds)
            nfds = it->MaxFd;
        it->MaxFd = -1;
    }
}

#include <sys/ioctl.h>

void Http::IOmultiplexer()
{
    signal(SIGPIPE, SIG_IGN);

    while(1)
    {
        Setfd_set();
        int ToCheck = select(nfds + 1, &ReadFDs,&WriteFDs,NULL,NULL);
        if (ToCheck <= 0)
            continue;
        else
        {
            for( std::size_t i = 0; i < Servers.size(); i++)
            {
                if(FD_ISSET(Servers[i].SocketFd , &ReadFDs))
                {
                    AddClient(i);
                    continue;
                }
                if(Servers[i].Clients.empty())
                {
                    Servers[i].Clients.clear();
                    continue;
                }
                for(std::size_t j = 0; j < Servers[i].Clients.size(); /*j++*/)
                {
                    std::vector<Client>::iterator it = Servers[i].Clients.begin() + j;

                    if(it->Request.DropClient || it->Response.send_done || it->Response.Drop_Client)
                    {
                        it->Request.Data = NULL;
                        it->Response.Data = NULL;
                        close(it->SocketFd);
                        close(it->Response.send_fd);
                        close(it->Request.fd);

                        it = Servers[i].Clients.erase(it);
                        if(it->SocketFd >= nfds)
                            nfds -=1;
                        
                        if(Servers[i].Clients.empty())
                        {
                            Servers[i].Clients.clear();
                            break;
                        }
                        continue;
                    }
                    it->SetData();
                    int X = it->ReadOrWriteOrNone(ReadFDs, WriteFDs);
                    if(0 == X)
                    { 

                        it->Request.ReadAndParse();
                        if(it->ErrorCode != -1)
                        {
                            it->Request.ReadyHandleReq = true;
                        }
                    }
                    else if (1 == X)
                    {
                        it->Response.ft_methods();
                    }
                    it->Request.Data = NULL;
                    it->Response.Data = NULL;
                    j++;
                }
            }
        }
    }  
}


