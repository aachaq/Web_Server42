#include "./Server/Http.hpp"

int main(int ac, char ** av)
{

    (void) ac;
    (void) av;
    std::string file;
    try
    {
        if(ac > 2)
        {
            throw Error("invalid num of args");
        }
        else if(ac == 1)
        {
            file = "./conf/default.conf";
        }
        else if(ac == 2)
        {
            if(!av[1][0])
            {
                file = "./conf/default.conf";
            }
            else {
                file = av[1];
            }
        }
        Http app(file);
        app.core();
    }
    catch(Error &err)
    {
        COUT << "catch error" << ENDL;
        std::cerr << RED << "Error : "<< err._msg << RESET << ENDL;
    }
    catch(std::exception &e)
    {
        COUT << "catch system" << ENDL;
        std::cerr << GRN << "std::exception ==> "<< e.what() << RESET << ENDL;
    }
    (void)av ;
    
}