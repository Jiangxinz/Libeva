#ifndef _LIBEVA_TIMEWHEEL_TIMER_H_
#define _LIBEVA_TIMEWHEEL_TIMER_H_

#include <atomic>
#include "libeva/Callbacks.h"
#include "libeva/Timestamp.h"

namespace libeva {

class TimeWheel;

class Timer {
    friend class TimeWheel;
public:
    Timer( TimerCallback , Timestamp , double );
    ~Timer();

    Timestamp expiration() const  { return expiration_; }
    double interval() const { return interval_; }
    bool repeat() const { return repeat_; }
    int index() const { return index_; }
    int id() const { return id_; }

    void set_index( int index ) { index_ = index; }
    void restart(Timestamp now);
    void run() const;

    // 连接两个Timer
    static void Join( Timer* , Timer* );

    Timer *getNext() { return next_; }
    
private:
    Timer* pre_;
    Timer* next_;

    int index_;
    int id_;
    TimerCallback cb_;
    Timestamp expiration_;
    const double interval_;
    const bool repeat_;

    static std::atomic_int numCreated_;
};

}

#endif /* _LIBEVA_TIMEWHEEL_HTIMER_H_ */