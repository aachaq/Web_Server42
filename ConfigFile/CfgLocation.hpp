#ifndef CFGLOCATION__HPP
#define CFGLOCATION__HPP

#include "GlobGet.hpp"
#include "Utiles.hpp"

struct CfgLocation : GlobGet
{   
    std::string path;
    int MethodsCount;
    int AutoIndexCount;
    bool AutoIndex;
    int UploadCount;
    std::string Upload;
    int RedirectCount;
    std::string Redirect;
    VEC_T Methods;
    VEC_T MethodsToCheck;
    std::map<std::string, std::string >  Cgi;

    void CheckRedirect(VEC_T &value);
    void CheckUpload(VEC_T &value);
    void CheckAutoIndex(VEC_T &value);
    void CheckMethods(VEC_T &value);
    void CheckCgi(VEC_T &value);
    
    CfgLocation(VEC_T::iterator  &start, VEC_T::iterator &end);
    CfgLocation();
    ~CfgLocation();
    CfgLocation(const CfgLocation &obj);

    CfgLocation &operator=(const CfgLocation &obj);

    void PrintData();
    void Print();
    void Set();
    void valid();

};

#endif