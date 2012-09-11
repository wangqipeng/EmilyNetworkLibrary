#include "EventHandler.h"
#include "Log.h"

using namespace ACache;

EventHandler::EventHandler(int fd, int event):fd_(fd),
                                              waiting_event_(event)
{
}
    
EventHandler::~EventHandler()
{
}

void EventHandler::HandleEvent()
{   
    FILE_LOG(logINFO)<<__FILE__<<" "<<__LINE__<<" active_event_: "<<active_event_;
    
    if (active_event_& (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
    {
        FILE_LOG(logINFO)<<__FILE__<<" "<<__LINE__<<" readcallback_(): ";
        read_callback_();
    }
   
    if(active_event_ & EPOLLOUT)
    {
        write_callback_();
    }
   
    if(active_event_ & (EPOLLERR | EPOLLHUP))
    {
        err_callback_();
        FILE_LOG(logERROR)<<__FILE__<<" "<<__LINE__<<" epoll_wait error on fd "<<GetFd()<<" "<<strerror(errno);
    }
    
}



