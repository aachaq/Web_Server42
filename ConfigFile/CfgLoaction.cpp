#include "CfgLocation.hpp"
#include "GlobGet.hpp"
#include "Utiles.hpp"
// #include <locale>


CfgLocation::~CfgLocation()
{

}
CfgLocation::CfgLocation(): GlobGet()
{
    MethodsToCheck.push_back("GET");
    MethodsToCheck.push_back("POST");
    MethodsToCheck.push_back("DELETE");
    path = "";
    MethodsCount = 0;
    AutoIndexCount = 0;
    AutoIndex = false;
    UploadCount = 0;
    Upload = "";
    RedirectCount = 0;
    Redirect = "";
}

CfgLocation::CfgLocation(const CfgLocation &obj): GlobGet(obj)
{
    *this = obj;
}

CfgLocation &CfgLocation::operator=(const CfgLocation &obj)
{
    if(this != &obj)
    {
        GlobGet::operator=(obj);
        path = obj.path;
        MethodsCount = obj.MethodsCount;
        AutoIndexCount = obj.AutoIndexCount;
        AutoIndex = obj.AutoIndex;
        UploadCount = obj.UploadCount;
        Upload = obj.Upload;
        RedirectCount = obj.RedirectCount;
        Redirect =  obj.Redirect;
        Methods = obj.Methods;
        MethodsToCheck = obj.MethodsToCheck;
        Cgi = obj.Cgi;
    }
    return *this;
}

CfgLocation::CfgLocation(VEC_T::iterator  &start, VEC_T::iterator &end) : GlobGet("location")
{
    if(*start  == "{")
        throw Error(DEBUGE+" : invalid number of arguments in [ location ] directive");
    path = *start;

    start++ ; 
    if(*start != "{")
        throw Error(DEBUGE+"directive [ location ] has no opening [ { ]");
    
    for(start++;start != end; start++)
    {
        if(*start == "}")
            break ;
        else if(*start == "server")
            throw Error(DEBUGE+"[ server ] directive is not allowed here");
        else if(*start == "location")
        {
            throw Error(DEBUGE+"[ loction ] directive is not allowed here");
        }
        else if(DefineToken(*start, name))
        {
            VEC_T values;
            std::string directive = *start;
            for(start++; start != end; start++)
            {
                if(*start == ";")
                {
                    if(values.empty())
                        throw Error(DEBUGE+" invalid number of arguments in [ " + directive +" ]");
                    else
                    {
                        directives.push_back(PAIR_T(directive, values));
                        break;
                    }
                }
                else if(DefineToken(*start, name))
                    throw Error(DEBUGE+"invalid parameter [ " + *start + " ]");
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


    MethodsCount = 0;
    AutoIndexCount = 0;
    AutoIndex = false;
    UploadCount = 0;
    Upload = "";
    RedirectCount = 0;
    Redirect= "";

    MethodsToCheck.push_back("GET");
    MethodsToCheck.push_back("POST");
    MethodsToCheck.push_back("DELETE");
}


void CfgLocation::Print()
{
    COUT << "[ Path === " << path << " ]" << ENDL;
    GlobGet::Print();
}
void CfgLocation::PrintData()
{
    COUT  << CYNB << "<<<<<<< begin config location >>>>>>>"<< RESET << ENDL;
    COUT << CYN << ENDL;
    GlobGet::PrintData();
    COUT << RESET  << ENDL;
    COUT << CYNB<< "<<<<<<<  end config loczation  >>>>>>>"<< RESET << ENDL;
}
void CfgLocation::CheckMethods(VEC_T &value)
{

    MethodsCount++;
    if(MethodsCount > 1)
        throw Error(DEBUGE + "[ methods ] directive is duplicate");

    if (MethodsToCheck.empty())
        throw Error(DEBUGE + "internal error [ MethodsToCheck ] is empty");
    
    std::size_t SizeMethodToCheck = MethodsToCheck.size();
    for(VEC_T::iterator it = value.begin(); it != value.end(); it++)
    {
        VEC_T::iterator it1 = std::find(MethodsToCheck.begin(),MethodsToCheck.end(), *it);
        if( it1 == MethodsToCheck.end())
            throw Error(DEBUGE + "invalid method [ " + *it +" ] in [ methods ] directive");
        else
            MethodsToCheck.erase(it1);
    }
    if(MethodsToCheck.size() + value.size() != SizeMethodToCheck)
        throw Error(DEBUGE + "methods are duplicate in [ methods ] directive");
}
void CfgLocation::CheckAutoIndex(VEC_T &value)
{
    AutoIndexCount++;
    if( AutoIndexCount > 1)
        throw Error(DEBUGE + "[ autoindex ] directive is duplicate");

    if(value.size() != 1)
        throw Error(DEBUGE + "invalid number of arguments in [ autoindex ] directive");
    if(value[0] != "on" && value[0] != "off")
        throw Error(DEBUGE + "invalid value "+value[0]+" in [ autoindex ] directive, it must be [ on ] or [ off ]");
}
void CfgLocation::CheckUpload(VEC_T &value)
{
    UploadCount++;
    if(UploadCount > 1)
        throw Error(DEBUGE + "[ upload ] directive is duplicate");

    if(value.size() != 1)
        throw Error(DEBUGE + "invalid number of arguments in [ Upload ] directive");
    if(*(value[0].end() - 1) != '/' )
        throw Error(DEBUGE + "add [ / ] to [ "+ value[0] +" ] [ Upload ] directive");

}
void CfgLocation::CheckRedirect(VEC_T &value)
{
    RedirectCount++;
    if(RedirectCount > 1)
        throw Error(DEBUGE + "[ redirect ] directive is duplicate");
     if (value.size() != 1)
        throw Error(DEBUGE + "invalid number of arguments in [ redirect ] directive");

}

void CfgLocation::CheckCgi(VEC_T &value)
{
    if(value.size() != 2)
        throw Error(DEBUGE + "invalid number of arguments in [ cgi ] directive");
    if(value[0] != "py" && value[0] != "php")
        throw Error(DEBUGE +"[ " + value[0] +" ] not supported as [ cgi ]");
    if(Cgi.find(value[0]) != Cgi.end())
        throw Error(DEBUGE + "[ "+ value[0] +" ] is duplicate in [ cgi ] directive");
    
}

void CfgLocation::Set()
{
    for(MAP_T::iterator it = directives.begin(); it != directives.end();it++)
    {
        if(it->first == "methods")
        {
            CheckMethods(it->second);
            Methods = it->second;
        }
        else if(it->first == "autoindex")
        {
            CheckAutoIndex(it->second);
            it->second[0] == "on" ? AutoIndex = true: AutoIndex = false; 
        }
        else if(it->first == "upload")
        {
            CheckUpload(it->second);
            Upload = it->second[0]; 
        }
        else if (it->first == "redirect")
        {
            CheckRedirect(it->second);
            Redirect = it->second[0];
        }
        else if(it->first == "cgi")
        {
            CheckCgi(it->second);
            Cgi[it->second[0]] = it->second[1];
        }
        else
            GlobGet::Set(it);
    }
}

void CfgLocation::valid()
{
    if(path.empty())
        throw Error(DEBUGE +"add path of [ location ] directive");

    if(path.find_first_of("*.") != std::string::npos)
        throw Error(DEBUGE +"invalid path of [ location ] directive");
    if(path[0]!= '/')
        throw Error(DEBUGE +"most start with [ / ] path of [ location ] directive");
    if(MethodsCount != 1)
        throw Error(DEBUGE + "add [ Methods ] directive");
    
    GlobGet::valid();
}