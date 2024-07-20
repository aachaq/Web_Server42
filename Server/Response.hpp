#ifndef Response__hpp
#define Response__hpp


#include "../Server/ClientData.hpp"
#include <map>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>

#include <sys/wait.h>
#include <signal.h>



class Response
{


    public:

        ClientData *Data;

        std::string http_status_code;
        std::string     auto_index;
        std::string     Content_Type;
        std::map<int, std::string> send_status_code;
        std::map<std::string, std::string> FileForm;
        std::vector<std::string>   auto_index_content;
        std::string _Form;
        std::string interpreter;
        int index_location;
        int GoSend;
        std::string Full_path;
        bool IsOpenAndHeader;
        int send_fd;
        size_t send_contentLenght;
        size_t count;
        bool send_done;
        bool Drop_Client;
        int pid;
        bool exec_cgi;
        int data_fd;
        bool length_cgi;
        bool type_cgi;
        bool status_c;
        std::string cgi_status;
        std::string header_send;
        std::string HeadersBuff;
        bool trouve_str;
        std::string tmp_file;

        struct stat fileInfo;
        Response();
;
        Response(const Response &obj);
        Response & operator=(const Response &obj);
        
        ~Response();


        // int         location_index();
        int         is_exist_location(std::string loc);
        void        send_function();
        std::string research_FileForm(std::string FileFormToCheck);
        int        add_auto_index();
        std::string get_auto_index_html();  
        void        send_auto_index_file();
        void        send_redirection(std::string );
        std::string get_interpreter(std::string extensio,int index_location);


        void        getMethode();
        void        deleteMethode();
        void        ft_methods();


        void    addStatusCode();

        void    sendStatusCode(int statusCode);



        int DeleteFile(const std::string filename);
        int DeleteDirectory(const std::string path);

        int TraverseDir(const std::string path);
        int IsAllowed(int index, std::string MethodToCheck);


        
        bool is_file(std::string path);
        bool is_dir(std::string path);

        int myfind(std::string path);
        std::string  matchLocation(std::string path);
        std::string clean_path(std::string path);
        std::string getExtension(std::string path);
        void    AddFilesExtension();

        void    getIsfile();

  
        void CgiFunction();
        void IntEnvCgi();

        int parseOutput_Php_cgi();
        void getTokers(std::string line);

       void     postIsfile();
        void    postMethode();
        std::string ReturnExtension(std::string Ctype);




};

#endif
