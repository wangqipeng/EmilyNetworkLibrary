#include "InetAddress.h"

using namespace Subpar;

const short InetAddress::kInvalidPort = -1;

InetAddress::InetAddress(const std::string& ip, uint16_t port)
{
    bzero(&addr_,sizeof(addr_));
    if(port > kInvalidPort)
    {
        addr_.sin_family = AF_INET;   
        addr_.sin_port = htobe16(port);
    
        if(::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr) == -1)
        {
            fprintf(stderr,"ERR:inet_pton\n");
            valid_ = false;  
        }
        else
        {
            valid_ = true;
        }
    }
    else
    {
        valid_ = false;
    }      
}

InetAddress::InetAddress(const std::string& port)
{
    short port_num = atoi(port.c_str());
    bzero(&addr_,sizeof(addr_));
    if(port_num > kInvalidPort)
    {
        addr_.sin_family = AF_INET;
        addr_.sin_addr.s_addr = htobe32(INADDR_ANY);
        addr_.sin_port = htobe16(port_num);
        valid_ = true;
    }
    else
    {
        valid_ = false;
    }
}

InetAddress::InetAddress(uint16_t port)
{
    bzero(&addr_,sizeof(addr_));
    if(port > kInvalidPort)
    {
        addr_.sin_family = AF_INET;
        addr_.sin_addr.s_addr = htobe32(INADDR_ANY);
        addr_.sin_port = htons(port);
        valid_ = true;
    }
    else
    {
        valid_ = false;
    }
}

InetAddress::~InetAddress()
{

}

void InetAddress::NumToStr(const SockAddr& addr, char* buf, size_t size)
{
    char host[INET_ADDRSTRLEN] = "INVALID";
    ::inet_ntop(AF_INET, &addr.sin_addr, host,sizeof(host));
    uint16_t port = be16toh(addr.sin_port);
    snprintf(buf, size,"%s,%u",host,port);
}

