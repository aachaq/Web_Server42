#ifndef Client__hpp
#define Client__hpp

#include "Request.hpp"
#include "Response.hpp"
#include "ClientData.hpp"
#include <sys/select.h>

struct Client : ClientData
{
    class Response Response;

    class Request Request;

    Client();
    Client(const Client &obj);
    Client & operator=(const Client &obj);
    ~Client();

    void SetData();
    int ReadOrWriteOrNone(const fd_set &ReadFDs,const fd_set &WriteFDs);
    
};

#endif