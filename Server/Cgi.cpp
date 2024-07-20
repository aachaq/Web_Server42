#include "Response.hpp"
#include <cctype>
#include <ctime>

void Response::getTokers(std::string line)
{

	std::istringstream tmp(line);
	std::string token;
	std::getline(tmp,token, ':');

	std::string a(token); 
	std::transform(token.begin(),token.end(),a.begin(),tolower);
	
	if(a == "status" && !status_c)
	{
		status_c = true;
		token = "";
		std::getline(tmp,token, ':');
		cgi_status = token;

	}
	else if(a == "content-type" && !type_cgi)
	{
		type_cgi = true;
		HeadersBuff += token;
		token = "";
		std::getline(tmp,token, ':');
		HeadersBuff += ": " +token+"\n";
	}
	else if(a == "content-Length" && !length_cgi)
	{
		length_cgi = true;
		HeadersBuff += token;
		token = "";
		std::getline(tmp,token, ':');
		HeadersBuff += ": " +token+"\n";
	}
	else 
	{
		HeadersBuff+= line;
	}

}


int Response::parseOutput_Php_cgi()
{
	if(!IsOpenAndHeader)
	{
		send_fd = open(tmp_file.c_str(), O_RDWR | O_CREAT, 0777);
		if (send_fd == -1)
		{         
			return(500);
		}


		char  buff[1024];
  		memset(buff, 0,sizeof(buff));
		int i =  read(send_fd, buff, sizeof(buff));
		if(i == -1)
		{
			return 500;
		}
		if(i == 0)
		{
			Drop_Client = true;
			return -1;
		}

		HeadersBuff.append(buff,i);

    	std::size_t pos = HeadersBuff.find("\r\n\r\n");

		if(pos == std::string::npos)
		{     
			return 500;
		}
		count = i - (pos +2);
		std::string body  = HeadersBuff.substr(pos + 2, count);
		count -= 2;

		std::stringstream ss( HeadersBuff.substr(0, pos+2));
		HeadersBuff = "";
		
		
		std::string line;
		
		while(std::getline(ss, line))
		{
			if(!line.empty())
				getTokers(line);
		}
		
		header_send += "HTTP/1.1";
		
		if(!cgi_status.empty())
			header_send  += cgi_status+"\n";
		else 
			header_send += " 200 OK\r\n";
		if (!type_cgi)
			header_send += "Content-type: text/html; charset=utf-8\r\n";
		if(!length_cgi)
		{
			std::memset(&fileInfo, 0, sizeof(struct stat));
    		if (fstat(send_fd, &fileInfo) == -1)
    		{
    		    return 500;
    		}
    		send_contentLenght = fileInfo.st_size - pos - 4;
    		std::stringstream sezi;
    		sezi << send_contentLenght;

			header_send += "Content-Length: " + sezi.str() + "\r\n";
		}
		header_send += HeadersBuff;
		header_send += body;

		int z = send(Data->SocketFd, header_send.c_str(),header_send.size(), 0);
    	if(z <= 0)
    	{
			Drop_Client = true;
    	    return -1;
    	}
		IsOpenAndHeader = true;
	}

	return (0);
}





int OpenTmpBodyFile(std::string &TmpFileName, int &fd)
{
    std::time_t now = std::time(NULL);

    std::tm* tm = std::localtime(&now);
    char Buffer[20];
    
    std::strftime(Buffer, 20, "%d_%m_%Y_%H_%M_%S", tm);

    TmpFileName += Buffer;
    fd = open(TmpFileName.c_str(), O_WRONLY | O_CREAT, 0644);
    if(fd == -1)
    {
		return 500;
    }
    return 0;
}

void    Response::getIsfile()
{
    bool isCgi = false;
    
	if (_Form == "php" || _Form == "py" )
    {
		if(get_interpreter(_Form,index_location) != "")
	    	isCgi = true;
    }
    if (isCgi)
    {   
		CgiFunction();
    }
    else
    {
		Content_Type = research_FileForm(_Form);

		if (Content_Type == "NOT")
		{
		    GoSend = true;
		    sendStatusCode(415);
		}
		else
		{
		    GoSend = true;
		    http_status_code = "200 OK";
		    send_function();
		}
    }
}

void    Response::CgiFunction()
{
    std::string QUERY_STRING = "QUERY_STRING="+ Data->Query;


    std::string SCRIPT_FILENAME = "SCRIPT_FILENAME="+ Full_path;
    std::string PATH_INFO = "PATH_INFO="+ Full_path;

    std::stringstream ss;
    ss << Data->ContentLength;
    std::string CONTENT_LENGTH = "CONTENT_LENGTH=" + ss.str();

    std::string CONTENT_TYPE = "CONTENT_TYPE="+ Data->ContentType;
    std::string HTTP_COOKIE = "HTTP_COOKIE="+Data->Cookie;

    if (!exec_cgi)
    {

		std::time_t now = std::time(NULL);
    	std::tm* tm = std::localtime(&now);
    	char Buffer[20];
	
    	std::strftime(Buffer, 20, "%d_%m_%Y_%H_%M_%S", tm);

		tmp_file = "./InternalUse/CGI_";

		tmp_file += Buffer;
		send_fd = open(tmp_file.c_str(),  O_TRUNC | O_RDWR | O_CREAT, 0777);
		if (send_fd == -1)
		{
		    GoSend = true;          
		    sendStatusCode( 500);
			return;
		}

		if (Data->Method == "POST")
		{
			data_fd = open(Data->BodyTmpFileName.c_str(),  O_RDONLY, 0777);
			if (data_fd == -1)
			{
			    GoSend = true;          

			    sendStatusCode( 500);
				return;
			}
		}


		exec_cgi = true;
		pid = fork();
		if(pid == -1)
		{
		    GoSend = true;
		    sendStatusCode( 500);
		    return;
		}
		if (pid == 0)
		{
		    char *const args[] = {(char *)interpreter.c_str(), (char *)Full_path.c_str(), NULL};
		    
			if (Data->Method == "GET")
		    {
				if(-1 == dup2(send_fd, 1))
					exit(1);

				char *const env[] = {
				(char *)"REDIRECT_STATUS=200",
				(char *)QUERY_STRING.c_str(),
				(char *)SCRIPT_FILENAME.c_str(),
				(char *)PATH_INFO.c_str(),
				(char *)HTTP_COOKIE.c_str(),
				(char *)"REQUEST_METHOD=GET", 
				NULL};

				if (execve(interpreter.c_str(), args, env) == -1)
					exit(1);
		    }
			
			else if (Data->Method == "POST")
		    {
				if(-1 == dup2(data_fd, 0))
				    exit(1);

				if(-1 == dup2(send_fd, 1))
				    exit(1);				

				char *const env[] = {
				(char *)"REDIRECT_STATUS=200",
				(char *)QUERY_STRING.c_str(),
				(char *)SCRIPT_FILENAME.c_str(),
				(char *)PATH_INFO.c_str(),
				(char *)HTTP_COOKIE.c_str(),
				(char *)"REQUEST_METHOD=POST",
				(char *)CONTENT_LENGTH.c_str(), 
				(char *)CONTENT_TYPE.c_str(), 
				NULL};

				if (execve(interpreter.c_str(), args, env) == -1)
					exit(1);
		    }
		}
    }
}
