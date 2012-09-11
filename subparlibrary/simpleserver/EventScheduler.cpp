#include "EventScheduler.h"
#include "TimerMonitor.h"
#include "Time.h"
#include <sys/time.h>
#include <sys/epoll.h>
using namespace ACache;

const int EventScheduler::kReadEvent  = EPOLLIN | EPOLLPRI;
const int EventScheduler::kWriteEvent = EPOLLOUT;
const int EventScheduler::kClearEvent = 0;

EventScheduler::EventScheduler():quit_(false), timer_monitor_(new TimerMonitor(0))
{
}

EventScheduler::~EventScheduler()
{
}

void EventScheduler::Schedule()
{
    GetActiveEvents(-1/*timeoutMs*/, handle_vector_);
    EventHandlerVector::iterator ibeg = handle_vector_.begin();
    EventHandlerVector::iterator iend = handle_vector_.end();
    FILE_LOG(logINFO)<<"file: "<<__FILE__<<" line: "<<__LINE__
			<<" events number: "<<handle_vector_.size();
    for(;ibeg != iend; ++ibeg)
    {
        FILE_LOG(logINFO)<<__FILE__<<" "<<__LINE__<<" active fd: "<<(*ibeg)->GetFd();
        (*ibeg)->HandleEvent();
    }
	//must clear the vector for the next loop
    handle_vector_.clear();
}

void EventScheduler:: RegisterOnceTimer(const int64_t delay_us, const TimerCallback& cb)
{
    Timer *timer(new Timer(delay_us + Time::NowTimeUs(), delay_us,cb, false));
    //FIXME:logic not clear
 	if(timer_monitor_->ActiveTimerCount() > 0)
    {
	    if(*timer < *timer_monitor_->FirstExpired())
	    {
	        timer_monitor_->TimerfdSetTimer(delay_us);
		    //EnableReading(*timer_monitor_->GetHandle());
	    }
    }
    else
    {
	    timer_monitor_->TimerfdSetTimer(delay_us);
	    EnableReading(*timer_monitor_->GetHandle());
    }	
    timer_monitor_->AddActiveTimer(timer);
}

void EventScheduler::RegisterPeriodicTimer(const int64_t period, const TimerCallback& cb)
{
    FILE_LOG(logINFO)<<__FILE__<<" "<<__FUNCTION__<<" "<<__LINE__<<" nowtimeus: "<<Time::GetFormatNowTime(Time::NowTimeUs());
    Timer *timer(new Timer(Time::NowTimeUs() + period, period, cb, true));
    //FIXME:logic not clear
 	if(timer_monitor_->ActiveTimerCount() > 0)
    {
	    if(*timer < *timer_monitor_->FirstExpired())
	    {
            timer_monitor_->TimerfdSetTimer(period);
            //EnableReading(*timer_monitor_->GetHandle());
            FILE_LOG(logINFO)<<__FILE__<<" "<<__FUNCTION__<<" "<<__LINE__;
        }
    }
    else
    {
        FILE_LOG(logINFO)<<__FILE__<<" "<<__LINE__<<" set timerfd and enable reading";
	    timer_monitor_->TimerfdSetTimer(period);
        EnableReading(*timer_monitor_->GetHandle());
    }
    timer_monitor_->AddActiveTimer(timer);
}

void EventScheduler::EnableWriting(EventHandler& handle)
{
    int waiting_event = handle.GetWaitingEvent();
    if(0 == waiting_event)
    {
	    InstallEvent(handle, kWriteEvent);
	    AddEvent(handle);
    }
    else if(waiting_event != EPOLLOUT)
    {
	    InstallEvent(handle, kWriteEvent);
	    ModifyEvent(handle);
    }
    else
    {
	    FILE_LOG(logERROR)<<"file :"<<__FILE__<<"line: "<<__LINE__
				<<"the handle already has waiting to write";
    }
}

void EventScheduler::EnableReading(EventHandler& handle)
{
    int waiting_event = handle.GetWaitingEvent();
    if(0 == waiting_event)
    {
        FILE_LOG(logINFO)<<"file: "<<__FILE__<<" line: "<<__LINE__
				<<" enableread";
	InstallEvent(handle, kReadEvent);
	AddEvent(handle);
    } 
    else if(waiting_event != EPOLLIN)
    {
        InstallEvent(handle, kReadEvent);
	ModifyEvent(handle);
    }
    else
    {
	//LOG
        FILE_LOG(logERROR)<<"file: "<<__FILE__<<"line: "<<__LINE__
				<<"the handle already has waiting to read";
    }
}

void EventScheduler::DisableWriting(EventHandler& handle)
{
    int waiting_event = handle.GetWaitingEvent();
    if(waiting_event == EPOLLOUT)
    {
	    UninstallEvent(handle, kWriteEvent);
	    ModifyEvent(handle);
    }
    else
    {
	    FILE_LOG(logERROR)<<"file: "<<__FILE__<<"line: "<<__LINE__
				<<"the handle already has waiting to write";
    }
}

void EventScheduler::DisableReading(EventHandler& handle)
{
    int waiting_event = handle.GetWaitingEvent();
    if(waiting_event == EPOLLIN)
    {
	    UninstallEvent(handle, kReadEvent);
	    ModifyEvent(handle);
    }
    else
    {
	    FILE_LOG(logERROR)<<"file: "<<__FILE__<<"line: "<<__LINE__
				<<"the handle already has been disabling reading";
    }
}

//FIXME:install a kClearEvent is somewhat strange
void EventScheduler::DisableAllEvent(EventHandler& handle)
{
    //int waiting_event = handle.GetWaitingEvent();
    InstallEvent(handle, kClearEvent);
    ModifyEvent(handle);
}

//TODO:Merge the install/uninstall_event() into update()
void EventScheduler::InstallEvent(EventHandler& handle, int event)
{
    int waiting_event = handle.GetWaitingEvent();
    waiting_event |= event;
    handle.SetWaitingEvent(waiting_event);
}

void EventScheduler::UninstallEvent(EventHandler& handle, int event)
{
    int waiting_event = handle.GetWaitingEvent();
    waiting_event &= ~event;
    handle.SetWaitingEvent(waiting_event);
}

void EventScheduler::AddEvent(EventHandler& ev_handle)
{
    poller_.AddEvent(ev_handle);    
}
 
void EventScheduler:: DeleteEvent(EventHandler& ev_handle)
{
    poller_.DeleteEvent(ev_handle);
} 
  
void EventScheduler::ModifyEvent(EventHandler& ev_handle)
{
    poller_.ModifyEvent(ev_handle);
}

void EventScheduler::GetActiveEvents(int timeout, EventHandlerVector& handle_vector)
{
    poller_.GetActiveEvents(timeout, handle_vector);
}

