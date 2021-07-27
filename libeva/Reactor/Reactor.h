#ifndef _LIBEVA_REACTOR_H_
#define _LIBEVA_REACTOR_H_

#include <memory>
#include <atomic>
#include <vector>
#include <functional>
#include <mutex>

#include "libeva/noncopyable.h"
#include "libeva/Thread/CurrentThread.h"
#include "libeva/TimeWheel/Timer.h"

namespace libeva {

class Event;
class Demultiplexer;
class TimeWheel;

class Reactor: public noncopyable {
public:
    typedef std::function<void()> Functor;
    Reactor();
    ~Reactor();

    void dispatch();

    // 注册事件
    void registerEvent(Event*);
    void unregisterEvent(Event*);
    void stop();
    void assertInDispatchingThread();
    bool isInDispatchingThread();
    void runInReactor( const Functor &cb );
    void queueInReactor( const Functor &cb );

    Timer* runAt(Timestamp time, TimerCallback cb);

    Timer* runAfter(double delay, TimerCallback cb);

    Timer* runEvery(double interval, TimerCallback cb);

    void cancel( Timer* timer );

    bool isInReactorThread() const
    { return threadId_ == CurrentThread::tid(); }
private:
    void printActiveEvents() const;
    int createEventfd() const;
    void wakeup();
    void doPendingFunctors();
    void readEventfd();
private:
    std::atomic_bool dispatching_;
    pid_t threadId_;
    int eventfd_;
    
    std::vector<Event*> activeEvents_;
    std::unique_ptr<Demultiplexer> poller_;
    
    std::unique_ptr<TimeWheel> timeWheel_;

    std::atomic_bool hasPendingFunctors_;
    std::atomic_bool callingPendingFunctors_;
    std::mutex mutex_;
    std::vector<Functor> pendingFunctors_;
    std::unique_ptr<Event> wakeupEvent_;
};

}   // libeva






#endif /* _LIBEVA_REACTOR_H_ */
