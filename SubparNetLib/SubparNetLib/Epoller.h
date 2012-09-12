#ifndef EPOLLMANAGER_H_
#define EPOLLMANAGER_H_
#include <map>
#include <vector>
#include <sys/epoll.h>

#include "EventHandler.h"
namespace Subpar
{
class Epoller
{
public:
    typedef std::vector<EventHandler*> EventHandlerVector;
    typedef std::vector<struct epoll_event> EpollEventsVector;
    Epoller();

    ~Epoller();

    int AddEvent(EventHandler& evHandle);

    int DeleteEvent(EventHandler& evHandle);

    int ModifyEvent(EventHandler& evHandle);

    int GetActiveEvents(int timeoutMs, EventHandlerVector& activeEvents);

private:
    static const int    kInitEventVectorSize = 32;     
    int                 epfd_;      
    EpollEventsVector   epoll_events_;
    EventHandlerVector  event_handlers_;
};
}
#endif 
