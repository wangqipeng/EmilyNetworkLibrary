#ifndef ECHOSERVER_H_
#define ECHOSERVER_H_
#include "TcpServer.h"
#include "EventScheduler.h"
#include "Connection.h"
#include "Buffer.h"

#include <boost/shared_ptr.hpp>
#include <string>
using namespace ACache;
using namespace std;

class EchoServer
{
public:
    typedef boost::shared_ptr<Connection> ConnectionPtr;
    EchoServer(const string& name, const char* port, EventScheduler* scheduler):scheduler_(scheduler), server_(new TcpServer(name, port, scheduler))
    {
        scheduler_->BindMessageCallback(boost::bind(&EchoServer::MessageLoop, this, _1, _2, _3));
    }

    void MessageLoop(const ConnectionPtr& conn, Buffer *buf, const Time& time)
    {
        FILE_LOG(logINFO)<<__FILE__<<" "<<__LINE__<<" EchoServer::MessageLoop()";
	    //1.extract All data from the buffer
	std::string str(buf->DrainAsString());
        if (str == "quit")
	{
            FILE_LOG(logINFO)<<__FILE__<<" "<<__FUNCTION__<<" "<<__LINE__<<" quit program";
	    scheduler_->Quit();   	
	}
       
        FILE_LOG(logINFO)<<__FILE__<<" "<<__LINE__<<" coming messsage: "<<str<<" size: "<<str.size();
        //2.send it to the peer
        conn->SendMessage(str);
        
        FILE_LOG(logINFO)<<__FILE__<<" "<<__LINE__<<" EchoServer::MessageLoop()";
    }

    ~EchoServer()
    {
        delete scheduler_;
        delete server_;
    }

    void start()
    {
        server_->ListenToThePort();
        server_->EventDrive();
    }

private:
    EventScheduler *scheduler_;
    TcpServer      *server_;
};

#endif
