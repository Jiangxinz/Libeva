#include "libeva/TimeWheel/Timer.h"
#include "libeva/Logger/Logging.h"

namespace libeva {

std::atomic_int Timer::numCreated_ = 0;

Timer::Timer( TimerCallback cb, Timestamp when, double interval ) :
            pre_( nullptr ), next_( nullptr ), index_( -1 ), 
	    id_( numCreated_.fetch_add( 1, std::memory_order_relaxed ) ), 
	    cb_( cb ), expiration_ ( when ), 
            interval_( interval ), repeat_ ( interval_ > 0.0 ) {

            }

Timer::~Timer() {

}

void Timer::run() const { if ( cb_ ) cb_(); }

// 将lhs 加入 rhs所在链表中
void Timer::Join( Timer* lhs, Timer* rhs ) {
    if ( !lhs || !rhs ) {
        return;
    }
    LOG_DEBUG << "Join " << lhs->interval() << " and " << rhs->interval();
    lhs->next_ = rhs;
    if ( rhs->pre_ ) {
        lhs->pre_ = rhs->pre_;
        rhs->pre_->next_ = lhs;
    } 
    rhs->pre_ = lhs;
}

};
