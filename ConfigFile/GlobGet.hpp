#ifndef GLOBGET__HPP
#define GLOBGET__HPP

#include "Utiles.hpp"

struct GlobGet
{   
    std::string name;
    int RootCount;
    std::string Root;
    int IndexCount;
    VEC_T Index;
    MAP_T directives;
    



    MAP_T::iterator Find(std::string &ToFind, std::string &value);
    void CheckIndex(VEC_T &value);
    void CheckRoot(VEC_T &value);
    GlobGet(std::string name);
    GlobGet();
    GlobGet(const GlobGet & obj);
    GlobGet &operator=(const GlobGet & obj);
    virtual ~GlobGet();
    
    virtual void Print();
    void PrintData();
    void PrintDirectives();
    void Set(MAP_T::iterator &it);

    void valid();

};

#endif