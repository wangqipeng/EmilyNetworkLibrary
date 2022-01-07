#ifndef TIME_H_
#define TIME_H_
#include <sys/time.h>
#include <boost/operators.hpp>
#include "Log.h"
namespace Subpar
{
class Time: public boost::less_than_comparable<Time>
{
public:
    const static int kMicroSecondsPerSecond = 1000 * 1000;

    Time();

    explicit Time(int64_t abstime);

    Time(const Time& time);

    ~Time();

    static int64_t  NowTimeUs();

    static std::string GetFormatNowTime(int64_t abstime);

    std::string GetFormatTime();

    void UpdateSelf(int64_t new_time)
    {
        time_value_ = new_time;
    }
   
    inline int64_t Value()
    {
        return time_value_;
    }  
private:
    int64_t time_value_;
};

//arith and relational operator should be defined non-member function
inline bool operator< (Time& lhs, Time& rhs)
{
    return lhs.Value() < rhs.Value();
}
}
#endif
