#include "TcpServer.h"
#include "EventScheduler.h"
#include "Connection.h"

using namespace ACache;

TcpServer::TcpServer(const std::string& name, const std::string& port)
			:host_name_(name),
			host_port_(port),
			listening_(false),
			acceptor_(new Acceptor(atoi(port.c_str()))),
            scheduler_(new EventScheduler),
            evhandle_(new EventHandler(0,0)),
            conn_id_(0)
{
}
	
TcpServer::TcpServer(const std::string& name, const std::string& port, EventScheduler* sheduler)
			:host_name_(name),
			host_port_(port),
			listening_(false),
			acceptor_(new Acceptor(atoi(port.c_str()))),
            scheduler_(sheduler),
            evhandle_(new EventHandler(0,0)),
            conn_id_(0)
{
}

TcpServer::TcpServer::~TcpServer()
{
	//FIXME: release resource
	for(ConnectionMap::iterator it(conns_.begin());
			it != conns_.end(); ++it)
	{
	    ConnectionPtr conn = it->second;
        conn->HandleCloseEvent();
	}	
}

void TcpServer::ListenToThePort()
{
    acceptor_->Listen();
    FILE_LOG(logINFO)<<"TcpServer::ListenToThePort(): listening";
    Listening();
    scheduler_->EnableReading(*evhandle_);
    evhandle_->BindReadCallback(boost::bind(&TcpServer::AcceptNewConnection,this));
    FILE_LOG(logINFO)<<__FILE__<<" "<<__LINE__<<" bind AcceptNewConnection";
}

bool TcpServer::IsListening()
{
    return listening_;
}

void TcpServer::Listening()
{
    evhandle_->SetFd(acceptor_->GetListeningSocket());
    listening_ = true;
}

void TcpServer::EventDrive()
{
   
    FILE_LOG(logINFO)<<__FILE__<<" "<<__LINE__<<" Loop running";
    //FIXME:while(!stop)
    while(!scheduler_->IsQuit())
    {
	    scheduler_->Schedule();
    }
}

void TcpServer::CloseOneConnection(ConnectionPtr& conn_ptr)
{
    conns_.erase(conn_ptr->Name());
    FILE_LOG(logINFO)<<__FILE__<<" "<<__FUNCTION__<<" "<<__LINE__<<" "<<"remove a connection from the server";
}

void TcpServer::AcceptNewConnection()
{
    assert(true == listening_);
    FILE_LOG(logINFO)<<__FILE__<<" "<<__FUNCTION__<<" "<<__LINE__;
    ConnectionPtr connected_ptr = acceptor_->Accept();
    if(NULL == connected_ptr)
    {
        //LOG
        //the program should go on running when this error ocurr
        FILE_LOG(logERROR)<<" error: "<<strerror(errno);
    }
    ++conn_id_;
    //the connection's name
    //host_name_,host_port_,clientname,port
    char buf[32];
    snprintf(buf,sizeof buf,":%s#%d", host_port_.c_str(), conn_id_);
    const std::string key = host_name_ + buf;
	connected_ptr->SetName(key);
    conns_[key] = connected_ptr;
    connected_ptr->InstallScheduler(scheduler_);
    connected_ptr->BindEventCallback();
    scheduler_->BindCloseCallback(boost::bind(&TcpServer::CloseOneConnection, this, _1));
    connected_ptr->ReadyReading();
}  

