#ifndef _LIBEVA_TIMEWHEEL_H_
#define _LIBEVA_TIMEWHEEL_H_

#include <memory>
#include <vector>
#include "libeva/Callbacks.h"
#include "libeva/noncopyable.h"
#include "libeva/Reactor/Event.h"

namespace libeva {

class Timer;
class Reactor;

// 每一个slot对应 1ms
class TimeWheel : noncopyable {
public:
    TimeWheel( Reactor* reactor, unsigned num_bucket = 60 * 1000, bool use_timerfd = false );
    ~TimeWheel();

    Timer* addTimer( TimerCallback, Timestamp, double );
    void cancel( Timer* timer );
    void delTimer( Timer* );
    // 开始轮转
    void tick();
    void tock();
    void restart();
private:
    static int createTimerfd();
    void readTimerfd(int timerfd, Timestamp now);
    
    Timer* addTimer( Timer* timer, Timestamp when );
    void cancelInReactor( Timer *timer );
    void addTimerInReactor(Timer* timer);
private:
    typedef std::vector<Timer*> SlotVec;
    Reactor *owner_;
    SlotVec slot_;
    int numSlot_;
    int timerfd_;
    
    std::unique_ptr<Event> timerEvent_;
    Timestamp start_; // 目前的中最早的时间，初始为当前时间
	int startIdx_;  // 目前最早的时间所对应的slot的索引
    // TODO: 
    Timer *delTimers_;
    Timer *activeTimers_;
};

}

#endif /* _LIBEVA_TIMEWHEEL_H_ */