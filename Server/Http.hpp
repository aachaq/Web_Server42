#ifndef Http__hpp
#define Http__hpp


// #include "Client.hpp"
#include "Server.hpp"
#include <sys/select.h>


struct Http
{

    int nfds;
    int i;
    std::vector<Server> Servers;
    
    fd_set ReadFDs;
    fd_set WriteFDs;
    
    Http(std::string FileName);
    Http(const Http &obj);
    Http &operator=(const Http &obj);
    Http();
    ~Http();

    void RunServers();
    void core();
    void AddClient(int NumServer);
    void IOmultiplexer();
    void Setfd_set();
};


#endif