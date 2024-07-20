#ifndef UTILES__HPP
#define UTILES__HPP

#include  "../Includes.hpp"


#define MAIN_B 0
#define SERVER_B 1
#define LOCATION_B 2

#define VEC_T std::vector<std::string>
#define PAIR_T std::pair<std::string, VEC_T>
#define MAP_T std::vector<PAIR_T>
#define INTSTRING_T std::pair<int, std::string>
#define IMAP_T std::vector<INTSTRING_T>
#define STRINGINT_T std::pair<std::string, int>
#define SMAP_T std::vector<std::pair<std::string, std::string>>

#define HERE std::string("==> ")
#define DEBUGE std::string("==> ")
#define MIN_ERROR_CODE 300
#define MAX_ERROR_CODE 599

void TrimLine(std::string &Line , const std::string FStr, const std::string LStr);
bool DefineToken(std::string &to_def, const std::string &block);
void printvector(VEC_T &v, char c);
void PrintMapT(MAP_T &m);
void PrintMapT(std::map<std::string, VEC_T> &m);
void PrintIMAP(IMAP_T &m, char c);

double ToLong(std::string &str, double min, double max);
long ToLong(std::string &str);

std::string SumStrings(VEC_T::iterator start, VEC_T::iterator end);

void PrintSpecial(std::string Str);

void PrintTokens(VEC_T &Tokens);


struct  Error
{
    std::string _msg;
    Error(const std::string &msg) :_msg(msg){};
};

#endif