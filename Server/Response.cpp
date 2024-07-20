#include "Response.hpp"
#include "ClientData.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <dirent.h>
#include <string>
#include <vector>


Response::Response()
{
    AddFilesExtension();
    addStatusCode();
    http_status_code ="";
    auto_index = "";
    Content_Type ="";
    _Form = "";
    index_location = -1;
    interpreter = "";
    Full_path = "";
    Data = NULL;
    pid =-1;
    GoSend = false;
    IsOpenAndHeader = false;
    send_fd = -1;
    send_contentLenght = 0;
    send_done = false;
    count = 0;
    Drop_Client = false;
    exec_cgi = false;
    HeadersBuff = "";
    trouve_str = false;
    length_cgi = 0;
    type_cgi =false; 
    status_c = false;
    data_fd = -1;
    cgi_status = "";
    header_send = "";
    tmp_file = "";
}

Response::Response(const Response &obj)
{
    *this = obj;
}

Response & Response::operator=(const Response &obj)
{
    if(this != &obj)
    {

        http_status_code = obj.http_status_code;
        auto_index = obj.auto_index;
        Content_Type = obj.Content_Type;
        send_status_code = obj.send_status_code;
        FileForm = obj.FileForm;
        auto_index_content = obj.auto_index_content;
        _Form = obj._Form;
        interpreter = obj.interpreter;
        index_location = obj.index_location;
        GoSend = obj.GoSend;
        Full_path = obj.Full_path;
        IsOpenAndHeader = obj.IsOpenAndHeader;
        send_fd = obj.send_fd;
        send_contentLenght =obj.send_contentLenght;
        count = obj.count;
        send_done = obj.send_done;
        Drop_Client= obj.Drop_Client;
        pid = obj.pid;
        exec_cgi = obj.exec_cgi;
        data_fd = obj.data_fd;
        length_cgi = obj.length_cgi;
        type_cgi = obj.type_cgi;
        status_c = obj.status_c;
        cgi_status = obj.cgi_status;
        header_send = obj.header_send;
        HeadersBuff = obj.HeadersBuff;
        trouve_str = obj.trouve_str;
        tmp_file = obj.tmp_file;


    }
    return *this;
}

Response::~Response()
{

}


int Response::is_exist_location(std::string loc)
{

    std::size_t size = Data->Config.Location.size();
    for(std::size_t i = 0; i < size; i++)
    {
        if (Data->Config.Location[i].path == loc)
        {
            return (i);
        }
    }
    return -1;
}



bool Response::is_file(std::string path)
{
    std::memset(&fileInfo, 0, sizeof(struct stat));
    if (stat(path.c_str(), &fileInfo) != 0)
        return (false);
    if (S_ISREG(fileInfo.st_mode))
        return (true);
    return (false);
}

bool Response::is_dir(std::string path)
{
    std::memset(&fileInfo, 0, sizeof(struct stat));
    if (stat(path.c_str(), &fileInfo) != 0)
        return (false);
    if (S_ISDIR(fileInfo.st_mode))
        return (true);
    return (false);
}


void    Response::addStatusCode()
{
    send_status_code[201] = "201 Created";
    send_status_code[204] = "204 No Content";

    send_status_code[400] = "400 Bad Request";

    send_status_code[403] = "403 Forbidden";
    send_status_code[404] = "404 Not Found";
    send_status_code[405] = "405 Method Not Allowed";
    send_status_code[409] = "409 Conflict";
    send_status_code[411] = "411 Length Required";
    send_status_code[413] = "413 Request Entity Too Large";
    send_status_code[414] = "414 Request-URI Too Long";
    send_status_code[415] = "415 Unsupported Media Type";

    send_status_code[500] = "500 Internal Server Error";
    send_status_code[501] = "501 Not Implemented";
    send_status_code[502] = "502 Bad Gateway";
    send_status_code[505] = "505 HTTP Version Not Supported";

}

void    Response::sendStatusCode(int statusCode)
{

    std::map<int, std::string> ::iterator it;
    it = Data->Config.ErrorPage.find(statusCode);
    if (it == Data->Config.ErrorPage.end())
    {
        std::map<int, std::string>::iterator it0;
        it0 = send_status_code.find(statusCode);

        std::string str_to_send = "<html>\n<h1>"+ it0->second +"</h1>";
        std::stringstream ss;
        ss << str_to_send.size();
        std::string str_to_send0 = "HTTP/1.1 "+ it0->second+"\r\nContent-Length: "+ss.str()+"\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n" +str_to_send;
        int y  =  send(Data->SocketFd, str_to_send0.c_str(),str_to_send0.size(), 0);
        if(y <= 0)
        {
            Drop_Client = true;
            return;
        }
        send_done = true;
    }
    else
    {
        std::map<int, std::string>::iterator it0;
        it0 = send_status_code.find(statusCode);

        http_status_code = it0->second;
        Content_Type = "text/html";
        Full_path = it->second;
        GoSend = true;
        send_function();
    }

}


std::string Response::research_FileForm(std::string FileFormToCheck)
{
    std::map<std::string ,std::string>::iterator it = FileForm.find(FileFormToCheck);
    if(it == FileForm.end())
        return "NOT";
    return it->second;
}

void    Response::AddFilesExtension()
{
    std::fstream inputFile;
	inputFile.open("extensionFile");
	if (!inputFile.is_open())
	{
		// std::cout << "extensionFile Not Exist" << std::endl;
        return;
	}
	else
	{
		std::string line;
		std::string word1;
		std::string word2;
		while (getline(inputFile, line))
		{
			int i = 0;
			while (line[i] != '=')
				i++;
			word1 = line.substr(0, i);
			word2 = line.substr(i + 1, line.size() - i + 1);
			FileForm.insert(std::make_pair(word1, word2));
		}
	}
    inputFile.close();
}

int    Response::add_auto_index()
{

    int dirDescriptor = open(Full_path.c_str(), O_RDONLY | O_DIRECTORY);
    if (dirDescriptor == -1)
    {
        return -1;
    }
    DIR* dir = fdopendir(dirDescriptor);
    if (dir == NULL)
    {
        close(dirDescriptor);
        return -1; 
    }
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name(entry->d_name);
        if ((entry->d_type == DT_DIR && name[0] != '.') || name == "..")
            auto_index_content.push_back(name + "/");
        else if ((entry->d_type == DT_REG && name[0] != '.') || name == "..")
            auto_index_content.push_back(name);
    }
    close(dirDescriptor);
    closedir(dir);
    return 0;
}        

std::string     Response::get_auto_index_html()
{
    std::string str;
    size_t size = auto_index_content.size();

    for (size_t i = 0; i < size; i++)
    {
        str += "<a href="+ Data->Url + auto_index_content[i] + ">"+auto_index_content[i]+"</a><br>";
        
    }
    std::string html = "<html>\n<body>\n<h1>Autoindex</h1>" + str + "</body>\n""</html>";
    return html;
}


void Response::send_auto_index_file()
{
    std::string str_send = get_auto_index_html();
    std::stringstream ss ;
    ss << str_send.size();

    http_status_code = "200 OK";;
    Content_Type = "text/html";
    
    
    std::string to_send = "HTTP/1.1 200 OK\r\nContent-Length: " +ss.str()
    + "\r\nContent-Type: text/html\r\n"
    + "Connection: close\r\n\r\n" + str_send;

    int y  =  send(Data->SocketFd, to_send.c_str(),to_send.size(), 0);
    if(y <= 0)
    {
        Drop_Client = true;
      return;
    }
    send_done = true;
}

void    Response::send_redirection(std::string redec )
{
    
    std::string httpHeader = Data->Protocl+" 301 Moved Permanently\r\n"
    + "Location: "+redec+"\r\n"
    + "Content-Type: text/html\r\n"
    + "Connection: close\r\n\r\n";

    int y = send(Data->SocketFd, httpHeader.c_str(), httpHeader.size(), 0);
    if(y <= 0)
    {

        Drop_Client = true;
        return;
    }
    send_done = true;
}

int Response::IsAllowed(int index, std::string MethodToCheck)
{
    
    VEC_T::iterator begin_it = Data->Config.Location[index].Methods.begin();
    VEC_T::iterator end_it = Data->Config.Location[index].Methods.end();
    VEC_T::iterator it;
    it = std::find(begin_it, end_it,MethodToCheck);
    if (it == end_it)
        return (-1);
    return (1);
}


std::string Response::clean_path(std::string path)
{
    size_t i = 0;
    for (; i < path.size(); i++)
    {
        if (path.c_str()[i] == '/' && path.c_str()[i + 1] == '/')
        {
            path = path.substr(0, i) + path.substr(i + 1, path.size());
            i--;
        }
    }
    return (path);
}



std::string  Response::matchLocation(std::string path)
{
    std::size_t size = Data->Config.Location.size();

    for (size_t i=0; i < size; i++)
    {
        size_t pos = path.find(Data->Config.Location[i].path);
        if (pos != std::string::npos)
        {
            size_t size = Data->Config.Location[i].path.size();
            if (Data->Config.Location[i].path == path.substr(0, size))
            {
                return Data->Config.Location[i].path;
            }
        }
            
    }
    return "NOT_EXIST";
}

std::string Response::getExtension(std::string path)
{
    for (size_t i = path.size(); i > 0; i--)
    {
        if (path[i] == '.')
        {
            return (path.substr(i + 1, path.size()));
        }
    }
    return ("NOT_EXIST");
}


std::string Response::get_interpreter(std::string extension,int index_location)
{
    std::map<std::string, std::string>::iterator it = Data->Config.Location[index_location].Cgi.find(extension);
    if(it == Data->Config.Location[index_location].Cgi.end())
        return "";
    interpreter = it->second;
    return it->second;
}

void getRoot(std::string & path , std::string loc, std::string root)
{
    if(*(loc.end() -1) == '/' && *(root.end() -1) != '/' )
    {
        path.replace(0, loc.length()-1,root);
    }
    else if(*(loc.end() -1) != '/' && *(root.end() -1) == '/')
    {
        path.replace(0, loc.length() + 1,root);
    }
    else
        path.replace(0, loc.length(),root);
}

void    Response::getMethode()
{
    Data->Url = clean_path(Data->Url);

    std::string location = matchLocation(Data->Url);
    index_location = is_exist_location(location);

    if (index_location != -1)
    {
        Full_path=Data->Url;
        getRoot(Full_path, Data->Config.Location[index_location].path,Data->Config.Location[index_location].Root);

        _Form = getExtension(Full_path);

        if(IsAllowed(index_location, "GET") == -1)
        {
            GoSend = true;
            sendStatusCode(405); //not allowd
            return;
        }
        else if (is_file(Full_path))
        {
            GoSend = true;
            getIsfile();
            return ;
        }
        else if (is_dir(Full_path))
        {
            if(*(Data->Url.end()-1) != '/')
            {
                std::string str("HTTP/1.1 301 Moved Permanently\r\nLocation: ");
                str += Data->Url+"/\r\n";
                int z = send(Data->SocketFd, str.c_str(), str.size(), 0);
                if(z <= 0)
                {
                    Drop_Client = true;
                    return ;
                }
                send_done = true;
                return;
            }
            if( Data->Config.Location[index_location].RedirectCount > 0 )
            {
                GoSend = true;
                send_redirection(Data->Config.Location[index_location].Redirect);
                return;
            }
            else if (!Data->Config.Location[index_location].Index.empty())
            {
                std::cout << "hna\n";
                std::size_t i = 0;
                std::size_t size = Data->Config.Location[index_location].Index.size();
                while (i < size)
                {
                    std::string SaveFullpath(Full_path);
                    SaveFullpath += Data->Config.Location[index_location].Index[i];
                    if (is_file(SaveFullpath))
                    {
                        Full_path = SaveFullpath;
                        _Form = getExtension(Full_path);
                        getIsfile();
                        return;
                    }
                    i++;
                }
                GoSend = true;
                sendStatusCode(403);
            }
            else if (Data->Config.Location[index_location].AutoIndex)
            {
                int y = add_auto_index();
                if(y == 0)
                    send_auto_index_file();
                else
                {
                    GoSend = true;
                    sendStatusCode( 500);
                }
            }
            else
            {
                // REFACTOR
                GoSend = true;
                sendStatusCode(403);
            }
        }
        else
        {
            GoSend = true;
            sendStatusCode(404);
        }
    }
    else// location not fond
    {
        GoSend = true;
        sendStatusCode(404);
    }
}


int Response::DeleteFile(const std::string filename) 
{
    if (std::remove(filename.c_str()) != 0)
    {
       return (-1);
    }
    return (0);
}

int Response::DeleteDirectory(const std::string path)
{
    if (rmdir(path.c_str()) != 0)
    {
        return (-1);
    }
    return (0);
}

int Response::TraverseDir(const std::string dpath)
{
    DIR *dir;
    struct dirent *entry;
    struct stat info;
    dir = opendir(dpath.c_str());
    if (dir == NULL)
    {
        return (-1);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        std::string fullpath = dpath + "/" + entry->d_name;
        if (stat(fullpath.c_str(), &info) != 0)
        {
           closedir(dir);
           return (0);
        }

        if (S_ISDIR(info.st_mode))
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                if (TraverseDir(fullpath) != 0)
                {
                    closedir(dir);
                   return (-1);
                }
            }
        }
        else
        {
            if (DeleteFile(fullpath) != 0)
            {
                closedir(dir);
                return (-1);

            }
        }
    }
    if (DeleteDirectory(dpath) != 0)
    {
        closedir(dir);
        return(-1);
    }
    closedir(dir);
    return (0);
}

void    Response::deleteMethode()
{
    Data->Url = clean_path(Data->Url);

    std::string location = matchLocation(Data->Url);

    index_location = is_exist_location(location);

    if(index_location != -1)
    {
        Full_path=Data->Url;
        getRoot(Full_path, Data->Config.Location[index_location].path,Data->Config.Location[index_location].Root);

        if(IsAllowed(index_location, "DELETE") == -1)
        {
            GoSend = true;
            sendStatusCode(405);
            return;


        }
        if (is_file(Full_path))
        {
            if (std::remove(Full_path.c_str()) == 0)
            {
                // 204 No Content
                GoSend = true;
                sendStatusCode(204);
                return;

            }
            else
            {
                // 403 Forbidden
                GoSend = true;
                sendStatusCode(403);
                return;

            }
        }
        else if (is_dir(Full_path))
        {
            if(*(Data->Url.end()-1) != '/')
            {
                GoSend = true;
                sendStatusCode(409);
                return;
            }
            int y = TraverseDir(Full_path.c_str());
            // recurseve
            if (y == 0)
            {
                // 204 No Content
                GoSend = true;
                sendStatusCode(204);
                return;
            }
            else
            {
                // 403 Forbidden
                GoSend = true;
                sendStatusCode(403);
                return;
            }
        }
        else
        {
            // 404 Not found;
            GoSend = true;
            sendStatusCode(404);
            return;
        }
    }
    else
    {
        // 404 Not found;
        GoSend = true;
        sendStatusCode(404);
        return;
    }
}

void    Response::postIsfile()
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
		GoSend = true;
		sendStatusCode(403);
    }
}


std::string Response::ReturnExtension(std::string Ctype)
{
    for (std::map<std::string, std::string>::iterator it = FileForm.begin(); it != FileForm.end(); it++)
    {
        if (it->second == Ctype) 
        {
            return it->first;
        }
    }
    return("");
}
void    Response::postMethode()
{
    Data->Url = clean_path(Data->Url);
   std::string location = matchLocation(Data->Url);

    // VERIFICATIONS START
    index_location = is_exist_location(location);
    if (index_location == -1 )
    {
        sendStatusCode(404);
        return;
    }
    if(IsAllowed(index_location, "POST") == -1)
    {
        sendStatusCode(405);
        return;
    }

    
    Full_path=Data->Url;
    getRoot(Full_path, Data->Config.Location[index_location].path,Data->Config.Location[index_location].Root);

    if (!Data->Config.Location[index_location].Upload.empty())
    {
        if(is_file(Data->Config.Location[index_location].Upload))
        {
            GoSend = true;
            sendStatusCode(500); 
            return;
        }
        else if (is_dir(Data->Config.Location[index_location].Upload))
        {
            std::string name = Data->Config.Location[index_location].Upload + Data->UploadFileName+ "." + ReturnExtension(Data->ContentType);
            int z = rename(Data->BodyTmpFileName.c_str(), name.c_str());

            if (z != 0) {
                GoSend = true;
                sendStatusCode(500);
                return;  
            }
            std::string str("HTTP/1.1 201 Created\r\nContent-type: text/html; charset=UTF-8\r\nConnection: close\r\n\r\n");
            z = send(Data->SocketFd, str.c_str(), str.size(), 0);
            if(z <= 0)
            {
                Drop_Client = true;
                return;
            }
            send_done = true;
            return;
        }
        else {
            GoSend = true;
            sendStatusCode(404);
            return;
        }
    }
    _Form = getExtension(Full_path);
    if (is_file(Full_path))
    {
        postIsfile();
        return ;
    }
    else if(is_dir(Full_path))
    {
        if(*(Data->Url.end()-1) != '/')
        {
            std::string str("HTTP/1.1 301 Moved Permanently\r\nLocation: ");
            str += Data->Url+"/\r\nConnection: close\r\n\r\n";
            int z = send(Data->SocketFd, str.c_str(), str.size(), 0);
            if(z <= 0)
            {
                Drop_Client = true;
            }
            send_done = true;
            return;
        }
        if (!Data->Config.Location[index_location].Index.empty())
        {
            std::size_t i = 0;
            std::size_t size = Data->Config.Location[index_location].Index.size();
            while (i < size)
            {
                Full_path += Data->Config.Location[index_location].Index[i];
                if (is_file(Full_path))
                {
                    _Form = getExtension(Full_path);
                    postIsfile();
                    return;
                }
                i++;
            }
            GoSend = true;
            sendStatusCode(403);
            return;
        }
        GoSend = true;
        sendStatusCode(403);
        return;
    }
    else
    {
        GoSend = true;
        sendStatusCode(404);
    }

}


void    Response::ft_methods()
{

    if(exec_cgi)
    {
        int status;
        int i = waitpid(pid,&status, WNOHANG);
        if( i == pid )
        {   
             if(WIFEXITED(status))
             {

                 int exit_status = WEXITSTATUS(status);
                 if(exit_status != 0)
                 {
                     exec_cgi = false;
                     GoSend = true;
                     sendStatusCode(500);
                     return;
                 }
            }
            if(!IsOpenAndHeader)
            {
                exec_cgi = false;
                close(send_fd);
                close(data_fd);
                int z = parseOutput_Php_cgi();
                if(z == -1)
                    Drop_Client = true;
                else
                {
                    GoSend = true;
                    if (z != 0)
                        sendStatusCode(z);
                }
            }

        }
        return;
    }
    if(GoSend)
    {
        send_function();
        return;
    }
    if(Data->ErrorCode != -1)
    {
        GoSend = true;
        sendStatusCode(Data->ErrorCode);
        return;
    }
    if (Data->Method== "GET")
        getMethode();
    else if (Data->Method == "DELETE")
        deleteMethode();
    else if (Data->Method == "POST")
    {
        postMethode();
    }
}

