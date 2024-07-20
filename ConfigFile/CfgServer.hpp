#ifndef CFGSERVER__HPP
#define CFGSERVER__HPP

#include "CfgLocation.hpp"
#include "Utiles.hpp"

struct CfgServer : GlobGet
{
    bool Isdefault;
    bool IsChecked;
    std::string Listen;
    int ListenCount;
    std::string Port;
    int PortCount;
    std::string ServerName;
    int ServerNameCount;
    int ClientMaxBodySizeCount;
    long ClientMaxBodySize;
    std::pair<std::string ,std::string> ListenPort;
    std::map<int, std::string> ErrorPage;
    std::vector<CfgLocation> Location;


    void CheckListenAndSet(VEC_T &value);
    void CheckPort(VEC_T &value);
    

        
    void CheckeServerName(VEC_T &value);



    CfgServer(VEC_T::iterator &start, VEC_T::iterator &end);
    CfgServer();
    CfgServer(const CfgServer &obj);
    CfgServer   &operator=(const CfgServer &obj);

    void PrintData();
    void Print();
    void Set();

    void CheckErrorPage(VEC_T &value);
    void CheckClientMaxBodySize(VEC_T &value);
    void valid();

};

#endif