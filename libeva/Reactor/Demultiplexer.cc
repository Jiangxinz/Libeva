#include <poll.h>
#include "libeva/utility.h"
#include "libeva/Reactor/Demultiplexer.h"
#include "libeva/Reactor/Reactor.h"
#include "libeva/Reactor/Event.h"
#include "libeva/Logger/Logging.h"


static_assert(EPOLLIN == POLLIN,        "epoll uses same flag values as poll");
static_assert(EPOLLPRI == POLLPRI,      "epoll uses same flag values as poll");
static_assert(EPOLLOUT == POLLOUT,      "epoll uses same flag values as poll");
static_assert(EPOLLRDHUP == POLLRDHUP,  "epoll uses same flag values as poll");
static_assert(EPOLLERR == POLLERR,      "epoll uses same flag values as poll");
static_assert(EPOLLHUP == POLLHUP,      "epoll uses same flag values as poll");

namespace libeva {

enum {
        kNew,
        kAdded,
        kDeled
};


const int Demultiplexer::kInitEventListSize;

Demultiplexer::Demultiplexer(Reactor* reactor) :
                ownerReactor_( reactor ),
                epollfd_( createEpollFd() ),
                eventList_( kInitEventListSize ),
                eventMap_() {

}

Demultiplexer::~Demultiplexer() {

}

int Demultiplexer::createEpollFd() {
    int epollfd = ::epoll_create1(EPOLL_CLOEXEC);
    if ( epollfd == -1 ) {
        LOG_FATAL << "epoll_create error " << errno;
    }
    return epollfd;
}

std::vector<Event*> Demultiplexer::demultiplex( int timeout ) {
    std::vector<Event*> ret;
    int numActive = epoll_wait(epollfd_, 
                                eventList_.data(), 
                                static_cast<int>(eventList_.size()), 
                                timeout );
    int savedErrno = errno;
    if ( numActive > 0 ) {
        // 处理活动事件
        // LOG_DEBUG << " Active Event's number " << numActive;
        for ( int i = 0; i < numActive; ++i ) {
            auto &stEv = eventList_.at(i);
            Event *activeEv = static_cast<Event*>( stEv.data.ptr );
            assert ( eventMap_.find( activeEv->fd() ) != eventMap_.end() );
            activeEv->set_revents( stEv.events );
            ret.push_back( activeEv );
        }
        // 如果空间满了，则2倍扩容
        if (implicit_cast<size_t>(numActive) == eventList_.size()) {
            eventList_.resize( eventList_.size()*2 );
        }
    } else if ( numActive == 0 ) {
        
        // LOG_TRACE << "Nothing hanppen";

    } else if ( numActive < 0 ) {

        if (savedErrno != EINTR) {
            errno = savedErrno;
            LOG_SYSERR << "EPollPoller::poll()";
        }
    }
    
    return ret;
}

void Demultiplexer::registerEvent( Event *ev ) {
    ownerReactor_->assertInDispatchingThread();
    int state = ev->state();
    int fd = ev->fd();
    LOG_TRACE << "fd = " << fd
            << " events = " << ev->events() << " state = " << state;
    switch ( state ) {
            case kNew:
                handleNewEvent( ev ); break;
            case kDeled:
                handleDelEvent( ev ); break;
            case kAdded:
                handleExistEvent( ev ); break;
            default:
                LOG_FATAL << "Unknown State " << state << " ev's fd " << fd;
                break;
    }
}

void Demultiplexer::unregisterEvent( Event *ev ) {
	ownerReactor_->assertInDispatchingThread();
	int fd = ev->fd();
	LOG_TRACE << "fd = " << fd;
	assert ( eventMap_.find( fd ) != eventMap_.end() );
	assert ( eventMap_[ fd ] == ev );
	assert ( ev->isNoneEvent() );
	int state = ev->state();
	assert ( state == kAdded || state == kDeleted );
	eventMap_.erase( fd );
	if ( state == kAdded ) {
		registerInner( EPOLL_CTL_DEL, ev );
	}
	ev->set_state( kNew );
}

void Demultiplexer::handleNewEvent( Event *ev ) {
    // int state = ev->state();
    int fd = ev->fd();
    assert( eventMap_.find(fd) == eventMap_.end() );  
    eventMap_[ fd ] = ev;
    ev->set_state( kAdded );
    registerInner( EPOLL_CTL_ADD, ev );
}

void Demultiplexer::handleDelEvent( Event *ev ) {
    
    assert( eventMap_.find( fd ) != eventMap_.end() );
    assert( eventMap_[ fd ] == ev );
    ev->set_state( kAdded );
    registerInner( EPOLL_CTL_ADD, ev );
}

void Demultiplexer::handleExistEvent( Event *ev ) {
    assert ( false );
    assert( eventMap_.find( fd ) != eventMap_.end() );
    assert( eventMap_[ fd ] == ev);
    assert( state == kAdded );
    if ( ev->isNoneEvent() ) {
        registerInner( EPOLL_CTL_DEL, ev );
        ev->set_state( kDeled );
    }
    else {
        registerInner( EPOLL_CTL_MOD, ev );
    }
}

void Demultiplexer::registerInner( int mod, Event *ev ) {
    struct epoll_event event;
    memset( &event, 0, sizeof event );
    event.events = ev->events();
    event.data.ptr = ev;
    int fd = ev->fd();
    LOG_TRACE << "epoll_ctl op = " << mod2String(mod)
         << " fd = " << fd << " event = { " << ev->eventsToString() << " }";
    if ( ::epoll_ctl( epollfd_, mod, fd, &event ) < 0 ) {
        if ( mod == EPOLL_CTL_DEL ) {
            LOG_SYSERR << "epoll_ctl op =" << mod2String(mod) << " fd =" << fd;
        }
        else {
            LOG_SYSFATAL << "epoll_ctl op =" << mod2String(mod) << " fd =" << fd;
        }
    }
}

const char* Demultiplexer::mod2String( int mod ) {
    switch ( mod ) {
        case EPOLL_CTL_ADD:
            return "ADD";
        case EPOLL_CTL_DEL:
            return "DEL";
        case EPOLL_CTL_MOD:
            return "MOD";
        default:
            LOG_FATAL << "Unknown Mod " << mod;
            return "Unknown mod";
    }

}

} // namespace libeva
