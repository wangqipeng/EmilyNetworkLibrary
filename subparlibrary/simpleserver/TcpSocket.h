#ifndef TCPSOCKET_H_
#define TCPSOCKET_H_
#include <boost/noncopyable.hpp>
#include "Log.h"

namespace ACache {
class InetAddress;
class TcpSocket:boost::noncopyable
{
public:
    static const int kInvalidHandle = -1;

    TcpSocket():sockfd_(kInvalidHandle){}

    explicit TcpSocket(int sfd):sockfd_(sfd){}
    
    ~TcpSocket();

    void Close();

    void ShutdownWrite();

    int CreateSocket();

    int Bind(InetAddress& addr);

    int Connect(InetAddress& addr);
     
    int Listen();

    int Accept(InetAddress& addr);

    const InetAddress GetLocalAddress();

    const InetAddress GetPeerAddress();

    int GetSocket() const
    {
        return sockfd_;
    }

    int GetSocketError(int sockfd);

    int SetReuseAddr(bool on);

    int SetIntOpt(int opt, bool on);

    int SetTimeOpt(int opt, int ms);

    int SetTcpNoDelay(bool on);

    int SetSoLinger(bool on, int sec);

    int SetNonBlocking(bool on);

    void SetSocket(int sock)
    {
        sockfd_ = sock;
    }
private:    
    int sockfd_;
};
}
#endif 
