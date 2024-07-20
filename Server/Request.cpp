#include "Request.hpp"
#include "ClientData.hpp"
#include <cstddef>


Request::Request()
{
    Data = NULL;

    ReadyHandleReq = false;
    save ="";
    HeaderDone = false;
    IsKey = true;
    Special = ",\n\r";
    BytesToBeRead = 1024;
    RecvedBytes = 0;
    Bytes = -1;
    DupBytes = 0;
    HeadersAreParsed = false;
    ssave = "";
    WriteReturn = 0;
    fd = -1;
    IsFirstLine = true;
    IsTime = true;
    Chunklength = -1;
    ChunklengthReaded = 0;
    ChunkValue = "";
    ChunkValueLength = 0;
    YouNeedToHandleIt = false;
    valid = false;
    TransferCoding = false;
    DropClient = false;
}

Request::Request(const Request &obj)
{
    *this = obj;
}

Request & Request::operator=(const Request &obj)
{
    if(this != &obj)
    {
        ReadyHandleReq  = obj.ReadyHandleReq;
        save = obj.save;
        HeaderDone = obj.HeaderDone;
        IsKey = obj.IsKey;
        Special = obj.Special;
        Tokens = obj.Tokens;
        BytesToBeRead = obj.BytesToBeRead;
        RecvedBytes = obj.RecvedBytes;
        Bytes = obj.Bytes;
        DupBytes = obj.DupBytes;
        HeadersAreParsed = obj.HeadersAreParsed;
        ssave = obj.ssave;
        WriteReturn = obj.WriteReturn;
        fd = obj.fd;
        IsFirstLine = obj.IsFirstLine;
        IsTime = obj.IsTime;
        Chunklength= obj.Chunklength;
        ChunklengthReaded = obj.ChunklengthReaded;
        ChunkValue = obj.ChunkValue;
        ChunkValueLength = obj.ChunkValueLength;
        YouNeedToHandleIt = obj.YouNeedToHandleIt;
        valid = obj.valid;
        TransferCoding = obj.TransferCoding;
        DropClient = obj.DropClient;
    }
    return *this;
}

Request::Request(ClientData &obj)
{
    Data = &obj;
}

Request::~Request()
{
}



void Request::HeadersAsTokens(std::string BuffToParse)
{
    int index = 0;
    for (std::string::iterator it = BuffToParse.begin() ; it != BuffToParse.end(); it++,index++)
    {
        std::size_t pos = Special.find(*it);

        if(pos != std::string::npos)
        {
            if (!save.empty()) 
            {
                Tokens.push_back(save);
                save = "";
            }
            if(*it == '\n')
            {
                VEC_T::iterator TmpToken = (Tokens.end() - 1);
                if(Tokens.size() >= 1 &&( (*TmpToken) == "\\r" || *(TmpToken) == "\\r\\n\\r"))
                {
                    *(TmpToken) += "\\n";
                    IsKey = true;
                    if(*(TmpToken) == "\\r\\n\\r\\n")
                    {
                        HeaderDone = true;
                        ssave = std::string(it +1, BuffToParse.end());
                        Bytes = index;
                        break;
                    }
                }
                else
                    Tokens.push_back("\\n");
            }
            else if (*it == '\r')
            {
                if(Tokens.size() >= 1 && *(Tokens.end() - 1) == "\\r\\n")
                    *(Tokens.end() - 1) +="\\r";
                else
                    Tokens.push_back("\\r");
            }
            else
            {
                Tokens.push_back(std::string(it , it +1));
            }
        }
        else {
            if(*it == ':' && IsKey == true)
            { 
                IsKey = false;

                save += ":";
                Tokens.push_back(save);
                save = "";
            }
            else 
                save += *it;
        }
    }
}

int Request::HandleFirstLine()
{
    std::stringstream ss(Tokens[0]);
    ss >> Data->Method;
    if(Data->Method != "POST" && Data->Method != "GET" && Data->Method !="DELETE")
    {
        return 501;
    }
    /*   parsing query from parse */
    std::string UrlTmp;
    ss >> UrlTmp;
    if(UrlTmp.empty())
    {

        return 500;

    }
    if(UrlTmp.size() > 2048)
    {

        return 414;
    }
    if(std::string::npos != UrlTmp.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~!#$&'()*+,/:;=?@[]"))
    {

        return 400;
    }
    std::size_t pos = UrlTmp.find('?');
    if(pos != std::string::npos)
    {
        Data->Query = UrlTmp.substr(pos + 1, UrlTmp.length() -  pos - 1);
        UrlTmp.erase(UrlTmp.begin() + pos, UrlTmp.end());
    }
    Data->Url  = UrlTmp;
    ss >> Data->Protocl;
    std::size_t SlashPos = Data->Protocl.find('/');
    if(SlashPos == std::string::npos)
    {
        return 400;
    }
    if(std::string(Data->Protocl,0, SlashPos) == "HTTP")
    {
        std::string Vrs(Data->Protocl.begin() + SlashPos +1 ,Data->Protocl.end());
        if(Vrs != "1.1" && Vrs != "1.0")
        {
            return 505;
        }
    }
    else
    {
        return 400;
    }
    return -1;
}

int Request::HeadersAsMap()
{
    std::size_t len = Tokens.size();
    if(IsFirstLine)
    {
        if(len > 1)
        {
            int z = HandleFirstLine();
            if(z != -1)
            {
                return z;
            }
            IsFirstLine = false;
        }
    }
    if(IsFirstLine)
        return -1;

    PAIR_T Tmp;
    bool IsValue = false;
    for (std::size_t Idx = 1; Idx < len; Idx++) 
    {
        if(Tokens[Idx] == "\\r\\n\\r\\n")
        {
            if(!Tmp.first.empty() || !Tmp.second.empty())
                Data->Headers[Tmp.first] = Tmp.second;
            HeadersAreParsed = true;
            return -1;
        }
        else if(Tokens[Idx] == "\\r\\n")
        {
            if(!Tmp.second.empty())
            {
                Data->Headers[Tmp.first] = Tmp.second;
                Tmp.first = "";
                Tmp.second.clear();
            }
            IsValue = false;
        }
        else
        {
            if(!IsValue)
            {
                Tmp.first = Tokens[Idx].erase(Tokens[Idx].size() -1);
                IsValue = true;
            }
            else if (Tokens[Idx] != ",")
                Tmp.second.push_back(Tokens[Idx]);
        }
    }
    return -1;
}

int Request::CheckSomeHeaders()
{

    /* content-type */
    std::map<std::string, VEC_T>::iterator It = Data->Headers.find("Content-Type");
    if(It == Data->Headers.end())            
    {
        return 400;
    }
    else 
    {
        TrimLine(It->second[0], " \t", " \t");
        if(It->second[0].empty())
        {
            return 400;
        }
        Data->ContentType = It->second[0];
    }
    /* content-length */
    if(Data->ContentLength == -1)
    {
        It = Data->Headers.find("Content-Length");
        if(It == Data->Headers.end())
        {
            return 411;

        }
        else 
        {
            if(It->second.size() != 1)
            {
                return 400;
            }
            TrimLine(It->second[0], " \t", " \t");
            if(It->second[0].empty())
            {
                return 400;
            }
            Data->ContentLength = ToLong(It->second[0]);
        }
    }
    if(Data->ContentLength <= 0)
    {
        return 400;
    }
    if(Data->ContentLength > Data->Config.ClientMaxBodySize)
    {
        return 413;
    }

    /* transfer-coding */
    It = Data->Headers.find("Transfer-Encoding");
    if(It != Data->Headers.end())
    {
        if(It->second.size() == 1 && It->second[0] == " chunked")
        {
            TransferCoding = true;
        }
        else {
            return 501;
        }
    }
    valid = true;
    return -1;
}
void Request::getRightConfig(std::string &host)
{
    Data->Config = Data->Configs[0];
    std::size_t size = Data->Configs.size();
    for(std::size_t i = 0; i< size; i++)
    {
        if(host == (Data->Configs[i].ServerName + ":" + Data->Configs[i].ListenPort.second))
        {
            Data->Config = Data->Configs[i];
            break;
        }
    }

}


void Request::ReadAndParse()
{
    if(ReadyHandleReq)
    {
        return;
    }

    char Buff[BytesToBeRead];
    Bytes = -1;

    memset(Buff, 0, BytesToBeRead);
    Bytes = recv(Data->SocketFd, Buff, sizeof(Buff),0);
    if(Bytes <= 0)
    {
        DropClient = true;

        return;
    }

    DupBytes = Bytes;
    
    if(!HeaderDone)
        HeadersAsTokens(std::string(Buff, Bytes));

    if(HeaderDone && !HeadersAreParsed)
    {
        Data->ErrorCode = HeadersAsMap();
        if(Data->ErrorCode != -1)
            return;
        if(Data->Headers.empty())
        {
            Data->ErrorCode = 400;
            return;
        }

        
        // PrintMapT(Data->Headers);
        
    
        std::map<std::string, VEC_T>::iterator It = Data->Headers.find("Host");
        if(It == Data->Headers.end())            
        {
            Data->ErrorCode = 400;
            return;
        }
        if(It->second.size() != 1 || It->second[0].empty())
        {
            Data->ErrorCode = 400;
            return; 
        }
        TrimLine(It->second[0], " \t", " \t");
        if(It->second[0].empty())
        {
            Data->ErrorCode = 400;
            return;
        }
        getRightConfig(It->second[0]);
        std::map<std::string, VEC_T>::iterator it = Data->Headers.find("Cookie"); 
        if(it != Data->Headers.end())
        {
            Data->Cookie = SumStrings(it->second.begin(), it->second.end());
        } 
    }
    if(HeaderDone && HeadersAreParsed)
    {
        if(Data->Method == "POST")
        {
            if(!valid)
            {
                Data->ErrorCode = CheckSomeHeaders();
                if(Data->ErrorCode != -1)
                {
                    return ;
                }
            }
            if(Data->IsFirstOpen)
            {

                Data->ErrorCode = OpenTmpBodyFile();
                if(Data->ErrorCode != -1)
                    return;

            }
            if(!TransferCoding)
            {
                Data->ErrorCode = ParseBody(Buff);
                if(Data->ErrorCode != -1)
                    return;
            }
            else if (TransferCoding)
            {
                Data->ErrorCode = ParseChunkedBody(Buff);
                if(Data->ErrorCode != -1)
                    return;
            }
            if(Data->ContentLength == RecvedBytes)
                ReadyHandleReq = true;
        }
        else {
            ReadyHandleReq = true;
        }
    } 
}

#include <fcntl.h>
#include <ctime>


int   Request::OpenTmpBodyFile()
{
    std::time_t now = std::time(NULL);

    std::tm* tm = std::localtime(&now);
    char Buffer[20];
    std::strftime(Buffer, 20, "%d_%m_%Y_%H_%M_%S", tm);

    Data->UploadFileName = "UPLOAD_TMP";
    Data->UploadFileName += Buffer;
    Data->BodyTmpFileName += Buffer;
    fd = open(Data->BodyTmpFileName.c_str(), O_WRONLY | O_CREAT, 0644);
    if(fd == -1)
    {
        return 500;
    }
    
    Data->IsFirstOpen = false;
    return  -1;
}

int Request::ParseBody(char *Buff)
{
    int ret = -1;
    if(!ssave.empty())
    {
        RecvedBytes = DupBytes - Bytes - 1;

        ret = write(fd,&Buff[Bytes+1], RecvedBytes);
        if(ret < 0)
        {
            return 500;
        }
        if(ret == 0)
        {
            ReadyHandleReq = true;
            return -1;
        }
        ssave = "";
        WriteReturn += ret;

        return -1;
    }
    RecvedBytes += Bytes;
    ret = write(fd,(void *)Buff, Bytes);
    if(ret < 0)
    {
        return 500;
    }
    if(ret == 0)
    {
        ReadyHandleReq = true;
        return -1;
    }
    WriteReturn += ret;
    return -1;
}

int Request::ParseChunkedBody(char *Buff)
{
    int ret = -1;
    if (!IsTime && Bytes + ChunklengthReaded > Chunklength) {
      IsTime = true;
    }
    if (IsTime) 
    {
        if(!ssave.empty())
        {
            ChunkValue.append("\r\n", 2);
 
            ChunkValue.append(&Buff[Bytes + 1], DupBytes - Bytes - 1);
            Bytes = DupBytes - Bytes - 1;
            Bytes += 2;
            ssave = "";
        }
        else 
        {
            ChunkValue.append(Buff, Bytes);
        }
        ChunkValueLength += Bytes;
            

        CheckHexa();
        if (YouNeedToHandleIt)
        {
          int z = GetHexa();
          if(z != -1)
            return z;
          YouNeedToHandleIt = false;
        }
    } 
    else 
    {
        ret = write(fd, (void *)Buff, Bytes);
        if(ret < 0)
        {
            return 500;
        }
        if(ret == 0)
        {
            ReadyHandleReq = true;
            RecvedBytes+=ChunklengthReaded;
            return -1;
        }
        WriteReturn += ret;
        ChunklengthReaded += Bytes;
    }
    return -1;
}

void Request::CheckHexa() 
{
    std::string spe = "\r\n";
    int count = 0;
    std::size_t pos = ChunkValue.find(spe);
    while (pos != std::string::npos && count < 2)
    {
        count++;
        pos = ChunkValue.find(spe, pos + 2);
    }

    if (count >= 2)
      YouNeedToHandleIt = true;
    else
      YouNeedToHandleIt = false;
}

int Request::HexaToLong(std::string &Tmp) 
{
  if(Tmp.find_first_not_of("0123456789abcdef") != std::string::npos)
  {
    return 400;
  }

  std::stringstream ss;

  ss << std::hex << Tmp.data() ;
  ss >> Chunklength;
  return -1;
}


int Request::GetHexa() 
{
    std::string toPush;
    std::string spe("\r\n");
    std::size_t dif0 = 0;
    std::size_t dif1 = 0;
    int ret;
    int j = 0;
    long case_zero =0;
    int to__push__l=0;
    while(1)
    {
        dif0 = 0;
        dif1 = 0;
        if(ChunklengthReaded + ChunkValueLength < Chunklength || !toPush.empty())
        {
            IsTime = false;
            toPush.append(ChunkValue.data() , ChunkValueLength);
            case_zero = ChunklengthReaded; 
            to__push__l += ChunkValueLength;
            ChunklengthReaded += ChunkValueLength;
            ChunkValue="";
            // ChunklengthReaded += toPush.size();
            ret =-1;
            ret = write(fd, toPush.data(), toPush.size());
            if(ret < 0)
            {
                return 500;
            }
            if(ret == 0)
            {
                ReadyHandleReq = true;
                RecvedBytes += case_zero;
                return -1;
            }
            WriteReturn += ret;
            toPush = "";
            ChunkValueLength = 0;
            return -1;
        }
        j++;
        if(Chunklength == -1)
        {

            dif0 = ChunkValue.find(spe);
            if(dif0 == std::string::npos)
            {
                return 500;
            }
        }
        else 
        {
            dif0 = Chunklength - ChunklengthReaded;
        }

        if(dif0 > 0)
        {
            toPush.append(ChunkValue.data(),dif0);
            to__push__l += dif0;
            ChunklengthReaded += dif0;
        }
        dif1 = ChunkValue.find(spe, dif0+2);
        if(dif1 == std::string::npos)
        {
            return 500;
        }

        std::string HexaString(&(ChunkValue.data())[dif0 + 2], &(ChunkValue.data())[dif1]);

        if(-1 != HexaToLong(HexaString))
        {
            return 400;
        }
        
        RecvedBytes += ChunklengthReaded;
        ChunklengthReaded = 0;

        ChunkValue.erase(0,dif1 + 2);
        ChunkValueLength = ChunkValueLength - dif1 - 2;

        if(Chunklength == 0  && ChunkValue == "\r\n")
        {

            ChunkValue = "";
            ChunkValueLength = 0;
        }
    }
    return -1;
}