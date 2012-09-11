#ifndef TIMEMONITOR_H_
#define TIMEMONITOR_H_
#include <boost/scoped_ptr.hpp>
#include <queue>

#include "EventHandler.h"
#include "PriorityQueue.h"
#include "Timer.h"
#include "Log.h"
namespace ACache{
class TimerMonitor
{
public:
    //typedef boost::scoped_ptr<Timer> TimerPtr;
    typedef boost::scoped_ptr<EventHandler> TimerHandlerPtr;	    
    typedef boost::function<void(void)> TimerCallback;    

    TimerMonitor(int event);

    ~TimerMonitor();

    int CreateTimerfd();
        
    void HandleTimer();
    
    void UpdateActiveQueue();

    void ResetTimerfd();

    void ProcessExpiredTimers();

    void ReadTimerfd();

	//FIXME: periodic setting
    void TimerfdSetTimer(int64_t delay_ms/*, bool periodic*/);
 
    int GetTimerfd()
    {
        return timerfd_;
    } 
   
    void AddActiveTimer(Timer* timer)
    {
        //init a timer obj with cb and delay_ms
        active_queue_.Push(timer);
        FILE_LOG(logINFO)<<__FILE__<<" "<<__FUNCTION__<<" "<<__LINE__<<" expired time: "<<timer->GetFormatTime();
    }

    void AddInactiveTimer(Timer* timer) 
    {
        inactive_queue_.Push(timer);
    }

	
    Timer* FirstExpired()
    {
	assert(ActiveTimerCount() > 0);
        return active_queue_.Top();
    }
 
    size_t ActiveTimerCount()
    {
        return active_queue_.Size();
    }

    size_t InactiveTimerCount()
    {
	return inactive_queue_.Size();
    }

    int64_t GetExpiredTime()
    {
        if(active_queue_.Size() > 0)
        {
            return FirstExpired()->GetRemainingTime();
        }
        else
        {
            FILE_LOG(logINFO)<<"file"<<__FILE__<<" line: "<<__LINE__
					<<"timer queue is empty";
            return kDefaultTimeout;
        }
    }
   
    TimerHandlerPtr& GetHandle()
    {
        return handle_;
    } 

private:
    //static int timerfd_ ?
    const static int64_t   kDefaultTimeout = 3000;
    const  int             timerfd_;
    TimerHandlerPtr        handle_;
    PriorityQueue<Timer*>  active_queue_;
	//FIXME: member name isn't appropriate
    PriorityQueue<Timer*>  inactive_queue_;
};
}
#endif
