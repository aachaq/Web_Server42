#include  "GlobGet.hpp"
#include "Utiles.hpp"
#include <cstdio>
#include <ios>

GlobGet::GlobGet(std::string name)
{
    this->name = name;
    RootCount = 0;
    Root= "";
    IndexCount = 0;
}

GlobGet::GlobGet()
{
    name = "default";
    Root = "";
    IndexCount = 0;
    RootCount = 0;  
}

GlobGet::GlobGet(const GlobGet & obj)
{
    *this = obj;
}

GlobGet::~GlobGet()
{
    
}

GlobGet &GlobGet::operator=(const GlobGet & obj)
{
    if(this != &obj)
    {
        name = obj.name;
        Root = obj.Root;
        RootCount = obj.RootCount;
        Index = obj.Index;
        IndexCount = obj.IndexCount;
        directives = obj.directives;
    }
    return *this;

 }

MAP_T::iterator GlobGet::Find(std::string &ToFind, std::string &value)
{
    MAP_T::iterator it  = directives.begin();
    for(; it != directives.end(); it++)
        if(it->first == ToFind)
            if(!value.empty() && it->second[0] == value)
                break;
    return it;
}

void GlobGet::Print()
{
    COUT << "------------------------" << ENDL;
    COUT << GRN << FLUSH;
    for (MAP_T::iterator it = directives.begin(); it != directives.end(); it++)
    {
        COUT << "[ " << it->first << " ==> " << FLUSH;
        for(size_t i = 0; i < it->second.size(); i++)
            COUT << GRN << (!i ? "":" <=> ") << BLU << it->second[i] << FLUSH;
        COUT << GRN << FLUSH;
        COUT << " ]" << ENDL;
    }
    COUT << "------------------------" << ENDL;
}

void GlobGet::PrintDirectives()
{
    for(MAP_T::iterator it = directives.begin(); it != directives.end(); it++)
    {
        COUT  << "["<< BLU << it->first << "] ==> [";
        printvector(it->second, ' ');
        COUT << "]" << RESET << ENDL;
    }
}

void GlobGet::PrintData()
{
    COUT << "[ Index => ";printvector(Index, ' ');COUT << " ]" << ENDL;
    COUT << "[ Root  => " << Root << " ]" << ENDL;
}


void GlobGet::CheckIndex(VEC_T &value)
{
    IndexCount++;
    if(IndexCount > 1)
        throw Error(DEBUGE +"[ index ] directive is duplicate");
    if(value.size() == 0)
        throw Error(DEBUGE + "invalid number of arguments in [ index ] directive");

}



void GlobGet::CheckRoot(VEC_T &value)
{
    RootCount++;
    if(RootCount > 1)
        throw Error(DEBUGE +"[ root ] directive is duplicate");
    if(value.size() != 1)
        throw Error(DEBUGE + "invalid number of arguments in [ root ] directive");
    if(value[0].empty())
        throw Error(DEBUGE +"empty value in [ root ] directive");
    
}


void GlobGet::Set(MAP_T::iterator &it)
{
                    
                    
    if(name == "server")         
    {
        throw Error(DEBUGE+"[ " + it->first + " ] directive is not implement");
    }
    else  
    {

        if(it->first == "index")
        {
            CheckIndex(it->second);
            Index = it->second;
        }
        else if(it->first == "root")
        {
            CheckRoot(it->second);
            Root = it->second[0];
        }
        else
        {
            throw Error(DEBUGE + "internal error (GlobGet::Set(...) ) [ "+ it->first + " ]");
        }
    }
}

void GlobGet::valid()
{
    if(name == "location")
    {
        if(RootCount ==  0)
            throw Error(DEBUGE + "add [ root ] directive");
        // if(IndexCount == 0)
        //     throw Error(DEBUGE + "add [ index ] directive");
    }
}
