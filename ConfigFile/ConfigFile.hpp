 #ifndef CONFIGFILE__HPP
#define CONFIGFILE__HPP

#include "CfgServer.hpp"
#include <fstream>

struct ConfigFile
{
    std::string Name;
    bool OnBlock;
    int TypeBlock;
    std::vector<CfgServer> Server;
    std::map<std::pair<std::string ,std::string>, std::vector<int> > MServers;
    VEC_T Tokens;
    std::ifstream *CfgFile;

    ConfigFile(std::string FileName);
    ~ConfigFile();
    ConfigFile(const ConfigFile &obj);
    ConfigFile &operator=(const ConfigFile &obj);
    ConfigFile();

    void Parse();
    void GenerateTokens();
    void Set();
    void GetSameServersForLisren(size_t idx,std::string name,std::pair<std::string ,std::string>Listen);


    void Print();
    void PrintTokens();
    
    void test();
}; 

#endif