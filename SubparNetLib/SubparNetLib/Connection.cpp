#include "Connection.h"
#include "EventHandler.h"
#include "Time.h"
#include "EventScheduler.h"
#include <stdlib.h>
using namespace Subpar;

Connection::Connection(const InetAddress& peer,const InetAddress& local, const int sockfd):state_(kConnecting),
		                                                                           peer_(peer),
		                                                                           local_(local),
									                   sock_(sockfd),
								                           handle_(new EventHandler(sockfd, 0)),
										           scheduler_(new EventScheduler)
{
    handle_->BindReadCallback(boost::bind(&Connection::HandleReadEvent, this));
    handle_->BindWriteCallback(boost::bind(&Connection::HandleWriteEvent, this));
    handle_->BindCloseCallback(boost::bind(&Connection::HandleCloseEvent, this));
    handle_->BindErrorCallback(boost::bind(&Connection::HandleErrorEvent, this));   
}


Connection::~Connection()
{
	//this way to destory a connection maybe hazard
	//FIXME
    scheduler_->DisableAllEvent(*handle_);
    scheduler_->DeleteEvent(*handle_);
}

void Connection::SendMessage(std::string& str)
{
    size_t len = str.size();
    SendBytesStream((void*)(str.c_str()), len);
}
 
void Connection::SendBytesStream(void *buf, size_t len)
{
    //if the outbuffer is empty, then send the bytes directly
    //else enqueue the bytes in the outbuffer
    size_t nwrote   = -1;
    size_t readable = outbuffer_.ReadableBytes();
    if(0 == readable)
    {
        FILE_LOG(logINFO)<<" readable bytes: "<<readable;
        nwrote = ::write(sock_.GetSocket(), buf, len);
        if(nwrote >= 0)
        {
            if(nwrote <= len)
            {
                //FIXME:LOG<<"i'm going to wrote more data";
                FILE_LOG(logINFO)<<" write "<<nwrote<<" bytes";
            }
                //write complete
            else if(kWriteEnd == state_) 
            {
				//FIXME
                handle_->RunWriteEndCallback();//write_end_callback_();
                FILE_LOG(logINFO)<<" write "<<nwrote<<" bytes";
	    }
          
        }
        else
        {
            if(errno != EWOULDBLOCK)
            { 
                //LOG_ERROR<<"Connection::SendBytesStream error";
                FILE_LOG(logERROR)<<" Error: "<<strerror(errno);
            }
        }
    }
  
    if(nwrote < len)
    {
        FILE_LOG(logINFO)<<" write "<<nwrote<<" bytes";
        outbuffer_.Append(static_cast<const char *>(buf) + nwrote, len - nwrote);
        scheduler_->EnableWriting(*handle_);
    }
}
   
void Connection::HandleReadEvent()
{
    char stackbuf[65536];
    struct iovec vec[2];
    //TODO:WriteableBytes();
    size_t writeable_bytes = inbuffer_.WriteableBytes();
		
    vec[0].iov_base = inbuffer_.GetWriteStart();
    vec[0].iov_len  = writeable_bytes;
    vec[1].iov_base = stackbuf;
    vec[1].iov_len  = sizeof stackbuf;

    size_t total = readv(handle_->GetFd(), vec, 2);
    //FIXME:the function name:Produce is somewhat trikey 
    //writeTobuffer
    inbuffer_.WriteBuffer(stackbuf, total, writeable_bytes);
    if(total > 0)
    {
        //FIXME:fix the time
        const Time time(Time::NowTimeUs());
        FILE_LOG(logINFO)<<" run message callback ";	
        scheduler_->RunAppMsgCallback(shared_from_this(), &inbuffer_, time);    
    }
    else if(0 == total)
    {
	    //TODO:immediately
	    //readable event(fd) active, but the input queue of tcp protocol 
	    //stack that referenced by the socket fd is empty, so it's should be a FIN 
	    //segment coming, at this condition, we close the sockfd.i'm not very sure     
        //FIXME
	HandleCloseEvent();
    }
    else
    {
        FILE_LOG(logERROR)<<" error: "<<strerror(errno);		
    }
}
 
void Connection::HandleWriteEvent() 
{
    size_t n = 0;
    char   *buf = outbuffer_.GetReadStart();
    size_t readable_bytes = outbuffer_.ReadableBytes();
    n = ::write(handle_->GetFd(), buf, readable_bytes);
    if(n > 0)
    {
        outbuffer_.UpdateReadableSize(n);
        if(0 == outbuffer_.ReadableBytes())
        {
            //because the outbuffer is empty, so
	    //we must disable writing events to avoid busy loop
	    scheduler_->DisableWriting(*handle_); 
            if(close_in_server_)
            {
	        close_in_server_(shared_from_this());//write_end_callback_();
            }
			
	    if(kDisConnecting == state_)
	    {
	        ShutdownWrite();
	    }
        }
        else
        {
            FILE_LOG(logINFO)<<" write more data";
        }
    }
    else
    {
        FILE_LOG(logERROR)<<" Connection::HandleWriteEvent error";
    }
}

void Connection::ShutdownWrite()
{
    if(!handle_->IsWriting())//not writing)
    {
	sock_.ShutdownWrite();
    }
}

void Connection::SetTcpNoDelay(bool on)
{
    sock_.SetTcpNoDelay(on);
}

void Connection::HandleCloseEvent()
{
    //1.set state to disconnect
    SetConnState(kDisConnecting);
	
    //2.disable all waiting events of the handle
    scheduler_->DisableAllEvent(*handle_);
	
    //3.remove the connection from the server connection vector
    scheduler_->RunCloseCallback(shared_from_this());
	
    //4.remove the handle from the epoll 
    scheduler_->DeleteEvent(*handle_);
}

void Connection::HandleErrorEvent()
{
    int err = sock_.GetSocketError(sock_.GetSocket()); 
    FILE_LOG(logERROR)<<" Error: "<<strerror(err);
}

void Connection::BindEventCallback()
{
    FILE_LOG(logINFO);
    handle_->BindReadCallback(boost::bind(&Connection::HandleReadEvent, this));
    //close a connection is the server's job
    //but we should avoid too many callback.
    //control the amount of the callback is a design policy 
    scheduler_->BindCloseCallback(boost::bind(&Connection::HandleCloseEvent, this));
}

void Connection::ReadyReading()
{
    //enable reading
    scheduler_->EnableReading(*handle_);
}

