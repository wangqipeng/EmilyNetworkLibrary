#include "Acceptor.h"
#include "Connection.h"
using namespace Subpar;

typedef boost::shared_ptr<Connection> ConnectionPtr;

//listen_sock_ , conn_sock_ = -1;
Acceptor::Acceptor(int port):established_(false),
                             address_(port),
                             listen_sock_(TcpSocket::kInvalidHandle),
			    conn_sock_(TcpSocket::kInvalidHandle)
{
}

Acceptor::Acceptor():established_(false),
                     address_(InetAddress::kInvalidPort),
		     listen_sock_(TcpSocket::kInvalidHandle),
                     conn_sock_(TcpSocket::kInvalidHandle)
{

}

Acceptor::~Acceptor()
{
}

ConnectionPtr Acceptor::Accept()
{
    InetAddress peer(InetAddress::kInvalidPort);
    int connfd = listen_sock_.Accept(peer);
    assert(connfd >= 0);
        
    FILE_LOG(logINFO)<<" established connfd: "<<connfd;      
    ConnectionPtr conn_ptr = EstablishConnection(peer, connfd);
    if(conn_ptr == NULL)
    {
        FILE_LOG(logINFO)<<" Error: "<<strerror(errno);
        //FIXME:may be call closecallback		
	:close(connfd);
    }
    return conn_ptr;
}

ConnectionPtr Acceptor::EstablishConnection(const InetAddress& peer, int connfd)
{
    established_ = true;
    //New Connection
    boost::shared_ptr<Connection> conn_ptr(new Connection(peer,
                                                          address_,
                                                          connfd));
    return conn_ptr;
}
  
void Acceptor::Listen()
{
    int ret = -1;
    ret = listen_sock_.CreateSocket();
    assert(ret == 0);
    
    ret = listen_sock_.SetNonBlocking(true);
    assert(ret == 0);
    //server end should skip TIME_WAIT
    ret = listen_sock_.SetReuseAddr(true);
    assert(ret == 0);

    ret = listen_sock_.Bind(address_);
    assert(ret == 0);

    ret = listen_sock_.Listen();
    assert(ret == 0);
}

