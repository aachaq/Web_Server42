#ifndef include__HPP
#define include__HPP

#include <iostream>
// #include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>



#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define RESET  "\033[0m"

#define RED  "\033[31m"
#define BLU  "\033[34m"
#define GRN  "\033[32m"
#define YEW  "\033[33m"
#define CYN  "\033[32m"

#define REDB  "\033[41m"
#define BLUB  "\033[44m"
#define GRNB  "\033[42m"
#define YEWB  "\033[43m"
#define CYNB  "\033[42m"

#define COUT std::cout
#define COUTP(x) std::cout << x << std::endl;
#define COUTC(a,x) std::cout << a << x  << RESET << std::endl;
// #define CERR  // std::cerr   
#define ENDL std::endl
#define FLUSH std::flush

#endif