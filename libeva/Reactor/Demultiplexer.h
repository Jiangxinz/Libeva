#ifndef _LIBEVA_DEMULTIPLEXER_H_
#define _LIBEVA_DEMULTIPLEXER_H_

#include <sys/epoll.h>
#include <vector>
#include <map>

struct epoll_event;

namespace libeva {

class Reactor;
class Event;

class Demultiplexer {
public:
    Demultiplexer(Reactor*);
    ~Demultiplexer();

    std::vector<Event*> demultiplex( int timeout = 5*1000 );
    void registerEvent( Event * );
    void unregisterEvent( Event * );

private:
    static int createEpollFd();
    void handleNewEvent( Event* );
    void handleDelEvent( Event* );
    void handleExistEvent( Event* );
    void registerInner( int mod, Event *ev );
    const char* mod2String( int mod );
private:
    static const int kInitEventListSize = 16;
    Reactor* ownerReactor_;

    int epollfd_;
    std::vector<struct epoll_event> eventList_;
    std::map<int, Event*> eventMap_;
};

}

#endif /* _LIBEVA_DEMULTIPLEXER_H_ */
