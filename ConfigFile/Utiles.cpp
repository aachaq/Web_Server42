#include "Utiles.hpp"
#include <sstream>
#include <string>

void TrimLine(std::string &Line , const std::string FStr, const std::string LStr)
{
    if(Line.empty())
        return;
    Line.erase(0,Line.find_first_not_of(FStr));
    Line.erase(Line.find_last_not_of(LStr)+1);
}

bool DefineToken(std::string &to_def, const std::string &block)
{
    VEC_T vec(12);
    if(block == "server")
    {
        vec.push_back("listen");
        vec.push_back("port");
        vec.push_back("server_name");
        vec.push_back("error_page");
        vec.push_back("client_max_body_size");
    }
    else {
    
        vec.push_back("methods");
        vec.push_back("autoindex");
        vec.push_back("cgi");
        vec.push_back("upload");
        vec.push_back("redirect");

    }
    vec.push_back("index");
    vec.push_back("root");

    if(std::find(vec.begin() , vec.end() , to_def) != vec.end())
        return true;
    return false;
}

void printvector(VEC_T &v, char c)
{
    for(VEC_T::iterator it = v.begin(); it != v.end() ; it++)
        COUT << "{" << *it << "}" << c << FLUSH;
}

void PrintMapT(MAP_T &m)
{
    for(MAP_T::iterator it = m.begin(); it != m.end(); it++)
    {
        COUT  << "["<< BLU << it->first << "] ==> [";
        printvector(it->second, ' ');
        COUT << "]" << RESET << ENDL;
    }
}


void PrintMapT(std::map<std::string, VEC_T> &m)
{
    for(std::map<std::string, VEC_T>::iterator it = m.begin(); it != m.end(); it++)
    {
        COUT  << "["<< BLU << it->first << "] ==> [";
        printvector(it->second, ' ');
        COUT << "]" << RESET << ENDL;
    }
}


void PrintIMAP(IMAP_T &m, char c)
{
    for(IMAP_T::iterator it = m.begin(); it != m.end() ; it++)
        COUT << "{" << it->first << c  << it->second <<  "}"<< FLUSH;
}

double ToLong(std::string &str, double min, double max)
{
    if(str.empty())
        return -2;
    if(str.find_first_not_of("1234567890") != std::string::npos)
        return -2;
    std::stringstream Ss(str);
    double L;
    Ss >> L;
    if(L < min || L > max)
        return -1;
    return L;
}

long ToLong(std::string &str)
{
    if(str.empty())
        return -2;
    if(str.find_first_not_of("1234567890") != std::string::npos)
        return -2;
    std::stringstream Ss(str);
    long L;
    Ss >> L;
    return L;
}

std::string SumStrings(VEC_T::iterator start, VEC_T::iterator end)
{
    std::string str;

    for (VEC_T::iterator it = start; it != end; it++)
    {
        if(it+1 == end)
        {
            str += *it;
            break;
        }
        if(it == start)
        {
            TrimLine(*it, " \t", " \t");
            str += *it;
        }
        else {
            str+=*it;
        }
        str+=";";
    }
    return str;
}


void PrintSpecial(std::string Str)
{
    // COUT << BLUB; 
    for(std::size_t i = 0; i < Str.length() ; i++)
    {
        if(Str[i] ==  '\r')
            COUT << "\\r";
        else if(Str[i] == '\n')
            COUT << "\\n" << ENDL;
        else
            COUT << Str[i];
        // COUT << "|";
    }
    // COUT << RESET << FLUSH;
}

void PrintTokens(VEC_T &Tokens)
{
    COUT << RED << "[[[[[[[[[[[[ Tokens ]]]]]]]]]]]]" << RESET << ENDL;
    {
        int i = 0;
        for (VEC_T::iterator it = Tokens.begin(); it < Tokens.end() ; it++, i++)
            COUT << BLU << "[" << i << "][" << *it << "]"<< RESET << ENDL;
    }
}
