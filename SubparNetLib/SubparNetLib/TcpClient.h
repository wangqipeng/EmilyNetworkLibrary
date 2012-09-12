#ifndef TCPCLIENT_H_
#define TCPCLIENT_H_
#include <string>
#include <boost/scoped_ptr.hpp>

#include "Connector.h"
#include "Connection.h"
#include "EventScheduler.h"
namespace Subpar
{
//TODO:A Client class, used by application
class TcpClient
{
public:
    typedef boost::scoped_ptr<Connector> ConnectorPtr;
    typedef boost::scoped_ptr<EventScheduler> SchedulerPtr;
    TcpClient(const std::string& ip, uint16_t port):connect_(new Connector(ip, port))
    {
    }

    ~TcpClient();

     
   
    void StartConnect()
    {
        FILE_LOG(logERROR)<<__FILE__<<" "<<__LINE__<<" StartConnect";
	connect_->Connect();    
    }

    void Disconnect()
    {
        connect_->Shutdown();	
    }

    void EventDriver()
    {
        while(!scheduler_->IsQuit())
        {
            scheduler_->Schedule();
        }
    }
    
private:
    MessageCallback msg_callback_; 
   
    ConnectorPtr   connect_;
    SchedulerPtr   scheduler_; 
};
}
#endif
