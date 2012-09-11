#ifndef CONNECTOR_H_
#define CONNECTOR_H_
#include <string>
#include "Connector.h"
#include "TcpSocket.h"
#include "InetAddress.h"

namespace ACache
{
class TcpSocket;
class InetAddress;
class Connector
{
public:
    typedef boost::shared_ptr<Connection> ConnectionPtr;

    Connector(const std::string& ip, uint16_t port):server_addr_(ip, port)
    {
    }

    ~Connector()
    {
       
    }

    const ConnectionPtr& Connect()
    {
	sock_.CreateSocket();
	sock_.Connect(svr_addr_);
        //ip:port(server address, connfd)
        ConnectionPtr conn = new Connection(svr_addr_, sock_.GetLocalAddress(), sock_);
        FILE_LOG(logINFO)<<__FILE__<<" "<<__LINE__<<" a new connection establish";
        return conn;
    }

    void ShutDown
    {
        //close or shutdown
    }
private:
    TcpSocket     sock_;
    InetAddress   srv_addr_;
    ConnectionPtr conn_;
};
}
#endif
