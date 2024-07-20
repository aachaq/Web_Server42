#include "CfgServer.hpp"
#include "CfgLocation.hpp"
#include "GlobGet.hpp"
#include "Utiles.hpp"
#include <climits>

CfgServer::CfgServer():GlobGet()
{
    Isdefault = false;
    IsChecked = false;
    Listen = "";
    ListenCount = 0;
    Port= "";
    PortCount = 0;
    ServerName="";
    ServerNameCount = 0;
    ClientMaxBodySizeCount = 0;
    ClientMaxBodySize = 1024;
    ListenPort.first = "";
    ListenPort.second = "";

}

CfgServer::CfgServer(const CfgServer &obj):GlobGet(obj)
{
    *this = obj;
}

CfgServer   &CfgServer::operator=(const CfgServer &obj)
{
    if(this != &obj)
    {
        GlobGet::operator=(obj);
        IsChecked = obj.IsChecked;
        Isdefault = obj.Isdefault;
        Listen = obj.Listen;
        ListenCount = obj.ListenCount;
        Port = obj.Port;
        PortCount = obj.PortCount;
        ServerName = obj.ServerName;
        ServerNameCount = obj.ServerNameCount;
        ClientMaxBodySizeCount = obj.ClientMaxBodySizeCount;
        ClientMaxBodySize = obj.ClientMaxBodySize;
        ListenPort = obj.ListenPort;
        ErrorPage = obj.ErrorPage;
        Location = obj.Location;   
    }
    return  *this;
}

CfgServer::CfgServer(VEC_T::iterator &start, VEC_T::iterator &end) : GlobGet("server")
{
    if(*start != "{")
        throw Error(DEBUGE+" : directive [ server ] has no opening [ { ]");
    for(start++; start != end; start++)
    {
        if(*start == "}")
            break ;
        else if(*start == "location")
            Location.push_back(CfgLocation(++start, end));
        else if(DefineToken(*start, name))
        {
            VEC_T values;
            std::string directive = *start;
            for(start++; start != end; start++)
            {
                if(*start == ";")
                {
                    if(values.empty())
                        throw Error(DEBUGE+" :  invalid number of arguments in [ " + directive +" ]");
                    else
                    {
                        directives.push_back(PAIR_T(directive, values));
                        break;
                    }
                }
                else if(DefineToken(*start, name))
                    throw Error(DEBUGE+" : invalid parameter [ " + *start + " ]");
                values.push_back(*start);
            }
        }
        else {
            if(*start == ";")
                throw Error(DEBUGE+" : unexpected [ ; ]");
            else
                throw Error(DEBUGE+" : unknown directive [ " + *start + " ]") ;
        }
    }
    /////////////////////////////////////////
    Isdefault = false;
    IsChecked = false;
    Listen = "";
    ListenCount = 0;
    Port="";
    PortCount = 0;
    ServerName = "";
    ServerNameCount = 0;
    ClientMaxBodySizeCount = 0;
    ClientMaxBodySize = 1024;
    ListenPort.second = "";
    ListenPort.first= "";
}

void CfgServer::PrintData()
{
    COUT << YEWB<< "<<<<<<< begin config server >>>>>>>"<< RESET << ENDL;
    COUT << YEW << ENDL;
    GlobGet::PrintData();
    COUT << "[ Listen => IP: " << ListenPort.first << " PORT: "<<ListenPort.second<< " ]"<< ENDL;
    COUT << "[ ServerName => "<< ServerName << " ]"<< ENDL;
    

    if (!ErrorPage.empty())
    {
        COUT << "[ Error page => " << ENDL;
        for(std::map<int , std::string>::iterator it = ErrorPage.begin(); it != ErrorPage.end(); it++)
            COUT << "- ERROR CODE = " << it->first << " | ERROR PATH = " << it->second << ENDL;
        COUT<< " ]" << ENDL;
    }
    COUT << "[ " << "ClientMaxBodysize => " << ClientMaxBodySize << "]" << ENDL;
    
    for(std::vector< CfgLocation>::iterator it = Location.begin(); it != Location.end(); it++)
        it->PrintData();
    COUT << RESET << ENDL;
    COUT << YEWB<< "<<<<<<<  end config server  >>>>>>>" << RESET<< ENDL;
}


void CfgServer::Print()
{
    COUT << RED << "[>>>>>] Defaut = "<< (Isdefault ? "true":"false") <<" [<<<<<]" << RESET << ENDL;
    GlobGet::Print();
    for (size_t i = 0; i < Location.size(); i++)
    {
        COUT << RED << "[[[[[[[[[[[[ Location ["<< i <<"] ]]]]]]]]]]]]" << RESET << ENDL;
        Location[i].Print();
        COUT << RED << "[[[[[[[[[[[[ {End Location} ]]]]]]]]]]]]" << RESET << ENDL;
    }
    COUT << RESET << FLUSH;
    COUT << "------------------------" << ENDL;
}


void CfgServer::CheckListenAndSet(VEC_T &value)
{
    if(ListenCount > 1)
        throw Error(DEBUGE +"[ listen ] directive is duplicate");

    if(value.size() != 1)
        throw Error(DEBUGE + "invalid number of arguments in [ listen ] directive");

    ListenCount++;
}


void CfgServer::CheckPort(VEC_T &value)
{  
    if(PortCount > 1)
        throw Error(DEBUGE +"[ port ] directive is duplicate");
    if(value.size() != 1)
        throw Error(DEBUGE + "invalid number of arguments in [ port ] directive");

    if(value[0].find_first_not_of("0123456789")!= std::string::npos)
        throw Error(DEBUGE + "invalid parameter [ " +value[0]+" ]");

    double port = ToLong(value[0], 0, 65535);
    if(port < 0)
    {
        throw Error(DEBUGE + "invalid parameter [ " +value[0]+" ]");
    }
    PortCount++;
}

void CfgServer::CheckeServerName(VEC_T &value)
{ 
    if(value.size() != 1 || ServerNameCount > 1)
        throw Error(DEBUGE + "invalid number of arguments in [ server_name ] directive");
    ServerNameCount++;   
}


void CfgServer::CheckErrorPage(VEC_T &value)
{
    if (value.size() != 2)
        throw Error(DEBUGE + "invalid number of arguments in [ error_page ] directive");

    if(value[0].find_first_not_of("1234567890") != std::string::npos)
        throw Error(DEBUGE + "invalid value [ "+ value[0] + " ] in  [ error_page ] directive");
}
void CfgServer::CheckClientMaxBodySize(VEC_T &value)
{
    ClientMaxBodySizeCount++;
    if(ClientMaxBodySizeCount > 1)
        throw Error(DEBUGE +"[ ClientMaxBodySize ] directive is duplicate");

    if(value.size() != 2)
        throw Error(DEBUGE + "invalid number of arguments in [ client_max_body_size ] directive");
    if(value[0].find_first_of("1234567890") == std::string::npos)
        throw Error(DEBUGE + "invalid value [ "+ value[0] + " ] in  [ client_max_body_size ] directive");

    if(value[1].size() != 1 || value[1].find_first_not_of("BKMG") != std::string::npos)
        throw Error(DEBUGE + "invalid value [ "+ value[1] + " ] in  [ client_max_body_size ] directive");

}



void CfgServer::Set()
{
    for(MAP_T::iterator it = directives.begin(); it != directives.end();it++)
    {
        if(it->first == "listen")
        {
            CheckListenAndSet(it->second);
            Listen = it->second[0];
            ListenPort.first = it->second[0];
        }
        else if (it->first == "port") 
        {
            CheckPort(it->second);
            Port = it->second[0];
            ListenPort.second = it->second[0];
        }
        else if (it->first == "server_name")
        {
            CheckeServerName(it->second);
            ServerName = it->second[0];
        }
        else if(it->first == "error_page")
        {
            CheckErrorPage(it->second);
            int Code = ToLong(it->second[0],MIN_ERROR_CODE, MAX_ERROR_CODE);
            if (Code == -1)
                throw Error(DEBUGE +"value [ "+ it->second[0] + " ] must be between 300 and 599 in [ error_page ] directive");
            if(Code == -2)
                throw Error(DEBUGE +"value [ "+ it->second[0] + " ] must be real number in  [ error_page ] directive");
            ErrorPage.insert(INTSTRING_T(Code, it->second[1]));
        }
        else if(it->first == "client_max_body_size")
        {
            CheckClientMaxBodySize(it->second);
            double Tmp = ToLong(it->second[0], 0, LONG_MAX);
            if(Tmp > 0)
            {
                if(it->second[1] == "G")
                    Tmp *= 1024 * 1024* 1024;
                else if(it->second[1] == "M")
                    Tmp *= 1024 * 1024;
                else if(it->second[1] == "K")
                    Tmp *= 1024;
                if(Tmp < 0 || Tmp > LONG_MAX)
                    throw Error(DEBUGE +"value [ "+ it->second[0] + " " + it->second[1] + " ] must be between 0 and LONG_MAX in  [ client_max_body_size ] directive");
            }
            else 
            {
                if(Tmp == -1)
                    throw Error(DEBUGE +"value [ "+ it->second[0] + " ] must be between 0 and LONG_MAX in  [ client_max_body_size ] directive");
                if(Tmp == -2)
                    throw Error(DEBUGE +"value [ "+ it->second[0] + " ] must be real number in  [ client_max_body_size ] directive");
            }
            ClientMaxBodySize = Tmp;
        }
        else {
            GlobGet::Set(it);
        }
    }

    for(std::vector<CfgLocation>::iterator  it = Location.begin(); it != Location.end(); it++)
        it->Set();
}


bool myfunction (CfgLocation i,CfgLocation j) { return (i.path>j.path); }
bool myfunction (CfgLocation i,CfgLocation j);
void CfgServer::valid()
{
    if(ListenCount != 1)
        throw Error(DEBUGE +" add [ listen ] directive");
    if(PortCount != 1)
        throw Error(DEBUGE +" only one [ port ] directive");

    if(Location.empty())
        throw Error(DEBUGE +" add [ location ] directive");

    for(std::vector<CfgLocation>::iterator  it = Location.begin(); it != Location.end(); it++)
        it->valid();

    GlobGet::valid();


    std::sort(Location.begin(), Location.end(), myfunction);
}