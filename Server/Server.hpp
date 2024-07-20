#ifndef Server__hpp 
#define Server__hpp


#include "../ConfigFile/CfgServer.hpp"
#include "Client.hpp"
#include <vector>
struct Server
{
    int id;
    int SocketFd;
    int MaxFd;
    CfgServer Config;
    std::pair<std::string, std::string> Listen;
    std::vector<CfgServer> Configs;
    std::vector<Client> Clients;

    
    Server();
    Server(int id,std::pair<std::string, std::string> Listen ,std::vector<CfgServer> Configs);
    Server(const Server &obj);
    Server &operator=(const Server &obj);
    ~Server();
    

    void Run();
    void SetReadfd_set(fd_set &Strct);
    void SetWritefd_set(fd_set &Strct);

};

#endif