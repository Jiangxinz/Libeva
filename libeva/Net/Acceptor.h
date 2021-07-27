#ifndef _LIBEVA_ACCEPTOR_H_
#define _LIBEVA_ACCEPTOR_H_

#include <functional>
#include "libeva/noncopyable.h"
#include "libeva/Net/Socket.h"

#include <memory>

namespace libeva {

class Event;
class Reactor;
class InetAddr;

class Acceptor : public noncopyable {
public:
    typedef std::function<void(Socket, InetAddr&)> NewConnCallback;
    Acceptor( Reactor *reactor, InetAddr &listenAddr );
    ~Acceptor();
    
    void setNewConnCallback( NewConnCallback cb ) 
    { connCb_ = cb; }

    void start();
    void handleRead();
private:
    Reactor *owner_;
    Socket listenSocket_;
    std::unique_ptr<Event> listenEvent_;
    NewConnCallback connCb_;
};

}

#endif /* _LIBEVA_ACCEPTOR_H_ */