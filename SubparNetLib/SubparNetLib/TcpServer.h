#ifndef TCPSERVER_H_
#define TCPSERVER_H_
#include <sys/epoll.h>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

//using namespace __gnu_cxx;
//#include <ext/hash_map>
#include <string>
#include <map>
#include "Acceptor.h"

namespace Subpar
{
class InetAddress;
class Acceptor;
class EventHandler;
class EventScheduler;
class Connection;
class TcpServer:boost::noncopyable
{
public:
    typedef boost::shared_ptr<Connection> ConnectionPtr;
    typedef std::map<std::string, ConnectionPtr> ConnectionMap;
    typedef boost::function<void (int sockFd, 
                                  const InetAddress&)> NewConnCallback;
    TcpServer(const std::string& name, const std::string& port);
  
    TcpServer(const std::string& name, const std::string& port, EventScheduler* scheduler);
			
    ~TcpServer();

    void ListenToThePort();
    
    bool IsListening();
    
    void Listening();

	std::string name() const
	{
	    return host_name_;
	}

    void EventDrive();
      
    void AcceptNewConnection();

	void CloseOneConnection(ConnectionPtr& conn_ptr);
 	
private:
    const std::string                 host_name_;
    const std::string                 host_port_;
    bool                              listening_;	
    boost::scoped_ptr<Acceptor>       acceptor_;
    EventScheduler                    *scheduler_;
    boost::scoped_ptr<EventHandler>   evhandle_;
    ConnectionMap                     conns_;
    uint32_t                          conn_id_;
};
} 
#endif
