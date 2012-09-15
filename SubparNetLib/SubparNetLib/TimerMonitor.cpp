#include "TimerMonitor.h"
#include <boost/bind.hpp>

using namespace Subpar;
using namespace BaseLib;
TimerMonitor::TimerMonitor(int event):timerfd_(CreateTimerfd()),
                                      handle_(new EventHandler(timerfd_, event))
{
	handle_->BindReadCallback(boost::bind(&TimerMonitor::HandleTimer, this));
}

TimerMonitor::~TimerMonitor()
{
    close(timerfd_);
    while(ActiveTimerCount() > 0)
    {
        Timer *timer = active_queue_.Pop();
	delete timer;
    }
}

int TimerMonitor::CreateTimerfd()
{
    int fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);    
    if (-1 == fd)
    {
        FILE_LOG(logERROR)<<"TimerMonitor::CreateTimerfd error :"<<strerror(errno);
        return -1;
    }
    FILE_LOG(logINFO)<<" fd:"<<fd<<" successful";
    return fd;
}
    
void TimerMonitor::HandleTimer()
{
    ReadTimerfd();
       
    ProcessExpiredTimers();

    UpdateActiveQueue();
    
    ResetTimerfd();
}

void TimerMonitor::UpdateActiveQueue()
{
    FILE_LOG(logINFO)<<" inactive timers: "<<InactiveTimerCount();
    for(size_t j = 0; j < InactiveTimerCount(); ++j)
    {
    	inactive_queue_.Top()->UpdateTimer();
	active_queue_.Push(inactive_queue_.Pop());
    }
}

void TimerMonitor::ResetTimerfd()
{
    if(ActiveTimerCount() > 0)
    {
	int64_t timeout = active_queue_.Top()->GetRemainingTime();
        FILE_LOG(logINFO)<<" timeout: "<<timeout;
        TimerfdSetTimer(timeout);
    }
}

void TimerMonitor::ProcessExpiredTimers()
{
    size_t total_timers = ActiveTimerCount();
    size_t i = 0; 
    FILE_LOG(logINFO)<<"total timers :"<<total_timers;
    while(i < total_timers)
    {
        Timer* top = FirstExpired(); 
	Time now(Time::NowTimeUs());
	Time expired(top->GetTime());
	FILE_LOG(logINFO)<<" now: "<<now.GetFormatTime()<<" expired: "<<expired.GetFormatTime();
	if(expired < now)
        {
	    FILE_LOG(logINFO)<<" callback run at ";//FIXME:use format time
	    top->RunTimerCallback();
            //remove the following logic to another loop
            //a loop run a action
            if(top->IsRepeat())
            {
                inactive_queue_.Push(top);
		active_queue_.Pop();                   //
	    }
	    else                                       //
	    {                                          //
		Timer *timer  = active_queue_.Pop();   //
		delete timer;                          //
	    }                                          //
			//release resource; 
			//active_queue_.Pop();                     //
			//next loop for another timer
            ++i;
 	}
	else
	{
	     FILE_LOG(logINFO)<<" finish check timer monitor";
             //end loop, there is no timer expired
             break;		
	}
		
     }
}

void TimerMonitor::ReadTimerfd()
{
    assert(timerfd_ > 0);
    int64_t timers_num;
    int n = read(timerfd_, &timers_num, sizeof(int64_t));
    if(n != sizeof(int64_t))
    {
        FILE_LOG(logERROR)<<" read timerfd error: "<<strerror(errno);
    }
}

//FIXME: periodic setting
void TimerMonitor::TimerfdSetTimer(int64_t delay_us/*, bool periodic*/)
{
    struct itimerspec new_value;
    struct itimerspec old_value;
    bzero(&new_value, sizeof new_value);
    bzero(&old_value, sizeof old_value);
    struct timeval now;
        
    int ret = gettimeofday(&now, NULL);
    if(-1 == ret)
    {
        FILE_LOG(logERROR)<<"TimerQueue::TimerfdSetTime:gettimeofday() error: "<<strerror(errno);
        return;
    }
 
    new_value.it_value.tv_sec  = delay_us / Time::kMicroSecondsPerSecond;
    new_value.it_value.tv_nsec = (delay_us % Time::kMicroSecondsPerSecond) * 1000;
	
    ret = timerfd_settime(timerfd_, 0, &new_value, &old_value);
    if(-1 == ret)
    {
        FILE_LOG(logERROR)<<"TimerQueue::TimerfdSetTime timerfd_settime error: "<<strerror(errno);
        return;
    }
	
    return;
}
