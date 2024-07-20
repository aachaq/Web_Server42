#ifndef Request__hpp
#define Request__hpp


// #include "../Includes.hpp"

#include "ClientData.hpp"


class Request
{

    public:

        ClientData *Data;

        bool ReadyHandleReq;
        std::string save;
        bool HeaderDone;
        bool IsKey;
        std::string Special;
        VEC_T Tokens;
        int BytesToBeRead;
        long RecvedBytes;
        int Bytes;
        int DupBytes;
        bool HeadersAreParsed;
        std::string ssave; 
        long WriteReturn;
        int fd;
        bool IsFirstLine;
        bool IsTime;
        long Chunklength;
        long ChunklengthReaded;
        std::string ChunkValue;
        long ChunkValueLength;
        bool YouNeedToHandleIt;
        bool valid;
        bool TransferCoding;
        bool DropClient;


        Request();
        Request(ClientData &obj);
        Request(const Request &obj);
        Request & operator=(const Request &obj);
        

        ~Request();

        void    ReadAndParse();
        void    HeadersAsTokens(std::string BuffToParse);
        int    HeadersAsMap();
        int    OpenTmpBodyFile();
        int    ParseBody(char *Buff);
        int    ParseChunkedBody(char *Buff);

        void CheckHexa(); 
        int HexaToLong(std::string &Tmp);
        int GetHexa();
        int HandleFirstLine();
        int CheckSomeHeaders();

        void getRightConfig(std::string &host);




};

#endif