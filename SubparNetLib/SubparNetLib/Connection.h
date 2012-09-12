#ifndef CONNECTION_H_
#define CONNECTION_H_
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/scoped_ptr.hpp>
#include <string>

#include <stdio.h>
#include <errno.h>
#include <sys/uio.h>

#include "InetAddress.h"
#include "TcpSocket.h"
#include "Buffer.h"

namespace Subpar 
{
class EventScheduler;
class EventHandler;
class Connection:public boost::enable_shared_from_this<Connection>,
                 boost::noncopyable 
{
public:
    typedef boost::shared_ptr<Connection> ConnectionPtr;
	typedef boost::function<void(ConnectionPtr)> CloseCallback;
    typedef enum 
    {
        kUnconnect,
        kConnecting,
        kEstablish,
        kDisConnecting,
	    kWriteEnd	
    }ConnState;

    Connection(const InetAddress& peer, const InetAddress& local, int sockfd);

    ~Connection();

    void InstallScheduler(EventScheduler* scheduler)
    {
        scheduler_ = scheduler;
    }

    void SendMessage(std::string& str); 

    void SendBytesStream(void *buf, size_t len);
       
    void HandleReadEvent();
     
    void HandleWriteEvent(); 
    
    void HandleMessageEvent();

    void HandleCloseEvent();
	
    void ShutdownWrite();

    void SetTcpNoDelay(bool on);

    void HandleErrorEvent();

    void BindEventCallback();

    void ReadyReading();
 
    void SetName(const std::string& name)
    {
	    name_ = name;
    }

    std::string Name()
    {
	    return name_;
    }

    void SetConnState(ConnState s)
    {
	    state_ = s;
    }
private:
    typedef boost::function<void(const ConnectionPtr&)> AppConnectionCallback;
    typedef boost::function<void(const ConnectionPtr&,  Buffer&)> MessageCallback;
    AppConnectionCallback conn_callback_;
    MessageCallback  msg_callback_;
	CloseCallback    close_in_server_;
	std::string      name_;
    ConnState        state_;
    Buffer           outbuffer_;
    Buffer           inbuffer_;
    InetAddress      peer_;
    InetAddress      local_;
    TcpSocket        sock_;
    EventHandler     *handle_;
    EventScheduler   *scheduler_;
};
}
#endif
