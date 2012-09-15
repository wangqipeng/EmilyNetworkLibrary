#ifndef EVENTDRIVER_H_
#define EVENTDRIVER_H_
#include <boost/noncopyable.hpp>

#include "EventScheduler.h"
namespace ACache 
{
class EventDriver:boost::noncopyable
{
public:
    EventDriver();
   
	//explicit EventDriver(const EventScheduler& schedule);

    ~EventDriver();

    void Drive();

    bool IsLooping()
    {
        return looping_;
    }

	void UpLoopingFlag()
	{
	    looping_ = true;
	}

    void OffLoopingFlag()
    {
        looping_ = false;
    }
private:
    bool           looping_;	
    EventScheduler *scheduler_;
};
}
#endif
