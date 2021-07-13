#include <string.h>

#include "libeva/Net/Acceptor.h"
#include "libeva/Reactor/Reactor.h"
#include "libeva/Net/Socket.h"

using namespace libeva;

void func( Socket connSock, InetAddr &peerAddr ) {
    int fd = connSock.fd();
    printf("void func(): fd = %d\n", fd);
    ::write(fd, "hello libeva\r\n", strlen("hello libeva\r\n"));
    // ::close( fd );
}

int
main( int argc, char *argv[] ) {
    Reactor reactor;
    InetAddr listenAddr( "127.0.0.1", 8888 );
    Acceptor acceptor( &reactor, listenAddr );
    acceptor.setNewConnCallback( func );
    acceptor.start();
    
    reactor.dispatch();
    return 0;
}