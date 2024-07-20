#include "Response.hpp"
#include <cstring>
#include <sys/wait.h>



void    Response::send_function()
{
  // static int b;
  signal(SIGPIPE, SIG_IGN); ///////////////

  if(!IsOpenAndHeader)
  {
    std::string file_path = Full_path;

    send_fd = open(file_path.c_str(), O_RDONLY);
    if (send_fd == -1)
    {
        GoSend = true;
        sendStatusCode( 500);
        return;
    }
    std::memset(&fileInfo, 0, sizeof(struct stat));
    if (fstat(send_fd, &fileInfo) == -1)
    {
        GoSend = true;          
        sendStatusCode( 500);
        return;
    }
    send_contentLenght = fileInfo.st_size;
    std::stringstream ss;
    ss << fileInfo.st_size;
    std::string httpHeader = "HTTP/1.1 "+ http_status_code +"\r\nContent-Type: "+Content_Type+"\r\nContent-Length: "+ss.str()+"\r\n\r\n";
    int  i = send(Data->SocketFd, httpHeader.c_str(), httpHeader.size(), 0);
    if(i <= 0)
    {
        Drop_Client = true;
        return;
    }
    IsOpenAndHeader = true;
    return;
  }


  char  buff[1024];
  memset(buff, 0, 1024);
  int i =  read(send_fd, buff, sizeof(buff));
  if(i == -1)
  {
    GoSend = true;          
    sendStatusCode( 500);
    return;
  }
  if(i == 0)
  {
    Drop_Client = true;
    return;
  }

  if (count + i >= send_contentLenght)
  {
    count +=i;
    int y  =  send(Data->SocketFd, buff,i , 0);
    if(y <= 0)
    {
      Drop_Client = true;
      return;
    }

    send_done = true; 
    return;
  }
  int y  =  send(Data->SocketFd, buff,i, 0);
  if(y <= 0)
  {
    Drop_Client = true;
    return;
  }
  count +=i;
}
