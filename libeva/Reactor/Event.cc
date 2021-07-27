#include <poll.h>

#include "libeva/Logger/Logging.h"
#include "libeva/Reactor/Event.h"
#include "libeva/Reactor/Reactor.h"

namespace libeva {

const int Event::kNoneEvent = 0;
const int Event::kReadEvent = POLLIN | POLLPRI;
const int Event::kWriteEvent = POLLOUT;

Event::Event(Reactor* reactor, int fd):
            reactor_(reactor), 
            fd_(fd), 
            state_(0),
            events_(0), 
            revents_(0) { }

Event::~Event() {}

void Event::handle() {
    // TODO: handle with shared_ptr
    if ( (revents_ & POLLHUP ) && !(revents_ & POLLIN) ) {
	    if ( closeCallback_ ) closeCallback_();
    }
    if ( revents_ & POLLNVAL ) {
        LOG_WARN << "Event::handle() POLLNVAL";
    }
    if ( revents_ & (POLLERR | POLLNVAL) ) {
        if ( errorCallback_ ) errorCallback_();
    }
    if ( revents_ & (POLLIN | POLLPRI | POLLRDHUP ) ) {
        if ( readCallback_ ) readCallback_(Timestamp::now());
    }
    if ( revents_ & POLLOUT ) {
        if ( writeCallback_ ) writeCallback_();
    }
}

void Event::update() {
    reactor_->registerEvent(this);
}

void Event::remove() {
    reactor_->unregisterEvent( this );
}

std::string Event::eventsToString() const {
    return eventsToString( fd_, events_ );
}

std::string Event::eventsToString( int fd, int events ) const {
    std::string ret( std::to_string(fd) );
    if ( events & POLLIN )
        ret.append(" IN");
    if ( events & POLLPRI )
        ret.append(" PRI");
    if ( events & POLLOUT )
        ret.append(" OUT");
    if ( events & POLLHUP )
        ret.append(" HUP");
    if ( events & POLLRDHUP )
        ret.append(" RDHUP");
    if ( events & POLLERR )
        ret.append(" ERR");
    if ( events & POLLNVAL )
        ret.append(" NVAL");
    return ret;
}

std::string Event::reventsToString() const {
    return eventsToString( fd_, revents_ );
}

// TODO:
void Event::tie( const std::shared_ptr<void> &ptr ) {
	// tie_ = ptr;
	// tied_ = true;
}

};
