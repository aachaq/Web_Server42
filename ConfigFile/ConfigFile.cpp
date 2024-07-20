#include "ConfigFile.hpp"
#include "CfgServer.hpp"
#include "Utiles.hpp"
#include <fstream>

/*  MEMBER FUNCTIONS OF CONFIGFILE */
ConfigFile::ConfigFile()
{
    Name = "";
    OnBlock = false;
    TypeBlock = MAIN_B;
    CfgFile = NULL;

}

ConfigFile::ConfigFile(std::string FileName)
{
    Name = FileName;
    OnBlock = false;
    TypeBlock = MAIN_B;
    CfgFile = new std::ifstream(FileName.c_str());
    if(!CfgFile->is_open())
    {
        delete CfgFile;
        throw Error(DEBUGE+" : opening of file fail !!!");
    }
}

ConfigFile::ConfigFile(const ConfigFile &obj)
{
    *this = obj;
}

ConfigFile &ConfigFile::operator=(const ConfigFile &obj)
{
    if(this != &obj)
    {
        Name = obj.Name;
        OnBlock = obj.OnBlock;
        TypeBlock = obj.TypeBlock;
        Server = obj.Server;
        MServers = obj.MServers;
        Tokens = obj.Tokens;
        CfgFile = obj.CfgFile;
    }
    return *this;
}

void ConfigFile::GenerateTokens()
{
    std::string specail(" \t{};");
    std::string line;

    while(std::getline(*CfgFile, line))
    {
        TrimLine(line," \t", " \t");
        if(line.empty() || line[0] == '#')
            continue;
        std::size_t pos = line.find_first_of("#");
        if(pos != std::string::npos)
            line.erase(pos);
        bool f = true;
        std::string s("");
        for (std::string::iterator it = line.begin(); it != line.end(); it++)
        {
            if(f == true || it == line.begin())
                f = false;
            if(specail.find(*it) != std::string::npos)
            {
                if(!s.empty())
                {
                    Tokens.push_back(s);
                    s= "";
                }
                f = true;
                if(*it != ' ' && *it != '\t')
                    Tokens.push_back(std::string(it, it + 1));
            }
            else
                s += *it;
        }
        if(!s.empty())
            Tokens.push_back(s);
    }
}


void ConfigFile::Parse()
{
    COUT << BLU << ">>>>>>>>>>>>>>>> Parsing >>>>>>>>>>>>>>>>" << RESET << ENDL;

    VEC_T::iterator  end = Tokens.end();
    for(VEC_T::iterator start = Tokens.begin(); start < end ;start++ )
    {
        if(*start != "server")
            throw Error(DEBUGE +"[ " + *start + " ] no [ server ] section in configuration");
        start++;
        Server.push_back(CfgServer(start, end));
    }
    
    COUT << BLU << ">>>>>>>>>>>>>>>> ======= >>>>>>>>>>>>>>>>" << RESET << ENDL;
}

void  ConfigFile::GetSameServersForLisren(std::size_t idx,std::string name,std::pair<std::string,std::string> Listen)
{
    (void) name;
    for(std::size_t size = Server.size(),i = 0; i < size; i++)
    {
        if(i == idx)
            continue;
        if(Server[i].ListenPort.second == Listen.second)
            throw Error(DEBUGE +"Same [ port ]");
        // if(Server[i].ListenPort.first == Listen.first && Server[i].ListenPort.second == Listen.second && Server[i].ServerName == name)
        //     throw Error(DEBUGE +"Same [ server_name ] and [ listen ]");
    }
}

void ConfigFile::Set()
{
    COUT << BLU << ">>>>>>>>>>>>>>>> setting >>>>>>>>>>>>>>>>" << RESET << ENDL;
    std::size_t size = Server.size();
    for(std::size_t i = 0; i < size; i++)
    {
        Server[i].Set();
        Server[i].valid();
    }


    for(std::size_t i = 0; i < size; i++)
    {
        GetSameServersForLisren(i , Server[i].ServerName,Server[i].ListenPort);
        MServers[Server[i].ListenPort].push_back(i);
    }

    
    COUT << BLU << ">>>>>>>>>>>>>>>> ======= >>>>>>>>>>>>>>>>" << RESET << ENDL;
}


void ConfigFile::Print()
{
    COUT << BLU << ">>>>>>>>>>>>>>>> printing >>>>>>>>>>>>>>>>" << RESET << ENDL;
    for(std::vector<CfgServer>::iterator it = Server.begin(); it != Server.end() ; it++)
    {
        COUT << "++++++++++++++++++++++++++++++++++" << ENDL;
        it->Print();
        COUT << "++++++++++++++++++++++++++++++++++" << ENDL;
    }
    COUT << BLU << ">>>>>>>>>>>>>>>> ======= >>>>>>>>>>>>>>>>" << RESET << ENDL;
}


void ConfigFile::PrintTokens()
{
    COUT << RED << "[[[[[[[[[[[[ Tokens ]]]]]]]]]]]]" << RESET << ENDL;
    {
        int i = 0;
        for (VEC_T::iterator it = Tokens.begin(); it < Tokens.end() ; it++, i++)
            COUT << BLU << "[" << i << "][" << *it << "]"<< RESET << ENDL;

    }
    for(size_t i = 0;i < Server.size(); i++)
    {
        COUT << RED << "[[[[[[[[[[[[ Server ["<< i <<"] ]]]]]]]]]]]]" << RESET << ENDL;
        Server[i].Print();
        COUT << RED << "[[[[[[[[[[[[ {End Server} ]]]]]]]]]]]]" << RESET << ENDL;
    }
}


void ConfigFile::test()
{
    COUT << ">>>>>>>>>>>>>>>>>>>>> begin TEST" << ENDL;
    this->Set();
    this->Print();
    COUT << ">>>>>>>>>>>>>>>>>>>>> end  TEST" << ENDL;
}

ConfigFile::~ConfigFile()
{
    if(CfgFile->is_open())
        CfgFile->close();
    delete CfgFile;
}