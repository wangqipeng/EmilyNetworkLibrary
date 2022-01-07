#ifndef INETADDRESS_H_
#define INETADDRESS_H_
#include <boost/any.hpp>
#include <endian.h>
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>

namespace Subpar 
{
class InetAddress 
{
public: 
    typedef struct sockaddr    SA;
    typedef struct sockaddr_in SockAddr;
    static const short kInvalidPort;
	
    InetAddress(const std::string& ip, uint16_t port);

    InetAddress(const SockAddr& address):addr_(address),valid_(true){}

    InetAddress(const std::string& port);

    InetAddress(uint16_t port);
    
    ~InetAddress();
    
    bool IsValid()
    {
	return valid_ == true;
    }

    SockAddr GetAddr() 
    {
        return addr_;
    }

    void NumToStr(const SockAddr& addr, char *ip, size_t size);

    const SockAddr& GetSockAddr() const 
    {
	return addr_; 
    }

    void SetSockAddr(const SockAddr& addr) 
    { 
	addr_ = addr;
    }
private:
    SockAddr  addr_; 
    bool      valid_;
};
}
#endif
