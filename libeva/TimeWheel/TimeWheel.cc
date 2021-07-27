#include <sys/timerfd.h>
#include "libeva/TimeWheel/TimeWheel.h"
#include "libeva/TimeWheel/Timer.h"
#include "libeva/Logger/Logging.h"
#include "libeva/Reactor/Reactor.h"


namespace libeva {

int TimeWheel::createTimerfd() {
        int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                 TFD_NONBLOCK | TFD_CLOEXEC);
        if (timerfd < 0) {
                LOG_SYSFATAL << "Failed in timerfd_create";
        }
        return timerfd;
}

// TODO: settimer

TimeWheel::TimeWheel( Reactor* reactor, unsigned num_bucket, bool use_timerfd ) :
        owner_( reactor ), slot_( num_bucket ), numSlot_( num_bucket ), 
        timerfd_ ( use_timerfd ? createTimerfd() : -1 ),
        timerEvent_ ( use_timerfd ? std::make_unique<Event>( reactor, timerfd_ ) : nullptr ), 
        start_( Timestamp::now() ), startIdx_( 0 ), 
        delTimers_( nullptr ), activeTimers_( nullptr ) {
                
                if ( use_timerfd ) {
                        timerEvent_->setReadCallback( std::bind( &TimeWheel::tock, this ) );
                        timerEvent_->enableReading();
                        tick();
                }
        }

void TimeWheel::restart() {
        start_ = Timestamp::now();
        startIdx_ = 0;
}

void TimeWheel::readTimerfd(int timerfd, Timestamp now) {
        uint64_t howmany;
        ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
        // LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
        if (n != sizeof howmany) {
                LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
        }
}

void TimeWheel::tock() {
        // LOG_TRACE << Timestamp::now().toString();
        Timestamp now = Timestamp::now();
        if ( now < start_ ) {
                LOG_ERROR << "Current Time " << now.toString() << " > start_ " << start_.toString();
        }
        if ( timerfd_ > 0 ) {
                readTimerfd( timerfd_, now );
        }
        now = Timestamp::now();
        int diff = static_cast<int>( timeDifference( now, start_ ) * 1000 );
        // LOG_DEBUG << " diff = " << diff;
        for ( int i = 1; i <= diff; ++i ) {
                startIdx_ = ( startIdx_ + 1 ) % numSlot_;
                // LOG_DEBUG << "start_ from " << start_.toString();
                start_ = addTime( start_, 0.001 );
                // LOG_DEBUG << " to " << start_.toString();
                if ( slot_[ startIdx_ ] ) {
                        // 处理链表上的事件
                        Timer* head = slot_[ startIdx_ ];
                        for ( ; head; ) {
                                // LOG_TRACE << " handle Timer " << head->interval();
                                // TODO: 加入超时链表
                                if ( startIdx_ != head->index() ) {
                                        LOG_FATAL << " startIdx = " << startIdx_ << " head->index = " << head->index();
                                }
                                head->run();
                                Timer *tmp = head;
                                head = head->next_;
                                tmp->next_ = nullptr;
                                if ( tmp->repeat() ) {
                                        Timestamp when = addTime( Timestamp::now(), tmp->interval() );
                                        addTimer( tmp, when );
                                } else {
                                        delete tmp;
                                }
                                
                        }
                        slot_[ startIdx_ ] = head;
                }

        }
        // start_ = Timestamp::now();
        if ( timerfd_ > 0 ) {
                tick();
        }
}

TimeWheel::~TimeWheel() {
        for ( auto slot : slot_ ) {
                for ( Timer *p = slot; p; ) {
                        Timer *tmp = p;
                        p = p->next_;
                        delete tmp;
                }
                
        }
}

void TimeWheel::tick() {
        LOG_DEBUG << "TimeWheel::tick()";
        owner_->assertInDispatchingThread();
        LOG_DEBUG << "TimeWheel::tick()";
        struct itimerspec newValue;
        struct itimerspec oldValue;
        memset(&newValue, 0, sizeof newValue);
        memset(&oldValue, 0, sizeof oldValue);
        
        struct timespec ts;
        ts.tv_sec = 0;
        // 1ms
        ts.tv_nsec = 1000*1000;
        newValue.it_value = ts;

        int ret = ::timerfd_settime(timerfd_, 0, &newValue, &oldValue);
        if (ret) { 
                LOG_SYSERR << "timerfd_settime()";
        }
}

Timer* TimeWheel::addTimer( TimerCallback cb, Timestamp when, double interval) {
        Timer* timer = new Timer( std::move( cb ), when, interval );
        if ( when < start_ ) {
                LOG_FATAL << " Timer's " << when.toString() << " < start_ " << start_.toString();
        }
        owner_->runInReactor(
                std::bind(&TimeWheel::addTimerInReactor, this, timer));
        return timer;
}


void TimeWheel::cancel(Timer* timer) {
        owner_->runInReactor(
                std::bind(&TimeWheel::cancelInReactor, this, timer));
}

void TimeWheel::cancelInReactor(Timer *timer) {
        owner_->assertInDispatchingThread();
        // FIXME: 
        // use weak_ptr instead of raw pointer
        if ( slot_[ timer->index() ] == timer ) {
                slot_[ timer->index() ] = timer->next_;   
        } else {
                timer->pre_->next_ = timer->next_;
                timer->next_->pre_ = timer->pre_;
        }
        delete timer;
}

void TimeWheel::addTimerInReactor(Timer* timer) {
        owner_->assertInDispatchingThread();
        // FIXME: timer->expiration() is not pricise
        addTimer( timer, timer->expiration() );
}

void TimeWheel::delTimer( Timer* timer ) {
        if ( timer->index() < 0 ) {
                LOG_FATAL << " Invalid index " << timer->index();
        }
        LOG_TRACE << "delete timer";
        // TODO: delTimer in loop
}

void printTimer( Timer *head ) {
        for ( Timer *p = head; p; p = p->getNext() ) {
                LOG_DEBUG << " Timer " << p->id();
        }
}

Timer* TimeWheel::addTimer( Timer* timer, Timestamp when ) {
        // 毫秒
        int diff = static_cast<int>( timeDifference( when, start_ ) * 1000 );
        if ( diff >= numSlot_ ) {
                LOG_FATAL << " diff " << diff << " > " << slot_.size();
        }
        int slotIndex = ( startIdx_ + diff ) % numSlot_;
        if ( slot_.at( slotIndex ) ) {
                if ( slot_[ slotIndex ]->pre_ != nullptr ) {
                        LOG_FATAL << " slot_[ " << slotIndex << " ] is not null\n";
                }
                timer->next_ = slot_[ slotIndex ];
        } 
        LOG_DEBUG << "add " << timer->id() << " successful";
        slot_[ slotIndex ] = timer;
        timer->set_index( slotIndex );
        return timer;
}

};