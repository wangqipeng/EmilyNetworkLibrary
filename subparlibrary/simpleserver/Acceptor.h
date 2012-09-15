#ifndef ACCEPTOR_H_
#define ACCEPTOR_H_
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <sys/socket.h>
#include <cassert>
#include "TcpSocket.h"
#include "InetAddress.h"

namespace ACache
{
class Connection;
class Acceptor: boost::noncopyable
{
public:
    //typedef boost::function<void (TcpSocket, InetAddress)> CallbackNewConnection;
    typedef boost::shared_ptr<Connection> ConnectionPtr;
    
    explicit Acceptor(int port);

    Acceptor();

    ~Acceptor();

    ConnectionPtr Accept();
    
    void ConfigNewConnection(ConnectionPtr conn_ptr);

    ConnectionPtr EstablishConnection(const InetAddress& peer, int connfd);
    
    void SetConnSocket(int connfd)
    {
        conn_sock_.SetSocket(connfd);
    }

    int GetConnSocket() const
    {
	return conn_sock_.GetSocket();
    }

    int GetListeningSocket() const
    {
	return listen_sock_.GetSocket();
    }
    
    void Listen();
    
private:
    bool           established_;
    InetAddress    address_;
    TcpSocket      listen_sock_;
    TcpSocket      conn_sock_;
};
}
#endif
