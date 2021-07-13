
#include <poll.h>
#include <sys/eventfd.h>
#include "libeva/Reactor/Reactor.h"
#include "libeva/Reactor/Event.h"
#include "libeva/Logger/Logging.h"
#include "libeva/Reactor/Demultiplexer.h"
#include "libeva/TimeWheel/TimeWheel.h"

// #include "muduo/base/Logging.h"
namespace libeva {

namespace {

__thread Reactor* t_reactorInCt = nullptr;

}

Reactor::Reactor() 
        : // running_( false ), 
          dispatching_ ( false ),
          threadId_ ( CurrentThread::tid() ),
          eventfd_( createEventfd() ),
          poller_( std::make_unique<Demultiplexer>( this ) ),
          // 时间轮默认不使用timerfd定时
          timeWheel_( std::make_unique<TimeWheel>( this ) ),
          hasPendingFunctors_( false ),
          callingPendingFunctors_( false ),
          mutex_(),
          pendingFunctors_(),
          wakeupEvent_( std::make_unique<Event>( this, eventfd_ ) ) {
                // printf("this = %p\n", this);
                LOG_TRACE << "Reactor created " << this << " in thread " << threadId_;
                if ( t_reactorInCt ) {
                        LOG_FATAL << "Reactor already created in current thread " << threadId_;
                } else {
                        t_reactorInCt = this;
                }
                wakeupEvent_->setReadCallback( std::bind( &Reactor::readEventfd, this ) );
                wakeupEvent_->enableReading();
        }

Reactor::~Reactor() {
        assert ( !dispatching_ );
        t_reactorInCt = nullptr;
}

void Reactor::dispatch() {
        assert ( !dispatching_ );
        dispatching_ = true;
        assertInDispatchingThread();

        // 如果poller定时1ms，那么eventfd已经没有必要了？
        // 如果去掉eventfd，那么异步处理可能会引入至少1ms延时，而1ms可以做很多事情
        while ( dispatching_ ) {

                activeEvents_ = poller_->demultiplex( 1 );
                
                for ( auto ev : activeEvents_ ) {
                        ev->handle();
                }
                activeEvents_.clear();

                timeWheel_->tock();
                
                if ( hasPendingFunctors_ ) {
                        doPendingFunctors();
                        hasPendingFunctors_ = false;
                }

        }
        // dispatching_ = false;
}

void Reactor::assertInDispatchingThread() {
        if ( !isInDispatchingThread() ) {
                LOG_FATAL << "Reactor.ThreadId: " << threadId_ <<
                        " is in another Thread: " << CurrentThread::tid();
        }
}

bool Reactor::isInDispatchingThread() {
        return threadId_ != CurrentThread::tid() ? false : true;
}

Timer* Reactor::runAt(Timestamp time, TimerCallback cb) {
        return timeWheel_->addTimer( std::move( cb ), time, 0.0 );
}

Timer* Reactor::runAfter(double delay, TimerCallback cb) {
        Timestamp time( addTime( Timestamp::now(), delay ) );
        return runAt( time, std::move( cb ) );
}

Timer* Reactor::runEvery(double interval, TimerCallback cb) {
        Timestamp time( addTime( Timestamp::now(), interval ) );
        return timeWheel_->addTimer( std::move( cb ), time, interval );
}

void Reactor::cancel( Timer* timer ) {
        timeWheel_->cancel( timer );
}

void Reactor::runInReactor( const Functor &cb ) {
        if ( isInReactorThread() ) {
                cb();
        } else {
                queueInReactor( cb );
        }
}

void Reactor::queueInReactor( const Functor &cb ) {
        {
                std::lock_guard<std::mutex> lock( mutex_ );
                pendingFunctors_.push_back( cb );
        }
	hasPendingFunctors_ = true;
        if ( !isInReactorThread() || callingPendingFunctors_ ) {
                wakeup();
        }
}

int Reactor::createEventfd() const {
        int ret = ::eventfd( 0, EFD_NONBLOCK | EFD_CLOEXEC );
        if (ret < 0) {
                LOG_FATAL << "Failed in eventfd";
        }
        return ret;
}

void Reactor::readEventfd() {
        uint64_t one = 1;
        ssize_t n = read( eventfd_, &one, sizeof( one ) );
        if ( n != sizeof( one ) ) {
                LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
        }
        hasPendingFunctors_ = true;
}

void Reactor::registerEvent(Event* ev) {
        assert ( ev->ownerReactor() == this );
        assertInDispatchingThread();
        poller_->registerEvent( ev );
}

void Reactor::unregisterEvent(Event* ev) {
        assert ( ev->ownerReactor() == this );
        assertInDispatchingThread();
        poller_->unregisterEvent( ev );
}

void Reactor::stop() {
        dispatching_ = false;
        if ( !isInReactorThread() ) {
                wakeup();
        }
}

void Reactor::doPendingFunctors() {
        std::vector<Functor> functors;
	// TODO: use lock-free queue
        callingPendingFunctors_ = true;
        {
                std::lock_guard<std::mutex> lock( mutex_ );
                functors.swap( pendingFunctors_ );
        }
        for ( auto &cb : functors ) {
                cb();
        }
        callingPendingFunctors_ = false;
}

void Reactor::wakeup() {
        uint64_t one = 1;
        ssize_t n = ::write( eventfd_, &one, sizeof( one ) );
        if ( n != sizeof( one ) ) {
                LOG_ERROR << "Reactor::wakeup() writes " << n << " bytes instead of 8";
        }
}

void Reactor::printActiveEvents() const
{
  for (const Event* ev : activeEvents_ )
  {
    LOG_TRACE << "{" << ev->reventsToString() << "} ";
  }
}

}
