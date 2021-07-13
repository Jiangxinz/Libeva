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

void func2( Socket connSock, InetAddr &peerAddr ) {
    int fd = connSock.fd();
    printf("void func(): fd = %d\n", fd);
    ::write(fd, "hello Cesar\r\n", strlen("hello libeva\r\n"));
    // ::close( fd );
}

int
main( int argc, char *argv[] ) {
    Reactor reactor;
    InetAddr listenAddr1( "127.0.0.1", 8888 );
    Acceptor acceptor1( &reactor, listenAddr1 );
    acceptor1.setNewConnCallback( func );
    acceptor1.start();
    

    InetAddr listenAddr2( "127.0.0.1", 8889 );
    Acceptor acceptor2( &reactor, listenAddr2 );
    acceptor2.setNewConnCallback( func );
    acceptor2.start();

    reactor.dispatch();
    return 0;
}