#ifndef EVENTHANDLER_H_
#define EVENTHANDLER_H_

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <sys/epoll.h>

namespace ACache{
class Acceptor;
class Connection;
class EventHandler:boost::noncopyable
{
public:
    typedef boost::shared_ptr<Connection> ConnectionPtr;
    typedef boost::function<void(void)> EventCallback;
    typedef boost::function<ConnectionPtr&(void)> AcceptCallback;

    EventHandler(int fd, int event);
 
    ~EventHandler();

    void HandleEvent();

    void RunAcceptCallback()
    {
        accept_callback_();
    }

    void RunReadCallback()
    {
        read_callback_();
    }

    void RunWriteCallback()
    {
	write_callback_();
    }

    void RunCloseCallback()
    {
	close_callback_();
    }

    void RunConnectionCallback()
    {
        conn_callback_();
    }

    void RunErrorCallback()
    {
	    err_callback_();
    }
/*
 * void RunTimeoutCallback()
	{
	    timer_callback_();
	}
*/
    void RunMessageCallback()
    {
	    message_callback_();
    }

    void RunWriteEndCallback()
    {
	    write_end_callback_();
    }

    void BindAcceptCallback(const AcceptCallback& cb)
    {
        accept_callback_ = cb;
    }

    void BindReadCallback(const EventCallback& cb)
    {
        read_callback_ = cb;
    }
    
    void BindWriteCallback(const EventCallback& cb)
    {
        write_callback_ = cb;
    }

    void BindCloseCallback(const EventCallback& cb)
    {
        close_callback_ = cb;
    }

    void BindConnectionCallback(const EventCallback& cb)
    {
        conn_callback_ = cb;
    }

    void BindErrorCallback(const EventCallback& cb)
    {
        err_callback_ = cb;
    }

    void BindMessageCallback(const EventCallback& cb)
    {
	    message_callback_ = cb;
    }

    void BindWriteEndCallback(const EventCallback& cb)
    {
	    write_end_callback_ = cb;
    }
    /*
    void BindTimerCallback(const EventCallback& cb)
    {
	   timer_callback_ = cb;
    }
    */

	bool IsWriting()
	{
	    return active_event_ & EPOLLOUT;
	}

    int GetFd() const 
    {
        return fd_;
    }
    
    void SetFd(int fd)
    {
	    fd_ = fd;
    }

    void SetWaitingEvent(int event)
    {
	    waiting_event_ = event;
    }
    
    int GetWaitingEvent() 
    {
	    return waiting_event_;
    }

    int GetActiveEvent() const
    {
        return active_event_;
    }
   
    void SetActiveEvent(int event)
    {
        active_event_ = event;	        
    }

private:
    int             fd_;
    int             waiting_event_;
    int             active_event_; 

    AcceptCallback  accept_callback_; 
    EventCallback   read_callback_;
    EventCallback   write_callback_;
    EventCallback   close_callback_;
    EventCallback   conn_callback_;
    EventCallback   err_callback_;
    EventCallback   message_callback_;
    EventCallback   write_end_callback_;
	EventCallback   timer_callback_;
};
}
#endif
