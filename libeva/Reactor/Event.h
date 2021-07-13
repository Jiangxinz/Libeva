#ifndef _LIBEVA_EVENT_H_
#define _LIBEVA_EVENT_H_

#include <functional>
#include "libeva/noncopyable.h"
#include "libeva/Timestamp.h"

namespace libeva {

class Reactor;

class Event : public noncopyable {
public:
    typedef std::function<void(Timestamp)> ReadCallback;
    typedef std::function<void(void)> EventCallback;

    Event(Reactor*, int);
    ~Event();

    void handle();

    void setReadCallback(const ReadCallback &cb)
    { readCallback_ = cb; }
    void setWriteCallback(const EventCallback &cb)
    { writeCallback_ = cb; }
    void setErrorCallback(const EventCallback &cb)
    { errorCallback_ = cb; }
    
    void enableReading() { events_ |= kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }

    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }

    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    int fd() const { return fd_; }
    int state() const { return state_; }
    int events() const { return events_; }
    void set_state( int state ) { state_ = state; }
    void set_revents( int revents ) { revents_ = revents; }

    void remove();

    Reactor* ownerReactor() { return reactor_; }

    std::string eventsToString() const;
    std::string reventsToString() const;
private:
    std::string eventsToString( int fd, int events ) const;
    void update();
    
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    Reactor* reactor_;
    int fd_;
    int state_;
    int events_;
    int revents_;

    ReadCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
};

}

#endif /* _LIBEVA_EVENT_H_ */
