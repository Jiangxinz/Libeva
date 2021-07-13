#include <poll.h>
#include "libeva/Net/Acceptor.h"
#include "libeva/Net/Socket.h"
#include "libeva/Reactor/Event.h"
#include "libeva/Logger/Logging.h"

namespace libeva {

Acceptor::Acceptor( Reactor *reactor, InetAddr &listenAddr ) :
            owner_( reactor ),
            listenSocket_( Socket::createListenSocketOn( listenAddr, REUSE_ADDR /* REUSE_ADDR */ ) ),
            listenEvent_( std::make_unique<Event>( reactor, listenSocket_.fd() ) )
            {
                listenEvent_->setReadCallback( std::bind( &Acceptor::handleRead, this ) );
        }

Acceptor::~Acceptor() {

}

void Acceptor::start() {
    listenSocket_.listen();
    listenEvent_->enableReading();
}

void Acceptor::handleRead() {
    InetAddr peerAddr;
    Socket connSock = listenSocket_.accept1( reinterpret_cast<struct sockaddr *>( peerAddr.getSocketAddr() ) );
    if ( connSock.fd() < 0 ) {
        LOG_DEBUG << " acceptor connfd " << connSock.fd() << " failed";
    } else {
        // FIXME: poll connfd if writeable call conncb otherwise close( connfd );
        struct pollfd pfd;
        pfd.fd = connSock.fd();
        pfd.events = POLLOUT;
        poll( &pfd, 1, 0 );
        if ( pfd.revents & POLLERR ) {
            LOG_ERROR << "connection fd error";
        } else {
            if ( connCb_ ) {
                connCb_( std::move( connSock ), peerAddr );
            }
        }
    }
    
}

}