#ifndef TIMER_H_
#define TIMER_H_
#include <sys/timerfd.h>
#include <time.h>
#include <boost/scoped_ptr.hpp>
#include <boost/function.hpp>
#include <algorithm>
#include <boost/operators.hpp>
#include "Time.h"
namespace Subpar
{
class EventHandler;
class Timer:public boost::less_than_comparable<Timer>
{
public:
    typedef boost::function<void (void)> TimerCallback;

    Timer();

    Timer(int64_t us, int64_t period, const TimerCallback& cb, bool repeat);
   
    Timer(const Timer& timer);
     
    Timer& operator= (const Timer& rhs)
    {
        Timer timer(rhs);
        Swap(timer);
        return *this;
    }
    
    ~Timer();

    void RunTimerCallback()
    {
        timer_callback_();    
    }    
    
	//Get the relative time
    int64_t GetRemainingTime()
    {
        int64_t now = Time::NowTimeUs();
        int64_t expired_time = expired_time_.Value();
        return expired_time > now ? expired_time - now : 0;
    }

    int64_t GetTime()
    {
        return expired_time_.Value();
    }
   
    std::string GetFormatTime()
    {
        return expired_time_.GetFormatTime();
    }

    void UpdateSelf(int64_t new_time)
    {
        expired_time_.UpdateSelf(new_time);
    }

    void UpdateTimer()
    {
	    int64_t new_time = Time::NowTimeUs() + period_;
        expired_time_.UpdateSelf(new_time);
    }

    int64_t GetPeriod()
    {
        return period_;
    }	

    inline bool IsRepeat()
    {
        return repeat_;
    }
private:
    void Swap(Timer& rhs)
    {
        std::swap(repeat_, rhs.repeat_);
        std::swap(period_, rhs.period_);
        std::swap(expired_time_, rhs.expired_time_);
        std::swap(timer_callback_, rhs.timer_callback_);
    }
	
private:
    bool            repeat_;
    int64_t         period_;
    Time            expired_time_;
    TimerCallback   timer_callback_;   
};

inline bool operator < (Timer& lhs, Timer& rhs)
{
    return lhs.GetTime() < rhs.GetTime();
}

}
#endif
