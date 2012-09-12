#include "Time.h"
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
using namespace Subpar;

Time::Time():time_value_(0)
{}

Time::Time(int64_t abstime):time_value_(abstime)
{}

Time::Time(const Time& time):time_value_(time.time_value_)
{}

Time::~Time(){}

int64_t Time::NowTimeUs()
{
    struct timeval tv;
    bzero(&tv, sizeof tv);
    int ret = -1;
    ret = gettimeofday(&tv, NULL);
    if (-1 == ret)
    {
        FILE_LOG(logERROR)<<"Time::NowTimeUs error :"<<strerror(errno);
        return -1;
    }
            
    return tv.tv_sec * kMicroSecondsPerSecond + tv.tv_usec;
}

std::string Time::GetFormatNowTime(int64_t abstime)
{
    assert(abstime > 0);
    char buffer[32] = {0};
    time_t seconds = static_cast<time_t>(abstime / kMicroSecondsPerSecond);
    int microseconds = static_cast<int>(abstime % kMicroSecondsPerSecond);
    struct tm tm_time;
    bzero(&tm_time, sizeof tm_time);

    gmtime_r(&seconds, &tm_time);

    snprintf(buffer, sizeof(buffer), "%4d%02d%02d %02d:%02d:%02d.%06d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, microseconds);
    return buffer;
}

std::string Time::GetFormatTime()
{
    assert(time_value_ > 0);
    char buffer[32] = {0};
    time_t seconds = static_cast<time_t>(time_value_ / kMicroSecondsPerSecond);
    int microseconds = static_cast<int>(time_value_ % kMicroSecondsPerSecond);
    struct tm tm_time;
    bzero(&tm_time, sizeof tm_time);

    gmtime_r(&seconds, &tm_time);

    snprintf(buffer, sizeof(buffer), "%4d%02d%02d %02d:%02d:%02d.%06d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, microseconds);
    return buffer;
}

