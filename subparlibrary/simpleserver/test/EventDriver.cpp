#include "EventDriver.h"
#include "EventScheduler.h"

using namespace ACache;

EventDriver::EventDriver():looping_(false)
{
}
/*(
EventDriver::EventDriver(const EventScheduler& schedu):looping_(false),
		                                               scheduler_(schedu)
{	
}
*/
EventDriver::~EventDriver()
{
}

void EventDriver::Drive()
{
    while(looping_)
    {
        scheduler_->Schedule();
    }
    OffLoopingFlag();
}

