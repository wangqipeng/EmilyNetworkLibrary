#include "Epoller.h"
#include "Log.h"

#include <errno.h>
#include <assert.h>
using namespace ACache;

Epoller::Epoller()
    :epfd_(epoll_create1(EPOLL_CLOEXEC)),epoll_events_(kInitEventVectorSize)   
{}

Epoller::~Epoller()
{
    close(epfd_);
}

int Epoller::AddEvent(EventHandler& ev_handle)
{
    struct epoll_event ev;
    bzero(&ev,sizeof ev);

    ev.events = ev_handle.GetWaitingEvent(); 
    ev.data.ptr = &ev_handle;
    //assert(ev.events != 0);
    int res = -1;
    FILE_LOG(logINFO)<<"file: "<<__FILE__<<" line: "<<__LINE__<<"add fd: "<<ev_handle.GetFd();
    res= epoll_ctl(epfd_, EPOLL_CTL_ADD, ev_handle.GetFd(), &ev);
    if(-1 == res)
    {
        FILE_LOG(logERROR)<<"file: "<<__FILE__<<" line: "<<__LINE__<<"add fd error: "<<strerror(errno);
        return -1;   
    }
 
    return 0;
}

int Epoller::DeleteEvent(EventHandler& ev_handle)
{
    struct epoll_event ev;
    bzero(&ev, sizeof ev);

    ev.data.ptr = &ev_handle;
    ev.events = ev_handle.GetWaitingEvent();
    assert(ev.data.ptr != 0);
 
    int res = -1;
    res = epoll_ctl(epfd_, EPOLL_CTL_DEL, ev_handle.GetFd(),&ev);
    if(-1 == res)
    {
        FILE_LOG(logINFO)<<"file: "<<__FILE__<<" line: "<<__LINE__<<" error: "<<strerror(errno);
        return -1;  
    }
    
    return 0;
}

int Epoller::ModifyEvent(EventHandler& ev_handle)
{
    struct epoll_event ev;
    bzero(&ev,sizeof ev);

    ev.events = ev_handle.GetWaitingEvent();
    ev.data.ptr = &ev_handle;
    assert(ev.events != 0);

    int res = -1;

    res = epoll_ctl(epfd_, EPOLL_CTL_MOD, ev_handle.GetFd(), &ev);
    if(-1 == res)
    {
	FILE_LOG(logERROR)<<"Epoller::ModifyEvent ERROR "<<strerror(errno)<<"line: "<<__LINE__;
        return -1;
    }

    return 0;  
}

int Epoller::GetActiveEvents(int timeout_ms, EventHandlerVector& active_events)
{
    int events_num = -1;
	events_num =::epoll_wait(epfd_, &(*epoll_events_.begin()), static_cast<int>(epoll_events_.size()), timeout_ms);
	if(events_num > 0)
    {
        for(int i = 0; i < events_num; i++)
        {
            //Initialize the EventHandler with epoll_events_ which returned by epoll_wait()
            EventHandler* ev_handler = static_cast<EventHandler *>(epoll_events_[i].data.ptr);
            ev_handler->SetActiveEvent(epoll_events_[i].events);
            active_events.push_back(ev_handler);
        }

        size_t size = epoll_events_.size();
        if(static_cast<size_t>(events_num) == size) 
        {
            epoll_events_.resize(size * 2); 
        }
    }
    else if(events_num == 0)
    {
        FILE_LOG(logINFO)<<"nothing events active";
    }
    else
    {
        FILE_LOG(logERROR)<<"epoll_wait error"<<strerror(errno);
        return -1;
    }
 
    return 0; 
}



