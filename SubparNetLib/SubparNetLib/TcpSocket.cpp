#include "TcpSocket.h"
#include "InetAddress.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/fcntl.h>
#include <boost/implicit_cast.hpp>

typedef struct sockaddr SA;

using namespace Subpar;

TcpSocket::~TcpSocket()
{
    Close();
}
 
void TcpSocket::Close()
{
    if (kInvalidHandle == sockfd_)
    {
        return;
    }

    ::close(sockfd_);
    return;
}

void TcpSocket::ShutdownWrite()
{
    int ret = shutdown(sockfd_, SHUT_WR);
    if (ret < 0)
    {
	FILE_LOG(logERROR)<<" shutdown error: "<<strerror(errno);
    }
}

int TcpSocket::CreateSocket()
{
    int sock = -1;
    if ((sock = ::socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        FILE_LOG(logERROR)<<" create socket error: "<<strerror(errno);
        return -1;
    }

    SetSocket(sock); 
    return 0;
}

int TcpSocket::Bind(InetAddress& addr)
{
    bool ret = addr.IsValid();
    if (false == ret)
    {
        FILE_LOG(logERROR)<<" address is invalid";
        return -1;
    }

    if (-1 == ::bind(sockfd_, (SA*)(&addr.GetAddr()), sizeof(addr.GetAddr())))
    {
        FILE_LOG(logERROR)<<" bind address error: "<<strerror(errno);
	return -1;       
    }

    return 0;
}
//TODO: timeout retry 
int TcpSocket::Connect(InetAddress& addr)
{
    if (-1 == ::connect(sockfd_, (SA *)&addr.GetAddr(), sizeof(addr.GetAddr())))
    {
 
        FILE_LOG(logERROR)<<" connect error: "<<strerror(errno);
        return -1;
    }

    return 0;
}


//TODO:Manage to return reference 
const InetAddress TcpSocket::GetLocalAddress()
{
    if (sockfd_ == kInvalidHandle)
    {
        FILE_LOG(logERROR)<<" sockfd_ is invalid";
        return static_cast<InetAddress>(-1); 
    }
    
    struct sockaddr_in addr;
    bzero(&addr,sizeof addr);
    socklen_t addrlen = sizeof(addr);
    if (-1 == ::getsockname(sockfd_,(SA *)&addr, &addrlen))
    {
        
        FILE_LOG(logERROR)<<" getsockname error: "<<strerror(errno);
        return static_cast<InetAddress>(-1);  
    }

    InetAddress address(addr);
    return address;
}

//TODO:Manage to return reference 
const InetAddress TcpSocket::GetPeerAddress()
{
    if (sockfd_ == kInvalidHandle)
    {
     
        FILE_LOG(logERROR)<<" sockfd_ is invalid";
        return static_cast<InetAddress>(-1); 
    }
    
    struct sockaddr_in addr;
    bzero(&addr,sizeof addr);
    socklen_t addrlen = sizeof(addr);
    if (-1 == ::getpeername(sockfd_,(SA *)&addr, &addrlen))
    {
        FILE_LOG(logERROR)<<" getpeername error: "<<strerror(errno);
        return static_cast<InetAddress>(-1);  
    }

    InetAddress address(addr);
    return address;   
}

int TcpSocket::Listen()
{
    int res = -1;
    if ((res = ::listen(sockfd_, SOMAXCONN)) == -1)
    {
        FILE_LOG(logERROR)<<" listen error: "<<strerror(errno);
        return -1;
    }

    return 0;
}

int TcpSocket::Accept(InetAddress& addr)
{
    int res = -1;
    socklen_t addrlen = sizeof(addr);
    res = ::accept(sockfd_, (SA *)&addr.GetAddr(), &addrlen);
    if (res == -1)
    {
        FILE_LOG(logERROR)<<" accept error: "<<strerror(errno);
        return res;
    }
   
    return res;
}

int TcpSocket::GetSocketError(int sockfd)
{
    int ret = -1;
    int err_code = 0;
    socklen_t len = sizeof(err_code);
    ret = ::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err_code, &len);
    if (-1 == ret)
    {
	return ret;
    }
    else
    {
	return err_code;
    }
}

int TcpSocket::SetReuseAddr(bool on)
{
    const int optval = on ? 1 : 0;
    int res = setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    if (-1 == res)
    {
	FILE_LOG(logERROR)<<" ERROR:"<<strerror(errno);
	return res;
    }	
    return res;
}

int TcpSocket::SetIntOpt(int opt, bool on)
{
    int optval = on ? 1 : 0;
    int res = setsockopt(sockfd_, SOL_SOCKET, opt, &optval, sizeof optval);
    if (res == -1)
    {
        FILE_LOG(logERROR)<<" SetIntopt error: "<<strerror(errno);
        return res; 
    }
    return res;
}

int TcpSocket::SetTimeOpt(int opt, int ms)
{
    int res = -1;
    struct timeval tv;
    tv.tv_sec =  (int)(ms/1000);
    tv.tv_usec = (ms%1000) * 1000000;
    res = ::setsockopt(sockfd_, SOL_SOCKET, opt, &tv, sizeof tv);
    if (res == -1)
    {
        FILE_LOG(logERROR)<<" SetTimeopt error: "<<strerror(errno);
        return res;
    }    
    return res;
}

int TcpSocket::SetTcpNoDelay(bool on)
{
    int res = -1;
    int optval = on ? 1 : 0;
    res = ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof optval);
    if (res == -1)
    {
        FILE_LOG(logERROR)<<" SetTcpNoDelay error: "<<strerror(errno);
        return res;
    }
    
    return res;
}

int TcpSocket::SetSoLinger(bool on, int sec)
{
    int res = -1;
    int optval = on ? 1 : 0;
    struct linger lngr;
    lngr.l_onoff = optval;
    lngr.l_linger = sec;
    res = ::setsockopt(sockfd_, SOL_SOCKET, SO_LINGER, (const void *)(&lngr), sizeof lngr);
    if (res == -1)
    {
        FILE_LOG(logERROR)<<" SetSoLinger error: "<<strerror(errno);
        return res;
    }

    return res;
}

int TcpSocket::SetNonBlocking(bool on)
{
    int res = -1;
    int flags = ::fcntl(sockfd_, F_GETFL, 0);
    if (flags >=0 )
    {
        if(on)
        {
            flags |= O_NONBLOCK;
        }
        else
        {
            flags &= ~O_NONBLOCK; 
        }

        if (fcntl(sockfd_, F_SETFL, flags) >= 0)
        {
            res = 0;
        }
    }
    
    return res;
}


