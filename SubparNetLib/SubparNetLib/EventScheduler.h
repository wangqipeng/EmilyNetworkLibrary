#ifndef EVENTSCHEDULER_H_
#define EVENTSCHEDULER_H_
#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/function.hpp>
#include <algorithm>
#include <vector>

#include "Epoller.h"
#include "EventHandler.h"
namespace Subpar
{
class TimerMonitor;
class Connection;
class Buffer;
class Timer;
class Time;
class EventScheduler: public boost::noncopyable
{
public:
    typedef boost::scoped_ptr<EventHandler> EventHandlerPtr;
    typedef std::vector<EventHandler*> EventHandlerVector;
    typedef boost::scoped_ptr<TimerMonitor> TimerMonitorPtr;
    typedef boost::function<void(void)> TimerCallback;
    typedef boost::shared_ptr<Connection> ConnectionPtr;
    typedef boost::function<void(const ConnectionPtr&, Buffer*, const Time&)> MessageCallback;
    typedef boost::function<void(ConnectionPtr&)> CloseCallback;
	
    EventScheduler();
 
    ~EventScheduler();

    void Schedule();
    
    void EnableWriting(EventHandler& handle);

    void EnableReading(EventHandler& handle);
   
    void DisableWriting(EventHandler& handle);
    
    void DisableReading(EventHandler& handle);
   
    //FIXME:install a kClearEvent is somewhat strange
    void DisableAllEvent(EventHandler& handle);
    
    //TODO:Merge the install/uninstall_event() into update()
    void InstallEvent(EventHandler& handle, int event);
    
    void UninstallEvent(EventHandler& handle, int event);
 
    bool IsQuit()
    {
	return quit_;
    }

    void Quit()
    {
	quit_ = true;
    }

    void RunAppMsgCallback(const ConnectionPtr& conn, Buffer *buf, const Time& time)
    {
        msg_callback_(conn, buf, time);
    }

    void BindMessageCallback(const MessageCallback& msg_cb)
    {
        msg_callback_ = msg_cb;    
    }

    void BindCloseCallback(const CloseCallback& close_cb)
    {
	close_callback_ = close_cb;
    }

    void RunCloseCallback(ConnectionPtr conn_ptr)
    {
	close_callback_(conn_ptr);
    }

    void RegisterOnceTimer(const int64_t delay_us, const TimerCallback& cb);
    
    void RegisterPeriodicTimer(const int64_t period, const TimerCallback& cb);

    void AddEvent(EventHandler& ev_handle);
     
    void DeleteEvent(EventHandler& ev_handle);
      
    void ModifyEvent(EventHandler& ev_handle);
    
    void GetActiveEvents(int timeout,  EventHandlerVector& handle_vector);
private:
    static const int   kReadEvent;
    static const int   kWriteEvent;
    static const int   kClearEvent;

    bool               quit_;
    Epoller            poller_;
    EventHandlerVector handle_vector_;
    TimerMonitorPtr    timer_monitor_;
    MessageCallback    msg_callback_;
    CloseCallback      close_callback_;
};
}
#endif
