#include "Timer.h"

using namespace ACache;

Timer::Timer():repeat_(false), period_(0),expired_time_(0)
{
}

Timer::Timer(int64_t us, int64_t period, const TimerCallback& cb, bool repeat):repeat_(repeat),
                                                                              period_(period),
                                                                              expired_time_(us), 
                                                                              timer_callback_(cb)
{
}
   
Timer::Timer(const Timer& timer):repeat_(timer.repeat_),
	                        period_(timer.period_),	
                                expired_time_(timer.expired_time_), 
                                timer_callback_(timer.timer_callback_)
{
}

Timer::~Timer()
{
}
