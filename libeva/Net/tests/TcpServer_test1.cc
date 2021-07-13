#include <string.h>

#include "libeva/Net/TcpServer.h"
#include "libeva/Reactor/Reactor.h"
#include "libeva/Net/Socket.h"

using namespace libeva;

Reactor reactor;

int
main( int argc, char *argv[] ) {
    reactor.runAfter( 5.0, [&]() {
		    reactor.stop();
		   } );
    InetAddr listenAddr( "127.0.0.1", 8888 );
    
    TcpServer server( &reactor, listenAddr );
    server.setSubReactorNum( 3 );
    server.start();

    reactor.dispatch();
    return 0;
}
